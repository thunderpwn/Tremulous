////////////////////////////////////////////////////////////////////////////////////////
// Copyright(C) 1999 - 2005 Id Software, Inc.
// Copyright(C) 2000 - 2006 Tim Angus
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
// File name:   cg_drawtools.cpp
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description: helper functions called by cg_draw, cg_scoreboard, cg_info, etc
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#include <cgame/cg_precompiled.h>

/*
===============
CG_DrawPlane

Draw a quad in 3 space - basically CG_DrawPic in 3 space
===============
*/
void idCGameLocal::DrawPlane( vec3_t origin, vec3_t down, vec3_t right, qhandle_t shader )
{
    polyVert_t  verts[ 4 ];
    vec3_t      temp;
    
    VectorCopy( origin, verts[ 0 ].xyz );
    verts[ 0 ].st[ 0 ] = 0;
    verts[ 0 ].st[ 1 ] = 0;
    verts[ 0 ].modulate[ 0 ] = 255;
    verts[ 0 ].modulate[ 1 ] = 255;
    verts[ 0 ].modulate[ 2 ] = 255;
    verts[ 0 ].modulate[ 3 ] = 255;
    
    VectorAdd( origin, right, temp );
    VectorCopy( temp, verts[ 1 ].xyz );
    verts[ 1 ].st[ 0 ] = 1;
    verts[ 1 ].st[ 1 ] = 0;
    verts[ 1 ].modulate[ 0 ] = 255;
    verts[ 1 ].modulate[ 1 ] = 255;
    verts[ 1 ].modulate[ 2 ] = 255;
    verts[ 1 ].modulate[ 3 ] = 255;
    
    VectorAdd( origin, right, temp );
    VectorAdd( temp, down, temp );
    VectorCopy( temp, verts[ 2 ].xyz );
    verts[ 2 ].st[ 0 ] = 1;
    verts[ 2 ].st[ 1 ] = 1;
    verts[ 2 ].modulate[ 0 ] = 255;
    verts[ 2 ].modulate[ 1 ] = 255;
    verts[ 2 ].modulate[ 2 ] = 255;
    verts[ 2 ].modulate[ 3 ] = 255;
    
    VectorAdd( origin, down, temp );
    VectorCopy( temp, verts[ 3 ].xyz );
    verts[ 3 ].st[ 0 ] = 0;
    verts[ 3 ].st[ 1 ] = 1;
    verts[ 3 ].modulate[ 0 ] = 255;
    verts[ 3 ].modulate[ 1 ] = 255;
    verts[ 3 ].modulate[ 2 ] = 255;
    verts[ 3 ].modulate[ 3 ] = 255;
    
    trap_R_AddPolyToScene( shader, 4, verts );
}

/*
================
CG_AdjustFrom640

Adjusted for resolution and screen aspect ratio
================
*/
void idCGameLocal::AdjustFrom640( F32* x, F32* y, F32* w, F32* h )
{
#if 0
    // adjust for wide screens
    if( cgs.glconfig.vidWidth * 480 > cgs.glconfig.vidHeight * 640 )
    {
        *x += 0.5 * ( cgs.glconfig.vidWidth - ( cgs.glconfig.vidHeight * 640 / 480 ) );
    }
#endif
    // scale for screen sizes
    *x *= cgs.screenXScale;
    *y *= cgs.screenYScale;
    *w *= cgs.screenXScale;
    *h *= cgs.screenYScale;
}


/*
================
CG_DrawSides

Coords are virtual 640x480
================
*/
void idCGameLocal::DrawSides( F32 x, F32 y, F32 w, F32 h, F32 size )
{
    AdjustFrom640( &x, &y, &w, &h );
    size *= cgs.screenXScale;
    trap_R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, cgs.media.whiteShader );
    trap_R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, cgs.media.whiteShader );
}

void idCGameLocal::DrawTopBottom( F32 x, F32 y, F32 w, F32 h, F32 size )
{
    AdjustFrom640( &x, &y, &w, &h );
    size *= cgs.screenYScale;
    trap_R_DrawStretchPic( x, y, w, size, 0, 0, 0, 0, cgs.media.whiteShader );
    trap_R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, cgs.media.whiteShader );
}

void idCGameLocal::DrawCorners( F32 x, F32 y, F32 w, F32 h, F32 size, qhandle_t pic )
{
    F32 hs, vs;
    AdjustFrom640( &x, &y, &w, &h );
    hs = size * cgs.screenXScale;
    vs = size * cgs.screenYScale;
    
    trap_R_DrawStretchPic( x, y, hs , vs, 0, 0, 0.5, 0.5, pic );
    trap_R_DrawStretchPic( x, y + h - vs, hs, vs, 0, 0.5, 0.5, 1, pic );
    trap_R_DrawStretchPic( x + w - hs, y, hs, vs, 0.5, 0, 1, 0.5, pic );
    trap_R_DrawStretchPic( x + w - hs, y + h - vs, hs, vs, 0.5, 0.5, 1, 1, pic );
}


