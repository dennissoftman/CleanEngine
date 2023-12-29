#ifndef W3D_DEFINES_HPP
#define W3D_DEFINES_HPP

#define W3D_MAKE_VERSION(major,minor)		                    (((major) << 16) | (minor))
#define W3D_GET_MAJOR_VERSION(ver)			                    ((ver)>>16)
#define W3D_GET_MINOR_VERSION(ver)			                    ((ver) & 0xFFFF)
#define W3D_NAME_LEN                                            (16)

#include <cstddef>

/////////////////////////////////////////////////////////////////////////////////////////////
// MATERIALS
//
// Surrender 1.40 significantly changed the way that materials are described. To 
// accomodate this, the w3d file format has changed since there are new features and 
// optimizations that we want to take advangage of. 
// 
// The VertexMaterial defines parameters which control the calculation of the primary
// and secondary gradients. The shader defines how those gradients are combined with
// the texel and the frame buffer contents.
//
/////////////////////////////////////////////////////////////////////////////////////////////
#define		W3DVERTMAT_USE_DEPTH_CUE							0x00000001
#define		W3DVERTMAT_ARGB_EMISSIVE_ONLY						0x00000002
#define		W3DVERTMAT_COPY_SPECULAR_TO_DIFFUSE					0x00000004
#define		W3DVERTMAT_DEPTH_CUE_TO_ALPHA						0x00000008

#define		W3DVERTMAT_STAGE0_MAPPING_MASK						0x00FF0000
#define		W3DVERTMAT_STAGE0_MAPPING_UV						0x00000000
#define		W3DVERTMAT_STAGE0_MAPPING_ENVIRONMENT				0x00010000
#define		W3DVERTMAT_STAGE0_MAPPING_CHEAP_ENVIRONMENT		    0x00020000
#define		W3DVERTMAT_STAGE0_MAPPING_SCREEN					0x00030000
#define		W3DVERTMAT_STAGE0_MAPPING_LINEAR_OFFSET			    0x00040000
#define		W3DVERTMAT_STAGE0_MAPPING_SILHOUETTE				0x00050000
#define		W3DVERTMAT_STAGE0_MAPPING_SCALE						0x00060000
#define		W3DVERTMAT_STAGE0_MAPPING_GRID						0x00070000
#define		W3DVERTMAT_STAGE0_MAPPING_ROTATE					0x00080000
#define		W3DVERTMAT_STAGE0_MAPPING_SINE_LINEAR_OFFSET		0x00090000
#define		W3DVERTMAT_STAGE0_MAPPING_STEP_LINEAR_OFFSET		0x000A0000
#define		W3DVERTMAT_STAGE0_MAPPING_ZIGZAG_LINEAR_OFFSET	    0x000B0000
#define		W3DVERTMAT_STAGE0_MAPPING_WS_CLASSIC_ENV			0x000C0000
#define		W3DVERTMAT_STAGE0_MAPPING_WS_ENVIRONMENT			0x000D0000
#define		W3DVERTMAT_STAGE0_MAPPING_GRID_CLASSIC_ENV		    0x000E0000
#define		W3DVERTMAT_STAGE0_MAPPING_GRID_ENVIRONMENT		    0x000F0000
#define		W3DVERTMAT_STAGE0_MAPPING_RANDOM					0x00100000
#define		W3DVERTMAT_STAGE0_MAPPING_EDGE						0x00110000
#define		W3DVERTMAT_STAGE0_MAPPING_BUMPENV					0x00120000

