//C++

/*
----------------------------------------------------
The Desktop Project
------------------

Copyright 2004 Daher Alfawares

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------------------------------
*/

#ifndef __DOPENGL_H
#define __DOPENGL_H

namespace gl
{
	typedef unsigned int     Enum;
	typedef unsigned char    Boolean;
	typedef unsigned int     Bitfield;
	typedef signed char      Byte;
	typedef short            Short;
	typedef int              Int;
	typedef int              Sizei;
	typedef unsigned char    UByte;
	typedef unsigned short   UShort;
	typedef unsigned int     UInt;
	typedef float            Float;
	typedef float            ClampF;
	typedef double           Double;
	typedef double           ClampD;
	typedef void             Void;

	/*************************************************************/

	/* Version */
	#define VERSION_1_1							1

	/* AccumOp */
	static const ACCUM                          = 0x0100;
	static const LOAD                           = 0x0101;
	static const RETURN                         = 0x0102;
	static const MULT                           = 0x0103;
	static const ADD                            = 0x0104;

	/* AlphaFunction */
	static const NEVER                          = 0x0200;
	static const LESS                           = 0x0201;
	static const EQUAL                          = 0x0202;
	static const LEQUAL                         = 0x0203;
	static const GREATER                        = 0x0204;
	static const NOTEQUAL                       = 0x0205;
	static const GEQUAL                         = 0x0206;
	static const ALWAYS                         = 0x0207;

	/* AttribMask */
	static const CURRENT_BIT                    = 0x00000001;
	static const POINT_BIT                      = 0x00000002;
	static const LINE_BIT                       = 0x00000004;
	static const POLYGON_BIT                    = 0x00000008;
	static const POLYGON_STIPPLE_BIT            = 0x00000010;
	static const PIXEL_MODE_BIT                 = 0x00000020;
	static const LIGHTING_BIT                   = 0x00000040;
	static const FOG_BIT                        = 0x00000080;
	static const DEPTH_BUFFER_BIT               = 0x00000100;
	static const ACCUM_BUFFER_BIT               = 0x00000200;
	static const STENCIL_BUFFER_BIT             = 0x00000400;
	static const VIEWPORT_BIT                   = 0x00000800;
	static const TRANSFORM_BIT                  = 0x00001000;
	static const ENABLE_BIT                     = 0x00002000;
	static const COLOR_BUFFER_BIT               = 0x00004000;
	static const HINT_BIT                       = 0x00008000;
	static const EVAL_BIT                       = 0x00010000;
	static const LIST_BIT                       = 0x00020000;
	static const TEXTURE_BIT                    = 0x00040000;
	static const SCISSOR_BIT                    = 0x00080000;
	static const ALL_ATTRIB_BITS                = 0x000fffff;

	/* BeginMode */
	static const POINTS                         = 0x0000;
	static const LINES                          = 0x0001;
	static const LINE_LOOP                      = 0x0002;
	static const LINE_STRIP                     = 0x0003;
	static const TRIANGLES                      = 0x0004;
	static const TRIANGLE_STRIP                 = 0x0005;
	static const TRIANGLE_FAN                   = 0x0006;
	static const QUADS                          = 0x0007;
	static const QUAD_STRIP                     = 0x0008;
	static const POLYGON                        = 0x0009;

	/* BlendingFactorDest */
	static const ZERO                           = 0;
	static const ONE                            = 1;
	static const SRC_COLOR                      = 0x0300;
	static const ONE_MINUS_SRC_COLOR            = 0x0301;
	static const SRC_ALPHA                      = 0x0302;
	static const ONE_MINUS_SRC_ALPHA            = 0x0303;
	static const DST_ALPHA                      = 0x0304;
	static const ONE_MINUS_DST_ALPHA            = 0x0305;

	/* BlendingFactorSrc */
	/*      ZERO */
	/*      ONE */
	static const DST_COLOR                      = 0x0306;
	static const ONE_MINUS_DST_COLOR            = 0x0307;
	static const SRC_ALPHA_SATURATE             = 0x0308;
	/*      SRC_ALPHA */
	/*      ONE_MINUS_SRC_ALPHA */
	/*      DST_ALPHA */
	/*      ONE_MINUS_DST_ALPHA */

	/* Boolean */
	static const TRUE                           = 1;
	static const FALSE                          = 0;

	/* ClearBufferMask */
	/*      COLOR_BUFFER_BIT */
	/*      ACCUM_BUFFER_BIT */
	/*      STENCIL_BUFFER_BIT */
	/*      DEPTH_BUFFER_BIT */

	/* ClientArrayType */
	/*      VERTEX_ARRAY */
	/*      NORMAL_ARRAY */
	/*      COLOR_ARRAY */
	/*      INDEX_ARRAY */
	/*      TEXTURE_COORD_ARRAY */
	/*      EDGE_FLAG_ARRAY */

	/* ClipPlaneName */
	static const CLIP_PLANE0                    = 0x3000;
	static const CLIP_PLANE1                    = 0x3001;
	static const CLIP_PLANE2                    = 0x3002;
	static const CLIP_PLANE3                    = 0x3003;
	static const CLIP_PLANE4                    = 0x3004;
	static const CLIP_PLANE5                    = 0x3005;

	/* ColorMaterialFace */
	/*      FRONT */
	/*      BACK */
	/*      FRONT_AND_BACK */

	/* ColorMaterialParameter */
	/*      AMBIENT */
	/*      DIFFUSE */
	/*      SPECULAR */
	/*      EMISSION */
	/*      AMBIENT_AND_DIFFUSE */

	/* ColorPointerType */
	/*      BYTE */
	/*      UNSIGNED_BYTE */
	/*      SHORT */
	/*      UNSIGNED_SHORT */
	/*      INT */
	/*      UNSIGNED_INT */
	/*      FLOAT */
	/*      DOUBLE */

	/* CullFaceMode */
	/*      FRONT */
	/*      BACK */
	/*      FRONT_AND_BACK */

	/* DataType */
	static const BYTE                           = 0x1400;
	static const UNSIGNED_BYTE                  = 0x1401;
	static const SHORT                          = 0x1402;
	static const UNSIGNED_SHORT                 = 0x1403;
	static const INT                            = 0x1404;
	static const UNSIGNED_INT                   = 0x1405;
	static const FLOAT                          = 0x1406;
	static const TWO_BYTES                      = 0x1407;
	static const THREE_BYTES                    = 0x1408;
	static const FOUR_BYTES                     = 0x1409;
	static const DOUBLE                         = 0x140A;

	/* DepthFunction */
	/*      NEVER */
	/*      LESS */
	/*      EQUAL */
	/*      LEQUAL */
	/*      GREATER */
	/*      NOTEQUAL */
	/*      GEQUAL */
	/*      ALWAYS */

	/* DrawBufferMode */
	static const NONE                           = 0;
	static const FRONT_LEFT                     = 0x0400;
	static const FRONT_RIGHT                    = 0x0401;
	static const BACK_LEFT                      = 0x0402;
	static const BACK_RIGHT                     = 0x0403;
	static const FRONT                          = 0x0404;
	static const BACK                           = 0x0405;
	static const LEFT                           = 0x0406;
	static const RIGHT                          = 0x0407;
	static const FRONT_AND_BACK                 = 0x0408;
	static const AUX0                           = 0x0409;
	static const AUX1                           = 0x040A;
	static const AUX2                           = 0x040B;
	static const AUX3                           = 0x040C;

	/* Enable */
	/*      FOG */
	/*      LIGHTING */
	/*      TEXTURE_1D */
	/*      TEXTURE_2D */
	/*      LINE_STIPPLE */
	/*      POLYGON_STIPPLE */
	/*      CULL_FACE */
	/*      ALPHA_TEST */
	/*      BLEND */
	/*      INDEX_LOGIC_OP */
	/*      COLOR_LOGIC_OP */
	/*      DITHER */
	/*      STENCIL_TEST */
	/*      DEPTH_TEST */
	/*      CLIP_PLANE0 */
	/*      CLIP_PLANE1 */
	/*      CLIP_PLANE2 */
	/*      CLIP_PLANE3 */
	/*      CLIP_PLANE4 */
	/*      CLIP_PLANE5 */
	/*      LIGHT0 */
	/*      LIGHT1 */
	/*      LIGHT2 */
	/*      LIGHT3 */
	/*      LIGHT4 */
	/*      LIGHT5 */
	/*      LIGHT6 */
	/*      LIGHT7 */
	/*      TEXTURE_GEN_S */
	/*      TEXTURE_GEN_T */
	/*      TEXTURE_GEN_R */
	/*      TEXTURE_GEN_Q */
	/*      MAP1_VERTEX_3 */
	/*      MAP1_VERTEX_4 */
	/*      MAP1_COLOR_4 */
	/*      MAP1_INDEX */
	/*      MAP1_NORMAL */
	/*      MAP1_TEXTURE_COORD_1 */
	/*      MAP1_TEXTURE_COORD_2 */
	/*      MAP1_TEXTURE_COORD_3 */
	/*      MAP1_TEXTURE_COORD_4 */
	/*      MAP2_VERTEX_3 */
	/*      MAP2_VERTEX_4 */
	/*      MAP2_COLOR_4 */
	/*      MAP2_INDEX */
	/*      MAP2_NORMAL */
	/*      MAP2_TEXTURE_COORD_1 */
	/*      MAP2_TEXTURE_COORD_2 */
	/*      MAP2_TEXTURE_COORD_3 */
	/*      MAP2_TEXTURE_COORD_4 */
	/*      POINT_SMOOTH */
	/*      LINE_SMOOTH */
	/*      POLYGON_SMOOTH */
	/*      SCISSOR_TEST */
	/*      COLOR_MATERIAL */
	/*      NORMALIZE */
	/*      AUTO_NORMAL */
	/*      VERTEX_ARRAY */
	/*      NORMAL_ARRAY */
	/*      COLOR_ARRAY */
	/*      INDEX_ARRAY */
	/*      TEXTURE_COORD_ARRAY */
	/*      EDGE_FLAG_ARRAY */
	/*      POLYGON_OFFSET_POINT */
	/*      POLYGON_OFFSET_LINE */
	/*      POLYGON_OFFSET_FILL */