/*
================
CG_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void idCGameLocal::DrawRect( F32 x, F32 y, F32 width, F32 height, F32 size, const F32* color )
{
    trap_R_SetColor( color );
    
    DrawTopBottom( x, y, width, height, size );
    DrawSides( x, y, width, height, size );
    
    trap_R_SetColor( NULL );
}


/*
================
CG_DrawRoundedRect

Coordinates are 640*480 virtual values
=================
*/
void idCGameLocal::DrawRoundedRect( F32 x, F32 y, F32 width, F32 height, F32 size, const F32* color )
{
    trap_R_SetColor( color );
    
    DrawTopBottom( x + size * 4, y, width - size * 8, height, size );
    DrawSides( x, y + size * 4, width, height - size * 8, size );
    DrawCorners( x, y, width, height, size * 4, cgDC.Assets.cornerOut );
    
    trap_R_SetColor( NULL );
}


/*
================
CG_FillRect

Coordinates are 640*480 virtual values
=================
*/
void idCGameLocal::FillRect( F32 x, F32 y, F32 width, F32 height, const F32* color )
{
    trap_R_SetColor( color );
    
    AdjustFrom640( &x, &y, &width, &height );
    trap_R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, cgs.media.whiteShader );
    
    trap_R_SetColor( NULL );
}


/*
================
CG_FillRoundedRect

Coordinates are 640*480 virtual values
=================
*/
void idCGameLocal::FillRoundedRect( F32 x, F32 y, F32 width, F32 height, F32 size, const F32* color )
{
    FillRect( x, y + size * 3, width, height - size * 6, color );
    
    trap_R_SetColor( color );
    
    DrawTopBottom( x + size * 3, y, width - size * 6, height, size * 3 );
    DrawCorners( x - size, y - size, width + size * 2, height + size * 2, size * 4, cgDC.Assets.cornerIn );
    
    trap_R_SetColor( NULL );
}


/*
================
CG_DrawPic

Coordinates are 640*480 virtual values
=================
*/
void idCGameLocal::DrawPic( F32 x, F32 y, F32 width, F32 height, qhandle_t hShader )
{
    AdjustFrom640( &x, &y, &width, &height );
    trap_R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}



/*
================
CG_DrawFadePic

Coordinates are 640*480 virtual values
=================
*/
void idCGameLocal::DrawFadePic( F32 x, F32 y, F32 width, F32 height, vec4_t fcolor, vec4_t tcolor, F32 amount, qhandle_t hShader )
{
    vec4_t  finalcolor;
    F32   inverse;
    
    inverse = 100 - amount;
    
    AdjustFrom640( &x, &y, &width, &height );
    
    finalcolor[ 0 ] = ( ( inverse * fcolor[ 0 ] ) + ( amount * tcolor[ 0 ] ) ) / 100;
    finalcolor[ 1 ] = ( ( inverse * fcolor[ 1 ] ) + ( amount * tcolor[ 1 ] ) ) / 100;
    finalcolor[ 2 ] = ( ( inverse * fcolor[ 2 ] ) + ( amount * tcolor[ 2 ] ) ) / 100;
    finalcolor[ 3 ] = ( ( inverse * fcolor[ 3 ] ) + ( amount * tcolor[ 3 ] ) ) / 100;
    
    trap_R_SetColor( finalcolor );
    trap_R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
    trap_R_SetColor( NULL );
}

/*
=================
CG_DrawStrlen

Returns character count, skiping color escape codes
=================
*/
S32 idCGameLocal::DrawStrlen( StringEntry str )
{
    StringEntry  s = str;
    S32         count = 0;
    
    while( *s )
    {
        if( Q_IsColorString( s ) )
            s += 2;
        else
        {
            count++;
            s++;
        }
    }
    
    return count;
}

/*
=============
CG_TileClearBox

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void idCGameLocal::TileClearBox( S32 x, S32 y, S32 w, S32 h, qhandle_t hShader )
{
    F32 s1, t1, s2, t2;
    
    s1 = x / 64.0;
    t1 = y / 64.0;
    s2 = ( x + w ) / 64.0;
    t2 = ( y + h ) / 64.0;
    trap_R_DrawStretchPic( x, y, w, h, s1, t1, s2, t2, hShader );
}



/*
==============
CG_TileClear

Clear around a sized down screen
==============
*/
void idCGameLocal::TileClear( void )
{
    S32   top, bottom, left, right;
    S32   w, h;
    
    w = cgs.glconfig.vidWidth;
    h = cgs.glconfig.vidHeight;
    
    if( cg.refdef.x == 0 && cg.refdef.y == 0 && cg.refdef.width == w && cg.refdef.height == h )
        return;   // full screen rendering
        
    top = cg.refdef.y;
    bottom = top + cg.refdef.height - 1;
    left = cg.refdef.x;
    right = left + cg.refdef.width - 1;
    
    // clear above view screen
    TileClearBox( 0, 0, w, top, cgs.media.backTileShader );
    
    // clear below view screen
    TileClearBox( 0, bottom, w, h - bottom, cgs.media.backTileShader );
    
    // clear left of view screen
    TileClearBox( 0, top, left, bottom - top + 1, cgs.media.backTileShader );
    
    // clear right of view screen
    TileClearBox( right, top, w - right, bottom - top + 1, cgs.media.backTileShader );
}

