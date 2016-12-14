#include "Camera.h"



Camera::Camera()
{
	DirectX::XMStoreFloat4x4(&this->m_viewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_baseViewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_projectionMatrix, DirectX::XMMatrixIdentity());
	this->m_cameraPos = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_lookAt = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_cameraUp = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	this->m_rotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//this->m_rotateAroundPos = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->m_screenAspect = 0.0f;
	this->m_fieldOfView = 0.0f;
}


Camera::~Camera()
{
}

int Camera::Initialize(float screenAspect, float fieldOfView, float nearPlane, float farPlane)
{
	int result = 1;

	DirectX::XMStoreFloat4x4(&this->m_viewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_baseViewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&this->m_projectionMatrix, DirectX::XMMatrixIdentity());
	//The three vectors that defines the new coordinate system
	this->m_cameraPos = DirectX::XMFLOAT4(0.0f, 2.0f, -2.5f, 1.0f);
	this->m_lookAt = DirectX::XMFLOAT4(0.0f, 2.0f, 1.0f, 1.0f);
	this->m_cameraUp = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	this->m_rotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//this->m_rotateAroundPos = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->m_screenAspect = 0.0f;
	this->m_fieldOfView = 0.0f;

	//Define the basic view matrix used in rendering the second stage of deferred rendering.
	DirectX::XMVECTOR camPos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	DirectX::XMVECTOR camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&this->m_baseViewMatrix, DirectX::XMMatrixLookAtLH(camPos, lookAt, camUp));

	//Define a transformation matrix based on the three rotations a 3D object is capable of
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&this->m_rotation));
	//Transform the three components of the view matrix based on the rotations
	camPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&this->m_cameraPos), camRotationMatrix);
	lookAt = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&this->m_lookAt), camRotationMatrix);
	camUp = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&this->m_cameraUp), camRotationMatrix);
	//Define the view matrix based on the transformed positions
	DirectX::XMStoreFloat4x4(&this->m_viewMatrix, DirectX::XMMatrixLookAtLH(camPos, lookAt, camUp));
	
	//Create the projection matrix
	DirectX::XMStoreFloat4x4(&this->m_projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, nearPlane, farPlane));
	return result;
}

int Camera::Update()
{
	int result = 1;

	//Define a transformation matrix based on the three rotations a 3D object is capable of
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&this->m_rotation));
	this->m_rotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	//Transform the three components of the view matrix based on the rotations
	DirectX::XMFLOAT4 tempLookAt = DirectX::XMFLOAT4(this->m_lookAt.x - this->m_cameraPos.x, this->m_lookAt.y - this->m_cameraPos.y, this->m_lookAt.z - this->m_cameraPos.z, 1.0f);
	DirectX::XMVECTOR lookAt = DirectX::XMVectorAdd(DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&tempLookAt), camRotationMatrix), DirectX::XMLoadFloat4(&this->m_cameraPos));
	/*DirectX::XMVECTOR camPos = DirectX::XMVectorSet(-this->m_rotateAroundPos.x, -this->m_rotateAroundPos.y, -this->m_rotateAroundPos.z, 1.0f);
	camPos = DirectX::XMVectorAdd(DirectX::XMVector3TransformCoord(camPos, camRotationMatrix), DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&this->m_rotateAroundPos), DirectX::XMLoadFloat4(&this->m_cameraPos)));*/
	DirectX::XMVECTOR camUp = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&this->m_cameraUp), camRotationMatrix);

	DirectX::XMStoreFloat4(&this->m_lookAt, lookAt);
	//DirectX::XMStoreFloat4(&this->m_cameraPos, camPos);
	DirectX::XMStoreFloat4(&this->m_cameraUp, camUp);
	//Define the view matrix based on the transformed positions
	DirectX::XMStoreFloat4x4(&this->m_viewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&this->m_cameraPos), lookAt, camUp));

	return result;
}

int Camera::UpdateProjection()
{
	int result = 1;
	if (this->m_screenAspect <= 0 || this->m_fieldOfView <= 0)
	{
		result = 0;
	}
	else
	{
		//Update the projection matrix
		DirectX::XMStoreFloat4x4(&this->m_projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(this->m_fieldOfView, this->m_screenAspect, 0.1f, 1000.0f));
	}

	return result;
}

#pragma region
void Camera::GetViewMatrix(DirectX::XMMATRIX & storeIn)
{
	storeIn = DirectX::XMLoadFloat4x4(&this->m_viewMatrix);
	return;
}

DirectX::XMFLOAT4X4 * Camera::GetViewMatrix()
{
	return &this->m_viewMatrix;
}

void Camera::GetBaseViewMatrix(DirectX::XMMATRIX & storeIn)
{
	storeIn = DirectX::XMLoadFloat4x4(&this->m_baseViewMatrix);
	return;
}

DirectX::XMFLOAT4X4 * Camera::GetProjectionMatrix()
{
	return &this->m_projectionMatrix;
}

void Camera::GetCameraPos(DirectX::XMVECTOR & storeIn)
{
	storeIn = DirectX::XMLoadFloat4(&this->m_cameraPos);
	return;
}

