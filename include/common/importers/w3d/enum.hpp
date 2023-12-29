#ifndef W3D_ENUM_HPP
#define W3D_ENUM_HPP

#include <cstdint>
#include <string>

enum class W3D_CHUNK : uint32_t
{
	eMESH									=0x00000000,	// Mesh definition 
		eVERTICES							=0x00000002,	// array of vertices (array of W3dVectorStruct's)
		eVERTEX_NORMALS						=0x00000003,	// array of normals (array of W3dVectorStruct's)
		eMESH_USER_TEXT						=0x0000000C,	// Text from the MAX comment field (Null terminated string)
		eVERTEX_INFLUENCES					=0x0000000E,	// Mesh Deformation vertex connections (array of W3dVertInfStruct's)
		eMESH_HEADER3						=0x0000001F,	//	mesh header contains general info about the mesh. (W3dMeshHeader3Struct)
		eTRIANGLES							=0x00000020,	// New improved triangles chunk (array of W3dTriangleStruct's)
		eVERTEX_SHADE_INDICES				=0x00000022,	// shade indexes for each vertex (array of uint32's)
		
		ePRELIT_UNLIT						=0x00000023,	// optional unlit material chunk wrapper
		ePRELIT_VERTEX						=0x00000024,	// optional vertex-lit material chunk wrapper
		ePRELIT_LIGHTMAP_MULTI_PASS		    =0x00000025,	// optional lightmapped multi-pass material chunk wrapper
		ePRELIT_LIGHTMAP_MULTI_TEXTURE	    =0x00000026,	// optional lightmapped multi-texture material chunk wrapper

			eMATERIAL_INFO					=0x00000028,	// materials information, pass count, etc (contains W3dMaterialInfoStruct)

			eSHADERS							=0x00000029,	// shaders (array of W3dShaderStruct's)
			
			eVERTEX_MATERIALS					=0x0000002A,	// wraps the vertex materials
				eVERTEX_MATERIAL				=0x0000002B,
					eVERTEX_MATERIAL_NAME	    =0x0000002C,	// vertex material name (NULL-terminated string)
					eVERTEX_MATERIAL_INFO	    =0x0000002D,	// W3dVertexMaterialStruct
					eVERTEX_MAPPER_ARGS0		=0x0000002E,	// Null-terminated string
					eVERTEX_MAPPER_ARGS1		=0x0000002F,	// Null-terminated string

			eTEXTURES							=0x00000030,	// wraps all of the texture info
				eTEXTURE						=0x00000031,	// wraps a texture definition
					eTEXTURE_NAME				=0x00000032,	// texture filename (NULL-terminated string)
					eTEXTURE_INFO				=0x00000033,	// optional W3dTextureInfoStruct
			
			eMATERIAL_PASS					=0x00000038,	// wraps the information for a single material pass
				eVERTEX_MATERIAL_IDS			=0x00000039,	// single or per-vertex array of uint32 vertex material indices (check chunk size)
				eSHADER_IDS					=0x0000003A,	// single or per-tri array of uint32 shader indices (check chunk size)
				eDCG							=0x0000003B,	// per-vertex diffuse color values (array of W3dRGBAStruct's)
				eDIG							=0x0000003C,	// per-vertex diffuse illumination values (array of W3dRGBStruct's)
				eSCG							=0x0000003E,	// per-vertex specular color values (array of W3dRGBStruct's)

				eTEXTURE_STAGE				=0x00000048,	// wrapper around a texture stage.
					eTEXTURE_IDS				=0x00000049,	// single or per-tri array of uint32 texture indices (check chunk size)
					eSTAGE_TEXCOORDS			=0x0000004A,	// per-vertex texture coordinates (array of W3dTexCoordStruct's)
					ePER_FACE_TEXCOORD_IDS	=0x0000004B,	// indices to W3D_CHUNK_STAGE_TEXCOORDS, (array of Vector3i)


		eDEFORM								=0x00000058,	// mesh deform or 'damage' information.
			eDEFORM_SET						=0x00000059,	// set of deform information
				eDEFORM_KEYFRAME				=0x0000005A,	// a keyframe of deform information in the set
					eDEFORM_DATA				=0x0000005B,	// deform information about a single vertex

