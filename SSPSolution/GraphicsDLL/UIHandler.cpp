#include "UIHandler.h"

UIHandler::UIHandler()
{
}

UIHandler::~UIHandler()
{
}

void UIHandler::Initialize()
{
	this->m_maxComponents = 10;
	this->m_nrOfComponents = 0;
	for (int i = 0; i < this->m_maxComponents; i++)
	{
		UIComponent* newUIComp = new UIComponent;
		this->m_UIComponents.push_back(newUIComp);
	}
}

void UIHandler::Shutdown()
{
	for (int i = 0; i < this->m_maxComponents; i++)
	{
		delete this->m_UIComponents.at(i);
	}
}

UIComponent* UIHandler::GetNextUIComponent()
{
	if (this->m_nrOfComponents < this->m_maxComponents)
	{
		return this->m_UIComponents.at(this->m_nrOfComponents++);
	}
	return nullptr;
}