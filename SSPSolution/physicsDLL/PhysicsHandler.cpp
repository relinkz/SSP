#include "PhysicsHandler.h"
#include <iostream>
#include <chrono>
#include <ctime>



bool PhysicsHandler::IntersectAABB()
{
	bool possibleCollitionX = false;
	bool possibleCollitionY = false;
	bool possibleCollitionZ = false;
	PhysicsComponent* PC_ptr = nullptr;
	PhysicsComponent* PC_toCheck = nullptr;
	bool result = false;

	DirectX::XMFLOAT3 temp;
	DirectX::XMFLOAT3 temp2;



	int nrOfComponents = this->m_physicsComponents.size();
	float vecToObj[3];

	for (int i = 0; i < (nrOfComponents - this->m_nrOfStaticObjects); i++)
	{
		PC_toCheck = this->m_physicsComponents.at(i);
		DirectX::XMStoreFloat3(&temp, PC_toCheck->PC_pos);

		for (int j = i + 1; j < nrOfComponents; j++)
		{
			PC_ptr = this->m_physicsComponents.at(j);
			DirectX::XMStoreFloat3(&temp2, PC_ptr->PC_pos);

			vecToObj[0] = 0; //remove clutter values, or old values
			vecToObj[0] = temp.x - temp2.x;

			vecToObj[1] = 0; //remove clutter values, or old values
			vecToObj[1] = temp.y - temp2.y;

			vecToObj[2] = 0; //remove clutter values, or old values
			vecToObj[2] = temp.z - temp2.z;

			//Fraps return the absolute value
			//http://www.cplusplus.com/reference/cmath/fabs/

			//if the extensions from objA and objB together is smaller than the vector to b, then no collition
			possibleCollitionX = (fabs(vecToObj[0]) <= (PC_toCheck->PC_AABB.ext[0] + PC_ptr->PC_AABB.ext[0]));
			if (possibleCollitionX == true)
			{
				possibleCollitionY = (fabs(vecToObj[1]) <= (PC_toCheck->PC_AABB.ext[1] + PC_ptr->PC_AABB.ext[1]));
				if (possibleCollitionY == true)
				{
					possibleCollitionZ = (fabs(vecToObj[2]) <= (PC_toCheck->PC_AABB.ext[2] + PC_ptr->PC_AABB.ext[2]));
					if (possibleCollitionZ == true)
					{
						// apply OOB check for more precisition
						result = true;
						//result = this->DoIntersectionTestOBB(PC_toCheck, PC_ptr);
					}
				}
			}
		}
	}
	return result;
}

//---

bool PhysicsHandler::OBBOBBIntersectionTest(OBB* &obb1, DirectX::XMVECTOR obb1Pos, OBB* &obb2, DirectX::XMVECTOR obb2Pos)
{
	DirectX::XMFLOAT3 transPF_v;
	DirectX::XMFLOAT3 transPF_t;

	DirectX::XMFLOAT3 orthA[3];
	DirectX::XMFLOAT3 orthB[3];

	DirectX::XMFLOAT3 posA;
	DirectX::XMFLOAT3 posB;

	DirectX::XMStoreFloat3(&posA, obb1Pos);
	DirectX::XMStoreFloat3(&posB, obb2Pos);


	//not very clever way, but I need to know if shit work, for debug purpuses
	for (int i = 0; i < 3; i++)
	{
		DirectX::XMStoreFloat3(&orthA[i], obb1->ort.r[i]);
		DirectX::XMStoreFloat3(&orthB[i], obb2->ort.r[i]);
	}

	OBB* a = nullptr;
	OBB* b = nullptr;

	a = obb1;
	b = obb2;

	float T[3];

	transPF_v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//scalar values
	float rA = 0.0f;
	float rB = 0.0f;
	float t = 0.0f;

	DirectX::XMFLOAT3 L = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//B's basis with respect to A's local frame
	DirectX::XMFLOAT3X3 R;

	//this holds the translation vector in parent frame
	transPF_v = this->VectorSubstract(posB, posA);


	//translation in A's frame (START)

	//really tedious to do this, if time is given, we should make a better dot product
	//really directX?!?!?
	for (int i = 0; i < 3; i++)
	{
		T[i] = this->DotProduct(transPF_v, orthA[i]);

	}
	//T[1] = -2.0f;
	//T[2] = 0.0;
	//translation in A's frame (END)

	//calculate the rotation matrix
	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			R.m[i][k] = this->DotProduct(orthA[i], orthB[k]);
		}
	}

	/*ALGORITHM: Use the separating axis test for all 15 potential
	separating axes. If a separating axis could not be found, the two
	boxes overlap. */

	//A's basis vectors
	for (int i = 0; i < 3; i++)
	{
		rA = a->ext[i];

		rB = b->ext[0] * fabs(R.m[i][0]) + b->ext[1] * fabs(R.m[i][1]) + b->ext[2] * fabs(R.m[i][2]);

		t = fabs(T[i]);

		if (t >(rA + rB))
		{
			return false;
		}
	}

	//B's basis vectors
	for (int i = 0; i < 3; i++)
	{
		rA = a->ext[0] * fabs(R.m[0][i]) + a->ext[1] * fabs(R.m[1][i]) + a->ext[2] * fabs(R.m[2][i]);

		rB = b->ext[i];

		t = fabs(T[0] * R.m[0][i] + T[1] * R.m[1][i] + T[2] * R.m[2][i]);

		if (t >(rA + rB))
		{
			return false;
		}
	}

	//9 cross products??
	/*
	I have no clue what happening here mathwise, if time is with us, I will try to understand what is happening
	here.

	//sorce
	http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=5
	*/

	// L = A0 x B0
	rA = a->ext[1] * fabs(R.m[2][0]) + a->ext[2] * fabs(R.m[1][0]);
	rB = b->ext[1] * fabs(R.m[0][2]) + b->ext[2] * fabs(R.m[0][1]);

	t = fabs(T[2] * R.m[1][0] - T[1] * R.m[2][0]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A0 x B1
	rA = a->ext[1] * fabs(R.m[2][1]) + a->ext[2] * fabs(R.m[1][1]);
	rB = b->ext[0] * fabs(R.m[0][2]) + b->ext[2] * fabs(R.m[0][0]);

	t = fabs(T[2] * R.m[1][1] - T[1] * R.m[2][1]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A0 x B2
	rA = a->ext[1] * fabs(R.m[2][2]) + a->ext[2] * fabs(R.m[1][2]);
	rB = b->ext[0] * fabs(R.m[0][1]) + b->ext[1] * fabs(R.m[0][0]);

	t = fabs(T[2] * R.m[1][2] - T[1] * R.m[2][2]);

	if (t > (rA + rB))
	{
		return false;
	}

	// L = A1 x B0
	rA = a->ext[0] * fabs(R.m[2][0]) + a->ext[2] * fabs(R.m[0][0]);
	rB = b->ext[1] * fabs(R.m[1][2]) + b->ext[2] * fabs(R.m[1][1]);

	t = fabs(T[0] * R.m[2][0] - T[2] * R.m[0][0]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A1 x B1
	rA = a->ext[0] * fabs(R.m[2][1]) + a->ext[2] * fabs(R.m[0][1]);
	rB = b->ext[0] * fabs(R.m[1][2]) + b->ext[2] * fabs(R.m[1][0]);

	t = fabs(T[0] * R.m[2][1] - T[2] * R.m[0][1]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A1 x B2
	rA = a->ext[0] * fabs(R.m[2][2]) + a->ext[2] * fabs(R.m[0][2]);
	rB = b->ext[0] * fabs(R.m[1][1]) + b->ext[1] * fabs(R.m[1][0]);

	t = fabs(T[0] * R.m[2][2] - T[2] * R.m[0][2]);

	if (t > (rA + rB))
	{
		return false;
	}

	// L = A2 x B0
	rA = a->ext[0] * fabs(R.m[1][0]) + a->ext[1] * fabs(R.m[0][0]);
	rB = b->ext[1] * fabs(R.m[2][2]) + b->ext[2] * fabs(R.m[2][1]);

	t = fabs(T[1] * R.m[0][0] - T[0] * R.m[1][0]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A2 x B1
	rA = a->ext[0] * fabs(R.m[1][1]) + a->ext[1] * fabs(R.m[0][1]);
	rB = b->ext[0] * fabs(R.m[2][2]) + b->ext[2] * fabs(R.m[2][0]);

	t = fabs(T[2] * R.m[1][1] - T[1] * R.m[2][1]);
	t = fabs(T[1] * R.m[0][1] - T[0] * R.m[1][1]);
	float test = rA + rB;

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A2 x B2
	rA = a->ext[0] * fabs(R.m[1][2]) + a->ext[1] * fabs(R.m[0][2]);
	rB = b->ext[0] * fabs(R.m[2][1]) + b->ext[1] * fabs(R.m[2][0]);

	t = fabs(T[1] * R.m[0][2] - T[0] * R.m[1][2]);

	if (t > (rA + rB))
	{
		return false;
	}
	/*no separating axis found,
	the two boxes overlap */

	return true;
}
bool PhysicsHandler::OBBAABBIntersectionTest(OBB* &obb, DirectX::XMVECTOR obbPos, AABB* &AABB, DirectX::XMVECTOR aabbPos)
{
	//this function just convertes the AABB in current and makes a temporary OBB that is the same size 
	//and then does OBBVSOBB intersection test.
	//IMPORTANT the second PhysicsComponent has to be the AABB
	bool result = false;



	PhysicsComponent OBBconverted;
	OBBconverted.PC_BVtype = BV_OBB;
	OBBconverted.PC_pos = aabbPos;
	OBBconverted.PC_OBB.ort = DirectX::XMMatrixIdentity();
	OBBconverted.PC_OBB.ext[0] = AABB->ext[0];
	OBBconverted.PC_OBB.ext[1] = AABB->ext[1];
	OBBconverted.PC_OBB.ext[2] = AABB->ext[2];

	OBB* obb_ptr = &OBBconverted.PC_OBB;

	result = this->OBBOBBIntersectionTest(obb, obbPos, obb_ptr, aabbPos);


	return result;
}
bool PhysicsHandler::SphereAABBIntersectionTest(Sphere* &sphere, DirectX::XMVECTOR spherePos, AABB* &AABB, DirectX::XMVECTOR aabbPos)
{
	//this section of the code found on http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=4
	DirectX::XMVECTOR sphere_local = DirectX::XMVectorSubtract(spherePos, aabbPos);
	float sphere_pos[3];
	sphere_pos[0] = DirectX::XMVectorGetX(sphere_local);
	sphere_pos[1] = DirectX::XMVectorGetY(sphere_local);
	sphere_pos[2] = DirectX::XMVectorGetZ(sphere_local);

	float box_axis[3];
	box_axis[0] = AABB->ext[0];
	box_axis[1] = AABB->ext[1];
	box_axis[2] = AABB->ext[2];


	float s = 0;
	float d = 0;

	for (int i = 0; i < 3; i++)
	{
		if (sphere_pos[i] < -box_axis[i])
		{
			s = sphere_pos[i] - (-box_axis[i]);
			d += s*s;
		}
		else if (sphere_pos[i] > box_axis[i])
		{
			s = sphere_pos[i] - box_axis[i];
			d += s*s;
		}
	}

	bool result = d <= (sphere->radius * sphere->radius);


	return result;
}
bool PhysicsHandler::SphereOBBIntersectionTest(Sphere* &sphere, DirectX::XMVECTOR spherePos, OBB* &obb, DirectX::XMVECTOR obbPos, DirectX::XMVECTOR obbRotation)
{
	float radX = DirectX::XMVectorGetX(obbRotation);
	float radY = DirectX::XMVectorGetY(obbRotation);
	float radZ = DirectX::XMVectorGetZ(obbRotation);

	//create rotation matrix to translate the sphere into OBBs local space and do SphereVSAABB test
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationX(radX) * DirectX::XMMatrixRotationY(radY) * DirectX::XMMatrixRotationZ(radZ);
	DirectX::XMVECTOR t;
	DirectX::XMVECTOR toSphere;
	DirectX::XMVECTOR originalToSphere;

	DirectX::XMVECTOR originalVel;

	DirectX::XMMATRIX rotInv = DirectX::XMMatrixInverse(&t, rot);

	toSphere = DirectX::XMVectorSubtract(spherePos, obbPos);


	//rotate the toSphere vector to get sphere in OBBs locan space
	toSphere = DirectX::XMVector3Transform(toSphere, rotInv);

	PhysicsComponent testAABB;
	testAABB.PC_BVtype = BV_AABB;
	testAABB.PC_pos = obbPos;
	testAABB.PC_AABB.ext[0] = obb->ext[0];
	testAABB.PC_AABB.ext[1] = obb->ext[1];
	testAABB.PC_AABB.ext[2] = obb->ext[2];

	AABB*  aabb_ptr = &testAABB.PC_AABB;

	//do sphereVSAABB test
	bool result = this->SphereAABBIntersectionTest(sphere, spherePos, aabb_ptr, obbPos);


	return result;
}
bool PhysicsHandler::SphereSphereIntersectionTest(Sphere* &sphere1, DirectX::XMVECTOR sphere1Pos, Sphere* &sphere2, DirectX::XMVECTOR sphere2Pos)
{
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(sphere1Pos, sphere2Pos);
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));

	float totalLenght = (sphere1->radius + sphere2->radius);
	bool result = d <= totalLenght;


	return result;
}
bool PhysicsHandler::SpherePlaneIntersectionTest(Sphere* &sphere, DirectX::XMVECTOR spherePos, Plane* &plane, DirectX::XMVECTOR planePos)
{
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(spherePos, planePos);
	DirectX::XMVECTOR pParallel;
	DirectX::XMVECTOR pPerpendicular;

	DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular, diffVec, plane->PC_normal);

	float d = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));

	bool result = d <= sphere->radius;


	return result;
}
bool PhysicsHandler::AABBPlaneIntersectionTest(AABB* &aabb, DirectX::XMVECTOR aabbPos, Plane* &plane, DirectX::XMVECTOR planePos)
{
	//this section of the code taken from http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=7
	float d = 0;
	float a1 = aabb->ext[0];
	float a2 = aabb->ext[1];
	float a3 = aabb->ext[2];

	float sum = 0;
	float dot1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(plane->PC_normal, DirectX::XMVectorSet(a1 / a1, 0, 0, 0)));
	float dot2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(plane->PC_normal, DirectX::XMVectorSet(0, a2 / a2, 0, 0)));
	float dot3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(plane->PC_normal, DirectX::XMVectorSet(0, 0, a3 / a3, 0)));

	sum += a1 * fabs(dot1);
	sum += a2 * fabs(dot2);
	sum += a3 * fabs(dot3);

	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(aabbPos, planePos);
	DirectX::XMVECTOR pParallel;
	DirectX::XMVECTOR dump;

	DirectX::XMVector3ComponentsFromNormal(&pParallel, &dump, diffVec, plane->PC_normal);

	DirectX::XMVECTOR temp_pos = DirectX::XMVectorSubtract(aabbPos, pParallel);

	diffVec = DirectX::XMVectorSubtract(aabbPos, temp_pos);
	pParallel = DirectX::XMVectorSet(0, 0, 0, 0);
	DirectX::XMVector3ComponentsFromNormal(&pParallel, &dump, diffVec, plane->PC_normal);

	d = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));

	bool result = d < sum;


	return result;
}
bool PhysicsHandler::OBBPlaneIntersectionTest(OBB* &obb, DirectX::XMVECTOR obbPos, Plane* &plane, DirectX::XMVECTOR planePos)
{
	float a1 = obb->ext[0];
	float a2 = obb->ext[1];
	float a3 = obb->ext[2];

	float sum = 0;
	float dot1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(plane->PC_normal, obb->ort.r[0]));
	float dot2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(plane->PC_normal, obb->ort.r[1]));
	float dot3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(plane->PC_normal, obb->ort.r[2]));

	sum += a1 * abs(dot1);
	sum += a2 * abs(dot2);
	sum += a3 * abs(dot3);

	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(obbPos, planePos);
	DirectX::XMVECTOR pParallel;
	DirectX::XMVECTOR pPerpendicular;

	DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular, diffVec, plane->PC_normal);

	float d = 0;

	d = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));

	bool result = d < sum;

	return result;
}
bool PhysicsHandler::AABBAABBIntersectionTest(AABB* &aabb1, DirectX::XMVECTOR aabb1Pos, AABB* &aabb2, DirectX::XMVECTOR aabb2Pos)
{
	bool possibleCollitionX = false;
	bool possibleCollitionY = false;
	bool possibleCollitionZ = false;
	PhysicsComponent* PC_ptr = nullptr;
	PhysicsComponent* PC_toCheck = nullptr;
	bool result = false;

	DirectX::XMFLOAT3 temp;
	DirectX::XMFLOAT3 temp2;

	float xOverlap;
	float yOverlap;
	float zOverlap;


	int nrOfComponents = this->m_physicsComponents.size();
	float vecToObj[3];

	//PC_toCheck = obj1;
	DirectX::XMStoreFloat3(&temp, aabb1Pos);

	//PC_ptr = obj2;
	DirectX::XMStoreFloat3(&temp2, aabb2Pos);

	vecToObj[0] = 0; //remove clutter values, or old values
	vecToObj[0] = temp.x - temp2.x;

	vecToObj[1] = 0; //remove clutter values, or old values
	vecToObj[1] = temp.y - temp2.y;

	vecToObj[2] = 0; //remove clutter values, or old values
	vecToObj[2] = temp.z - temp2.z;

	//Fraps return the absolute value
	//http://www.cplusplus.com/reference/cmath/fabs/


	bool noCollision = false;

	possibleCollitionX = (fabs(vecToObj[0]) <= PC_toCheck->PC_AABB.ext[0] + PC_ptr->PC_AABB.ext[0]);
	if (possibleCollitionX == true)
	{
		yOverlap = vecToObj[1];
		possibleCollitionY = (fabs(vecToObj[1]) <= PC_toCheck->PC_AABB.ext[1] + PC_ptr->PC_AABB.ext[1]);
		if (possibleCollitionY == true)
		{
			zOverlap = vecToObj[2];
			possibleCollitionZ = (fabs(vecToObj[2]) <= PC_toCheck->PC_AABB.ext[2] + PC_ptr->PC_AABB.ext[2]);
			if (possibleCollitionZ == true)
			{
				result = true;

			}
		}
	}
	return result;
}

//---

