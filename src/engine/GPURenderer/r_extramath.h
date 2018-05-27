////////////////////////////////////////////////////////////////////////////////////////
// Copyright(C) 2016 James Canete
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
// File name:   r_extrememath.h
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description:
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#ifndef __R_EXTRAMATH_H__
#define __R_EXTRAMATH_H__

typedef vec_t mat4_t[16];
typedef S32 ivec2_t[2];
typedef S32 ivec3_t[3];
typedef S32 ivec4_t[4];

void Mat4Zero( mat4_t out );
void Mat4Identity( mat4_t out );
void Mat4Copy( const mat4_t in, mat4_t out );
void Mat4Multiply( const mat4_t in1, const mat4_t in2, mat4_t out );
void Mat4Transform( const mat4_t in1, const vec4_t in2, vec4_t out );
bool Mat4Compare( const mat4_t a, const mat4_t b );
void Mat4Dump( const mat4_t in );
void Mat4Translation( vec3_t vec, mat4_t out );
void Mat4Ortho( F32 left, F32 right, F32 bottom, F32 top, F32 znear, F32 zfar, mat4_t out );
void Mat4View( vec3_t axes[3], vec3_t origin, mat4_t out );
void Mat4SimpleInverse( const mat4_t in, mat4_t out );

#define VectorCopy2(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1])
#define VectorSet2(v,x,y)       ((v)[0]=(x),(v)[1]=(y));

#define VectorCopy4(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])
#define VectorSet4(v,x,y,z,w)	((v)[0]=(x),(v)[1]=(y),(v)[2]=(z),(v)[3]=(w))
#define DotProduct4(a,b)        ((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3])
#define VectorScale4(a,b,c)     ((c)[0]=(a)[0]*(b),(c)[1]=(a)[1]*(b),(c)[2]=(a)[2]*(b),(c)[3]=(a)[3]*(b))

#define VectorCopy5(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3],(b)[4]=(a)[4])

#define OffsetByteToFloat(a)    ((F32)(a) * 1.0f/127.5f - 1.0f)
#define FloatToOffsetByte(a)    (U8)((a) * 127.5f + 128.0f)
#define ByteToFloat(a)          ((F32)(a) * 1.0f/255.0f)
#define FloatToByte(a)          (U8)((a) * 255.0f)

static ID_INLINE S32 VectorCompare4( const vec4_t v1, const vec4_t v2 )
{
    if( v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2] || v1[3] != v2[3] )
    {
        return 0;
    }
    return 1;
}

static ID_INLINE S32 VectorCompare5( const vec5_t v1, const vec5_t v2 )
{
    if( v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2] || v1[3] != v2[3] || v1[4] != v2[4] )
    {
        return 0;
    }
    return 1;
}

void VectorLerp( vec3_t a, vec3_t b, F32 lerp, vec3_t c );


bool SpheresIntersect( vec3_t origin1, F32 radius1, vec3_t origin2, F32 radius2 );
void BoundingSphereOfSpheres( vec3_t origin1, F32 radius1, vec3_t origin2, F32 radius2, vec3_t origin3, F32* radius3 );

#ifndef SGN
#define SGN(x) (((x) >= 0) ? !!(x) : -1)
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(a,b,c) MIN(MAX((a),(b)),(c))
#endif

S32 NextPowerOfTwo( S32 in );
U16 FloatToHalf( F32 in );
F32 HalfToFloat( U16 in );

#endif //!__R_EXTRAMATH_H__