void Camera::GetCameraPos(DirectX::XMFLOAT4 & storeIn)
{
	storeIn = DirectX::XMFLOAT4(this->m_cameraPos);
	return;
}

void Camera::GetCameraPos(DirectX::XMFLOAT3 & storeIn)
{
	storeIn = DirectX::XMFLOAT3(this->m_cameraPos.x, this->m_cameraPos.y, this->m_cameraPos.z);
	return;
}

DirectX::XMFLOAT3 Camera::GetCameraPos()
{
	return DirectX::XMFLOAT3(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
}

void Camera::GetLookAt(DirectX::XMVECTOR & storeIn)
{
	storeIn = DirectX::XMLoadFloat4(&this->m_lookAt);
	return;
}

void Camera::GetLookAt(DirectX::XMFLOAT4 & storeIn)
{
	storeIn = DirectX::XMFLOAT4(this->m_lookAt);
	return;
}

void Camera::GetLookAt(DirectX::XMFLOAT3 & storeIn)
{
	storeIn = DirectX::XMFLOAT3(this->m_lookAt.x, this->m_lookAt.y, this->m_lookAt.z);
	return;
}

DirectX::XMFLOAT3 Camera::GetLookAt()
{
	return DirectX::XMFLOAT3(m_lookAt.x, m_lookAt.y, m_lookAt.z);
}

void Camera::GetCameraUp(DirectX::XMVECTOR & storeIn)
{
	storeIn = DirectX::XMLoadFloat4(&this->m_cameraUp);
	return;
}

void Camera::GetCameraUp(DirectX::XMFLOAT4 & storeIn)
{
	storeIn = DirectX::XMFLOAT4(this->m_cameraUp);
}

void Camera::GetCameraUp(DirectX::XMFLOAT3 & storeIn)
{
	storeIn = DirectX::XMFLOAT3(this->m_cameraUp.x, this->m_cameraUp.y, this->m_cameraUp.z);
	return;
}
#pragma endregion getters
#pragma region


void Camera::SetCameraPos(DirectX::XMFLOAT4 newCamPos)
{
	this->m_cameraPos = DirectX::XMFLOAT4(newCamPos);
	return;
}

void Camera::SetCameraPos(DirectX::XMVECTOR newCamPos)
{
	DirectX::XMStoreFloat4(&this->m_cameraPos, newCamPos);
	return;
}

void Camera::SetLookAt(DirectX::XMFLOAT4 newLookAt)
{
	this->m_lookAt = DirectX::XMFLOAT4(newLookAt);
	return;
}


void Camera::SetLookAt(DirectX::XMVECTOR newLookAt)
{
	DirectX::XMStoreFloat4(&this->m_lookAt, newLookAt);
	return;
}

void Camera::SetCameraUp(DirectX::XMFLOAT4 newCamUp)
{
	this->m_lookAt = DirectX::XMFLOAT4(newCamUp);
	return;
}


void Camera::SetCameraUp(DirectX::XMVECTOR newCamUp)
{
	DirectX::XMStoreFloat4(&this->m_cameraUp, newCamUp);
	return;
}

void Camera::SetRotation(DirectX::XMFLOAT4 newRotation)
{
	this->m_rotation = newRotation;
}

void Camera::AddToCameraPos(DirectX::XMFLOAT3 applyValue)
{
	this->m_cameraPos = DirectX::XMFLOAT4(this->m_cameraPos.x + applyValue.x, this->m_cameraPos.y + applyValue.y, this->m_cameraPos.z + applyValue.z, 1.0f);
	return;
}

void Camera::AddToLookAt(DirectX::XMFLOAT3 applyValue)
{
	this->m_lookAt = DirectX::XMFLOAT4(this->m_lookAt.x + applyValue.x, this->m_lookAt.y + applyValue.y, this->m_lookAt.z + applyValue.z, 1.0f);
	return;
}

void Camera::AddToCameraUp(DirectX::XMFLOAT3 applyValue)
{
	this->m_cameraUp = DirectX::XMFLOAT4(this->m_cameraUp.x + applyValue.x, this->m_cameraUp.y + applyValue.y, this->m_cameraUp.z + applyValue.z, 1.0f);
	return;
}

void Camera::MultiplyCameraPos(DirectX::XMFLOAT3 multiplyValue)
{
	this->m_cameraPos = DirectX::XMFLOAT4(this->m_cameraPos.x * multiplyValue.x, this->m_cameraPos.y * multiplyValue.y, this->m_cameraPos.z * multiplyValue.z, 1.0f);
	return;
}

void Camera::MultiplyLookAt(DirectX::XMFLOAT3 multiplyValue)
{
	this->m_lookAt = DirectX::XMFLOAT4(this->m_lookAt.x * multiplyValue.x, this->m_lookAt.y * multiplyValue.y, this->m_lookAt.z * multiplyValue.z, 1.0f);
	return;
}

void Camera::MultiplyCameraUp(DirectX::XMFLOAT3 multiplyValue)
{
	this->m_cameraUp = DirectX::XMFLOAT4(this->m_cameraUp.x * multiplyValue.x, this->m_cameraUp.y * multiplyValue.y, this->m_cameraUp.z * multiplyValue.z, 1.0f);
	return;
}

void Camera::ApplyRotation(DirectX::XMFLOAT4 rotationAddition)
{
	//this->m_rotation = DirectX::XMFLOAT4(this->m_rotation.x + rotationAddition.x, this->m_rotation.y + rotationAddition.y, this->m_rotation.z + rotationAddition.z, this->m_rotation.w + rotationAddition.w);
	DirectX::XMStoreFloat4(&this->m_rotation, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&rotationAddition), DirectX::XMLoadFloat4(&this->m_rotation)));
	return;
}

