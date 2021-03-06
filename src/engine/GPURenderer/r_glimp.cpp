////////////////////////////////////////////////////////////////////////////////////////
// Copyright(C) 1999 - 2005 id Software, Inc.
// Copyright(C) 2011 - 2018 Dusan Jocic <dusanjocic@msn.com>
//
// This file is part of OpenWolf.
//
// OpenWolf is free software; you can redistribute it
// and / or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
//
// OpenWolf is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenWolf; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110 - 1301  USA
//
// -------------------------------------------------------------------------------------
// File name:   sdl_glimp.cpp
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description:
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#include <OWLib/precompiled.h>

static bool SDL_VIDEODRIVER_externallySet = false;

/* Just hack it for now. */
#ifdef MACOS_X
#include <OpenGL/OpenGL.h>
typedef CGLContextObj QGLContext;

static QGLContext opengl_context;

static void GLimp_GetCurrentContext( void )
{
    opengl_context = CGLGetCurrentContext();
}

#ifdef SMP
static void GLimp_SetCurrentContext( bool enable )
{
    if( enable )
    {
        CGLSetCurrentContext( opengl_context );
    }
    else
    {
        CGLSetCurrentContext( NULL );
    }
}
#endif

#elif SDL_VIDEO_DRIVER_X11

#include <GL/glx.h>
typedef struct
{
    GLXContext      ctx;
    Display*        dpy;
    GLXDrawable     drawable;
} QGLContext_t;
typedef QGLContext_t QGLContext;

static QGLContext opengl_context;

static void GLimp_GetCurrentContext( void )
{
    opengl_context.ctx = glXGetCurrentContext();
    opengl_context.dpy = glXGetCurrentDisplay();
    opengl_context.drawable = glXGetCurrentDrawable();
}

#ifdef SMP
static void GLimp_SetCurrentContext( bool enable )
{
    if( enable )
    {
        glXMakeCurrent( opengl_context.dpy, opengl_context.drawable, opengl_context.ctx );
    }
    else
    {
        glXMakeCurrent( opengl_context.dpy, None, NULL );
    }
}

#endif

#elif _WIN32

typedef struct
{
    HDC             hDC;		// handle to device context
    HGLRC           hGLRC;		// handle to GL rendering context
} QGLContext_t;
typedef QGLContext_t QGLContext;

static QGLContext opengl_context;

static void GLimp_GetCurrentContext( void )
{
    SDL_SysWMinfo info;

    SDL_VERSION( &info.version );

    if( !SDL_GetWMInfo( &info ) )
    {
        CL_RefPrintf( PRINT_WARNING, "Failed to obtain HWND from SDL (InputRegistry)" );
        return;
    }

    opengl_context.hDC = GetDC( info.window );
    opengl_context.hGLRC = info.hglrc;
}

#ifdef SMP
static void GLimp_SetCurrentContext( bool enable )
{
    if( enable )
    {
        wglMakeCurrent( opengl_context.hDC, opengl_context.hGLRC );
    }
    else
    {
        wglMakeCurrent( opengl_context.hDC, NULL );
    }
}

#endif
#else
static void GLimp_GetCurrentContext( void )
{
}

#ifdef SMP
static void GLimp_SetCurrentContext( bool enable )
{
}

#endif
#endif

#ifdef SMP

/*
===========================================================

SMP acceleration

===========================================================
*/

/*
 * I have no idea if this will even work...most platforms don't offer
 * thread-safe OpenGL libraries, and it looks like the original Linux
 * code counted on each thread claiming the GL context with glXMakeCurrent(),
 * which you can't currently do in SDL. We'll just have to hope for the best.
 */

static SDL_mutex* smpMutex = NULL;
static SDL_cond* renderCommandsEvent = NULL;
static SDL_cond* renderCompletedEvent = NULL;
static void ( *renderThreadFunction )( void ) = NULL;
static SDL_Thread* renderThread = NULL;

/*
===============
GLimp_RenderThreadWrapper
===============
*/
static S32 GLimp_RenderThreadWrapper( void* arg )
{
    // These printfs cause race conditions which mess up the console output
    Com_Printf( "Render thread starting\n" );
    
    renderThreadFunction();
    
    GLimp_SetCurrentContext( false );
    
    Com_Printf( "Render thread terminating\n" );
    
    return 0;
}

/*
===============
GLimp_SpawnRenderThread
===============
*/
bool GLimp_SpawnRenderThread( void ( *function )( void ) )
{
    static bool warned = false;
    
    if( !warned )
    {
        Com_Printf( "WARNING: You enable r_smp at your own risk!\n" );
        warned = true;
    }
    
#if !defined( MACOS_X ) && !defined( WIN32 ) && !defined ( SDL_VIDEO_DRIVER_X11 )
    return false; /* better safe than sorry for now. */
#endif
    
    if( renderThread != NULL )  /* hopefully just a zombie at this point... */
    {
        Com_Printf( "Already a render thread? Trying to clean it up...\n" );
        GLimp_ShutdownRenderThread();
    }
    
    smpMutex = SDL_CreateMutex();
    
    if( smpMutex == NULL )
    {
        Com_Printf( "smpMutex creation failed: %s\n", SDL_GetError() );
        GLimp_ShutdownRenderThread();
        return false;
    }
    
    renderCommandsEvent = SDL_CreateCond();
    
    if( renderCommandsEvent == NULL )
    {
        Com_Printf( "renderCommandsEvent creation failed: %s\n", SDL_GetError() );
        GLimp_ShutdownRenderThread();
        return false;
    }
    
    renderCompletedEvent = SDL_CreateCond();
    
    if( renderCompletedEvent == NULL )
    {
        Com_Printf( "renderCompletedEvent creation failed: %s\n", SDL_GetError() );
        GLimp_ShutdownRenderThread();
        return false;
    }
    
    renderThreadFunction = function;
    renderThread = SDL_CreateThread( GLimp_RenderThreadWrapper, NULL );
    
    if( renderThread == NULL )
    {
        CL_RefPrintf( PRINT_ALL, "SDL_CreateThread() returned %s", SDL_GetError() );
        GLimp_ShutdownRenderThread();
        return false;
    }
    else
    {
        // tma 01/09/07: don't think this is necessary anyway?
        //
        // !!! FIXME: No detach API available in SDL!
        //ret = pthread_detach( renderThread );
        //if ( ret ) {
        //CL_RefPrintf( PRINT_ALL, "pthread_detach returned %d: %s", ret, strerror( ret ) );
        //}
    }
    
    return true;
}

/*
===============
GLimp_ShutdownRenderThread
===============
*/
void GLimp_ShutdownRenderThread( void )
{
    if( renderThread != NULL )
    {
        SDL_WaitThread( renderThread, NULL );
        renderThread = NULL;
    }
    
    if( smpMutex != NULL )
    {
        SDL_DestroyMutex( smpMutex );
        smpMutex = NULL;
    }
    
    if( renderCommandsEvent != NULL )
    {
        SDL_DestroyCond( renderCommandsEvent );
        renderCommandsEvent = NULL;
    }
    
    if( renderCompletedEvent != NULL )
    {
        SDL_DestroyCond( renderCompletedEvent );
        renderCompletedEvent = NULL;
    }
    
    renderThreadFunction = NULL;
}