bool PhysicsHandler::AABBAABBIntersectionTest(PhysicsComponent *obj1, PhysicsComponent *obj2, float dt)
{
	bool possibleCollitionX = false;
	bool possibleCollitionY = false;
	bool possibleCollitionZ = false;
	PhysicsComponent* PC_ptr = nullptr;
	PhysicsComponent* PC_toCheck = nullptr;
	bool result = false;

	DirectX::XMFLOAT3 temp;
	DirectX::XMFLOAT3 temp2;

	float xOverlap;
	float yOverlap;
	float zOverlap;


	int nrOfComponents = this->m_physicsComponents.size();
	float vecToObj[3];

	PC_toCheck = obj1;
	DirectX::XMStoreFloat3(&temp, PC_toCheck->PC_pos);

	PC_ptr = obj2;
	DirectX::XMStoreFloat3(&temp2, PC_ptr->PC_pos);

	vecToObj[0] = 0; //remove clutter values, or old values
	vecToObj[0] = temp.x - temp2.x;

	vecToObj[1] = 0; //remove clutter values, or old values
	vecToObj[1] = temp.y - temp2.y;

	vecToObj[2] = 0; //remove clutter values, or old values
	vecToObj[2] = temp.z - temp2.z;

			//Fraps return the absolute value
			//http://www.cplusplus.com/reference/cmath/fabs/

			//if the extensions from objA and objB together is smaller than the vector to b, then no collition
		xOverlap = vecToObj[0];
		float x_total_ext = PC_toCheck->PC_AABB.ext[0] + PC_ptr->PC_AABB.ext[0];
		float y_total_ext = PC_toCheck->PC_AABB.ext[1] + PC_ptr->PC_AABB.ext[1];
		float z_total_ext = PC_toCheck->PC_AABB.ext[2] + PC_ptr->PC_AABB.ext[2];

		DirectX::XMVECTOR correction = DirectX::XMVectorAdd(obj1->PC_pos, obj2->PC_pos);
		correction = DirectX::XMVector4Normalize(correction);

		float procentMargin = 0.997f;

		float yCorrection = 0.0f;
		float correctionMargin = 0.4f;

		bool noCollision = false;

		possibleCollitionX = (fabs(vecToObj[0]) <= PC_toCheck->PC_AABB.ext[0] + PC_ptr->PC_AABB.ext[0]);
		if (possibleCollitionX == true)
		{
			yOverlap = vecToObj[1];
			possibleCollitionY = (fabs(vecToObj[1]) <= PC_toCheck->PC_AABB.ext[1] + PC_ptr->PC_AABB.ext[1]);
			if (possibleCollitionY == true)
			{
				zOverlap = vecToObj[2];
				possibleCollitionZ = (fabs(vecToObj[2]) <= PC_toCheck->PC_AABB.ext[2] + PC_ptr->PC_AABB.ext[2]);
				if (possibleCollitionZ == true)
				{
					DirectX::XMVECTOR normal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
					float xProcent = fabs(xOverlap / x_total_ext);
					float yProcent = fabs(yOverlap / y_total_ext);
					float zProcent = fabs(zOverlap / z_total_ext);

					float highLimit = 0.1f;
					float diff = 0.0f;

					// apply OOB check for more precisition
					result = true;
					if (
						fabs(xOverlap  / x_total_ext) > fabs(yOverlap / y_total_ext)
						&&
						fabs(xOverlap / x_total_ext) > fabs(zOverlap / z_total_ext)
						)
					{
						//overlapX
						float distanceToMove = obj1->PC_AABB.ext[0] + obj2->PC_AABB.ext[0];
						distanceToMove = distanceToMove - fabs(vecToObj[0]);
						normal = DirectX::XMVectorSet(1, 0, 0, 0);
						if (vecToObj[0] < 0)
						{
							distanceToMove *= -1;
							normal = DirectX::XMVectorSet(-1, 0, 0, 0);
						}
						if (!obj1->PC_steadfast)
						{

							//obj1->PC_pos = DirectX::XMVectorAdd(obj1->PC_pos, DirectX::XMVectorSet(distanceToMove, 0, 0, 0)); // old

							float obj1YMin = DirectX::XMVectorGetY(obj1->PC_pos) - obj1->PC_AABB.ext[1];
							float obj2YMax = DirectX::XMVectorGetY(obj2->PC_pos) + obj2->PC_AABB.ext[1];
							diff = fabs(obj1YMin - obj2YMax);

							if (diff < highLimit)
							{
								normal = DirectX::XMVectorSet(0, 1, 0, 0);
								yCorrection = diff;
								distanceToMove = 0;
							}

							obj1->PC_pos = DirectX::XMVectorAdd(obj1->PC_pos, DirectX::XMVectorSet(distanceToMove, yCorrection, 0, 0));
						}
						else if (!obj2->PC_steadfast && !obj2->PC_is_Static)
						{

							//obj2->PC_pos = DirectX::XMVectorAdd(obj2->PC_pos, DirectX::XMVectorSet(-distanceToMove, 0, 0, 0)); // old

							float obj2YMin = DirectX::XMVectorGetY(obj2->PC_pos) - obj2->PC_AABB.ext[1];
							float obj1YMax = DirectX::XMVectorGetY(obj1->PC_pos) + obj1->PC_AABB.ext[1];
							diff = fabs(obj2YMin - obj1YMax);

							if (diff < highLimit)
							{
								yCorrection = diff;
								distanceToMove = 0;
							}

							obj2->PC_pos = DirectX::XMVectorAdd(obj2->PC_pos, DirectX::XMVectorSet(distanceToMove, yCorrection, 0, 0));
						}
					}

					if (
						fabs(yOverlap / y_total_ext) > fabs(xOverlap / x_total_ext)
						&&
						fabs(yOverlap / y_total_ext) > fabs(zOverlap / z_total_ext)
						)
					{
						float distanceToMove = obj1->PC_AABB.ext[1] + obj2->PC_AABB.ext[1];
						distanceToMove = distanceToMove - fabs(vecToObj[1]);
						normal = DirectX::XMVectorSet(0, 1, 0, 0);
						if (vecToObj[1] < 0)
						{
							distanceToMove *= -1;
							normal = DirectX::XMVectorSet(0, -1, 0, 0);
						}
						if (!obj1->PC_steadfast)
						{
							obj1->PC_pos = DirectX::XMVectorAdd(obj1->PC_pos, DirectX::XMVectorSet(0, distanceToMove, 0, 0));
						}
						else if (!obj2->PC_steadfast && !obj2->PC_is_Static)
						{
							obj2->PC_pos = DirectX::XMVectorAdd(obj2->PC_pos, DirectX::XMVectorSet(0, -distanceToMove, 0, 0));
						}
					}

					if (
						fabs(zOverlap / z_total_ext) > fabs(xOverlap / x_total_ext)
						&&
						fabs(zOverlap / z_total_ext) > fabs(yOverlap / y_total_ext)
						)
					{
						float distanceToMove = obj1->PC_AABB.ext[2] + obj2->PC_AABB.ext[2];
						distanceToMove = distanceToMove - fabs(vecToObj[2]);
						normal = DirectX::XMVectorSet(0, 0, 1, 0);
						if (vecToObj[2] < 0)
						{
							distanceToMove *= -1;
							normal = DirectX::XMVectorSet(0, 0, -1, 0);
						}
						if (!obj1->PC_steadfast)
						{


							//obj1->PC_pos = DirectX::XMVectorAdd(obj1->PC_pos, DirectX::XMVectorSet(0, 0, distanceToMove, 0)); // old

							float obj1YMin = DirectX::XMVectorGetY(obj1->PC_pos) - obj1->PC_AABB.ext[1];
							float obj2YMax = DirectX::XMVectorGetY(obj2->PC_pos) + obj2->PC_AABB.ext[1];

							diff = fabs(obj1YMin - obj2YMax);

							if (diff < highLimit)
							{
								yCorrection = diff;
								distanceToMove = 0;
								normal = DirectX::XMVectorSet(0, 1, 0, 0);
							}

							obj1->PC_pos = DirectX::XMVectorAdd(obj1->PC_pos, DirectX::XMVectorSet(0, yCorrection, distanceToMove, 0));

						}
						else if (!obj2->PC_steadfast && !obj2->PC_is_Static)
						{

							
							//obj2->PC_pos = DirectX::XMVectorAdd(obj2->PC_pos, DirectX::XMVectorSet(0, 0, -distanceToMove, 0)); // old

							float obj2YMin = DirectX::XMVectorGetY(obj2->PC_pos) - obj2->PC_AABB.ext[1];
							float obj1YMax = DirectX::XMVectorGetY(obj1->PC_pos) + obj1->PC_AABB.ext[1];

							diff = fabs(obj2YMin - obj1YMax);

							if (diff < highLimit)
							{
								yCorrection = diff;
								distanceToMove = 0;
							}

							obj2->PC_pos = DirectX::XMVectorAdd(obj2->PC_pos, DirectX::XMVectorSet(0, yCorrection, distanceToMove, 0));
						}
					}
					//obj1->PC_pos = DirectX::XMVectorAdd(obj1->PC_pos, correction);
					this->CollitionDynamics(obj1, obj2, normal, dt);
					obj1->PC_normalForce = normal;

					//overlappY

					//overlappZ



				}
			}
		}
	return result;
}

bool PhysicsHandler::ObbObbIntersectionTest(PhysicsComponent* objA, PhysicsComponent* objB, float dt)
{
	DirectX::XMFLOAT3 transPF_v;
	DirectX::XMFLOAT3 transPF_t;

	DirectX::XMFLOAT3 orthA[3];
	DirectX::XMFLOAT3 orthB[3];

	DirectX::XMFLOAT3 posA;
	DirectX::XMFLOAT3 posB;

	DirectX::XMStoreFloat3(&posA, objA->PC_pos);
	DirectX::XMStoreFloat3(&posB, objB->PC_pos);


	//not very clever way, but I need to know if shit work, for debug purpuses
	for (int i = 0; i < 3; i++)
	{
		DirectX::XMStoreFloat3(&orthA[i], objA->PC_OBB.ort.r[i]);
		DirectX::XMStoreFloat3(&orthB[i], objB->PC_OBB.ort.r[i]);
	}

	OBB* a = nullptr;
	OBB* b = nullptr;

	a = &objA->PC_OBB;
	b = &objB->PC_OBB;

	float T[3];

	transPF_v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//scalar values
	float rA = 0.0f;
	float rB = 0.0f;
	float t = 0.0f;

	DirectX::XMFLOAT3 L = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	//B's basis with respect to A's local frame
	DirectX::XMFLOAT3X3 R;

	//this holds the translation vector in parent frame
	transPF_v = this->VectorSubstract(posB, posA);


	//translation in A's frame (START)

	//really tedious to do this, if time is given, we should make a better dot product
	//really directX?!?!?
	for (int i = 0; i < 3; i++)
	{
		T[i] = this->DotProduct(transPF_v, orthA[i]);

	}
	//T[1] = -2.0f;
	//T[2] = 0.0;
	//translation in A's frame (END)

	//calculate the rotation matrix
	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			R.m[i][k] = this->DotProduct(orthA[i], orthB[k]);
		}
	}

	/*ALGORITHM: Use the separating axis test for all 15 potential
	separating axes. If a separating axis could not be found, the two
	boxes overlap. */

	//A's basis vectors
	for (int i = 0; i < 3; i++)
	{
		rA = a->ext[i];

		rB = b->ext[0] * fabs(R.m[i][0]) + b->ext[1] * fabs(R.m[i][1]) + b->ext[2] * fabs(R.m[i][2]);

		t = fabs(T[i]);

		if (t >(rA + rB))
		{
			return false;
		}
	}

	//B's basis vectors
	for (int i = 0; i < 3; i++)
	{
		rA = a->ext[0] * fabs(R.m[0][i]) + a->ext[1] * fabs(R.m[1][i]) + a->ext[2] * fabs(R.m[2][i]);

		rB = b->ext[i];

		t = fabs(T[0] * R.m[0][i] + T[1] * R.m[1][i] + T[2] * R.m[2][i]);

		if (t >(rA + rB))
		{
			return false;
		}
	}

	//9 cross products??
	/*
	I have no clue what happening here mathwise, if time is with us, I will try to understand what is happening
	here.

	//sorce
	http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=5
	*/

	// L = A0 x B0
	rA = a->ext[1] * fabs(R.m[2][0]) + a->ext[2] * fabs(R.m[1][0]);
	rB = b->ext[1] * fabs(R.m[0][2]) + b->ext[2] * fabs(R.m[0][1]);

	t = fabs(T[2] * R.m[1][0] - T[1] * R.m[2][0]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A0 x B1
	rA = a->ext[1] * fabs(R.m[2][1]) + a->ext[2] * fabs(R.m[1][1]);
	rB = b->ext[0] * fabs(R.m[0][2]) + b->ext[2] * fabs(R.m[0][0]);

	t = fabs(T[2] * R.m[1][1] - T[1] * R.m[2][1]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A0 x B2
	rA = a->ext[1] * fabs(R.m[2][2]) + a->ext[2] * fabs(R.m[1][2]);
	rB = b->ext[0] * fabs(R.m[0][1]) + b->ext[1] * fabs(R.m[0][0]);

	t = fabs(T[2] * R.m[1][2] - T[1] * R.m[2][2]);

	if (t > (rA + rB))
	{
		return false;
	}

	// L = A1 x B0
	rA = a->ext[0] * fabs(R.m[2][0]) + a->ext[2] * fabs(R.m[0][0]);
	rB = b->ext[1] * fabs(R.m[1][2]) + b->ext[2] * fabs(R.m[1][1]);

	t = fabs(T[0] * R.m[2][0] - T[2] * R.m[0][0]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A1 x B1
	rA = a->ext[0] * fabs(R.m[2][1]) + a->ext[2] * fabs(R.m[0][1]);
	rB = b->ext[0] * fabs(R.m[1][2]) + b->ext[2] * fabs(R.m[1][0]);

	t = fabs(T[0] * R.m[2][1] - T[2] * R.m[0][1]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A1 x B2
	rA = a->ext[0] * fabs(R.m[2][2]) + a->ext[2] * fabs(R.m[0][2]);
	rB = b->ext[0] * fabs(R.m[1][1]) + b->ext[1] * fabs(R.m[1][0]);

	t = fabs(T[0] * R.m[2][2] - T[2] * R.m[0][2]);

	if (t > (rA + rB))
	{
		return false;
	}

	// L = A2 x B0
	rA = a->ext[0] * fabs(R.m[1][0]) + a->ext[1] * fabs(R.m[0][0]);
	rB = b->ext[1] * fabs(R.m[2][2]) + b->ext[2] * fabs(R.m[2][1]);

	t = fabs(T[1] * R.m[0][0] - T[0] * R.m[1][0]);

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A2 x B1
	rA = a->ext[0] * fabs(R.m[1][1]) + a->ext[1] * fabs(R.m[0][1]);
	rB = b->ext[0] * fabs(R.m[2][2]) + b->ext[2] * fabs(R.m[2][0]);

	t = fabs(T[2] * R.m[1][1] - T[1] * R.m[2][1]);
	t = fabs(T[1] * R.m[0][1] - T[0] * R.m[1][1]);
	float test = rA + rB;

	if (t > (rA + rB))
	{
		return false;
	}

	//L = A2 x B2
	rA = a->ext[0] * fabs(R.m[1][2]) + a->ext[1] * fabs(R.m[0][2]);
	rB = b->ext[0] * fabs(R.m[2][1]) + b->ext[1] * fabs(R.m[2][0]);

	t = fabs(T[1] * R.m[0][2] - T[0] * R.m[1][2]);

	if (t > (rA + rB))
	{
		return false;
	}
	/*no separating axis found,
	the two boxes overlap */
	printf("Collition has been detected\n");
	this->ObbObbCollitionCorrection(objA, objB, dt);

	return true;
}

bool PhysicsHandler::OBBAABBIntersectionTest(PhysicsComponent * objOBB, PhysicsComponent * objAABB, float dt)
{
	//this function just convertes the AABB in current and makes a temporary OBB that is the same size 
	//and then does OBBVSOBB intersection test.
	//IMPORTANT the second PhysicsComponent has to be the AABB
	bool result = false;

	PhysicsComponent OBBconverted;
	OBBconverted.PC_BVtype = BV_OBB;
	OBBconverted.PC_is_Static = objAABB->PC_is_Static;
	OBBconverted.PC_pos = objAABB->PC_pos;
	OBBconverted.PC_velocity = objAABB->PC_velocity;
	OBBconverted.PC_OBB.ort = DirectX::XMMatrixIdentity();
	OBBconverted.PC_OBB.ext[0] = objAABB->PC_AABB.ext[0];
	OBBconverted.PC_OBB.ext[1] = objAABB->PC_AABB.ext[1];
	OBBconverted.PC_OBB.ext[2] = objAABB->PC_AABB.ext[2];
	OBBconverted.PC_elasticity = objAABB->PC_elasticity;
	OBBconverted.PC_friction = objAABB->PC_friction;

	result = this->ObbObbIntersectionTest(objOBB, &OBBconverted, dt);

	if (result)
	{
		int a = 0;
	}

	objAABB->PC_pos = OBBconverted.PC_pos;
	objAABB->PC_velocity = OBBconverted.PC_velocity;

	return result;
}

void PhysicsHandler::ObbObbCollitionCorrection(PhysicsComponent * obj1, PhysicsComponent * obj2, float dt)
{
	DirectX::XMVECTOR collitionPoint;
	collitionPoint = this->FindCollitionPoint(obj1, obj2, dt);
}

DirectX::XMVECTOR PhysicsHandler::FindCollitionPoint(PhysicsComponent * obj1, PhysicsComponent * obj2, float dt)
{
	OBB* A = &obj1->PC_OBB;
	OBB* B = &obj2->PC_OBB;

	//get the position of the objects
	DirectX::XMVECTOR Apos = obj1->PC_pos;
	DirectX::XMVECTOR Bpos = obj2->PC_pos;

	//vector holding  the vector to obj1 to obj2
	DirectX::XMVECTOR BtoAvector;
	BtoAvector = DirectX::XMVectorSubtract(Apos, Bpos);

	//vector holding the vector to obj2 to obj1
	DirectX::XMVECTOR AtoBvector;
	AtoBvector = DirectX::XMVectorSubtract(Bpos, Apos);

	DirectX::XMFLOAT3 scalarA(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 scalarB(0.0f, 0.0f, 0.0f);

	//project this vector onto the axises of the obb of A
	DirectX::XMStoreFloat(&scalarA.x, DirectX::XMVector3Dot(AtoBvector, A->ort.r[0]));
	DirectX::XMStoreFloat(&scalarA.y, DirectX::XMVector3Dot(AtoBvector, A->ort.r[1]));
	DirectX::XMStoreFloat(&scalarA.z, DirectX::XMVector3Dot(AtoBvector, A->ort.r[2]));

	//check the projected scalar against extended
	if (scalarA.x > A->ext[0])
	{
		scalarA.x = A->ext[0];
	}
	else if (scalarA.x < -A->ext[0])
	{
		scalarA.x = -A->ext[0];
	}

	if (scalarA.y > A->ext[1])
	{
		scalarA.y = A->ext[1];
	}

	else if (scalarA.y < -A->ext[1])
	{
		scalarA.y = -A->ext[1];
	}

	if (scalarA.z > A->ext[2])
	{
		scalarA.z = A->ext[2];
	}
	else if (scalarA.z < -A->ext[2])
	{
		scalarA.z = -A->ext[2];
	}


	//project this vector onto the axises of the obb of B
	DirectX::XMStoreFloat(&scalarB.x, DirectX::XMVector3Dot(BtoAvector, B->ort.r[0]));
	DirectX::XMStoreFloat(&scalarB.y, DirectX::XMVector3Dot(BtoAvector, B->ort.r[1]));
	DirectX::XMStoreFloat(&scalarB.z, DirectX::XMVector3Dot(BtoAvector, B->ort.r[2]));

	if (scalarB.x > B->ext[0])
	{
		scalarB.x = B->ext[0];
	}
	else if (scalarB.x < -B->ext[0])
	{
		scalarB.x = -B->ext[0];
	}

	if (scalarB.y > B->ext[1])
	{
		scalarB.y = B->ext[1];
	}

	else if (scalarB.y < -B->ext[1])
	{
		scalarB.y = -B->ext[1];
	}

	if (scalarB.z > B->ext[2])
	{
		scalarB.z = B->ext[2];
	}
	else if (scalarB.z < -B->ext[2])
	{
		scalarB.z = -B->ext[2];
	}

	DirectX::XMVECTOR pointOfIntersectA;
	pointOfIntersectA = Apos;

	DirectX::XMVECTOR pointOfIntersectB;
	pointOfIntersectB = Bpos;

	pointOfIntersectA = DirectX::XMVectorAdd(pointOfIntersectA, DirectX::XMVectorScale(A->ort.r[0], scalarA.x));
	pointOfIntersectA = DirectX::XMVectorAdd(pointOfIntersectA, DirectX::XMVectorScale(A->ort.r[1], scalarA.y));
	pointOfIntersectA = DirectX::XMVectorAdd(pointOfIntersectA, DirectX::XMVectorScale(A->ort.r[2], scalarA.z));

	pointOfIntersectB = DirectX::XMVectorAdd(pointOfIntersectB, DirectX::XMVectorScale(B->ort.r[0], scalarB.x));
	pointOfIntersectB = DirectX::XMVectorAdd(pointOfIntersectB, DirectX::XMVectorScale(B->ort.r[1], scalarB.y));
	pointOfIntersectB = DirectX::XMVectorAdd(pointOfIntersectB, DirectX::XMVectorScale(B->ort.r[2], scalarB.z));

	bool pointOfIntersectionBInA = true;
	pointOfIntersectionBInA = this->IsPointInBox(pointOfIntersectB, A, Apos);

	bool pointOfIntersectionAInB = true;
	pointOfIntersectionAInB = this->IsPointInBox(pointOfIntersectA, B, Bpos);

	DirectX::XMVECTOR result = DirectX::XMVectorSet(0, 0, 0, 0);
	if (pointOfIntersectionAInB == true && pointOfIntersectionBInA == true)
	{
		/*
		case 1
		both points are in both OBBs
		*/

		result = pointOfIntersectB;
	}

	else if (pointOfIntersectionAInB == true || pointOfIntersectionBInA == true)
	{
		/*
		case 2/3 only one point of intersection exists in both OBBs
		*/
		int a = -1;
		fabs(a);
		DirectX::XMVECTOR pointOfInterest;
		PhysicsComponent* componentOfInterst = nullptr;
		PhysicsComponent* otherComponent = nullptr;
		if (pointOfIntersectionAInB == true)
		{
			pointOfInterest = pointOfIntersectA;
			componentOfInterst = obj2;
			otherComponent = obj1;
		}
		else if (pointOfIntersectionBInA == true)
		{
			pointOfInterest = pointOfIntersectB;
			componentOfInterst = obj1;
			otherComponent = obj2;
		}


		result = pointOfInterest;
	}
	else if (pointOfIntersectionAInB == false && pointOfIntersectionBInA == false)
	{
		/*
		case 4 when neither of the points can be found in both OBBs
		*/

		DirectX::XMVECTOR corner1 = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR corner2 = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR corner3 = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR corner4 = DirectX::XMVectorSet(0, 0, 0, 0);

		DirectX::XMVECTOR corners[2];
		corners[0] = DirectX::XMVectorSet(0, 0, 0, 0);
		corners[1] = DirectX::XMVectorSet(0, 0, 0, 0);

		corner1 = DirectX::XMVectorAdd(corner1, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[0], obj1->PC_OBB.ext[0]));
		corner1 = DirectX::XMVectorAdd(corner1, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[1], obj1->PC_OBB.ext[1]));
		corner1 = DirectX::XMVectorAdd(corner1, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[2], obj1->PC_OBB.ext[2]));

		corner2 = DirectX::XMVectorAdd(corner2, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[0], -obj1->PC_OBB.ext[0]));
		corner2 = DirectX::XMVectorAdd(corner2, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[1], obj1->PC_OBB.ext[1]));
		corner2 = DirectX::XMVectorAdd(corner2, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[2], obj1->PC_OBB.ext[2]));

		corner3 = DirectX::XMVectorAdd(corner3, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[0], obj1->PC_OBB.ext[0]));
		corner3 = DirectX::XMVectorAdd(corner3, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[1], obj1->PC_OBB.ext[1]));
		corner3 = DirectX::XMVectorAdd(corner3, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[2], -obj1->PC_OBB.ext[2]));

		corner4 = DirectX::XMVectorAdd(corner4, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[0], -obj1->PC_OBB.ext[0]));
		corner4 = DirectX::XMVectorAdd(corner4, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[1], obj1->PC_OBB.ext[1]));
		corner4 = DirectX::XMVectorAdd(corner4, DirectX::XMVectorScale(obj1->PC_OBB.ort.r[2], -obj1->PC_OBB.ext[2]));

		int nrOfCornersFound = 0;

		if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner1), B, Bpos))
		{
			corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner1);
			nrOfCornersFound++;
		}
		if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner2), B, Bpos))
		{
			corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner2);
			nrOfCornersFound++;
		}
		if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner3), B, Bpos) && nrOfCornersFound < 2)
		{
			corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner3);
			nrOfCornersFound++;
		}
		if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner4), B, Bpos) && nrOfCornersFound < 2)
		{
			corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner4);
			nrOfCornersFound++;
		}
		if (nrOfCornersFound < 2)
		{
			corner1 = DirectX::XMVectorScale(corner1, -1);
			corner2 = DirectX::XMVectorScale(corner2, -1);
			corner3 = DirectX::XMVectorScale(corner3, -1);
			corner4 = DirectX::XMVectorScale(corner4, -1);

			if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner1), B, Bpos) && nrOfCornersFound < 2)
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner1);
				nrOfCornersFound++;
			}
			if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner2), B, Bpos) && nrOfCornersFound < 2)
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner2);
				nrOfCornersFound++;
			}
			if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner3), B, Bpos) && nrOfCornersFound < 2)
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner3);
				nrOfCornersFound++;
			}
			if (this->IsPointInBox(DirectX::XMVectorAdd(Apos, corner4), B, Bpos) && nrOfCornersFound < 2)
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Apos, corner4);
				nrOfCornersFound++;
			}

		}
		else if (nrOfCornersFound == 0)
		{
			corner1 = DirectX::XMVectorSet(0, 0, 0, 0);
			corner2 = DirectX::XMVectorSet(0, 0, 0, 0);
			corner3 = DirectX::XMVectorSet(0, 0, 0, 0);
			corner4 = DirectX::XMVectorSet(0, 0, 0, 0);


			corner1 = DirectX::XMVectorAdd(corner1, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[0], obj2->PC_OBB.ext[0]));
			corner1 = DirectX::XMVectorAdd(corner1, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[1], obj2->PC_OBB.ext[1]));
			corner1 = DirectX::XMVectorAdd(corner1, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[2], obj2->PC_OBB.ext[2]));

			corner2 = DirectX::XMVectorAdd(corner2, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[0], -obj2->PC_OBB.ext[0]));
			corner2 = DirectX::XMVectorAdd(corner2, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[1], obj2->PC_OBB.ext[1]));
			corner2 = DirectX::XMVectorAdd(corner2, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[2], obj2->PC_OBB.ext[2]));

			corner3 = DirectX::XMVectorAdd(corner3, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[0], obj2->PC_OBB.ext[0]));
			corner3 = DirectX::XMVectorAdd(corner3, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[1], obj2->PC_OBB.ext[1]));
			corner3 = DirectX::XMVectorAdd(corner3, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[2], -obj2->PC_OBB.ext[2]));

			corner4 = DirectX::XMVectorAdd(corner4, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[0], -obj2->PC_OBB.ext[0]));
			corner4 = DirectX::XMVectorAdd(corner4, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[1], obj2->PC_OBB.ext[1]));
			corner4 = DirectX::XMVectorAdd(corner4, DirectX::XMVectorScale(obj2->PC_OBB.ort.r[2], -obj2->PC_OBB.ext[2]));

			if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner1), A, Apos))
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner1);
				nrOfCornersFound++;
			}
			if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner2), A, Apos))
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner2);
				nrOfCornersFound++;
			}
			if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner3), A, Apos) && nrOfCornersFound < 2)
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner3);
				nrOfCornersFound++;
			}
			if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner4), A, Apos) && nrOfCornersFound < 2)
			{
				corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner4);
				nrOfCornersFound++;
			}
			if (nrOfCornersFound < 2)
			{
				corner1 = DirectX::XMVectorScale(corner1, -1);
				corner2 = DirectX::XMVectorScale(corner2, -1);
				corner3 = DirectX::XMVectorScale(corner3, -1);
				corner4 = DirectX::XMVectorScale(corner4, -1);

				if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner1), A, Apos) && nrOfCornersFound < 2)
				{
					corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner1);
					nrOfCornersFound++;
				}
				if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner2), A, Apos) && nrOfCornersFound < 2)
				{
					corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner2);
					nrOfCornersFound++;
				}
				if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner3), A, Apos) && nrOfCornersFound < 2)
				{
					corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner3);
					nrOfCornersFound++;
				}
				if (this->IsPointInBox(DirectX::XMVectorAdd(Bpos, corner4), A, Apos) && nrOfCornersFound < 2)
				{
					corners[nrOfCornersFound] = DirectX::XMVectorAdd(Bpos, corner4);
					nrOfCornersFound++;
				}

			}
		}
		DirectX::XMVECTOR FINALPOINT;
		DirectX::XMVECTOR diffVec;
		if (nrOfCornersFound == 1)
		{
			FINALPOINT = corners[0];
		}
		else if (nrOfCornersFound == 2)
		{
			diffVec = DirectX::XMVectorSubtract(corners[0], corners[1]);
			diffVec = DirectX::XMVectorDivide(diffVec, DirectX::XMVectorSet(2, 2, 2, 2));

			FINALPOINT = DirectX::XMVectorAdd(corners[1], diffVec);

		}
		result = FINALPOINT;

		int a = 0;
	}


	//the corner intersecting the static OBB
	DirectX::XMVECTOR pointOfIntersection;
	pointOfIntersection = result;

	PhysicsComponent* componentToMove = nullptr;
	PhysicsComponent* staticComponent = nullptr;
	if (!obj1->PC_is_Static)
	{
		componentToMove = obj1;
		staticComponent = obj2;
	}
	else
	{
		componentToMove = obj2;
		staticComponent = obj1;
	}


	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(pointOfIntersection, staticComponent->PC_pos);
	DirectX::XMVECTOR normal = DirectX::XMVectorSet(0, 0, 0, 0);

	float dot[3];

	dot[0] = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, staticComponent->PC_OBB.ort.r[0]));
	dot[1] = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, staticComponent->PC_OBB.ort.r[1]));
	dot[2] = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, staticComponent->PC_OBB.ort.r[2]));

	if (dot[0] > staticComponent->PC_OBB.ext[0])
	{
		dot[0] = staticComponent->PC_OBB.ext[0];
	}
	else if (dot[0] < -staticComponent->PC_OBB.ext[0])
	{
		dot[0] = -staticComponent->PC_OBB.ext[0];
	}

	if (dot[1] > staticComponent->PC_OBB.ext[1])
	{
		dot[1] = staticComponent->PC_OBB.ext[1];
	}

	else if (dot[1] < -staticComponent->PC_OBB.ext[1])
	{
		dot[1] = -staticComponent->PC_OBB.ext[1];
	}

	if (dot[2] > staticComponent->PC_OBB.ext[2])
	{
		dot[2] = staticComponent->PC_OBB.ext[2];
	}
	else if (dot[2] < -staticComponent->PC_OBB.ext[2])
	{
		dot[2] = -staticComponent->PC_OBB.ext[2];
	}

	float test2[3];

	test2[0] = dot[0] / staticComponent->PC_OBB.ext[0];
	test2[1] = dot[1] / staticComponent->PC_OBB.ext[1];
	test2[2] = dot[2] / staticComponent->PC_OBB.ext[2];

	float axises[3];
	if (test2[0] >= 0.9)
	{
		axises[0] = 1;
	}
	else if (test2[0] <= -0.9)
	{
		axises[0] = -1;
	}
	else
	{
		axises[0] = 0;
	}

	if (test2[1] >= 0.9)
	{
		axises[1] = 1;
	}
	else if (test2[1] <= -0.9)
	{
		axises[1] = -1;
	}
	else
	{
		axises[1] = 0;
	}

	if (test2[2] >= 0.9)
	{
		axises[2] = 1;
	}
	else if (test2[2] <= -0.9)
	{
		axises[2] = -1;
	}
	else
	{
		axises[2] = 0;
	}

	for (int i = 0; i < 3; i++)
	{
		normal = DirectX::XMVectorAdd(normal, DirectX::XMVectorScale(staticComponent->PC_OBB.ort.r[i], axises[i]));
	}
	normal = DirectX::XMVector3Normalize(normal);
	DirectX::XMVECTOR toCorrect = DirectX::XMVectorSubtract(normal, DirectX::XMVectorMultiply(normal, DirectX::XMVectorSet(test2[0], test2[1], test2[2], 0)));
	DirectX::XMVECTOR POItoA = DirectX::XMVectorSubtract(componentToMove->PC_pos, pointOfIntersection);


	componentToMove->PC_pos = DirectX::XMVectorAdd(componentToMove->PC_pos, toCorrect);
	this->CollitionDynamics(componentToMove, staticComponent, normal, dt);
	componentToMove->PC_pos = DirectX::XMVectorAdd(componentToMove->PC_pos, DirectX::XMVectorScale(componentToMove->PC_velocity, dt));

	return pointOfIntersection;

}