	/* ErrorCode */
	static const NO_ERROR                       = 0;
	static const INVALID_ENUM                   = 0x0500;
	static const INVALID_VALUE                  = 0x0501;
	static const INVALID_OPERATION              = 0x0502;
	static const STACK_OVERFLOW                 = 0x0503;
	static const STACK_UNDERFLOW                = 0x0504;
	static const OUT_OF_MEMORY                  = 0x0505;

	/* FeedBackMode */
	static const _2D                            = 0x0600;
	static const _3D                            = 0x0601;
	static const _3D_COLOR                      = 0x0602;
	static const _3D_COLOR_TEXTURE              = 0x0603;
	static const _4D_COLOR_TEXTURE              = 0x0604;

	/* FeedBackToken */
	static const PASS_THROUGH_TOKEN             = 0x0700;
	static const POINT_TOKEN                    = 0x0701;
	static const LINE_TOKEN                     = 0x0702;
	static const POLYGON_TOKEN                  = 0x0703;
	static const BITMAP_TOKEN                   = 0x0704;
	static const DRAW_PIXEL_TOKEN               = 0x0705;
	static const COPY_PIXEL_TOKEN               = 0x0706;
	static const LINE_RESET_TOKEN               = 0x0707;

	/* FogMode */
	/*      LINEAR */
	static const EXP                            = 0x0800;
	static const EXP2                           = 0x0801;


	/* FogParameter */
	/*      FOG_COLOR */
	/*      FOG_DENSITY */
	/*      FOG_END */
	/*      FOG_INDEX */
	/*      FOG_MODE */
	/*      FOG_START */

	/* FrontFaceDirection */
	static const CW                             = 0x0900;
	static const CCW                            = 0x0901;

	/* GetMapTarget */
	static const COEFF                          = 0x0A00;
	static const ORDER                          = 0x0A01;
	static const DOMAIN                         = 0x0A02;

	/* GetPixelMap */
	/*      PIXEL_MAP_I_TO_I */
	/*      PIXEL_MAP_S_TO_S */
	/*      PIXEL_MAP_I_TO_R */
	/*      PIXEL_MAP_I_TO_G */
	/*      PIXEL_MAP_I_TO_B */
	/*      PIXEL_MAP_I_TO_A */
	/*      PIXEL_MAP_R_TO_R */
	/*      PIXEL_MAP_G_TO_G */
	/*      PIXEL_MAP_B_TO_B */
	/*      PIXEL_MAP_A_TO_A */

	/* GetPointerTarget */
	/*      VERTEX_ARRAY_POINTER */
	/*      NORMAL_ARRAY_POINTER */
	/*      COLOR_ARRAY_POINTER */
	/*      INDEX_ARRAY_POINTER */
	/*      TEXTURE_COORD_ARRAY_POINTER */
	/*      EDGE_FLAG_ARRAY_POINTER */

