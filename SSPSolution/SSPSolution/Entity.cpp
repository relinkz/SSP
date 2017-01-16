#include "Entity.h"



Entity::Entity()
{
}


Entity::~Entity()
{
}

PhysicsComponent* Entity::SetPhysicsComponent(PhysicsComponent * pComp)
{
	PhysicsComponent* tempReturn = this->m_pComp;
	this->m_pComp = pComp;
	return tempReturn;
}

GraphicsComponent* Entity::SetGraphicsComponent(GraphicsComponent * gComp)
{
	GraphicsComponent* tempReturn = this->m_gComp;
	this->m_gComp = gComp;
	return tempReturn;
}

bool Entity::SetGrabbed(int isGrabbed)
{
	bool lastValue = this->m_isGrabbed;
	this->m_isGrabbed = isGrabbed;
	return lastValue;
}

int Entity::SetEntityID(int entityID)
{
	int lastValue = this->m_entityID;
	this->m_entityID = entityID;
	return lastValue;
}

PhysicsComponent * Entity::GetPhysicsComponent()
{
	return this->m_pComp;
}

GraphicsComponent * Entity::GetGraphicComponent()
{
	return this->m_gComp;
}

bool Entity::GetGrabbed()
{
	return this->m_isGrabbed;
}

int Entity::GetEntityID()
{
	return this->m_entityID;
}

int Entity::InitializeBase(int entityID, PhysicsComponent* pComp, GraphicsComponent* gComp)
{
	int result = 1;
	this->m_isGrabbed = false;
	this->m_subject = Subject();
	this->m_entityID = entityID;
	this->m_pComp = pComp;
	this->m_gComp = gComp;
	return result;
}