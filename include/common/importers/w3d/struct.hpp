#ifndef W3D_STRUCT_HPP
#define W3D_STRUCT_HPP
// information source: https://github.com/mikolalysenko/w3d2ply

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <istream>
#include <vector>

#include "common/importers/w3d/defines.hpp"
#include "common/importers/w3d/enum.hpp"

class Assimp::IOStream;

class W3DChunkHeader {
public:
	// must be 8 bytes of data
	W3DChunkHeader(Assimp::IOStream *stream);

    W3D_CHUNK type() const;
    uint32_t size() const;
private:
    W3D_CHUNK m_type;
    uint32_t m_size;
};

class W3DTexCoord
{
public:
    W3DTexCoord() = default;
	W3DTexCoord(Assimp::IOStream *stream);
    W3DTexCoord(float u, float v);

	const glm::vec2& texCoord() const;
	operator std::string() const;
private:
    glm::vec2 m_texCoord;
};

class W3DVector
{
public:
	W3DVector() = default;
    W3DVector(Assimp::IOStream *stream);
    W3DVector(float x, float y, float z);

	const glm::vec3& vector() const;
	operator std::string() const;
private:
    glm::vec3 m_vector;
};

class W3DVectori
{
public:
	W3DVectori() = default;
	W3DVectori(Assimp::IOStream *stream);
	W3DVectori(int32_t x, int32_t y, int32_t z);

	const glm::i32vec3& vector() const;
	operator std::string() const;
private:
	glm::i32vec3 m_vector;
};

class W3DQuaternion
{
public:
    W3DQuaternion() = default;
	W3DQuaternion(Assimp::IOStream *stream);
    W3DQuaternion(float x, float y, float z, float w);

	const glm::quat& quaternion() const;
	operator std::string() const;
private:
    glm::quat m_quaternion;
};

class W3DRGB
{
public:
    W3DRGB() = default;
	W3DRGB(Assimp::IOStream *stream);
    W3DRGB(uint8_t r, uint8_t g, uint8_t b);

	const glm::u8vec3& rgb() const;
	operator std::string() const;
private:
    glm::u8vec3 m_rgb;
	// padding 1 byte
};

class W3DRGBA
{
public:
    W3DRGBA() = default;
	W3DRGBA(Assimp::IOStream *stream);
    W3DRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	const glm::u8vec4& rgba() const;
	operator std::string() const;
private:
    glm::u8vec4 m_rgba;
};

class W3DMaterialInfo
{
public:
    W3DMaterialInfo() = default;
	W3DMaterialInfo(Assimp::IOStream *stream);

	uint32_t passCount() const;
	uint32_t vertexMaterialCount() const;
	uint32_t shaderCount() const;
	uint32_t textureCount() const;

	operator std::string() const;
private:
    uint32_t m_passCount;
    uint32_t m_vertexMaterialCount;
    uint32_t m_shaderCount;
    uint32_t m_textureCount;
};

class W3DVertexMaterial
{
public:
	W3DVertexMaterial() = default;
	W3DVertexMaterial(Assimp::IOStream *stream);

	uint32_t attributes() const;
	const W3DRGB& ambient() const;
	const W3DRGB& diffuse() const;
	const W3DRGB& specular() const;
	const W3DRGB& emissive() const;
	float shininess() const;
	float opacity() const;
	float translucency() const;
private:
	uint32_t m_attributes;
	W3DRGB m_ambient;
	W3DRGB m_diffuse;
	W3DRGB m_specular;
	W3DRGB m_emissive;
	float m_shininess;
	float m_opacity;
	float m_translucency;
};

class W3DShader
{
public:
	W3DShader() = default;
	W3DShader(Assimp::IOStream *stream);

