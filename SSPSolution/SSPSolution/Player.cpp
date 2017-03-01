#include "Player.h"



Player::Player()
{
	this->m_maxSpeed = 5.0f;
	this->m_acceleration = 5.0f;
	this->m_grabbed = nullptr;
	this->m_isAiming = false;
	this->m_walkingSound = nullptr;
	this->m_oldAnimState = 0;
	this->m_timeSinceThrow = 0;
	this->m_ragdoll = nullptr;
}

Player::~Player()
{
	if (this->m_walkingSound)
	{
		this->m_walkingSound->drop();
	}
}

int Player::Initialize(int entityID, PhysicsComponent * pComp, GraphicsComponent * gComp, AnimationComponent* aComp)
{
	int result = 0;

	this->InitializeBase(entityID, pComp, gComp, aComp);
	this->m_maxSpeed = 11.0f;
	this->m_acceleration = 5.0f;
	this->m_grabbed = nullptr;
	this->m_lookDir = DirectX::XMVectorSet(0, 0, 1, 0);
	this->m_carryOffset = DirectX::XMVectorSet(0, 0, 0, 0);
	this->m_walkingSound = nullptr;
	this->m_chainSoundTimer = 0.0f;

	return result;
}

int Player::Update(float dT, InputHandler* inputHandler)
{
	int result = 0;
	//Determine the player behavior that should accour based on the user input
	//Map the user input to values
	int sideways = 0, forwards = 0;
	float rotationY = 0.0f;
	if (inputHandler->IsKeyDown(SDL_SCANCODE_O))
	{
		this->m_ragdoll->state = ANIMATED;
	
		this->m_oldAnimState = this->m_aComp->previousState;
		SetAnimationComponent(PLAYER_IDLE, 0, Blending::NO_TRANSITION, true, false, 2.0f, 1.0f);			
		this->m_aComp->previousState = PLAYER_IDLE;
	}
	if (inputHandler->IsKeyDown(SDL_SCANCODE_K))
	{
		DirectX::XMVECTOR vel = DirectX::XMVectorScale(this->m_lookDir, 30);

		this->m_ball->GetPhysicsComponent()->PC_velocity = vel;
		
	}
	if (inputHandler->IsKeyDown(SDL_SCANCODE_H))
	{
		DirectX::XMVECTOR vel = DirectX::XMVectorSet(0, 0, 0, 0);

		this->m_ragdoll->upperBody.center->PC_velocity = vel;
		this->m_ragdoll->upperBody.next->PC_velocity = vel;
		this->m_ragdoll->upperBody.next2->PC_velocity = vel;

		this->m_ball->GetPhysicsComponent()->PC_velocity = vel;
	}
	if (inputHandler->IsKeyDown(SDL_SCANCODE_L))
	{
		this->m_ragdoll->state = RAGDOLL_TRANSITION;
		this->m_ragdoll->playerPC->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
		this->m_ragdoll->ballPC->PC_velocity = DirectX::XMVectorSet(0.1, 0, 0, 0);
	}

	if (this->m_ragdoll != nullptr)
	{
		if (this->m_ragdoll->state == KEYFRAMEBLEND)
		{
			//int animationIndex = PLAYER_RISE_UP;
			//DirectX::XMMATRIX test[21];
			//for (int i = 0; i < 21; i++)
			//{
			//	//DirectX::XMFLOAT3 translation = { 
			//	//	this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].translation[0] ,
			//	//	this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].translation[1] ,
			//	//	this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].translation[2] };
			//	//

			//	DirectX::XMMATRIX BindPose = DirectX::XMMatrixInverse(nullptr, static_cast<DirectX::XMMATRIX>(this->m_aComp->skeleton->GetSkeletonData()->joints[i].invBindPose));
			//	DirectX::XMMATRIX inverseBindPose = static_cast<DirectX::XMMATRIX>(this->m_aComp->skeleton->GetSkeletonData()->joints[i].invBindPose);

			//	DirectX::XMVECTOR animPos;
			//	animPos = DirectX::XMVectorSet(
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].translation[0],
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].translation[1],
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].translation[2],
			//		1.0f);

			//	DirectX::XMFLOAT4 quat = {
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].quaternion[0] ,
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].quaternion[1] ,
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].quaternion[2] ,
			//		this->m_aComp->skeleton->GetAnimation(animationIndex)->GetJoint(i)->keyframes[0].quaternion[3] };

			//	DirectX::XMVECTOR jointPos;
			//	jointPos = this->m_ragdoll->jointMatrixes[i].r[3];
			//	//animPos = DirectX::XMVector3Transform(animPos, BindPose);
			//	DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(animPos, jointPos);

			//	//this->m_ragdoll->jointMatrixes[i].r[3] = DirectX::XMVectorAdd(this->m_ragdoll->jointMatrixes[i].r[3], DirectX::XMVectorScale(diffVec, 0.01));
			//	//DirectX::XMVECTOR translation = DirectX::XMVectorAdd(jointPos, DirectX::XMVectorScale(diffVec, 0.01));
			//	DirectX::XMVECTOR translation = DirectX::XMVectorLerp(jointPos, animPos, 0.01f);


			//	this->m_ragdoll->jointMatrixes[i] = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslationFromVector(translation), DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quat)));
			//	this->m_ragdoll->jointMatrixes[i] = DirectX::XMMatrixTranslationFromVector(animPos);
			//	this->m_ragdoll->jointMatrixes[i] = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranslationFromVector(animPos), DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quat)));

			//	test[i] = this->m_ragdoll->jointMatrixes[i];

			//	int parent = this->m_ragdoll->Skeleton[i].parentIndex;

			//	if (parent != -1)
			//	{
			//		this->m_ragdoll->jointMatrixes[i] = DirectX::XMMatrixMultiply(this->m_ragdoll->jointMatrixes[i], this->m_ragdoll->jointMatrixes[parent]);
			//	}
			//}
			//test[0];
			//int a = 0;

		}
		if (this->m_ragdoll->state == ANIMATED_TRANSITION)
		{
			this->m_oldAnimState = this->m_aComp->previousState;
			SetAnimationComponent(PLAYER_RISE_UP, 0.5f, Blending::NO_TRANSITION, false, true, 2.5f, 1.0f);
			this->m_aComp->previousState = PLAYER_IDLE;
			this->m_ragdoll->state = ANIMATED;
		}
		if (this->m_ragdoll->state == RAGDOLL || this->m_ragdoll->state == KEYFRAMEBLEND)
		{
			this->m_aComp;
			this->m_ragdoll->playerPC->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
			
			if (!stateExists(RAGDOLL_STATE))
			{
				this->m_oldAnimState = this->m_aComp->previousState;
				SetAnimationComponent(RAGDOLL_STATE, 0.f, Blending::NO_TRANSITION, false, false, 0.f, 1.0);	
				this->m_aComp->previousState = RAGDOLL_STATE;
				
			}
			for (int i = 0; i < 21; i++)
			{
				this->m_ragdoll->jointMatrixes[i].r[3] = DirectX::XMVectorSetW(this->m_ragdoll->jointMatrixes[i].r[3], 1);

				DirectX::XMMATRIX* inverseBindPose = &static_cast<DirectX::XMMATRIX>(this->m_aComp->skeleton->GetSkeletonData()->joints[i].invBindPose);

				((GraphicsAnimationComponent*)this->m_gComp)->finalJointTransforms[i] = DirectX::XMMatrixMultiply(*inverseBindPose, this->m_ragdoll->jointMatrixes[i]);
			}
		}
	}
	if (this->m_ragdoll != nullptr && this->m_ragdoll->state == ANIMATED)
	{
	if (m_chainSoundTimer < 1)
		this->m_chainSoundTimer += dT;

	/*Run forward.*/
	if (inputHandler->IsKeyDown(SDL_SCANCODE_W))
	{
		forwards++;

		if (this->m_aComp->lockAnimation != true)
		{
			/*If the player is currently walking, dont update any information from here.*/
			if (m_grabbed != nullptr)
			{
				if (!stateExists(PLAYER_RUN_FORWARD_BALL))
				{
					this->m_oldAnimState = this->m_aComp->previousState;
					SetAnimationComponent(PLAYER_RUN_FORWARD_BALL, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.7f, this->m_aComp->velocity);
					this->m_aComp->previousState = PLAYER_RUN_FORWARD_BALL;
				}
			}
			else
			{
				if (!stateExists(PLAYER_RUN_FORWARD))
				{
					this->m_oldAnimState = this->m_aComp->previousState;
					SetAnimationComponent(PLAYER_RUN_FORWARD, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.7f, this->m_aComp->velocity);
					this->m_aComp->previousState = PLAYER_RUN_FORWARD;
				}
			}
		}
	}
	/*Run backwards.*/
	if (inputHandler->IsKeyDown(SDL_SCANCODE_S))
	{
		forwards--;

		if (this->m_aComp->lockAnimation != true)
		{
			if (m_grabbed != nullptr)
			{
				if (!stateExists(PLAYER_RUN_BACKWARD_BALL))
				{
					this->m_oldAnimState = this->m_aComp->previousState;
					SetAnimationComponent(PLAYER_RUN_BACKWARD_BALL, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.45f, this->m_aComp->velocity);
					this->m_aComp->previousState = PLAYER_RUN_BACKWARD_BALL;
				}
			}
			else
			{
				if (!stateExists(PLAYER_RUN_BACKWARD))
				{
					this->m_oldAnimState = this->m_aComp->previousState;
					SetAnimationComponent(PLAYER_RUN_BACKWARD, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.45f, this->m_aComp->velocity);
					this->m_aComp->previousState = PLAYER_RUN_BACKWARD;
				}
			}
		}
	}
	/*Strafe run to the right.*/
	if (inputHandler->IsKeyDown(SDL_SCANCODE_D))
	{
		sideways++;

		if (this->m_aComp->lockAnimation != true)
		{
			if (!inputHandler->IsKeyDown(SDL_SCANCODE_W) && inputHandler->IsKeyDown(SDL_SCANCODE_D) & 
				!inputHandler->IsKeyDown(SDL_SCANCODE_S) && inputHandler->IsKeyDown(SDL_SCANCODE_D))
			{
				if (m_grabbed != nullptr)
				{
					if (!stateExists(PLAYER_RUN_RIGHT_BALL))
					{
						this->m_oldAnimState = this->m_aComp->previousState;
						SetAnimationComponent(PLAYER_RUN_RIGHT_BALL, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.5f, this->m_aComp->velocity);
						this->m_aComp->previousState = PLAYER_RUN_RIGHT_BALL;
					}
				}
				else
				{
					if (!stateExists(PLAYER_RUN_RIGHT))
					{
						this->m_oldAnimState = this->m_aComp->previousState;
						SetAnimationComponent(PLAYER_RUN_RIGHT, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.5f, this->m_aComp->velocity);
						this->m_aComp->previousState = PLAYER_RUN_RIGHT;
					}
				}
			}
		}
	}
	/*Strafe run to the left.*/
	if (inputHandler->IsKeyDown(SDL_SCANCODE_A))
	{
		sideways--;

		if (!inputHandler->IsKeyDown(SDL_SCANCODE_W) && inputHandler->IsKeyDown(SDL_SCANCODE_A) &
			!inputHandler->IsKeyDown(SDL_SCANCODE_S) && inputHandler->IsKeyDown(SDL_SCANCODE_A))
		{
			if (this->m_aComp->lockAnimation != true)
			{
				if (m_grabbed != nullptr)
				{
					if (!stateExists(PLAYER_RUN_LEFT_BALL))
					{
						this->m_oldAnimState = this->m_aComp->previousState;
						SetAnimationComponent(PLAYER_RUN_LEFT_BALL, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.5f, this->m_aComp->velocity);
						this->m_aComp->previousState = PLAYER_RUN_LEFT_BALL;
					}
				}
				else
				{
					if (!stateExists(PLAYER_RUN_LEFT))
					{
						this->m_oldAnimState = this->m_aComp->previousState;
						SetAnimationComponent(PLAYER_RUN_LEFT, 0.40f, Blending::SMOOTH_TRANSITION, true, false, 0.5f, this->m_aComp->velocity);
						this->m_aComp->previousState = PLAYER_RUN_LEFT;
					}
				}
			}
		}
	}

	/*Releasing the any button for running with player will blend back to idle state with ball or not.*/
	if (inputHandler->IsKeyReleased(SDL_SCANCODE_W) || inputHandler->IsKeyReleased(SDL_SCANCODE_D)
		|| inputHandler->IsKeyReleased(SDL_SCANCODE_S) || inputHandler->IsKeyReleased(SDL_SCANCODE_A))
	{
		if (m_aComp->lockAnimation != true)
		{
			if (m_grabbed != nullptr)
			{
				this->m_oldAnimState = this->m_aComp->previousState;
						SetAnimationComponent(PLAYER_BALL_IDLE, 0.50f, Blending::SMOOTH_TRANSITION, true, false, 0.8f, 1.0f);
				this->m_aComp->previousState = PLAYER_BALL_IDLE;
			}
			else
			{
				this->m_oldAnimState = this->m_aComp->previousState;
						SetAnimationComponent(PLAYER_IDLE, 0.50f, Blending::SMOOTH_TRANSITION, true, false, 0.8f, 1.0f);
				this->m_aComp->previousState = PLAYER_IDLE;
			}
		}
	}

	if (this->m_grabbed != nullptr)
	{

		//Set the ball to be between the two hands
		
		//left hand index  : 8
		//right hand index : 12

		//Get left hand, multiply it by bind pose to correct position
		DirectX::XMMATRIX joint = ((GraphicsAnimationComponent*)this->GetGraphicComponent())->finalJointTransforms[8];
		DirectX::XMMATRIX tpose = DirectX::XMMATRIX(this->GetAnimationComponent()->skeleton->GetSkeletonData()->joints[8].invBindPose);
				DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(tpose);
		tpose = DirectX::XMMatrixInverse(&det, tpose);
		joint = DirectX::XMMatrixMultiply(tpose, joint);

		//Get right hand, multiply it by its bind pose to correct the position
		DirectX::XMMATRIX jointTwo = ((GraphicsAnimationComponent*)this->GetGraphicComponent())->finalJointTransforms[12];
		DirectX::XMMATRIX tposeTwo = DirectX::XMMATRIX(this->GetAnimationComponent()->skeleton->GetSkeletonData()->joints[12].invBindPose);
				det = DirectX::XMMatrixDeterminant(tposeTwo);
		tposeTwo = DirectX::XMMatrixInverse(&det, tposeTwo);
		jointTwo = DirectX::XMMatrixMultiply(tposeTwo, jointTwo);

		//Get world matrix of the character
		DirectX::XMMATRIX world = this->GetGraphicComponent()->worldMatrix;
		
		//Multiply the hand joints into world space
				joint = DirectX::XMMatrixMultiply(joint, world);
		jointTwo = DirectX::XMMatrixMultiply(jointTwo, world);

		//Get a vector from left hand to right hand
				DirectX::XMVECTOR jointToJoint = DirectX::XMVectorSubtract(jointTwo.r[3], joint.r[3]);

		//Cut the vector in half
		jointToJoint = DirectX::XMVectorScale(jointToJoint, 0.5);

		PhysicsComponent* ptr = this->m_grabbed->GetPhysicsComponent(); 

		//Final pos = left hand + half jointToJoint vector
		ptr->PC_pos = DirectX::XMVectorAdd(joint.r[3], jointToJoint);


		ptr->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
		ptr->PC_rotationVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
		ptr->PC_gravityInfluence = 1.0;
	}


	//if (inputHandler->IsKeyPressed(SDL_SCANCODE_P))
	bool hasThrown = false;
	if(inputHandler->IsMouseKeyPressed(SDL_BUTTON_LEFT) && this->m_grabbed != nullptr)
	{
		//assumes grabbed is ALWAYS the ball
		if (this->m_grabbed != nullptr)
		{
			//Play sound
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, this->GetPhysicsComponent()->PC_pos);

			/*Playing the corresponding throw sounds based on which entity id the player has, 2 for studley, 1 for abbington*/
			if (this->GetEntityID() == 2)
			SoundHandler::instance().PlayRandomSound3D(Sounds3D::STUDLEY_THROW_1, Sounds3D::STUDLEY_THROW_3, pos, false, false);
			else
				SoundHandler::instance().PlayRandomSound3D(Sounds3D::ABBINGTON_THROWING_1, Sounds3D::ABBINGTON_THROWING_3, pos, false, false);
				
			float strength = 25.0f; //stregth higher than 50 can cause problems pullinh through walls and such

			//if the player is holding its own ball
			if (this->m_ball->GetEntityID() == this->m_grabbed->GetEntityID())
			{
				/*Set the component to play the "release ball" animation for player IDLE.*/
				this->m_oldAnimState = this->m_aComp->previousState;
				SetAnimationComponent(PLAYER_IDLE, 0.50f, Blending::SMOOTH_TRANSITION, true, false, 0.8f, this->m_aComp->velocity);

				this->m_aComp->previousState = PLAYER_IDLE;

				strength = 2; //weak as f��k if the player tries to throw himself
			}

			else
			{
				/*Set the component to play the animation for throwing the other player's ball.*/
				this->m_oldAnimState = this->m_aComp->previousState;
				SetAnimationComponent(PLAYER_THROW, 0.25f, Blending::FROZEN_TRANSITION, false, true, 2.0f, 1.0f);
				this->m_aComp->velocity = 1.0f;
				this->m_aComp->previousState = PLAYER_THROW;
			}

			m_grabbed->GetPhysicsComponent()->PC_active = true;
			this->m_grabbed->GetPhysicsComponent()->PC_velocity = DirectX::XMVectorScale(this->m_lookDir, strength);
			this->m_grabbed->GetPhysicsComponent()->PC_gravityInfluence = 1;
					//this->m_ragdoll->state = RAGDOLL;
			this->SetGrabbed(nullptr);	//Release the entity

			hasThrown = true;
			this->m_timeSinceThrow = 0;
		}
	}

	//Check if we have not thrown something
	if (hasThrown == false)
	{
		this->m_timeSinceThrow += dT;	//Add time to timmer
	}
	}
	//Check if player is grounded

	//Check if the player CAN	 update its physics component
	if (this->m_pComp != nullptr)
	{
		//Check if the player should update its physics component
		//if (this->m_pComp->PC_entityID == 0)
		//{
		if (forwards == 0)
		{
			//this->m_pComp->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
			//this->m_ragdoll->upperBody.center->PC_velocity = DirectX::XMVectorSet(0, 0, 0, 0);
		}
		if (forwards != 0 || sideways != 0)
			{
				//Use those values for the player behaviour calculations
				//Get the rotation around the Y-axis, also called the Yaw axis
				//float yaw = DirectX::XMVectorGetY(this->m_pComp->PC_rotation);

				//Define a quaternion rotation so we can rotate the velocity vector
				//DirectX::XMVECTOR rotation = DirectX::XMVectorSet(0.0f, DirectX::XMScalarASin(yaw / 2.0f), 0.0f, DirectX::XMScalarACos(yaw / 2.0f));
				float forwardsVel = 0.0f, sidewaysVel = 0.0f;
				//DirectX::XMVECTOR velocity = DirectX::XMVectorSet(m_speed * sideways, 0.0f, m_speed * forwards, 1.0f);
				DirectX::XMVECTOR velocity = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				DirectX::XMVECTOR lookAtDir = this->m_lookDir;
				lookAtDir.m128_f32[1] = 0.0f;
				lookAtDir = DirectX::XMVector3Normalize(lookAtDir);
				//Scale lookDir with forwards
				velocity = DirectX::XMVectorAdd(velocity, DirectX::XMVectorScale(lookAtDir, float(forwards)));

				this->m_rightDir.m128_f32[1] = 0.0f;
				lookAtDir = this->m_rightDir;
				lookAtDir.m128_f32[1] = 0.0f;
				lookAtDir = DirectX::XMVector3Normalize(lookAtDir);
				//Scale lookAtDir / m_rightDir with sideways
				velocity = DirectX::XMVectorAdd(velocity, DirectX::XMVectorScale(lookAtDir, float(sideways)));

				//Velocity now contains both forwards and sideways velocity
				velocity = DirectX::XMVector3Normalize(velocity);

				/*When the player move backwards, the movement speed of the character should be slower with a down-scale factor.*/
				if (this->m_aComp->source_State->stateIndex == PLAYER_RUN_BACKWARD)
				{
					float scaleFactor = 0.80f;
					velocity = DirectX::XMVectorScale(velocity, this->m_acceleration * scaleFactor);
					velocity = DirectX::XMVectorSetW(velocity, 1.0f);
				}
				/*When the player move backwards with ball, the movement speed of the character should be slower with a down-scale factor.*/
				else if (this->m_aComp->source_State->stateIndex == PLAYER_RUN_BACKWARD_BALL)
				{
					float scaleFactor = 0.60f;
					velocity = DirectX::XMVectorScale(velocity, this->m_acceleration * scaleFactor);
					velocity = DirectX::XMVectorSetW(velocity, 1.0f);
				}
				/*When the player run forwards and strafe left to right, scale with the normal acceleration.*/
				else
				{
					//Scale that velocity with speed and deltaTime
					velocity = DirectX::XMVectorScale(velocity, this->m_acceleration);
					velocity = DirectX::XMVectorSetW(velocity, 1.0f);
				}

				//Add the velocity to our physicsComponent
				float ySpeed = 0;
				ySpeed = DirectX::XMVectorGetY(this->m_pComp->PC_velocity);
				//ySpeed += DirectX::XMVectorGetY(velocity);

				this->m_pComp->PC_velocity = velocity;
				this->m_pComp->PC_velocity = DirectX::XMVectorSetY(this->m_pComp->PC_velocity, ySpeed);
				this->m_ragdoll->upperBody.center->PC_velocity = this->m_pComp->PC_velocity;
				
				/*Store the velocity of the player to use as a scale factor for animation playing speed.*/
				DirectX::XMVECTOR velocityAnimation = DirectX::XMVector3Length(this->m_pComp->PC_velocity);
				this->m_aComp->velocity = DirectX::XMVectorGetX(velocityAnimation);
				
				//Rotates the player to run in the direction that the camera faces. 
				DirectX::XMVECTOR walkDir = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(this->m_rightDir, { 0.0f,1.0f,0.0f,0.0f }));
				this->m_pComp->PC_OBB.ort = DirectX::XMMatrixSet(
					-this->m_rightDir.m128_f32[0], -this->m_rightDir.m128_f32[1], -this->m_rightDir.m128_f32[2], 0.0f,
					0.f, 1.f, 0.f, 0.0f,
					-walkDir.m128_f32[0], -walkDir.m128_f32[1], -walkDir.m128_f32[2], 0.0f,		
					0, 0, 0, 1.0f
				);
				//If the player is grabbing the ball, the ball entity should also rotate in the direction of the camera. 
				if (this->m_grabbed != nullptr)
				{
					PhysicsComponent* physicComp = this->m_grabbed->GetPhysicsComponent();

					physicComp->PC_OBB.ort = DirectX::XMMatrixSet(
						-this->m_rightDir.m128_f32[0], -this->m_rightDir.m128_f32[1], -this->m_rightDir.m128_f32[2], 0.0f,
						0.f, 1.f, 0.f, 0.0f,
						-walkDir.m128_f32[0], -walkDir.m128_f32[1], -walkDir.m128_f32[2], 0.0f,
						0, 0, 0, 1.0f
					);
				}
				//Play walking sounds
				/*Playing the corresponding walk sounds based on which entity id the player has, 2 for studley, 1 for abbington*/
				if (this->m_walkingSound == nullptr)	//Check if we have a walking sound
				{
					DirectX::XMFLOAT3 pos;
					DirectX::XMStoreFloat3(&pos, this->GetPhysicsComponent()->PC_pos);
					if (this->GetEntityID() == 2)
					this->m_walkingSound = SoundHandler::instance().PlaySound3D(Sounds3D::STUDLEY_WALK, pos, true, true);
					else
						this->m_walkingSound = SoundHandler::instance().PlaySound3D(Sounds3D::ABBINGTON_WALK, pos, true, true);
					pos.y = pos.y - 1;
					SoundHandler::instance().PlayRandomSound3D(Sounds3D::GENERAL_CHAIN_DRAG_1, Sounds3D::GENERAL_CHAIN_DRAG_3, pos, false, false);
				}
				else
				{
					if (this->m_walkingSound->getIsPaused())	//If the walking sound is paused
					{
						this->m_walkingSound->setPlayPosition(0);
						this->m_walkingSound->setIsPaused(false);	//Un pause it
						if (this->m_chainSoundTimer > 0.37f)
						{
							DirectX::XMFLOAT3 pos;
							DirectX::XMStoreFloat3(&pos, this->GetPhysicsComponent()->PC_pos);
							pos.y = pos.y - 1;
							SoundHandler::instance().PlayRandomSound3D(Sounds3D::GENERAL_CHAIN_DRAG_1, Sounds3D::GENERAL_CHAIN_DRAG_3, pos, false, false);
							this->m_chainSoundTimer = 0.0f;
						}
					}
				}
			}
			else
			{
				if (this->m_walkingSound != nullptr && !this->m_walkingSound->getIsPaused())
				{
					this->m_walkingSound->setIsPaused(true);	//Pause the walking sound
				}
				if (m_isAiming)
				{
					DirectX::XMVECTOR walkDir = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(this->m_rightDir, { 0.0,1.0,0.0,0.0 }));
					this->m_pComp->PC_OBB.ort = DirectX::XMMatrixSet(
						-this->m_rightDir.m128_f32[0], -this->m_rightDir.m128_f32[1], -this->m_rightDir.m128_f32[2], 0.0f,
						0.f, 1.f, 0.f, 0.0f,
						-walkDir.m128_f32[0], -walkDir.m128_f32[1], -walkDir.m128_f32[2], 0.0f,
						0, 0, 0, 1.0f
					);
				}
			}


		//Update sound pos
		if (this->m_walkingSound != nullptr)
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, this->GetPhysicsComponent()->PC_pos);
			irrklang::vec3df iv(pos.x, pos.y, pos.z);
			this->m_walkingSound->setPosition(iv);
		}


		//}
		if (this->m_gComp != nullptr)
		{
			this->UnsafeSyncComponents();
			if (this->m_ragdoll->state == RAGDOLL )
			{
				//offsets for when player ragdoll is in RAGDOLL stage
				DirectX::XMVECTOR offSet = this->m_ragdoll->playerPC->PC_OBB.ort.r[1];
				offSet = DirectX::XMVectorScale(offSet, this->m_ragdoll->playerPC->PC_OBB.ext[1] * -1);
				this->m_gComp->worldMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorAdd(this->m_ragdoll->upperBody.center->PC_pos, offSet));
			}
			if (this->m_ragdoll->state == KEYFRAMEBLEND)
			{
				//offsets for when player ragdoll is in KEYFRAMEBLEND stage
				DirectX::XMVECTOR offSet = this->m_ragdoll->playerPC->PC_OBB.ort.r[1];
				offSet = DirectX::XMVectorScale(offSet, this->m_ragdoll->playerPC->PC_OBB.ext[1] * -0.0);
				offSet = DirectX::XMVectorAdd(offSet, DirectX::XMVectorSet(0, 0, -1.5, 0));
				this->m_gComp->worldMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorAdd(this->m_ragdoll->lowerBody.center->PC_pos, offSet));

			}

		}
	}

	//End the update
	return result;
}

