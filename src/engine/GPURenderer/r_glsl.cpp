////////////////////////////////////////////////////////////////////////////////////////
// Copyright(C) 2006 - 2008 Robert Beckebans <trebor_7@users.sourceforge.net>
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
// File name:   r_glsl.cpp
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description:
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#include <OWLib/precompiled.h>

const GPUProgramDesc fallback_null;

struct uniformInfo_t
{
    StringEntry name;
    S32 type;
};

// These must be in the same order as in uniform_t in r_local.h.
static uniformInfo_t uniformsInfo[] =
{
    { "u_DiffuseMap",  GLSL_INT },
    { "u_LightMap",    GLSL_INT },
    { "u_NormalMap",   GLSL_INT },
    { "u_DeluxeMap",   GLSL_INT },
    { "u_SpecularMap", GLSL_INT },
    
    { "u_TextureMap", GLSL_INT },
    { "u_LevelsMap",  GLSL_INT },
    { "u_CubeMap",    GLSL_INT },
    
    { "u_ScreenImageMap", GLSL_INT },
    { "u_ScreenDepthMap", GLSL_INT },
    
    { "u_ShadowMap",  GLSL_INT },
    { "u_ShadowMap2", GLSL_INT },
    { "u_ShadowMap3", GLSL_INT },
    { "u_ShadowMap4", GLSL_INT },
    
    { "u_ShadowMvp",  GLSL_MAT16 },
    { "u_ShadowMvp2", GLSL_MAT16 },
    { "u_ShadowMvp3", GLSL_MAT16 },
    { "u_ShadowMvp4", GLSL_MAT16 },
    
    { "u_EnableTextures", GLSL_VEC4 },
    
    { "u_DiffuseTexMatrix",  GLSL_VEC4 },
    { "u_DiffuseTexOffTurb", GLSL_VEC4 },
    
    { "u_TCGen0",        GLSL_INT },
    { "u_TCGen0Vector0", GLSL_VEC3 },
    { "u_TCGen0Vector1", GLSL_VEC3 },
    
    { "u_DeformGen",    GLSL_INT },
    { "u_DeformParams", GLSL_FLOAT5 },
    
    { "u_ColorGen",  GLSL_INT },
    { "u_AlphaGen",  GLSL_INT },
    { "u_Color",     GLSL_VEC4 },
    { "u_BaseColor", GLSL_VEC4 },
    { "u_VertColor", GLSL_VEC4 },
    
    { "u_DlightInfo",    GLSL_VEC4 },
    { "u_LightForward",  GLSL_VEC3 },
    { "u_LightUp",       GLSL_VEC3 },
    { "u_LightRight",    GLSL_VEC3 },
    { "u_LightOrigin",   GLSL_VEC4 },
    { "u_LightOrigin1",   GLSL_VEC4 },
    { "u_LightOrigin2",   GLSL_VEC4 },
    { "u_LightOrigin3",   GLSL_VEC4 },
    { "u_LightOrigin4",   GLSL_VEC4 },
    { "u_LightOrigin5",   GLSL_VEC4 },
    { "u_LightOrigin6",   GLSL_VEC4 },
    { "u_LightOrigin7",   GLSL_VEC4 },
    { "u_LightOrigin8",   GLSL_VEC4 },
    { "u_LightOrigin9",   GLSL_VEC4 },
    { "u_LightOrigin10",   GLSL_VEC4 },
    { "u_LightOrigin11",   GLSL_VEC4 },
    { "u_LightOrigin12",   GLSL_VEC4 },
    { "u_LightOrigin13",   GLSL_VEC4 },
    { "u_LightOrigin14",   GLSL_VEC4 },
    { "u_LightOrigin15",   GLSL_VEC4 },
    { "u_LightOrigin16",   GLSL_VEC4 },
    { "u_LightOrigin17",   GLSL_VEC4 },
    { "u_LightOrigin18",   GLSL_VEC4 },
    { "u_LightOrigin19",   GLSL_VEC4 },
    { "u_LightOrigin20",   GLSL_VEC4 },
    { "u_LightOrigin21",   GLSL_VEC4 },
    { "u_LightOrigin22",   GLSL_VEC4 },
    { "u_LightOrigin23",   GLSL_VEC4 },
    { "u_LightOrigin24",   GLSL_VEC4 },
    { "u_LightOrigin25",   GLSL_VEC4 },
    { "u_LightOrigin26",   GLSL_VEC4 },
    { "u_LightOrigin27",   GLSL_VEC4 },
    { "u_LightOrigin28",   GLSL_VEC4 },
    { "u_LightOrigin29",   GLSL_VEC4 },
    { "u_LightOrigin30",   GLSL_VEC4 },
    { "u_LightOrigin31",   GLSL_VEC4 },
    { "u_LightColor",   GLSL_VEC4, },
    { "u_LightColor1",   GLSL_VEC4 },
    { "u_LightColor2",   GLSL_VEC4 },
    { "u_LightColor3",   GLSL_VEC4 },
    { "u_LightColor4",   GLSL_VEC4 },
    { "u_LightColor5",   GLSL_VEC4 },
    { "u_LightColor6",   GLSL_VEC4 },
    { "u_LightColor7",   GLSL_VEC4 },
    { "u_LightColor8",   GLSL_VEC4 },
    { "u_LightColor9",   GLSL_VEC4 },
    { "u_LightColor10",   GLSL_VEC4 },
    { "u_LightColor11",   GLSL_VEC4 },
    { "u_LightColor12",   GLSL_VEC4 },
    { "u_LightColor13",   GLSL_VEC4 },
    { "u_LightColor14",   GLSL_VEC4 },
    { "u_LightColor15",   GLSL_VEC4 },
    { "u_LightColor16",   GLSL_VEC4 },
    { "u_LightColor17",   GLSL_VEC4 },
    { "u_LightColor18",   GLSL_VEC4 },
    { "u_LightColor19",   GLSL_VEC4 },
    { "u_LightColor20",   GLSL_VEC4 },
    { "u_LightColor21",   GLSL_VEC4 },
    { "u_LightColor22",   GLSL_VEC4 },
    { "u_LightColor23",   GLSL_VEC4 },
    { "u_LightColor24",   GLSL_VEC4 },
    { "u_LightColor25",   GLSL_VEC4 },
    { "u_LightColor26",   GLSL_VEC4 },
    { "u_LightColor27",   GLSL_VEC4 },
    { "u_LightColor28",   GLSL_VEC4 },
    { "u_LightColor29",   GLSL_VEC4 },
    { "u_LightColor30",   GLSL_VEC4 },
    { "u_LightColor31",   GLSL_VEC4 },
    { "u_ModelLightDir", GLSL_VEC3 },
    { "u_LightRadius",   GLSL_FLOAT },
    { "u_LightRadius1",   GLSL_FLOAT },
    { "u_LightRadius2",   GLSL_FLOAT },
    { "u_LightRadius3",   GLSL_FLOAT },
    { "u_LightRadius4",   GLSL_FLOAT },
    { "u_LightRadius5",   GLSL_FLOAT },
    { "u_LightRadius6",   GLSL_FLOAT },
    { "u_LightRadius7",   GLSL_FLOAT },
    { "u_LightRadius8",   GLSL_FLOAT },
    { "u_LightRadius9",   GLSL_FLOAT },
    { "u_LightRadius10",   GLSL_FLOAT },
    { "u_LightRadius11",   GLSL_FLOAT },
    { "u_LightRadius12",   GLSL_FLOAT },
    { "u_LightRadius13",   GLSL_FLOAT },
    { "u_LightRadius14",   GLSL_FLOAT },
    { "u_LightRadius15",   GLSL_FLOAT },
    { "u_LightRadius16",   GLSL_FLOAT },
    { "u_LightRadius17",   GLSL_FLOAT },
    { "u_LightRadius18",   GLSL_FLOAT },
    { "u_LightRadius19",   GLSL_FLOAT },
    { "u_LightRadius20",   GLSL_FLOAT },
    { "u_LightRadius21",   GLSL_FLOAT },
    { "u_LightRadius22",   GLSL_FLOAT },
    { "u_LightRadius23",   GLSL_FLOAT },
    { "u_LightRadius24",   GLSL_FLOAT },
    { "u_LightRadius25",   GLSL_FLOAT },
    { "u_LightRadius26",   GLSL_FLOAT },
    { "u_LightRadius27",   GLSL_FLOAT },
    { "u_LightRadius28",   GLSL_FLOAT },
    { "u_LightRadius29",   GLSL_FLOAT },
    { "u_LightRadius30",   GLSL_FLOAT },
    { "u_LightRadius31",   GLSL_FLOAT },
    { "u_AmbientLight",  GLSL_VEC3 },
    { "u_DirectedLight", GLSL_VEC3 },
    
    { "u_PortalRange", GLSL_FLOAT },
    
    { "u_FogDistance",  GLSL_VEC4 },
    { "u_FogDepth",     GLSL_VEC4 },
    { "u_FogEyeT",      GLSL_FLOAT },
    { "u_FogColorMask", GLSL_VEC4 },
    
    { "u_ModelMatrix",               GLSL_MAT16 },
    { "u_ModelViewProjectionMatrix", GLSL_MAT16 },
    { "u_invProjectionMatrix", GLSL_MAT16 },
    { "u_invEyeProjectionMatrix", GLSL_MAT16 },
    
    { "u_Time",          GLSL_FLOAT },
    { "u_VertexLerp",   GLSL_FLOAT },
    { "u_NormalScale",   GLSL_VEC4 },
    { "u_SpecularScale", GLSL_VEC4 },
    
    { "u_ViewInfo",        GLSL_VEC4 },
    { "u_ViewOrigin",      GLSL_VEC3 },
    { "u_LocalViewOrigin", GLSL_VEC3 },
    { "u_ViewForward",     GLSL_VEC3 },
    { "u_ViewLeft",        GLSL_VEC3 },
    { "u_ViewUp",          GLSL_VEC3 },
    
    { "u_InvTexRes",           GLSL_VEC2 },
    { "u_AutoExposureMinMax",  GLSL_VEC2 },
    { "u_ToneMinAvgMaxLinear", GLSL_VEC3 },
    
    { "u_PrimaryLightOrigin",  GLSL_VEC4  },
    { "u_PrimaryLightColor",   GLSL_VEC3  },
    { "u_PrimaryLightAmbient", GLSL_VEC3  },
    { "u_PrimaryLightRadius",  GLSL_FLOAT },
    
    { "u_CubeMapInfo", GLSL_VEC4 },
    
    { "u_AlphaTest", GLSL_INT },
    
    { "u_Brightness",	GLSL_FLOAT },
    { "u_Contrast",		GLSL_FLOAT },
    { "u_Gamma",		GLSL_FLOAT },
    
    { "u_Dimensions",           GLSL_VEC2 },
    { "u_HeightMap",			GLSL_INT },
    { "u_Local0",				GLSL_VEC4 },
    { "u_Local1",				GLSL_VEC4 },
    { "u_Local2",				GLSL_VEC4 },
    { "u_Local3",				GLSL_VEC4 },
    { "u_Texture0",				GLSL_INT },
    { "u_Texture1",				GLSL_INT },
    { "u_Texture2",				GLSL_INT },
    { "u_Texture3",				GLSL_INT },
};