static volatile void*     smpData = NULL;
static volatile bool smpDataReady;

/*
===============
GLimp_RendererSleep
===============
*/
void*           GLimp_RendererSleep( void )
{
    void* data = NULL;
    
    GLimp_SetCurrentContext( false );
    
    SDL_LockMutex( smpMutex );
    {
        smpData = NULL;
        smpDataReady = false;
        
        // after this, the front end can exit GLimp_FrontEndSleep
        SDL_CondSignal( renderCompletedEvent );
        
        while( !smpDataReady )
        {
            SDL_CondWait( renderCommandsEvent, smpMutex );
        }
        
        data = ( void* ) smpData;
    }
    SDL_UnlockMutex( smpMutex );
    
    GLimp_SetCurrentContext( true );
    
    return data;
}

/*
===============
GLimp_FrontEndSleep
===============
*/
void GLimp_FrontEndSleep( void )
{
    SDL_LockMutex( smpMutex );
    {
        while( smpData )
        {
            SDL_CondWait( renderCompletedEvent, smpMutex );
        }
    }
    SDL_UnlockMutex( smpMutex );
    
    GLimp_SetCurrentContext( true );
}

/*
===============
GLimp_WakeRenderer
===============
*/
void GLimp_WakeRenderer( void* data )
{
    GLimp_SetCurrentContext( false );
    
    SDL_LockMutex( smpMutex );
    {
        assert( smpData == NULL );
        smpData = data;
        smpDataReady = true;
        
        // after this, the renderer can continue through GLimp_RendererSleep
        SDL_CondSignal( renderCommandsEvent );
    }
    SDL_UnlockMutex( smpMutex );
}

#else

// No SMP - stubs
void GLimp_RenderThreadWrapper( void* arg )
{
}

bool GLimp_SpawnRenderThread( void ( *function )( void ) )
{
    CL_RefPrintf( PRINT_WARNING, "ERROR: SMP support was disabled at compile time\n" );
    return false;
}

void GLimp_ShutdownRenderThread( void )
{
}

void*           GLimp_RendererSleep( void )
{
    return NULL;
}

void GLimp_FrontEndSleep( void )
{
}

void GLimp_WakeRenderer( void* data )
{
}

#endif

typedef enum
{
    RSERR_OK,
    
    RSERR_INVALID_FULLSCREEN,
    RSERR_INVALID_MODE,
    RSERR_OLD_GL,
    
    RSERR_UNKNOWN
} rserr_t;

static SDL_Surface*         screen = NULL;
static const SDL_VideoInfo* videoInfo = NULL;

cvar_t*                     r_allowResize; // make window resizable
cvar_t*                     r_centerWindow;
cvar_t*                     r_sdlDriver;
cvar_t*                     r_minimize;

/*
===============
GLimp_Shutdown
===============
*/
void GLimp_Shutdown( void )
{
    CL_RefPrintf( PRINT_ALL, "Shutting down OpenGL subsystem\n" );
    
    IN_Shutdown();
    
    SDL_QuitSubSystem( SDL_INIT_VIDEO );
    screen = NULL;
    
#if defined( SMP )
    
    if( renderThread != NULL )
    {
        Com_Printf( "Destroying renderer thread...\n" );
        GLimp_ShutdownRenderThread();
    }
    
#endif
    
    ::memset( &glConfig, 0, sizeof( glConfig ) );
    ::memset( &glState, 0, sizeof( glState ) );
}

/*
===============
GLimp_CompareModes
===============
*/
static S32 GLimp_CompareModes( const void* a, const void* b )
{
    const F32     ASPECT_EPSILON = 0.001f;
    SDL_Rect*       modeA = *( SDL_Rect** ) a;
    SDL_Rect*       modeB = *( SDL_Rect** ) b;
    F32           aspectA = ( F32 )modeA->w / ( F32 )modeA->h;
    F32           aspectB = ( F32 )modeB->w / ( F32 )modeB->h;
    S32             areaA = modeA->w * modeA->h;
    S32             areaB = modeB->w * modeB->h;
    F32           aspectDiffA = fabs( aspectA - displayAspect );
    F32           aspectDiffB = fabs( aspectB - displayAspect );
    F32           aspectDiffsDiff = aspectDiffA - aspectDiffB;
    
    if( aspectDiffsDiff > ASPECT_EPSILON )
    {
        return 1;
    }
    else if( aspectDiffsDiff < -ASPECT_EPSILON )
    {
        return -1;
    }
    else
    {
        return areaA - areaB;
    }
}

/*
===============
GLimp_DetectAvailableModes
===============
*/
static void GLimp_DetectAvailableModes( void )
{
    UTF8            buf[MAX_STRING_CHARS] = { 0 };
    SDL_Rect**      modes;
    S32             numModes;
    S32             i;
    
    modes = SDL_ListModes( videoInfo->vfmt, SDL_OPENGL | SDL_FULLSCREEN );
    
    if( !modes )
    {
        CL_RefPrintf( PRINT_WARNING, "Can't get list of available modes\n" );
        return;
    }
    
    if( modes == ( SDL_Rect** ) - 1 )
    {
        CL_RefPrintf( PRINT_ALL, "Display supports any resolution\n" );
        return; // can set any resolution
    }
    
    for( numModes = 0; modes[ numModes ]; numModes++ )
    {
        ;
    }
    
    if( numModes > 1 )
    {
        qsort( modes, numModes, sizeof( SDL_Rect* ), GLimp_CompareModes );
    }
    
    for( i = 0; i < numModes; i++ )
    {
        StringEntry newModeString = va( "%ux%u ", modes[ i ]->w, modes[ i ]->h );
        
        if( strlen( newModeString ) < ( S32 ) sizeof( buf ) - strlen( buf ) )
        {
            Q_strcat( buf, sizeof( buf ), newModeString );
        }
        else
        {
            CL_RefPrintf( PRINT_WARNING, "Skipping mode %ux%x, buffer too small\n", modes[ i ]->w, modes[ i ]->h );
        }
    }
    
    if( *buf )
    {
        buf[ strlen( buf ) - 1 ] = 0;
        CL_RefPrintf( PRINT_ALL, "Available modes: '%s'\n", buf );
        Cvar_Set( "r_availableModes", buf );
    }
}

