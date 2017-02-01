#include "DebugHandler.h"

//DebugHandler* DebugHandler::m_instance = nullptr;

DebugHandler::DebugHandler()
{
	QueryPerformanceFrequency(&this->m_frequency);
	this->m_displayFPS = true;
	this->ClearConsole();
	for (int i = 0; i < this->m_FRAMES_FOR_AVG; i++)
	{
		this->m_frameTimes[i] = 40;
	}
	this->m_minFPS = 999999;
	this->m_maxFPS = 0;
}

void DebugHandler::ClearConsole()
{
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}


DebugHandler::~DebugHandler()
{
}

DebugHandler* DebugHandler::instance()
{
	static DebugHandler instance;
	return &instance;
}

int DebugHandler::SetComponentHandler(ComponentHandler * compHandler)
{
	this->compHandler = compHandler;
	this->m_fpsTextComp = this->compHandler->GetTextComponent();

	return 0;
}

int DebugHandler::StartTimer(size_t timerID)
{
	int result = 0;

	if (timerID < this->m_timers.size())
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		this->m_timers.at(timerID).startTime = currTime;
		result = 1;
	}

	return result;
}

int DebugHandler::EndTimer(size_t timerID)
{
	int result = 0;

	if (timerID < this->m_timers.size())
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		this->m_timers.at(timerID).endTime = currTime;
		result = 1;
	}

	return result;
}

int DebugHandler::CreateTimer(std::wstring label)
{
	Timer timer;
	if (this->compHandler == nullptr)
	{
		return -1;
	}
	timer.textComp = this->compHandler->GetTextComponent();
	timer.label = label;
	timer.textComp->text = label + L": [" + L"0" + L"] "
		+ L"0" + L" [" + L"0" + L"] us, " + L"0.0" + L"%";
	timer.textComp->active = false;
	timer.textComp->position = DirectX::XMFLOAT2(20.f, 20.f + (this->m_timers.size() * 30.f));
	timer.textComp->scale = DirectX::XMFLOAT2(.4f, .4f);

	this->m_timers.push_back(timer);

	return 0;
}

int DebugHandler::StartProgram()
{
	QueryPerformanceCounter(&this->m_programStart);

	return 0;
}

int DebugHandler::EndProgram()
{
	QueryPerformanceCounter(&this->m_programEnd);

	return 0;
}

int DebugHandler::ShowFPS(bool show)
{
	this->m_displayFPS = show;

	return 0;
}

int DebugHandler::ToggleDebugInfo()
{
	if (this->m_displayDebug)
	{
		this->m_displayDebug = false;

		for (std::vector<Timer>::iterator iter = this->m_timers.begin(); iter != this->m_timers.end(); iter++)
		{
			iter->textComp->active = false;
		}
		for (std::vector<Value>::iterator iter = this->m_values.begin(); iter != this->m_values.end(); iter++)
		{
			iter->textComp->active = false;
		}

	}
	else 
	{
		this->m_displayDebug = true;

		for (std::vector<Timer>::iterator iter = this->m_timers.begin(); iter != this->m_timers.end(); iter++)
		{
			iter->textComp->active = true;
		}
		for (std::vector<Value>::iterator iter = this->m_values.begin(); iter != this->m_values.end(); iter++)
		{
			iter->textComp->active = true;
		}
	}

	this->ClearConsole();

	return 0;
}

int DebugHandler::CreateCustomLabel(std::wstring label, float value)
{
	Value tempValue;
	TextComponent* textComp = this->compHandler->GetTextComponent();
	textComp->active = false;
	tempValue.textComp = textComp;
	tempValue.textComp->position = DirectX::XMFLOAT2(20.f, 20.f + ((this->m_timers.size() + this->m_values.size()) * 30.f));
	tempValue.textComp->scale = DirectX::XMFLOAT2(.4f, .4f);
	tempValue.label = label;
	tempValue.value = value;
	this->m_values.push_back(tempValue);

	return 0;
}

int DebugHandler::UpdateCustomLabel(int labelID, float newValue)
{
	if ((unsigned int)labelID < this->m_values.size())
	{
		this->m_values.at(labelID).value = newValue;
	}
	else 
	{
		return -1;
	}

	return 0;
}

int DebugHandler::UpdateCustomLabelIncrease(int labelID, float addValue)
{
	if ((unsigned int)labelID < this->m_values.size())
	{
		this->m_values.at(labelID).value += addValue;
	}
	else
	{
		return -1;
	}

	return 0;
}

int DebugHandler::ResetMinMax()
{
	std::vector<Timer>::iterator iter;

	for (iter = this->m_timers.begin(); iter != this->m_timers.end(); iter++)
	{
		iter->maxTime = 0;
		iter->minTime = 999999;
	}

	this->m_minFPS = 999999;
	this->m_maxFPS = 0;

	return 0;
}