	/* GetTarget */
	static const CURRENT_COLOR                  = 0x0B00;
	static const CURRENT_INDEX                  = 0x0B01;
	static const CURRENT_NORMAL                 = 0x0B02;
	static const CURRENT_TEXTURE_COORDS         = 0x0B03;
	static const CURRENT_RASTER_COLOR           = 0x0B04;
	static const CURRENT_RASTER_INDEX           = 0x0B05;
	static const CURRENT_RASTER_TEXTURE_COORDS  = 0x0B06;
	static const CURRENT_RASTER_POSITION        = 0x0B07;
	static const CURRENT_RASTER_POSITION_VALID  = 0x0B08;
	static const CURRENT_RASTER_DISTANCE        = 0x0B09;
	static const POINT_SMOOTH                   = 0x0B10;
	static const POINT_SIZE                     = 0x0B11;
	static const POINT_SIZE_RANGE               = 0x0B12;
	static const POINT_SIZE_GRANULARITY         = 0x0B13;
	static const LINE_SMOOTH                    = 0x0B20;
	static const LINE_WIDTH                     = 0x0B21;
	static const LINE_WIDTH_RANGE               = 0x0B22;
	static const LINE_WIDTH_GRANULARITY         = 0x0B23;
	static const LINE_STIPPLE                   = 0x0B24;
	static const LINE_STIPPLE_PATTERN           = 0x0B25;
	static const LINE_STIPPLE_REPEAT            = 0x0B26;
	static const LIST_MODE                      = 0x0B30;
	static const MAX_LIST_NESTING               = 0x0B31;
	static const LIST_BASE                      = 0x0B32;
	static const LIST_INDEX                     = 0x0B33;
	static const POLYGON_MODE                   = 0x0B40;
	static const POLYGON_SMOOTH                 = 0x0B41;
	static const POLYGON_STIPPLE                = 0x0B42;
	static const EDGE_FLAG                      = 0x0B43;
	static const CULL_FACE                      = 0x0B44;
	static const CULL_FACE_MODE                 = 0x0B45;
	static const FRONT_FACE                     = 0x0B46;
	static const LIGHTING                       = 0x0B50;
	static const LIGHT_MODEL_LOCAL_VIEWER       = 0x0B51;
	static const LIGHT_MODEL_TWO_SIDE           = 0x0B52;
	static const LIGHT_MODEL_AMBIENT            = 0x0B53;
	static const SHADE_MODEL                    = 0x0B54;
	static const COLOR_MATERIAL_FACE            = 0x0B55;
	static const COLOR_MATERIAL_PARAMETER       = 0x0B56;
	static const COLOR_MATERIAL                 = 0x0B57;
	static const FOG                            = 0x0B60;
	static const FOG_INDEX                      = 0x0B61;
	static const FOG_DENSITY                    = 0x0B62;
	static const FOG_START                      = 0x0B63;
	static const FOG_END                        = 0x0B64;
	static const FOG_MODE                       = 0x0B65;
	static const FOG_COLOR                      = 0x0B66;
	static const DEPTH_RANGE                    = 0x0B70;
	static const DEPTH_TEST                     = 0x0B71;
	static const DEPTH_WRITEMASK                = 0x0B72;
	static const DEPTH_CLEAR_VALUE              = 0x0B73;
	static const DEPTH_FUNC                     = 0x0B74;
	static const ACCUM_CLEAR_VALUE              = 0x0B80;
	static const STENCIL_TEST                   = 0x0B90;
	static const STENCIL_CLEAR_VALUE            = 0x0B91;
	static const STENCIL_FUNC                   = 0x0B92;
	static const STENCIL_VALUE_MASK             = 0x0B93;
	static const STENCIL_FAIL                   = 0x0B94;
	static const STENCIL_PASS_DEPTH_FAIL        = 0x0B95;
	static const STENCIL_PASS_DEPTH_PASS        = 0x0B96;
	static const STENCIL_REF                    = 0x0B97;
	static const STENCIL_WRITEMASK              = 0x0B98;
	static const MATRIX_MODE                    = 0x0BA0;
	static const NORMALIZE                      = 0x0BA1;
	static const VIEWPORT                       = 0x0BA2;
	static const MODELVIEW_STACK_DEPTH          = 0x0BA3;
	static const PROJECTION_STACK_DEPTH         = 0x0BA4;
	static const TEXTURE_STACK_DEPTH            = 0x0BA5;
	static const MODELVIEW_MATRIX               = 0x0BA6;
	static const PROJECTION_MATRIX              = 0x0BA7;
	static const TEXTURE_MATRIX                 = 0x0BA8;
	static const ATTRIB_STACK_DEPTH             = 0x0BB0;
	static const CLIENT_ATTRIB_STACK_DEPTH      = 0x0BB1;
	static const ALPHA_TEST                     = 0x0BC0;
	static const ALPHA_TEST_FUNC                = 0x0BC1;
	static const ALPHA_TEST_REF                 = 0x0BC2;
	static const DITHER                         = 0x0BD0;
	static const BLEND_DST                      = 0x0BE0;
	static const BLEND_SRC                      = 0x0BE1;
	static const BLEND                          = 0x0BE2;
	static const LOGIC_OP_MODE                  = 0x0BF0;
	static const INDEX_LOGIC_OP                 = 0x0BF1;
	static const COLOR_LOGIC_OP                 = 0x0BF2;
	static const AUX_BUFFERS                    = 0x0C00;
	static const DRAW_BUFFER                    = 0x0C01;
	static const READ_BUFFER                    = 0x0C02;
	static const SCISSOR_BOX                    = 0x0C10;
	static const SCISSOR_TEST                   = 0x0C11;
	static const INDEX_CLEAR_VALUE              = 0x0C20;
	static const INDEX_WRITEMASK                = 0x0C21;
	static const COLOR_CLEAR_VALUE              = 0x0C22;
	static const COLOR_WRITEMASK                = 0x0C23;
	static const INDEX_MODE                     = 0x0C30;
	static const RGBA_MODE                      = 0x0C31;
	static const DOUBLEBUFFER                   = 0x0C32;
	static const STEREO                         = 0x0C33;
	static const RENDER_MODE                    = 0x0C40;
	static const PERSPECTIVE_CORRECTION_HINT    = 0x0C50;
	static const POINT_SMOOTH_HINT              = 0x0C51;
	static const LINE_SMOOTH_HINT               = 0x0C52;
	static const POLYGON_SMOOTH_HINT            = 0x0C53;
	static const FOG_HINT                       = 0x0C54;
	static const TEXTURE_GEN_S                  = 0x0C60;
	static const TEXTURE_GEN_T                  = 0x0C61;
	static const TEXTURE_GEN_R                  = 0x0C62;
	static const TEXTURE_GEN_Q                  = 0x0C63;
	static const PIXEL_MAP_I_TO_I               = 0x0C70;
	static const PIXEL_MAP_S_TO_S               = 0x0C71;
	static const PIXEL_MAP_I_TO_R               = 0x0C72;
	static const PIXEL_MAP_I_TO_G               = 0x0C73;
	static const PIXEL_MAP_I_TO_B               = 0x0C74;
	static const PIXEL_MAP_I_TO_A               = 0x0C75;
	static const PIXEL_MAP_R_TO_R               = 0x0C76;
	static const PIXEL_MAP_G_TO_G               = 0x0C77;
	static const PIXEL_MAP_B_TO_B               = 0x0C78;
	static const PIXEL_MAP_A_TO_A               = 0x0C79;
	static const PIXEL_MAP_I_TO_I_SIZE          = 0x0CB0;
	static const PIXEL_MAP_S_TO_S_SIZE          = 0x0CB1;
	static const PIXEL_MAP_I_TO_R_SIZE          = 0x0CB2;
	static const PIXEL_MAP_I_TO_G_SIZE          = 0x0CB3;
	static const PIXEL_MAP_I_TO_B_SIZE          = 0x0CB4;
	static const PIXEL_MAP_I_TO_A_SIZE          = 0x0CB5;
	static const PIXEL_MAP_R_TO_R_SIZE          = 0x0CB6;
	static const PIXEL_MAP_G_TO_G_SIZE          = 0x0CB7;
	static const PIXEL_MAP_B_TO_B_SIZE          = 0x0CB8;
	static const PIXEL_MAP_A_TO_A_SIZE          = 0x0CB9;
	static const UNPACK_SWAP_BYTES              = 0x0CF0;
	static const UNPACK_LSB_FIRST               = 0x0CF1;
	static const UNPACK_ROW_LENGTH              = 0x0CF2;
	static const UNPACK_SKIP_ROWS               = 0x0CF3;
	static const UNPACK_SKIP_PIXELS             = 0x0CF4;
	static const UNPACK_ALIGNMENT               = 0x0CF5;
	static const PACK_SWAP_BYTES                = 0x0D00;
	static const PACK_LSB_FIRST                 = 0x0D01;
	static const PACK_ROW_LENGTH                = 0x0D02;
	static const PACK_SKIP_ROWS                 = 0x0D03;
	static const PACK_SKIP_PIXELS               = 0x0D04;
	static const PACK_ALIGNMENT                 = 0x0D05;
	static const MAP_COLOR                      = 0x0D10;
	static const MAP_STENCIL                    = 0x0D11;
	static const INDEX_SHIFT                    = 0x0D12;
	static const INDEX_OFFSET                   = 0x0D13;
	static const RED_SCALE                      = 0x0D14;
	static const RED_BIAS                       = 0x0D15;
	static const ZOOM_X                         = 0x0D16;
	static const ZOOM_Y                         = 0x0D17;
	static const GREEN_SCALE                    = 0x0D18;
	static const GREEN_BIAS                     = 0x0D19;
	static const BLUE_SCALE                     = 0x0D1A;
	static const BLUE_BIAS                      = 0x0D1B;
	static const ALPHA_SCALE                    = 0x0D1C;
	static const ALPHA_BIAS                     = 0x0D1D;
	static const DEPTH_SCALE                    = 0x0D1E;
	static const DEPTH_BIAS                     = 0x0D1F;
	static const MAX_EVAL_ORDER                 = 0x0D30;
	static const MAX_LIGHTS                     = 0x0D31;
	static const MAX_CLIP_PLANES                = 0x0D32;
	static const MAX_TEXTURE_SIZE               = 0x0D33;
	static const MAX_PIXEL_MAP_TABLE            = 0x0D34;
	static const MAX_ATTRIB_STACK_DEPTH         = 0x0D35;
	static const MAX_MODELVIEW_STACK_DEPTH      = 0x0D36;
	static const MAX_NAME_STACK_DEPTH           = 0x0D37;
	static const MAX_PROJECTION_STACK_DEPTH     = 0x0D38;
	static const MAX_TEXTURE_STACK_DEPTH        = 0x0D39;
	static const MAX_VIEWPORT_DIMS              = 0x0D3A;
	static const MAX_CLIENT_ATTRIB_STACK_DEPTH  = 0x0D3B;
	static const SUBPIXEL_BITS                  = 0x0D50;
	static const INDEX_BITS                     = 0x0D51;
	static const RED_BITS                       = 0x0D52;
	static const GREEN_BITS                     = 0x0D53;
	static const BLUE_BITS                      = 0x0D54;
	static const ALPHA_BITS                     = 0x0D55;
	static const DEPTH_BITS                     = 0x0D56;
	static const STENCIL_BITS                   = 0x0D57;
	static const ACCUM_RED_BITS                 = 0x0D58;
	static const ACCUM_GREEN_BITS               = 0x0D59;
	static const ACCUM_BLUE_BITS                = 0x0D5A;
	static const ACCUM_ALPHA_BITS               = 0x0D5B;
	static const NAME_STACK_DEPTH               = 0x0D70;
	static const AUTO_NORMAL                    = 0x0D80;
	static const MAP1_COLOR_4                   = 0x0D90;
	static const MAP1_INDEX                     = 0x0D91;
	static const MAP1_NORMAL                    = 0x0D92;
	static const MAP1_TEXTURE_COORD_1           = 0x0D93;
	static const MAP1_TEXTURE_COORD_2           = 0x0D94;
	static const MAP1_TEXTURE_COORD_3           = 0x0D95;
	static const MAP1_TEXTURE_COORD_4           = 0x0D96;
	static const MAP1_VERTEX_3                  = 0x0D97;
	static const MAP1_VERTEX_4                  = 0x0D98;
	static const MAP2_COLOR_4                   = 0x0DB0;
	static const MAP2_INDEX                     = 0x0DB1;
	static const MAP2_NORMAL                    = 0x0DB2;
	static const MAP2_TEXTURE_COORD_1           = 0x0DB3;
	static const MAP2_TEXTURE_COORD_2           = 0x0DB4;
	static const MAP2_TEXTURE_COORD_3           = 0x0DB5;
	static const MAP2_TEXTURE_COORD_4           = 0x0DB6;
	static const MAP2_VERTEX_3                  = 0x0DB7;
	static const MAP2_VERTEX_4                  = 0x0DB8;
	static const MAP1_GRID_DOMAIN               = 0x0DD0;
	static const MAP1_GRID_SEGMENTS             = 0x0DD1;
	static const MAP2_GRID_DOMAIN               = 0x0DD2;
	static const MAP2_GRID_SEGMENTS             = 0x0DD3;
	static const TEXTURE_1D                     = 0x0DE0;
	static const TEXTURE_2D                     = 0x0DE1;
	static const FEEDBACK_BUFFER_POINTER        = 0x0DF0;
	static const FEEDBACK_BUFFER_SIZE           = 0x0DF1;
	static const FEEDBACK_BUFFER_TYPE           = 0x0DF2;
	static const SELECTION_BUFFER_POINTER       = 0x0DF3;
	static const SELECTION_BUFFER_SIZE          = 0x0DF4;
	/*      TEXTURE_BINDING_1D */
	/*      TEXTURE_BINDING_2D */
	/*      VERTEX_ARRAY */
	/*      NORMAL_ARRAY */
	/*      COLOR_ARRAY */
	/*      INDEX_ARRAY */
	/*      TEXTURE_COORD_ARRAY */
	/*      EDGE_FLAG_ARRAY */
	/*      VERTEX_ARRAY_SIZE */
	/*      VERTEX_ARRAY_TYPE */
	/*      VERTEX_ARRAY_STRIDE */
	/*      NORMAL_ARRAY_TYPE */
	/*      NORMAL_ARRAY_STRIDE */
	/*      COLOR_ARRAY_SIZE */
	/*      COLOR_ARRAY_TYPE */
	/*      COLOR_ARRAY_STRIDE */
	/*      INDEX_ARRAY_TYPE */
	/*      INDEX_ARRAY_STRIDE */
	/*      TEXTURE_COORD_ARRAY_SIZE */
	/*      TEXTURE_COORD_ARRAY_TYPE */
	/*      TEXTURE_COORD_ARRAY_STRIDE */
	/*      EDGE_FLAG_ARRAY_STRIDE */
	/*      POLYGON_OFFSET_FACTOR */
	/*      POLYGON_OFFSET_UNITS */

	/* GetTextureParameter */
	/*      TEXTURE_MAG_FILTER */
	/*      TEXTURE_MIN_FILTER */
	/*      TEXTURE_WRAP_S */
	/*      TEXTURE_WRAP_T */
	static const TEXTURE_WIDTH                  = 0x1000;
	static const TEXTURE_HEIGHT                 = 0x1001;
	static const TEXTURE_INTERNAL_FORMAT        = 0x1003;
	static const TEXTURE_BORDER_COLOR           = 0x1004;
	static const TEXTURE_BORDER                 = 0x1005;
	/*      TEXTURE_RED_SIZE */
	/*      TEXTURE_GREEN_SIZE */
	/*      TEXTURE_BLUE_SIZE */
	/*      TEXTURE_ALPHA_SIZE */
	/*      TEXTURE_LUMINANCE_SIZE */
	/*      TEXTURE_INTENSITY_SIZE */
	/*      TEXTURE_PRIORITY */
	/*      TEXTURE_RESIDENT */