static void GLimp_InitOpenGL3xContext()
{
#if defined( _WIN32 ) || defined( __LINUX__ )
    S32        retVal;
    StringEntry success[] = { "failed", "success" };
#endif
    S32        GLmajor, GLminor;
    
    GLimp_GetCurrentContext();
    sscanf( ( StringEntry ) glGetString( GL_VERSION ), "%d.%d", &GLmajor, &GLminor );
    
    // GL_VERSION returns the highest OpenGL version supported by the driver
    // which is also compatible with the version we requested (i.e. OpenGL 1.1).
    // Requesting a version below that will just give us the same GL version
    // again, so just keep the context, but pretend to the engine that we
    // have the lower version.
    if( r_glMajorVersion->integer && r_glMinorVersion->integer && 100 * r_glMajorVersion->integer + r_glMinorVersion->integer <= 100 * GLmajor + GLminor )
    {
        GLmajor = r_glMajorVersion->integer;
        GLminor = r_glMinorVersion->integer;
    }
    
    // Check if we have to create a core profile.
    // Core profiles are not necessarily compatible, so we have
    // to request the desired version.
#if defined( _WIN32 )
    
    if( WGLEW_ARB_create_context_profile && ( r_glCoreProfile->integer || r_glDebugProfile->integer ) )
    {
        S32 attribs[ 256 ]; // should be really enough
        S32 numAttribs;
        
        memset( attribs, 0, sizeof( attribs ) );
        numAttribs = 0;
        
        if( r_glMajorVersion->integer > 0 )
        {
            attribs[ numAttribs++ ] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            attribs[ numAttribs++ ] = r_glMajorVersion->integer;
            
            attribs[ numAttribs++ ] = WGL_CONTEXT_MINOR_VERSION_ARB;
            attribs[ numAttribs++ ] = r_glMinorVersion->integer;
        }
        
        attribs[ numAttribs++ ] = WGL_CONTEXT_FLAGS_ARB;
        
        if( r_glDebugProfile->integer )
        {
            attribs[ numAttribs++ ] = WGL_CONTEXT_DEBUG_BIT_ARB;
        }
        else
        {
            attribs[ numAttribs++ ] = 0;
        }
        
        attribs[ numAttribs++ ] = WGL_CONTEXT_PROFILE_MASK_ARB;
        
        if( r_glCoreProfile->integer )
        {
            attribs[ numAttribs++ ] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
        }
        else
        {
            attribs[ numAttribs++ ] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        }
        
        // set current context to NULL
        retVal = wglMakeCurrent( opengl_context.hDC, NULL ) != 0;
        CL_RefPrintf( PRINT_ALL, "...wglMakeCurrent( %p, %p ): %s\n", opengl_context.hDC, NULL, success[ retVal ] );
        
        // delete HGLRC
        if( opengl_context.hGLRC )
        {
            retVal = wglDeleteContext( opengl_context.hGLRC ) != 0;
            CL_RefPrintf( PRINT_ALL, "...deleting initial GL context: %s\n", success[ retVal ] );
            opengl_context.hGLRC = NULL;
        }
        
        CL_RefPrintf( PRINT_ALL, "...initializing new OpenGL context" );
        
        opengl_context.hGLRC = wglCreateContextAttribsARB( opengl_context.hDC, 0, attribs );
        
        if( wglMakeCurrent( opengl_context.hDC, opengl_context.hGLRC ) )
        {
            CL_RefPrintf( PRINT_ALL, " done\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_WARNING, "Could not initialize requested OpenGL profile\n" );
        }
    }
#elif defined( __LINUX__ )
    
    if( GLXEW_ARB_create_context_profile && ( r_glCoreProfile->integer || r_glDebugProfile->integer ) )
    {
        S32         numAttribs;
        S32         attribs[ 256 ];
        GLXFBConfig* FBConfig;
    
        // get FBConfig XID
        ::memset( attribs, 0, sizeof( attribs ) );
        numAttribs = 0;
    
        attribs[ numAttribs++ ] = GLX_FBCONFIG_ID;
        glXQueryContext( opengl_context.dpy, opengl_context.ctx, GLX_FBCONFIG_ID, &attribs[ numAttribs++ ] );
        FBConfig = glXChooseFBConfig( opengl_context.dpy, 0, attribs, &numAttribs );
    
        if( numAttribs == 0 )
        {
            CL_RefPrintf( PRINT_WARNING, "Could not get FBConfig for XID %d\n", attribs[ 1 ] );
        }
    
        memset( attribs, 0, sizeof( attribs ) );
        numAttribs = 0;
    
        if( r_glMajorVersion->integer > 0 )
        {
            attribs[ numAttribs++ ] = GLX_CONTEXT_MAJOR_VERSION_ARB;
            attribs[ numAttribs++ ] = r_glMajorVersion->integer;
    
            attribs[ numAttribs++ ] = GLX_CONTEXT_MINOR_VERSION_ARB;
            attribs[ numAttribs++ ] = r_glMinorVersion->integer;
        }
    
        attribs[ numAttribs++ ] = GLX_CONTEXT_FLAGS_ARB;
    
        if( r_glDebugProfile->integer )
        {
            attribs[ numAttribs++ ] = GLX_CONTEXT_DEBUG_BIT_ARB;
        }
        else
        {
            attribs[ numAttribs++ ] = 0;
        }
    
        attribs[ numAttribs++ ] = GLX_CONTEXT_PROFILE_MASK_ARB;
    
        if( r_glCoreProfile->integer )
        {
            attribs[ numAttribs++ ] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
        }
        else
        {
            attribs[ numAttribs++ ] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
        }
    
        // set current context to NULL
        retVal = glXMakeCurrent( opengl_context.dpy, None, NULL ) != 0;
        CL_RefPrintf( PRINT_ALL, "...glXMakeCurrent( %p, %p ): %s\n", opengl_context.dpy, NULL, success[ retVal ] );
    
        // delete dpy
        if( opengl_context.ctx )
        {
            glXDestroyContext( opengl_context.dpy, opengl_context.ctx );
            retVal = ( glGetError() == 0 );
            CL_RefPrintf( PRINT_ALL, "...deleting initial GL context: %s\n", success[ retVal ] );
            opengl_context.ctx = NULL;
        }
    
        CL_RefPrintf( PRINT_ALL, "...initializing new OpenGL context " );
    
        opengl_context.ctx = glXCreateContextAttribsARB( opengl_context.dpy, FBConfig[ 0 ], NULL, GL_TRUE, attribs );
    
        if( glXMakeCurrent( opengl_context.dpy, opengl_context.drawable, opengl_context.ctx ) )
        {
            CL_RefPrintf( PRINT_ALL, " done\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_WARNING, "Could not initialize requested OpenGL profile\n" );
        }
    }
    
#endif
    
    if( GLmajor < 2 || ( GLmajor == 3 && GLminor < 2 ) )
    {
        // shaders are supported, but not all GL3.x features
        CL_RefPrintf( PRINT_ALL, "Using enhanced (GL3) Renderer in GL 2.x mode...\n" );
        return;
    }
    
    CL_RefPrintf( PRINT_ALL, "Using enhanced (GL3) Renderer in GL 3.x mode...\n" );
    glConfig.driverType = GLDRV_OPENGL3;
    
    return;
}

#define MSG_ERR_OLD_VIDEO_DRIVER                                                       \
"\nOpenWolf with OpenGL 3.x renderer can not run on this "                             \
"machine since it is missing one or more required OpenGL "                             \
"extensions. Please update your video card drivers and try again.\n"

S32 s_windowWidth = 0;
S32 s_windowHeight = 0;

/*
===============
GLimp_SetMode
===============
*/
static S32 GLimp_SetMode( S32 mode, S32 fullscreen, S32 noborder )
{
    StringEntry     glstring;
    S32             sdlcolorbits;
    S32             colorbits, depthbits, stencilbits;
    S32             tcolorbits, tdepthbits, tstencilbits;
    S32             i = 0;
    SDL_Surface*    vidscreen = NULL;
    Uint32          flags = SDL_OPENGL;
    U32  			glewResult;
    
    CL_RefPrintf( PRINT_ALL, "Initializing OpenGL display\n" );
    
    if( r_allowResize->integer )
    {
        flags |= SDL_RESIZABLE;
    }
    
    if( videoInfo == NULL )
    {
        static SDL_VideoInfo   sVideoInfo;
        static SDL_PixelFormat sPixelFormat;
        
        videoInfo = SDL_GetVideoInfo();
        
        // Take a copy of the videoInfo
        ::memcpy( &sPixelFormat, videoInfo->vfmt, sizeof( SDL_PixelFormat ) );
        sPixelFormat.palette = NULL; // Should already be the case
        ::memcpy( &sVideoInfo, videoInfo, sizeof( SDL_VideoInfo ) );
        sVideoInfo.vfmt = &sPixelFormat;
        videoInfo = &sVideoInfo;
        
        if( videoInfo->current_h > 0 )
        {
            // Guess the display aspect ratio through the desktop resolution
            // by assuming (relatively safely) that it is set at or close to
            // the display's native aspect ratio
            displayAspect = ( F32 ) videoInfo->current_w / ( F32 ) videoInfo->current_h;
            
            CL_RefPrintf( PRINT_ALL, "Estimated display aspect: %.3f\n", displayAspect );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "Cannot estimate display aspect, assuming 1.333\n" );
        }
    }
    if( videoInfo->current_h > 0 )
    {
        glConfig.vidWidth = videoInfo->current_w;
        glConfig.vidHeight = videoInfo->current_h;
    }
    else
    {
        glConfig.vidWidth = 480;
        glConfig.vidHeight = 640;
        CL_RefPrintf( PRINT_ALL, "Cannot determine display resolution, assuming 640x480\n" );
    }
    
    CL_RefPrintf( PRINT_ALL, "...setting mode %d:", mode );
    
    if( !R_GetModeInfo( &glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect, mode ) )
    {
        CL_RefPrintf( PRINT_ALL, " invalid mode\n" );
        return RSERR_INVALID_MODE;
    }
    
    CL_RefPrintf( PRINT_ALL, " %d %d\n", glConfig.vidWidth, glConfig.vidHeight );
    
    if( fullscreen )
    {
        flags |= SDL_FULLSCREEN;
        glConfig.isFullscreen = true;
    }
    else
    {
        if( noborder )
        {
            flags |= SDL_NOFRAME;
        }
        
        glConfig.isFullscreen = false;
    }
    
    colorbits = r_colorbits->value;
    
    if( ( !colorbits ) || ( colorbits >= 32 ) )
    {
        colorbits = 24;
    }
    
    if( !r_depthbits->value )
    {
        depthbits = 24;
    }
    else
    {
        depthbits = r_depthbits->value;
    }
    
    stencilbits = r_stencilbits->value;
    
    for( i = 0; i < 16; i++ )
    {
        // 0 - default
        // 1 - minus colorbits
        // 2 - minus depthbits
        // 3 - minus stencil
        if( ( i % 4 ) == 0 && i )
        {
            // one pass, reduce
            switch( i / 4 )
            {
                case 2:
                    if( colorbits == 24 )
                    {
                        colorbits = 16;
                    }
                    
                    break;
                    
                case 1:
                    if( depthbits == 24 )
                    {
                        depthbits = 16;
                    }
                    else if( depthbits == 16 )
                    {
                        depthbits = 8;
                    }
                    
                case 3:
                    if( stencilbits == 24 )
                    {
                        stencilbits = 16;
                    }
                    else if( stencilbits == 16 )
                    {
                        stencilbits = 8;
                    }
            }
        }
        
        tcolorbits = colorbits;
        tdepthbits = depthbits;
        tstencilbits = stencilbits;
        
        if( ( i % 4 ) == 3 )
        {
            // reduce colorbits
            if( tcolorbits == 24 )
            {
                tcolorbits = 16;
            }
        }
        
        if( ( i % 4 ) == 2 )
        {
            // reduce depthbits
            if( tdepthbits == 24 )
            {
                tdepthbits = 16;
            }
            else if( tdepthbits == 16 )
            {
                tdepthbits = 8;
            }
        }
        
        if( ( i % 4 ) == 1 )
        {
            // reduce stencilbits
            if( tstencilbits == 24 )
            {
                tstencilbits = 16;
            }
            else if( tstencilbits == 16 )
            {
                tstencilbits = 8;
            }
            else
            {
                tstencilbits = 0;
            }
        }
        
        sdlcolorbits = 4;
        
        if( tcolorbits == 24 )
        {
            sdlcolorbits = 8;
        }
        
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, sdlcolorbits );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, sdlcolorbits );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, sdlcolorbits );
        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, tdepthbits );
        SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, tstencilbits );
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        
        if( SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, r_swapInterval->integer ) < 0 )
        {
            CL_RefPrintf( PRINT_ALL, "r_swapInterval requires libSDL >= 1.2.10\n" );
        }
        
