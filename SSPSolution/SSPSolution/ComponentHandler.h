#ifndef SSPAPPLICATION_COMPONENTHANDLER_H
#define SSPAPPLICATION_COMPONENTHANDLER_H

//#include "ComponentStructs.h"
#include "../GraphicsDLL/GraphicsHandler.h"
#include "../GraphicsDLL/AnimationHandler.h"
#include "../physicsDLL/PhysicsHandler.h"
#include "../AIDLL/AIHandler.h"

class ComponentHandler
{
private:
	GraphicsHandler* m_graphicsHandler;
	PhysicsHandler* m_physicsHandler;
	AIHandler* m_aiHandler;
	AnimationHandler* m_aHandler;
public:
	ComponentHandler();
	~ComponentHandler();

	//Returns 0 if the graphicsHandler or physicshandler is a nullptr
	int Initialize(GraphicsHandler* graphicsHandler, PhysicsHandler* physicsHandler, AIHandler* aiHandler, AnimationHandler* aHandler);

	GraphicsComponent* GetGraphicsComponent();
	GraphicsAnimationComponent * GetGraphicsAnimationComponent();
	PhysicsComponent* GetPhysicsComponent();
	AIComponent* GetAIComponent();
	AnimationComponent* GetAnimationComponent();
	void UpdateGraphicsComponents();
	void UpdateGraphicsAnimationComponents();
	void UpdateAIComponents();
	void SetGraphicsComponentListSize(int gCompSize);
	void SetGraphicsAnimationComponentListSize(int gCompSize);
	//temporary function
	PhysicsHandler* GetPhysicsHandler() const;
};

#endif