	/* HintMode */
	static const DONT_CARE                      = 0x1100;
	static const FASTEST                        = 0x1101;
	static const NICEST                         = 0x1102;

	/* HintTarget */
	/*      PERSPECTIVE_CORRECTION_HINT */
	/*      POINT_SMOOTH_HINT */
	/*      LINE_SMOOTH_HINT */
	/*      POLYGON_SMOOTH_HINT */
	/*      FOG_HINT */
	/*      PHONG_HINT */

	/* IndexPointerType */
	/*      SHORT */
	/*      INT */
	/*      FLOAT */
	/*      DOUBLE */

	/* LightModelParameter */
	/*      LIGHT_MODEL_AMBIENT */
	/*      LIGHT_MODEL_LOCAL_VIEWER */
	/*      LIGHT_MODEL_TWO_SIDE */

	/* LightName */
	static const LIGHT0                         = 0x4000;
	static const LIGHT1                         = 0x4001;
	static const LIGHT2                         = 0x4002;
	static const LIGHT3                         = 0x4003;
	static const LIGHT4                         = 0x4004;
	static const LIGHT5                         = 0x4005;
	static const LIGHT6                         = 0x4006;
	static const LIGHT7                         = 0x4007;

	/* LightParameter */
	static const AMBIENT                        = 0x1200;
	static const DIFFUSE                        = 0x1201;
	static const SPECULAR                       = 0x1202;
	static const POSITION                       = 0x1203;
	static const SPOT_DIRECTION                 = 0x1204;
	static const SPOT_EXPONENT                  = 0x1205;
	static const SPOT_CUTOFF                    = 0x1206;
	static const CONSTANT_ATTENUATION           = 0x1207;
	static const LINEAR_ATTENUATION             = 0x1208;
	static const QUADRATIC_ATTENUATION          = 0x1209;

	/* InterleavedArrays */
	/*      V2F */
	/*      V3F */
	/*      C4UB_V2F */
	/*      C4UB_V3F */
	/*      C3F_V3F */
	/*      N3F_V3F */
	/*      C4F_N3F_V3F */
	/*      T2F_V3F */
	/*      T4F_V4F */
	/*      T2F_C4UB_V3F */
	/*      T2F_C3F_V3F */
	/*      T2F_N3F_V3F */
	/*      T2F_C4F_N3F_V3F */
	/*      T4F_C4F_N3F_V4F */

	/* ListMode */
	static const COMPILE                        = 0x1300;
	static const COMPILE_AND_EXECUTE            = 0x1301;

	/* ListNameType */
	/*      BYTE */
	/*      UNSIGNED_BYTE */
	/*      SHORT */
	/*      UNSIGNED_SHORT */
	/*      INT */
	/*      UNSIGNED_INT */
	/*      FLOAT */
	/*      2_BYTES */
	/*      3_BYTES */
	/*      4_BYTES */

	/* LogicOp */
	static const CLEAR                          = 0x1500;
	static const AND                            = 0x1501;
	static const AND_REVERSE                    = 0x1502;
	static const COPY                           = 0x1503;
	static const AND_INVERTED                   = 0x1504;
	static const NOOP                           = 0x1505;
	static const XOR                            = 0x1506;
	static const OR                             = 0x1507;
	static const NOR                            = 0x1508;
	static const EQUIV                          = 0x1509;
	static const INVERT                         = 0x150A;
	static const OR_REVERSE                     = 0x150B;
	static const COPY_INVERTED                  = 0x150C;
	static const OR_INVERTED                    = 0x150D;
	static const NAND                           = 0x150E;
	static const SET                            = 0x150F;

	/* MapTarget */
	/*      MAP1_COLOR_4 */
	/*      MAP1_INDEX */
	/*      MAP1_NORMAL */
	/*      MAP1_TEXTURE_COORD_1 */
	/*      MAP1_TEXTURE_COORD_2 */
	/*      MAP1_TEXTURE_COORD_3 */
	/*      MAP1_TEXTURE_COORD_4 */
	/*      MAP1_VERTEX_3 */
	/*      MAP1_VERTEX_4 */
	/*      MAP2_COLOR_4 */
	/*      MAP2_INDEX */
	/*      MAP2_NORMAL */
	/*      MAP2_TEXTURE_COORD_1 */
	/*      MAP2_TEXTURE_COORD_2 */
	/*      MAP2_TEXTURE_COORD_3 */
	/*      MAP2_TEXTURE_COORD_4 */
	/*      MAP2_VERTEX_3 */
	/*      MAP2_VERTEX_4 */

	/* MaterialFace */
	/*      FRONT */
	/*      BACK */
	/*      FRONT_AND_BACK */

	/* MaterialParameter */
	static const EMISSION                       = 0x1600;
	static const SHININESS                      = 0x1601;
	static const AMBIENT_AND_DIFFUSE            = 0x1602;
	static const COLOR_INDEXES                  = 0x1603;
	/*      AMBIENT */
	/*      DIFFUSE */
	/*      SPECULAR */

	/* MatrixMode */
	static const MODELVIEW                      = 0x1700;
	static const PROJECTION                     = 0x1701;
	static const TEXTURE                        = 0x1702;

	/* MeshMode1 */
	/*      POINT */
	/*      LINE */

	/* MeshMode2 */
	/*      POINT */
	/*      LINE */
	/*      FILL */

	/* NormalPointerType */
	/*      BYTE */
	/*      SHORT */
	/*      INT */
	/*      FLOAT */
	/*      DOUBLE */

	/* PixelCopyType */
	static const COLOR                          = 0x1800;
	static const DEPTH                          = 0x1801;
	static const STENCIL                        = 0x1802;

	/* PixelFormat */
	static const COLOR_INDEX                    = 0x1900;
	static const STENCIL_INDEX                  = 0x1901;
	static const DEPTH_COMPONENT                = 0x1902;
	static const RED                            = 0x1903;
	static const GREEN                          = 0x1904;
	static const BLUE                           = 0x1905;
	static const ALPHA                          = 0x1906;
	static const RGB                            = 0x1907;
	static const RGBA                           = 0x1908;
	static const LUMINANCE                      = 0x1909;
	static const LUMINANCE_ALPHA                = 0x190A;

	/* PixelMap */
	/*      PIXEL_MAP_I_TO_I */
	/*      PIXEL_MAP_S_TO_S */
	/*      PIXEL_MAP_I_TO_R */
	/*      PIXEL_MAP_I_TO_G */
	/*      PIXEL_MAP_I_TO_B */
	/*      PIXEL_MAP_I_TO_A */
	/*      PIXEL_MAP_R_TO_R */
	/*      PIXEL_MAP_G_TO_G */
	/*      PIXEL_MAP_B_TO_B */
	/*      PIXEL_MAP_A_TO_A */

	/* PixelStore */
	/*      UNPACK_SWAP_BYTES */
	/*      UNPACK_LSB_FIRST */
	/*      UNPACK_ROW_LENGTH */
	/*      UNPACK_SKIP_ROWS */
	/*      UNPACK_SKIP_PIXELS */
	/*      UNPACK_ALIGNMENT */
	/*      PACK_SWAP_BYTES */
	/*      PACK_LSB_FIRST */
	/*      PACK_ROW_LENGTH */
	/*      PACK_SKIP_ROWS */
	/*      PACK_SKIP_PIXELS */
	/*      PACK_ALIGNMENT */

	/* PixelTransfer */
	/*      MAP_COLOR */
	/*      MAP_STENCIL */
	/*      INDEX_SHIFT */
	/*      INDEX_OFFSET */
	/*      RED_SCALE */
	/*      RED_BIAS */
	/*      GREEN_SCALE */
	/*      GREEN_BIAS */
	/*      BLUE_SCALE */
	/*      BLUE_BIAS */
	/*      ALPHA_SCALE */
	/*      ALPHA_BIAS */
	/*      DEPTH_SCALE */
	/*      DEPTH_BIAS */

	/* PixelType */
	static const BITMAP                         = 0x1A00;
	/*      BYTE */
	/*      UNSIGNED_BYTE */
	/*      SHORT */
	/*      UNSIGNED_SHORT */
	/*      INT */
	/*      UNSIGNED_INT */
	/*      FLOAT */

	/* PolygonMode */
	static const POINT                          = 0x1B00;
	static const LINE                           = 0x1B01;
	static const FILL                           = 0x1B02;

	/* ReadBufferMode */
	/*      FRONT_LEFT */
	/*      FRONT_RIGHT */
	/*      BACK_LEFT */
	/*      BACK_RIGHT */
	/*      FRONT */
	/*      BACK */
	/*      LEFT */
	/*      RIGHT */
	/*      AUX0 */
	/*      AUX1 */
	/*      AUX2 */
	/*      AUX3 */

	/* RenderingMode */
	static const RENDER                         = 0x1C00;
	static const FEEDBACK                       = 0x1C01;
	static const SELECT                         = 0x1C02;

	/* ShadingModel */
	static const FLAT                           = 0x1D00;
	static const SMOOTH                         = 0x1D01;


	/* StencilFunction */
	/*      NEVER */
	/*      LESS */
	/*      EQUAL */
	/*      LEQUAL */
	/*      GREATER */
	/*      NOTEQUAL */
	/*      GEQUAL */
	/*      ALWAYS */

	/* StencilOp */
	/*      ZERO */
	static const KEEP                           = 0x1E00;
	static const REPLACE                        = 0x1E01;
	static const INCR                           = 0x1E02;
	static const DECR                           = 0x1E03;
	/*      INVERT */