#ifdef USE_ICON
        {
            SDL_Surface* icon = SDL_CreateRGBSurfaceFrom( ( void* ) CLIENT_WINDOW_ICON.pixel_data,
                                CLIENT_WINDOW_ICON.width,
                                CLIENT_WINDOW_ICON.height,
                                CLIENT_WINDOW_ICON.bytes_per_pixel * 8,
                                CLIENT_WINDOW_ICON.bytes_per_pixel * CLIENT_WINDOW_ICON.width,
#ifdef Q3_LITTLE_ENDIAN
                                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
                                0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
                                                        );
                                                        
            SDL_WM_SetIcon( icon, NULL );
            SDL_FreeSurface( icon );
        }
#endif
        
        SDL_WM_SetCaption( CLIENT_WINDOW_TITLE, CLIENT_WINDOW_MIN_TITLE );
        SDL_ShowCursor( 0 );
        
        if( !( vidscreen = SDL_SetVideoMode( glConfig.vidWidth, glConfig.vidHeight, colorbits, flags ) ) )
        {
            CL_RefPrintf( PRINT_DEVELOPER, "SDL_SetVideoMode failed: %s\n", SDL_GetError() );
            continue;
        }
        
        CL_RefPrintf( PRINT_ALL, "Using %d/%d/%d Color bits, %d depth, %d stencil display.\n",
                      sdlcolorbits, sdlcolorbits, sdlcolorbits, tdepthbits, tstencilbits );
                      
        glConfig.colorBits = tcolorbits;
        glConfig.depthBits = tdepthbits;
        glConfig.stencilBits = tstencilbits;
        break;
    }
    
    glewExperimental = true;
    glewResult = glewInit();
    
    if( GLEW_OK != glewResult )
    {
        // glewInit failed, something is seriously wrong
        Com_Error( ERR_FATAL, "GLW_StartOpenGL() - could not load OpenGL subsystem: %s", glewGetErrorString( glewResult ) );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "Using GLEW %s\n", glewGetString( GLEW_VERSION ) );
        CL_RefPrintf( PRINT_ALL, "Using OpenGL %s, GLSL %s\n", glGetString( GL_VERSION ), glGetString( GL_SHADING_LANGUAGE_VERSION ) );
    }
    
    GLimp_InitOpenGL3xContext();
    GLimp_DetectAvailableModes();
    
    if( !vidscreen )
    {
        CL_RefPrintf( PRINT_ALL, "Couldn't get a visual\n" );
        return RSERR_INVALID_MODE;
    }
    
    screen = vidscreen;
    
    glstring = ( UTF8* ) glGetString( GL_RENDERER );
    CL_RefPrintf( PRINT_ALL, "GL_RENDERER: %s\n", glstring );
    
    return RSERR_OK;
}