#define		W3DVERTMAT_STAGE1_MAPPING_MASK						0x0000FF00
#define		W3DVERTMAT_STAGE1_MAPPING_UV						0x00000000
#define		W3DVERTMAT_STAGE1_MAPPING_ENVIRONMENT				0x00000100
#define		W3DVERTMAT_STAGE1_MAPPING_CHEAP_ENVIRONMENT		    0x00000200
#define		W3DVERTMAT_STAGE1_MAPPING_SCREEN					0x00000300
#define		W3DVERTMAT_STAGE1_MAPPING_LINEAR_OFFSET			    0x00000400
#define		W3DVERTMAT_STAGE1_MAPPING_SILHOUETTE				0x00000500
#define		W3DVERTMAT_STAGE1_MAPPING_SCALE						0x00000600
#define		W3DVERTMAT_STAGE1_MAPPING_GRID						0x00000700
#define		W3DVERTMAT_STAGE1_MAPPING_ROTATE					0x00000800
#define		W3DVERTMAT_STAGE1_MAPPING_SINE_LINEAR_OFFSET		0x00000900
#define		W3DVERTMAT_STAGE1_MAPPING_STEP_LINEAR_OFFSET		0x00000A00
#define		W3DVERTMAT_STAGE1_MAPPING_ZIGZAG_LINEAR_OFFSET	    0x00000B00
#define		W3DVERTMAT_STAGE1_MAPPING_WS_CLASSIC_ENV			0x00000C00
#define		W3DVERTMAT_STAGE1_MAPPING_WS_ENVIRONMENT			0x00000D00
#define		W3DVERTMAT_STAGE1_MAPPING_GRID_CLASSIC_ENV		    0x00000E00
#define		W3DVERTMAT_STAGE1_MAPPING_GRID_ENVIRONMENT		    0x00000F00
#define		W3DVERTMAT_STAGE1_MAPPING_RANDOM					0x00001000
#define		W3DVERTMAT_STAGE1_MAPPING_EDGE						0x00001100
#define		W3DVERTMAT_STAGE1_MAPPING_BUMPENV					0x00001200

#define		W3DVERTMAT_PSX_MASK									0xFF000000
#define		W3DVERTMAT_PSX_TRANS_MASK 							0x07000000
#define		W3DVERTMAT_PSX_TRANS_NONE 							0x00000000
#define		W3DVERTMAT_PSX_TRANS_100 							0x01000000
#define		W3DVERTMAT_PSX_TRANS_50 							0x02000000
#define		W3DVERTMAT_PSX_TRANS_25 							0x03000000
#define		W3DVERTMAT_PSX_TRANS_MINUS_100 						0x04000000
#define		W3DVERTMAT_PSX_NO_RT_LIGHTING 						0x08000000

/////////////////////////////////////////////////////////////////////////////////////////////
// Texture Animation parameters
// May occur inside a texture chunk if its needed
/////////////////////////////////////////////////////////////////////////////////////////////
#define W3DTEXTURE_PUBLISH					                    0x0001		// this texture should be "published" (indirected so its changeable in code)
#define W3DTEXTURE_RESIZE_OBSOLETE		                        0x0002		// this texture should be resizeable (OBSOLETE!!!)
#define W3DTEXTURE_NO_LOD					                    0x0004		// this texture should not have any LOD (mipmapping or resizing)
#define W3DTEXTURE_CLAMP_U					                    0x0008		// this texture should be clamped on U
#define W3DTEXTURE_CLAMP_V					                    0x0010		// this texture should be clamped on V
#define W3DTEXTURE_ALPHA_BITMAP			                        0x0020		// this texture's alpha channel should be collapsed to one bit

// Specify desired no. of mip-levels to be generated.
#define W3DTEXTURE_MIP_LEVELS_MASK		                        0x00c0
#define W3DTEXTURE_MIP_LEVELS_ALL		                        0x0000		// generate all mip-levels
#define W3DTEXTURE_MIP_LEVELS_2			                        0x0040		// generate up to 2 mip-levels (NOTE: use W3DTEXTURE_NO_LOD to generate just 1 mip-level)
#define W3DTEXTURE_MIP_LEVELS_3			                        0x0080		// generate up to 3 mip-levels
#define W3DTEXTURE_MIP_LEVELS_4			                        0x00c0		// generate up to 4 mip-levels

// Hints to describe the intended use of the various passes / stages
// This will go into the high byte of Attributes.
#define W3DTEXTURE_HINT_SHIFT				                    8				// number of bits to shift up
#define W3DTEXTURE_HINT_MASK				                    0x0000ff00	// mask for shifted hint value