	/* StringName */
	static const VENDOR                         = 0x1F00;
	static const RENDERER                       = 0x1F01;
	static const VERSION                        = 0x1F02;
	static const EXTENSIONS                     = 0x1F03;

	/* TextureCoordName */
	static const S                              = 0x2000;
	static const T                              = 0x2001;
	static const R                              = 0x2002;
	static const Q                              = 0x2003;

	/* TexCoordPointerType */
	/*      SHORT */
	/*      INT */
	/*      FLOAT */
	/*      DOUBLE */

	/* TextureEnvMode */
	static const MODULATE                       = 0x2100;
	static const DECAL                          = 0x2101;
	/*      BLEND */
	/*      REPLACE */

	/* TextureEnvParameter */
	static const TEXTURE_ENV_MODE               = 0x2200;
	static const TEXTURE_ENV_COLOR              = 0x2201;

	/* TextureEnvTarget */
	static const TEXTURE_ENV                    = 0x2300;

	/* TextureGenMode */
	static const EYE_LINEAR                     = 0x2400;
	static const OBJECT_LINEAR                  = 0x2401;
	static const SPHERE_MAP                     = 0x2402;

	/* TextureGenParameter */
	static const TEXTURE_GEN_MODE               = 0x2500;
	static const OBJECT_PLANE                   = 0x2501;
	static const EYE_PLANE                      = 0x2502;

	/* TextureMagFilter */
	static const NEAREST                        = 0x2600;
	static const LINEAR                         = 0x2601;

	/* TextureMinFilter */
	/*      NEAREST */
	/*      LINEAR */
	static const NEAREST_MIPMAP_NEAREST         = 0x2700;
	static const LINEAR_MIPMAP_NEAREST          = 0x2701;
	static const NEAREST_MIPMAP_LINEAR          = 0x2702;
	static const LINEAR_MIPMAP_LINEAR           = 0x2703;

	/* TextureParameterName */
	static const TEXTURE_MAG_FILTER             = 0x2800;
	static const TEXTURE_MIN_FILTER             = 0x2801;
	static const TEXTURE_WRAP_S                 = 0x2802;
	static const TEXTURE_WRAP_T                 = 0x2803;
	/*      TEXTURE_BORDER_COLOR */
	/*      TEXTURE_PRIORITY */

	/* TextureTarget */
	/*      TEXTURE_1D */
	/*      TEXTURE_2D */
	/*      PROXY_TEXTURE_1D */
	/*      PROXY_TEXTURE_2D */

	/* TextureWrapMode */
	static const CLAMP                          = 0x2900;
	static const REPEAT                         = 0x2901;

	/* VertexPointerType */
	/*      SHORT */
	/*      INT */
	/*      FLOAT */
	/*      DOUBLE */

	/* ClientAttribMask */
	static const CLIENT_PIXEL_STORE_BIT         = 0x00000001;
	static const CLIENT_VERTEX_ARRAY_BIT        = 0x00000002;
	static const CLIENT_ALL_ATTRIB_BITS         = 0xffffffff;

	/* polygon_offset */
	static const POLYGON_OFFSET_FACTOR          = 0x8038;
	static const POLYGON_OFFSET_UNITS           = 0x2A00;
	static const POLYGON_OFFSET_POINT           = 0x2A01;
	static const POLYGON_OFFSET_LINE            = 0x2A02;
	static const POLYGON_OFFSET_FILL            = 0x8037;

	/* texture */
	static const ALPHA4                         = 0x803B;
	static const ALPHA8                         = 0x803C;
	static const ALPHA12                        = 0x803D;
	static const ALPHA16                        = 0x803E;
	static const LUMINANCE4                     = 0x803F;
	static const LUMINANCE8                     = 0x8040;
	static const LUMINANCE12                    = 0x8041;
	static const LUMINANCE16                    = 0x8042;
	static const LUMINANCE4_ALPHA4              = 0x8043;
	static const LUMINANCE6_ALPHA2              = 0x8044;
	static const LUMINANCE8_ALPHA8              = 0x8045;
	static const LUMINANCE12_ALPHA4             = 0x8046;
	static const LUMINANCE12_ALPHA12            = 0x8047;
	static const LUMINANCE16_ALPHA16            = 0x8048;
	static const INTENSITY                      = 0x8049;
	static const INTENSITY4                     = 0x804A;
	static const INTENSITY8                     = 0x804B;
	static const INTENSITY12                    = 0x804C;
	static const INTENSITY16                    = 0x804D;
	static const R3_G3_B2                       = 0x2A10;
	static const RGB4                           = 0x804F;
	static const RGB5                           = 0x8050;
	static const RGB8                           = 0x8051;
	static const RGB10                          = 0x8052;
	static const RGB12                          = 0x8053;
	static const RGB16                          = 0x8054;
	static const RGBA2                          = 0x8055;
	static const RGBA4                          = 0x8056;
	static const RGB5_A1                        = 0x8057;
	static const RGBA8                          = 0x8058;
	static const RGB10_A2                       = 0x8059;
	static const RGBA12                         = 0x805A;
	static const RGBA16                         = 0x805B;
	static const TEXTURE_RED_SIZE               = 0x805C;
	static const TEXTURE_GREEN_SIZE             = 0x805D;
	static const TEXTURE_BLUE_SIZE              = 0x805E;
	static const TEXTURE_ALPHA_SIZE             = 0x805F;
	static const TEXTURE_LUMINANCE_SIZE         = 0x8060;
	static const TEXTURE_INTENSITY_SIZE         = 0x8061;
	static const PROXY_TEXTURE_1D               = 0x8063;
	static const PROXY_TEXTURE_2D               = 0x8064;

	/* texture_object */
	static const TEXTURE_PRIORITY               = 0x8066;
	static const TEXTURE_RESIDENT               = 0x8067;
	static const TEXTURE_BINDING_1D             = 0x8068;
	static const TEXTURE_BINDING_2D             = 0x8069;

	/* vertex_array */
	static const VERTEX_ARRAY                   = 0x8074;
	static const NORMAL_ARRAY                   = 0x8075;
	static const COLOR_ARRAY                    = 0x8076;
	static const INDEX_ARRAY                    = 0x8077;
	static const TEXTURE_COORD_ARRAY            = 0x8078;
	static const EDGE_FLAG_ARRAY                = 0x8079;
	static const VERTEX_ARRAY_SIZE              = 0x807A;
	static const VERTEX_ARRAY_TYPE              = 0x807B;
	static const VERTEX_ARRAY_STRIDE            = 0x807C;
	static const NORMAL_ARRAY_TYPE              = 0x807E;
	static const NORMAL_ARRAY_STRIDE            = 0x807F;
	static const COLOR_ARRAY_SIZE               = 0x8081;
	static const COLOR_ARRAY_TYPE               = 0x8082;
	static const COLOR_ARRAY_STRIDE             = 0x8083;
	static const INDEX_ARRAY_TYPE               = 0x8085;
	static const INDEX_ARRAY_STRIDE             = 0x8086;
	static const TEXTURE_COORD_ARRAY_SIZE       = 0x8088;
	static const TEXTURE_COORD_ARRAY_TYPE       = 0x8089;
	static const TEXTURE_COORD_ARRAY_STRIDE     = 0x808A;
	static const EDGE_FLAG_ARRAY_STRIDE         = 0x808C;
	static const VERTEX_ARRAY_POINTER           = 0x808E;
	static const NORMAL_ARRAY_POINTER           = 0x808F;
	static const COLOR_ARRAY_POINTER            = 0x8090;
	static const INDEX_ARRAY_POINTER            = 0x8091;
	static const TEXTURE_COORD_ARRAY_POINTER    = 0x8092;
	static const EDGE_FLAG_ARRAY_POINTER        = 0x8093;
	static const V2F                            = 0x2A20;
	static const V3F                            = 0x2A21;
	static const C4UB_V2F                       = 0x2A22;
	static const C4UB_V3F                       = 0x2A23;
	static const C3F_V3F                        = 0x2A24;
	static const N3F_V3F                        = 0x2A25;
	static const C4F_N3F_V3F                    = 0x2A26;
	static const T2F_V3F                        = 0x2A27;
	static const T4F_V4F                        = 0x2A28;
	static const T2F_C4UB_V3F                   = 0x2A29;
	static const T2F_C3F_V3F                    = 0x2A2A;
	static const T2F_N3F_V3F                    = 0x2A2B;
	static const T2F_C4F_N3F_V3F                = 0x2A2C;
	static const T4F_C4F_N3F_V4F                = 0x2A2D;

	/* Extensions */
	#define EXT_vertex_array               1
	#define EXT_bgra                       1
	#define EXT_paletted_texture           1
	#define WIN_swap_hint                  1
	#define WIN_draw_range_elements        1
	// static const WIN_phong_shading              1
	// static const WIN_specular_fog               1