typedef enum
{
    GLSL_PRINTLOG_PROGRAM_INFO,
    GLSL_PRINTLOG_SHADER_INFO,
    GLSL_PRINTLOG_SHADER_SOURCE
}
glslPrintLog_t;

static void GLSL_PrintLog( U32 programOrShader, glslPrintLog_t type, bool developerOnly )
{
    UTF8*           msg;
    static UTF8     msgPart[1024];
    S32             maxLength = 0;
    S32             i;
    S32             printLevel = developerOnly ? PRINT_DEVELOPER : PRINT_ALL;
    
    switch( type )
    {
        case GLSL_PRINTLOG_PROGRAM_INFO:
            CL_RefPrintf( printLevel, "Program info log:\n" );
            glGetProgramiv( programOrShader, GL_INFO_LOG_LENGTH, &maxLength );
            break;
            
        case GLSL_PRINTLOG_SHADER_INFO:
            CL_RefPrintf( printLevel, "Shader info log:\n" );
            glGetShaderiv( programOrShader, GL_INFO_LOG_LENGTH, &maxLength );
            break;
            
        case GLSL_PRINTLOG_SHADER_SOURCE:
            CL_RefPrintf( printLevel, "Shader source:\n" );
            glGetShaderiv( programOrShader, GL_SHADER_SOURCE_LENGTH, &maxLength );
            break;
    }
    
    if( maxLength <= 0 )
    {
        CL_RefPrintf( printLevel, "None.\n" );
        return;
    }
    
    if( maxLength < 1023 )
        msg = msgPart;
    else
        msg = ( UTF8* )CL_RefMalloc( maxLength );
        
    switch( type )
    {
        case GLSL_PRINTLOG_PROGRAM_INFO:
            glGetProgramInfoLog( programOrShader, maxLength, &maxLength, msg );
            break;
            
        case GLSL_PRINTLOG_SHADER_INFO:
            glGetShaderInfoLog( programOrShader, maxLength, &maxLength, msg );
            break;
            
        case GLSL_PRINTLOG_SHADER_SOURCE:
            glGetShaderSource( programOrShader, maxLength, &maxLength, msg );
            break;
    }
    
    if( maxLength < 1023 )
    {
        msgPart[maxLength + 1] = '\0';
        
        CL_RefPrintf( printLevel, "%s\n", msgPart );
    }
    else
    {
        for( i = 0; i < maxLength; i += 1023 )
        {
            Q_strncpyz( msgPart, msg + i, sizeof( msgPart ) );
            
            CL_RefPrintf( printLevel, "%s", msgPart );
        }
        
        CL_RefPrintf( printLevel, "\n" );
        
        Z_Free( msg );
    }
    
}

static void GLSL_GetShaderHeader( U32 shaderType, StringEntry extra, S32 firstLine, UTF8* dest, S64 size )
{
    F32 fbufWidthScale, fbufHeightScale;
    
    dest[0] = '\0';
    
    Q_strcat( dest, size, "#version 150 core\n" );
    
    if( shaderType == GL_VERTEX_SHADER )
    {
        Q_strcat( dest, size, "#define attribute in\n" );
        Q_strcat( dest, size, "#define varying out\n" );
    }
    else
    {
        Q_strcat( dest, size, "#define varying in\n" );
        
        Q_strcat( dest, size, "out vec4 out_Color;\n" );
        Q_strcat( dest, size, "#define gl_FragColor out_Color\n" );
    }
    
    // HACK: add some macros to avoid extra uniforms and save speed and code maintenance
    //Q_strcat(dest, size,
    //		 va("#ifndef r_SpecularExponent\n#define r_SpecularExponent %f\n#endif\n", r_specularExponent->value));
    //Q_strcat(dest, size,
    //		 va("#ifndef r_SpecularScale\n#define r_SpecularScale %f\n#endif\n", r_specularScale->value));
    //Q_strcat(dest, size,
    //       va("#ifndef r_NormalScale\n#define r_NormalScale %f\n#endif\n", r_normalScale->value));
    
    
    Q_strcat( dest, size, "#ifndef M_PI\n#define M_PI 3.14159265358979323846\n#endif\n" );
    
    //Q_strcat(dest, size, va("#ifndef MAX_SHADOWMAPS\n#define MAX_SHADOWMAPS %i\n#endif\n", MAX_SHADOWMAPS));
    
    Q_strcat( dest, size,
              va( "#ifndef deformGen_t\n"
                  "#define deformGen_t\n"
                  "#define DGEN_WAVE_SIN %i\n"
                  "#define DGEN_WAVE_SQUARE %i\n"
                  "#define DGEN_WAVE_TRIANGLE %i\n"
                  "#define DGEN_WAVE_SAWTOOTH %i\n"
                  "#define DGEN_WAVE_INVERSE_SAWTOOTH %i\n"
                  "#define DGEN_BULGE %i\n"
                  "#define DGEN_MOVE %i\n"
                  "#endif\n",
                  DGEN_WAVE_SIN,
                  DGEN_WAVE_SQUARE,
                  DGEN_WAVE_TRIANGLE,
                  DGEN_WAVE_SAWTOOTH,
                  DGEN_WAVE_INVERSE_SAWTOOTH,
                  DGEN_BULGE,
                  DGEN_MOVE ) );
                  
    Q_strcat( dest, size,
              va( "#ifndef tcGen_t\n"
                  "#define tcGen_t\n"
                  "#define TCGEN_LIGHTMAP %i\n"
                  "#define TCGEN_TEXTURE %i\n"
                  "#define TCGEN_ENVIRONMENT_MAPPED %i\n"
                  "#define TCGEN_FOG %i\n"
                  "#define TCGEN_VECTOR %i\n"
                  "#endif\n",
                  TCGEN_LIGHTMAP,
                  TCGEN_TEXTURE,
                  TCGEN_ENVIRONMENT_MAPPED,
                  TCGEN_FOG,
                  TCGEN_VECTOR ) );
                  
    Q_strcat( dest, size,
              va( "#ifndef colorGen_t\n"
                  "#define colorGen_t\n"
                  "#define CGEN_LIGHTING_DIFFUSE %i\n"
                  "#endif\n",
                  CGEN_LIGHTING_DIFFUSE ) );
                  
    Q_strcat( dest, size,
              va( "#ifndef alphaGen_t\n"
                  "#define alphaGen_t\n"
                  "#define AGEN_LIGHTING_SPECULAR %i\n"
                  "#define AGEN_PORTAL %i\n"
                  "#endif\n",
                  AGEN_LIGHTING_SPECULAR,
                  AGEN_PORTAL ) );
                  
    Q_strcat( dest, size,
              va( "#ifndef texenv_t\n"
                  "#define texenv_t\n"
                  "#define TEXENV_MODULATE %i\n"
                  "#define TEXENV_ADD %i\n"
                  "#define TEXENV_REPLACE %i\n"
                  "#endif\n",
                  GL_MODULATE,
                  GL_ADD,
                  GL_REPLACE ) );
                  
    fbufWidthScale = 1.0f / ( ( F32 )glConfig.vidWidth );
    fbufHeightScale = 1.0f / ( ( F32 )glConfig.vidHeight );
    Q_strcat( dest, size,
              va( "#ifndef r_FBufScale\n#define r_FBufScale vec2(%f, %f)\n#endif\n", fbufWidthScale, fbufHeightScale ) );
              
    if( r_pbr->integer )
        Q_strcat( dest, size, "#define USE_PBR\n" );
        
    if( r_cubeMapping->integer )
    {
        Q_strcat( dest, size, va( "#define ROUGHNESS_MIPS float(%i)\n", CUBE_MAP_MIPS - 4 ) );
    }
    
    if( r_horizonFade->integer )
    {
        F32 fade = 1 + ( 0.1 * r_horizonFade->integer );
        Q_strcat( dest, size, va( "#define HORIZON_FADE float(%f)\n", fade ) );
    }
    
    
    if( extra )
    {
        Q_strcat( dest, size, extra );
    }
    
    // OK we added a lot of stuff but if we do something bad in the GLSL shaders then we want the proper line
    // so we have to reset the line counting
    Q_strcat( dest, size, va( "#line %d\n", firstLine - 1 ) );
}