	uint8_t depthCompare() const;
	uint8_t depthMask() const;
	[[deprecated]] uint8_t colorMask() const;
	uint8_t destBlend() const;
	[[deprecated]] uint8_t fogFunc() const;
	uint8_t priGradient() const;
	uint8_t secGradient() const;
	uint8_t srcBlend() const;
	uint8_t texturing() const;
	uint8_t detailColorFunc() const;
	uint8_t detailAlphaFunc() const;
	[[deprecated]] uint8_t shaderPreset() const;
	uint8_t alphaTest() const;
	uint8_t postDetailColorFunc() const;
	uint8_t postDetailAlphaFunc() const;
private:
    uint8_t m_depthCompare;
    uint8_t m_depthMask;
    uint8_t m_colorMask;
    uint8_t m_destBlend;
    uint8_t m_fogFunc;
    uint8_t m_priGradient;
    uint8_t m_secGradient;
    uint8_t m_srcBlend;
    uint8_t m_texturing;
    uint8_t m_detailColorFunc;
    uint8_t m_detailAlphaFunc;
	uint8_t m_shaderPreset;
    uint8_t m_alphaTest;
    uint8_t m_postDetailColorFunc;
    uint8_t m_postDetailAlphaFunc;
	// padding 1 byte
};

class W3DTextureInfo
{
public:
	W3DTextureInfo() = default;
	W3DTextureInfo(Assimp::IOStream *stream);

	uint16_t attributes() const;
	uint16_t animType() const;
	uint32_t frameCount() const;
	float frameRate() const;
private:
	uint16_t m_attributes;
	uint16_t m_animType;
	uint32_t m_frameCount;
	float m_frameRate;
};

class W3DTriangle
{
public:
	W3DTriangle() = default;
	W3DTriangle(Assimp::IOStream *stream);

	uint32_t vertexIndex(uint32_t index) const;
	uint32_t attributes() const;
	const W3DVector& normal() const;
	float dist() const;

	operator std::string() const;
private:
	uint32_t m_vertexIndices[3];
	uint32_t m_attributes;
	W3DVector m_normal;
	float m_dist;
};

class W3DMeshHeader3
{
public:
	W3DMeshHeader3() = default;
	W3DMeshHeader3(Assimp::IOStream *stream);

	uint32_t version() const;
	uint32_t attributes() const;
	const std::string& meshName() const;
	const std::string& containerName() const;
	uint32_t numTriangles() const;
	uint32_t numVertices() const;
	uint32_t numMaterials() const;
	uint32_t numDamageStages() const;
	int32_t sortLevel() const;
	uint32_t prelitVersion() const;
	uint32_t futureCounts() const;
	uint32_t vertexChannels() const;
	uint32_t faceChannels() const;
	const W3DVector& boundingBoxMin() const;
	const W3DVector& boundingBoxMax() const;
	const W3DVector& boundingSphereCenter() const;
	float boundingSphereRadius() const;
private:
	uint32_t m_version;
	uint32_t m_attributes;
	std::string m_meshName;
	std::string m_containerName;
	uint32_t m_numTriangles;
	uint32_t m_numVertices;
	uint32_t m_numMaterials;
	uint32_t m_numDamageStages;
	int32_t m_sortLevel;
	uint32_t m_prelitVersion;
	uint32_t m_futureCounts[1];
	uint32_t m_vertexChannels;
	uint32_t m_faceChannels;
	W3DVector m_boundingBoxMin;
	W3DVector m_boundingBoxMax;
	W3DVector m_boundingSphereCenter;
	float m_boundingSphereRadius;
};

//
// Vertex Influences. For "skins" each vertex can be associated with a
// different bone.
//
class W3DVertexInfo
{
public:
	W3DVertexInfo() = default;
	W3DVertexInfo(Assimp::IOStream *stream);

	uint16_t boneIndex() const;
private:
	uint16_t m_boneIndex;
	// padding 6 bytes
};

//
// Deform information. Each mesh can have sets of keyframes of
//	deform info associated with it.
//
class W3DMeshDeform
{
public:
	W3DMeshDeform() = default;
	W3DMeshDeform(Assimp::IOStream *stream);

	uint32_t setCount() const;
	uint32_t alphaPasses() const;
private:
	uint32_t m_setCount;
	uint32_t m_alphaPasses;
	// reserved 12 bytes
};

//
// Deform set information. Each set is made up of a series
// of keyframes.
//
class W3DDeformSetInfo
{
public:
	W3DDeformSetInfo() = default;
	W3DDeformSetInfo(Assimp::IOStream *stream);

	uint32_t keyframeCount() const;
	uint32_t flags() const;
private:
	uint32_t m_keyframeCount;
	uint32_t m_flags;
	// reserved 4 bytes
};