int Player::React(int entityID, EVENT reactEvent)
{
	int result = 0;

	return result;
}

Entity* Player::SetGrabbed(Entity * entityPtr)
{
	Entity* oldValue = nullptr;
	
	if (entityPtr != this->m_grabbed)	//Cant be the same entity that we are currently holding
	{
		oldValue = this->m_grabbed;
		this->m_grabbed = entityPtr;
		
		if (this->m_grabbed != nullptr)	//If we grab something that is not a nullptr
		{
			m_grabbed->GetPhysicsComponent()->PC_active = false;
			this->m_grabbed->SetGrabbed(this);	//Set the new entity to be grabbed by this entity	
		}
		if (oldValue != nullptr)	//If we drop something
		{
			oldValue->SetGrabbed(nullptr);	//Set the old entity to NOT grabbed
		}
	}
	return oldValue;	//Returns nullptr if nothing is droped for the new entity
}

float Player::SetMaxSpeed(float speed)
{
	float oldSpeed = this->m_maxSpeed;
	this->m_maxSpeed = speed;
	return oldSpeed;
}

float Player::SetAcceleration(float acceleration)
{
	float oldAcceleration = this->m_acceleration;
	this->m_acceleration = acceleration;
	return oldAcceleration;
}


DirectX::XMVECTOR Player::SetLookDir(DirectX::XMVECTOR lookDir)
{
	DirectX::XMVECTOR oldValue = this->m_lookDir;
	this->m_lookDir = lookDir;
	return oldValue;
}