static S32 GLSL_CompileGPUShader( U32 program, U32* prevShader, StringEntry buffer, S32 size, U32 shaderType )
{
    S32 compiled;
    U32 shader;
    
    shader = glCreateShader( shaderType );
    
    glShaderSource( shader, 1, ( StringEntry* )&buffer, &size );
    
    // compile shader
    glCompileShader( shader );
    
    // check if shader compiled
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
    if( !compiled )
    {
        GLSL_PrintLog( shader, GLSL_PRINTLOG_SHADER_SOURCE, false );
        GLSL_PrintLog( shader, GLSL_PRINTLOG_SHADER_INFO, false );
        Com_Error( ERR_DROP, "Couldn't compile shader" );
        return 0;
    }
    
    if( *prevShader )
    {
        glDetachShader( program, *prevShader );
        glDeleteShader( *prevShader );
    }
    
    // attach shader to program
    glAttachShader( program, shader );
    
    *prevShader = shader;
    
    return 1;
}

Block* FindBlock( StringEntry name, Block* blocks, S64 numBlocks )
{
    for( S64 i = 0; i < numBlocks; ++i )
    {
        Block* block = blocks + i;
        if( Q_stricmpn( block->blockHeaderTitle, name, block->blockHeaderTitleLength ) == 0 )
        {
            return block;
        }
    }
    
    return nullptr;
}

static const GPUProgramDesc* LoadProgramSource( StringEntry programName, Allocator& allocator, const GPUProgramDesc& fallback )
{
    const GPUProgramDesc* result = &fallback;
    UTF8* buffer;
    UTF8 programPath[MAX_QPATH];
    
    Com_sprintf( programPath, sizeof( programPath ), "renderProgs/%s.glsl", programName );
    
    S64 size = FS_ReadFile( programPath, ( void** )&buffer );
    if( size )
    {
        GPUProgramDesc* externalProgramDesc = ojkAlloc<GPUProgramDesc>( allocator );
        *externalProgramDesc = ParseProgramSource( allocator, buffer );
        result = externalProgramDesc;
        FS_FreeFile( buffer );
    }
    
    return result;
}

static S32 GLSL_LoadGPUShaderText( StringEntry name, StringEntry fallback, U32 shaderType, UTF8* dest, S32 destSize )
{
    UTF8            filename[MAX_QPATH];
    GLcharARB*      buffer = NULL;
    const GLcharARB* shaderText = NULL;
    S32             size, result;
    
    if( shaderType == GL_VERTEX_SHADER )
    {
        Com_sprintf( filename, sizeof( filename ), "renderProgs/%s.vertex", name );
    }
    else
    {
        Com_sprintf( filename, sizeof( filename ), "renderProgs/%s.fragment", name );
    }
    
    size = FS_ReadFile( filename, ( void** )&buffer );
    
    if( !buffer )
    {
        if( fallback )
        {
            CL_RefPrintf( PRINT_DEVELOPER, "...loading built-in '%s'\n", filename );
            shaderText = fallback;
            size = strlen( shaderText );
        }
        else
        {
            CL_RefPrintf( PRINT_DEVELOPER, "couldn't load '%s'\n", filename );
            return 0;
        }
    }
    else
    {
        CL_RefPrintf( PRINT_DEVELOPER, "...loading '%s'\n", filename );
        shaderText = buffer;
    }
    
    if( size > destSize )
    {
        result = 0;
    }
    else
    {
        Q_strncpyz( dest, shaderText, size + 1 );
        result = 1;
    }
    
    if( buffer )
    {
        FS_FreeFile( buffer );
    }
    
    return result;
}

static void GLSL_LinkProgram( U32 program )
{
    S32           linked;
    
    glLinkProgram( program );
    
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if( !linked )
    {
        GLSL_PrintLog( program, GLSL_PRINTLOG_PROGRAM_INFO, false );
        Com_Error( ERR_DROP, "shaders failed to link" );
    }
}

static void GLSL_ValidateProgram( U32 program )
{
    S32           validated;
    
    glValidateProgram( program );
    
    glGetProgramiv( program, GL_VALIDATE_STATUS, &validated );
    if( !validated )
    {
        GLSL_PrintLog( program, GLSL_PRINTLOG_PROGRAM_INFO, false );
        Com_Error( ERR_DROP, "shaders failed to validate" );
    }
}

static void GLSL_ShowProgramUniforms( U32 program )
{
    S32             i, count, size;
    U32			type;
    UTF8            uniformName[1000];
    
    // query the number of active uniforms
    glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &count );
    
    // Loop over each of the active uniforms, and set their value
    for( i = 0; i < count; i++ )
    {
        glGetActiveUniform( program, i, sizeof( uniformName ), NULL, &size, &type, uniformName );
        
        CL_RefPrintf( PRINT_DEVELOPER, "active uniform: '%s'\n", uniformName );
    }
}

static S32 GLSL_InitGPUShader2( shaderProgram_t* program, StringEntry name, S32 attribs, StringEntry vpCode, StringEntry fpCode )
{
    CL_RefPrintf( PRINT_DEVELOPER, "------- GPU shader -------\n" );
    
    if( strlen( name ) >= MAX_QPATH )
    {
        Com_Error( ERR_DROP, "GLSL_InitGPUShader2: \"%s\" is too long", name );
    }
    
    Q_strncpyz( program->name, name, sizeof( program->name ) );
    
    program->program = glCreateProgram();
    program->attribs = attribs;
    
    if( !( GLSL_CompileGPUShader( program->program, &program->vertexShader, vpCode, strlen( vpCode ), GL_VERTEX_SHADER ) ) )
    {
        CL_RefPrintf( PRINT_ALL, "GLSL_InitGPUShader2: Unable to load \"%s\" as GL_VERTEX_SHADER\n", name );
        glDeleteProgram( program->program );
        return 0;
    }
    
    if( fpCode )
    {
        if( !( GLSL_CompileGPUShader( program->program, &program->fragmentShader, fpCode, strlen( fpCode ), GL_FRAGMENT_SHADER ) ) )
        {
            CL_RefPrintf( PRINT_ALL, "GLSL_InitGPUShader2: Unable to load \"%s\" as GL_FRAGMENT_SHADER\n", name );
            glDeleteProgram( program->program );
            return 0;
        }
    }
    
    if( attribs & ATTR_POSITION )
        glBindAttribLocation( program->program, ATTR_INDEX_POSITION, "attr_Position" );
        
    if( attribs & ATTR_TEXCOORD )
        glBindAttribLocation( program->program, ATTR_INDEX_TEXCOORD, "attr_TexCoord0" );
        
    if( attribs & ATTR_LIGHTCOORD )
        glBindAttribLocation( program->program, ATTR_INDEX_LIGHTCOORD, "attr_TexCoord1" );
        
//  if(attribs & ATTR_TEXCOORD2)
//      glBindAttribLocation(program->program, ATTR_INDEX_TEXCOORD2, "attr_TexCoord2");

//  if(attribs & ATTR_TEXCOORD3)
//      glBindAttribLocation(program->program, ATTR_INDEX_TEXCOORD3, "attr_TexCoord3");

    if( attribs & ATTR_TANGENT )
        glBindAttribLocation( program->program, ATTR_INDEX_TANGENT, "attr_Tangent" );
        
    if( attribs & ATTR_NORMAL )
        glBindAttribLocation( program->program, ATTR_INDEX_NORMAL, "attr_Normal" );
        
    if( attribs & ATTR_COLOR )
        glBindAttribLocation( program->program, ATTR_INDEX_COLOR, "attr_Color" );
        
    if( attribs & ATTR_PAINTCOLOR )
        glBindAttribLocation( program->program, ATTR_INDEX_PAINTCOLOR, "attr_PaintColor" );
        
    if( attribs & ATTR_LIGHTDIRECTION )
        glBindAttribLocation( program->program, ATTR_INDEX_LIGHTDIRECTION, "attr_LightDirection" );
        
    if( attribs & ATTR_POSITION2 )
        glBindAttribLocation( program->program, ATTR_INDEX_POSITION2, "attr_Position2" );
        
    if( attribs & ATTR_NORMAL2 )
        glBindAttribLocation( program->program, ATTR_INDEX_NORMAL2, "attr_Normal2" );
        
    if( attribs & ATTR_TANGENT2 )
        glBindAttribLocation( program->program, ATTR_INDEX_TANGENT2, "attr_Tangent2" );
        
    GLSL_LinkProgram( program->program );
    
    return 1;
}