//
// Deform keyframe information. Each keyframe is made up of
// a set of per-vert deform data.
//
class W3DDeformKeyframeInfo
{
public:
	W3DDeformKeyframeInfo() = default;
	W3DDeformKeyframeInfo(Assimp::IOStream *stream);

	float deformPercent() const;
	uint32_t dataCount() const;
private:
	float m_deformPercent;
	uint32_t m_dataCount;
	// reserved 8 bytes
};

//
// Deform data. Contains deform information about a vertex
// in the mesh.
//
class W3DDeformData
{
public:
	W3DDeformData() = default;
	W3DDeformData(Assimp::IOStream *stream);

	uint32_t vertexIndex() const;
	const W3DVector& position() const;
	const W3DRGBA& color() const;
private:
	uint32_t m_vertexIndex;
	W3DVector m_position;
	W3DRGBA m_color;
	// reserved 8 bytes
};

// 
// AABTree header. Each mesh can have an associated Axis-Aligned-Bounding-Box tree
// which is used for collision detection and certain rendering algorithms (like 
// texture projection.
//
class W3DAABTreeHeader
{
private:
	uint32_t m_nodeCount;
	uint32_t m_polyCount;
	// padding 24 bytes
};

// 
// AABTree Node. This is a node in the AABTree.
// If the MSB of FrontOrPoly0 is 1, then the node is a leaf and contains Poly0 and PolyCount
// else, the node is not a leaf and contains indices to its front and back children. This matches
// the format used by AABTreeClass in WW3D.
//
class W3DAABTreeNode
{
private:
	W3DVector m_min;
	W3DVector m_max;
	uint32_t m_frontOrPoly0;
	uint32_t m_backOrPolyCount;
};

/********************************************************************************

	WHT ( Westwood Hierarchy Tree )

	A hierarchy tree defines a set of coordinate systems which are connected
	hierarchically. The header defines the name, number of pivots, etc. 
	The pivots chunk will contain a W3dPivotStructs for each node in the
	tree. 
	
	The W3dPivotFixupStruct contains a transform for each MAX coordinate
	system and our version of that same coordinate system (bone). It is 
	needed when the user exports the base pose using "Translation Only".
	These are the matrices which go from the MAX rotated coordinate systems
	to a system which is unrotated in the base pose. These transformations
	are needed when exporting a hierarchy animation with the given hierarchy
	tree file.

	Another explanation of these kludgy "fixup" matrices:

	What are the "fixup" matrices? These are the transforms which
	were applied to the base pose when the user wanted to force the
	base pose to use only matrices with certain properties. For 
	example, if we wanted the base pose to use translations only,
	the fixup transform for each node is a transform which when
	multiplied by the real node's world transform, yeilds a pure
	translation matrix. Fixup matrices are used in the mesh
	exporter since all vertices must be transformed by their inverses
	in order to make things work. They also show up in the animation
	exporter because they are needed to make the animation work with
	the new base pose.

********************************************************************************/

class W3DHierarchyHeader
{
public:
	W3DHierarchyHeader() = default;
	W3DHierarchyHeader(Assimp::IOStream *stream);

	uint32_t version() const;
	const std::string& name() const;
	uint32_t numPivots() const;
	const W3DVector& center() const;

	operator std::string() const;
private:
	uint32_t m_version;
	std::string m_name;
	uint32_t m_numPivots;
	W3DVector m_center;
};

class W3DPivot
{
public:
	W3DPivot() = default;
	W3DPivot(Assimp::IOStream *stream);

	const std::string& name() const;
	uint32_t parentIndex() const;
	const W3DVector& translation() const;
	const W3DVector& eulerAngles() const;
	const W3DQuaternion& rotation() const;

	bool valid() const;
private:
	std::string m_name;
	uint32_t m_parentIndex;
	W3DVector m_translation;
	W3DVector m_eulerAngles;
	W3DQuaternion m_rotation;

	bool m_valid;
};

class W3DPivotFixup
{
private:
	glm::mat3x4 m_fixup;	// this is a direct dump of a MAX 3x4 matrix	
};

class W3DAnimHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];
	char m_hierarchyName[W3D_NAME_LEN];
	uint32_t m_numFrames;
	uint32_t m_frameRate;
};

class W3DCompressedAnimHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];
	char m_hierarchyName[W3D_NAME_LEN];
	uint32_t m_numFrames;
	uint16_t m_frameRate;
	uint16_t m_flavor;
};