	/* EXT_vertex_array */
	static const VERTEX_ARRAY_EXT               = 0x8074;
	static const NORMAL_ARRAY_EXT               = 0x8075;
	static const COLOR_ARRAY_EXT                = 0x8076;
	static const INDEX_ARRAY_EXT                = 0x8077;
	static const TEXTURE_COORD_ARRAY_EXT        = 0x8078;
	static const EDGE_FLAG_ARRAY_EXT            = 0x8079;
	static const VERTEX_ARRAY_SIZE_EXT          = 0x807A;
	static const VERTEX_ARRAY_TYPE_EXT          = 0x807B;
	static const VERTEX_ARRAY_STRIDE_EXT        = 0x807C;
	static const VERTEX_ARRAY_COUNT_EXT         = 0x807D;
	static const NORMAL_ARRAY_TYPE_EXT          = 0x807E;
	static const NORMAL_ARRAY_STRIDE_EXT        = 0x807F;
	static const NORMAL_ARRAY_COUNT_EXT         = 0x8080;
	static const COLOR_ARRAY_SIZE_EXT           = 0x8081;
	static const COLOR_ARRAY_TYPE_EXT           = 0x8082;
	static const COLOR_ARRAY_STRIDE_EXT         = 0x8083;
	static const COLOR_ARRAY_COUNT_EXT          = 0x8084;
	static const INDEX_ARRAY_TYPE_EXT           = 0x8085;
	static const INDEX_ARRAY_STRIDE_EXT         = 0x8086;
	static const INDEX_ARRAY_COUNT_EXT          = 0x8087;
	static const TEXTURE_COORD_ARRAY_SIZE_EXT   = 0x8088;
	static const TEXTURE_COORD_ARRAY_TYPE_EXT   = 0x8089;
	static const TEXTURE_COORD_ARRAY_STRIDE_EXT = 0x808A;
	static const TEXTURE_COORD_ARRAY_COUNT_EXT  = 0x808B;
	static const EDGE_FLAG_ARRAY_STRIDE_EXT     = 0x808C;
	static const EDGE_FLAG_ARRAY_COUNT_EXT      = 0x808D;
	static const VERTEX_ARRAY_POINTER_EXT       = 0x808E;
	static const NORMAL_ARRAY_POINTER_EXT       = 0x808F;
	static const COLOR_ARRAY_POINTER_EXT        = 0x8090;
	static const INDEX_ARRAY_POINTER_EXT        = 0x8091;
	static const TEXTURE_COORD_ARRAY_POINTER_EXT = 0x8092;
	static const EDGE_FLAG_ARRAY_POINTER_EXT    = 0x8093;
	static const DOUBLE_EXT                     = DOUBLE;

	/* EXT_bgra */
	static const BGR_EXT                        = 0x80E0;
	static const BGRA_EXT                       = 0x80E1;

	/* EXT_paletted_texture */

	/* These must match the COLOR_TABLE_*_SGI enumerants */
	static const COLOR_TABLE_FORMAT_EXT         = 0x80D8;
	static const COLOR_TABLE_WIDTH_EXT          = 0x80D9;
	static const COLOR_TABLE_RED_SIZE_EXT       = 0x80DA;
	static const COLOR_TABLE_GREEN_SIZE_EXT     = 0x80DB;
	static const COLOR_TABLE_BLUE_SIZE_EXT      = 0x80DC;
	static const COLOR_TABLE_ALPHA_SIZE_EXT     = 0x80DD;
	static const COLOR_TABLE_LUMINANCE_SIZE_EXT = 0x80DE;
	static const COLOR_TABLE_INTENSITY_SIZE_EXT = 0x80DF;

	static const COLOR_INDEX1_EXT               = 0x80E2;
	static const COLOR_INDEX2_EXT               = 0x80E3;
	static const COLOR_INDEX4_EXT               = 0x80E4;
	static const COLOR_INDEX8_EXT               = 0x80E5;
	static const COLOR_INDEX12_EXT              = 0x80E6;
	static const COLOR_INDEX16_EXT              = 0x80E7;

	/* WIN_draw_range_elements */
	static const MAX_ELEMENTS_VERTICES_WIN      = 0x80E8;
	static const MAX_ELEMENTS_INDICES_WIN       = 0x80E9;

	/* WIN_phong_shading */
	static const PHONG_WIN                      = 0x80EA;
	static const PHONG_HINT_WIN                 = 0x80EB;

	/* WIN_specular_fog */
	static const FOG_SPECULAR_TEXTURE_WIN       = 0x80EC;

	/* For compatibility with OpenGL v1.0 */
	#define LOGIC_OP INDEX_LOGIC_OP
	#define TEXTURE_COMPONENTS TEXTURE_INTERNAL_FORMAT

	/*************************************************************/