static S32 GLSL_InitGPUShader( shaderProgram_t* program, StringEntry name, S32 attribs, bool fragmentShader, StringEntry extra, bool addHeader, const GPUProgramDesc& programDesc )
{
    UTF8 vpCode[32000];
    UTF8 fpCode[32000];
    UTF8* postHeader;
    S32 size;
    S32 result;
    
    assert( programDesc.numShaders == 3 );
    assert( programDesc.shaders[0].type == GPUSHADER_VERTEX );
    assert( programDesc.shaders[1].type == GPUSHADER_FRAGMENT );
    assert( programDesc.shaders[1].type == GPUSHADER_GEOMETRY );
    
    size = sizeof( vpCode );
    if( addHeader )
    {
        GLSL_GetShaderHeader( GL_VERTEX_SHADER, extra, programDesc.shaders[0].firstLineNumber, vpCode, size );
        postHeader = &vpCode[strlen( vpCode )];
        size -= strlen( vpCode );
    }
    else
    {
        postHeader = &vpCode[0];
    }
    
    if( !GLSL_LoadGPUShaderText( name, programDesc.shaders[0].source, GL_VERTEX_SHADER, postHeader, size ) )
    {
        return 0;
    }
    
    if( fragmentShader )
    {
        size = sizeof( fpCode );
        if( addHeader )
        {
            GLSL_GetShaderHeader( GL_FRAGMENT_SHADER, extra, programDesc.shaders[0].firstLineNumber, fpCode, size );
            postHeader = &fpCode[strlen( fpCode )];
            size -= strlen( fpCode );
        }
        else
        {
            postHeader = &fpCode[0];
        }
        
        if( !GLSL_LoadGPUShaderText( name, programDesc.shaders[1].source, GL_FRAGMENT_SHADER, postHeader, size ) )
        {
            return 0;
        }
    }
    
    result = GLSL_InitGPUShader2( program, name, attribs, vpCode, fragmentShader ? fpCode : NULL );
    
    return result;
}

void GLSL_InitUniforms( shaderProgram_t* program )
{
    S32 i, size;
    
    S32* uniforms = program->uniforms;
    
    size = 0;
    for( i = 0; i < UNIFORM_COUNT; i++ )
    {
        uniforms[i] = glGetUniformLocation( program->program, uniformsInfo[i].name );
        
        if( uniforms[i] == -1 )
            continue;
            
        program->uniformBufferOffsets[i] = size;
        
        switch( uniformsInfo[i].type )
        {
            case GLSL_INT:
                size += sizeof( S32 );
                break;
            case GLSL_FLOAT:
                size += sizeof( F32 );
                break;
            case GLSL_FLOAT5:
                size += sizeof( vec_t ) * 5;
                break;
            case GLSL_VEC2:
                size += sizeof( vec_t ) * 2;
                break;
            case GLSL_VEC3:
                size += sizeof( vec_t ) * 3;
                break;
            case GLSL_VEC4:
                size += sizeof( vec_t ) * 4;
                break;
            case GLSL_MAT16:
                size += sizeof( vec_t ) * 16;
                break;
            default:
                break;
        }
    }
    
    program->uniformBuffer = ( UTF8* )CL_RefMalloc( size );
}

void GLSL_FinishGPUShader( shaderProgram_t* program )
{
    //GLSL_ValidateProgram(program->program);
    GLSL_ShowProgramUniforms( program->program );
    GL_CheckErrors();
}

void GLSL_SetUniformInt( shaderProgram_t* program, S32 uniformNum, S32 value )
{
    S32* uniforms = program->uniforms;
    S32* compare = ( S32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_INT )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformInt: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( value == *compare )
    {
        return;
    }
    
    *compare = value;
    
    glProgramUniform1iEXT( program->program, uniforms[uniformNum], value );
}

void GLSL_SetUniformFloat( shaderProgram_t* program, S32 uniformNum, F32 value )
{
    S32* uniforms = program->uniforms;
    F32* compare = ( F32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_FLOAT )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformFloat: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( value == *compare )
    {
        return;
    }
    
    *compare = value;
    
    glProgramUniform1fEXT( program->program, uniforms[uniformNum], value );
}

void GLSL_SetUniformVec2( shaderProgram_t* program, S32 uniformNum, const vec2_t v )
{
    S32* uniforms = program->uniforms;
    vec_t* compare = ( F32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_VEC2 )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformVec2: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( v[0] == compare[0] && v[1] == compare[1] )
    {
        return;
    }
    
    compare[0] = v[0];
    compare[1] = v[1];
    
    glProgramUniform2fEXT( program->program, uniforms[uniformNum], v[0], v[1] );
}

void GLSL_SetUniformVec3( shaderProgram_t* program, S32 uniformNum, const vec3_t v )
{
    S32* uniforms = program->uniforms;
    vec_t* compare = ( F32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_VEC3 )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformVec3: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( VectorCompare( v, compare ) )
    {
        return;
    }
    
    VectorCopy( v, compare );
    
    glProgramUniform3fEXT( program->program, uniforms[uniformNum], v[0], v[1], v[2] );
}

void GLSL_SetUniformVec4( shaderProgram_t* program, S32 uniformNum, const vec4_t v )
{
    S32* uniforms = program->uniforms;
    vec_t* compare = ( F32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_VEC4 )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformVec4: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( VectorCompare4( v, compare ) )
    {
        return;
    }
    
    VectorCopy4( v, compare );
    
    glProgramUniform4fEXT( program->program, uniforms[uniformNum], v[0], v[1], v[2], v[3] );
}

void GLSL_SetUniformFloat5( shaderProgram_t* program, S32 uniformNum, const vec5_t v )
{
    S32* uniforms = program->uniforms;
    vec_t* compare = ( F32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_FLOAT5 )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformFloat5: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( VectorCompare5( v, compare ) )
    {
        return;
    }
    
    VectorCopy5( v, compare );
    
    glProgramUniform1fvEXT( program->program, uniforms[uniformNum], 5, v );
}

void GLSL_SetUniformMat4( shaderProgram_t* program, S32 uniformNum, const mat4_t matrix )
{
    S32* uniforms = program->uniforms;
    vec_t* compare = ( F32* )( program->uniformBuffer + program->uniformBufferOffsets[uniformNum] );
    
    if( uniforms[uniformNum] == -1 )
        return;
        
    if( uniformsInfo[uniformNum].type != GLSL_MAT16 )
    {
        CL_RefPrintf( PRINT_WARNING, "GLSL_SetUniformMat4: wrong type for uniform %i in program %s\n", uniformNum, program->name );
        return;
    }
    
    if( Mat4Compare( matrix, compare ) )
    {
        return;
    }
    
    Mat4Copy( matrix, compare );
    
    glProgramUniformMatrix4fvEXT( program->program, uniforms[uniformNum], 1, GL_FALSE, matrix );
}

void GLSL_DeleteGPUShader( shaderProgram_t* program )
{
    if( program->program )
    {
        if( program->vertexShader )
        {
            glDetachShader( program->program, program->vertexShader );
            glDeleteShader( program->vertexShader );
        }
        
        if( program->fragmentShader )
        {
            glDetachShader( program->program, program->fragmentShader );
            glDeleteShader( program->fragmentShader );
        }
        
        glDeleteProgram( program->program );
        
        if( program->uniformBuffer )
        {
            Z_Free( program->uniformBuffer );
        }
        
        ::memset( program, 0, sizeof( *program ) );
    }
}