/*
================
CG_FadeColor
================
*/
F32* idCGameLocal::FadeColor( S32 startMsec, S32 totalMsec )
{
    static vec4_t   color;
    S32     t;
    
    if( startMsec == 0 )
        return NULL;
        
    t = cg.time - startMsec;
    
    if( t >= totalMsec )
        return NULL;
        
    // fade out
    if( totalMsec - t < FADE_TIME )
        color[ 3 ] = ( totalMsec - t ) * 1.0 / FADE_TIME;
    else
        color[ 3 ] = 1.0;
        
    color[ 0 ] = color[ 1 ] = color[ 2 ] = 1;
    
    return color;
}

/*
================
CG_WorldToScreen
================
*/
bool idCGameLocal::WorldToScreen( vec3_t poS32, F32* x, F32* y )
{
    vec3_t  trans;
    F32   xc, yc;
    F32   px, py;
    F32   z;
    
    px = tan( cg.refdef.fov_x * M_PI / 360.0 );
    py = tan( cg.refdef.fov_y * M_PI / 360.0 );
    
    VectorSubtract( poS32, cg.refdef.vieworg, trans );
    
    xc = 640.0f / 2.0f;
    yc = 480.0f / 2.0f;
    
    z = DotProduct( trans, cg.refdef.viewaxis[ 0 ] );
    if( z <= 0.001f )
        return false;
        
    if( x )
        *x = xc - DotProduct( trans, cg.refdef.viewaxis[ 1 ] ) * xc / ( z * px );
        
    if( y )
        *y = yc - DotProduct( trans, cg.refdef.viewaxis[ 2 ] ) * yc / ( z * py );
        
    return true;
}

/*
================
CG_WorldToScreenWrap
================
*/
bool idCGameLocal::WorldToScreenWrap( vec3_t poS32, F32* x, F32* y )
{
    vec3_t trans;
    F32 px, py, dotForward, dotRight, dotUp, distance, propX, propY;
    
    px = tan( cg.refdef.fov_x * M_PI / 360.0 );
    py = tan( cg.refdef.fov_y * M_PI / 360.0 );
    
    VectorSubtract( poS32, cg.refdef.vieworg, trans );
    
    dotForward = DotProduct( trans, cg.refdef.viewaxis[ 0 ] );
    dotRight = DotProduct( trans, cg.refdef.viewaxis[ 1 ] );
    dotUp = DotProduct( trans, cg.refdef.viewaxis[ 2 ] );
    
    distance = fabs( dotForward );
    propX = dotRight / ( distance * px );
    propY = dotUp / ( distance * py );
    
    // The distance along the forward axis does not make sense once the poS32
    // moves off-screen so we need to use either the side or the up axis instead
    if( propX < -1.f || propX > 1.f )
    {
        distance = fabs( dotRight ) / px;
        propY = dotUp / ( distance * py );
    }
    if( propY < -1.f || propY > 1.f )
    {
        distance = fabs( dotUp ) / py;
        propX = dotRight / ( distance * px );
    }
    
    if( x )
        *x = 320 - propX * 320;
    if( y )
        *y = 240 - propY * 240;
        
    // Snap to the edge of the screen when the poS32 is behind us
    if( dotForward < 0.f && *x > 0 && *x < 640 && *y > 0 && *y < 480 )
    {
        if( fabs( *x - 320 ) > fabs( *y - 240 ) )
            *x = *x <= 320 ? 0.f : 640;
        else
            *y = *y <= 240 ? 0.f : 480;
    }
    
    return true;
}

/*
================
CG_KeyBinding
================
*/
UTF8* idCGameLocal::KeyBinding( StringEntry bind )
{
    static UTF8 key[ 32 ];
    UTF8 bindbuff[ MAX_CVAR_VALUE_STRING ];
    S32 i;
    
    key[ 0 ] = '\0';
    // NOTE: change K_LAST_KEY to MAX_KEYS for full key support (eventually)
    for( i = 0; i < K_LAST_KEY; i++ )
    {
        trap_Key_GetBindingBuf( i, bindbuff, sizeof( bindbuff ) );
        if( !Q_stricmp( bindbuff, bind ) )
        {
            trap_Key_KeynumToStringBuf( i, key, sizeof( key ) );
            break;
        }
    }
    if( !key[ 0 ] )
    {
        Q_strncpyz( key, "\\", sizeof( key ) );
        Q_strcat( key, sizeof( key ), bind );
    }
    return key;
}