bool PhysicsHandler::IsPointInBox(DirectX::XMVECTOR point, OBB *& src, DirectX::XMVECTOR BoxPos)
{
	bool result = true;

	float acceptanceRange = 0.09f;
	DirectX::XMVECTOR BoxToPoint;
	BoxToPoint = DirectX::XMVectorSubtract(point, BoxPos);

	DirectX::XMFLOAT3 scalar;

	//does pointOfIntersectA exist in the OBB B
	DirectX::XMStoreFloat(&scalar.x, DirectX::XMVector3Dot(BoxToPoint, src->ort.r[0]));
	DirectX::XMStoreFloat(&scalar.y, DirectX::XMVector3Dot(BoxToPoint, src->ort.r[1]));
	DirectX::XMStoreFloat(&scalar.z, DirectX::XMVector3Dot(BoxToPoint, src->ort.r[2]));

	result = scalar.x <= src->ext[0] + acceptanceRange && scalar.x >= -src->ext[0] - acceptanceRange;

	if (result == true)
	{
		result = scalar.y <= src->ext[1] + acceptanceRange && scalar.y >= -src->ext[1] - acceptanceRange;

		if (result == true)
		{
			result = scalar.z <= src->ext[2] + acceptanceRange && scalar.z >= -src->ext[2] - acceptanceRange;
		}
	}
	return result;
}

bool PhysicsHandler::SphereAABBIntersectionTest(PhysicsComponent * objSphere, PhysicsComponent * objAABB, bool doPhysics, float dt)
{
	//this section of the code found on http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=4
	DirectX::XMVECTOR sphere_local = DirectX::XMVectorSubtract(objSphere->PC_pos, objAABB->PC_pos);
	float sphere_pos[3];
	sphere_pos[0] = DirectX::XMVectorGetX(sphere_local);
	sphere_pos[1] = DirectX::XMVectorGetY(sphere_local);
	sphere_pos[2] = DirectX::XMVectorGetZ(sphere_local);

	float box_axis[3];
	box_axis[0] = objAABB->PC_AABB.ext[0];
	box_axis[1] = objAABB->PC_AABB.ext[1];
	box_axis[2] = objAABB->PC_AABB.ext[2];


	float s = 0;
	float d = 0;

	for (int i = 0; i < 3; i++)
	{
		if (sphere_pos[i] < -box_axis[i])
		{
			s = sphere_pos[i] - (-box_axis[i]);
			d += s*s;
		}
		else if (sphere_pos[i] > box_axis[i])
		{
			s = sphere_pos[i] - box_axis[i];
			d += s*s;
		}
	}

	bool result = d <= (objSphere->PC_Sphere.radius * objSphere->PC_Sphere.radius);
	//-------------

	if (result && doPhysics)
	{
		float axises[3];
		axises[0] = 0.0f;
		axises[1] = 0.0f;
		axises[2] = 0.0f;

		int correction_axis_index = -1;
		int correction_axis_dir = 0;
		DirectX::XMVECTOR vec;
		for (int i = 0; i < 3; i++)
		{
			vec = DirectX::XMVectorSet(0, 0, 0, 0);
			vec = DirectX::XMVectorSetByIndex(vec, objAABB->PC_AABB.ext[i], i);
			float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphere_local, DirectX::XMVector3Normalize(vec)));
			axises[i] = dot;
			if (dot != 0.0f)
			{
				if (dot > objAABB->PC_AABB.ext[i])
				{
					axises[i] = objAABB->PC_AABB.ext[i];
				}
				if (dot < -objAABB->PC_AABB.ext[i])
				{
					axises[i] = -objAABB->PC_AABB.ext[i];
				}
			}
		}
		DirectX::XMVECTOR pointOfIntersection = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR toAdd = DirectX::XMVectorSet(axises[0], axises[1], axises[2], 0);
		pointOfIntersection = DirectX::XMVectorAdd(pointOfIntersection, toAdd);

		DirectX::XMVECTOR diffVec = DirectX::XMVectorSet(0, 0, 0, 0);
		diffVec = DirectX::XMVectorSubtract(sphere_local, pointOfIntersection);
		
		float lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));

		diffVec = DirectX::XMVector3Normalize(diffVec);



		DirectX::XMVECTOR toMove = DirectX::XMVectorSubtract(DirectX::XMVectorScale(diffVec, objSphere->PC_Sphere.radius), DirectX::XMVectorScale(diffVec, lenght));

		objSphere->PC_pos = DirectX::XMVectorAdd(objSphere->PC_pos, toMove);

		this->CollitionDynamics(objSphere, objAABB, diffVec, dt);
		objSphere->PC_normalForce = diffVec;
		if (!objAABB->PC_is_Static)
		{
			objAABB->PC_normalForce = DirectX::XMVectorScale(diffVec, -1);
		}
	}

	return result;
}

bool PhysicsHandler::SphereOBBIntersectionTest(PhysicsComponent * objSphere, PhysicsComponent * objOBB, float dt)
{
	float radX = DirectX::XMVectorGetX(objOBB->PC_rotation);
	float radY = DirectX::XMVectorGetY(objOBB->PC_rotation);
	float radZ = DirectX::XMVectorGetZ(objOBB->PC_rotation);

	//create rotation matrix to translate the sphere into OBBs local space and do SphereVSAABB test
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationX(radX) * DirectX::XMMatrixRotationY(radY) * DirectX::XMMatrixRotationZ(radZ);
	DirectX::XMVECTOR t;
	DirectX::XMVECTOR toSphere;
	DirectX::XMVECTOR originalToSphere;

	DirectX::XMVECTOR originalVel;

	DirectX::XMMATRIX rotInv = DirectX::XMMatrixInverse(&t, rot);

	toSphere = DirectX::XMVectorSubtract(objSphere->PC_pos, objOBB->PC_pos);
	toSphere = DirectX::XMVector3Transform(toSphere, rotInv);
	toSphere = DirectX::XMVectorSetByIndex(toSphere, 0, 3);

	AABB aabb;
	aabb.ext[0] = objOBB->PC_OBB.ext[0];
	aabb.ext[1] = objOBB->PC_OBB.ext[1];
	aabb.ext[2] = objOBB->PC_OBB.ext[2];

	Sphere* sphere_ptr = &objSphere->PC_Sphere;
	AABB* aabb_ptr = &aabb;

	bool result = this->SphereAABBIntersectionTest(sphere_ptr, toSphere, aabb_ptr, DirectX::XMVectorSet(0, 0, 0, 0));

	if (result)
	{
		DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(objSphere->PC_pos, objOBB->PC_pos);
		DirectX::XMVECTOR pointOfIntersection = objOBB->PC_pos;

		float dot[3];
		for (int i = 0; i < 3; i++)
		{
			dot[i] = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, objOBB->PC_OBB.ort.r[i]));
			if (dot[i] > objOBB->PC_OBB.ext[i])
			{
				dot[i] = objOBB->PC_OBB.ext[i];
			}
			else if (dot[i] < -objOBB->PC_OBB.ext[i])
			{
				dot[i] = -objOBB->PC_OBB.ext[i];
			}

			pointOfIntersection = DirectX::XMVectorAdd(pointOfIntersection, DirectX::XMVectorScale(objOBB->PC_OBB.ort.r[i], dot[i]));
		}
		//pointOfIntersection = DirectX::XMVectorAdd(pointOfIntersection, DirectX::XMVectorScale(objOBB->PC_OBB.ort.r[0], dot[0]));

		DirectX::XMVECTOR POI_to_sphere = DirectX::XMVectorSubtract(objSphere->PC_pos, pointOfIntersection);

		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(POI_to_sphere));

		float distanceToMove = objSphere->PC_Sphere.radius - distance;

		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(POI_to_sphere);
		DirectX::XMVECTOR toMove = DirectX::XMVectorScale(normal, distanceToMove);
		if (!objSphere->PC_steadfast)
		{
			objSphere->PC_pos = DirectX::XMVectorAdd(objSphere->PC_pos, toMove);
			this->CollitionDynamics(objSphere, objOBB, normal, dt);
			objSphere->PC_normalForce = normal;
		}
		else
		{

		}

	}

	return result;

}

bool PhysicsHandler::SphereSphereIntersectionTest(PhysicsComponent * objSphere1, PhysicsComponent * objSphere2, float dt)
{
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(objSphere1->PC_pos, objSphere2->PC_pos);
	float d = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));

	float totalLenght = (objSphere1->PC_Sphere.radius + objSphere2->PC_Sphere.radius);
	bool result = d <= totalLenght;

	if (result)
	{
		if (d < totalLenght)
		{
			diffVec = DirectX::XMVector3Normalize(diffVec);
			DirectX::XMVECTOR toMove = DirectX::XMVectorScale(diffVec, totalLenght - d);
			objSphere1->PC_pos = DirectX::XMVectorAdd(objSphere1->PC_pos, toMove);
			this->CollitionDynamics(objSphere1, objSphere2, diffVec, dt);
			objSphere1->PC_normalForce = diffVec;
			if (!objSphere2->PC_is_Static)
			{
				objSphere2->PC_normalForce = DirectX::XMVectorScale(diffVec, -1);
			}
		}
	}

	return result;
}

bool PhysicsHandler::SpherePlaneIntersectionTest(PhysicsComponent * objSphere, PhysicsComponent * objPlane, float dt)
{
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(objSphere->PC_pos, objPlane->PC_pos);
	DirectX::XMVECTOR pParallel;
	DirectX::XMVECTOR pPerpendicular;

	DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular, diffVec, objPlane->PC_Plane.PC_normal);

	float d = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));

	bool result = d <= objSphere->PC_Sphere.radius;

	if (result)
	{
		float lenght = (objSphere->PC_Sphere.radius - d);
		objSphere->PC_pos = DirectX::XMVectorAdd(objSphere->PC_pos, DirectX::XMVectorScale(pParallel, lenght));

		this->CollitionDynamics(objSphere, objPlane, objPlane->PC_Plane.PC_normal, dt);


		objSphere->PC_normalForce = objPlane->PC_Plane.PC_normal;
	}

	return result;
}

bool PhysicsHandler::AABBPlaneIntersectionTest(PhysicsComponent * objAABB, PhysicsComponent * objPlane, float dt)
{
	//this section of the code taken from http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=7
	float d = 0;
	float a1 = objAABB->PC_AABB.ext[0];
	float a2 = objAABB->PC_AABB.ext[1];
	float a3 = objAABB->PC_AABB.ext[2];

	float sum = 0;
	float dot1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(objPlane->PC_Plane.PC_normal, DirectX::XMVectorSet(a1 / a1, 0, 0, 0)));
	float dot2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(objPlane->PC_Plane.PC_normal, DirectX::XMVectorSet(0, a2 / a2, 0, 0)));
	float dot3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(objPlane->PC_Plane.PC_normal, DirectX::XMVectorSet(0, 0, a3 / a3, 0)));

	sum += a1 * fabs(dot1);
	sum += a2 * fabs(dot2);
	sum += a3 * fabs(dot3);

	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(objAABB->PC_pos, objPlane->PC_pos);
	DirectX::XMVECTOR pParallel;
	DirectX::XMVECTOR dump;

	DirectX::XMVector3ComponentsFromNormal(&pParallel, &dump, diffVec, objPlane->PC_Plane.PC_normal);

	DirectX::XMVECTOR temp_pos = DirectX::XMVectorSubtract(objAABB->PC_pos, pParallel);

	diffVec = DirectX::XMVectorSubtract(objAABB->PC_pos, temp_pos);
	pParallel = DirectX::XMVectorSet(0, 0, 0, 0);
	DirectX::XMVector3ComponentsFromNormal(&pParallel, &dump, diffVec, objPlane->PC_Plane.PC_normal);

	d = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));

	bool result = d < sum;
	//---------------------------
	if (result)
	{
		float t = sum - d;

		DirectX::XMVECTOR toMove = DirectX::XMVectorScale(objPlane->PC_Plane.PC_normal, t);

		objAABB->PC_pos = DirectX::XMVectorAdd(objAABB->PC_pos, toMove);
		this->CollitionDynamics(objAABB, objPlane, objPlane->PC_Plane.PC_normal, dt);

		objAABB->PC_normalForce = objPlane->PC_normalForce;

	}

	return result;
}

bool PhysicsHandler::OBBPlaneIntersectionTest(PhysicsComponent * objOBB, PhysicsComponent * objPlane, float dt)
{
	//this section of the code taken from http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=7
	float a1 = objOBB->PC_OBB.ext[0];
	float a2 = objOBB->PC_OBB.ext[1];
	float a3 = objOBB->PC_OBB.ext[2];

	float sum = 0;
	float dot1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(objPlane->PC_Plane.PC_normal, objOBB->PC_OBB.ort.r[0]));
	float dot2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(objPlane->PC_Plane.PC_normal, objOBB->PC_OBB.ort.r[1]));
	float dot3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(objPlane->PC_Plane.PC_normal, objOBB->PC_OBB.ort.r[2]));

	sum += a1 * abs(dot1);
	sum += a2 * abs(dot2);
	sum += a3 * abs(dot3);

	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(objOBB->PC_pos, objPlane->PC_pos);
	DirectX::XMVECTOR pParallel;
	DirectX::XMVECTOR pPerpendicular;

	DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular, diffVec, objPlane->PC_Plane.PC_normal);

	float d = 0;

	d = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));

	bool result = d < sum;
	//-----------------------------------
	if (result)
	{
		float t = sum - d;
		DirectX::XMVECTOR toMove = DirectX::XMVectorScale(objPlane->PC_Plane.PC_normal, t);

		objOBB->PC_pos = DirectX::XMVectorAdd(objOBB->PC_pos, toMove);
		this->CollitionDynamics(objOBB, objPlane, objPlane->PC_Plane.PC_normal, dt);

		objOBB->PC_normalForce = objPlane->PC_normalForce;

	}

	return result;
}