void idRenderSystemLocal::InitGPUShaders( void )
{
    S32             startTime, endTime;
    S32 i;
    UTF8 extradefines[1024];
    S32 attribs;
    S32 numGenShaders = 0, numLightShaders = 0, numEtcShaders = 0;
    
    CL_RefPrintf( PRINT_ALL, "------- idRenderSystemLocal::InitGPUShaders -------\n" );
    
    R_IssuePendingRenderCommands();
    
    startTime = CL_ScaledMilliseconds();
    
    Allocator allocator( 512 * 1024 );
    const GPUProgramDesc* programDesc;
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "generic", allocator, fallback_null );
    
    for( i = 0; i < GENERICDEF_COUNT; i++ )
    {
        attribs = ATTR_POSITION | ATTR_TEXCOORD | ATTR_LIGHTCOORD | ATTR_NORMAL | ATTR_COLOR;
        extradefines[0] = '\0';
        
        if( i & GENERICDEF_USE_DEFORM_VERTEXES )
            Q_strcat( extradefines, 1024, "#define USE_DEFORM_VERTEXES\n" );
            
        if( i & GENERICDEF_USE_TCGEN_AND_TCMOD )
        {
            Q_strcat( extradefines, 1024, "#define USE_TCGEN\n" );
            Q_strcat( extradefines, 1024, "#define USE_TCMOD\n" );
        }
        
        if( i & GENERICDEF_USE_VERTEX_ANIMATION )
        {
            Q_strcat( extradefines, 1024, "#define USE_VERTEX_ANIMATION\n" );
            attribs |= ATTR_POSITION2 | ATTR_NORMAL2;
        }
        
        if( i & GENERICDEF_USE_FOG )
            Q_strcat( extradefines, 1024, "#define USE_FOG\n" );
            
        if( i & GENERICDEF_USE_RGBAGEN )
            Q_strcat( extradefines, 1024, "#define USE_RGBAGEN\n" );
            
        if( r_parallaxMapping->integer ) // Parallax without normal maps...
            Q_strcat( extradefines, 1024, "#define USE_PARALLAXMAP_NONORMALS\n" );
            
        if( r_parallaxMapping->integer && r_parallaxMapping->integer < 2 ) // Fast parallax mapping...
            Q_strcat( extradefines, 1024, "#define FAST_PARALLAX\n" );
            
            
        if( !GLSL_InitGPUShader( &tr.genericShader[i], "generic", attribs, true, extradefines, true, *programDesc ) )
        {
            Com_Error( ERR_FATAL, "Could not load generic shader!" );
        }
        
        GLSL_InitUniforms( &tr.genericShader[i] );
        
        GLSL_SetUniformInt( &tr.genericShader[i], UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
        GLSL_SetUniformInt( &tr.genericShader[i], UNIFORM_LIGHTMAP,   TB_LIGHTMAP );
        
        GLSL_FinishGPUShader( &tr.genericShader[i] );
        
        numGenShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "texturecolor", allocator, fallback_null );
    
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    
    if( !GLSL_InitGPUShader( &tr.textureColorShader, "texturecolor", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load texturecolor shader!" );
    }
    
    GLSL_InitUniforms( &tr.textureColorShader );
    
    GLSL_SetUniformInt( &tr.textureColorShader, UNIFORM_TEXTUREMAP, TB_DIFFUSEMAP );
    
    GLSL_FinishGPUShader( &tr.textureColorShader );
    
    numEtcShaders++;
    
    for( i = 0; i < FOGDEF_COUNT; i++ )
    {
        attribs = ATTR_POSITION | ATTR_POSITION2 | ATTR_NORMAL | ATTR_NORMAL2 | ATTR_TEXCOORD;
        extradefines[0] = '\0';
        
        if( i & FOGDEF_USE_DEFORM_VERTEXES )
            Q_strcat( extradefines, 1024, "#define USE_DEFORM_VERTEXES\n" );
            
        if( i & FOGDEF_USE_VERTEX_ANIMATION )
            Q_strcat( extradefines, 1024, "#define USE_VERTEX_ANIMATION\n" );
            
        if( !GLSL_InitGPUShader( &tr.fogShader[i], "fogpass", attribs, true, extradefines, true, *programDesc ) )
        {
            Com_Error( ERR_FATAL, "Could not load fogpass shader!" );
        }
        
        GLSL_InitUniforms( &tr.fogShader[i] );
        GLSL_FinishGPUShader( &tr.fogShader[i] );
        
        numEtcShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "dlight", allocator, fallback_null );
    for( i = 0; i < DLIGHTDEF_COUNT; i++ )
    {
        attribs = ATTR_POSITION | ATTR_NORMAL | ATTR_TEXCOORD;
        extradefines[0] = '\0';
        
        if( i & DLIGHTDEF_USE_DEFORM_VERTEXES )
        {
            Q_strcat( extradefines, 1024, "#define USE_DEFORM_VERTEXES\n" );
        }
        
        if( !GLSL_InitGPUShader( &tr.dlightShader[i], "dlight", attribs, true, extradefines, false, *programDesc ) )
        {
            Com_Error( ERR_FATAL, "Could not load dlight shader!" );
        }
        
        GLSL_InitUniforms( &tr.dlightShader[i] );
        
        GLSL_SetUniformInt( &tr.dlightShader[i], UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
        
        GLSL_FinishGPUShader( &tr.dlightShader[i] );
        
        numEtcShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "lightall", allocator, fallback_null );
    for( i = 0; i < LIGHTDEF_COUNT; i++ )
    {
        S32 lightType = i & LIGHTDEF_LIGHTTYPE_MASK;
        bool fastLight = !( r_normalMapping->integer || r_specularMapping->integer );
        
        // skip impossible combos
        if( ( i & LIGHTDEF_USE_PARALLAXMAP ) && !r_parallaxMapping->integer )
            continue;
            
        if( ( i & LIGHTDEF_USE_SHADOWMAP ) && ( !lightType || !r_sunlightMode->integer ) )
            continue;
            
        attribs = ATTR_POSITION | ATTR_TEXCOORD | ATTR_COLOR | ATTR_NORMAL;
        
        extradefines[0] = '\0';
        
        if( r_dlightMode->integer >= 2 )
            Q_strcat( extradefines, 1024, "#define USE_SHADOWMAP\n" );
            
        if( glRefConfig.swizzleNormalmap )
            Q_strcat( extradefines, 1024, "#define SWIZZLE_NORMALMAP\n" );
            
        if( lightType )
        {
            Q_strcat( extradefines, 1024, "#define USE_LIGHT\n" );
            
            if( fastLight )
                Q_strcat( extradefines, 1024, "#define USE_FAST_LIGHT\n" );
                
            switch( lightType )
            {
                case LIGHTDEF_USE_LIGHTMAP:
                    Q_strcat( extradefines, 1024, "#define USE_LIGHTMAP\n" );
                    if( r_deluxeMapping->integer && !fastLight )
                    {
                        Q_strcat( extradefines, 1024, "#define USE_DELUXEMAP\n" );
                    }
                    attribs |= ATTR_LIGHTCOORD | ATTR_LIGHTDIRECTION;
                    break;
                case LIGHTDEF_USE_LIGHT_VECTOR:
                    Q_strcat( extradefines, 1024, "#define USE_LIGHT_VECTOR\n" );
                    break;
                case LIGHTDEF_USE_LIGHT_VERTEX:
                    Q_strcat( extradefines, 1024, "#define USE_LIGHT_VERTEX\n" );
                    attribs |= ATTR_LIGHTDIRECTION;
                    break;
                default:
                    break;
            }
            
            if( r_normalMapping->integer )
            {
                Q_strcat( extradefines, 1024, "#define USE_NORMALMAP\n" );
                
                attribs |= ATTR_TANGENT;
                
                if( ( i & LIGHTDEF_USE_PARALLAXMAP ) && r_parallaxMapping->integer )
                {
                    Q_strcat( extradefines, 1024, "#define USE_PARALLAXMAP\n" );
                }
                else if( r_parallaxMapping->integer ) // Parallax without normal maps...
                {
                    Q_strcat( extradefines, 1024, "#define USE_PARALLAXMAP_NONORMALS\n" );
                }
                
                if( r_parallaxMapping->integer )
                {
                    Q_strcat( extradefines, 1024, "#define USE_RELIEFMAP\n" );
                }
                
                if( r_parallaxMapping->integer && r_parallaxMapping->integer < 2 ) // Fast parallax mapping...
                {
                    Q_strcat( extradefines, 1024, "#define FAST_PARALLAX\n" );
                }
            }
            else if( r_parallaxMapping->integer ) // Parallax without normal maps...
            {
                Q_strcat( extradefines, 1024, "#define USE_PARALLAXMAP_NONORMALS\n" );
                
                if( r_parallaxMapping->integer && r_parallaxMapping->integer < 2 ) // Fast parallax mapping...
                {
                    Q_strcat( extradefines, 1024, "#define FAST_PARALLAX\n" );
                }
            }
            
            if( r_specularMapping->integer )
            {
                Q_strcat( extradefines, 1024, "#define USE_SPECULARMAP\n" );
            }
            else if( r_deluxeSpecular->value > 0.000001f )
            {
                Q_strcat( extradefines, 1024, va( "#define r_deluxeSpecular %f\n", r_deluxeSpecular->value ) );
            }
            
            if( r_cubeMapping->integer )
            {
                Q_strcat( extradefines, 1024, "#define USE_CUBEMAP\n" );
            }
            
            switch( r_glossType->integer )
            {
                case 0:
                default:
                    Q_strcat( extradefines, 1024, "#define GLOSS_IS_GLOSS\n" );
                    break;
                case 1:
                    Q_strcat( extradefines, 1024, "#define GLOSS_IS_SMOOTHNESS\n" );
                    break;
                case 2:
                    Q_strcat( extradefines, 1024, "#define GLOSS_IS_ROUGHNESS\n" );
                    break;
                case 3:
                    Q_strcat( extradefines, 1024, "#define GLOSS_IS_SHININESS\n" );
                    break;
            }
        }
        
        if( i & LIGHTDEF_USE_SHADOWMAP )
        {
            Q_strcat( extradefines, 1024, "#define USE_SHADOWMAP\n" );
            
            if( r_sunlightMode->integer == 1 )
                Q_strcat( extradefines, 1024, "#define SHADOWMAP_MODULATE\n" );
            else if( r_sunlightMode->integer == 2 )
                Q_strcat( extradefines, 1024, "#define USE_PRIMARY_LIGHT\n" );
        }
        
        if( i & LIGHTDEF_USE_TCGEN_AND_TCMOD )
        {
            Q_strcat( extradefines, 1024, "#define USE_TCGEN\n" );
            Q_strcat( extradefines, 1024, "#define USE_TCMOD\n" );
        }
        
        if( i & LIGHTDEF_ENTITY )
        {
            Q_strcat( extradefines, 1024, "#define USE_VERTEX_ANIMATION\n#define USE_MODELMATRIX\n" );
            attribs |= ATTR_POSITION2 | ATTR_NORMAL2;
            
            if( r_normalMapping->integer )
            {
                attribs |= ATTR_TANGENT2;
            }
        }
        
        if( !GLSL_InitGPUShader( &tr.lightallShader[i], "lightall", attribs, true, extradefines, true, *programDesc ) )
        {
            Com_Error( ERR_FATAL, "Could not load lightall shader!" );
        }
        
        GLSL_InitUniforms( &tr.lightallShader[i] );
        
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_DIFFUSEMAP,  TB_DIFFUSEMAP );
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_LIGHTMAP,    TB_LIGHTMAP );
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_NORMALMAP,   TB_NORMALMAP );
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_DELUXEMAP,   TB_DELUXEMAP );
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_SPECULARMAP, TB_SPECULARMAP );
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_SHADOWMAP,   TB_SHADOWMAP );
        GLSL_SetUniformInt( &tr.lightallShader[i], UNIFORM_CUBEMAP,     TB_CUBEMAP );
        
        GLSL_FinishGPUShader( &tr.lightallShader[i] );
        
        numLightShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "shadowfill", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_POSITION2 | ATTR_NORMAL | ATTR_NORMAL2 | ATTR_TEXCOORD;
    
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.shadowmapShader, "shadowfill", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load shadowfill shader!" );
    }
    
    GLSL_InitUniforms( &tr.shadowmapShader );
    GLSL_FinishGPUShader( &tr.shadowmapShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "pshadow", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_NORMAL;
    extradefines[0] = '\0';
    
    Q_strcat( extradefines, 1024, "#define USE_PCF\n#define USE_DISCARD\n" );
    
    if( !GLSL_InitGPUShader( &tr.pshadowShader, "pshadow", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load pshadow shader!" );
    }
    
    GLSL_InitUniforms( &tr.pshadowShader );
    
    GLSL_SetUniformInt( &tr.pshadowShader, UNIFORM_SHADOWMAP, TB_DIFFUSEMAP );
    
    GLSL_FinishGPUShader( &tr.pshadowShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "down4x", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.down4xShader, "down4x", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load down4x shader!" );
    }
    
    GLSL_InitUniforms( &tr.down4xShader );
    
    GLSL_SetUniformInt( &tr.down4xShader, UNIFORM_TEXTUREMAP, TB_DIFFUSEMAP );
    
    GLSL_FinishGPUShader( &tr.down4xShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "bokeh", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.bokehShader, "bokeh", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load bokeh shader!" );
    }
    
    GLSL_InitUniforms( &tr.bokehShader );
    
    GLSL_SetUniformInt( &tr.bokehShader, UNIFORM_TEXTUREMAP, TB_DIFFUSEMAP );
    
    GLSL_FinishGPUShader( &tr.bokehShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "tonemap", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.tonemapShader, "tonemap", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load tonemap shader!" );
    }
    
    GLSL_InitUniforms( &tr.tonemapShader );
    
    GLSL_SetUniformInt( &tr.tonemapShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.tonemapShader, UNIFORM_LEVELSMAP,  TB_LEVELSMAP );
    
    GLSL_FinishGPUShader( &tr.tonemapShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "calclevels4x", allocator, fallback_null );
    for( i = 0; i < 2; i++ )
    {
        attribs = ATTR_POSITION | ATTR_TEXCOORD;
        extradefines[0] = '\0';
        
        if( !i )
            Q_strcat( extradefines, 1024, "#define FIRST_PASS\n" );
            
        if( !GLSL_InitGPUShader( &tr.calclevels4xShader[i], "calclevels4x", attribs, true, extradefines, true, *programDesc ) )
        {
            Com_Error( ERR_FATAL, "Could not load calclevels4x shader!" );
        }
        
        GLSL_InitUniforms( &tr.calclevels4xShader[i] );
        
        GLSL_SetUniformInt( &tr.calclevels4xShader[i], UNIFORM_TEXTUREMAP, TB_DIFFUSEMAP );
        
        GLSL_FinishGPUShader( &tr.calclevels4xShader[i] );
        
        numEtcShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "shadowmask", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( r_shadowFilter->integer >= 1 )
        Q_strcat( extradefines, 1024, "#define USE_SHADOW_FILTER\n" );
        
    if( r_shadowFilter->integer >= 2 )
        Q_strcat( extradefines, 1024, "#define USE_SHADOW_FILTER2\n" );
        
    if( r_shadowCascadeZFar->integer != 0 )
        Q_strcat( extradefines, 1024, "#define USE_SHADOW_CASCADE\n" );
        
    Q_strcat( extradefines, 1024, va( "#define r_shadowMapSize %f\n", r_shadowMapSize->value ) );
    Q_strcat( extradefines, 1024, va( "#define r_shadowCascadeZFar %f\n", r_shadowCascadeZFar->value ) );
    
    
    if( !GLSL_InitGPUShader( &tr.shadowmaskShader, "shadowmask", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load shadowmask shader!" );
    }
    
    GLSL_InitUniforms( &tr.shadowmaskShader );
    
    GLSL_SetUniformInt( &tr.shadowmaskShader, UNIFORM_SCREENDEPTHMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.shadowmaskShader, UNIFORM_SHADOWMAP,  TB_SHADOWMAP );
    GLSL_SetUniformInt( &tr.shadowmaskShader, UNIFORM_SHADOWMAP2, TB_SHADOWMAP2 );
    GLSL_SetUniformInt( &tr.shadowmaskShader, UNIFORM_SHADOWMAP3, TB_SHADOWMAP3 );
    GLSL_SetUniformInt( &tr.shadowmaskShader, UNIFORM_SHADOWMAP4, TB_SHADOWMAP4 );
    
    GLSL_FinishGPUShader( &tr.shadowmaskShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "ssao", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.ssaoShader, "ssao", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load ssao shader!" );
    }
    
    GLSL_InitUniforms( &tr.ssaoShader );
    
    GLSL_SetUniformInt( &tr.ssaoShader, UNIFORM_SCREENDEPTHMAP, TB_COLORMAP );
    
    GLSL_FinishGPUShader( &tr.ssaoShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "depthblur", allocator, fallback_null );
    for( i = 0; i < 4; i++ )
    {
        attribs = ATTR_POSITION | ATTR_TEXCOORD;
        extradefines[0] = '\0';
        
        if( i & 1 )
            Q_strcat( extradefines, 1024, "#define USE_VERTICAL_BLUR\n" );
        else
            Q_strcat( extradefines, 1024, "#define USE_HORIZONTAL_BLUR\n" );
            
        if( !( i & 2 ) )
            Q_strcat( extradefines, 1024, "#define USE_DEPTH\n" );
            
        if( !GLSL_InitGPUShader( &tr.depthBlurShader[i], "depthblur", attribs, true, extradefines, true, *programDesc ) )
        {
            Com_Error( ERR_FATAL, "Could not load depthblur shader!" );
        }
        
        GLSL_InitUniforms( &tr.depthBlurShader[i] );
        
        GLSL_SetUniformInt( &tr.depthBlurShader[i], UNIFORM_SCREENIMAGEMAP, TB_COLORMAP );
        GLSL_SetUniformInt( &tr.depthBlurShader[i], UNIFORM_SCREENDEPTHMAP, TB_LIGHTMAP );
        
        GLSL_FinishGPUShader( &tr.depthBlurShader[i] );
        
        numEtcShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "darkexpand", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.darkexpandShader, "darkexpand", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load darkexpand shader!" );
    }
    
    GLSL_InitUniforms( &tr.darkexpandShader );
    GLSL_SetUniformInt( &tr.darkexpandShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.darkexpandShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.darkexpandShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.darkexpandShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.darkexpandShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "gaussian_blur", allocator, fallback_null );
    attribs = 0;
    extradefines[0] = '\0';
    Q_strcat( extradefines, sizeof( extradefines ), "#define BLUR_X" );
    
    if( !GLSL_InitGPUShader( &tr.gaussianBlurShader[0], "gaussian_blur", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load gaussian_blur (X-direction) shader!" );
    }
    
    attribs = 0;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.gaussianBlurShader[1], "gaussian_blur", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load gaussian_blur (Y-direction) shader!" );
    }
    
    for( i = 0; i < 2; i++ )
    {
        GLSL_InitUniforms( &tr.gaussianBlurShader[i] );
        GLSL_FinishGPUShader( &tr.gaussianBlurShader[i] );
        
        numEtcShaders++;
    }
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "multipost", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.multipostShader, "multipost", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load darkexpand shader!" );
    }
    
    GLSL_InitUniforms( &tr.multipostShader );
    GLSL_SetUniformInt( &tr.multipostShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.multipostShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "texturedetail", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.texturedetailShader, "texturedetail", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load texturedetail shader!" );
    }
    
    GLSL_InitUniforms( &tr.texturedetailShader );
    GLSL_SetUniformInt( &tr.texturedetailShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.texturedetailShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.texturedetailShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.texturedetailShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.texturedetailShader );
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "rbm", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.rbmShader, "rbm", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load rbm shader!" );
    }
    
    GLSL_InitUniforms( &tr.rbmShader );
    GLSL_SetUniformInt( &tr.rbmShader, UNIFORM_SCREENDEPTHMAP, TB_LIGHTMAP );
    GLSL_SetUniformInt( &tr.rbmShader, UNIFORM_NORMALMAP, TB_NORMALMAP );
    GLSL_SetUniformInt( &tr.rbmShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.rbmShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "contrast", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.contrastShader, "contrast", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load bloom shader!" );
    }
    
    GLSL_InitUniforms( &tr.contrastShader );
    GLSL_SetUniformInt( &tr.contrastShader, UNIFORM_TEXTUREMAP, TB_DIFFUSEMAP );
    GLSL_SetUniformFloat( &tr.contrastShader, UNIFORM_BRIGHTNESS, r_brightness->value );
    GLSL_SetUniformFloat( &tr.contrastShader, UNIFORM_CONTRAST, r_contrast->value );
    GLSL_SetUniformFloat( &tr.contrastShader, UNIFORM_GAMMA, r_gamma->value );
    GLSL_FinishGPUShader( &tr.contrastShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "volumelight", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.volumelightShader, "volumelight", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load volumelight shader!" );
    }
    
    GLSL_InitUniforms( &tr.volumelightShader );
    GLSL_SetUniformInt( &tr.volumelightShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.volumelightShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "lensflare", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.lensflareShader, "lensflare", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load lensflare shader!" );
    }
    
    GLSL_InitUniforms( &tr.lensflareShader );
    GLSL_SetUniformInt( &tr.lensflareShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.lensflareShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "anamorphic_darken", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.anamorphicDarkenShader, "anamorphic_darken", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load anamorphic_darken shader!" );
    }
    
    GLSL_InitUniforms( &tr.anamorphicDarkenShader );
    GLSL_SetUniformInt( &tr.anamorphicDarkenShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.anamorphicDarkenShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "anamorphic_blur", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.anamorphicBlurShader, "anamorphic_blur", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load anamorphic_blur shader!" );
    }
    
    GLSL_InitUniforms( &tr.anamorphicBlurShader );
    GLSL_SetUniformInt( &tr.anamorphicBlurShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.anamorphicBlurShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "anamorphic_combine", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.anamorphicCombineShader, "anamorphic_combine", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load anamorphic_combine shader!" );
    }
    
    GLSL_InitUniforms( &tr.anamorphicCombineShader );
    GLSL_SetUniformInt( &tr.anamorphicCombineShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_SetUniformInt( &tr.anamorphicCombineShader, UNIFORM_NORMALMAP, TB_NORMALMAP );
    GLSL_FinishGPUShader( &tr.anamorphicCombineShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "truehdr", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.hdrShader, "truehdr", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load hdr shader!" );
    }
    
    GLSL_InitUniforms( &tr.hdrShader );
    GLSL_SetUniformInt( &tr.hdrShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.hdrShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.hdrShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.hdrShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.hdrShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "depthOfField", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.dofShader, "depthOfField", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load depthOfField shader!" );
    }
    
    GLSL_InitUniforms( &tr.dofShader );
    GLSL_SetUniformInt( &tr.dofShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.dofShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.dofShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.dofShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.dofShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "esharpening", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.esharpeningShader, "esharpening", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load esharpening shader!" );
    }
    
    GLSL_InitUniforms( &tr.esharpeningShader );
    GLSL_SetUniformInt( &tr.esharpeningShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.esharpeningShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.esharpeningShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.esharpeningShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.esharpeningShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "esharpening2", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.esharpening2Shader, "esharpening2", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load esharpening2 shader!" );
    }
    
    GLSL_InitUniforms( &tr.esharpening2Shader );
    GLSL_SetUniformInt( &tr.esharpening2Shader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.esharpening2Shader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.esharpening2Shader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.esharpening2Shader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.esharpening2Shader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "textureclean", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.texturecleanShader, "textureclean", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load textureclean shader!" );
    }
    
    GLSL_InitUniforms( &tr.texturecleanShader );
    GLSL_SetUniformInt( &tr.texturecleanShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.texturecleanShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.texturecleanShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.texturecleanShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.texturecleanShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "vibrancy", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.vibrancyShader, "vibrancy", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load vibrancy shader!" );
    }
    
    GLSL_InitUniforms( &tr.vibrancyShader );
    GLSL_SetUniformInt( &tr.vibrancyShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.vibrancyShader, UNIFORM_DIMENSIONS, screensize );
    }
    
    GLSL_FinishGPUShader( &tr.vibrancyShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "anaglyph", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.anaglyphShader, "anaglyph", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load anaglyph shader!" );
    }
    
    GLSL_InitUniforms( &tr.anaglyphShader );
    GLSL_SetUniformInt( &tr.anaglyphShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.anaglyphShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.anaglyphShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.anaglyphShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    {
        vec4_t local0;
        VectorSet4( local0, r_trueAnaglyphSeparation->value, r_trueAnaglyphRed->value, r_trueAnaglyphGreen->value, r_trueAnaglyphBlue->value );
        GLSL_SetUniformVec4( &tr.anaglyphShader, UNIFORM_LOCAL0, local0 );
    }
    
    GLSL_FinishGPUShader( &tr.anaglyphShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "uniquewater", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD | ATTR_NORMAL | ATTR_COLOR;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.waterShader, "uniquewater", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load water shader!" );
    }
    
    GLSL_InitUniforms( &tr.waterShader );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_LIGHTMAP, TB_LIGHTMAP );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_NORMALMAP, TB_NORMALMAP );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_DELUXEMAP, TB_DELUXEMAP );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_SPECULARMAP, TB_SPECULARMAP );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_SHADOWMAP, TB_SHADOWMAP );
    GLSL_SetUniformInt( &tr.waterShader, UNIFORM_CUBEMAP, TB_CUBEMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.waterShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.waterShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.waterShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "fxaa", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    if( !GLSL_InitGPUShader( &tr.fxaaShader, "fxaa", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load fxaa shader!" );
    }
    
    GLSL_InitUniforms( &tr.fxaaShader );
    
    GLSL_SetUniformInt( &tr.fxaaShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    GLSL_SetUniformInt( &tr.fxaaShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec4_t viewInfo;
        
        float zmax = backEnd.viewParms.zFar;
        float zmin = r_znear->value;
        
        VectorSet4( viewInfo, zmax / zmin, zmax, 0.0, 0.0 );
        //VectorSet4(viewInfo, zmin, zmax, 0.0, 0.0);
        
        GLSL_SetUniformVec4( &tr.fxaaShader, UNIFORM_VIEWINFO, viewInfo );
    }
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.fxaaShader, UNIFORM_DIMENSIONS, screensize );
        
        //CL_RefPrintf(PRINT_WARNING, "Sent dimensions %f %f.\n", screensize[0], screensize[1]);
    }
    
    GLSL_FinishGPUShader( &tr.fxaaShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "ssgi", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.ssgiShader, "ssgi", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load ssgi shader!" );
    }
    
    GLSL_InitUniforms( &tr.ssgiShader );
    GLSL_SetUniformInt( &tr.ssgiShader, UNIFORM_LEVELSMAP, TB_LEVELSMAP );
    
    {
        vec2_t screensize;
        screensize[0] = glConfig.vidWidth;
        screensize[1] = glConfig.vidHeight;
        
        GLSL_SetUniformVec2( &tr.ssgiShader, UNIFORM_DIMENSIONS, screensize );
    }
    
    GLSL_FinishGPUShader( &tr.ssgiShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "ssgiBlur", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.ssgiBlurShader, "ssgiBlur", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load ssgi_blur shader!" );
    }
    
    GLSL_InitUniforms( &tr.ssgiBlurShader );
    GLSL_SetUniformInt( &tr.ssgiBlurShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.ssgiBlurShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "bloom_darken", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.bloomDarkenShader, "bloom_darken", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load bloom_darken shader!" );
    }
    
    GLSL_InitUniforms( &tr.bloomDarkenShader );
    GLSL_SetUniformInt( &tr.bloomDarkenShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.bloomDarkenShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "bloom_blur", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.bloomBlurShader, "bloom_blur", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load bloom_blur shader!" );
    }
    
    GLSL_InitUniforms( &tr.bloomBlurShader );
    GLSL_SetUniformInt( &tr.bloomBlurShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_FinishGPUShader( &tr.bloomBlurShader );
    
    numEtcShaders++;
    allocator.Reset();
    
    /////////////////////////////////////////////////////////////////////////////
    programDesc = LoadProgramSource( "bloom_combine", allocator, fallback_null );
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.bloomCombineShader, "bloom_combine", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load bloom_combine shader!" );
    }
    
    GLSL_InitUniforms( &tr.bloomCombineShader );
    GLSL_SetUniformInt( &tr.bloomCombineShader, UNIFORM_DIFFUSEMAP, TB_DIFFUSEMAP );
    GLSL_SetUniformInt( &tr.bloomCombineShader, UNIFORM_NORMALMAP, TB_NORMALMAP );
    
    GLSL_FinishGPUShader( &tr.bloomCombineShader );
    
    numEtcShaders++;
    allocator.Reset();
    
#if 0
    attribs = ATTR_POSITION | ATTR_TEXCOORD;
    extradefines[0] = '\0';
    
    if( !GLSL_InitGPUShader( &tr.testcubeShader, "testcube", attribs, true, extradefines, true, *programDesc ) )
    {
        Com_Error( ERR_FATAL, "Could not load testcube shader!" );
    }
    
    GLSL_InitUniforms( &tr.testcubeShader );
    
    GLSL_SetUniformInt( &tr.testcubeShader, UNIFORM_TEXTUREMAP, TB_COLORMAP );
    
    GLSL_FinishGPUShader( &tr.testcubeShader );
    
    numEtcShaders++;
#endif
    
    endTime = CL_ScaledMilliseconds();
    
    CL_RefPrintf( PRINT_ALL, "loaded %i GLSL shaders (%i gen %i light %i etc) in %5.2f seconds\n",
                  numGenShaders + numLightShaders + numEtcShaders, numGenShaders, numLightShaders,
                  numEtcShaders, ( endTime - startTime ) / 1000.0 );
}

