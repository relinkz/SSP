#ifndef PHYSICSDLL_PHYSICS_PHYSICSHANDLER_H
#define PHYSICSDLL_PHYSICS_PHYSICSHANDLER_H

#ifdef PHYSICSDLL_EXPORTS
#define PHYSICSDLL_API __declspec(dllexport)
#else
#define PHYSICSDLL_API __declspec(dllimport)
#endif

#include <DirectXMath.h>
#include <vector>

#pragma region

enum BoundingVolumeType
{
	BV_AABB,
	BV_OBB,
	BV_Sphere,
	BV_Plane
};

struct AABB
{
	float ext[3];
};

struct OBB
{
	float ext[3];
	DirectX::XMMATRIX ort;
};

struct Ray
{
	DirectX::XMVECTOR Origin;
	DirectX::XMVECTOR RayDir;
};
struct Sphere
{
	float radius;
};

struct Plane
{
	DirectX::XMVECTOR PC_normal;
};
#pragma endregion

__declspec(align(16)) struct PhysicsComponent
{
	DirectX::XMVECTOR PC_pos;
	DirectX::XMVECTOR PC_velocity;
	DirectX::XMVECTOR PC_rotation;
	DirectX::XMVECTOR PC_rotationVelocity;
	DirectX::XMVECTOR PC_normalForce;
	double PC_gravityInfluence;
	int PC_active;
	int PC_entityID;
	float PC_mass;
	bool PC_is_Static;
	bool PC_coolides;
	float PC_friction;
	float PC_elasticity;
	BoundingVolumeType PC_BVtype;

	AABB PC_AABB;
	OBB PC_OBB;
	Sphere PC_Sphere;
	Plane PC_Plane;

	void* operator new(size_t i) { return _aligned_malloc(i, 16); };
	void operator delete(void* p) { _aligned_free(p); };
};
struct ChainLink
{
	float CL_lenght;
	PhysicsComponent* CL_next; 
	PhysicsComponent* CL_previous;
};

class PhysicsHandler
{
private:
	std::vector<PhysicsComponent*> m_physicsComponents;
	std::vector<ChainLink> m_links;
	int m_nrOfStaticObjects;

	DirectX::XMVECTOR m_gravity;

	unsigned int	m_startIndex;		// At what index to start to check colision
	unsigned int	m_numberOfDynamics;	// Number of dynamic objects to check since we only want half
	bool			m_isHost;			// isHost is to check if this client should check collision between dynamic entities

	const float m_offSet = 0.5f;
	bool IntersectAABB();

	//intersection tests
	bool ObbObbIntersectionTest(PhysicsComponent* objA, PhysicsComponent* objB);
	bool SphereAABBIntersectionTest(PhysicsComponent* objSphere, PhysicsComponent* objAABB);
	bool SphereOBBIntersectionTest(PhysicsComponent* objSphere, PhysicsComponent* objOBB);
	bool SphereSphereIntersectionTest(PhysicsComponent* objSphere1, PhysicsComponent* objSphere2);
	bool SpherePlaneIntersectionTest(PhysicsComponent* objSphere, PhysicsComponent* objPlane, float dt);
	bool AABBPlaneIntersectionTest(PhysicsComponent* objAABB, PhysicsComponent* objPlane);
	bool OBBPlaneIntersectionTest(PhysicsComponent* objOBB, PhysicsComponent* objPlane);
	bool AABBAABBIntersectionTest(PhysicsComponent *obj1, PhysicsComponent *obj2, float dt);

	void CollitionDynamics(PhysicsComponent* obj1, PhysicsComponent* obj2, DirectX::XMVECTOR normal, float dt);

	//Math functions
	float DotProduct(const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2) const;
	DirectX::XMFLOAT3 CrossProduct(const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2) const;
	float DotProduct(const DirectX::XMVECTOR &v1, const DirectX::XMVECTOR &v2) const;

	DirectX::XMFLOAT3 VectorSubstract(const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2) const;

	//void CreateBB();
	void CreateDefaultBB(const DirectX::XMVECTOR &pos, PhysicsComponent* src);
	void CreateDefaultAABB(const DirectX::XMVECTOR &pos, PhysicsComponent* src);
	void CreateDefaultOBB(const DirectX::XMVECTOR &pos, PhysicsComponent* src);

	void SetStartIndex(unsigned int newStartIndex);
	void SetNumberOfDynamics(unsigned int newNumberOfDynamics);
	void SetIsHost(bool newIsHost);

public:
	PHYSICSDLL_API PhysicsHandler();
	PHYSICSDLL_API ~PhysicsHandler();

	PHYSICSDLL_API bool Initialize();
	PHYSICSDLL_API void ShutDown();
	PHYSICSDLL_API void Update(float deltaTime);

	PHYSICSDLL_API DirectX::XMMATRIX RotateBB_X(PhysicsComponent* src, const float &radian);
	PHYSICSDLL_API DirectX::XMMATRIX RotateBB_Y(PhysicsComponent* src, const float &radian);
	PHYSICSDLL_API DirectX::XMMATRIX RotateBB_Z(PhysicsComponent* src, const float &radian);


	PHYSICSDLL_API void TranslateBB(const DirectX::XMVECTOR &newPos, PhysicsComponent* src);
	PHYSICSDLL_API void Add_toRotateVec(PhysicsComponent* src);

	PHYSICSDLL_API void DoChainPhysics(ChainLink* link, float dt);
	PHYSICSDLL_API void AdjustChainLinkPosition(ChainLink* link);

	PHYSICSDLL_API void ApplyForceToComponent(PhysicsComponent* componentPtr, DirectX::XMVECTOR force, float dt);

	PHYSICSDLL_API PhysicsComponent* CreatePhysicsComponent(const DirectX::XMVECTOR &pos, const bool &isStatic);

	PHYSICSDLL_API void CreateChainLink(int index1, int index2, int nrOfLinks, float linkLenght);
	PHYSICSDLL_API bool IntersectRayOBB(const DirectX::XMVECTOR &rayOrigin, const DirectX::XMVECTOR &rayDir, const OBB &obj, const DirectX::XMVECTOR &obbPos);
	PHYSICSDLL_API bool IntersectRayOBB(const DirectX::XMVECTOR &rayOrigin, const DirectX::XMVECTOR &rayDir, const OBB &obj, const DirectX::XMVECTOR &obbPos, float &distanceToOBB);

	PHYSICSDLL_API void SimpleCollition(float dt);
	PHYSICSDLL_API void SimpleGravity(PhysicsComponent* componentPtr, const float &dt);

	PHYSICSDLL_API int GetNrOfComponents()const;
	PHYSICSDLL_API PhysicsComponent* GetDynamicComponentAt(int index)const;

	PHYSICSDLL_API void SetBB_Rotation(const DirectX::XMVECTOR &rotVec, PhysicsComponent* toRotate);

	PHYSICSDLL_API bool checkCollition();

	PHYSICSDLL_API void SortComponents(); //sorts the array so the dynamic components are first and static are last

#ifdef _DEBUG
	PHYSICSDLL_API void GetPhysicsComponentOBB(OBB*& src, int index);
	PHYSICSDLL_API void GetPhysicsComponentAABB(AABB*& src, int index);
	PHYSICSDLL_API void GetPhysicsComponentPlane(Plane*& src, int index);
#endif
};

#endif