		ePS2_SHADERS							=0x00000080,	// Shader info specific to the Playstation 2.
		
		eAABTREE								=0x00000090,	// Axis-Aligned Box Tree for hierarchical polygon culling
			eAABTREE_HEADER,										// catalog of the contents of the AABTree
			eAABTREE_POLYINDICES,								// array of uint32 polygon indices with count=mesh.PolyCount
			eAABTREE_NODES,										// array of W3dMeshAABTreeNode's with count=aabheader.NodeCount

	eHIERARCHY								=0x00000100,	// hierarchy tree definition
		eHIERARCHY_HEADER,
		ePIVOTS,
		ePIVOT_FIXUPS,												// only needed by the exporter...
	
	eANIMATION								=0x00000200,	// hierarchy animation data
		eANIMATION_HEADER,
		eANIMATION_CHANNEL,										// channel of vectors
		eBIT_CHANNEL,												// channel of boolean values (e.g. visibility)

	eCOMPRESSED_ANIMATION					    =0x00000280,	// compressed hierarchy animation data
		eCOMPRESSED_ANIMATION_HEADER,							// describes playback rate, number of frames, and type of compression
		eCOMPRESSED_ANIMATION_CHANNEL,						// compressed channel, format dependent on type of compression
		eCOMPRESSED_BIT_CHANNEL,								// compressed bit stream channel, format dependent on type of compression

	eMORPH_ANIMATION							=0x000002C0,	// hierarchy morphing animation data (morphs between poses, for facial animation)
		eMORPHANIM_HEADER,										// W3dMorphAnimHeaderStruct describes playback rate, number of frames, and type of compression
		eMORPHANIM_CHANNEL,										// wrapper for a channel
			eMORPHANIM_POSENAME,									// name of the other anim which contains the poses for this morph channel
			eMORPHANIM_KEYDATA,									// morph key data for this channel
		eMORPHANIM_PIVOTCHANNELDATA,							// uin32 per pivot in the htree, indicating which channel controls the pivot

	eHMODEL									=0x00000300,	// blueprint for a hierarchy model
		eHMODEL_HEADER,											// Header for the hierarchy model
		eNODE,														// render objects connected to the hierarchy
		eCOLLISION_NODE,											// collision meshes connected to the hierarchy
		eSKIN_NODE,													// skins connected to the hierarchy
		eHMODEL_AUX_DATA [[deprecated]],								// extension of the hierarchy model header
		eSHADOW_NODE [[deprecated]],									// shadow object connected to the hierarchy

	eLODMODEL								    =0x00000400,		// blueprint for an LOD model. This is simply a
		eLODMODEL_HEADER,											// collection of 'n' render objects, ordered in terms
		eLOD,															// of their expected rendering costs. (highest is first)

	eCOLLECTION								=0x00000420,		// collection of render object names
		eCOLLECTION_HEADER,										// general info regarding the collection
		eCOLLECTION_OBJ_NAME,									// contains a string which is the name of a render object
		ePLACEHOLDER,												// contains information about a 'dummy' object that will be instanced later
		eTRANSFORM_NODE,											// contains the filename of another w3d file that should be transformed by this node

	ePOINTS									=0x00000440,		// array of W3dVectorStruct's. May appear in meshes, hmodels, lodmodels, or collections.

	eLIGHT									=0x00000460,		// description of a light
		eLIGHT_INFO,												// generic light parameters
		eSPOT_LIGHT_INFO,											// extra spot light parameters
		eNEAR_ATTENUATION,										// optional near attenuation parameters
		eFAR_ATTENUATION,											// optional far attenuation parameters

