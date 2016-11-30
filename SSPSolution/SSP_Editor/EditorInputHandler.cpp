#include "EditorInputHandler.h"

EditorInputHandler::EditorInputHandler(HINSTANCE handleInstance, HWND handle, Camera* camera, int w, int h)
{
	this->m_Width = w;
	this->m_Height = h;
	HRESULT hr = DirectInput8Create(
		handleInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_directInput,
		NULL);

	hr = m_directInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = m_directInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(handle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(handle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	this->m_hwnd = handle;
	this->m_Camera = camera;
	this->m_PreviousPos = camera->GetCameraPos();
}

EditorInputHandler::~EditorInputHandler()
{
}

void EditorInputHandler::detectInput(double dT)
{

	DIMOUSESTATE mouseCurrentState;

	BYTE keyBoardState[256]; // the amount of buttons a char array of 256.

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);

	DIKeyboard->GetDeviceState(sizeof(keyBoardState), (LPVOID)&keyBoardState);

	float speed = 10.0f * dT;
	float speedrot = 5.0f * dT;
	int result = 1;
	float translateCameraX = 0, translateCameraY = 0, translateCameraZ = 0;
	float yaw = 0, pitch = 0;

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(this->m_hwnd, &mousePos);

#pragma region SHIFT + ALT +

#pragma endregion

#pragma region CONTROL +
	if (keyBoardState[DIK_LCONTROL] & 0x80)
	{
		if (mouseCurrentState.rgbButtons[0])
		{
			DirectX::XMVECTOR rayOrigin, rayDirection;
			int m_MouseX = mousePos.x;
			int m_MouseY = mousePos.y;
			if (m_MouseX > m_Width)
				m_MouseX = m_Width;
			else if (m_MouseX < 0)
				m_MouseX = 0;
			if (m_MouseY > m_Height)
				m_MouseY = m_Height;
			else if (m_MouseY < 0)
				m_MouseY = 0;

			DirectX::XMVECTOR localRayDirection = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			DirectX::XMVECTOR LocalRayOrigin = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			float X, Y, Z;
			DirectX::XMFLOAT4X4 camProjection;
			float fieldOfView = (float)DirectX::XM_PI / 4.0f;
			float screenAspect = (float)m_Width / (float)m_Height;

			DirectX::XMStoreFloat4x4(
				&camProjection,
				DirectX::XMMatrixPerspectiveFovLH(
					fieldOfView,
					screenAspect,
					0.1f,
					1000.0f)
			);

			//Transform 2D pick position on screen space to 3D ray in View space
			X = (((2.0f * m_MouseX) / m_Width) - 1) / camProjection._11;
			Y = -(((2.0f * m_MouseY) / m_Height) - 1) / camProjection._22;
			Z = 1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

			localRayDirection = DirectX::XMVectorSet(X, Y, Z, 0.0f);

			DirectX::XMMATRIX inverseCamView;
			DirectX::XMVECTOR det = { 1,1,1,1 };
			DirectX::XMMATRIX temp;
			this->m_Camera->GetBaseViewMatrix(temp);

			//inverseCamView = DirectX::XMMatrixInverse(&det, temp);

			//rayOrigin = XMVector3TransformCoord(LocalRayOrigin, inverseCamView);
			//rayDirection = DirectX::XMVector3TransformNormal(localRayDirection, inverseCamView);
			//rayDirection = DirectX::XMVector3Normalize(rayDirection);

			DirectX::XMFLOAT4 test;
			DirectX::XMStoreFloat4(&test, localRayDirection);
			this->m_Camera->SetLookAt(test);
		}
	}
#pragma endregion

#pragma region SHIFT +
	if (keyBoardState[DIK_LSHIFT] & 0x80)
	{
		if (keyBoardState[DIK_W] & 0x80)
		{
			translateCameraZ += speed;
		}
		if (keyBoardState[DIK_S] & 0x80)
		{
			translateCameraZ -= speed;
		}
		if (keyBoardState[DIK_A] & 0x80)
		{
			translateCameraX -= speed;
		}
		if (keyBoardState[DIK_D] & 0x80)
		{
			translateCameraX += speed;
		}
		if (keyBoardState[DIK_C] & 0x80)
		{
			translateCameraY -= speed;
		}
		if (keyBoardState[DIK_SPACE] & 0x80)
		{
			translateCameraY += speed;
		}
		if (mouseCurrentState.rgbButtons[0])
		{
			if (mouseCurrentState.lY < 0)
			{
				translateCameraZ += speedrot;
			}
			if (mouseCurrentState.lY > 0)
			{
				translateCameraZ -= speedrot;
			}

		}


	}
#pragma endregion

#pragma region ALT +
	if (keyBoardState[DIK_LALT] & 0x80)
	{
		if (mouseCurrentState.rgbButtons[0])
		{
			if ((mouseCurrentState.lX!= m_mouseLastState.lX) || (mouseCurrentState.lY != m_mouseLastState.lY))
			{
				pitch += mouseCurrentState.lX * 0.01f;
		
				yaw += mouseCurrentState.lY * 0.01f;

				// Ensure the mouse location doesn't exceed the screen width or height.
				if (m_MouseX < 0) { m_MouseX = 0; }
				if (m_MouseY < 0) { m_MouseY = 0; }

				if (m_MouseX > m_Width) { m_MouseX = m_Width; }
				if (m_MouseY > m_Height) { m_MouseY = m_Height; }

				m_mouseLastState = mouseCurrentState;
			}
		}
	}
#pragma endregion
	
#pragma region ONLY MOUSE
	if (mouseCurrentState.rgbButtons[0] &&
		!keyBoardState[DIK_LALT] & 0x80 ||
		!keyBoardState[DIK_LSHIFT] & 0x80)
	{
		DirectX::XMVECTOR rayOrigin, rayDirection;
		int m_MouseX = mousePos.x;
		int m_MouseY = mousePos.y;
		if (m_MouseX > m_Width)
			m_MouseX = m_Width;
		else if (m_MouseX < 0)
			m_MouseX = 0;
		if (m_MouseY > m_Height)
			m_MouseY = m_Height;
		else if (m_MouseY < 0)
			m_MouseY = 0;

		DirectX::XMVECTOR localRayDirection = 
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR LocalRayOrigin = 
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		float X, Y, Z;
		DirectX::XMFLOAT4X4 camProjection;
		float fieldOfView = (float)DirectX::XM_PI / 4.0f;
		float screenAspect = (float)m_Width / (float)m_Height;

		DirectX::XMStoreFloat4x4(
			&camProjection,
			DirectX::XMMatrixPerspectiveFovLH(
				fieldOfView,
				screenAspect,
				0.1f,
				1000.0f)
		);
		X = (((2.0f * m_MouseX) / m_Width) - 1) / camProjection._11;
		Y = -(((2.0f * m_MouseY) / m_Height) - 1) / camProjection._22;
		Z = 1.0f;
		localRayDirection = DirectX::XMVectorSet(X, Y, Z, 0.0f);

		DirectX::XMMATRIX inverseCamView;
		DirectX::XMVECTOR det = { 1,1,1,1 };
		DirectX::XMMATRIX temp;
		this->m_Camera->GetBaseViewMatrix(temp);

		inverseCamView = DirectX::XMMatrixInverse(&det, temp);

		rayOrigin = XMVector3TransformCoord(LocalRayOrigin, inverseCamView);
		rayDirection = DirectX::XMVector3TransformNormal(localRayDirection, inverseCamView);
		rayDirection = DirectX::XMVector3Normalize(rayDirection);

		//DO PICKING TEST TO FIND MESH IN SCENE
		//GET POINTER?
		//STORE AS LAST PICK
		//NEED A GET FUNC SO OTHER CLASSES CAN GET CURRENT PICK
		//HIGHLIGHT?


		//DirectX::XMFLOAT4 test;
		//DirectX::XMStoreFloat4(&test, localRayDirection);
		//this->m_Camera->SetLookAt(test);
	}
#pragma endregion




	if (keyBoardState[DIK_F] & 0x80)
	{
		this->m_Camera->Initialize();
		this->m_Camera->Update();
	}




	if ((translateCameraY || translateCameraZ || translateCameraX))
	{
		DirectX::XMFLOAT3 posTranslation =
			DirectX::XMFLOAT3(
				float(translateCameraX),
				float(translateCameraY),
				float(translateCameraZ)
			);

		this->m_PreviousPos = this->m_Camera->GetCameraPos();
		this->m_Camera->ApplyLocalTranslation(
			float(translateCameraX),
			float(translateCameraY),
			float(translateCameraZ)
		);
		//this->m_Camera->AddToCameraPos(posTranslation);
		//this->m_Camera->AddToLookAt(posTranslation);
		this->m_Camera->Update();
	}

		float rotationAmount = DirectX::XM_PI / 8;

		DirectX::XMFLOAT4 newRotation = 
			DirectX::XMFLOAT4(
				yaw * DirectX::XMScalarSin(rotationAmount / 2.0f),
				pitch * DirectX::XMScalarSin(rotationAmount / 2.0f),
				0.0f,
				DirectX::XMScalarCos(rotationAmount / 2.0f)
			);

		this->m_Camera->SetRotation(newRotation);
		//this->m_Camera->ApplyRotation(newRotation);
		this->m_Camera->Update();
	this->m_Camera->Update();
}