void PhysicsHandler::CollitionDynamics(PhysicsComponent* obj1, PhysicsComponent* obj2, DirectX::XMVECTOR normal, float dt)
{
	if (obj2->PC_is_Static)
	{
		DirectX::XMVECTOR pParallel;
		DirectX::XMVECTOR pPerpendicular;
		DirectX::XMVECTOR velDir = DirectX::XMVector3Normalize(obj1->PC_velocity);
		DirectX::XMVECTOR oldVel = obj1->PC_velocity;
		DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular, oldVel, normal);
		float old_parallelVel = DirectX::XMVectorGetX(DirectX::XMVector3Length(pParallel));
		float old_perpendicularVel = DirectX::XMVectorGetX(DirectX::XMVector3Length(pPerpendicular));

		float e = (obj1->PC_elasticity + obj2->PC_elasticity) / 2;
		float frictionConstant = (obj1->PC_friction + obj2->PC_friction) / 2;


		float newParallelVel = (old_parallelVel * e);

		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(velDir, normal)) < 0)
		{
			velDir = DirectX::XMVector3Reflect(velDir, normal);
			//for direction when new force is applied
			pParallel = DirectX::XMVectorScale(pParallel, -1);
			old_parallelVel *= -1;
		}
		pParallel = DirectX::XMVector3Normalize(pParallel);
		//pPerpendicular = DirectX::XMVector3Normalize(pPerpendicular);

		pParallel = DirectX::XMVectorSetByIndex(pParallel, 0, 3);
		pPerpendicular = DirectX::XMVectorSetByIndex(pPerpendicular, 0, 3);

		float parallelImpuls = obj1->PC_mass * newParallelVel - obj1->PC_mass * old_parallelVel;
		float parallelForce = parallelImpuls / dt;

		//float newPerpendicularVel = old_perpendicularVel * obj2->PC_friction;
		//float perpendicularImpuls = obj1->PC_mass * newPerpendicularVel - obj1->PC_mass * old_perpendicularVel;
		//float perpendicularForce = perpendicularImpuls / dt;
		//this->ApplyForceToComponent(obj1, DirectX::XMVectorScale(pPerpendicular, perpendicularForce), dt);


		float frictionForce = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorScale(DirectX::XMVectorScale(pParallel, parallelForce), frictionConstant)));
		float maxFriction = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorScale(this->m_gravity, frictionConstant))));
		if (frictionForce > maxFriction)
		{
			frictionForce = maxFriction;
		}
		DirectX::XMVECTOR frictionFORCE = DirectX::XMVectorScale(DirectX::XMVectorScale(pPerpendicular, -1), frictionConstant);
		//frictionFORCE = DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVectorScale(pPerpendicular, -1)), frictionForce);

		this->ApplyForceToComponent(obj1, DirectX::XMVectorScale(pParallel, parallelForce), dt);
		this->ApplyForceToComponent(obj1, frictionFORCE, dt);

	}
	else
	{
		DirectX::XMVECTOR pParallel;
		DirectX::XMVECTOR pPerpendicular1 = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR pPerpendicular2 = DirectX::XMVectorSet(0, 0, 0, 0);

		float v1_old[3];
		float v1_new[3];
		float m1 = obj1->PC_mass;
		DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular1, obj1->PC_velocity, normal);
		v1_old[0] = DirectX::XMVectorGetX(pParallel);
		v1_old[1] = DirectX::XMVectorGetY(pParallel);
		v1_old[2] = DirectX::XMVectorGetZ(pParallel);

		float v2_old[3];
		float v2_new[3];
		float m2 = obj2->PC_mass;
		DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular2, obj2->PC_velocity, normal);
		v2_old[0] = DirectX::XMVectorGetX(pParallel);
		v2_old[1] = DirectX::XMVectorGetY(pParallel);
		v2_old[2] = DirectX::XMVectorGetZ(pParallel);

		float e = (obj1->PC_elasticity + obj2->PC_elasticity) / 2;

		for (int i = 0; i < 3; i++)
		{
			//not sure what value e should be

			v1_new[i] = (v1_old[i] * (m1 - m2*e) + m2*v2_old[i] * (1.0f + e)) / (m1 + m2);

			v2_new[i] = (v1_old[i] * m1*(1.0f + e) + (m2 - e*m1)*v2_old[i]) / (m1 + m2);

		}

		DirectX::XMVECTOR pParallelForce1;
		pParallelForce1 = DirectX::XMVectorSubtract(DirectX::XMVectorScale(DirectX::XMVectorSet(v1_new[0], v1_new[1], v1_new[2], 0), obj1->PC_mass), DirectX::XMVectorScale(DirectX::XMVectorSet(v1_old[0], v1_old[1], v1_old[2], 0), obj1->PC_mass));
		pParallelForce1 = DirectX::XMVectorScale(pParallelForce1, 1 / dt);

		DirectX::XMVECTOR pParallelForce2;
		pParallelForce2 = DirectX::XMVectorSubtract(DirectX::XMVectorScale(DirectX::XMVectorSet(v2_new[0], v2_new[1], v2_new[2], 0), obj2->PC_mass), DirectX::XMVectorScale(DirectX::XMVectorSet(v2_old[0], v2_old[1], v2_old[2], 0), obj2->PC_mass));
		pParallelForce2 = DirectX::XMVectorScale(pParallelForce2, 1 / dt);

		float frictionConstant = (obj1->PC_friction + obj2->PC_friction) / 2;


		DirectX::XMVECTOR frictionForce1 = DirectX::XMVectorSet(0, 0, 0, 0);
		frictionForce1 = DirectX::XMVectorScale(DirectX::XMVectorScale(pPerpendicular1, -1), frictionConstant);
		frictionForce1 = DirectX::XMVectorAdd(frictionForce1, DirectX::XMVectorScale(pPerpendicular2, frictionConstant));

		DirectX::XMVECTOR frictionForce2 = DirectX::XMVectorSet(0, 0, 0, 0);
		frictionForce2 = DirectX::XMVectorScale(DirectX::XMVectorScale(pPerpendicular2, -1), frictionConstant);
		frictionForce2 = DirectX::XMVectorAdd(frictionForce2, DirectX::XMVectorScale(pPerpendicular1, frictionConstant));

		this->ApplyForceToComponent(obj1, pParallelForce1, dt);
		this->ApplyForceToComponent(obj1, frictionForce1, dt);

		this->ApplyForceToComponent(obj2, pParallelForce2, dt);
		this->ApplyForceToComponent(obj2, frictionForce2, dt);

	}


}

float PhysicsHandler::DotProduct(const DirectX::XMFLOAT3 & v1, const DirectX::XMFLOAT3 & v2) const
{
	DirectX::XMVECTOR temp1;
	DirectX::XMVECTOR temp2;
	float result = 0;
	
	temp1 = DirectX::XMLoadFloat3(&v1);
	temp2 = DirectX::XMLoadFloat3(&v2);

	DirectX::XMStoreFloat(&result, DirectX::XMVector3Dot(temp1, temp2));

	return result;
}

DirectX::XMFLOAT3 PhysicsHandler::CrossProduct(const DirectX::XMFLOAT3 & v1, const DirectX::XMFLOAT3 & v2) const
{
	DirectX::XMFLOAT3 result = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR temp1;
	DirectX::XMVECTOR temp2;

	temp1 = DirectX::XMLoadFloat3(&v1);
	temp2 = DirectX::XMLoadFloat3(&v2);

	temp1 = DirectX::XMVector2Cross(temp1, temp2);
	DirectX::XMStoreFloat3(&result, temp1);
	
	return result;
}

float PhysicsHandler::DotProduct(const DirectX::XMVECTOR & v1, const DirectX::XMVECTOR & v2) const
{
	DirectX::XMVECTOR cont;
	float result = 0.0f;

	cont = DirectX::XMVector3Dot(v1, v2);
	DirectX::XMStoreFloat(&result, cont);

	return result;
}

DirectX::XMFLOAT3 PhysicsHandler::VectorSubstract(const DirectX::XMFLOAT3 & v1, const DirectX::XMFLOAT3 & v2) const
{
	DirectX::XMFLOAT3 result;
	
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	
	return result;
}

DirectX::XMMATRIX PhysicsHandler::RotateBB_X(PhysicsComponent* src, const float &radian)
{
	DirectX::XMMATRIX xMatrix;
	DirectX::XMMATRIX test = src->PC_OBB.ort;

	src->PC_rotation = DirectX::XMVectorSetByIndex(src->PC_rotation, radian, 0);
	//read the value of PC_rotation
	//DirectX::XMStoreFloat3(&rot,src->PC_rotation);
	float rotAngle = (radian);

	xMatrix = DirectX::XMMatrixRotationX(radian);
	test = DirectX::XMMatrixMultiply(test, xMatrix);


	//xMatrix = DirectX::XMMatrixRotationX((3.14159265359 / 4.0));
	//test = DirectX::XMMatrixMultiply(test, xMatrix);
	//xMatrix = DirectX::XMMatrixRotationZ((3.14159265359 / 4.0));
	//test = DirectX::XMMatrixMultiply(test, xMatrix);

	src->PC_OBB.ort = test;

	return xMatrix;
}

DirectX::XMMATRIX PhysicsHandler::RotateBB_Y(PhysicsComponent* src, const float &radian)
{
	DirectX::XMMATRIX xMatrix;
	DirectX::XMMATRIX test = src->PC_OBB.ort;
	src->PC_rotation = DirectX::XMVectorSetByIndex(src->PC_rotation, radian, 1);

	xMatrix = DirectX::XMMatrixRotationY(radian);
	test = DirectX::XMMatrixMultiply(test, xMatrix);

	src->PC_OBB.ort = test;

	return xMatrix;
}

DirectX::XMMATRIX PhysicsHandler::RotateBB_Z(PhysicsComponent* src, const float &radian)
{
	DirectX::XMMATRIX xMatrix;
	DirectX::XMMATRIX test = src->PC_OBB.ort;
	src->PC_rotation = DirectX::XMVectorSetByIndex(src->PC_rotation, radian, 2);

	float rotAngle = (3.14159265359f / 180.0f);

	xMatrix = DirectX::XMMatrixRotationZ(radian);
	test = DirectX::XMMatrixMultiply(test, xMatrix);

	src->PC_OBB.ort = test;

	return xMatrix;
}

void PhysicsHandler::CreateDefaultBB(const DirectX::XMVECTOR & pos, PhysicsComponent * src)
{
	this->CreateDefaultAABB(pos, src);
	this->CreateDefaultOBB(pos, src);
}

void PhysicsHandler::CreateDefaultAABB(const DirectX::XMVECTOR & pos, PhysicsComponent* src)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, pos);

	OBB* temp2 = &src->PC_OBB;

	//AABB components
	src->PC_AABB.ext[0] = 1.0;
	src->PC_AABB.ext[1] = 1.0;
	src->PC_AABB.ext[2] = 1.0;
}

void PhysicsHandler::CreateDefaultOBB(const DirectX::XMVECTOR & pos, PhysicsComponent* src)
{
	//AABB components

	src->PC_pos = pos;

	src->PC_OBB.ext[0] = 1.0f;
	src->PC_OBB.ext[1] = 1.0f;
	src->PC_OBB.ext[2] = 1.0f;

	src->PC_OBB.ort = DirectX::XMMatrixIdentity();
}

void PhysicsHandler::SetStartIndex(unsigned int newStartIndex)
{
	this->m_startIndex = newStartIndex;
}

void PhysicsHandler::SetNumberOfDynamics(unsigned int newNumberOfDynamics)
{
	this->m_numberOfDynamics = newNumberOfDynamics;
}

void PhysicsHandler::SetIsHost(bool newIsHost)
{
	this->m_isHost = newIsHost;
}

PhysicsHandler::PhysicsHandler()
{
}

PhysicsHandler::~PhysicsHandler()
{
}

bool PhysicsHandler::Initialize()
{
	this->m_gravity = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	this->m_startIndex = 0;
	this->m_nrOfStaticObjects = this->m_physicsComponents.size();
	this->m_isHost = true;
	this->m_bullet.Initialize();

	DirectX::XMMATRIX child = DirectX::XMMatrixRotationX((3.14 / 180) * 45);
	DirectX::XMMATRIX parent = DirectX::XMMatrixRotationZ((3.14 / 180) * 45);

	DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, parent), child);

	return true;
}

void PhysicsHandler::ShutDown()
{
	int size = this->m_physicsComponents.size();
	for (int i = 0; i < size; i++)
	{
		delete this->m_physicsComponents.at(i);
	}
}

void PhysicsHandler::Update(float deltaTime)
{


	float dt = (deltaTime / 1000000);
	//dt = (deltaTime / 50000);
	std::vector<PhysicsComponent*>::iterator toProcess = this->m_physicsComponents.begin();
	int i = 0;
	//if (this->m_playerRagDoll.state == ANIMATED_TRANSITION)
	//{
	//	//this->m_playerRagDoll.playerPC->PC_OBB.ort = this->m_playerRagDoll.lowerBody.center->PC_OBB.ort;
	//	this->m_playerRagDoll.state = ANIMATED;
	//}
	if (this->m_playerRagDoll.state == ANIMATED)
	{

		this->SetRagdollToBindPose(&this->m_playerRagDoll, this->m_playerRagDoll.playerPC->PC_pos);
		float rightfootVel = DirectX::XMVectorGetX(DirectX::XMVector3Length(this->m_playerRagDoll.rightLeg.next2->PC_velocity));
		float ballVel = DirectX::XMVectorGetX(DirectX::XMVector3Length(this->m_playerRagDoll.ballPC->PC_velocity));
		if (rightfootVel > 0.7 && ballVel > 0.7)
		{
			//this->m_playerRagDoll.state = RAGDOLL;

		}
	}
	if (this->m_playerRagDoll.state == RAGDOLL_TRANSITION)
	{
		this->m_playerRagDoll.state = RAGDOLL;
	}
	int size = this->m_physicsComponents.size();
	for (toProcess; toProcess != this->m_physicsComponents.end(); toProcess++)
	{
		PhysicsComponent* temp = nullptr;
		temp = *(toProcess);
		this->m_bullet.SyncBulletWithGame((*(toProcess)));
	}

	//sync positions with bullet world
	this->m_bullet.UpdateBulletEngine(dt);

	//
	for (int i = 0; i < size; i++)
	{
		PhysicsComponent* ptr = this->GetComponentAt(i);

		this->m_bullet.Update(ptr, i, dt);
	}


	for (size_t i = 0; i < this->m_physicsComponents.size(); i++)
	{
		PhysicsComponent* ptr = this->m_physicsComponents.at(i);
		if (this->m_physicsComponents.at(i)->PC_steadfast == true)
		{
			DirectX::XMVECTOR velocity = ptr->PC_velocity;
			velocity = DirectX::XMVectorScale(velocity, dt);

			ptr->PC_pos = DirectX::XMVectorAdd(ptr->PC_pos, velocity);
		}
	}

	int nrOfChainLinks = this->m_links.size();
	if (this->m_playerRagDoll.state == RAGDOLL)
	{
		for (int i = 0; i < nrOfChainLinks; i++)
		{
			this->DoChainPhysics(&this->m_links.at(i), dt);
		}
		this->AdjustRagdoll(&this->m_playerRagDoll, dt);
		for (int i = 0; i < nrOfChainLinks; i++)
		{
			this->AdjustChainLinkPosition(&this->m_links.at(i));
		}
		this->DoRagdollIntersection(dt);
	}

}

void PhysicsHandler::DoRagdollIntersection(float dt)
{
	float resultingVelLenght = 0;

	int nrOfBodyParts = this->m_bodyPC.size();

	int nrOfStaticObjects = this->m_staticComponents.size();
	if (this->m_playerRagDoll.state == RAGDOLL)
	{
		this->m_playerRagDoll.playerPC->PC_pos = DirectX::XMVectorAdd(this->m_playerRagDoll.lowerBody.center->PC_pos, DirectX::XMVectorSet(0, -1.6, 0, 0));

		for (int i = 0; i < nrOfBodyParts; i++)
		{
			PhysicsComponent* current = nullptr;
			current = this->m_bodyPC.at(i);
			for (int y = 0; y < nrOfStaticObjects; y++)
			{
				PhysicsComponent* toCompare = nullptr;
				toCompare = this->m_staticComponents.at(y);
				if (current->PC_BVtype == BV_OBB)
				{
					if (toCompare->PC_BVtype == BV_Plane)
					{
						//this->SpherePlaneIntersectionTest(current, toCompare, dt);
					}
					if (toCompare->PC_BVtype == BV_AABB)
					{
						//this->SphereAABBIntersectionTest(current, toCompare, true, dt);
					}
				}
			}

			DirectX::XMVECTOR gravityForce = DirectX::XMVectorScale(DirectX::XMVectorScale(this->m_gravity, (float)current->PC_gravityInfluence), current->PC_mass);

			this->ApplyForceToComponent(current, gravityForce, dt);

			if (!current->PC_is_Static)
			{
				current->PC_pos = DirectX::XMVectorAdd(current->PC_pos, DirectX::XMVectorScale(current->PC_velocity, dt));
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, current->PC_pos);
				if (temp.y < -7)
				{
					//current->PC_pos = DirectX::XMVectorSet(temp.x, 2, temp.z, 0);
					//current->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
				}
			}
			else
			{
				current->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
			}
			resultingVelLenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(current->PC_velocity));
		}
		if (resultingVelLenght < 0.1)
		{
			this->m_ragdollNotMovingCounter++;
			if (m_ragdollNotMovingCounter > 500)
			{
				//this->m_playerRagDoll.state = ANIMATED;
			}
		}
		else
		{
			this->m_ragdollNotMovingCounter = 0;
		}
		this->SyncRagdollWithSkelton(&this->m_playerRagDoll);
		//if (DirectX::XMVectorGetX(DirectX::XMVector3Length(this->playerRagDoll.upperBody.center->PC_velocity)) < 1)
		//{
		//	this->playerRagDoll.state = ANIMATED;
		//}
	}
	else
	{
		//assume player is on index 0
		PhysicsComponent* player = this->m_playerRagDoll.playerPC;
		//this->ResetRagdollToTPose(DirectX::XMVectorAdd(player->PC_pos, DirectX::XMVectorSet(0, player->PC_AABB.ext[1] + 0.5, 0, 0)));
		this->m_ragdollNotMovingCounter = 0;
	}

}



void PhysicsHandler::CheckFieldIntersection()
{
	Field* field = nullptr;
	int nrOfFields = this->m_fields.size();
	for (int i = 0; i < nrOfFields; i++)
	{
		field = &this->m_fields.at(i);

		DirectX::XMVECTOR fieldPos = DirectX::XMLoadFloat3(&field->F_pos);
		PhysicsComponent* ptr = nullptr;
		int nrOfPhysicsComponents = this->m_dynamicComponents.size();
		for (int y = 0; y < nrOfPhysicsComponents; y++)
		{
			ptr = this->m_dynamicComponents.at(y);
			bool result = false;
			if (ptr->PC_entityID == field->F_entitityID1 || ptr->PC_entityID == field->F_entitityID2)
			{
				if (ptr->PC_BVtype == BV_AABB)
				{
					OBB* obb_ptr = &field->F_BV;
					AABB* aabb_ptr = &ptr->PC_AABB;
					result = this->OBBAABBIntersectionTest(obb_ptr, fieldPos, aabb_ptr, ptr->PC_pos);
				}
				else if (ptr->PC_BVtype == BV_Sphere)
				{
					OBB* obb_ptr = &field->F_BV;
					Sphere* sphere_ptr = &ptr->PC_Sphere;

					result = this->SphereOBBIntersectionTest(sphere_ptr, ptr->PC_pos, obb_ptr, fieldPos, ptr->PC_rotation);
				}
				else if (ptr->PC_BVtype == BV_OBB)
				{
					OBB* FIELD_obb_ptr = &field->F_BV;
					OBB* PC_obb_ptr = &ptr->PC_OBB;

					result = this->OBBOBBIntersectionTest(FIELD_obb_ptr, fieldPos, PC_obb_ptr, ptr->PC_pos);
				}
				if (result)
				{
					//intersection found
					if (ptr->PC_entityID == field->F_entitityID1)
					{
						field->F_first_inside = true;
					}
					if (ptr->PC_entityID == field->F_entitityID2)
					{
						field->F_second_inside = true;
					}
				}
				else
				{
					//no intersection found
					if (ptr->PC_entityID == field->F_entitityID1)
					{
						field->F_first_inside = false;
					}
					if (ptr->PC_entityID == field->F_entitityID2)
					{
						field->F_second_inside = false;
					}
				}
			}
		}
	}
}

void PhysicsHandler::TranslateBB(const DirectX::XMVECTOR &newPos, PhysicsComponent* src)
{
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, newPos);

	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(temp.x, temp.y, temp.z);
	
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	src->PC_pos = DirectX::XMVector3Transform(src->PC_pos, worldMatrix);
}

void PhysicsHandler::Add_toRotateVec(PhysicsComponent* src)
{
	src->PC_rotation = DirectX::XMVectorAdd(src->PC_rotation, src->PC_rotationVelocity);
}

