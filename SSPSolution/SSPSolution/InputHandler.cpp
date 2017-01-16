#include "InputHandler.h"

InputHandler::InputHandler()
{
	this->m_mouseX = 0;
	this->m_mouseY = 0;
	int m_mouseDX = 0;
	int m_mouseDY = 0;
	int m_mouseWheelX = 0;
	int m_mouseWheelY = 0;
	this->m_screenWidth = 0;
	this->m_screenHeight = 0;
	this->m_mouseButtonState.left = 0;
	this->m_mouseButtonState.right = 0;
	this->m_mouseButtonState.middle = 0;
	this->m_mouseButtonState.x1 = 0;
	this->m_mouseButtonState.x2 = 0;
	this->m_oldMouseButtonState = this->m_mouseButtonState;
}

InputHandler::~InputHandler()
{
	this->Shutdown();
}

void InputHandler::Initialize(int screenWidth, int screenHeight, SDL_Window * window)
{
	this->m_mouseX = 0;
	this->m_mouseY = 0;
	int m_mouseDX = 0;
	int m_mouseDY = 0;
	int m_mouseWheelX = 0;
	int m_mouseWheelY = 0;
	this->m_mouseButtonState.left = 0;
	this->m_mouseButtonState.right = 0;
	this->m_mouseButtonState.middle = 0;
	this->m_mouseButtonState.x1 = 0;
	this->m_mouseButtonState.x2 = 0;
	this->m_oldMouseButtonState = this->m_mouseButtonState;
	//Save the resolution for future use
	this->m_screenWidth = screenWidth;
	this->m_screenHeight = screenHeight;

	SDL_WarpMouseInWindow(window, m_screenWidth/2, m_screenHeight/2);
	
	return;
}

void InputHandler::Shutdown()
{
	this->m_oldKeyboardState.clear();
	this->m_keyboardState.clear();
	return;
}

void InputHandler::Update()
{

	//Check if we can read the devices. 
	//If we cant, the old data will be used

	this->ReadKeyboard();
	this->ReadMouse();

	this->ProcessInput();
	return;
}

void InputHandler::SetMouseState(int button, bool state)
{
	switch (button)
	{
	case 1:
		this->m_mouseButtonState.left = state;
		break;
	case 2:
		this->m_mouseButtonState.middle = state;
		break;
	case 3:
		this->m_mouseButtonState.right = state;
		break;
	case 4:
		this->m_mouseButtonState.x1 = state;
		break;
	case 5:
		this->m_mouseButtonState.x2 = state;
		break;
	default:
		break;
	}
	return;
}

void InputHandler::SetKeyState(int key, bool state)
{
	this->m_keyboardState[key] = state;
	return;
}

void InputHandler::ReadKeyboard()
{
	//Copy the old data
	this->m_oldKeyboardState = this->m_keyboardState;
	return;
}

void InputHandler::ReadMouse()
{
	//Copy the old data
	this->m_oldMouseButtonState = this->m_mouseButtonState;
	//Read the new data
	int xPos = 0, yPos = 0;
	SDL_GetMouseState(&xPos, &yPos);
	this->m_mouseDX = xPos - this->m_mouseX;
	this->m_mouseDY = yPos - this->m_mouseY;
	this->m_mouseX = xPos;
	this->m_mouseY = yPos;
	return;
}

void InputHandler::ProcessInput()
{
	//Set the mousewheel to 0; 0 so the new values are not corrupted by the old state
	this->m_mouseWheelX = 0;
	this->m_mouseWheelY = 0;
	return;
}

bool InputHandler::IsKeyDown(unsigned int key)
{
	if (key > 0 && key < SDL_NUM_SCANCODES)
	{
		if (this->m_keyboardState[key])
		{
			return true;
		}
	}

	return false;
}

bool InputHandler::IsKeyPressed(unsigned int key)
{
	if (key > 0 && key < SDL_NUM_SCANCODES)
	{
		if (!this->m_oldKeyboardState[key] && this->m_keyboardState[key])
		{
			return true;
		}
	}

	return false;
}