int DebugHandler::DisplayConsole(float dTime)
{
	if (!this->m_displayDebug)
	{
		return 0;
	}
	COORD topLeft = { 0, 0 };
	COORD FPSLocation = { 50, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	//GetConsoleScreenBufferInfo(console, &screen);
	/*FillConsoleOutputCharacterA(
		console, ' ', toClear.X * toClear.Y, topLeft, &written
	);*/
	/*FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		toClear.X * toClear.Y, topLeft, &written
	);*/
	SetConsoleCursorPosition(console, topLeft);

	std::vector<Timer>::iterator iter;

	unsigned int time;
	int i;
	for (i = 0, iter = this->m_timers.begin();
		iter != this->m_timers.end();
		i++, iter++)
	{
		time = iter->GetTimeMS(this->m_frequency);

		LARGE_INTEGER elapsedTime;
		elapsedTime.QuadPart = this->m_programEnd.QuadPart - this->m_programStart.QuadPart;
		elapsedTime.QuadPart *= 1000000;
		elapsedTime.QuadPart /= this->m_frequency.QuadPart;

		std::wcout << std::fixed << std::setprecision(1) << iter->label << ": [" << iter->minTime << "] "
			<< time << " [" << iter->maxTime << "] us, " 
			<< (float)((time / (float)elapsedTime.QuadPart) * 100) << "%";
		GetConsoleScreenBufferInfo(console, &screen);
		FillConsoleOutputCharacterA(
			console, ' ', 5, screen.dwCursorPosition, &written
		);
		std::cout << std::endl;
	}
	int nrOfCustomLabels = this->m_values.size();
	for (int j = 0; j < nrOfCustomLabels; j++)
	{
		std::wcout << this->m_values.at(j).label << ": " << this->m_values.at(j).value;
		GetConsoleScreenBufferInfo(console, &screen);
		FillConsoleOutputCharacterA(
			console, ' ', 5, screen.dwCursorPosition, &written
		);
		std::cout << std::endl;
	}

	if (this->m_displayFPS)
	{
		int sum = 0, avgFPS;
		this->m_currFrameTimesPtr = (this->m_currFrameTimesPtr >= this->m_FRAMES_FOR_AVG) ? 0 : this->m_currFrameTimesPtr;
		this->m_frameTimes[this->m_currFrameTimesPtr] = (unsigned int)(1000000 / dTime);
		for (int k = 0; k < this->m_FRAMES_FOR_AVG; k++)
		{
			sum += this->m_frameTimes[k];
		}
		avgFPS = sum / this->m_FRAMES_FOR_AVG;
		this->m_minFPS = (this->m_minFPS < this->m_frameTimes[this->m_currFrameTimesPtr]) ? this->m_minFPS : this->m_frameTimes[this->m_currFrameTimesPtr];
		this->m_maxFPS = (this->m_maxFPS > this->m_frameTimes[this->m_currFrameTimesPtr]) ? this->m_maxFPS : this->m_frameTimes[this->m_currFrameTimesPtr];
		SetConsoleCursorPosition(console, FPSLocation);
		std::cout << "FPS: " << avgFPS << " [" << this->m_minFPS << "] (" << std::to_string(this->m_frameTimes[this->m_currFrameTimesPtr]) << ") [" << this->m_maxFPS << "]";
		GetConsoleScreenBufferInfo(console, &screen);
		FillConsoleOutputCharacterA(
			console, ' ', 8, screen.dwCursorPosition, &written
		);
		this->m_currFrameTimesPtr++;
		
	}

	COORD finishedCursonLoc = { (SHORT)0, (SHORT)(this->m_timers.size() + (size_t)nrOfCustomLabels + 1) };
	SetConsoleCursorPosition(console, finishedCursonLoc);

	return 1;
}

int DebugHandler::DisplayOnScreen(float dTime)
{
	std::vector<Timer>::iterator iter;
	unsigned int time;
	int i;
	for (i = 0, iter = this->m_timers.begin();
		iter != this->m_timers.end();
		i++, iter++)
	{
		time = iter->GetTimeMS(this->m_frequency);

		LARGE_INTEGER elapsedTime;
		elapsedTime.QuadPart = this->m_programEnd.QuadPart - this->m_programStart.QuadPart;
		elapsedTime.QuadPart *= 1000000;
		elapsedTime.QuadPart /= this->m_frequency.QuadPart;

		iter->textComp->text = iter->label + L": [" + std::to_wstring(iter->minTime) + L"] "
			+ std::to_wstring(time) + L" [" + std::to_wstring(iter->maxTime) + L"] us, "
			+ std::to_wstring((float)((time / (float)elapsedTime.QuadPart) * 100)) + L"%";
	}

	int nrOfCustomLabels = this->m_values.size();
	for (int j = 0; j < nrOfCustomLabels; j++)
	{
		this->m_values.at(j).textComp->text = this->m_values.at(j).label + L": "
			+ std::to_wstring(this->m_values.at(j).value);
	}

	if (this->m_displayFPS)
	{
		int sum = 0, avgFPS;
		this->m_currFrameTimesPtr = (this->m_currFrameTimesPtr >= this->m_FRAMES_FOR_AVG) ? 0 : this->m_currFrameTimesPtr;
		this->m_frameTimes[this->m_currFrameTimesPtr] = (unsigned int)(1000000 / dTime);
		for (int k = 0; k < this->m_FRAMES_FOR_AVG; k++)
		{
			sum += this->m_frameTimes[k];
		}
		avgFPS = sum / this->m_FRAMES_FOR_AVG;
		this->m_minFPS = (this->m_minFPS < this->m_frameTimes[this->m_currFrameTimesPtr]) ? this->m_minFPS : this->m_frameTimes[this->m_currFrameTimesPtr];
		this->m_maxFPS = (this->m_maxFPS > this->m_frameTimes[this->m_currFrameTimesPtr]) ? this->m_maxFPS : this->m_frameTimes[this->m_currFrameTimesPtr];
		
		this->m_fpsTextComp->text = L"FPS: " + std::to_wstring(avgFPS) + L" ["
			+ std::to_wstring(this->m_minFPS) + L"] (" + std::to_wstring(this->m_frameTimes[this->m_currFrameTimesPtr])
			+ L") [" + std::to_wstring(this->m_maxFPS) + L"]";

		this->m_currFrameTimesPtr++;
	}

	return 0;
}

void DebugHandler::Shutdown()
{
	this->m_timers.clear();
	this->m_values.clear();
}