/*
===============
GLimp_StartDriverAndSetMode
===============
*/
static bool GLimp_StartDriverAndSetMode( S32 mode, S32 fullscreen, S32 noborder )
{
    rserr_t err;
    
    if( !SDL_WasInit( SDL_INIT_VIDEO ) )
    {
        UTF8 driverName[ 64 ];
        
        CL_RefPrintf( PRINT_ALL, "SDL_Init( SDL_INIT_VIDEO )... " );
        
        if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) == -1 )
        {
            CL_RefPrintf( PRINT_ALL, "SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) FAILED (%s)\n", SDL_GetError() );
            return false;
        }
        
        SDL_VideoDriverName( driverName, sizeof( driverName ) - 1 );
        CL_RefPrintf( PRINT_ALL, "SDL using driver \"%s\"\n", driverName );
        Cvar_Set( "r_sdlDriver", driverName );
    }
    
    if( fullscreen && Cvar_VariableIntegerValue( "in_nograb" ) )
    {
        CL_RefPrintf( PRINT_ALL, "Fullscreen not allowed with in_nograb 1\n" );
        Cvar_Set( "r_fullscreen", "0" );
        r_fullscreen->modified = false;
        fullscreen = false;
    }
    
    err = ( rserr_t )GLimp_SetMode( mode, fullscreen, noborder );
    
    switch( err )
    {
        case RSERR_INVALID_FULLSCREEN:
            CL_RefPrintf( PRINT_ALL, "...WARNING: fullscreen unavailable in this mode\n" );
            return false;
            
        case RSERR_INVALID_MODE:
            CL_RefPrintf( PRINT_ALL, "...WARNING: could not set the given mode (%d)\n", mode );
            return false;
            
        case RSERR_OLD_GL:
            CL_RefPrintf( PRINT_ALL, "...WARNING: OpenGL too old\n" );
            return false;
            
        default:
            break;
    }
    
    return true;
}