	eEMITTER									=0x00000500,		// description of a particle emitter
		eEMITTER_HEADER,											// general information such as name and version
		eEMITTER_USER_DATA,										// user-defined data that specific loaders can switch on
		eEMITTER_INFO,												// generic particle emitter definition
		eEMITTER_INFOV2,											// generic particle emitter definition (version 2.0)
		eEMITTER_PROPS,											// Key-frameable properties
		eEMITTER_COLOR_KEYFRAME [[deprecated]],					// structure defining a single color keyframe
		eEMITTER_OPACITY_KEYFRAME [[deprecated]],					// structure defining a single opacity keyframe
		eEMITTER_SIZE_KEYFRAME [[deprecated]],						// structure defining a single size keyframe
		eEMITTER_LINE_PROPERTIES,								// line properties, used by line rendering mode
		eEMITTER_ROTATION_KEYFRAMES,							// rotation keys for the particles
		eEMITTER_FRAME_KEYFRAMES,								// frame keys (u-v based frame animation)
		eEMITTER_BLUR_TIME_KEYFRAMES,						// length of tail for line groups

	eAGGREGATE								=0x00000600,		// description of an aggregate object
		eAGGREGATE_HEADER,										// general information such as name and version
			eAGGREGATE_INFO,										// references to 'contained' models
		eTEXTURE_REPLACER_INFO,									// information about which meshes need textures replaced
		eAGGREGATE_CLASS_INFO,									// information about the original class that created this aggregate

	eHLOD										=0x00000700,		// description of an HLod object (see HLodClass)
		eHLOD_HEADER,												// general information such as name and version
		eHLOD_LOD_ARRAY,											// wrapper around the array of objects for each level of detail
			eHLOD_SUB_OBJECT_ARRAY_HEADER,					// info on the objects in this level of detail array
			eHLOD_SUB_OBJECT,										// an object in this level of detail array
		eHLOD_AGGREGATE_ARRAY,									// array of aggregates, contains W3D_CHUNK_SUB_OBJECT_ARRAY_HEADER and W3D_CHUNK_SUB_OBJECT_ARRAY
		eHLOD_PROXY_ARRAY,										// array of proxies, used for application-defined purposes, provides a name and a bone.

	eBOX										=0x00000740,		// defines an collision box render object (W3dBoxStruct)
	eSPHERE,
	eRING,

	eNULL_OBJECT							    =0x00000750,		// defines a NULL object (W3dNullObjectStruct)

	eLIGHTSCAPE								=0x00000800,		// wrapper for lights created with Lightscape.	
		eLIGHTSCAPE_LIGHT,										// definition of a light created with Lightscape.
			eLIGHT_TRANSFORM,										// position and orientation (defined as right-handed 4x3 matrix transform W3dLightTransformStruct).

	eDAZZLE									=0x00000900,		// wrapper for a glare object. Creates halos and flare lines seen around a bright light source
		eDAZZLE_NAME,												// null-terminated string, name of the dazzle (typical w3d object naming: "container.object")
		eDAZZLE_TYPENAME,											// null-terminated string, type of dazzle (from dazzle.ini)

	eSOUNDROBJ								=0x00000A00,		// description of a sound render object
		eSOUNDROBJ_HEADER,										// general information such as name and version
		eSOUNDROBJ_DEFINITION,									// chunk containing the definition of the sound that is to play	
};

enum class W3D_SHADER : uint8_t
{
	eDEPTHCOMPARE_PASS_NEVER = 0,			// pass never (i.e. always fail depth comparison test)
	eDEPTHCOMPARE_PASS_LESS,				// pass if incoming less than stored
	eDEPTHCOMPARE_PASS_EQUAL,				// pass if incoming equal to stored
	eDEPTHCOMPARE_PASS_LEQUAL,				// pass if incoming less than or equal to stored (default)
	eDEPTHCOMPARE_PASS_GREATER,			    // pass if incoming greater than stored	
	eDEPTHCOMPARE_PASS_NOTEQUAL,			// pass if incoming not equal to stored
	eDEPTHCOMPARE_PASS_GEQUAL,				// pass if incoming greater than or equal to stored
	eDEPTHCOMPARE_PASS_ALWAYS,				// pass always
	eDEPTHCOMPARE_PASS_MAX,					// end of enumeration

	eDEPTHMASK_WRITE_DISABLE = 0,			// disable depth buffer writes 
	eDEPTHMASK_WRITE_ENABLE,				// enable depth buffer writes		(default)
	eDEPTHMASK_WRITE_MAX,					// end of enumeration

	eALPHATEST_DISABLE = 0,					// disable alpha testing (default)
	eALPHATEST_ENABLE,						// enable alpha testing
	eALPHATEST_MAX,							// end of enumeration