void PhysicsHandler::DoChainPhysics(PhysicsLink * link, float dt)
{
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(link->PL_previous->PC_pos, link->PL_next->PC_pos);
	float lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));
	diffVec = DirectX::XMVector3Normalize(diffVec);

	if (link->PL_type == PL_BODY)
	{
		int type = 0;
		if (type == 0)
		{
			if (lenght >= link->PL_lenght)
			{
				//force = DirectX::XMVectorScale(link->PL_next->PC_velocity, -0.5);
				//DirectX::XMVECTOR para;
				//DirectX::XMVECTOR perp;

				//DirectX::XMVector3ComponentsFromNormal(&para, &perp, link->PL_next->PC_velocity, diffVec);
				//link->PL_previous->PC_velocity = DirectX::XMVectorScale(para, 0.9);


				DirectX::XMVECTOR force = DirectX::XMVectorScale(DirectX::XMVectorScale(diffVec, lenght - link->PL_lenght), 0.9);
				force = DirectX::XMVectorAdd(force, DirectX::XMVectorScale(DirectX::XMVectorSubtract(link->PL_previous->PC_velocity, link->PL_next->PC_velocity), 0.999));
				//force = DirectX::XMVectorAdd(force, DirectX::XMVectorScale(link->PL_next->PC_velocity,0.1));

				this->ApplyForceToComponent(link->PL_next, force, dt);
				force = DirectX::XMVectorScale(force, -1.0);
				this->ApplyForceToComponent(link->PL_previous, force, dt);
			}
		}
		else if (type == 1)
		{
			if (lenght >= link->PL_lenght)
			{
				DirectX::XMVECTOR pParallel;
				DirectX::XMVECTOR pPerpendicular1;
				DirectX::XMVECTOR pPerpendicular2;

				float v1_old[3];
				float v1_new[3];
				float m1 = link->PL_previous->PC_mass;
				DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular1, link->PL_previous->PC_velocity, diffVec);
				v1_old[0] = DirectX::XMVectorGetX(pParallel);
				v1_old[1] = DirectX::XMVectorGetY(pParallel);
				v1_old[2] = DirectX::XMVectorGetZ(pParallel);

				float v2_old[3];
				float v2_new[3];
				float m2 = link->PL_next->PC_mass;
				DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular2, link->PL_next->PC_velocity, diffVec);
				v2_old[0] = DirectX::XMVectorGetX(pParallel);
				v2_old[1] = DirectX::XMVectorGetY(pParallel);
				v2_old[2] = DirectX::XMVectorGetZ(pParallel);

				float e = 0.5;

				for (int i = 0; i < 3; i++)
				{

					v1_new[i] = (v1_old[i] * (m1 - m2*e) + m2*v2_old[i] * (1 + e)) / (m1 + m2);

					v2_new[i] = (v1_old[i] * m1*(1 + e) + (m2 - e*m1)*v2_old[i]) / (m1 + m2);

				}
				v1_old[0] += DirectX::XMVectorGetX(pPerpendicular1);
				v1_old[1] += DirectX::XMVectorGetY(pPerpendicular1);
				v1_old[2] += DirectX::XMVectorGetZ(pPerpendicular1);

				v2_old[0] += DirectX::XMVectorGetX(pPerpendicular2);
				v2_old[1] += DirectX::XMVectorGetY(pPerpendicular2);
				v2_old[2] += DirectX::XMVectorGetZ(pPerpendicular2);

				v1_new[0] += DirectX::XMVectorGetX(pPerpendicular1);
				v1_new[1] += DirectX::XMVectorGetY(pPerpendicular1);
				v1_new[2] += DirectX::XMVectorGetZ(pPerpendicular1);

				v2_new[0] += DirectX::XMVectorGetX(pPerpendicular2);
				v2_new[1] += DirectX::XMVectorGetY(pPerpendicular2);
				v2_new[2] += DirectX::XMVectorGetZ(pPerpendicular2);


				float forceVec1[3];
				float forceVec2[3];

				for (int i = 0; i < 3; i++)
				{
					forceVec1[i] = (m1 * v1_new[i] - m1 * v1_old[i]) / dt;
					forceVec2[i] = (m2 * v2_new[i] - m2 * v2_old[i]) / dt;
				}

				this->ApplyForceToComponent(link->PL_previous, DirectX::XMVectorSet(forceVec1[0], forceVec1[1], forceVec1[2], 0), dt);
				this->ApplyForceToComponent(link->PL_next, DirectX::XMVectorSet(forceVec2[0], forceVec2[1], forceVec2[2], 0), dt);

				//if (!link->PL_previous->PC_is_Static)
				//{
				//	link->PL_previous->PC_velocity = DirectX::XMVectorSet(v1_new[0], v1_new[1], v1_new[2], 0);
				//}
				//if (!link->PL_next->PC_is_Static)
				//{
				//	link->PL_next->PC_velocity = DirectX::XMVectorSet(v2_new[0], v2_new[1], v2_new[2], 0);
				//}
			}
		}

	}
	else if (link->PL_type == PL_CHAIN)
	{
		if (lenght >= link->PL_lenght)
		{
			DirectX::XMVECTOR pParallel;
			DirectX::XMVECTOR pPerpendicular1;
			DirectX::XMVECTOR pPerpendicular2;

			float v1_old[3];
			float v1_new[3];
			float m1 = link->PL_previous->PC_mass;
			DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular1, link->PL_previous->PC_velocity, diffVec);
			v1_old[0] = DirectX::XMVectorGetX(pParallel);
			v1_old[1] = DirectX::XMVectorGetY(pParallel);
			v1_old[2] = DirectX::XMVectorGetZ(pParallel);

			float v2_old[3];
			float v2_new[3];
			float m2 = link->PL_next->PC_mass;
			DirectX::XMVector3ComponentsFromNormal(&pParallel, &pPerpendicular2, link->PL_next->PC_velocity, diffVec);
			v2_old[0] = DirectX::XMVectorGetX(pParallel);
			v2_old[1] = DirectX::XMVectorGetY(pParallel);
			v2_old[2] = DirectX::XMVectorGetZ(pParallel);

			float e = 0.5;

			for (int i = 0; i < 3; i++)
			{

				v1_new[i] = (v1_old[i] * (m1 - m2*e) + m2*v2_old[i] * (1 + e)) / (m1 + m2);

				v2_new[i] = (v1_old[i] * m1*(1 + e) + (m2 - e*m1)*v2_old[i]) / (m1 + m2);

			}
			v1_old[0] += DirectX::XMVectorGetX(pPerpendicular1);
			v1_old[1] += DirectX::XMVectorGetY(pPerpendicular1);
			v1_old[2] += DirectX::XMVectorGetZ(pPerpendicular1);

			v2_old[0] += DirectX::XMVectorGetX(pPerpendicular2);
			v2_old[1] += DirectX::XMVectorGetY(pPerpendicular2);
			v2_old[2] += DirectX::XMVectorGetZ(pPerpendicular2);

			v1_new[0] += DirectX::XMVectorGetX(pPerpendicular1);
			v1_new[1] += DirectX::XMVectorGetY(pPerpendicular1);
			v1_new[2] += DirectX::XMVectorGetZ(pPerpendicular1);

			v2_new[0] += DirectX::XMVectorGetX(pPerpendicular2);
			v2_new[1] += DirectX::XMVectorGetY(pPerpendicular2);
			v2_new[2] += DirectX::XMVectorGetZ(pPerpendicular2);


			float forceVec1[3];
			float forceVec2[3];

			for (int i = 0; i < 3; i++)
			{
				forceVec1[i] = (m1 * v1_new[i] - m1 * v1_old[i]) / dt;
				forceVec2[i] = (m2 * v2_new[i] - m2 * v2_old[i]) / dt;
			}

			this->ApplyForceToComponent(link->PL_previous, DirectX::XMVectorSet(forceVec1[0], forceVec1[1], forceVec1[2], 0), dt);
			this->ApplyForceToComponent(link->PL_next, DirectX::XMVectorSet(forceVec2[0], forceVec2[1], forceVec2[2], 0), dt);

			//if (!link->CL_previous->PC_is_Static)
			//{
			//	link->CL_previous->PC_velocity = DirectX::XMVectorSet(v1_new[0], v1_new[1], v1_new[2], 0);
			//}
			//if (!link->CL_next->PC_is_Static)
			//{
			//	link->CL_next->PC_velocity = DirectX::XMVectorSet(v2_new[0], v2_new[1], v2_new[2], 0);
			//}
		}
	}



}

void PhysicsHandler::AdjustChainLinkPosition(PhysicsLink * link)
{
	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(link->PL_next->PC_pos, link->PL_previous->PC_pos);

	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));
	if (link->PL_type == PL_BODY)
	{

		DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(link->PL_next->PC_pos, link->PL_previous->PC_pos);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));
		diffVec = DirectX::XMVector3Normalize(diffVec);

		link->PL_next->PC_pos = DirectX::XMVectorAdd(link->PL_previous->PC_pos, DirectX::XMVectorScale(diffVec, link->PL_lenght));


	}
	else if (link->PL_type == PL_CHAIN)
	{
		if (distance > link->PL_lenght)
		{
			DirectX::XMVECTOR toMove = DirectX::XMVectorSubtract(diffVec, DirectX::XMVectorScale(DirectX::XMVector3Normalize(diffVec), link->PL_lenght));
			toMove = DirectX::XMVectorScale(toMove, 0.5f);

			link->PL_next->PC_pos = DirectX::XMVectorAdd(link->PL_previous->PC_pos, DirectX::XMVectorScale(DirectX::XMVector3Normalize(diffVec), link->PL_lenght));


		}
	}


}

void PhysicsHandler::ApplyForceToComponent(PhysicsComponent * componentPtr, DirectX::XMVECTOR force, float dt)
{
	if (!componentPtr->PC_is_Static && !componentPtr->PC_steadfast)
	{
		componentPtr->PC_velocity = DirectX::XMVectorAdd(componentPtr->PC_velocity, DirectX::XMVectorScale(DirectX::XMVectorScale(force, (1 / componentPtr->PC_mass)), dt));
	}
}

PhysicsComponent* PhysicsHandler::CreatePhysicsComponent(const DirectX::XMVECTOR &pos, const bool &isStatic)
{
	PhysicsComponent* newObject = nullptr;
	newObject = new PhysicsComponent;


	newObject->PC_pos = pos;
	newObject->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_rotationVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_normalForce = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_active = 1;
	newObject->PC_collides = true;
	newObject->PC_entityID = 0;
	newObject->PC_is_Static = isStatic;
	newObject->PC_steadfast = false;
	newObject->PC_mass = 1.0f;
	newObject->PC_gravityInfluence = 1.0f;
	newObject->PC_Sphere.radius = 1.0f;
	newObject->PC_friction = 0.9f;
	newObject->PC_elasticity = 0.5f;
	newObject->PC_BVtype = BV_AABB;
	//newObject->PC_OBB.quat = DirectX::XMVectorSet(0, 0, 0, 1);

	this->CreateDefaultBB(pos, newObject);
	this->m_physicsComponents.push_back(newObject);

	PhysicsComponent* toSwap;
	if (isStatic == false)
	{
		toSwap = this->m_physicsComponents.at((this->m_physicsComponents.size() - this->m_nrOfStaticObjects) - 1);
		this->m_physicsComponents.at((this->m_physicsComponents.size() - this->m_nrOfStaticObjects) - 1) = newObject;
		this->m_physicsComponents.at(this->m_physicsComponents.size() - 1) = toSwap;
	}
	else
	{
		this->m_nrOfStaticObjects++;
	}

	return newObject;
}

PhysicsComponent * PhysicsHandler::CreateBodyPartPhysicsComponent(const DirectX::XMVECTOR & pos, const bool & isStatic)
{

	PhysicsComponent* newObject = nullptr;
	newObject = new PhysicsComponent;


	newObject->PC_pos = pos;
	newObject->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_rotationVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_normalForce = DirectX::XMVectorSet(0, 0, 0, 0);
	newObject->PC_active = 1;
	newObject->PC_collides = true;
	newObject->PC_entityID = 0;
	newObject->PC_is_Static = isStatic;
	newObject->PC_steadfast = false;
	newObject->PC_mass = 1.0f;
	newObject->PC_gravityInfluence = 1.0f;
	newObject->PC_Sphere.radius = 0.125f;
	newObject->PC_friction = 0.5f;
	newObject->PC_elasticity = 0.5f;
	newObject->PC_BVtype = BV_Sphere;

	this->CreateDefaultBB(pos, newObject);
	newObject->PC_OBB.ext[0] = 0.25f;
	newObject->PC_OBB.ext[1] = 0.25f;
	newObject->PC_OBB.ext[2] = 0.25f;
	this->m_bodyPC.push_back(newObject);
	//this->m_physicsComponents.push_back(newObject);

	return newObject;
}

void PhysicsHandler::CreateChainLink(PhysicsComponent* playerComponent, PhysicsComponent* ballComponent, int nrOfLinks, float linkLenght)
{
	//important function will link from index1 to index2 and change the position of the PhysicsComponent at index2

	PhysicsComponent* ptr = playerComponent;
	PhysicsComponent* previous = playerComponent;
	PhysicsComponent* next = nullptr;

	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(ballComponent->PC_pos, playerComponent->PC_pos);
	
	if (!DirectX::XMVector3NotEqual(diffVec, DirectX::XMVectorSet(0, 0, 0, 0)))
	{
		diffVec = DirectX::XMVectorSet((float)nrOfLinks, 0, 0, 0);
	}
	diffVec = DirectX::XMVectorDivide(diffVec, DirectX::XMVectorSet((float)nrOfLinks, (float)nrOfLinks, (float)nrOfLinks, (float)nrOfLinks));

	DirectX::XMVECTOR nextPos = DirectX::XMVectorAdd(previous->PC_pos, diffVec);

	for (int i = 1; i <= nrOfLinks; i++)
	{
		PhysicsLink link;
		link.PL_lenght = linkLenght;

		//next = this->CreatePhysicsComponent(DirectX::XMVectorAdd(ptr->PC_pos, DirectX::XMVectorScale(diffVec, i)));
		next = this->CreatePhysicsComponent(nextPos, false);
		int indexBullet = this->m_physicsComponents.size() - 1;
		
		next->PC_BVtype = BV_OBB;
		next->PC_collides = false;
		next->PC_active = true;
		//next->PC_Sphere.radius = 0.35f;
		//next->PC_friction = 0;

		this->TransferBoxesToBullet(next, indexBullet);

		next->PC_OBB.ext[0] = 0.15f;
		next->PC_OBB.ext[1] = 0.15f;
		next->PC_OBB.ext[2] = 0.15f;
		next->PC_gravityInfluence = 1.0f;

		link.PL_previous = previous;
		link.PL_next = next;
		this->m_links.push_back(link);
		previous = next;
		nextPos = DirectX::XMVectorAdd(previous->PC_pos, diffVec);
	}
	PhysicsLink link;
	link.PL_lenght = linkLenght;

	next = ballComponent;
	next->PC_pos = nextPos;
	link.PL_previous = previous;
	link.PL_next = next;
	this->m_links.push_back(link);
}

void PhysicsHandler::ResetChainLink()
{
	// This resets the player, the chain link, and the ball
	int nrOfChainLinks = this->m_links.size();
	this->m_links[0].PL_previous->PC_velocity = { 0 };
	for (int i = 0; i < nrOfChainLinks; i++)
	{
		this->m_links[i].PL_next->PC_velocity = { 0 };
		this->m_links[i].PL_next->PC_pos =
			DirectX::XMVectorAdd(
				this->m_links[i].PL_previous->PC_pos, DirectX::XMVectorSet(0.33f, 0.11f, 0, 0));
	}
}

void PhysicsHandler::ResetRagdollToTPose(DirectX::XMVECTOR pos)
{
	float hitboxSize = 0.125f;
	float torsoWidth = 0.125;
	float torsoHeight = 0.5f;
	float armLenght = 0.5f;
	float legLenght = 0.5f;

	//hitboxSize = 0.125f;
	//torsoWidth = 0.5;
	//torsoHeight = 1.0f;
	//armLenght = 1.0f;
	//legLenght = 1.0f;

#pragma region
	PhysicsComponent* upperBody = this->m_bodyPC.at(0);
	upperBody->PC_pos = pos;
	upperBody->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightShoulder = this->m_bodyPC.at(1);
	rightShoulder->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, 0, 0, 0));
	rightShoulder->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* rightElbow = this->m_bodyPC.at(2);
	rightElbow->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth + armLenght, 0, 0, 0));
	rightElbow->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* rightHand = this->m_bodyPC.at(3);
	rightHand->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth + armLenght * 2, 0, 0, 0));
	rightHand->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* leftShoulder = this->m_bodyPC.at(4);
	leftShoulder->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, 0, 0, 0));
	leftShoulder->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* leftElbow = this->m_bodyPC.at(5);
	leftElbow->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth - armLenght, 0, 0, 0));
	leftElbow->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* leftHand = this->m_bodyPC.at(6);
	leftHand->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth - armLenght * 2, 0, 0, 0));
	leftHand->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* lowerBody = this->m_bodyPC.at(7);
	lowerBody->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0, -torsoHeight, 0, 0));
	lowerBody->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* rightLeg = this->m_bodyPC.at(8);
	rightLeg->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight, 0, 0));
	rightLeg->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* rightKnee = this->m_bodyPC.at(9);
	rightKnee->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight - legLenght, 0, 0));
	rightKnee->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* rightFoot = this->m_bodyPC.at(10);
	rightFoot->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight - legLenght * 2, 0, 0));
	rightFoot->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* leftLeg = this->m_bodyPC.at(11);
	leftLeg->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight, 0, 0));
	leftLeg->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* leftKnee = this->m_bodyPC.at(12);
	leftKnee->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight - legLenght, 0, 0));
	leftKnee->PC_OBB.ort = DirectX::XMMatrixIdentity();


	PhysicsComponent* leftFoot = this->m_bodyPC.at(13);
	leftFoot->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight - legLenght * 2, 0, 0));
	leftFoot->PC_OBB.ort = DirectX::XMMatrixIdentity();

#pragma endregion
}

void PhysicsHandler::CreateRagdollBody(DirectX::XMVECTOR pos, PhysicsComponent * playerPC)
{
	float hitboxSize = 0.25f;

	this->m_playerRagDoll.playerPC = playerPC;

#pragma region
	PhysicsComponent* upperBody = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	upperBody->PC_pos = pos;
	upperBody->PC_is_Static = false;
	upperBody->PC_BVtype = BV_OBB;
	upperBody->PC_OBB.ext[0] = hitboxSize;
	upperBody->PC_OBB.ext[1] = hitboxSize;
	upperBody->PC_OBB.ext[2] = hitboxSize;
	upperBody->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightShoulder = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightShoulder->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0.5, 0, 0, 0));
	rightShoulder->PC_is_Static = false;
	rightShoulder->PC_BVtype = BV_OBB;
	rightShoulder->PC_OBB.ext[0] = hitboxSize;
	rightShoulder->PC_OBB.ext[1] = hitboxSize;
	rightShoulder->PC_OBB.ext[2] = hitboxSize;
	rightShoulder->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightElbow = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightElbow->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(1.5, 0, 0, 0));
	rightElbow->PC_is_Static = false;
	rightElbow->PC_BVtype = BV_OBB;
	rightElbow->PC_OBB.ext[0] = hitboxSize;
	rightElbow->PC_OBB.ext[1] = hitboxSize;
	rightElbow->PC_OBB.ext[2] = hitboxSize;
	rightElbow->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightHand = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightHand->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(2.5, 0, 0, 0));
	rightHand->PC_is_Static = false;
	rightHand->PC_BVtype = BV_OBB;
	rightHand->PC_OBB.ext[0] = hitboxSize;
	rightHand->PC_OBB.ext[1] = hitboxSize;
	rightHand->PC_OBB.ext[2] = hitboxSize;
	rightHand->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftShoulder = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftShoulder->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-0.5, 0, 0, 0));
	leftShoulder->PC_is_Static = false;
	leftShoulder->PC_BVtype = BV_OBB;
	leftShoulder->PC_OBB.ext[0] = hitboxSize;
	leftShoulder->PC_OBB.ext[1] = hitboxSize;
	leftShoulder->PC_OBB.ext[2] = hitboxSize;
	leftShoulder->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftElbow = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftElbow->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-1.5, 0, 0, 0));
	leftElbow->PC_is_Static = false;
	leftElbow->PC_BVtype = BV_OBB;
	leftElbow->PC_OBB.ext[0] = hitboxSize;
	leftElbow->PC_OBB.ext[1] = hitboxSize;
	leftElbow->PC_OBB.ext[2] = hitboxSize;
	leftElbow->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftHand = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftHand->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-2.5, 0, 0, 0));
	leftHand->PC_is_Static = false;
	leftHand->PC_BVtype = BV_OBB;
	leftHand->PC_OBB.ext[0] = hitboxSize;
	leftHand->PC_OBB.ext[1] = hitboxSize;
	leftHand->PC_OBB.ext[2] = hitboxSize;
	leftHand->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* lowerBody = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	lowerBody->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0, -1, 0, 0));
	lowerBody->PC_is_Static = false;
	lowerBody->PC_BVtype = BV_OBB;
	lowerBody->PC_OBB.ext[0] = hitboxSize;
	lowerBody->PC_OBB.ext[1] = hitboxSize;
	lowerBody->PC_OBB.ext[2] = hitboxSize;
	lowerBody->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightLeg = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightLeg->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0.5, -1, 0, 0));
	rightLeg->PC_is_Static = false;
	rightLeg->PC_BVtype = BV_OBB;
	rightLeg->PC_OBB.ext[0] = hitboxSize;
	rightLeg->PC_OBB.ext[1] = hitboxSize;
	rightLeg->PC_OBB.ext[2] = hitboxSize;
	rightLeg->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightKnee = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightKnee->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0.5, -2, 0, 0));
	rightKnee->PC_is_Static = false;
	rightKnee->PC_BVtype = BV_OBB;
	rightKnee->PC_OBB.ext[0] = hitboxSize;
	rightKnee->PC_OBB.ext[1] = hitboxSize;
	rightKnee->PC_OBB.ext[2] = hitboxSize;
	rightKnee->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightFoot = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightFoot->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0.5, -3, 0, 0));
	rightFoot->PC_is_Static = false;
	rightFoot->PC_BVtype = BV_OBB;
	rightFoot->PC_OBB.ext[0] = hitboxSize;
	rightFoot->PC_OBB.ext[1] = hitboxSize;
	rightFoot->PC_OBB.ext[2] = hitboxSize;
	rightFoot->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftLeg = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftLeg->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-0.5, -1, 0, 0));
	leftLeg->PC_is_Static = false;
	leftLeg->PC_BVtype = BV_OBB;
	leftLeg->PC_OBB.ext[0] = hitboxSize;
	leftLeg->PC_OBB.ext[1] = hitboxSize;
	leftLeg->PC_OBB.ext[2] = hitboxSize;
	leftLeg->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftKnee = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftKnee->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-0.5, -2, 0, 0));
	leftKnee->PC_is_Static = false;
	leftKnee->PC_BVtype = BV_OBB;
	leftKnee->PC_OBB.ext[0] = hitboxSize;
	leftKnee->PC_OBB.ext[1] = hitboxSize;
	leftKnee->PC_OBB.ext[2] = hitboxSize;
	leftKnee->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftFoot = this->CreatePhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftFoot->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-0.5, -3, 0, 0));
	leftFoot->PC_is_Static = false;
	leftFoot->PC_BVtype = BV_OBB;
	leftFoot->PC_OBB.ext[0] = hitboxSize;
	leftFoot->PC_OBB.ext[1] = hitboxSize;
	leftFoot->PC_OBB.ext[2] = hitboxSize;
	leftFoot->PC_OBB.ort = DirectX::XMMatrixIdentity();
#pragma endregion