bool InputHandler::IsKeyReleased(unsigned int key)
{
	if (key > 0 && key < SDL_NUM_SCANCODES)
	{
		if (this->m_oldKeyboardState[key] && !this->m_keyboardState[key])
		{
			return true;
		}
	}

	return false;
}

bool InputHandler::IsMouseKeyPressed(unsigned int key)
{
	bool result = false;
	switch (key)
	{
	case SDL_BUTTON_LEFT: result = this->m_mouseButtonState.left && !this->m_oldMouseButtonState.left;
		break;
	case SDL_BUTTON_MIDDLE:	result = this->m_mouseButtonState.middle && !this->m_oldMouseButtonState.middle;
		break;
	case SDL_BUTTON_RIGHT:	result = this->m_mouseButtonState.right && !this->m_oldMouseButtonState.right;
		break;
	case SDL_BUTTON_X1:	result = this->m_mouseButtonState.x1 && !this->m_oldMouseButtonState.x1;
		break;
	case SDL_BUTTON_X2:	result = this->m_mouseButtonState.x2 && !this->m_oldMouseButtonState.x2;
		break;
	default:
		break;
	}
	return result;
}

bool InputHandler::IsMouseKeyDown(unsigned int key)
{
	bool result = false;
	switch (key)
	{
	case SDL_BUTTON_LEFT: result = this->m_mouseButtonState.left;
		break;
	case SDL_BUTTON_MIDDLE:	result = this->m_mouseButtonState.middle;
		break;
	case SDL_BUTTON_RIGHT:	result = this->m_mouseButtonState.right;
		break;
	case SDL_BUTTON_X1:	result = this->m_mouseButtonState.x1;
		break;
	case SDL_BUTTON_X2:	result = this->m_mouseButtonState.x2;
		break;
	default:
		break;
	}
	return result;
}

bool InputHandler::IsMouseKeyReleased(unsigned int key)
{
	bool result = false;
	switch (key)
	{
	case SDL_BUTTON_LEFT: result = !this->m_mouseButtonState.left && this->m_oldMouseButtonState.left;
		break;
	case SDL_BUTTON_MIDDLE:	result = !this->m_mouseButtonState.middle && this->m_oldMouseButtonState.middle;
		break;
	case SDL_BUTTON_RIGHT:	result = !this->m_mouseButtonState.right && this->m_oldMouseButtonState.right;
		break;
	case SDL_BUTTON_X1:	result = !this->m_mouseButtonState.x1 && this->m_oldMouseButtonState.x1;
		break;
	case SDL_BUTTON_X2:	result = !this->m_mouseButtonState.x2 && this->m_oldMouseButtonState.x2;
		break;
	default:
		break;
	}
	return result;
}

void InputHandler::SetMousePos(int x, int y)
{
	this->m_mouseX = x;
	this->m_mouseY = y;
	return;
}

void InputHandler::SetMouseWheel(int x, int y)
{
	this->m_mouseWheelX = x;
	this->m_mouseWheelY = y;
	return;
}

void InputHandler::ApplyMouseWheel(int x, int y)
{
	this->m_mouseWheelX += x;
	this->m_mouseWheelY += y;
	return;
}

void InputHandler::mouseMovement(SDL_Window * window, float &pitch, float &yaw)
{
	int tmpx, tmpy;
	float mouseSens = 0.02;

	int midx = this->m_screenWidth / 2;
	int midy = this->m_screenHeight / 2;

	SDL_ShowCursor(SDL_DISABLE);
	SDL_GetMouseState(&tmpx, &tmpy);
	
	yaw = mouseSens * (midx - tmpx);
	pitch = mouseSens * (midy - tmpy);


	SDL_WarpMouseInWindow(window, midx, midy);
}



DirectX::XMFLOAT2 InputHandler::GetMousePos()
{
	return DirectX::XMFLOAT2((float)m_mouseX, (float)m_mouseY);
}

DirectX::XMFLOAT2 InputHandler::GetMouseDelta()
{
	return DirectX::XMFLOAT2((float)this->m_mouseDX, (float)this->m_mouseDY);
}

DirectX::XMFLOAT2 InputHandler::GetMouseWheel()
{
	return DirectX::XMFLOAT2(float(this->m_mouseWheelX), float(this->m_mouseWheelY));
}