class W3DAnimChannelHeader
{
private:
	uint16_t m_firstFrame;
	uint16_t m_lastFrame;
	uint16_t m_vectorLen;
	uint16_t m_flags;
	uint16_t m_pivot;
	// padding 2 bytes
};

class W3DBitChannelHeader
{
private:
	uint16_t m_firstFrame;
	uint16_t m_lastFrame;
	uint16_t m_flags;
	uint16_t m_pivot;
	uint8_t m_defaultValue;
};

class W3DTimeCodedAnimChannel
{
private:
	uint32_t m_numTimeCodes;	// number of time coded entries
	uint16_t m_pivot;			// pivot affected by this channel
	uint8_t m_vectorLen;		// length of each vector in this channel
	uint8_t m_flags;			// channel type
	uint32_t m_data[1];			// will be (NumTimeCodes * ((VectorLen * sizeof(uint32)) + sizeof(uint32)))
};

class W3DTimeCodedBitChannel
{
private:
	uint32_t m_numTimeCodes;	// number of time coded entries
	uint16_t m_pivot;			// pivot affected by this channel
	uint8_t m_flags;			// channel type
	uint8_t m_defaultValue;		// default value for this channel
	uint32_t m_data[1];			// will be (NumTimeCodes * sizeof(uint32))
};

class W3DAdaptiveDeltaAnimChannel
{
private:
	uint32_t m_numFrames;		// number of frames of animation
	uint16_t m_pivot;			// pivot effected by this channel
	uint8_t m_vectorLen;		// num Channels
	uint8_t m_flags;			// channel type
	float m_scale;				// Filter Table Scale
	uint32_t m_data[1];			// OpCode Data Stream
};

/********************************************************************************
	
	HMorphAnimClass

	This is an animation format which describes morphs between poses in another
	animation. It is used for Renegade's facial animation system. There is
	a normal anim which defines the pose for each phoneme and then a "Morph Anim"
	which defines the transitions between phonemes over time. In addition there
	is the concept of multiple morph channels in a morph anim. Each "channel"
	controls a set of pivots in the skeleton and has its own set of morph keys
	and poses. This lets us have one set of poses for expressions and another
	for phonemes (a bone is only moved in one or the other anims though)

	The chunks used to describe a "morph" anim are as follows:

	W3D_CHUNK_MORPH_ANIMATION							=0x000002C0,	// wrapper for the entire anim
		W3D_CHUNK_MORPHANIM_HEADER,										// W3dMorphAnimHeaderStruct describes playback rate, number of frames, and type of compression
		W3D_CHUNK_MORPHANIM_CHANNEL,										// wrapper for a channel
			W3D_CHUNK_MORPHANIM_POSENAME,									// name of the other anim which contains the poses for this morph channel
			W3D_CHUNK_MORPHANIM_KEYDATA,									// array of W3dMorphAnimKeyStruct's (use chunk length to determine how many)
		W3D_CHUNK_MORPHANIM_PIVOTCHANNELDATA,							// uin32 per pivot in the htree, indicating which channel controls the pivot


********************************************************************************/

class W3DMorphAnimHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];
	char m_hierarchyName[W3D_NAME_LEN];
	uint32_t m_frameCount;
	float m_frameRate;
	uint32_t m_channelCount;
};

class W3DMorphAnimKey
{
private:
	uint32_t m_morphFrame;
	uint32_t m_poseFrame;
};

/********************************************************************************

	HModel - Hiearchical Model

	A Hierarchy Model is a set of render objects which should be attached to 
	bones in a hierarchy tree. There can be multiple objects per node
	in the tree. Or there may be no objects attached to a particular bone.

	(gth) 09/22/2000 - Simplified the HModel file format. The W3DHModelAuxDataStruct
	was un-needed and moved to w3d_obsolete.h. The safe way to parse previous
	and current HModel formats is this:
	- Read in the header from W3D_CHUNK_HMODEL_HEADER
	- Allocate space for 'NumConnections' nodes that will follow
	- Read in the rest of the chunks
		- Create a sub-object for W3D_CHUNK_NODE, W3D_CHUNK_COLLISION_NODE, or
		 W3D_CHUNK_SKIN_NODE.
		- Skip the OBSOLETE_W3D_CHUNK_HMODEL_AUX_DATA and OBSOLETE_W3D_CHUNK_SHADOW_NODE

********************************************************************************/