	void __stdcall Accum (Enum op, Float value);
	void __stdcall AlphaFunc (Enum func, ClampF ref);
	Boolean __stdcall AreTexturesResident (Sizei n, const UInt *textures, Boolean *residences);
	void __stdcall ArrayElement (Int i);
	void __stdcall Begin (Enum mode);
	void __stdcall BindTexture (Enum target, UInt texture);
	void __stdcall Bitmap (Sizei width, Sizei height, Float xorig, Float yorig, Float xmove, Float ymove, const UByte *bitmap);
	void __stdcall BlendFunc (Enum sfactor, Enum dfactor);
	void __stdcall CallList (UInt list);
	void __stdcall CallLists (Sizei n, Enum type, const Void *lists);
	void __stdcall Clear (Bitfield mask);
	void __stdcall ClearAccum (Float red, Float green, Float blue, Float alpha);
	void __stdcall ClearColor (ClampF red, ClampF green, ClampF blue, ClampF alpha);
	void __stdcall ClearDepth (ClampD depth);
	void __stdcall ClearIndex (Float c);
	void __stdcall ClearStencil (Int s);
	void __stdcall ClipPlane (Enum plane, const Double *equation);
	void __stdcall Color3b (Byte red, Byte green, Byte blue);
	void __stdcall Color3bv (const Byte *v);
	void __stdcall Color3d (Double red, Double green, Double blue);
	void __stdcall Color3dv (const Double *v);
	void __stdcall Color3f (Float red, Float green, Float blue);
	void __stdcall Color3fv (const Float *v);
	void __stdcall Color3i (Int red, Int green, Int blue);
	void __stdcall Color3iv (const Int *v);
	void __stdcall Color3s (Short red, Short green, Short blue);
	void __stdcall Color3sv (const Short *v);
	void __stdcall Color3ub (UByte red, UByte green, UByte blue);
	void __stdcall Color3ubv (const UByte *v);
	void __stdcall Color3ui (UInt red, UInt green, UInt blue);
	void __stdcall Color3uiv (const UInt *v);
	void __stdcall Color3us (UShort red, UShort green, UShort blue);
	void __stdcall Color3usv (const UShort *v);
	void __stdcall Color4b (Byte red, Byte green, Byte blue, Byte alpha);
	void __stdcall Color4bv (const Byte *v);
	void __stdcall Color4d (Double red, Double green, Double blue, Double alpha);
	void __stdcall Color4dv (const Double *v);
	void __stdcall Color4f (Float red, Float green, Float blue, Float alpha);
	void __stdcall Color4fv (const Float *v);
	void __stdcall Color4i (Int red, Int green, Int blue, Int alpha);
	void __stdcall Color4iv (const Int *v);
	void __stdcall Color4s (Short red, Short green, Short blue, Short alpha);
	void __stdcall Color4sv (const Short *v);
	void __stdcall Color4ub (UByte red, UByte green, UByte blue, UByte alpha);
	void __stdcall Color4ubv (const UByte *v);
	void __stdcall Color4ui (UInt red, UInt green, UInt blue, UInt alpha);
	void __stdcall Color4uiv (const UInt *v);
	void __stdcall Color4us (UShort red, UShort green, UShort blue, UShort alpha);
	void __stdcall Color4usv (const UShort *v);
	void __stdcall ColorMask (Boolean red, Boolean green, Boolean blue, Boolean alpha);
	void __stdcall ColorMaterial (Enum face, Enum mode);
	void __stdcall ColorPointer (Int size, Enum type, Sizei stride, const Void *pointer);
	void __stdcall CopyPixels (Int x, Int y, Sizei width, Sizei height, Enum type);
	void __stdcall CopyTexImage1D (Enum target, Int level, Enum internalFormat, Int x, Int y, Sizei width, Int border);
	void __stdcall CopyTexImage2D (Enum target, Int level, Enum internalFormat, Int x, Int y, Sizei width, Sizei height, Int border);
	void __stdcall CopyTexSubImage1D (Enum target, Int level, Int xoffset, Int x, Int y, Sizei width);
	void __stdcall CopyTexSubImage2D (Enum target, Int level, Int xoffset, Int yoffset, Int x, Int y, Sizei width, Sizei height);
	void __stdcall CullFace (Enum mode);
	void __stdcall DeleteLists (UInt list, Sizei range);
	void __stdcall DeleteTextures (Sizei n, const UInt *textures);
	void __stdcall DepthFunc (Enum func);
	void __stdcall DepthMask (Boolean flag);
	void __stdcall DepthRange (ClampD zNear, ClampD zFar);
	void __stdcall Disable (Enum cap);
	void __stdcall DisableClientState (Enum array);
	void __stdcall DrawArrays (Enum mode, Int first, Sizei count);
	void __stdcall DrawBuffer (Enum mode);
	void __stdcall DrawElements (Enum mode, Sizei count, Enum type, const Void *indices);
	void __stdcall DrawPixels (Sizei width, Sizei height, Enum format, Enum type, const Void *pixels);
	void __stdcall EdgeFlag (Boolean flag);
	void __stdcall EdgeFlagPointer (Sizei stride, const Void *pointer);
	void __stdcall EdgeFlagv (const Boolean *flag);
	void __stdcall Enable (Enum cap);
	void __stdcall EnableClientState (Enum array);
	void __stdcall End (void);
	void __stdcall EndList (void);
	void __stdcall EvalCoord1d (Double u);
	void __stdcall EvalCoord1dv (const Double *u);
	void __stdcall EvalCoord1f (Float u);
	void __stdcall EvalCoord1fv (const Float *u);
	void __stdcall EvalCoord2d (Double u, Double v);
	void __stdcall EvalCoord2dv (const Double *u);
	void __stdcall EvalCoord2f (Float u, Float v);
	void __stdcall EvalCoord2fv (const Float *u);
	void __stdcall EvalMesh1 (Enum mode, Int i1, Int i2);
	void __stdcall EvalMesh2 (Enum mode, Int i1, Int i2, Int j1, Int j2);
	void __stdcall EvalPoint1 (Int i);
	void __stdcall EvalPoint2 (Int i, Int j);
	void __stdcall FeedbackBuffer (Sizei size, Enum type, Float *buffer);
	void __stdcall Finish (void);
	void __stdcall Flush (void);
	void __stdcall Fogf (Enum pname, Float param);
	void __stdcall Fogfv (Enum pname, const Float *params);
	void __stdcall Fogi (Enum pname, Int param);
	void __stdcall Fogiv (Enum pname, const Int *params);
	void __stdcall FrontFace (Enum mode);
	void __stdcall Frustum (Double left, Double right, Double bottom, Double top, Double zNear, Double zFar);
	UInt __stdcall GenLists (Sizei range);
	void __stdcall GenTextures (Sizei n, UInt *textures);
	void __stdcall GetBooleanv (Enum pname, Boolean *params);
	void __stdcall GetClipPlane (Enum plane, Double *equation);
	void __stdcall GetDoublev (Enum pname, Double *params);
	Enum __stdcall GetError (void);
	void __stdcall GetFloatv (Enum pname, Float *params);
	void __stdcall GetIntegerv (Enum pname, Int *params);
	void __stdcall GetLightfv (Enum light, Enum pname, Float *params);
	void __stdcall GetLightiv (Enum light, Enum pname, Int *params);
	void __stdcall GetMapdv (Enum target, Enum query, Double *v);
	void __stdcall GetMapfv (Enum target, Enum query, Float *v);
	void __stdcall GetMapiv (Enum target, Enum query, Int *v);
	void __stdcall GetMaterialfv (Enum face, Enum pname, Float *params);
	void __stdcall GetMaterialiv (Enum face, Enum pname, Int *params);
	void __stdcall GetPixelMapfv (Enum map, Float *values);
	void __stdcall GetPixelMapuiv (Enum map, UInt *values);
	void __stdcall GetPixelMapusv (Enum map, UShort *values);
	void __stdcall GetPointerv (Enum pname, Void* *params);
	void __stdcall GetPolygonStipple (UByte *mask);
	const UByte * __stdcall GetString (Enum name);
	void __stdcall GetTexEnvfv (Enum target, Enum pname, Float *params);
	void __stdcall GetTexEnviv (Enum target, Enum pname, Int *params);
	void __stdcall GetTexGendv (Enum coord, Enum pname, Double *params);
	void __stdcall GetTexGenfv (Enum coord, Enum pname, Float *params);
	void __stdcall GetTexGeniv (Enum coord, Enum pname, Int *params);
	void __stdcall GetTexImage (Enum target, Int level, Enum format, Enum type, Void *pixels);
	void __stdcall GetTexLevelParameterfv (Enum target, Int level, Enum pname, Float *params);
	void __stdcall GetTexLevelParameteriv (Enum target, Int level, Enum pname, Int *params);
	void __stdcall GetTexParameterfv (Enum target, Enum pname, Float *params);
	void __stdcall GetTexParameteriv (Enum target, Enum pname, Int *params);
	void __stdcall Hint (Enum target, Enum mode);
	void __stdcall IndexMask (UInt mask);
	void __stdcall IndexPointer (Enum type, Sizei stride, const Void *pointer);
	void __stdcall Indexd (Double c);
	void __stdcall Indexdv (const Double *c);
	void __stdcall Indexf (Float c);
	void __stdcall Indexfv (const Float *c);
	void __stdcall Indexi (Int c);
	void __stdcall Indexiv (const Int *c);
	void __stdcall Indexs (Short c);
	void __stdcall Indexsv (const Short *c);
	void __stdcall Indexub (UByte c);
	void __stdcall Indexubv (const UByte *c);
	void __stdcall InitNames (void);
	void __stdcall InterleavedArrays (Enum format, Sizei stride, const Void *pointer);
	Boolean __stdcall IsEnabled (Enum cap);
	Boolean __stdcall IsList (UInt list);
	Boolean __stdcall IsTexture (UInt texture);
	void __stdcall LightModelf (Enum pname, Float param);
	void __stdcall LightModelfv (Enum pname, const Float *params);
	void __stdcall LightModeli (Enum pname, Int param);
	void __stdcall LightModeliv (Enum pname, const Int *params);
	void __stdcall Lightf (Enum light, Enum pname, Float param);
	void __stdcall Lightfv (Enum light, Enum pname, const Float *params);
	void __stdcall Lighti (Enum light, Enum pname, Int param);
	void __stdcall Lightiv (Enum light, Enum pname, const Int *params);
	void __stdcall LineStipple (Int factor, UShort pattern);
	void __stdcall LineWidth (Float width);
	void __stdcall ListBase (UInt base);
	void __stdcall LoadIdentity (void);
	void __stdcall LoadMatrixd (const Double *m);
	void __stdcall LoadMatrixf (const Float *m);
	void __stdcall LoadName (UInt name);
	void __stdcall LogicOp (Enum opcode);
	void __stdcall Map1d (Enum target, Double u1, Double u2, Int stride, Int order, const Double *points);
	void __stdcall Map1f (Enum target, Float u1, Float u2, Int stride, Int order, const Float *points);
	void __stdcall Map2d (Enum target, Double u1, Double u2, Int ustride, Int uorder, Double v1, Double v2, Int vstride, Int vorder, const Double *points);
	void __stdcall Map2f (Enum target, Float u1, Float u2, Int ustride, Int uorder, Float v1, Float v2, Int vstride, Int vorder, const Float *points);
	void __stdcall MapGrid1d (Int un, Double u1, Double u2);
	void __stdcall MapGrid1f (Int un, Float u1, Float u2);
	void __stdcall MapGrid2d (Int un, Double u1, Double u2, Int vn, Double v1, Double v2);
	void __stdcall MapGrid2f (Int un, Float u1, Float u2, Int vn, Float v1, Float v2);
	void __stdcall Materialf (Enum face, Enum pname, Float param);
	void __stdcall Materialfv (Enum face, Enum pname, const Float *params);
	void __stdcall Materiali (Enum face, Enum pname, Int param);
	void __stdcall Materialiv (Enum face, Enum pname, const Int *params);
	void __stdcall MatrixMode (Enum mode);
	void __stdcall MultMatrixd (const Double *m);
	void __stdcall MultMatrixf (const Float *m);
	void __stdcall NewList (UInt list, Enum mode);
	void __stdcall Normal3b (Byte nx, Byte ny, Byte nz);
	void __stdcall Normal3bv (const Byte *v);
	void __stdcall Normal3d (Double nx, Double ny, Double nz);
	void __stdcall Normal3dv (const Double *v);
	void __stdcall Normal3f (Float nx, Float ny, Float nz);
	void __stdcall Normal3fv (const Float *v);
	void __stdcall Normal3i (Int nx, Int ny, Int nz);
	void __stdcall Normal3iv (const Int *v);
	void __stdcall Normal3s (Short nx, Short ny, Short nz);
	void __stdcall Normal3sv (const Short *v);
	void __stdcall NormalPointer (Enum type, Sizei stride, const Void *pointer);
	void __stdcall Ortho (Double left, Double right, Double bottom, Double top, Double zNear, Double zFar);
	void __stdcall PassThrough (Float token);
	void __stdcall PixelMapfv (Enum map, Sizei mapsize, const Float *values);
	void __stdcall PixelMapuiv (Enum map, Sizei mapsize, const UInt *values);
	void __stdcall PixelMapusv (Enum map, Sizei mapsize, const UShort *values);
	void __stdcall PixelStoref (Enum pname, Float param);
	void __stdcall PixelStorei (Enum pname, Int param);
	void __stdcall PixelTransferf (Enum pname, Float param);
	void __stdcall PixelTransferi (Enum pname, Int param);
	void __stdcall PixelZoom (Float xfactor, Float yfactor);
	void __stdcall PointSize (Float size);
	void __stdcall PolygonMode (Enum face, Enum mode);
	void __stdcall PolygonOffset (Float factor, Float units);
	void __stdcall PolygonStipple (const UByte *mask);
	void __stdcall PopAttrib (void);
	void __stdcall PopClientAttrib (void);
	void __stdcall PopMatrix (void);
	void __stdcall PopName (void);
	void __stdcall PrioritizeTextures (Sizei n, const UInt *textures, const ClampF *priorities);
	void __stdcall PushAttrib (Bitfield mask);
	void __stdcall PushClientAttrib (Bitfield mask);
	void __stdcall PushMatrix (void);
	void __stdcall PushName (UInt name);
	void __stdcall RasterPos2d (Double x, Double y);
	void __stdcall RasterPos2dv (const Double *v);
	void __stdcall RasterPos2f (Float x, Float y);
	void __stdcall RasterPos2fv (const Float *v);
	void __stdcall RasterPos2i (Int x, Int y);
	void __stdcall RasterPos2iv (const Int *v);
	void __stdcall RasterPos2s (Short x, Short y);
	void __stdcall RasterPos2sv (const Short *v);
	void __stdcall RasterPos3d (Double x, Double y, Double z);
	void __stdcall RasterPos3dv (const Double *v);
	void __stdcall RasterPos3f (Float x, Float y, Float z);
	void __stdcall RasterPos3fv (const Float *v);
	void __stdcall RasterPos3i (Int x, Int y, Int z);
	void __stdcall RasterPos3iv (const Int *v);
	void __stdcall RasterPos3s (Short x, Short y, Short z);
	void __stdcall RasterPos3sv (const Short *v);
	void __stdcall RasterPos4d (Double x, Double y, Double z, Double w);
	void __stdcall RasterPos4dv (const Double *v);
	void __stdcall RasterPos4f (Float x, Float y, Float z, Float w);
	void __stdcall RasterPos4fv (const Float *v);
	void __stdcall RasterPos4i (Int x, Int y, Int z, Int w);
	void __stdcall RasterPos4iv (const Int *v);
	void __stdcall RasterPos4s (Short x, Short y, Short z, Short w);
	void __stdcall RasterPos4sv (const Short *v);
	void __stdcall ReadBuffer (Enum mode);
	void __stdcall ReadPixels (Int x, Int y, Sizei width, Sizei height, Enum format, Enum type, Void *pixels);
	void __stdcall Rectd (Double x1, Double y1, Double x2, Double y2);
	void __stdcall Rectdv (const Double *v1, const Double *v2);
	void __stdcall Rectf (Float x1, Float y1, Float x2, Float y2);
	void __stdcall Rectfv (const Float *v1, const Float *v2);
	void __stdcall Recti (Int x1, Int y1, Int x2, Int y2);
	void __stdcall Rectiv (const Int *v1, const Int *v2);
	void __stdcall Rects (Short x1, Short y1, Short x2, Short y2);
	void __stdcall Rectsv (const Short *v1, const Short *v2);
	Int __stdcall RenderMode (Enum mode);
	void __stdcall Rotated (Double angle, Double x, Double y, Double z);
	void __stdcall Rotatef (Float angle, Float x, Float y, Float z);
	void __stdcall Scaled (Double x, Double y, Double z);
	void __stdcall Scalef (Float x, Float y, Float z);
	void __stdcall Scissor (Int x, Int y, Sizei width, Sizei height);
	void __stdcall SelectBuffer (Sizei size, UInt *buffer);
	void __stdcall ShadeModel (Enum mode);
	void __stdcall StencilFunc (Enum func, Int ref, UInt mask);
	void __stdcall StencilMask (UInt mask);
	void __stdcall StencilOp (Enum fail, Enum zfail, Enum zpass);
	void __stdcall TexCoord1d (Double s);
	void __stdcall TexCoord1dv (const Double *v);
	void __stdcall TexCoord1f (Float s);
	void __stdcall TexCoord1fv (const Float *v);
	void __stdcall TexCoord1i (Int s);
	void __stdcall TexCoord1iv (const Int *v);
	void __stdcall TexCoord1s (Short s);
	void __stdcall TexCoord1sv (const Short *v);
	void __stdcall TexCoord2d (Double s, Double t);
	void __stdcall TexCoord2dv (const Double *v);
	void __stdcall TexCoord2f (Float s, Float t);
	void __stdcall TexCoord2fv (const Float *v);
	void __stdcall TexCoord2i (Int s, Int t);
	void __stdcall TexCoord2iv (const Int *v);
	void __stdcall TexCoord2s (Short s, Short t);
	void __stdcall TexCoord2sv (const Short *v);
	void __stdcall TexCoord3d (Double s, Double t, Double r);
	void __stdcall TexCoord3dv (const Double *v);
	void __stdcall TexCoord3f (Float s, Float t, Float r);
	void __stdcall TexCoord3fv (const Float *v);
	void __stdcall TexCoord3i (Int s, Int t, Int r);
	void __stdcall TexCoord3iv (const Int *v);
	void __stdcall TexCoord3s (Short s, Short t, Short r);
	void __stdcall TexCoord3sv (const Short *v);
	void __stdcall TexCoord4d (Double s, Double t, Double r, Double q);
	void __stdcall TexCoord4dv (const Double *v);
	void __stdcall TexCoord4f (Float s, Float t, Float r, Float q);
	void __stdcall TexCoord4fv (const Float *v);
	void __stdcall TexCoord4i (Int s, Int t, Int r, Int q);
	void __stdcall TexCoord4iv (const Int *v);
	void __stdcall TexCoord4s (Short s, Short t, Short r, Short q);
	void __stdcall TexCoord4sv (const Short *v);
	void __stdcall TexCoordPointer (Int size, Enum type, Sizei stride, const Void *pointer);
	void __stdcall TexEnvf (Enum target, Enum pname, Float param);
	void __stdcall TexEnvfv (Enum target, Enum pname, const Float *params);
	void __stdcall TexEnvi (Enum target, Enum pname, Int param);
	void __stdcall TexEnviv (Enum target, Enum pname, const Int *params);
	void __stdcall TexGend (Enum coord, Enum pname, Double param);
	void __stdcall TexGendv (Enum coord, Enum pname, const Double *params);
	void __stdcall TexGenf (Enum coord, Enum pname, Float param);
	void __stdcall TexGenfv (Enum coord, Enum pname, const Float *params);
	void __stdcall TexGeni (Enum coord, Enum pname, Int param);
	void __stdcall TexGeniv (Enum coord, Enum pname, const Int *params);
	void __stdcall TexImage1D (Enum target, Int level, Int internalformat, Sizei width, Int border, Enum format, Enum type, const Void *pixels);
	void __stdcall TexImage2D (Enum target, Int level, Int internalformat, Sizei width, Sizei height, Int border, Enum format, Enum type, const Void *pixels);
	void __stdcall TexParameterf (Enum target, Enum pname, Float param);
	void __stdcall TexParameterfv (Enum target, Enum pname, const Float *params);
	void __stdcall TexParameteri (Enum target, Enum pname, Int param);
	void __stdcall TexParameteriv (Enum target, Enum pname, const Int *params);
	void __stdcall TexSubImage1D (Enum target, Int level, Int xoffset, Sizei width, Enum format, Enum type, const Void *pixels);
	void __stdcall TexSubImage2D (Enum target, Int level, Int xoffset, Int yoffset, Sizei width, Sizei height, Enum format, Enum type, const Void *pixels);
	void __stdcall Translated (Double x, Double y, Double z);
	void __stdcall Translatef (Float x, Float y, Float z);
	void __stdcall Vertex2d (Double x, Double y);
	void __stdcall Vertex2dv (const Double *v);
	void __stdcall Vertex2f (Float x, Float y);
	void __stdcall Vertex2fv (const Float *v);
	void __stdcall Vertex2i (Int x, Int y);
	void __stdcall Vertex2iv (const Int *v);
	void __stdcall Vertex2s (Short x, Short y);
	void __stdcall Vertex2sv (const Short *v);
	void __stdcall Vertex3d (Double x, Double y, Double z);
	void __stdcall Vertex3dv (const Double *v);
	void __stdcall Vertex3f (Float x, Float y, Float z);
	void __stdcall Vertex3fv (const Float *v);
	void __stdcall Vertex3i (Int x, Int y, Int z);
	void __stdcall Vertex3iv (const Int *v);
	void __stdcall Vertex3s (Short x, Short y, Short z);
	void __stdcall Vertex3sv (const Short *v);
	void __stdcall Vertex4d (Double x, Double y, Double z, Double w);
	void __stdcall Vertex4dv (const Double *v);
	void __stdcall Vertex4f (Float x, Float y, Float z, Float w);
	void __stdcall Vertex4fv (const Float *v);
	void __stdcall Vertex4i (Int x, Int y, Int z, Int w);
	void __stdcall Vertex4iv (const Int *v);
	void __stdcall Vertex4s (Short x, Short y, Short z, Short w);
	void __stdcall Vertex4sv (const Short *v);
	void __stdcall VertexPointer (Int size, Enum type, Sizei stride, const Void *pointer);
	void __stdcall Viewport (Int x, Int y, Sizei width, Sizei height);