#define W3DTEXTURE_HINT_BASE				                    0x0000		// base texture
#define W3DTEXTURE_HINT_EMISSIVE			                    0x0100		// emissive map
#define W3DTEXTURE_HINT_ENVIRONMENT		                        0x0200		// environment/reflection map
#define W3DTEXTURE_HINT_SHINY_MASK		                        0x0300		// shinyness mask map

#define W3DTEXTURE_TYPE_MASK				                    0x1000	
#define W3DTEXTURE_TYPE_COLORMAP			                    0x0000		// Color map.
#define W3DTEXTURE_TYPE_BUMPMAP			                        0x1000		// Grayscale heightmap (to be converted to bumpmap).

// Animation types
#define W3DTEXTURE_ANIM_LOOP				                    0x0000
#define W3DTEXTURE_ANIM_PINGPONG			                    0x0001
#define W3DTEXTURE_ANIM_ONCE				                    0x0002
#define W3DTEXTURE_ANIM_MANUAL			                        0x0003

/////////////////////////////////////////////////////////////////////////////////////////////
// Flags for the Mesh Attributes member
/////////////////////////////////////////////////////////////////////////////////////////////
#define W3D_MESH_FLAG_NONE										0x00000000		// plain ole normal mesh
#define W3D_MESH_FLAG_COLLISION_BOX							    0x00000001		// (obsolete as of 4.1) mesh is a collision box (should be 8 verts, should be hidden, etc)
#define W3D_MESH_FLAG_SKIN										0x00000002		// (obsolete as of 4.1) skin mesh 
#define W3D_MESH_FLAG_SHADOW									0x00000004		// (obsolete as of 4.1) intended to be projected as a shadow
#define W3D_MESH_FLAG_ALIGNED									0x00000008		// (obsolete as of 4.1) always aligns with camera

#define W3D_MESH_FLAG_COLLISION_TYPE_MASK					    0x00000FF0		// mask for the collision type bits
#define W3D_MESH_FLAG_COLLISION_TYPE_SHIFT						4		        // shifting to get to the collision type bits
#define W3D_MESH_FLAG_COLLISION_TYPE_PHYSICAL			        0x00000010      // physical collisions
#define W3D_MESH_FLAG_COLLISION_TYPE_PROJECTILE			        0x00000020     	// projectiles (rays) collide with this
#define W3D_MESH_FLAG_COLLISION_TYPE_VIS					    0x00000040	    // vis rays collide with this mesh
#define W3D_MESH_FLAG_COLLISION_TYPE_CAMERA				        0x00000080      // camera rays/boxes collide with this mesh
#define W3D_MESH_FLAG_COLLISION_TYPE_VEHICLE				    0x00000100	    // vehicles collide with this mesh (and with physical collision meshes)

#define W3D_MESH_FLAG_HIDDEN									0x00001000		// this mesh is hidden by default
#define W3D_MESH_FLAG_TWO_SIDED								    0x00002000   	// render both sides of this mesh
#define OBSOLETE_W3D_MESH_FLAG_LIGHTMAPPED				        0x00004000      // obsolete lightmapped mesh
																				// NOTE: retained for backwards compatibility - use W3D_MESH_FLAG_PRELIT_* instead.
#define W3D_MESH_FLAG_CAST_SHADOW							    0x00008000	    // this mesh casts shadows

#define W3D_MESH_FLAG_GEOMETRY_TYPE_MASK					    0x00FF0000		// (introduced with 4.1)
#define W3D_MESH_FLAG_GEOMETRY_TYPE_NORMAL				        0x00000000		// (4.1+) normal mesh geometry
#define W3D_MESH_FLAG_GEOMETRY_TYPE_CAMERA_ALIGNED		        0x00010000		// (4.1+) camera aligned mesh
#define W3D_MESH_FLAG_GEOMETRY_TYPE_SKIN					    0x00020000		// (4.1+) skin mesh
#define OBSOLETE_W3D_MESH_FLAG_GEOMETRY_TYPE_SHADOW	            0x00030000		// (4.1+) shadow mesh OBSOLETE!
#define W3D_MESH_FLAG_GEOMETRY_TYPE_AABOX					    0x00040000		// (4.1+) aabox OBSOLETE!
#define W3D_MESH_FLAG_GEOMETRY_TYPE_OBBOX					    0x00050000		// (4.1+) obbox OBSOLETE!
#define W3D_MESH_FLAG_GEOMETRY_TYPE_CAMERA_ORIENTED	            0x00060000		// (4.1+) camera oriented mesh (points _towards_ camera)