 	eDESTBLENDFUNC_ZERO = 0,				// destination pixel doesn't affect blending (default)
 	eDESTBLENDFUNC_ONE,						// destination pixel added unmodified
 	eDESTBLENDFUNC_SRC_COLOR,				// destination pixel multiplied by fragment RGB components
 	eDESTBLENDFUNC_ONE_MINUS_SRC_COLOR,     // destination pixel multiplied by one minus (i.e. inverse) fragment RGB components
 	eDESTBLENDFUNC_SRC_ALPHA,				// destination pixel multiplied by fragment alpha component
 	eDESTBLENDFUNC_ONE_MINUS_SRC_ALPHA,	    // destination pixel multiplied by fragment inverse alpha
 	eDESTBLENDFUNC_SRC_COLOR_PREFOG,		// destination pixel multiplied by fragment RGB components prior to fogging
	eDESTBLENDFUNC_MAX,						// end of enumeration

	ePRIGRADIENT_DISABLE = 0,				// disable primary gradient (same as OpenGL 'decal' texture blend)
	ePRIGRADIENT_MODULATE,					// modulate fragment ARGB by gradient ARGB (default)
	ePRIGRADIENT_ADD,						// add gradient RGB to fragment RGB, copy gradient A to fragment A
	ePRIGRADIENT_BUMPENVMAP,				// environment-mapped bump mapping
	ePRIGRADIENT_MAX,						// end of enumeration

	eSECGRADIENT_DISABLE = 0,				// don't draw secondary gradient (default)
	eSECGRADIENT_ENABLE,					// add secondary gradient RGB to fragment RGB 
	eSECGRADIENT_MAX,						// end of enumeration

 	eSRCBLENDFUNC_ZERO = 0,					// fragment not added to color buffer
 	eSRCBLENDFUNC_ONE,						// fragment added unmodified to color buffer (default)
 	eSRCBLENDFUNC_SRC_ALPHA,				// fragment RGB components multiplied by fragment A
 	eSRCBLENDFUNC_ONE_MINUS_SRC_ALPHA,	    // fragment RGB components multiplied by fragment inverse (one minus) A
	eSRCBLENDFUNC_MAX,						// end of enumeration

	eTEXTURING_DISABLE = 0,					// no texturing (treat fragment initial color as 1,1,1,1) (default)
	eTEXTURING_ENABLE,						// enable texturing
	eTEXTURING_MAX,							// end of enumeration

	eDETAILCOLORFUNC_DISABLE = 0,			// local (default)
	eDETAILCOLORFUNC_DETAIL,				// other
	eDETAILCOLORFUNC_SCALE,					// local * other
	eDETAILCOLORFUNC_INVSCALE,				// ~(~local * ~other) = local + (1-local)*other
	eDETAILCOLORFUNC_ADD,					// local + other
	eDETAILCOLORFUNC_SUB,					// local - other
	eDETAILCOLORFUNC_SUBR,					// other - local
	eDETAILCOLORFUNC_BLEND,					// (localAlpha)*local + (~localAlpha)*other
	eDETAILCOLORFUNC_DETAILBLEND,			// (otherAlpha)*local + (~otherAlpha)*other
	eDETAILCOLORFUNC_MAX,					// end of enumeration

	eDETAILALPHAFUNC_DISABLE = 0,			// local (default)
	eDETAILALPHAFUNC_DETAIL,				// other
	eDETAILALPHAFUNC_SCALE,					// local * other
	eDETAILALPHAFUNC_INVSCALE,				// ~(~local * ~other) = local + (1-local)*other
	eDETAILALPHAFUNC_MAX,					// end of enumeration