class W3DHModelHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];				// Name of this model
	char m_hierarchyName[W3D_NAME_LEN];		// Name of the hierarchy tree this model uses
	uint16_t m_numConnections;
};

class W3DHModelNode
{
private:
	// Note: the full name of the Render object is expected to be: <HModelName>.<RenderObjName>
	char m_renderObjName[W3D_NAME_LEN];
	uint16_t m_pivotIndex;
};

/********************************************************************************

	Lights

	The following structs are used to define lights in the w3d file. Currently
	we have point lights, directional lights, and spot lights.

********************************************************************************/

class W3DLight
{
private:
	uint32_t m_attributes;
	// unused 4 bytes (Old exclusion bit deprecated)
	W3DRGB m_ambient;
	W3DRGB m_diffuse;
	W3DRGB m_specular;
	float m_intensity;
};

class W3DSpotLight
{
private:
	W3DVector m_spotDirection;
	float m_spotAngle;
	float m_spotExponent;
};

class W3DLightAttenuation
{
private:
	float m_start;
	float m_end;
};

class W3DLightTransform
{
private:
	float m_transform[3][4];
};

/********************************************************************************

	Particle emitters

	The following structs are used to define emitters in the w3d file.

********************************************************************************/

class W3DEmitterHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];
};

class W3DEmitterUserInfo
{
private:
	uint32_t m_type;				// One of the EMITTER_TYPEID_ enum's defined above
	uint32_t m_sizeofStringParam;	// Size (in bytes) of the following string data
	char m_stringParam[1];			// Array of bytes. Where "count = SizeofStringParam"
};

class W3DEmitterInfo
{
private:
	char m_textureFilename[260];
	float m_startSize;
	float m_endSize;
	float m_lifeTime;
	float m_emissionRate;
	float m_maxEmissions;
	float m_velocityRandom;
	float m_positionRandom;
	float m_fadeTime;
	float m_gravity;
	float m_elasticity;
	W3DVector m_velocity;
	W3DVector m_acceleration;
	W3DRGBA m_startColor;
	W3DRGBA m_endColor;
};

class W3DVolumeRandomizer
{
private:
	uint32_t m_classID;
	float m_value1;
	float m_value2;
	float m_value3;
	// reserved 16 bytes
};

class W3DEmitterInfoV2
{
private:
	uint32_t m_burstSize;
	W3DVolumeRandomizer m_creationVolume;
	W3DVolumeRandomizer m_velocityRandom;
	float m_outwardVelocity;
	float m_velocityInherit;
	W3DShader m_shader;
	uint32_t m_renderMode;
	uint32_t m_frameMode;
	// reserved 24 bytes
};

// W3D_CHUNK_EMITTER_PROPS
// Contains a W3dEmitterPropertyStruct followed by a number of color keyframes, 
// opacity keyframes, and size keyframes
class W3DEmitterProperty
{
private:
	uint32_t m_colorKeyframes;
	uint32_t m_opacityKeyframes;
	uint32_t m_sizeKeyframes;
	W3DRGBA m_colorRandom;
	float m_opacityRandom;
	float m_sizeRandom;
	// reserved 16 bytes
};

class W3DEmitterColorKeyframe
{
private:
	float m_time;
	W3DRGBA m_color;
};

class W3DEmitterOpacityKeyframe
{
private:
	float m_time;
	float m_opacity;
};

class W3DEmitterSizeKeyframe
{
private:
	float m_time;
	float m_size;
};

// W3D_CHUNK_EMITTER_ROTATION_KEYFRAMES 
// Contains a W3dEmitterRotationHeaderStruct followed by a number of
// rotational velocity keyframes.
class W3DEmitterRotationHeader
{
private:
	uint32_t m_keyframeCount;
	float m_random;				// random initial rotational velocity (rotations/sec)
	float m_orientationRandom;	// random initial orientation (rotations, 1.0=360deg)
	// reserved 4 bytes
};

class W3DEmitterRotationKeyframe
{
private:
	float m_time;
	float m_rotation;	// rotational velocity in rotations/sec
};

