#include "System.h"



System::System()
{
	this->m_inputHandler = NULL;
	this->m_window = NULL;
}


System::~System()
{
}

int System::Shutdown()
{
	int result = 0;
	this->m_inputHandler->Shutdown();
	delete this->m_inputHandler;
	//Destroy the display window
	SDL_DestroyWindow(m_window);
	//Quit SDL subsystems
	SDL_Quit();
	return result;
}

int System::Initialize()
{
	int result = 1;
	this->m_fullscreen = false;
	this->m_running = true;
	this->m_window = NULL;
	//Get the instance if this application
	this->m_hinstance = GetModuleHandle(NULL);


	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL failed in initializing the window! SDL_Error: %hS\n", SDL_GetError());
	}
	else
	{
		printf("SDL succeeded in initializing the window!\n");
	}

	m_window = SDL_CreateWindow("SSD Application", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (m_window == NULL)
	{
		printf("Window creation failed! SDL_ERROR: %hS\n", SDL_GetError());
	}
	else
	{
		printf("Window creation succeeded!\n");

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(m_window, &wmInfo);
		m_hwnd = wmInfo.info.win.window;
	}

	//Initialize the InputHandler
	this->m_inputHandler = new InputHandler();
	this->m_inputHandler->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return result;
}

int System::Run()
{
	int result = 0;

	while (this->m_running)
	{
		//Prepare the InputHandler
		this->m_inputHandler->Update();
		//Handle events and update inputhandler
		result = this->HandleEvents();
		SDL_PumpEvents();
		//Update game
		//Render
	}


	return result;
}

int System::HandleEvents()
{
	SDL_Event m_event;
	while (SDL_PollEvent(&m_event))
	{
		switch (m_event.type)
		{
#pragma region
		case SDL_WINDOWEVENT:
		{
			switch (m_event.window.event)
			{
			case SDL_WINDOWEVENT_ENTER:
			{
				//OnMouseFocus();
				break;
			}
			case SDL_WINDOWEVENT_LEAVE:
			{
				//OnMouseBlur();
				break;
			}
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			{
				//OnInputFocus();
				break;
			}
			case SDL_WINDOWEVENT_FOCUS_LOST:
			{
				//OnInputBlur();
				break;
			}
			case SDL_WINDOWEVENT_SHOWN:
			{
				break;
			}
			case SDL_WINDOWEVENT_HIDDEN:
			{
				break;
			}
			case SDL_WINDOWEVENT_EXPOSED:
			{
				break;
			}
			case SDL_WINDOWEVENT_MOVED:
			{
				break;
			}
			case SDL_WINDOWEVENT_RESIZED:
			{
				break;
			}
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			{
				break;
			}
			case SDL_WINDOWEVENT_MINIMIZED:
			{
				break;
			}
			case SDL_WINDOWEVENT_MAXIMIZED:
			{
				break;
			}
			case SDL_WINDOWEVENT_RESTORED:
			{
				break;
			}
			case SDL_WINDOWEVENT_CLOSE:
			{
				break;
			}
			}
			break;
		}
#pragma endregion window events
		case SDL_MOUSEMOTION:
		{
			break;
		}
		case SDL_QUIT:
		{
			//The big X in the corner
			this->m_running = false;
			break;
		}
#pragma region
		case SDL_KEYDOWN:
		{
			//OnKeyDown(Event->key.keysym.sym, Event->key.keysym.mod, Event->key.keysym.scancode);
			
			this->m_inputHandler->SetKeyState(m_event.key.keysym.scancode, true);
			break;
		}
		case SDL_KEYUP:
		{
			//OnKeyUp(Event->key.keysym.sym, Event->key.keysym.mod, Event->key.keysym.scancode);
			this->m_inputHandler->SetKeyState(m_event.key.keysym.scancode, false);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			break;
		}
#pragma endregion Key / Button events
		case SDL_MOUSEWHEEL:
		{
			this->m_inputHandler->ApplyMouseWheel(m_event.wheel.x, m_event.wheel.y);
			break;
		}
		}
	}
	return 1;
}

int System::FullscreenToggle()
{
	int result = 0;

	bool IsFullscreen = SDL_GetWindowFlags(this->m_window) & SDL_WINDOW_FULLSCREEN;
	SDL_SetWindowFullscreen(this->m_window, IsFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
	SDL_ShowCursor(IsFullscreen);
	return result;
}