void Camera::SetLocalTranslation(float x, float y, float z)
{
	//Define the three vectors that make up the cameras rotated coordinate system
	DirectX::XMVECTOR forwards = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&this->m_lookAt), DirectX::XMLoadFloat4(&this->m_cameraPos));
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4(&this->m_cameraUp);
	DirectX::XMVECTOR right = DirectX::XMVector3Cross(forwards, up);
	//The translation in along the 3 local axis
	forwards = DirectX::XMVectorScale(forwards, x);
	up = DirectX::XMVectorScale(up, y);
	right = DirectX::XMVectorScale(right, z);
	//Combine the three translations
	DirectX::XMFLOAT4 translation;
	DirectX::XMStoreFloat4(&translation, DirectX::XMVectorAdd(DirectX::XMVectorAdd(forwards, up), right));
	forwards = DirectX::XMVector3Normalize(forwards);
	//Apply the translation to the camera & lookAt position
	this->m_cameraPos.x = translation.x;
	this->m_cameraPos.y = translation.y;
	this->m_cameraPos.z = translation.z;
	this->m_lookAt.x = translation.x + DirectX::XMVectorGetX(forwards);
	this->m_lookAt.y = translation.y + DirectX::XMVectorGetY(forwards);
	this->m_lookAt.z = translation.z + DirectX::XMVectorGetZ(forwards);
}

void Camera::ApplyLocalTranslation(float x, float y, float z)
{
	//Define the three vectors that make up the cameras rotated coordinate system
	DirectX::XMVECTOR forwards = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&this->m_lookAt), DirectX::XMLoadFloat4(&this->m_cameraPos));
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4(&this->m_cameraUp);
	DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, forwards);
	//The translation in along the 3 local axis
	forwards = DirectX::XMVectorScale(forwards, z);
	up = DirectX::XMVectorScale(up, y);
	right = DirectX::XMVectorScale(right, x);
	//Combine the three translations
	DirectX::XMFLOAT4 translation;
	DirectX::XMStoreFloat4(&translation, DirectX::XMVectorAdd(DirectX::XMVectorAdd(forwards, up), right));
	//Apply the translation to the camera & lookAt position
	this->m_cameraPos.x += translation.x;
	this->m_cameraPos.y += translation.y;
	this->m_cameraPos.z += translation.z;
	this->m_lookAt.x += translation.x;
	this->m_lookAt.y += translation.y;
	this->m_lookAt.z += translation.z;
}
void Camera::ApplyLocalTranslation(DirectX::XMFLOAT3 translation)
{
	this->ApplyLocalTranslation(translation.x, translation.y, translation.z);
}
void Camera::AlignWithRay(DirectX::XMVECTOR direction)
{
	//Align camera rotation with direction
	//Define the rotation between the ray and the camera
	DirectX::XMVECTOR cameraDir = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&this->m_lookAt), DirectX::XMLoadFloat4(&this->m_cameraPos));
	DirectX::XMVECTOR rotateVec = DirectX::XMVector3Cross(direction, cameraDir);
	float rotateAmount = DirectX::XMScalarACos(DirectX::XMVectorGetX(DirectX::XMVector3Dot(direction, cameraDir)));
	float tempRotateAxis = DirectX::XMScalarASin(rotateAmount / 2);
	//Define the rotation as a quaternion
	DirectX::XMVECTOR rotation = DirectX::XMVectorSet(tempRotateAxis * DirectX::XMVectorGetX(rotateVec), tempRotateAxis * DirectX::XMVectorGetY(rotateVec), tempRotateAxis * DirectX::XMVectorGetZ(rotateVec), DirectX::XMScalarACos(rotateAmount / 2));
	//Apply the rotation
	DirectX::XMStoreFloat4(&this->m_rotation, rotation);
	this->Update();
}
//void Camera::SetRotationAroundPosOffset(float x, float y, float z)
//{
//	this->m_rotateAroundPos.x = x;
//	this->m_rotateAroundPos.y = y;
//	this->m_rotateAroundPos.z = z;
//}
//void Camera::SetRotationAroundPos(float x, float y, float z)
//{
//	this->m_rotateAroundPos.x = x - this->m_cameraPos.x;
//	this->m_rotateAroundPos.y = y - this->m_cameraPos.y;
//	this->m_rotateAroundPos.z = z - this->m_cameraPos.z;
//}
#pragma endregion setters