DirectX::XMVECTOR Player::SetUpDir(DirectX::XMVECTOR upDir)
{
	DirectX::XMVECTOR oldValue = this->m_upDir;
	this->m_upDir = upDir;
	return oldValue;
}

DirectX::XMVECTOR Player::SetRightDir(DirectX::XMVECTOR rightDir)
{
	DirectX::XMVECTOR oldValue = this->m_rightDir;
	this->m_rightDir = rightDir;
	return oldValue;
}

void Player::SetAiming(bool isAming)
{
	this->m_isAiming = isAming;
}

bool Player::stateExists(int animationState)
{
	/*If the previous state is not equal to current state.*/
	if (m_aComp->previousState != animationState)
	{
		//m_aComp->previousState = animationState;
		return false;
	}
	/*If the previous state is equal to current state.*/
	else
	{
		return true;
	}
}

void Player::SetAnimationComponent(int animationState, float transitionDuration, Blending blendingType, bool isLooping, bool lockAnimation, float playingSpeed, float velocity)
{
	if (animationState != RAGDOLL_STATE)
	{
		if (this->m_ragdoll->state == ANIMATED_TRANSITION)
		{
			this->m_aComp->target_State = this->m_aComp->animation_States->at(animationState)->GetAnimationStateData();
			this->m_aComp->source_State = this->m_aComp->animation_States->at(animationState)->GetAnimationStateData();
			this->m_aComp->source_State->stateIndex = animationState;
			this->m_aComp->blendFlag = blendingType;
			this->m_aComp->source_State->isLooping = isLooping;
			this->m_aComp->lockAnimation = lockAnimation;
			this->m_aComp->playingSpeed = playingSpeed;
			this->m_aComp->velocity = velocity;
		}

		else
		{
			this->m_aComp->target_State = this->m_aComp->animation_States->at(animationState)->GetAnimationStateData();
			this->m_aComp->target_State->stateIndex = animationState;
			this->m_aComp->transitionDuration = transitionDuration;
			this->m_aComp->blendFlag = blendingType;
			this->m_aComp->target_State->isLooping = isLooping;
			this->m_aComp->lockAnimation = lockAnimation;
			this->m_aComp->playingSpeed = playingSpeed;
			this->m_aComp->velocity = velocity;
		}
	}

	else
	{
		this->m_aComp->source_State->stateIndex = animationState;
		this->m_aComp->blendFlag = blendingType;
		this->m_aComp->lockAnimation = lockAnimation;
		this->m_aComp->playingSpeed = playingSpeed;
		this->m_aComp->target_State = nullptr;
		this->m_aComp->target_Time = 0.f;
		this->m_aComp->source_Time = 0.f;
	}
}