	eDEPTHCOMPARE_DEFAULT    = eDEPTHCOMPARE_PASS_LEQUAL,
	eDEPTHMASK_DEFAULT       = eDEPTHMASK_WRITE_ENABLE,
	eALPHATEST_DEFAULT       = eALPHATEST_DISABLE,
	eDESTBLENDFUNC_DEFAULT   = eDESTBLENDFUNC_ZERO,
	ePRIGRADIENT_DEFAULT     = ePRIGRADIENT_MODULATE,
	eSECGRADIENT_DEFAULT     = eSECGRADIENT_DISABLE,
	eSRCBLENDFUNC_DEFAULT    = eSRCBLENDFUNC_ONE,
	eTEXTURING_DEFAULT       = eTEXTURING_DISABLE,
	eDETAILCOLORFUNC_DEFAULT = eDETAILCOLORFUNC_DISABLE,
	eDETAILALPHAFUNC_DEFAULT = eDETAILALPHAFUNC_DISABLE,
};

enum class PS2_SHADER_SETTINGS : uint8_t
{
    eSRC = 0,
	eDEST,
	eZERO,
	
	eSRC_ALPHA = 0,
	eDEST_ALPHA,
	eONE,

	// From combo box. To match the PC default gradient.
	ePRIGRADIENT_DECAL = 0,
	ePRIGRADIENT_MODULATE,
	ePRIGRADIENT_HIGHLIGHT,
	ePRIGRADIENT_HIGHLIGHT2,

	// Actual PS2 numbers.
	ePS2_PRIGRADIENT_MODULATE = 0,
	ePS2_PRIGRADIENT_DECAL,
	ePS2_PRIGRADIENT_HIGHLIGHT,
	ePS2_PRIGRADIENT_HIGHLIGHT2,


	eDEPTHCOMPARE_PASS_NEVER = 0,
	eDEPTHCOMPARE_PASS_LESS,
	eDEPTHCOMPARE_PASS_ALWAYS,
	eDEPTHCOMPARE_PASS_LEQUAL,
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Flags for the Triangle Attributes member
/////////////////////////////////////////////////////////////////////////////////////////////
enum class W3D_SURFACE : uint8_t
{
	eLIGHT_METAL = 0,
	eHEAVY_METAL,
	eWATER,
	eSAND,
	eDIRT,
	eMUD,
	eGRASS,
	eWOOD,
	eCONCRETE,
	eFLESH,
	eROCK,
	eSNOW,
	eICE,
	eDEFAULT,
	eGLASS,
	eCLOTH,
	eTIBERIUM_FIELD,
	eFOLIAGE_PERMEABLE,
	eGLASS_PERMEABLE,
	eICE_PERMEABLE,
	eCLOTH_PERMEABLE,
	eELECTRICAL,
	eFLAMMABLE,
	eSTEAM,
	eELECTRICAL_PERMEABLE,
	eFLAMMABLE_PERMEABLE,
	eSTEAM_PERMEABLE,
	eWATER_PERMEABLE,
	eTIBERIUM_WATER,
	eTIBERIUM_WATER_PERMEABLE,
	eUNDERWATER_DIRT,
	eUNDERWATER_TIBERIUM_DIRT,

    // NOTE: if you add a surface type, add it to the SurfaceEffects.INI file!
	eMAX
};

enum class W3D_ANIMATION_CHANNEL : uint8_t
{
	eX = 0,
	eY,
	eZ,
	eXR,
	eYR,
	eZR,
	eQ,

	eTIMECODED_X,
	eTIMECODED_Y,
	eTIMECODED_Z,
	eTIMECODED_Q,

	eADAPTIVEDELTA_X,
	eADAPTIVEDELTA_Y,
	eADAPTIVEDELTA_Z,
	eADAPTIVEDELTA_Q,
};

//
// Flavor Enumerations
//
enum class W3D_ANIMATION_FLAVOR : uint8_t
{
    eTIMECODED = 0,
    eADAPTIVE_DELTA,

	eVALID
};

enum class W3D_BIT_CHANNEL : uint8_t
{
    // turn meshes on and off depending on anim frame.
	eVIS = 0,
	eTIMECODED_VIS,
};

//
//	This enum contains valid defines for the Type field
// of the W3dEmitterUserInfoStruct. The programmer
// can add entries here that their specific loader
// can switch on to determine what type the emitter is.
//
// NOTE: Please add a string the the EMITTER_TYPE_NAMES
// array when you add an entry to the enum.
//
enum class W3D_EMITTER : uint8_t
{
	eDEFAULT = 0,
	eCOUNT
};

#endif