#define W3D_MESH_FLAG_PRELIT_MASK							    0x0F000000		// (4.2+) 
#define W3D_MESH_FLAG_PRELIT_UNLIT							    0x01000000		// mesh contains an unlit material chunk wrapper
#define W3D_MESH_FLAG_PRELIT_VERTEX							    0x02000000		// mesh contains a precalculated vertex-lit material chunk wrapper 
#define W3D_MESH_FLAG_PRELIT_LIGHTMAP_MULTI_PASS		        0x04000000		// mesh contains a precalculated multi-pass lightmapped material chunk wrapper
#define W3D_MESH_FLAG_PRELIT_LIGHTMAP_MULTI_TEXTURE             0x08000000		// mesh contains a precalculated multi-texture lightmapped material chunk wrapper

#define W3D_MESH_FLAG_SHATTERABLE							    0x10000000		// this mesh is shatterable.
#define W3D_MESH_FLAG_NPATCHABLE								0x20000000		// it is ok to NPatch this mesh

/********************************************************************************

	Meshes

	Version 3 Mesh Header, trimmed out some of the junk that was in the
	previous versions. 

********************************************************************************/

#define W3D_CURRENT_MESH_VERSION		                        W3D_MAKE_VERSION(4,2)

#define W3D_VERTEX_CHANNEL_LOCATION		                        0x00000001	    // object-space location of the vertex
#define W3D_VERTEX_CHANNEL_NORMAL		                        0x00000002	    // object-space normal for the vertex
#define W3D_VERTEX_CHANNEL_TEXCOORD		                        0x00000004	    // texture coordinate
#define W3D_VERTEX_CHANNEL_COLOR		                        0x00000008	    // vertex color
#define W3D_VERTEX_CHANNEL_BONEID		                        0x00000010	    // per-vertex bone id for skins

#define W3D_FACE_CHANNEL_FACE				                    0x00000001	    // basic face info, W3dTriStruct...

// boundary values for W3dMeshHeaderStruct::SortLevel
#define SORT_LEVEL_NONE						                    0
#define MAX_SORT_LEVEL						                    32
#define SORT_LEVEL_BIN1						                    20
#define SORT_LEVEL_BIN2						                    15
#define SORT_LEVEL_BIN3						                    10

#define W3D_DEFORM_SET_MANUAL_DEFORM	                        0x00000001	    // set is isn't applied during sphere or point tests.

#define W3D_CURRENT_HTREE_VERSION								W3D_MAKE_VERSION(4,1)

#define W3D_CURRENT_HANIM_VERSION								W3D_MAKE_VERSION(4,1)
#define W3D_CURRENT_COMPRESSED_HANIM_VERSION					W3D_MAKE_VERSION(0,1)
#define W3D_CURRENT_MORPH_HANIM_VERSION							W3D_MAKE_VERSION(0,1)

#define W3D_TIMECODED_BINARY_MOVEMENT_FLAG 						0x80000000

// The bit channel is encoded right into the MSB of each time code
#define W3D_TIMECODED_BIT_MASK									0x80000000

#define W3D_CURRENT_HMODEL_VERSION								W3D_MAKE_VERSION(4,2)

#define W3D_CURRENT_LIGHT_VERSION								W3D_MAKE_VERSION(1,0)

#define W3D_LIGHT_ATTRIBUTE_TYPE_MASK							0x000000FF
#define W3D_LIGHT_ATTRIBUTE_POINT								0x00000001
#define W3D_LIGHT_ATTRIBUTE_DIRECTIONAL							0x00000002
#define W3D_LIGHT_ATTRIBUTE_SPOT								0x00000003
#define W3D_LIGHT_ATTRIBUTE_CAST_SHADOWS						0x00000100

#define W3D_CURRENT_EMITTER_VERSION								0x00020000