// W3D_CHUNK_EMITTER_FRAME_KEYFRAMES
// Contains a W3dEmitterFrameHeaderStruct followed by a number of
// frame keyframes (sub-texture indexing)
class W3DEmitterFrameHeader
{
private:
	uint32_t m_keyframeCount;
	float m_random;
	// reserved 8 bytes
};

class W3DEmitterFrameKeyframe
{
private:
	float m_time;
	uint32_t m_frame;
};

// W3D_CHUNK_EMITTER_BLUR_TIME_KEYFRAMES
// Contains a W3dEmitterFrameHeaderStruct followed by a number of
// frame keyframes (sub-texture indexing)
class W3DEmitterBlurTimeHeader
{
private:
	uint32_t m_keyframeCount;
	float m_random;
	// reserved 4 bytes
};

class W3DEmitterBlurTimeKeyframe
{
private:
	float m_time;
	float m_blurTime;
};

class W3DEmitterLineProperties
{
private:
	uint32_t m_flags;
	uint32_t m_subdivisionLevel;
	float m_noiseAmplitude;
	float m_mergeAbortFactor;
	float m_textureTileFactor;
	float m_UPerSec;
	float m_VPerSec;
	// reserved 36 bytes
};

/********************************************************************************

	Aggregate objects

	The following structs are used to define aggregates in the w3d file. An
	'aggregate' is simply a 'shell' that contains references to a hierarchy
	model and subobjects to attach to its bones.

********************************************************************************/
class W3DAggregateHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];
};

class W3DAggregateInfo
{
private:
	char m_baseModelName[W3D_NAME_LEN];
	uint32_t m_subObjectCount;
};

class W3DAggregateSubObject
{
private:
	char m_subObjectName[W3D_NAME_LEN];
	char m_boneName[W3D_NAME_LEN];
};

//
// Structures for version 1.2 and newer
//
class W3DAggregateMiscInfo
{
private:
	uint32_t m_originalClassID;
	uint32_t m_flags;
	// reserved 12 bytes
};

/********************************************************************************

	HLod (Hierarchical LOD Model)

	This is a hierarchical model which has multiple arrays of models which can
	be switched for LOD purposes.

	Relevant Chunks:
	----------------
	W3D_CHUNK_HLOD										=0x00000700,		// description of an HLod object (see HLodClass)
		W3D_CHUNK_HLOD_HEADER,												// general information such as name and version
		W3D_CHUNK_HLOD_LOD_ARRAY,											// wrapper around the array of objects for each level of detail
			W3D_CHUNK_HLOD_SUB_OBJECT_ARRAY_HEADER,					// info on the objects in this level of detail array
			W3D_CHUNK_HLOD_SUB_OBJECT,										// an object in this level of detail array
		W3D_CHUNK_HLOD_AGGREGATE_ARRAY,									// array of aggregates, contains W3D_CHUNK_SUB_OBJECT_ARRAY_HEADER and W3D_CHUNK_SUB_OBJECT_ARRAY
		W3D_CHUNK_HLOD_PROXY_ARRAY,										// array of proxies, used for application-defined purposes
	
	An HLOD is the basic hierarchical model format used by W3D. It references
	an HTree for its hierarchical structure and animation data and several arrays
	of sub-objects; one for each LOD in the model. In addition, it can contain
	an array of "aggregates" which are references to external W3D objects to
	be automatically attached into it. And it can have a list of "proxy" objects
	which can be used for application purposes such as instantiating game objects
	at the specified transform. 

********************************************************************************/
class W3DHLODHeader
{
private:
	uint32_t m_version;
	uint32_t m_lodCount;
	char m_name[W3D_NAME_LEN];
	char m_hierarchyName[W3D_NAME_LEN];
};

class W3DHLODArrayHeader
{
private:
	uint32_t m_modelCount;
	float m_maxScreenSize;
};

class W3DHLODSubObject
{
private:
	uint32_t m_boneIndex;
	char m_name[W3D_NAME_LEN];
};