/*
===============
GLimp_OGL3InitExtensions
===============
*/
static void GLimp_OGL3InitExtensions( void )
{
    bool good;
    UTF8 missingExts[4096];
    
    CL_RefPrintf( PRINT_ALL, "Initializing OpenGL extensions\n" );
    
    // GL_EXT_texture_env_add
    glConfig.textureEnvAddAvailable = false;
    if( glewGetExtension( "GL_EXT_texture_env_add" ) )
    {
        if( r_ext_texture_env_add->integer )
        {
            glConfig.textureEnvAddAvailable = true;
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_EXT_texture_env_add\n" );
        }
        else
        {
            glConfig.textureEnvAddAvailable = false;
            CL_RefPrintf( PRINT_ALL, "...ignoring GL_EXT_texture_env_add\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_EXT_texture_env_add not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ARB_multitexture
    if( glConfig.driverType != GLDRV_OPENGL3 )
    {
        if( glewGetExtension( "GL_ARB_multitexture" ) )
        {
            glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &glConfig.numTextureUnits );
            
            if( glConfig.numTextureUnits > 1 )
            {
                good = true;
                
                CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_multitexture\n" );
            }
            else
            {
                good = false;
                
                Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_multitexture\n" );
                Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_multitexture, < 2 texture units" );
            }
        }
        else
        {
            good = false;
            
            Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_multitexture\n" );
            Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_multitexture" );
        }
    }
    GL_CheckErrors();
    
    // GL_ARB_depth_texture
    if( glewGetExtension( "GL_ARB_depth_texture" ) )
    {
        good = true;
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_depth_texture\n" );
    }
    else
    {
        good = false;
        
        Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_depth_texture\n" );
        Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_depth_texture" );
    }
    GL_CheckErrors();
    
    // GL_ARB_texture_cube_map
    if( glewGetExtension( "GL_ARB_texture_cube_map" ) )
    {
        good = true;
        
        //glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &glConfig2.maxCubeMapTextureSize );
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_texture_cube_map\n" );
    }
    else
    {
        good = false;
        
        Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_texture_cube_map\n" );
        Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_texture_cube_map" );
    }
    GL_CheckErrors();
    
    // GL_ARB_vertex_program
    if( glewGetExtension( "GL_ARB_vertex_program" ) )
    {
        good = true;
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_vertex_program\n" );
    }
    else
    {
        good = false;
        
        Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_vertex_program\n" );
        Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_vertex_program" );
    }
    GL_CheckErrors();
    
    // GL_ARB_vertex_array_object
    glRefConfig.vertexArrayObject = false;
    if( glewGetExtension( "GL_ARB_vertex_array_object" ) )
    {
        if( r_arb_vertex_array_object->integer )
        {
            glRefConfig.vertexArrayObject = true;
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_vertex_array_object\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "...ignoring GL_ARB_vertex_array_object\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_vertex_array_object not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ARB_shader_objects
    if( glewGetExtension( "GL_ARB_shader_objects" ) )
    {
        good = true;
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_shader_objects\n" );
    }
    else
    {
        good = false;
        
        Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_shader_objects\n" );
        Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_shader_objects" );
    }
    GL_CheckErrors();
    
    // GL_ARB_vertex_shader
    if( glewGetExtension( "GL_ARB_vertex_shader" ) )
    {
        S32	reservedComponents;
        good = true;
        
        GL_CheckErrors();
        
        reservedComponents = 16 * 10; // approximation how many uniforms we have besides the bone matrices
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_vertex_shader\n" );
    }
    else
    {
        good = false;
        
        Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_vertex_shader\n" );
        Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_vertex_shader" );
    }
    GL_CheckErrors();
    
    // GL_ARB_fragment_shader
    if( glewGetExtension( "GL_ARB_fragment_shader" ) )
    {
        good = true;
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_fragment_shader\n" );
    }
    else
    {
        good = false;
        
        Q_strcat( missingExts, sizeof( missingExts ), "GL_ARB_fragment_shader\n" );
        Com_Error( ERR_FATAL, MSG_ERR_OLD_VIDEO_DRIVER "\nYour GL driver is missing support for: GL_ARB_fragment_shader" );
    }
    GL_CheckErrors();
    
    // GL_ARB_texture_float
    glRefConfig.textureFloat = false;
    if( glewGetExtension( "GL_ARB_texture_float" ) )
    {
        if( r_ext_texture_float->integer )
        {
            glRefConfig.textureFloat = !!r_ext_texture_float->integer;
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_texture_float\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "...ignoring GL_ARB_texture_float\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_texture_float not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ATI_meminfo
    if( glewGetExtension( "GL_ATI_meminfo" ) )
    {
        if( glRefConfig.memInfo == MI_NONE )
        {
            glRefConfig.memInfo = MI_ATI;
            
            GLint vbo_mem_kb = 0, texture_mem_kb = 0, renderbuffer_mem_kb = 0;
            glGetIntegerv( GL_VBO_FREE_MEMORY_ATI, &vbo_mem_kb );
            glGetIntegerv( GL_TEXTURE_FREE_MEMORY_ATI, &texture_mem_kb );
            glGetIntegerv( GL_RENDERBUFFER_FREE_MEMORY_ATI, &renderbuffer_mem_kb );
            CL_RefPrintf( PRINT_ALL, "GL_ATI_meminfo:\n\tVBO free memory: %u\n\tTexture free memory: %u\n\tRenderbuffer free memory: %u\n",
                          vbo_mem_kb / 1024, texture_mem_kb / 1024, renderbuffer_mem_kb / 1024 );
                          
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ATI_meminfo\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "...GL_ATI_meminfo not found\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ATI_meminfo not found\n" );
    }
    GL_CheckErrors();
    
    if( glewGetExtension( "GL_NVX_gpu_memory_info" ) )
    {
        GLint total_mem_kb = 0, cur_avail_mem_kb = 0;
        glGetIntegerv( GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &total_mem_kb );
        glGetIntegerv( GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur_avail_mem_kb );
        
        glRefConfig.memInfo = MI_NVX;
        CL_RefPrintf( PRINT_ALL, "GL_NVX_gpu_memory_info:\n\tNumber of megabytes of video memory: %u\n\tNumber of megabytes of video memory available to the renderer: %u\n",
                      total_mem_kb / 1024, cur_avail_mem_kb / 1024 );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_NVX_gpu_memory_info not found\n" );
    }
    GL_CheckErrors();
    
    // GL_EXT_texture_compression_s3tc
    glRefConfig.textureCompression = TCR_NONE;
    if( glewGetExtension( "GL_EXT_texture_compression_s3tc" ) )
    {
        if( r_ext_compressed_textures->integer )
        {
            glConfig.textureCompression = TC_S3TC;
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_EXT_texture_compression_s3tc\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "...ignoring GL_EXT_texture_compression_s3tc\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_EXT_texture_compression_s3tc not found\n" );
    }
    GL_CheckErrors();
    
    // GL_EXT_texture_filter_anisotropic
    glConfig.textureFilterAnisotropic = false;
    if( glewGetExtension( "GL_EXT_texture_filter_anisotropic" ) )
    {
        glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glConfig.maxAnisotropy );
        
        if( r_ext_texture_filter_anisotropic->value )
        {
            glConfig.textureFilterAnisotropic = true;
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_EXT_texture_filter_anisotropic\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "...ignoring GL_EXT_texture_filter_anisotropic\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_EXT_texture_filter_anisotropic not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ARB_framebuffer_object
    glRefConfig.framebufferObject = false;
    glRefConfig.framebufferBlit = false;
    //glRefConfig.framebufferMultisample = false;
    if( glewGetExtension( "GL_ARB_framebuffer_object" ) )
    {
        glRefConfig.framebufferObject = r_ext_framebuffer_object->integer;
        glRefConfig.framebufferBlit = true;
        //glRefConfig.framebufferMultisample = true;
        
        glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &glRefConfig.maxRenderbufferSize );
        glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS_EXT, &glRefConfig.maxColorAttachments );
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_framebuffer_object\n" );
        
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_framebuffer_object not found\n" );
    }
    GL_CheckErrors();
    
    // GL_EXT_direct_state_access
    glRefConfig.directStateAccess = false;
    if( glewGetExtension( "GL_EXT_direct_state_access" ) )
    {
        if( r_ext_direct_state_access->integer )
        {
            glRefConfig.directStateAccess = !!r_ext_direct_state_access->integer;
            CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_EXT_direct_state_access\n" );
        }
        else
        {
            CL_RefPrintf( PRINT_ALL, "...ignoring GL_EXT_direct_state_access\n" );
        }
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_EXT_direct_state_access not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ARB_occlusion_query
    glRefConfig.occlusionQuery = false;
    if( glewGetExtension( "GL_ARB_occlusion_query" ) )
    {
        glRefConfig.occlusionQuery = true;
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_occlusion_query\n" );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_occlusion_query not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ARB_texture_compression_rgtc
    if( glewGetExtension( "GL_ARB_texture_compression_rgtc" ) )
    {
        bool useRgtc = !!r_ext_compressed_textures->integer >= 1;
        
        if( useRgtc )
        {
            glRefConfig.textureCompression |= TCR_RGTC;
        }
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_texture_compression_rgtc\n" );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_texture_compression_rgtc not found\n" );
    }
    GL_CheckErrors();
    
    glRefConfig.swizzleNormalmap = !!r_ext_compressed_textures->integer && !( glRefConfig.textureCompression & TCR_RGTC );
    
    // GL_ARB_texture_compression_bptc
    if( glewGetExtension( "GL_ARB_texture_compression_bptc" ) )
    {
        bool useBptc = !!r_ext_compressed_textures->integer >= 2;
        
        if( useBptc )
            glRefConfig.textureCompression |= TCR_BPTC;
            
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_texture_compression_bptc\n" );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_texture_compression_bptc not found\n" );
    }
    GL_CheckErrors();
    
    // GL_ARB_depth_clamp
    glRefConfig.depthClamp = false;
    if( glewGetExtension( "GL_ARB_depth_clamp" ) )
    {
        glRefConfig.depthClamp = true;
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_depth_clamp\n" );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_depth_clamp not found\n" );
    }
    GL_CheckErrors();
    
    //GL_ARB_seamless_cube_map
    glRefConfig.seamlessCubeMap = false;
    if( glewGetExtension( "GL_ARB_seamless_cube_map" ) )
    {
        glRefConfig.seamlessCubeMap = !!r_arb_seamless_cube_map->integer;
        
        CL_RefPrintf( PRINT_ALL, "...found OpenGL extension - GL_ARB_seamless_cube_map\n" );
    }
    else
    {
        CL_RefPrintf( PRINT_ALL, "...GL_ARB_seamless_cube_map not found\n" );
    }
    GL_CheckErrors();
}

#define R_MODE_FALLBACK 3 // 640 * 480

/* Support code for GLimp_Init */

static void reportDriverType( bool force )
{
    static StringEntry const drivers[] =
    {
        "integrated", "stand-alone", "Voodoo", "OpenGL 3+", "Mesa"
    };
    if( glConfig.driverType > GLDRV_UNKNOWN && glConfig.driverType < sizeof( drivers ) / sizeof( drivers[0] ) )
    {
        CL_RefPrintf( PRINT_ALL, "%s graphics driver class '%s'\n", force ? "User has forced" : "Detected", drivers[glConfig.driverType] );
    }
}

static void reportHardwareType( bool force )
{
    static StringEntry const hardware[] =
    {
        "generic", "Voodoo", "Riva 128", "Rage Pro", "Permedia 2", "ATI Radeon", "AMD Radeon DX10-class", "nVidia DX10-class"
    };
    if( glConfig.hardwareType > GLDRV_UNKNOWN && glConfig.driverType < sizeof( hardware ) / sizeof( hardware[0] ) )
    {
        CL_RefPrintf( PRINT_ALL, "%s graphics hardware class '%s'\n",
                      force ? "User has forced" : "Detected",
                      hardware[glConfig.hardwareType] );
    }
}

/*
===============
GLimp_Init

This routine is responsible for initializing the OS specific portions
of OpenGL
===============
*/
void GLimp_Init( void )
{

    //bool        success = true;
    
    glConfig.driverType = GLDRV_ICD;
    
    r_sdlDriver = Cvar_Get( "r_sdlDriver", "", CVAR_ROM );
    r_allowResize = Cvar_Get( "r_allowResize", "0", CVAR_ARCHIVE );
    r_centerWindow = Cvar_Get( "r_centerWindow", "0", CVAR_ARCHIVE );
    r_minimize = Cvar_Get( "r_minimize", "0", CVAR_ARCHIVE );
    
    if( Cvar_VariableIntegerValue( "com_abnormalExit" ) )
    {
        Cvar_Set( "r_mode", va( "%d", R_MODE_FALLBACK ) );
        Cvar_Set( "r_fullscreen", "0" );
        Cvar_Set( "r_centerWindow", "0" );
        Cvar_Set( "com_abnormalExit", "0" );
    }
    
    //Sys_SetEnv("SDL_VIDEO_CENTERED", r_centerWindow->integer ? "1" : "");
    
    Sys_GLimpInit();
    
    // Create the window and set up the context
    if( GLimp_StartDriverAndSetMode( r_mode->integer, r_fullscreen->integer, false ) )
    {
        goto success;
    }
    
    // Try again, this time in a platform specific "safe mode"
    Sys_GLimpSafeInit();
    
    if( GLimp_StartDriverAndSetMode( r_mode->integer, r_fullscreen->integer, false ) )
    {
        goto success;
    }
    
    // Finally, try the default screen resolution
    if( r_mode->integer != R_MODE_FALLBACK )
    {
        CL_RefPrintf( PRINT_ALL, "Setting r_mode %d failed, falling back on r_mode %d\n", r_mode->integer, R_MODE_FALLBACK );
        
        if( GLimp_StartDriverAndSetMode( R_MODE_FALLBACK, false, false ) )
        {
            goto success;
        }
    }
    
    // Nothing worked, give up
    SDL_QuitSubSystem( SDL_INIT_VIDEO );
    return;
    
success:
    // This values force the UI to disable driver selection
    glConfig.hardwareType = GLHW_GENERIC;
    glConfig.deviceSupportsGamma = ( bool )( SDL_SetGamma( 1.0f, 1.0f, 1.0f ) >= 0 );
    
    // Mysteriously, if you use an NVidia graphics card and multiple monitors,
    // SDL_SetGamma will incorrectly return false... the first time; ask
    // again and you get the correct answer. This is a suspected driver bug, see
    // http://bugzilla.icculus.org/show_bug.cgi?id=4316
    glConfig.deviceSupportsGamma = ( bool )( SDL_SetGamma( 1.0f, 1.0f, 1.0f ) >= 0 );
    
    if( r_ignorehwgamma->integer )
    {
        glConfig.deviceSupportsGamma = ( bool )0;
    }
    
    // get our config strings
    Q_strncpyz( glConfig.vendor_string, ( UTF8* ) glGetString( GL_VENDOR ), sizeof( glConfig.vendor_string ) );
    Q_strncpyz( glConfig.renderer_string, ( UTF8* ) glGetString( GL_RENDERER ), sizeof( glConfig.renderer_string ) );
    
    if( *glConfig.renderer_string && glConfig.renderer_string[ strlen( glConfig.renderer_string ) - 1 ] == '\n' )
    {
        glConfig.renderer_string[ strlen( glConfig.renderer_string ) - 1 ] = 0;
    }
    
    Q_strncpyz( glConfig.version_string, ( UTF8* ) glGetString( GL_VERSION ), sizeof( glConfig.version_string ) );
    
    if( glConfig.driverType != GLDRV_OPENGL3 )
    {
        Q_strncpyz( glConfig.extensions_string, ( UTF8* ) glGetString( GL_EXTENSIONS ), sizeof( glConfig.extensions_string ) );
    }
    
    if( Q_stristr( glConfig.renderer_string, "mesa" ) ||
            Q_stristr( glConfig.renderer_string, "gallium" ) ||
            Q_stristr( glConfig.vendor_string, "nouveau" ) ||
            Q_stristr( glConfig.vendor_string, "mesa" ) )
    {
        // suckage
        glConfig.driverType = GLDRV_MESA;
    }
    
    if( Q_stristr( glConfig.renderer_string, "geforce" ) )
    {
        if( Q_stristr( glConfig.renderer_string, "8400" ) ||
                Q_stristr( glConfig.renderer_string, "8500" ) ||
                Q_stristr( glConfig.renderer_string, "8600" ) ||
                Q_stristr( glConfig.renderer_string, "8800" ) ||
                Q_stristr( glConfig.renderer_string, "9500" ) ||
                Q_stristr( glConfig.renderer_string, "9600" ) ||
                Q_stristr( glConfig.renderer_string, "9800" ) ||
                Q_stristr( glConfig.renderer_string, "gts 240" ) ||
                Q_stristr( glConfig.renderer_string, "gts 250" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 260" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 275" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 280" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 285" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 295" ) ||
                Q_stristr( glConfig.renderer_string, "gt 320" ) ||
                Q_stristr( glConfig.renderer_string, "gt 330" ) ||
                Q_stristr( glConfig.renderer_string, "gt 340" ) ||
                Q_stristr( glConfig.renderer_string, "gt 415" ) ||
                Q_stristr( glConfig.renderer_string, "gt 420" ) ||
                Q_stristr( glConfig.renderer_string, "gt 425" ) ||
                Q_stristr( glConfig.renderer_string, "gt 430" ) ||
                Q_stristr( glConfig.renderer_string, "gt 435" ) ||
                Q_stristr( glConfig.renderer_string, "gt 440" ) ||
                Q_stristr( glConfig.renderer_string, "gt 520" ) ||
                Q_stristr( glConfig.renderer_string, "gt 525" ) ||
                Q_stristr( glConfig.renderer_string, "gt 540" ) ||
                Q_stristr( glConfig.renderer_string, "gt 550" ) ||
                Q_stristr( glConfig.renderer_string, "gt 555" ) ||
                Q_stristr( glConfig.renderer_string, "gts 450" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 460" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 470" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 480" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 485" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 560" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 570" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 580" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 590" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 630" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 640" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 645" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 670" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 680" ) ||
                Q_stristr( glConfig.renderer_string, "gtx 690" ) )
                
            glConfig.hardwareType = GLHW_NV_DX10;
            
    }
    else if( Q_stristr( glConfig.renderer_string, "quadro fx" ) )
    {
        if( Q_stristr( glConfig.renderer_string, "3600" ) )
        {
            glConfig.hardwareType = GLHW_NV_DX10;
        }
    }
    else if( Q_stristr( glConfig.renderer_string, "gallium" ) &&
             Q_stristr( glConfig.renderer_string, " amd " ) )
    {
        // anything prior to R600 is listed as ATI.
        glConfig.hardwareType = GLHW_ATI_DX10;
    }
    else if( Q_stristr( glConfig.renderer_string, "rv770" ) ||
             Q_stristr( glConfig.renderer_string, "eah4850" ) ||
             Q_stristr( glConfig.renderer_string, "eah4870" ) ||
             // previous three are too specific?
             Q_stristr( glConfig.renderer_string, "radeon hd" ) )
    {
        glConfig.hardwareType = GLHW_ATI_DX10;
    }
    else if( Q_stristr( glConfig.renderer_string, "radeon" ) )
    {
        glConfig.hardwareType = GLHW_ATI_DX10;
    }
    // Check if we need Intel graphics specific fixes.
    else if( Q_stristr( glConfig.renderer_string, "Intel" ) )
    {
        glRefConfig.intelGraphics = true;
    }
    
    reportDriverType( false );
    reportHardwareType( false );
    
    {
        // allow overriding where the user really does know better
        cvar_t*          forceGL;
        glDriverType_t   driverType   = GLDRV_UNKNOWN;
        glHardwareType_t hardwareType = GLHW_UNKNOWN;
        
        forceGL = Cvar_Get( "r_glForceDriver", "", CVAR_LATCH );
        
        if( !Q_stricmp( forceGL->string, "icd" ) )
        {
            driverType = GLDRV_ICD;
        }
        else if( !Q_stricmp( forceGL->string, "standalone" ) )
        {
            driverType = GLDRV_STANDALONE;
        }
        else if( !Q_stricmp( forceGL->string, "voodoo" ) )
        {
            driverType = GLDRV_VOODOO;
        }
        else if( !Q_stricmp( forceGL->string, "opengl3" ) )
        {
            driverType = GLDRV_OPENGL3;
        }
        else if( !Q_stricmp( forceGL->string, "mesa" ) )
        {
            driverType = GLDRV_MESA;
        }
        
        forceGL = Cvar_Get( "r_glForceHardware", "", CVAR_LATCH );
        
        if( !Q_stricmp( forceGL->string, "generic" ) )
        {
            hardwareType = GLHW_GENERIC;
        }
        else if( !Q_stricmp( forceGL->string, "voodoo" ) )
        {
            hardwareType = GLHW_3DFX_2D3D;
        }
        else if( !Q_stricmp( forceGL->string, "riva128" ) )
        {
            hardwareType = GLHW_RIVA128;
        }
        else if( !Q_stricmp( forceGL->string, "ragepro" ) )
        {
            hardwareType = GLHW_RAGEPRO;
        }
        else if( !Q_stricmp( forceGL->string, "permedia2" ) )
        {
            hardwareType = GLHW_PERMEDIA2;
        }
        else if( !Q_stricmp( forceGL->string, "ati" ) )
        {
            hardwareType = GLHW_ATI_DX10;
        }
        else if( !Q_stricmp( forceGL->string, "atidx10" ) ||
                 !Q_stricmp( forceGL->string, "radeonhd" ) ||
                 !Q_stricmp( forceGL->string, "radeon" ) )
        {
            hardwareType = GLHW_ATI_DX10;
        }
        else if( !Q_stricmp( forceGL->string, "nvdx10" ) )
        {
            hardwareType = GLHW_NV_DX10;
        }
        
        if( driverType != GLDRV_UNKNOWN )
        {
            glConfig.driverType = driverType;
            reportDriverType( true );
        }
        
        if( hardwareType != GLHW_UNKNOWN )
        {
            glConfig.hardwareType = hardwareType;
            reportHardwareType( true );
        }
    }
    
    // initialize extensions
    GLimp_OGL3InitExtensions();
    
    Cvar_Get( "r_availableModes", "", CVAR_ROM );
    
    // This depends on SDL_INIT_VIDEO, hence having it here
    IN_Init();
    
    return;
}


/*
===============
GLimp_EndFrame

Responsible for doing a swapbuffers
===============
*/
void GLimp_EndFrame( void )
{
    // don't flip if drawing to front buffer
    if( Q_stricmp( r_drawBuffer->string, "GL_FRONT" ) != 0 )
    {
        SDL_GL_SwapBuffers();
    }
    
    if( r_minimize && r_minimize->integer )
    {
    
#ifdef MACOS_X
        SDL_Surface* s = SDL_GetVideoSurface();
        bool    fullscreen = ( s && ( s->flags & SDL_FULLSCREEN ) );
        
        if( !fullscreen )
        {
            SDL_WM_IconifyWindow();
            Cvar_Set( "r_minimize", "0" );
        }
        else if( r_fullscreen->integer )
        {
            Cvar_Set( "r_fullscreen", "0" );
        }
        
#else
        SDL_WM_IconifyWindow();
        Cvar_Set( "r_minimize", "0" );
#endif
    }
    
    if( r_fullscreen->modified )
    {
        bool    fullscreen;
        bool    needToToggle = true;
        bool    sdlToggled = false;
        SDL_Surface* s = SDL_GetVideoSurface();
        
        if( s )
        {
            // Find out the current state
            fullscreen = ( bool )!!( s->flags & SDL_FULLSCREEN );
            
            if( r_fullscreen->integer && Cvar_VariableIntegerValue( "in_nograb" ) )
            {
                CL_RefPrintf( PRINT_ALL, "Fullscreen not allowed with in_nograb 1\n" );
                Cvar_Set( "r_fullscreen", "0" );
                r_fullscreen->modified = false;
            }
            
            // Is the state we want different from the current state?
            needToToggle = ( bool )( !!r_fullscreen->integer != fullscreen );
            
            if( needToToggle )
            {
                sdlToggled = ( bool )SDL_WM_ToggleFullScreen( s );
            }
        }
        
        if( needToToggle )
        {
            // SDL_WM_ToggleFullScreen didn't work, so do it the slow way
            if( !sdlToggled )
            {
                Cbuf_ExecuteText( EXEC_APPEND, "vid_restart\n" );
            }
            
            IN_Restart();
        }
        
        r_fullscreen->modified = false;
    }
    
}


void GLimp_LogComment( StringEntry comment )
{
    static UTF8 buf[ 4096 ];
    
    if( r_logFile->integer && GLEW_GREMEDY_string_marker )
    {
        // copy string and ensure it has a trailing '\0'
        Q_strncpyz( buf, comment, sizeof( buf ) );
        
        glStringMarkerGREMEDY( strlen( buf ), buf );
    }
}
