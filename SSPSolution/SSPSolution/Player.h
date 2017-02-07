#ifndef SSPAPPLICATION_ENTITIES_PLAYER_H
#define SSPAPPLICATION_ENTITIES_PLAYER_H

#include "..\GraphicsDLL\AnimationStateEnums.h"
#include "Entity.h"

class Player :
	public Entity
{
private:
	//Meters per second
	float m_speed;
	float m_throwStrength;
	bool m_isAiming;
	Entity* m_ball;

	Entity* m_grabbed;
	DirectX::XMVECTOR m_carryOffset;

	DirectX::XMVECTOR m_lookDir;
	DirectX::XMVECTOR m_upDir;
	DirectX::XMVECTOR m_rightDir;

	irrklang::ISound* m_walkingSound;

public:
	Player();
	~Player();

	int Initialize(int entityID, PhysicsComponent* pComp, GraphicsComponent* gComp, AnimationComponent* aComp);

	int Update(float dT, InputHandler* inputHandler);
	int React(int entityID, EVENT reactEvent);

	//Returns the old value
	Entity* SetGrabbed(Entity* entityPtr);
	float SetSpeed(float speed);
	DirectX::XMVECTOR SetLookDir(DirectX::XMVECTOR lookDir);
	DirectX::XMVECTOR SetUpDir(DirectX::XMVECTOR upDir);
	DirectX::XMVECTOR SetRightDir(DirectX::XMVECTOR rightDir);
	void SetAiming(bool isAming);
	void SetBall(Entity* ball);

	bool stateExists(int animationState);
	void SetAnimationComponent(int animationState, float transitionDuration, Blending blendingType, bool isLooping, bool lockAnimation);

	float GetSpeed();
	DirectX::XMVECTOR GetLookDir();
	DirectX::XMVECTOR GetUpDir();
	DirectX::XMVECTOR GetRightDir();
	bool GetIsAming();
	Entity* GetGrabbed();
	Entity* GetBall();

private:

};

#endif