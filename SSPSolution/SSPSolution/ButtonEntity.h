#ifndef SSPAPPLICATION_ENTITIES_BUTTONENTITY_H
#define SSPAPPLICATION_ENTITIES_BUTTONENTITY_H
#include "Entity.h"
class ButtonEntity :
	public Entity
{
private:
	//Variables
	bool m_isActive;
	float m_range;
	float m_resetTime;
	float m_elapsedResetTime;
public:
	ButtonEntity();
	virtual ~ButtonEntity();

	int Update(float dT, InputHandler* inputHandler);
	int React(int entityID, EVENT reactEvent);

	//Time in seconds
	int Initialize(int entityID, PhysicsComponent* pComp, GraphicsComponent* gComp, float interactionDistance = 1.5f, float resetTime = 4.0f);

	//For now we check only if the player is close enough
	int CheckPressed(DirectX::XMFLOAT3 playerPos);

private:
	//Functions
};

#endif