/********************************************************************************

	Collision Boxes

	Collision boxes are meant to be used for, you guessed it, collision detection.
	For this reason, they only contain a minimal amount of rendering information
	(a color). 

	Axis Aligned - This is a bounding box which is *always* aligned with the world 
	coordinate system. So, the center point is to be transformed by whatever
	transformation matrix is being used but the extents always point down the
	world space x,y, and z axes. (in effect, you are translating the center).

	Oriented - This is an oriented 3D box. It is aligned with the coordinate system
	it is in. So its extents always point along the local coordinate system axes.

********************************************************************************/
class W3DBox
{
private:
	uint32_t m_version;
	uint32_t m_attributes;
	char m_name[MESH_PATH_ENTRY_LEN];
	W3DRGB m_color;
	W3DVector m_center;
	W3DVector m_extents;
};

/********************************************************************************

	NULL Objects

	Null objects are used by the LOD system to make meshes dissappear at lower
	levels of detail.

********************************************************************************/
class W3DNullObject
{
private:
	uint32_t m_version;
	uint32_t m_attributes;
	// padding 8 bytes
	char m_name[MESH_PATH_ENTRY_LEN];
};

/********************************************************************************

	Sound render objects

	The following structs are used to define sound render object in the w3d file.

	These objects are used to trigger a sound effect in the world. When the object
	is shown, its associated sound is added to the world and played, when the object
	is hidden, the associated sound is stopped and removed from the world.


********************************************************************************/
class W3DSoundRObjHeader
{
private:
	uint32_t m_version;
	char m_name[W3D_NAME_LEN];
	uint32_t m_flags;
	// padding 32 bytes
};

// w3dstruct.h
/*	Data Structures for stoing the data from W3D files
	Origonally provided by Westwood Studios, some modifications to make things compile in C done by Jonathan Wilson

	This file is part of W3DView
	W3DView is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 2, or (at your option) any later
	version. See the file COPYING for more details.
*/

// Structure for holding data about a material
class W3DMaterial
{
public:
	W3DMaterial() = default;

	void setName(const std::string &name);
	const std::string& name() const;

	void setVertexMaterialInfo(const W3DVertexMaterial &vertexMaterialInfo);
	const W3DVertexMaterial& vertexMaterialInfo() const;

	void setMapperArgs0(const std::string &mapperArgs0);
	const std::string& mapperArgs0() const;

	void setMapperArgs1(const std::string &mapperArgs1);
	const std::string& mapperArgs1() const;

	void setUPerSec(float UPerSec);
	float UPerSec() const;

	void setVPerSec(float VPerSec);
	float VPerSec() const;

	void setCurrentU(float currentU);
	float currentU() const;

	void setCurrentV(float currentV);
	float currentV() const;
private:
	std::string m_name;
	W3DVertexMaterial m_vertexMaterialInfo;
	std::string m_mapperArgs0;
	std::string m_mapperArgs1;
	float m_UPerSec;
	float m_VPerSec;
	float m_currentU;
	float m_currentV;
};

// Structure for holding information about a texture
class W3DTexture
{
public:
	W3DTexture() = default;

	void setName(const std::string &name);
	const std::string& name() const;

	void setTextureInfo(const W3DTextureInfo &textureInfo);
	const W3DTextureInfo& textureInfo() const;

	bool textureInfoLoaded() const;
private:
	std::string m_name;
	W3DTextureInfo m_textureInfo;
	bool m_textureInfoLoaded;
};

// Structure for holding information about a texture stage
class W3DTextureStage
{
public:
	W3DTextureStage() = default;

	void addTextureID(uint32_t textureID);
	size_t textureIDCount() const;
	uint32_t textureID(uint32_t index) const;

	void addPerFaceTexCoordID(const W3DVectori &perFaceTexCoordID);
	size_t perFaceTexCoordIDCount() const;
	const W3DVectori& perFaceTexCoordID(uint32_t index) const;

	void addTexCoord(const W3DTexCoord &texCoord);
	size_t texCoordCount() const;
	const W3DTexCoord& texCoord(uint32_t index) const;
private:
	std::vector<uint32_t> m_textureIDs;
	std::vector<W3DVectori> m_perFaceTexCoordIDs;
	std::vector<W3DTexCoord> m_texCoords;
};

// Structure for describing a material pass
class W3DMaterialPass
{
public:
	W3DMaterialPass() = default;

	void addVertexMaterialID(uint32_t vertexMaterialID);
	size_t vertexMaterialIDCount() const;
	uint32_t vertexMaterialID(uint32_t index) const;

