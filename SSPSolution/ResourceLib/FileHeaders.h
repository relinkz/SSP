#ifndef RESOURCELIB_FILEHEADERS_H
#define RESOURCELIB_FILEHEADERS_H

struct MainHeader
{
	unsigned int id;
	int resourceType;
};



struct MeshHeader
{
	unsigned int numVerts;
	unsigned int indexLength;
	unsigned int jointCount;
	bool skeleton;
};

struct SkeletonHeader {
	unsigned int jointCount;
};
struct JointHeader {
	float bindPose[16];
	int jointIndex;
	int parentIndex;
};
struct MaterialHeader
{
	unsigned int textureIDs[5]{0,0,0,0,0};

	float m_Metallic	  = 0.0f	;
	float m_Roughness	  = 0.0f	;
	float m_EmissiveValue = 0.0f	;

};

struct TextureHeader
{
	char filePath[256];
};
#endif