void idRenderSystemLocal::ShutdownGPUShaders( void )
{
    S32 i;
    
    CL_RefPrintf( PRINT_ALL, "------- idRenderSystemLocal::ShutdownGPUShaders -------\n" );
    
    for( i = 0; i < ATTR_INDEX_COUNT; i++ )
        glDisableVertexAttribArray( i );
        
    GL_BindNullProgram();
    
    for( i = 0; i < GENERICDEF_COUNT; i++ )
        GLSL_DeleteGPUShader( &tr.genericShader[i] );
        
    GLSL_DeleteGPUShader( &tr.textureColorShader );
    
    for( i = 0; i < FOGDEF_COUNT; i++ )
        GLSL_DeleteGPUShader( &tr.fogShader[i] );
        
    for( i = 0; i < DLIGHTDEF_COUNT; i++ )
        GLSL_DeleteGPUShader( &tr.dlightShader[i] );
        
    for( i = 0; i < LIGHTDEF_COUNT; i++ )
        GLSL_DeleteGPUShader( &tr.lightallShader[i] );
        
    GLSL_DeleteGPUShader( &tr.shadowmapShader );
    GLSL_DeleteGPUShader( &tr.pshadowShader );
    GLSL_DeleteGPUShader( &tr.down4xShader );
    GLSL_DeleteGPUShader( &tr.bokehShader );
    GLSL_DeleteGPUShader( &tr.tonemapShader );
    
    for( i = 0; i < 2; i++ )
        GLSL_DeleteGPUShader( &tr.calclevels4xShader[i] );
        
    GLSL_DeleteGPUShader( &tr.shadowmaskShader );
    GLSL_DeleteGPUShader( &tr.ssaoShader );
    
    for( i = 0; i < 4; i++ )
        GLSL_DeleteGPUShader( &tr.depthBlurShader[i] );
        
    GLSL_DeleteGPUShader( &tr.darkexpandShader );
    GLSL_DeleteGPUShader( &tr.multipostShader );
    GLSL_DeleteGPUShader( &tr.volumelightShader );
    GLSL_DeleteGPUShader( &tr.lensflareShader );
    GLSL_DeleteGPUShader( &tr.anamorphicDarkenShader );
    GLSL_DeleteGPUShader( &tr.anamorphicBlurShader );
    GLSL_DeleteGPUShader( &tr.anamorphicCombineShader );
    GLSL_DeleteGPUShader( &tr.hdrShader );
    GLSL_DeleteGPUShader( &tr.dofShader );
    GLSL_DeleteGPUShader( &tr.esharpeningShader );
    GLSL_DeleteGPUShader( &tr.esharpening2Shader );
    GLSL_DeleteGPUShader( &tr.texturecleanShader );
    GLSL_DeleteGPUShader( &tr.anaglyphShader );
    GLSL_DeleteGPUShader( &tr.waterShader );
    GLSL_DeleteGPUShader( &tr.fxaaShader );
    GLSL_DeleteGPUShader( &tr.bloomDarkenShader );
    GLSL_DeleteGPUShader( &tr.bloomBlurShader );
    GLSL_DeleteGPUShader( &tr.bloomCombineShader );
    GLSL_DeleteGPUShader( &tr.ssgiShader );
    GLSL_DeleteGPUShader( &tr.ssgiBlurShader );
    GLSL_DeleteGPUShader( &tr.texturedetailShader );
    GLSL_DeleteGPUShader( &tr.contrastShader );
    GLSL_DeleteGPUShader( &tr.rbmShader );
    
    for( i = 0; i < 2; i++ )
        GLSL_DeleteGPUShader( &tr.gaussianBlurShader[i] );
        
}