#pragma region

	this->m_playerRagDoll.upperBody.BP_type = BP_UPPERBODY;
	this->m_playerRagDoll.upperBody.center = upperBody;
	this->m_playerRagDoll.upperBody.next = rightShoulder;
	this->m_playerRagDoll.upperBody.next2 = leftShoulder;
	this->m_playerRagDoll.upperBody.previous = lowerBody;

	this->m_playerRagDoll.lowerBody.BP_type = BP_LOWERBODY;
	this->m_playerRagDoll.lowerBody.center = lowerBody;
	this->m_playerRagDoll.lowerBody.next = rightLeg;
	this->m_playerRagDoll.lowerBody.next2 = leftLeg;
	this->m_playerRagDoll.lowerBody.previous = upperBody;

	this->m_playerRagDoll.rightArm.BP_type = BP_RIGHT_ARM;
	this->m_playerRagDoll.rightArm.center = rightShoulder;
	this->m_playerRagDoll.rightArm.next = rightElbow;
	this->m_playerRagDoll.rightArm.next2 = rightHand;

	this->m_playerRagDoll.leftArm.BP_type = BP_LEFT_ARM;
	this->m_playerRagDoll.leftArm.center = leftShoulder;
	this->m_playerRagDoll.leftArm.next = leftElbow;
	this->m_playerRagDoll.leftArm.next2 = leftHand;

	this->m_playerRagDoll.rightLeg.BP_type = BP_RIGHT_LEG;
	this->m_playerRagDoll.rightLeg.center = rightLeg;
	this->m_playerRagDoll.rightLeg.next = rightKnee;
	this->m_playerRagDoll.rightLeg.next2 = rightFoot;

	this->m_playerRagDoll.leftLeg.BP_type = BP_LEFT_LEG;
	this->m_playerRagDoll.leftLeg.center = leftLeg;
	this->m_playerRagDoll.leftLeg.next = leftKnee;
	this->m_playerRagDoll.leftLeg.next2 = leftFoot;

	this->m_links.push_back(PhysicsLink());
	PhysicsLink* link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 0.5f;
	link->PL_next = rightShoulder;
	link->PL_previous = upperBody;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 0.5f;
	link->PL_next = leftShoulder;
	link->PL_previous = upperBody;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = lowerBody;
	link->PL_previous = upperBody;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 0.5f;
	link->PL_next = rightLeg;
	link->PL_previous = lowerBody;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 0.5f;
	link->PL_next = leftLeg;
	link->PL_previous = lowerBody;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = rightElbow;
	link->PL_previous = rightShoulder;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = rightHand;
	link->PL_previous = rightElbow;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = leftElbow;
	link->PL_previous = leftShoulder;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = leftHand;
	link->PL_previous = leftElbow;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = rightKnee;
	link->PL_previous = rightLeg;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = rightFoot;
	link->PL_previous = rightKnee;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = leftKnee;
	link->PL_previous = leftLeg;

	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = 1.0f;
	link->PL_next = leftFoot;
	link->PL_previous = leftKnee;

#pragma endregion

}

void PhysicsHandler::CreateRagdollBodyWithChainAndBall(Resources::Skeleton::Joint *Skeleton, DirectX::XMVECTOR pos, PhysicsComponent * playerPC, PhysicsComponent* ball)
{
	float hitboxSize = 0.125f;
	float torsoWidth = 0.125;
	float torsoHeight = 0.5f;
	float armLenght = 0.5f;
	float legLenght = 0.5f;

	//hitboxSize = 0.125f;
	//torsoWidth = 0.5;
	//torsoHeight = 1.0f;
	//armLenght = 1.0f;
	//legLenght = 1.0f;

#pragma region

	PhysicsComponent* lowerBody = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	lowerBody->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0, -torsoHeight, 0, 0));
	lowerBody->PC_is_Static = false;
	lowerBody->PC_BVtype = BV_Sphere;
	lowerBody->PC_OBB.ext[0] = hitboxSize;
	lowerBody->PC_OBB.ext[1] = hitboxSize;
	lowerBody->PC_OBB.ext[2] = hitboxSize;
	lowerBody->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* MiddleBody = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	MiddleBody->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(0, -torsoHeight / 2, 0, 0));
	MiddleBody->PC_is_Static = false;
	MiddleBody->PC_BVtype = BV_Sphere;
	MiddleBody->PC_OBB.ext[0] = hitboxSize;
	MiddleBody->PC_OBB.ext[1] = hitboxSize;
	MiddleBody->PC_OBB.ext[2] = hitboxSize;
	MiddleBody->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* upperBody = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	upperBody->PC_pos = pos;
	upperBody->PC_is_Static = false;
	upperBody->PC_BVtype = BV_Sphere;
	upperBody->PC_OBB.ext[0] = hitboxSize;
	upperBody->PC_OBB.ext[1] = hitboxSize;
	upperBody->PC_OBB.ext[2] = hitboxSize;
	upperBody->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* neck = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0.2, 0, 0), false);
	neck->PC_pos = pos;
	neck->PC_is_Static = false;
	neck->PC_BVtype = BV_Sphere;
	neck->PC_OBB.ext[0] = hitboxSize;
	neck->PC_OBB.ext[1] = hitboxSize;
	neck->PC_OBB.ext[2] = hitboxSize;
	neck->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* head = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0.4, 0, 0), false);
	head->PC_pos = pos;
	head->PC_is_Static = false;
	head->PC_BVtype = BV_Sphere;
	head->PC_OBB.ext[0] = hitboxSize;
	head->PC_OBB.ext[1] = hitboxSize;
	head->PC_OBB.ext[2] = hitboxSize;
	head->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightShoulder = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightShoulder->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, 0, 0, 0));
	rightShoulder->PC_is_Static = false;
	rightShoulder->PC_BVtype = BV_Sphere;
	rightShoulder->PC_OBB.ext[0] = hitboxSize;
	rightShoulder->PC_OBB.ext[1] = hitboxSize;
	rightShoulder->PC_OBB.ext[2] = hitboxSize;
	rightShoulder->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightElbow = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightElbow->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth + armLenght, 0, 0, 0));
	rightElbow->PC_is_Static = false;
	rightElbow->PC_BVtype = BV_Sphere;
	rightElbow->PC_OBB.ext[0] = hitboxSize;
	rightElbow->PC_OBB.ext[1] = hitboxSize;
	rightElbow->PC_OBB.ext[2] = hitboxSize;
	rightElbow->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightHand = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightHand->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth + armLenght * 2, 0, 0, 0));
	rightHand->PC_is_Static = false;
	rightHand->PC_BVtype = BV_Sphere;
	rightHand->PC_OBB.ext[0] = hitboxSize;
	rightHand->PC_OBB.ext[1] = hitboxSize;
	rightHand->PC_OBB.ext[2] = hitboxSize;
	rightHand->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightHandEnd = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightHandEnd->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth + armLenght * 2.5, 0, 0, 0));
	rightHandEnd->PC_is_Static = false;
	rightHandEnd->PC_BVtype = BV_Sphere;
	rightHandEnd->PC_OBB.ext[0] = hitboxSize;
	rightHandEnd->PC_OBB.ext[1] = hitboxSize;
	rightHandEnd->PC_OBB.ext[2] = hitboxSize;
	rightHandEnd->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftShoulder = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftShoulder->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, 0, 0, 0));
	leftShoulder->PC_is_Static = false;
	leftShoulder->PC_BVtype = BV_Sphere;
	leftShoulder->PC_OBB.ext[0] = hitboxSize;
	leftShoulder->PC_OBB.ext[1] = hitboxSize;
	leftShoulder->PC_OBB.ext[2] = hitboxSize;
	leftShoulder->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftElbow = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftElbow->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth - armLenght, 0, 0, 0));
	leftElbow->PC_is_Static = false;
	leftElbow->PC_BVtype = BV_Sphere;
	leftElbow->PC_OBB.ext[0] = hitboxSize;
	leftElbow->PC_OBB.ext[1] = hitboxSize;
	leftElbow->PC_OBB.ext[2] = hitboxSize;
	leftElbow->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftHand = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftHand->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth - armLenght * 2, 0, 0, 0));
	leftHand->PC_is_Static = false;
	leftHand->PC_BVtype = BV_Sphere;
	leftHand->PC_OBB.ext[0] = hitboxSize;
	leftHand->PC_OBB.ext[1] = hitboxSize;
	leftHand->PC_OBB.ext[2] = hitboxSize;
	leftHand->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftHandEnd = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftHandEnd->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth - armLenght * 2.5, 0, 0, 0));
	leftHandEnd->PC_is_Static = false;
	leftHandEnd->PC_BVtype = BV_Sphere;
	leftHandEnd->PC_OBB.ext[0] = hitboxSize;
	leftHandEnd->PC_OBB.ext[1] = hitboxSize;
	leftHandEnd->PC_OBB.ext[2] = hitboxSize;
	leftHandEnd->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightLeg = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightLeg->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight, 0, 0));
	rightLeg->PC_is_Static = false;
	rightLeg->PC_BVtype = BV_Sphere;
	rightLeg->PC_OBB.ext[0] = hitboxSize;
	rightLeg->PC_OBB.ext[1] = hitboxSize;
	rightLeg->PC_OBB.ext[2] = hitboxSize;
	rightLeg->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightKnee = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightKnee->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight - legLenght, 0, 0));
	rightKnee->PC_is_Static = false;
	rightKnee->PC_BVtype = BV_Sphere;
	rightKnee->PC_OBB.ext[0] = hitboxSize;
	rightKnee->PC_OBB.ext[1] = hitboxSize;
	rightKnee->PC_OBB.ext[2] = hitboxSize;
	rightKnee->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightFoot = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightFoot->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight - legLenght * 2, 0, 0));
	rightFoot->PC_is_Static = false;
	rightFoot->PC_BVtype = BV_Sphere;
	rightFoot->PC_OBB.ext[0] = hitboxSize;
	rightFoot->PC_OBB.ext[1] = hitboxSize;
	rightFoot->PC_OBB.ext[2] = hitboxSize;
	rightFoot->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* rightFootEnd = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	rightFootEnd->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(-torsoWidth, -torsoHeight - legLenght * 2, 0, 0));
	rightFootEnd->PC_is_Static = false;
	rightFootEnd->PC_BVtype = BV_Sphere;
	rightFootEnd->PC_OBB.ext[0] = hitboxSize;
	rightFootEnd->PC_OBB.ext[1] = hitboxSize;
	rightFootEnd->PC_OBB.ext[2] = hitboxSize;
	rightFootEnd->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftLeg = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftLeg->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight, 0, 0));
	leftLeg->PC_is_Static = false;
	leftLeg->PC_BVtype = BV_Sphere;
	leftLeg->PC_OBB.ext[0] = hitboxSize;
	leftLeg->PC_OBB.ext[1] = hitboxSize;
	leftLeg->PC_OBB.ext[2] = hitboxSize;
	leftLeg->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftKnee = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftKnee->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight - legLenght, 0, 0));
	leftKnee->PC_is_Static = false;
	leftKnee->PC_BVtype = BV_Sphere;
	leftKnee->PC_OBB.ext[0] = hitboxSize;
	leftKnee->PC_OBB.ext[1] = hitboxSize;
	leftKnee->PC_OBB.ext[2] = hitboxSize;
	leftKnee->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftFoot = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftFoot->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight - legLenght * 2, 0, 0));
	leftFoot->PC_is_Static = false;
	leftFoot->PC_BVtype = BV_Sphere;
	leftFoot->PC_OBB.ext[0] = hitboxSize;
	leftFoot->PC_OBB.ext[1] = hitboxSize;
	leftFoot->PC_OBB.ext[2] = hitboxSize;
	leftFoot->PC_OBB.ort = DirectX::XMMatrixIdentity();

	PhysicsComponent* leftFootEnd = this->CreateBodyPartPhysicsComponent(DirectX::XMVectorSet(0, 0, 0, 0), false);
	leftFootEnd->PC_pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorSet(torsoWidth, -torsoHeight - legLenght * 2, 0, 0));
	leftFootEnd->PC_is_Static = false;
	leftFootEnd->PC_BVtype = BV_Sphere;
	leftFootEnd->PC_OBB.ext[0] = hitboxSize;
	leftFootEnd->PC_OBB.ext[1] = hitboxSize;
	leftFootEnd->PC_OBB.ext[2] = hitboxSize;
	leftFootEnd->PC_OBB.ort = DirectX::XMMatrixIdentity();
#pragma endregion create all joints




#pragma region
	this->m_playerRagDoll.playerPC = playerPC;
	this->m_playerRagDoll.ballPC = ball;
	this->m_playerRagDoll.upperBody.BP_type = BP_UPPERBODY;
	this->m_playerRagDoll.upperBody.center = upperBody;
	this->m_playerRagDoll.upperBody.next = rightShoulder;
	this->m_playerRagDoll.upperBody.next2 = leftShoulder;
	this->m_playerRagDoll.upperBody.previous = lowerBody;
	this->m_playerRagDoll.upperBody.next3 = MiddleBody;
	this->m_playerRagDoll.upperBody.next4 = neck;
	this->m_playerRagDoll.upperBody.next5 = head;

	this->m_playerRagDoll.lowerBody.BP_type = BP_LOWERBODY;
	this->m_playerRagDoll.lowerBody.center = lowerBody;
	this->m_playerRagDoll.lowerBody.next = rightLeg;
	this->m_playerRagDoll.lowerBody.next2 = leftLeg;
	this->m_playerRagDoll.lowerBody.previous = upperBody;
	this->m_playerRagDoll.lowerBody.next3 = MiddleBody;

	this->m_playerRagDoll.rightArm.BP_type = BP_RIGHT_ARM;
	this->m_playerRagDoll.rightArm.center = rightShoulder;
	this->m_playerRagDoll.rightArm.next = rightElbow;
	this->m_playerRagDoll.rightArm.next2 = rightHand;
	this->m_playerRagDoll.rightArm.next3 = rightHandEnd;

	this->m_playerRagDoll.leftArm.BP_type = BP_LEFT_ARM;
	this->m_playerRagDoll.leftArm.center = leftShoulder;
	this->m_playerRagDoll.leftArm.next = leftElbow;
	this->m_playerRagDoll.leftArm.next2 = leftHand;
	this->m_playerRagDoll.leftArm.next3 = leftHandEnd;

	this->m_playerRagDoll.rightLeg.BP_type = BP_RIGHT_LEG;
	this->m_playerRagDoll.rightLeg.center = rightLeg;
	this->m_playerRagDoll.rightLeg.next = rightKnee;
	this->m_playerRagDoll.rightLeg.next2 = rightFoot;
	this->m_playerRagDoll.rightLeg.next3 = rightFootEnd;

	this->m_playerRagDoll.leftLeg.BP_type = BP_LEFT_LEG;
	this->m_playerRagDoll.leftLeg.center = leftLeg;
	this->m_playerRagDoll.leftLeg.next = leftKnee;
	this->m_playerRagDoll.leftLeg.next2 = leftFoot;
	this->m_playerRagDoll.leftLeg.next3 = leftFootEnd;


	for (int i = 0; i < 21; i++)
	{
		this->m_playerRagDoll.jointMatrixes[i] = DirectX::XMMatrixIdentity();
	}

	this->m_playerRagDoll.Skeleton = Skeleton;
	this->SetRagdollToBindPose(&this->m_playerRagDoll, DirectX::XMVectorAdd(DirectX::XMVectorAdd(playerPC->PC_pos, pos), DirectX::XMVectorSet(0, 4, 0, 0)));

	//UPPERBODY - NECK
	this->m_links.push_back(PhysicsLink());
	PhysicsLink* link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(upperBody->PC_pos, neck->PC_pos)));
	link->PL_next = neck;
	link->PL_previous = upperBody;

	//NECK - HEAD
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(neck->PC_pos, head->PC_pos)));
	link->PL_next = head;
	link->PL_previous = neck;

	//UPPERBODY - RIGHTSHOULDER
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(upperBody->PC_pos, rightShoulder->PC_pos)));
	link->PL_next = rightShoulder;
	link->PL_previous = upperBody;

	//UPPERBODY - LEFTSHOULDER
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(upperBody->PC_pos, leftShoulder->PC_pos)));;
	link->PL_next = leftShoulder;
	link->PL_previous = upperBody;

	//UPPERBODY - MIDDLEBODY
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(upperBody->PC_pos, MiddleBody->PC_pos)));;
	link->PL_next = MiddleBody;
	link->PL_previous = upperBody;

	//MIDDLEBODY - LOWERBODY
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(lowerBody->PC_pos, MiddleBody->PC_pos)));;
	link->PL_next = lowerBody;
	link->PL_previous = MiddleBody;

	//UPPERBODY - LOWERBODY
	//this->m_links.push_back(PhysicsLink());
	//link = &this->m_links.at(this->m_links.size() - 1);
	//link->PL_type = PL_BODY;
	//link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(upperBody->PC_pos, lowerBody->PC_pos)));;
	//link->PL_next = lowerBody;
	//link->PL_previous = upperBody;

	//LOWERBODY - RIGHTLEG
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(lowerBody->PC_pos, rightLeg->PC_pos)));;
	link->PL_next = rightLeg;
	link->PL_previous = lowerBody;

	//LOWERBODY - LEFTLEG
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(lowerBody->PC_pos, leftLeg->PC_pos)));;
	link->PL_next = leftLeg;
	link->PL_previous = lowerBody;

	//RIGHTSHOULDER - RIGHTELBOW
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(rightShoulder->PC_pos, rightElbow->PC_pos)));;
	link->PL_next = rightElbow;
	link->PL_previous = rightShoulder;

	//RIGHTELBOW - RIGHTHAND
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(rightElbow->PC_pos, rightHand->PC_pos)));;
	link->PL_next = rightHand;
	link->PL_previous = rightElbow;

	//RIGHTHAND - RIGHTHANDEND
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(rightHandEnd->PC_pos, rightHand->PC_pos)));;
	link->PL_next = rightHandEnd;
	link->PL_previous = rightHand;

	//LEFTSHOULDER - LEFTELBOW
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(leftShoulder->PC_pos, leftElbow->PC_pos)));;
	link->PL_next = leftElbow;
	link->PL_previous = leftShoulder;

	//LEFTELBOW - LEFTHAND
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(leftElbow->PC_pos, leftHand->PC_pos)));;
	link->PL_next = leftHand;
	link->PL_previous = leftElbow;

	//LEFTHAND - LEFTHANDEND
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(leftHandEnd->PC_pos, leftHand->PC_pos)));;
	link->PL_next = leftHandEnd;
	link->PL_previous = leftHand;

	//RIGHTLEG - RIGHTKNEE
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(rightLeg->PC_pos, rightKnee->PC_pos)));;
	link->PL_next = rightKnee;
	link->PL_previous = rightLeg;

	//RIGHTKNEE - RIGHTFOOT
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(rightKnee->PC_pos, rightFoot->PC_pos)));;
	link->PL_next = rightFoot;
	link->PL_previous = rightKnee;

	//RIGHTFOOT - RIGHTFOOTEND
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(rightFootEnd->PC_pos, rightFoot->PC_pos)));;
	link->PL_next = rightFootEnd;
	link->PL_previous = rightFoot;

	//LEFTLEG - LEFTKNEE
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(leftLeg->PC_pos, leftKnee->PC_pos)));;
	link->PL_next = leftKnee;
	link->PL_previous = leftLeg;

	//LEFTKNEE - LEFTFOTT
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(leftKnee->PC_pos, leftFoot->PC_pos)));;
	link->PL_next = leftFoot;
	link->PL_previous = leftKnee;

	//LEFTFOTT - LEFTFOTTEnd
	this->m_links.push_back(PhysicsLink());
	link = &this->m_links.at(this->m_links.size() - 1);
	link->PL_type = PL_BODY;
	link->PL_lenght = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(leftFootEnd->PC_pos, leftFoot->PC_pos)));;
	link->PL_next = leftFootEnd;
	link->PL_previous = leftFoot;

#pragma endregion link all joints
	

	this->m_playerRagDoll.state = ANIMATED;
	rightFoot->PC_entityID = -1;

	//upperBody->PC_mass = 5;
	//rightShoulder->PC_mass = 2.5f;
	//rightElbow->PC_mass = 1.25f;
	//rightHand->PC_mass = 0.625f;

	//leftShoulder->PC_mass = 2.5f;
	//leftElbow->PC_mass = 1.25f;
	//leftHand->PC_mass = 0.625f;

	//lowerBody->PC_mass = 5;
	//rightLeg->PC_mass = 2.5f;
	//rightKnee->PC_mass = 1.25;
	//rightFoot->PC_mass = 0.625;

	//leftLeg->PC_mass = 2.5f;
	//leftKnee->PC_mass = 1.25;
	//leftFoot->PC_mass = 0.625;

	//rightShoulder->PC_is_Static = true;
	////rightElbow->PC_is_Static = true;
	//leftShoulder->PC_is_Static = true;
	////leftElbow->PC_is_Static = true;
	//leftLeg->PC_is_Static = true;
	////leftKnee->PC_is_Static = true;
	//rightLeg->PC_is_Static = true;
	////rightKnee->PC_is_Static = true;

	//rightFoot->PC_velocity = DirectX::XMVectorSet(0, 0, 1, 0);
	//rightHand->PC_velocity = DirectX::XMVectorSet(0, 0, 1, 0);

	//this->CreateChainLink(rightFoot, ball, 4, 2);
	//this->CreateChainLink(leftFoot, ball, 4, 2);
}