	void addShaderID(uint32_t shaderID);
	size_t shaderIDCount() const;
	uint32_t shaderID(uint32_t index) const;

	void addDCG(const W3DRGBA &DCG);
	size_t DCGCount() const;
	const W3DRGBA& DCG(uint32_t index) const;

	void addDIG(const W3DRGBA &DIG);
	size_t DIGCount() const;
	const W3DRGBA& DIG(uint32_t index) const;

	void addSCG(const W3DRGBA &SCG);
	size_t SCGCount() const;
	const W3DRGBA& SCG(uint32_t index) const;

	void addTextureStage(const W3DTextureStage &textureStage);
	size_t textureStageCount() const;
	const W3DTextureStage& textureStage(uint32_t index) const;
private:
	std::vector<uint32_t> m_vertexMaterialIDs;
	std::vector<uint32_t> m_shaderIDs;
	std::vector<W3DRGBA> m_DCGs;
	std::vector<W3DRGBA> m_DIGs;
	std::vector<W3DRGBA> m_SCGs;
	std::vector<W3DTextureStage> m_textureStages;
};

class W3DMesh
{
public:
	W3DMesh() = default;

	void setHeader(const W3DMeshHeader3 &header);
	const W3DMeshHeader3& header() const;

	void addVertex(const W3DVector &vertex);
	const W3DVector& vertex(uint32_t index) const;

	void addNormal(const W3DVector &normal);
	const W3DVector& normal(uint32_t index) const;

	void addInfluence(const W3DVertexInfo &influence);
	const W3DVertexInfo& influence(uint32_t index) const;

	void addTriangle(const W3DTriangle &triangle);
	const W3DTriangle& triangle(uint32_t index) const;

	void addShadeIndex(uint32_t shadeIndex);
	uint32_t shadeIndex(uint32_t index) const;

	void setMaterialInfo(const W3DMaterialInfo &materialInfo);
	const W3DMaterialInfo& materialInfo() const;

	void addMaterial(const W3DMaterial &material);
	const W3DMaterial& material(uint32_t index) const;

	void addShader(const W3DShader &shader);
	const W3DShader& shader(uint32_t index) const;

	void addTexture(const W3DTexture &texture);
	const W3DTexture& texture(uint32_t index) const;

	void addMaterialPass(const W3DMaterialPass &materialPass);
	const W3DMaterialPass& materialPass(uint32_t index) const;
private:
	W3DMeshHeader3 m_header;
	std::vector<W3DVector> m_vertices;
	std::vector<W3DVector> m_normals;
	std::vector<W3DVertexInfo> m_influences;
	std::vector<W3DTriangle> m_triangles;
	std::vector<uint32_t> m_shadeIndices;
	W3DMaterialInfo m_materialInfo;
	std::vector<W3DMaterial> m_materials;
	std::vector<W3DShader> m_shaders;
	std::vector<W3DTexture> m_textures;
	std::vector<W3DMaterialPass> m_materialPasses;
};

class W3DHierarchy
{
public:
	W3DHierarchy() = default;
	void setHeader(const W3DHierarchyHeader &header);
	const W3DHierarchyHeader& header() const;

	void addPivot(const W3DPivot &pivot);
	const W3DPivot& pivot(uint32_t index) const;
private:
	W3DHierarchyHeader m_header;
	std::vector<W3DPivot> m_pivots;
	std::vector<glm::mat4> m_rotateMatrices;	// rotate matrices for pivots
	std::vector<uint32_t> m_pivotVisibilities;	// pivot visibility flags
};

class W3DAnimChannel
{
private:
	W3DAnimChannelHeader m_header;
	std::vector<float> m_data;
};

class W3DBitChannel
{
private:
	W3DBitChannelHeader m_header;
	std::vector<uint8_t> m_data;
};

class W3DAnimation
{
private:
	W3DAnimHeader m_header;
	std::vector<W3DAnimChannel> m_animChannels;
	std::vector<W3DBitChannel> m_bitChannels;
};

class W3DLODArray
{
private:
	W3DHLODArrayHeader m_header;
	std::vector<W3DHLODSubObject> m_subObjects;
};

class W3DHLOD
{
private:
	W3DHLODHeader m_header;
	std::vector<W3DLODArray> m_lodArrays;
};

#endif