void GLSL_BindProgram( shaderProgram_t* program )
{
    U32 programObject = program ? program->program : 0;
    StringEntry name = program ? program->name : "NULL";
    
    if( r_logFile->integer )
    {
        // don't just call LogComment, or we will get a call to va() every frame!
        GLimp_LogComment( ( UTF8* )va( "--- GLSL_BindProgram( %s ) ---\n", name ) );
    }
    
    if( GL_UseProgram( programObject ) )
        backEnd.pc.c_glslShaderBinds++;
}


shaderProgram_t* GLSL_GetGenericShaderProgram( S32 stage )
{
    shaderStage_t* pStage = tess.xstages[stage];
    S32 shaderAttribs = 0;
    
    if( tess.fogNum && pStage->adjustColorsForFog )
    {
        shaderAttribs |= GENERICDEF_USE_FOG;
    }
    
    switch( pStage->rgbGen )
    {
        case CGEN_LIGHTING_DIFFUSE:
            shaderAttribs |= GENERICDEF_USE_RGBAGEN;
            break;
        default:
            break;
    }
    
    switch( pStage->alphaGen )
    {
        case AGEN_LIGHTING_SPECULAR:
        case AGEN_PORTAL:
            shaderAttribs |= GENERICDEF_USE_RGBAGEN;
            break;
        default:
            break;
    }
    
    if( pStage->bundle[0].tcGen != TCGEN_TEXTURE )
    {
        shaderAttribs |= GENERICDEF_USE_TCGEN_AND_TCMOD;
    }
    
    if( tess.shader->numDeforms && !ShaderRequiresCPUDeforms( tess.shader ) )
    {
        shaderAttribs |= GENERICDEF_USE_DEFORM_VERTEXES;
    }
    
    if( glState.vertexAnimation )
    {
        shaderAttribs |= GENERICDEF_USE_VERTEX_ANIMATION;
    }
    
    if( pStage->bundle[0].numTexMods )
    {
        shaderAttribs |= GENERICDEF_USE_TCGEN_AND_TCMOD;
    }
    
    return &tr.genericShader[shaderAttribs];
}