#define W3D_EMITTER_RENDER_MODE_TRI_PARTICLES					0
#define W3D_EMITTER_RENDER_MODE_QUAD_PARTICLES					1
#define W3D_EMITTER_RENDER_MODE_LINE							2
#define W3D_EMITTER_RENDER_MODE_LINEGRP_TETRA					3
#define W3D_EMITTER_RENDER_MODE_LINEGRP_PRISM					4

#define W3D_EMITTER_FRAME_MODE_1x1								0
#define W3D_EMITTER_FRAME_MODE_2x2								1
#define W3D_EMITTER_FRAME_MODE_4x4								2
#define W3D_EMITTER_FRAME_MODE_8x8								3
#define W3D_EMITTER_FRAME_MODE_16x16							4

// W3D_CHUNK_EMITTER_LINE_PROPERTIES
// Contains a W3dEmitterLinePropertiesStruct.
// Emiter Line Flags (used in the Flags field of W3dEmitterLinePropertiesStruct):
#define W3D_ELINE_MERGE_INTERSECTIONS 							0x00000001	// Merge intersections
#define W3D_ELINE_FREEZE_RANDOM									0x00000002	// Freeze random (note: offsets are in camera space)
#define W3D_ELINE_DISABLE_SORTING								0x00000004	// Disable sorting (even if shader has alpha-blending)
#define W3D_ELINE_END_CAPS 										0x00000008	// Draw end caps on the line
#define W3D_ELINE_TEXTURE_MAP_MODE_MASK 						0xFF000000	// Must cover all possible TextureMapMode values

#define W3D_ELINE_TEXTURE_MAP_MODE_OFFSET 						24				// By how many bits do I need to shift the texture mapping mode?
#define W3D_ELINE_UNIFORM_WIDTH_TEXTURE_MAP						0x00000000	// Entire line uses one row of texture (constant V)
#define W3D_ELINE_UNIFORM_LENGTH_TEXTURE_MAP 					0x00000001	// Entire line uses one row of texture stretched length-wise
#define W3D_ELINE_TILED_TEXTURE_MAP								0x00000002	// Tiled continuously over line

#define W3D_ELINE_DEFAULT_BITS									(W3D_ELINE_MERGE_INTERSECTIONS | (W3D_ELINE_UNIFORM_WIDTH_TEXTURE_MAP << W3D_ELINE_TEXTURE_MAP_MODE_OFFSET))

#define W3D_CURRENT_AGGREGATE_VERSION							0x00010003
#define MESH_PATH_ENTRIES										15
#define MESH_PATH_ENTRY_LEN										(W3D_NAME_LEN * 2)

//
// Flags used in the W3dAggregateMiscInfo structure
//
#define W3D_AGGREGATE_FORCE_SUB_OBJ_LOD							0x00000001

#define W3D_CURRENT_HLOD_VERSION								W3D_MAKE_VERSION(1,0)
#define NO_MAX_SCREEN_SIZE										FLT_MAX

#define W3D_BOX_CURRENT_VERSION									W3D_MAKE_VERSION(1,0)

#define W3D_BOX_ATTRIBUTE_ORIENTED								0x00000001
#define W3D_BOX_ATTRIBUTE_ALIGNED								0x00000002
#define W3D_BOX_ATTRIBUTE_COLLISION_TYPE_MASK					0x00000FF0		// mask for the collision type bits
#define W3D_BOX_ATTRIBUTE_COLLISION_TYPE_SHIFT					4		// shifting to get to the collision type bits
#define W3D_BOX_ATTRIBTUE_COLLISION_TYPE_PHYSICAL				0x00000010		// physical collisions
#define W3D_BOX_ATTRIBTUE_COLLISION_TYPE_PROJECTILE				0x00000020		// projectiles (rays) collide with this
#define W3D_BOX_ATTRIBTUE_COLLISION_TYPE_VIS					0x00000040		// vis rays collide with this mesh
#define W3D_BOX_ATTRIBTUE_COLLISION_TYPE_CAMERA					0x00000080		// cameras collide with this mesh
#define W3D_BOX_ATTRIBTUE_COLLISION_TYPE_VEHICLE				0x00000100		// vehicles collide with this mesh

#define W3D_CURRENT_SOUNDROBJ_VERSION							0x00010000

#endif