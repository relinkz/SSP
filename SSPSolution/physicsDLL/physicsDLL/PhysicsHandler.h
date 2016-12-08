#ifndef PHYSICSDLL_PHYSICS_PHYSICSHANDLER_H
#define PHYSICSDLL_PHYSICS_PHYSICSHANDLER_H

#ifdef PHYSICSLIBRARY_EXPORTS
#define PHYSICSDLL_PHYSICS_PHYSICSLIBRARY_API __declspec(dllexport)
#else
#define PHYSICSDLL_PHYSICS_PHYSICSLIBRARY_API __declspec(dllimport)
#endif

#include <DirectXMath.h>
#include <vector>

#pragma region

struct AABB
{
	float pos[3];
	float ext[3];
};

struct OBB
{
	DirectX::XMVECTOR pos;
	float ext[3];
	DirectX::XMMATRIX ort;
};

struct Ray
{
	DirectX::XMVECTOR Origin;
	DirectX::XMVECTOR RayDir;
};

#pragma endregion

__declspec(align(16)) struct PhysicsComponent
{
	DirectX::XMVECTOR PC_pos;
	DirectX::XMVECTOR PC_velocity;
	DirectX::XMVECTOR PC_rotationVelocity;
	double PC_gravityInfluence;
	int PC_active;
	int PC_entityID;
	float PC_mass;
	bool PC_is_Static;
	bool PC_coolides;

	AABB PC_AABB;
	OBB PC_OBB;
	//AABB PC_looseBoundingBox
	//BoundingVolume* PC_tightBoundingVolume; 
	//std::vector<int entityID, event EVENT> PC_eventlist;

};

class PHYSICSDLL_PHYSICS_PHYSICSLIBRARY_API PhysicsHandler
{
private:
	std::vector<PhysicsComponent*> m_dynamicComponents;
	int m_nrOfStaticObjects;
	
	DirectX::XMVECTOR m_gravity;

	const float m_offSet = 0.5f;
	bool IntersectAABB();
	bool DoIntersectionTestOBB(PhysicsComponent* objA, PhysicsComponent* objB);

	//Math functions
	float DotProduct(const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2) const;
	DirectX::XMFLOAT3 CrossProduct(const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2) const;
	float CrossProductf(const DirectX::XMVECTOR &v1, const DirectX::XMVECTOR &v2) const;


	DirectX::XMFLOAT3 VectorSubstract(const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2) const;

	//void CreateBB();
	void CreateDefaultBB(const DirectX::XMVECTOR &pos, PhysicsComponent* src);

	void CreateDefaultAABB(const DirectX::XMVECTOR &pos, PhysicsComponent* src);
	void CreateDefaultOBB(const DirectX::XMVECTOR &pos, PhysicsComponent* src);
public:
	PhysicsHandler();
	~PhysicsHandler();

	bool Initialize();
	void ShutDown();
	void Update();

	void CreatePhysicsComponent(const DirectX::XMVECTOR &pos);
	bool IntersectRayOBB(const DirectX::XMVECTOR &rayOrigin, const DirectX::XMVECTOR &rayDir, const OBB &obj);

	void SimpleCollition(float dt);
	void SimpleGravity(PhysicsComponent* componentPtr, const float &dt);

	int getNrOfComponents()const;
	PhysicsComponent* getDynamicComponents(int index)const;
	bool checkCollition();

};

#endif