void Player::SetBall(Entity * ball)
{
	this->m_ball = ball;
}

void Player::SetRagdoll(Ragdoll * ragdoll)
{
	this->m_ragdoll = ragdoll;
}

void Player::SetOldAnimState(int newOldState)
{
	this->m_oldAnimState = newOldState;
}

float Player::GetMaxSpeed()
{
	return this->m_maxSpeed;
}

float Player::GetAcceleration()
{
	return this->m_acceleration;
}

DirectX::XMVECTOR Player::GetLookDir()
{
	return this->m_lookDir;
}

DirectX::XMVECTOR Player::GetUpDir()
{
	return this->m_upDir;
}

DirectX::XMVECTOR Player::GetRightDir()
{
	return this->m_rightDir;
}

bool Player::GetIsAming()
{
	return this->m_isAiming;
}

Entity * Player::GetGrabbed()
{
	return this->m_grabbed;
}

Entity * Player::GetBall()
{
	return this->m_ball;
}

Ragdoll * Player::GetRagdoll()
{
	return this->m_ragdoll;
}

bool Player::isAnimationChanged()
{
	bool result = false;

	if (this->m_oldAnimState != this->m_aComp->previousState)
	{
		this->m_oldAnimState = this->m_aComp->previousState;
		result = true;
	}

	return result;
}

float Player::TimeSinceThrow()
{
	return this->m_timeSinceThrow;
}