void PhysicsHandler::AdjustRagdoll(Ragdoll * ragdoll, float dt)
{
	this->AdjustBodyParts(&ragdoll->upperBody, dt);
	this->AdjustBodyParts(&ragdoll->lowerBody, dt);
	this->AdjustBodyParts(&ragdoll->rightArm, dt);
	this->AdjustBodyParts(&ragdoll->leftArm, dt);
	this->AdjustBodyParts(&ragdoll->rightLeg, dt);
	this->AdjustBodyParts(&ragdoll->leftLeg, dt);


	ragdoll->upperBody.next4->PC_pos = DirectX::XMVectorAdd(ragdoll->upperBody.center->PC_pos, ragdoll->upperBody.center->PC_OBB.ort.r[1]);
	ragdoll->upperBody.next4->PC_OBB.ort = ragdoll->upperBody.center->PC_OBB.ort;

	ragdoll->upperBody.next5->PC_pos = DirectX::XMVectorAdd(ragdoll->upperBody.next4->PC_pos, ragdoll->upperBody.next4->PC_OBB.ort.r[1]);
	ragdoll->upperBody.next5->PC_pos = DirectX::XMVectorAdd(ragdoll->upperBody.next5->PC_pos, DirectX::XMVectorScale(ragdoll->upperBody.next4->PC_OBB.ort.r[2], -1));
	ragdoll->upperBody.next5->PC_OBB.ort = ragdoll->upperBody.next4->PC_OBB.ort;

	ragdoll->rightArm.next3->PC_pos = DirectX::XMVectorAdd(ragdoll->rightArm.next2->PC_pos, ragdoll->rightArm.next2->PC_OBB.ort.r[0]);
	ragdoll->rightArm.next3->PC_OBB.ort = ragdoll->rightArm.next2->PC_OBB.ort;

	ragdoll->leftArm.next3->PC_pos = DirectX::XMVectorAdd(ragdoll->leftArm.next2->PC_pos, DirectX::XMVectorScale(ragdoll->leftArm.next2->PC_OBB.ort.r[0], -1));
	ragdoll->leftArm.next3->PC_OBB.ort = ragdoll->leftArm.next2->PC_OBB.ort;

	ragdoll->rightLeg.next3->PC_pos = DirectX::XMVectorAdd(ragdoll->rightLeg.next2->PC_pos, DirectX::XMVectorScale(ragdoll->rightLeg.next2->PC_OBB.ort.r[2], -1));
	ragdoll->rightLeg.next3->PC_OBB.ort = ragdoll->rightLeg.next2->PC_OBB.ort;

	ragdoll->leftLeg.next3->PC_pos = DirectX::XMVectorAdd(ragdoll->leftLeg.next2->PC_pos, DirectX::XMVectorScale(ragdoll->leftLeg.next2->PC_OBB.ort.r[2], -1));
	ragdoll->leftLeg.next3->PC_OBB.ort = ragdoll->leftLeg.next2->PC_OBB.ort;
}

DirectX::XMVECTOR PhysicsHandler::AdjustBodyPartDistance(PhysicsComponent * previous, PhysicsComponent * next, float lenght)
{



	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(next->PC_pos, previous->PC_pos);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(diffVec));
	diffVec = DirectX::XMVector3Normalize(diffVec);


	if (!DirectX::XMVector3NotEqual(next->PC_normalForce, DirectX::XMVectorSet(0, 0, 0, 0)))
	{
		next->PC_pos = DirectX::XMVectorAdd(previous->PC_pos, DirectX::XMVectorScale(diffVec, lenght));
		return DirectX::XMVectorSet(0, 0, 0, 0);
	}
	else
	{
		DirectX::XMVECTOR toMove = DirectX::XMVectorScale(diffVec, lenght - distance);
		DirectX::XMVECTOR para;
		DirectX::XMVECTOR perp;

		DirectX::XMVector3ComponentsFromNormal(&para, &perp, toMove, next->PC_normalForce);
		next->PC_pos = DirectX::XMVectorAdd(next->PC_pos, perp);
		previous->PC_pos = DirectX::XMVectorAdd(previous->PC_pos, DirectX::XMVectorScale(para, -1));
		previous->PC_normalForce = DirectX::XMVector3Normalize(DirectX::XMVectorAdd(previous->PC_normalForce, DirectX::XMVectorScale(diffVec, -1)));
		return para;
	}
}

void PhysicsHandler::AdjustBodyParts(BodyPart * bodypart, float dt)
{

#pragma region
	if (bodypart->BP_type == BP_UPPERBODY)
	{
		DirectX::XMVECTOR uppVec = DirectX::XMVectorSubtract(bodypart->center->PC_pos, bodypart->next3->PC_pos);
		uppVec = DirectX::XMVector3Normalize(uppVec);

		DirectX::XMVECTOR rightVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(bodypart->next->PC_pos, bodypart->center->PC_pos));
		DirectX::XMVECTOR perp;
		DirectX::XMVECTOR para;

		DirectX::XMVector3ComponentsFromNormal(&para, &perp, rightVec, uppVec);
		rightVec = DirectX::XMVector3Normalize(perp);

		DirectX::XMVECTOR forwardVec = DirectX::XMVector3Cross(rightVec, uppVec);

		bodypart->center->PC_OBB.ort.r[0] = rightVec;
		bodypart->center->PC_OBB.ort.r[1] = uppVec;
		bodypart->center->PC_OBB.ort.r[2] = forwardVec;

		//----Update bodypart physicsComponent

		bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->center->PC_pos, DirectX::XMVectorScale(rightVec, 0.125f));
		bodypart->next->PC_OBB.ort = bodypart->center->PC_OBB.ort;


		bodypart->next2->PC_pos = DirectX::XMVectorAdd(bodypart->center->PC_pos, DirectX::XMVectorScale(DirectX::XMVectorScale(rightVec, -1), 0.125));
		bodypart->next2->PC_OBB.ort = bodypart->center->PC_OBB.ort;

		bodypart->next3->PC_OBB.ort = bodypart->center->PC_OBB.ort;
		bodypart->previous->PC_OBB.ort = bodypart->center->PC_OBB.ort;

		//----


	}
#pragma endregion Adjust upperbody

#pragma region
	if (bodypart->BP_type == BP_LOWERBODY)
	{
		DirectX::XMFLOAT3 offSetScale;
		DirectX::XMStoreFloat3(&offSetScale, DirectX::XMVectorSubtract(this->TESTjointMatrixes[0].r[3], this->TESTjointMatrixes[1].r[3]));
		DirectX::XMVECTOR toMove = DirectX::XMVectorAdd(
			DirectX::XMVectorScale(bodypart->next3->PC_OBB.ort.r[1], offSetScale.y), 
			DirectX::XMVectorScale(bodypart->next3->PC_OBB.ort.r[2], offSetScale.z));

		bodypart->center->PC_pos = DirectX::XMVectorAdd(bodypart->next3->PC_pos, toMove);

		DirectX::XMStoreFloat3(&offSetScale, DirectX::XMVectorSubtract(this->TESTjointMatrixes[13].r[3], this->TESTjointMatrixes[0].r[3]));
		toMove = DirectX::XMVectorAdd(
			DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[1], offSetScale.y),
			DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[0], offSetScale.x));

		bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->center->PC_pos, toMove);

		DirectX::XMStoreFloat3(&offSetScale, DirectX::XMVectorSubtract(this->TESTjointMatrixes[17].r[3], this->TESTjointMatrixes[0].r[3]));
		toMove = DirectX::XMVectorAdd(
			DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[1], offSetScale.y),
			DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[0], offSetScale.x));

		bodypart->next2->PC_pos = DirectX::XMVectorAdd(bodypart->center->PC_pos, toMove);

		bodypart->next2->PC_OBB.ort = bodypart->center->PC_OBB.ort;
		bodypart->next->PC_OBB.ort = bodypart->center->PC_OBB.ort;



	}
#pragma endregion Adjust lowerbody

#pragma region
	if (bodypart->BP_type == BP_RIGHT_ARM || bodypart->BP_type == BP_LEFT_ARM)
	{
		//bodypart->next->PC_OBB.ort = bodypart->center->PC_OBB.ort;
		//bodypart->next2->PC_OBB.ort = bodypart->center->PC_OBB.ort;

		////updates the ortoMatrix of the elbow
		////bodypart->center == shoulder
		////bodypart->next == elbow
		DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(bodypart->next->PC_pos, bodypart->center->PC_pos);
		diffVec = DirectX::XMVector3Normalize(diffVec);

		DirectX::XMVECTOR para;
		DirectX::XMVECTOR perp;

		////use the vector from the shoulder to the elbow as the rightVec and takes the perpendicular parts of the 
		////uppVector from the shoulder and sets that as the elbows uppVec
		DirectX::XMVector3ComponentsFromNormal(&para, &perp, bodypart->center->PC_OBB.ort.r[1], diffVec);
		DirectX::XMVECTOR uppVec = DirectX::XMVector3Normalize(perp);

		////forward vector from crossproduct between rightVec(diffVec) and uppvec
		DirectX::XMVECTOR forwardVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(diffVec, uppVec));
		uppVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(forwardVec, diffVec));

		//set the ortoMatrix (stored in the OBB of the PhysicsComponent)
		bodypart->next->PC_OBB.ort.r[0] = diffVec;
		bodypart->next->PC_OBB.ort.r[1] = uppVec;
		bodypart->next->PC_OBB.ort.r[2] = forwardVec;

		//bodypart->next->PC_OBB.ort = DirectX::XMMatrixTranspose(bodypart->next->PC_OBB.ort);

		//----
		if (bodypart->BP_type == BP_LEFT_ARM)
		{
			//revers the rightVec and the forwardVec of the ortoMatrix if it belongs to the left arm
			bodypart->next->PC_OBB.ort.r[0] = DirectX::XMVectorScale(bodypart->next->PC_OBB.ort.r[0], -1);
			bodypart->next->PC_OBB.ort.r[2] = DirectX::XMVectorScale(bodypart->next->PC_OBB.ort.r[2], -1);

			float axis1dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[0]));
			float axis2dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[1]));
			float axis3dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[2]));

			//the constraints that keeps the arm from bending in weird ways
			if (axis1dot > -0.1)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[0]);
				para = DirectX::XMVectorScale(DirectX::XMVectorSubtract(DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[0], -0.1), para),dt);
				bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->next->PC_pos, para);
				bodypart->next2->PC_pos = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, para);
			}
			if (axis3dot > 0)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[2]);
				//para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[2], 0.2));
				bodypart->next->PC_pos = DirectX::XMVectorSubtract(bodypart->next->PC_pos, para);
			}
		}
		//constraint that keep the arm from bending weird ways if it belongs to the right arm
		else if (bodypart->BP_type == BP_RIGHT_ARM)
		{
			float axis1dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[0]));
			float axis2dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[1]));
			float axis3dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[2]));

			if (axis1dot < 0.1)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[0]);
				para = DirectX::XMVectorScale(DirectX::XMVectorSubtract(DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[0], -0.1), para),dt);
				bodypart->next->PC_pos = DirectX::XMVectorSubtract(bodypart->next->PC_pos, para);
				bodypart->next2->PC_pos = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, para);
			}
			if (axis3dot > 0)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[2]);
				//para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[2], -0.2));
				bodypart->next->PC_pos = DirectX::XMVectorSubtract(bodypart->next->PC_pos, para);
			}
		}
		

		//updates the right hand dependent on the right elbow
		//bodypart->next == elbow
		//bodypart->next2 == hand 

		diffVec = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, bodypart->next->PC_pos);
		diffVec = DirectX::XMVector3Normalize(diffVec);

		//use the vector from the elbow to the hand as the rightVec and gets the uppVec by taking the perpendicular
		//part of the uppVec from the elbow on the rightVec(diffVec) 
		DirectX::XMVector3ComponentsFromNormal(&para, &perp, bodypart->next->PC_OBB.ort.r[1], diffVec);
		uppVec = DirectX::XMVector3Normalize(perp);

		//gets the forwardVec with CrossProduct
		forwardVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(diffVec, uppVec));
		uppVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(forwardVec, diffVec));


		//set the ortoMatrix (stored in the OBB of the PhysicsComponent)
		bodypart->next2->PC_OBB.ort.r[0] = diffVec;
		bodypart->next2->PC_OBB.ort.r[1] = uppVec;
		bodypart->next2->PC_OBB.ort.r[2] = forwardVec;


		if (bodypart->BP_type == BP_LEFT_ARM)
		{
			//reverse the rightVec and the forwardVec if they belong to the leftArm
			bodypart->next2->PC_OBB.ort.r[0] = DirectX::XMVectorScale(bodypart->next2->PC_OBB.ort.r[0], -1);
			bodypart->next2->PC_OBB.ort.r[2] = DirectX::XMVectorScale(bodypart->next2->PC_OBB.ort.r[2], -1);

			float axis1dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[0]));
			float axis2dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[1]));
			float axis3dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[2]));

			//constraint that keeps the arm from bending in weird ways
			if (axis3dot > 0)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[2]);
				bodypart->next2->PC_pos = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, para);
				this->CollitionDynamics(bodypart->next, bodypart->next2, DirectX::XMVectorScale(diffVec,-1), dt);
			}
			if (axis1dot > -0.1)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[0]);
				para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->next->PC_OBB.ort.r[0], -0.1));
				bodypart->next2->PC_pos = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, para);
			}
		}
		//constrints that keep the arm from bendin in weird ways
		else if (bodypart->BP_type == BP_RIGHT_ARM)
		{
			float axis1dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[0]));
			float axis2dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[1]));
			float axis3dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[2]));

			if (axis3dot > 0)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[2]);
				bodypart->next2->PC_pos = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, para);
			}
			
			if (axis1dot < 0.1)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[0]);
				para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->next->PC_OBB.ort.r[0], 0.1));
				bodypart->next2->PC_pos = DirectX::XMVectorSubtract(bodypart->next2->PC_pos, para);
			}
		}

	}
#pragma endregion Adjust right/left arm

#pragma region
	if (bodypart->BP_type == BP_RIGHT_LEG || bodypart->BP_type == BP_LEFT_LEG)
	{

		DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(bodypart->center->PC_pos, bodypart->next->PC_pos);
		diffVec = DirectX::XMVector3Normalize(diffVec);

		DirectX::XMVECTOR rightVec;
		DirectX::XMVECTOR para;
		DirectX::XMVECTOR perp;

		DirectX::XMVector3ComponentsFromNormal(&para, &perp, bodypart->center->PC_OBB.ort.r[0], diffVec);
		rightVec = DirectX::XMVector3Normalize(perp);


		DirectX::XMVECTOR forwardVec = DirectX::XMVector3Cross(rightVec, diffVec);

		bodypart->next->PC_OBB.ort.r[0] = rightVec;
		bodypart->next->PC_OBB.ort.r[1] = diffVec;
		bodypart->next->PC_OBB.ort.r[2] = forwardVec;

		float axis1dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[0]));
		float axis2dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[1]));
		float axis3dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->center->PC_OBB.ort.r[2]));

		if (axis2dot < 0)
		{
			DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[1]);
			bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->next->PC_pos, para);
		}
		if (bodypart->BP_type == BP_RIGHT_LEG)
		{
			if (axis1dot > 0)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[0]);
				bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->next->PC_pos, para);
			}
			if (axis1dot < -0.9)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[0]);
				para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[0], -0.9));
				bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->next->PC_pos, para);
			}
		}
		if (bodypart->BP_type == BP_LEFT_LEG)
		{
			if (axis1dot < 0)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[0]);
				bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->next->PC_pos, para);
			}
			if (axis1dot > 0.9)
			{
				DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->center->PC_OBB.ort.r[0]);
				para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->center->PC_OBB.ort.r[0], 0.9));
				bodypart->next->PC_pos = DirectX::XMVectorAdd(bodypart->next->PC_pos, para);
			}
		}


		//update foot to knee

		diffVec = DirectX::XMVectorSubtract(bodypart->next->PC_pos, bodypart->next2->PC_pos);
		diffVec = DirectX::XMVector3Normalize(diffVec);

		DirectX::XMVector3ComponentsFromNormal(&para, &perp, bodypart->next->PC_OBB.ort.r[0], diffVec);
		rightVec = DirectX::XMVector3Normalize(perp);

		forwardVec = DirectX::XMVector3Cross(rightVec, diffVec);

		bodypart->next2->PC_OBB.ort.r[0] = rightVec;
		bodypart->next2->PC_OBB.ort.r[1] = diffVec;
		bodypart->next2->PC_OBB.ort.r[2] = forwardVec;


		axis1dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[0]));
		axis2dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[1]));
		axis3dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(diffVec, bodypart->next->PC_OBB.ort.r[2]));
		float constraint = 0.1;
		if (axis1dot != 0 /*axis1dot > constraint || axis1dot < -constraint*/)
		{
			DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[0]);
			//if (axis1dot > constraint)
			//{
			//	para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->next->PC_OBB.ort.r[0], constraint));
			//}
			//else if (axis1dot < -constraint)
			//{
			//	para = DirectX::XMVectorSubtract(para, DirectX::XMVectorScale(bodypart->next->PC_OBB.ort.r[0], -constraint));
			//}
			bodypart->next2->PC_pos = DirectX::XMVectorAdd(bodypart->next2->PC_pos, para);
		}
		if (axis2dot < 0)
		{
			DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[1]);
			bodypart->next2->PC_pos = DirectX::XMVectorAdd(bodypart->next2->PC_pos, para);
		}
		if (axis3dot > 0)
		{
			DirectX::XMVector3ComponentsFromNormal(&para, &perp, diffVec, bodypart->next->PC_OBB.ort.r[2]);
			bodypart->next2->PC_pos = DirectX::XMVectorAdd(bodypart->next2->PC_pos, para);
		}


	}
#pragma endregion Adjust right/left Leg
}


bool PhysicsHandler::IntersectRayOBB(const DirectX::XMVECTOR & rayOrigin, const DirectX::XMVECTOR & rayDir, const OBB &obj, const DirectX::XMVECTOR &obbPos)
{
	Ray ray;
	ray.Origin = rayOrigin;
	ray.RayDir = rayDir;


	float t1, t2 = 0.0;
	const int NR_OF_NORMALS = 3;
	
	//Vec rayD = ray.d;
	DirectX::XMVECTOR radD = ray.RayDir;

	DirectX::XMVECTOR sideVector[NR_OF_NORMALS];

	sideVector[0] = obj.ort.r[0];
	sideVector[1] = obj.ort.r[1];
	sideVector[2] = obj.ort.r[2];

	float tMin;
	float tMax;

	tMin = -INFINITY;
	tMax = INFINITY;

	//Vec pointVec = this->Bcenter - ray.o;
	DirectX::XMVECTOR pointVec = DirectX::XMVectorSubtract(obbPos, ray.Origin);

	//rayD.Normalize();
	ray.RayDir = DirectX::XMVector3Normalize(radD);

	float temp;
	float length[NR_OF_NORMALS];

	length[0] = obj.ext[0];
	length[1] = obj.ext[1];
	length[2] = obj.ext[2];

	float e = 0.0f;
	float f = 0.0f;

	for (int i = 0; i < NR_OF_NORMALS; i++)
	{

		e = this->DotProduct(sideVector[i], pointVec);
		f = this->DotProduct(sideVector[i], ray.RayDir);

		if (abs(f) > 1e-20f)
		{
			t1 = (e + length[i]) / f;
			t2 = (e - length[i]) / f;

			if (t1 > t2)
			{
				//swap
				temp = t2;
				t2 = t1;
				t1 = temp;
			}
			if (t1 > tMin)
			{
				tMin = t1;
			}
			if (t2 < tMax)
			{
				tMax = t2;
			}
			if (tMin > tMax)
			{
				return false;
			}
			if (tMax < 0)
			{
				return false;
			}
		}
		else if ((-e - length[i]) > 0 || (-e + length[i] < 0))
		{
			return false;
		}
	}

	if (tMin > 0)
	{
		//min intersect
		//return tMin
	}
	else
	{
		//max intersect
		//return tMax;
	}

	/*
	if (tMin > 0)
	{
		// min intersect
		if (tMin < hit.t || hit.t == -1)
		{
			hit.t = tMin;
			hit.lastShape = this;
			pointVec = ray.o + ray.d*tMin; //calc point were it hits
			hit.lastNormal = this->normal(pointVec);
			//hit.color = this->c;
		}
	}
	else
	{
		//max intersect
		if (tMax < hit.t || hit.t == -1)
		{
			hit.t = tMax;
			hit.lastShape = this;
			pointVec = ray.o + ray.d*tMax; //calc point were it hits
			hit.lastNormal = this->normal(pointVec);
			//hit.color = this->c;
		}
	}
	*/
	return true;
}

bool PhysicsHandler::IntersectRayOBB(const DirectX::XMVECTOR & rayOrigin, const DirectX::XMVECTOR & rayDir, const OBB & obj, const DirectX::XMVECTOR & obbPos, float & distanceToOBB)
{
	Ray ray;
	ray.Origin = rayOrigin;
	ray.RayDir = rayDir;


	float t1, t2 = 0.0;
	const int NR_OF_NORMALS = 3;

	//Vec rayD = ray.d;
	DirectX::XMVECTOR radD = ray.RayDir;

	DirectX::XMVECTOR sideVector[NR_OF_NORMALS];

	sideVector[0] = obj.ort.r[0];
	sideVector[1] = obj.ort.r[1];
	sideVector[2] = obj.ort.r[2];

	float tMin;
	float tMax;

	tMin = -INFINITY;
	tMax = INFINITY;

	//Vec pointVec = this->Bcenter - ray.o;
	DirectX::XMVECTOR pointVec = DirectX::XMVectorSubtract(obbPos, ray.Origin);

	//rayD.Normalize();
	ray.RayDir = DirectX::XMVector3Normalize(radD);

	float temp;
	float length[NR_OF_NORMALS];

	length[0] = obj.ext[0];
	length[1] = obj.ext[1];
	length[2] = obj.ext[2];

	float e = 0.0f;
	float f = 0.0f;

	distanceToOBB = 0;
	for (int i = 0; i < NR_OF_NORMALS; i++)
	{

		e = this->DotProduct(sideVector[i], pointVec);
		f = this->DotProduct(sideVector[i], ray.RayDir);

		if (abs(f) > 1e-20f)
		{
			t1 = (e + length[i]) / f;
			t2 = (e - length[i]) / f;

			if (t1 > t2)
			{
				//swap
				temp = t2;
				t2 = t1;
				t1 = temp;
			}
			if (t1 > tMin)
			{
				tMin = t1;
			}
			if (t2 < tMax)
			{
				tMax = t2;
			}
			if (tMin > tMax)
			{
				return false;
			}
			if (tMax < 0)
			{
				return false;
			}
		}
		else if ((-e - length[i]) > 0 || (-e + length[i] < 0))
		{
			return false;
		}
	}

	if (tMin > 0)
	{
		//min intersect
		distanceToOBB = tMin;
	}
	else
	{
		//max intersect
		distanceToOBB = tMax;
	}

	return true;
}