	/* EXT_vertex_array */
	void __stdcall ArrayElementEXT(Int i);
	void __stdcall DrawArraysEXT(Enum mode, Int first, Sizei count);
	void __stdcall VertexPointerEXT(Int size, Enum type, Sizei stride, Sizei count, const Void *pointer);
	void __stdcall NormalPointerEXT(Enum type, Sizei stride, Sizei count, const Void *pointer);
	void __stdcall ColorPointerEXT(Int size, Enum type, Sizei stride, Sizei count, const Void *pointer);
	void __stdcall IndexPointerEXT(Enum type, Sizei stride, Sizei count, const Void *pointer);
	void __stdcall TexCoordPointerEXT(Int size, Enum type, Sizei stride, Sizei count, const Void *pointer);
	void __stdcall EdgeFlagPointerEXT(Sizei stride, Sizei count, const Boolean *pointer);
	void __stdcall GetPointervEXT(Enum pname, Void* *params);
	void __stdcall ArrayElementArrayEXT(Enum mode, Sizei count, const Void* pi);

	/* WIN_draw_range_elements */
	void __stdcall DrawRangeElementsWIN(Enum mode, UInt start, UInt end, Sizei count, Enum type, const Void *indices);

	/* WIN_swap_hint */
	void __stdcall AddSwapHintRectWIN (Int x, Int y, Sizei width, Sizei height);

	/* EXT_paletted_texture */
	void __stdcall ColorTableEXT
		(Enum target, Enum internalFormat, Sizei width, Enum format,
		Enum type, const Void *data);
	void __stdcall ColorSubTableEXT
		(Enum target, Sizei start, Sizei count, Enum format,
		Enum type, const Void *data);
	void __stdcall GetColorTableEXT
		(Enum target, Enum format, Enum type, Void *data);
	void __stdcall GetColorTableParameterivEXT
		(Enum target, Enum pname, Int *params);
	void __stdcall getColorTableParameterfvEXT
		(Enum target, Enum pname, Float *params);

} // namespace gl

#endif // __DOPENGL_H