bool PhysicsHandler::IntersectRaySphere(const DirectX::XMVECTOR & rayOrigin, const DirectX::XMVECTOR & rayDir, const Sphere & obj, const DirectX::XMVECTOR & pos, float & distance)
{
	DirectX::XMVECTOR p = DirectX::XMVectorSubtract(rayOrigin, pos);
	float r2 = obj.radius * obj.radius;
	float PDotD = DirectX::XMVector3Dot(p, rayDir).m128_f32[0];
	float PDotP = DirectX::XMVector3Dot(p, p).m128_f32[0];

	if (PDotD > 0 || PDotP < r2)
		return false;

	DirectX::XMVECTOR a = DirectX::XMVectorSubtract(p, DirectX::XMVectorScale(rayDir, PDotD));
	float ADotA = DirectX::XMVector3Dot(a, a).m128_f32[0];

	if (ADotA > r2)
		return false;

	float h = sqrt(r2 - ADotA);
	DirectX::XMVECTOR i = DirectX::XMVectorSubtract(a, DirectX::XMVectorScale(rayDir, h));

	DirectX::XMVECTOR intersection = DirectX::XMVectorAdd(pos, i);

	distance = abs(DirectX::XMVector3Length(DirectX::XMVectorSubtract(intersection, rayOrigin)).m128_f32[0]);

	return true;
}

Field * PhysicsHandler::CreateField(DirectX::XMVECTOR & pos, unsigned int entityID1, unsigned int entityID2, OBB* & obb)
{
	//this->m_fields.push_back(Field());
	//Field* field = &this->m_fields.at(this->m_fields.size() - 1);
	//DirectX::XMStoreFloat3(&field->F_pos, pos);
	//field->F_BV.ext[0] = obb->ext[0];
	//field->F_BV.ext[1] = obb->ext[1];
	//field->F_BV.ext[2] = obb->ext[2];
	//field->F_BV.ort = obb->ort;
	//field->F_entitityID1 = entityID1;
	//field->F_entitityID2 = entityID2;
	//field->F_first_inside = false;
	//field->F_second_inside = false;
	//return field;
	printf("A");
	Field temp;
	printf("B");
	temp.F_BV.ort = obb->ort;
	temp.F_BV.ext[0] = obb->ext[0];
	temp.F_BV.ext[1] = obb->ext[1];
	temp.F_BV.ext[2] = obb->ext[2];
	printf("C");
	DirectX::XMStoreFloat3(&temp.F_pos, pos);
	//temp.F_pos = pos;
	printf("D");

	temp.F_entitityID1 = entityID1;
	temp.F_entitityID2 = entityID2;
	temp.F_first_inside = false;
	temp.F_second_inside = false;
	printf("E");

	this->m_fields.push_back(temp);
	printf("F");
	return &this->m_fields.back();
}

void PhysicsHandler::SimpleCollition(float dt)
{
	float m_frictionConstant = 0.999f;
	PhysicsComponent* ptr;
	int size = this->m_physicsComponents.size();
	for (int i = 0; i < size; i++)
	{
		ptr = this->m_physicsComponents.at(i);


		DirectX::XMVECTOR pos = ptr->PC_pos;

		float y = DirectX::XMVectorGetY(pos);

		if (y > (0 + this->m_offSet))
		{
			SimpleGravity(ptr, dt);
		}
		else if (y < (0 + this->m_offSet))
		{
			ptr->PC_pos =  (DirectX::XMVectorSet(DirectX::XMVectorGetX(pos), (0 + this->m_offSet), DirectX::XMVectorGetZ(pos), 0.0f));
			DirectX::XMVECTOR vel = ptr->PC_velocity;
			ptr->PC_velocity = (DirectX::XMVectorSet(DirectX::XMVectorGetX(vel) * m_frictionConstant, 0, DirectX::XMVectorGetZ(vel) * m_frictionConstant, 0.0f));
		}
		else if (y == (0 + this->m_offSet))
		{
			DirectX::XMVECTOR vel = ptr->PC_velocity;
			ptr->PC_velocity = (DirectX::XMVectorSet(DirectX::XMVectorGetX(vel) * m_frictionConstant, 0.0f, DirectX::XMVectorGetZ(vel) * m_frictionConstant, 0.0f));
		}

		ptr->PC_pos = DirectX::XMVectorAdd(ptr->PC_pos, DirectX::XMVectorScale(ptr->PC_velocity, dt));
	}
}

void PhysicsHandler::SimpleGravity(PhysicsComponent* componentPtr, const float &dt)
{
	DirectX::XMVECTOR test = DirectX::XMVECTOR();
	DirectX::XMFLOAT3 testRes(0, 5, 0);
	test = DirectX::XMLoadFloat3(&testRes);


	componentPtr->PC_velocity = DirectX::XMVectorAdd(componentPtr->PC_velocity, this->m_gravity);

}

int PhysicsHandler::GetNrOfComponents()const
{
	return this->m_physicsComponents.size();
}

PhysicsComponent* PhysicsHandler::GetComponentAt(int index)const
{
	if (index >= 0 && index < (signed int)this->m_physicsComponents.size())
	{
		return this->m_physicsComponents.at(index);
	}
	return nullptr;

}

int PhysicsHandler::GetNrOfDynamicComponents() const
{
	return this->m_dynamicComponents.size();
}

PhysicsComponent * PhysicsHandler::GetDynamicComponentAt(int index) const
{
	if (index >= 0 && index < this->m_dynamicComponents.size())
	{
		return this->m_dynamicComponents.at(index);
	}
	return nullptr;
}


int PhysicsHandler::GetNrOfStaticComponents() const
{
	return this->m_staticComponents.size();
}

PhysicsComponent * PhysicsHandler::GetStaticComponentAt(int index) const
{
	if (index >= 0 && index < this->m_staticComponents.size())
	{
		return this->m_staticComponents.at(index);
	}
	return nullptr;
}

int PhysicsHandler::GetNrOfBodyComponents() const
{
	return this->m_bodyPC.size();
}
PhysicsComponent * PhysicsHandler::GetBodyComponentAt(int index) const
{
	if (index >= 0 && index < this->m_bodyPC.size())
	{
		return this->m_bodyPC.at(index);
	}
	return nullptr;
}

int PhysicsHandler::GetNrOfMagnets() const
{
	//return this->m_magnets.size();
	return 0;
}

//Magnet * PhysicsHandler::GetMagnetAt(int index)
//{
//	if (index >= 0 && index < this->m_magnets.size())
//	{
//		Magnet* ptr;
//		ptr = &this->m_magnets.at(index);
//		return ptr;
//	}
//	return nullptr;
//
//}

int PhysicsHandler::GetNrOfFields() const
{
	return this->m_fields.size();
}

Field * PhysicsHandler::GetFieldAt(int index)
{
	if (index >= 0 && index < this->m_fields.size())
	{
		Field* ptr;
		ptr = &this->m_fields.at(index);
		return ptr;
	}
	return nullptr;
}

void PhysicsHandler::SetBB_Rotation(const DirectX::XMVECTOR &rotVec, PhysicsComponent* toRotate)
{
	toRotate->PC_rotation = rotVec;
}

bool PhysicsHandler::checkCollition()
{
	bool result = false;

	std::chrono::time_point<std::chrono::system_clock>start;
	std::chrono::time_point<std::chrono::system_clock>end;

	start = std::chrono::system_clock::now();
	result = this->IntersectAABB();
	end = std::chrono::system_clock::now();

	std::chrono::duration<double>elapsed_secounds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	return result;
}

void PhysicsHandler::SortComponents()
{
	//this->m_nrOfStaticObjects = 20;
	//this->m_nrOfStaticObjects = 0;
	//int nrOfComponents = this->m_physicsComponents.size();
	//PhysicsComponent* current;
	//for (int i = 0; i < nrOfComponents; i++)
	//{
	//	current = this->m_physicsComponents.at(i);
	//	if (current->PC_is_Static)
	//	{
	//		this->m_nrOfStaticObjects++;
	//	}
	//}
	//int pivot = nrOfComponents - this->m_nrOfStaticObjects;
	//int lastKnownStatic = pivot;
	//PhysicsComponent* dynamicToSwap = nullptr;
	//PhysicsComponent* staticToSwap = nullptr;
	//for (int i = pivot; i < nrOfComponents; i++)
	//{
	//	current = this->m_physicsComponents.at(i);
	//	if(!current->PC_is_Static)
	//	{ 
	//		dynamicToSwap = this->m_physicsComponents.at(i);
	//		for (int x = lastKnownStatic; x >= 0 && dynamicToSwap != nullptr; x--)
	//		{
	//			staticToSwap = this->m_physicsComponents.at(x);
	//			if (staticToSwap->PC_is_Static)
	//			{
	//				lastKnownStatic = x;
	//				this->m_physicsComponents.at(i) = staticToSwap;
	//				this->m_physicsComponents.at(x) = dynamicToSwap;
	//				dynamicToSwap = nullptr;
	//			}

	//		}
	//	}
	//}
	//int a = this->m_physicsComponents.size();

	this->m_dynamicComponents.clear();
	this->m_staticComponents.clear();

	int size = this->m_physicsComponents.size();

	PhysicsComponent* ptr = nullptr;
	for (int i = 0; i < size; i++)
	{
		ptr = this->m_physicsComponents.at(i);
		if (ptr->PC_is_Static)
		{
			this->m_staticComponents.push_back(ptr);
		}
		else
		{
			this->m_dynamicComponents.push_back(ptr);
		}
	}

	int nrOfDynamic = this->m_dynamicComponents.size();
	int nrOfStatic = this->m_staticComponents.size();
	ptr = nullptr;
	for (int i = 0; i < nrOfDynamic; i++)
	{
		ptr = this->m_dynamicComponents.at(i);
		if (ptr->PC_is_Static)
		{
			int wrong = 1;
		}
	}
	for (int i = 0; i < nrOfStatic; i++)
	{
		ptr = this->m_staticComponents.at(i);
		if (!ptr->PC_is_Static)
		{
			int wrong = 1;
		}
	}
}


PHYSICSDLL_API void PhysicsHandler::TransferBoxesToBullet(PhysicsComponent * src, int index)
{	
	if (src->PC_BVtype == BV_AABB)
	{
		this->m_bullet.CreateAABB(src, index);
	}
	else if(src->PC_BVtype == BV_OBB)
	{
		this->m_bullet.CreateOBB(src,index);
	}
	else
	{
		int i = 0;
	}
}

PHYSICSDLL_API void PhysicsHandler::ApplyPlayer1ToBullet(PhysicsComponent * player1)
{
	this->m_bullet.SetPlayer1(player1);
}

PHYSICSDLL_API void PhysicsHandler::ApplyPlayer2ToBullet(PhysicsComponent * player2)
{
	this->m_bullet.SetPlayer2(player2);
}


PHYSICSDLL_API btRigidBody * PhysicsHandler::GetRigidBody(int index)
{
	return this->m_bullet.GetRigidBody(index);
}

void PhysicsHandler::SetRagdollToBindPose(Ragdoll* ragdoll, DirectX::XMVECTOR pos)
{
	for (int i = 0; i < 21; i++)
	{
		DirectX::XMMATRIX* inverseBindPose = &static_cast<DirectX::XMMATRIX>(ragdoll->Skeleton[i].invBindPose);
		this->m_playerRagDoll.jointMatrixes[i] = DirectX::XMMatrixInverse(nullptr, *inverseBindPose);
		this->TESTjointMatrixes[i] = DirectX::XMMatrixInverse(nullptr, *inverseBindPose);
	}

	for (int i = 0; i < 21; i++)
	{
		this->m_bodyPC.at(i)->PC_OBB.ort = ragdoll->playerPC->PC_OBB.ort;
		this->m_bodyPC.at(i)->PC_pos = DirectX::XMVectorAdd(DirectX::XMVector3Transform(ragdoll->jointMatrixes[i].r[3], ragdoll->playerPC->PC_OBB.ort), pos);
		this->m_bodyPC.at(i)->PC_OBB.ort.r[3] = DirectX::XMVectorSet(0, 0, 0, 1);
	}
	for (int i = 5; i < 9; i++)
	{
		int parentIndex = ragdoll->Skeleton[i].parentIndex;
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(this->m_bodyPC.at(i)->PC_pos, this->m_bodyPC.at(parentIndex)->PC_pos)));
		this->m_bodyPC.at(i)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(parentIndex)->PC_pos, DirectX::XMVectorScale(this->m_bodyPC.at(parentIndex)->PC_OBB.ort.r[0], distance));
	}
	for (int i = 9; i < 13; i++)
	{
		int parentIndex = ragdoll->Skeleton[i].parentIndex;
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(this->m_bodyPC.at(i)->PC_pos, this->m_bodyPC.at(parentIndex)->PC_pos)));
		this->m_bodyPC.at(i)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(parentIndex)->PC_pos, DirectX::XMVectorScale(this->m_bodyPC.at(parentIndex)->PC_OBB.ort.r[0], -distance));
	}
	for (int i = 14; i < 16; i++)
	{
		int parentIndex = ragdoll->Skeleton[i].parentIndex;
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(this->m_bodyPC.at(i)->PC_pos, this->m_bodyPC.at(parentIndex)->PC_pos)));
		this->m_bodyPC.at(i)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(parentIndex)->PC_pos, DirectX::XMVectorScale(this->m_bodyPC.at(parentIndex)->PC_OBB.ort.r[1], -distance));

	}
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(this->m_bodyPC.at(16)->PC_pos, this->m_bodyPC.at(15)->PC_pos)));
	this->m_bodyPC.at(16)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(15)->PC_pos, DirectX::XMVectorScale(this->m_bodyPC.at(15)->PC_OBB.ort.r[2], -distance));
	for (int i = 18; i < 20; i++)
	{
		int parentIndex = ragdoll->Skeleton[i].parentIndex;
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(this->m_bodyPC.at(i)->PC_pos, this->m_bodyPC.at(parentIndex)->PC_pos)));
		this->m_bodyPC.at(i)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(parentIndex)->PC_pos, DirectX::XMVectorScale(this->m_bodyPC.at(parentIndex)->PC_OBB.ort.r[1], -distance));

	}
	distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(this->m_bodyPC.at(20)->PC_pos, this->m_bodyPC.at(19)->PC_pos)));
	this->m_bodyPC.at(20)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(19)->PC_pos, DirectX::XMVectorScale(this->m_bodyPC.at(19)->PC_OBB.ort.r[2], -distance));
}

PHYSICSDLL_API void PhysicsHandler::SyncRagdollWithSkelton(Ragdoll * ragdoll)
{
	
	//center of body
	ragdoll->jointMatrixes[0] = ragdoll->lowerBody.center->PC_OBB.ort;
	ragdoll->jointMatrixes[0].r[3] = this->TESTjointMatrixes[0].r[3];

	//1-4 lowerbody - head
	ragdoll->jointMatrixes[1] = this->CalcTransformMatrix(ragdoll->upperBody.next3, ragdoll->upperBody.center);
	ragdoll->jointMatrixes[2] = this->CalcTransformMatrix(ragdoll->upperBody.center, ragdoll->upperBody.next4);
	ragdoll->jointMatrixes[3] = this->CalcTransformMatrix(ragdoll->upperBody.next4, ragdoll->upperBody.next5);
	ragdoll->jointMatrixes[4] = this->CalcTransformMatrix(ragdoll->upperBody.next5, ragdoll->upperBody.next5);
	//---
	//5-8 rightarm
	ragdoll->jointMatrixes[5] = this->CalcTransformMatrix(ragdoll->rightArm.center, ragdoll->rightArm.next);
	ragdoll->jointMatrixes[6] = this->CalcTransformMatrix(ragdoll->rightArm.next, ragdoll->rightArm.next2);
	ragdoll->jointMatrixes[7] = this->CalcTransformMatrix(ragdoll->rightArm.next2, ragdoll->rightArm.next3);
	ragdoll->jointMatrixes[8] = this->CalcTransformMatrix(ragdoll->rightArm.next3, ragdoll->rightArm.next3);
	//---
	//9-12 leftarm
	ragdoll->jointMatrixes[9] = this->CalcTransformMatrix(ragdoll->leftArm.center, ragdoll->leftArm.next);
	ragdoll->jointMatrixes[10] = this->CalcTransformMatrix(ragdoll->leftArm.next, ragdoll->leftArm.next2);
	ragdoll->jointMatrixes[11] = this->CalcTransformMatrix(ragdoll->leftArm.next2, ragdoll->leftArm.next3);
	ragdoll->jointMatrixes[12] = this->CalcTransformMatrix(ragdoll->leftArm.next3, ragdoll->leftArm.next3);
	//----
	//13-16 rightLeg
	ragdoll->jointMatrixes[13] = this->CalcTransformMatrix(ragdoll->rightLeg.center, ragdoll->rightLeg.next);
	ragdoll->jointMatrixes[14] = this->CalcTransformMatrix(ragdoll->rightLeg.next, ragdoll->rightLeg.next2);
	ragdoll->jointMatrixes[15] = this->CalcTransformMatrix(ragdoll->rightLeg.next2, ragdoll->rightLeg.next3);
	ragdoll->jointMatrixes[16] = this->CalcTransformMatrix(ragdoll->rightLeg.next3, ragdoll->rightLeg.next3);
	//----
	//17-20 leftLeg
	ragdoll->jointMatrixes[17] = this->CalcTransformMatrix(ragdoll->leftLeg.center, ragdoll->leftLeg.next);
	ragdoll->jointMatrixes[18] = this->CalcTransformMatrix(ragdoll->leftLeg.next, ragdoll->leftLeg.next2);
	ragdoll->jointMatrixes[19] = this->CalcTransformMatrix(ragdoll->leftLeg.next2, ragdoll->leftLeg.next3);
	ragdoll->jointMatrixes[20] = this->CalcTransformMatrix(ragdoll->leftLeg.next3, ragdoll->leftLeg.next3);
	//----

	for (int childIndex = 1; childIndex < 21; childIndex++)
	{

		int parentI = ragdoll->Skeleton[childIndex].parentIndex;
		DirectX::XMMATRIX parentTransform = ragdoll->jointMatrixes[parentI];

		DirectX::XMMATRIX childTransform = ragdoll->jointMatrixes[childIndex];

		if (childIndex == 2)
		{
			int a = 0;

		}

		DirectX::XMVECTOR parentOffSet = this->TESTjointMatrixes[parentI].r[3];

		DirectX::XMVECTOR childOffSet = this->TESTjointMatrixes[childIndex].r[3];

		DirectX::XMVECTOR offset = DirectX::XMVectorSubtract(childOffSet, parentOffSet);

		DirectX::XMMATRIX relationTransform = DirectX::XMMatrixMultiply(childTransform, parentTransform);

		offset = DirectX::XMVector3Transform(offset, parentTransform);

		relationTransform.r[3] = offset;


		ragdoll->jointMatrixes[childIndex] = relationTransform;
	}

	//rotations[0] = this->m_bodyPC.at(0)->PC_OBB.ort;
	//for (int i = 1; i < 21; i++)
	//{
	//	int parentIndex = ragdoll->Skeleton[i].parentIndex;
	//	rotations[i] = this->m_bodyPC.at(i)->PC_OBB.ort;
	//	ragdoll->jointMatrixes[i] = DirectX::XMMatrixTranslationFromVector((this->m_bodyPC.at(i)->PC_pos, this->m_bodyPC.at(parentIndex)->PC_pos)) * rotations[i];
	//}



	//ragdoll->jointMatrixes[0].r[3] = this->TESTjointMatrixes[0].r[3];

	//for (int i = 1; i < 21; i++)
	//{
	//	int parentIndex = ragdoll->Skeleton[i].parentIndex;
	//	ragdoll->jointMatrixes[i].r[3] = DirectX::XMVectorSubtract(this->m_bodyPC.at(i)->PC_pos, this->m_bodyPC.at(parentIndex)->PC_pos);
	//	ragdoll->jointMatrixes[i].r[3] = this->TESTjointMatrixes[i].r[3];
	//	//ragdoll->jointMatrixes[i] = this->TESTjointMatrixes[i] * this->m_bodyPC.at(i)->PC_OBB.ort;
	//	ragdoll->jointMatrixes[i] = this->m_bodyPC.at(parentIndex)->PC_OBB.ort * this->m_bodyPC.at(i)->PC_OBB.ort;
	//}

}

PHYSICSDLL_API DirectX::XMMATRIX PhysicsHandler::CalcTransformMatrix(PhysicsComponent * joint2, PhysicsComponent* joint3)
{
	DirectX::XMMATRIX orto1 = joint2->PC_OBB.ort;
	DirectX::XMMATRIX orto2 = joint3->PC_OBB.ort;

	DirectX::XMMATRIX localAxises = orto2;
	localAxises = DirectX::XMMatrixMultiply(orto2, DirectX::XMMatrixInverse(nullptr, orto1));

	return localAxises;
}

PHYSICSDLL_API DirectX::XMMATRIX  PhysicsHandler::CalcNewRotationAxises(PhysicsComponent * parent, PhysicsComponent * child)
{
	DirectX::XMMATRIX parentRot = parent->PC_OBB.ort;
	DirectX::XMMATRIX childRot = child->PC_OBB.ort;

	DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, parentRot), childRot);
	//DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(childRot, DirectX::XMMatrixInverse(nullptr, parentRot));

	return result;
}

PHYSICSDLL_API void PhysicsHandler::MovePhysicsJoint(DirectX::XMVECTOR toMove, int index, int nrOfChildren)
{
	for (int i = index; i < index + nrOfChildren; i++)
	{
		this->m_bodyPC.at(i)->PC_pos = DirectX::XMVectorAdd(this->m_bodyPC.at(i)->PC_pos, toMove);
	}
}

#ifdef _DEBUG



void PhysicsHandler::GetPhysicsComponentOBB(OBB*& src, int index)
{
	src = &(this->m_physicsComponents.at(index)->PC_OBB);
}

void PhysicsHandler::GetPhysicsComponentAABB(AABB*& src, int index)
{
	src = &(this->m_physicsComponents.at(index)->PC_AABB);
}

void PhysicsHandler::GetPhysicsComponentPlane(Plane*& src, int index)
{
	src = &(this->m_physicsComponents.at(index)->PC_Plane);
}

void PhysicsHandler::GetPhysicsComponentSphere(Sphere *& src, int index)
{
	src = &(this->m_physicsComponents.at(index)->PC_Sphere);
}

#endif 
PHYSICSDLL_API Ragdoll * PhysicsHandler::GetPlayerRagdoll()
{
	return &this->m_playerRagDoll;
}