#include "LevelState.h"

inline OBB m_ConvertOBB(BoundingBoxHeader & boundingBox) //Convert from BBheader to OBB struct										
{														 
	OBB obj;

	obj.ext[0] = boundingBox.extension[0];
	obj.ext[1] = boundingBox.extension[1];
	obj.ext[2] = boundingBox.extension[2];


	DirectX::XMMATRIX extensionMatrix;
	extensionMatrix = DirectX::XMMatrixSet(
		boundingBox.extensionDir[0].x, boundingBox.extensionDir[0].y, boundingBox.extensionDir[0].z, 0.0f,
		boundingBox.extensionDir[1].x, boundingBox.extensionDir[1].y, boundingBox.extensionDir[1].z, 0.0f,
		boundingBox.extensionDir[2].x, boundingBox.extensionDir[2].y, boundingBox.extensionDir[2].z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	obj.ort = extensionMatrix;


	return obj;
}


LevelState::LevelState()
{
}


LevelState::~LevelState()
{
	ShutDown();
}

int LevelState::ShutDown()
{

	int result = 1;
	// Clear the dynamic entities
	for (size_t i = 0; i < this->m_dynamicEntitys.size(); i++)
	{
		delete this->m_dynamicEntitys[i];
		this->m_dynamicEntitys[i] = nullptr;
	}
	
	// Clear the static entities
	for (size_t i = 0; i < this->m_staticEntitys.size(); i++)
	{
		delete this->m_staticEntitys[i];
		this->m_staticEntitys[i] = nullptr;
	}

	//Clear the puzzle entities
	for (size_t i = 0; i < this->m_doorEntities.size(); i++)
	{
		delete this->m_doorEntities[i];
		this->m_doorEntities[i] = nullptr;
	}
	this->m_doorEntities.clear();
	for (size_t i = 0; i < this->m_buttonEntities.size(); i++)
	{
		delete this->m_buttonEntities[i];
		this->m_buttonEntities[i] = nullptr;
	}
	this->m_buttonEntities.clear();
	for (size_t i = 0; i < this->m_wheelEntities.size(); i++)
	{
		delete this->m_wheelEntities[i];
		this->m_wheelEntities[i] = nullptr;
	}
	this->m_wheelEntities.clear();
	// Clear level director
	this->m_director.Shutdown();
	
	return result;
}

int LevelState::Initialize(GameStateHandler * gsh, ComponentHandler* cHandler, Camera* cameraRef)
{
	int result = 1;
	result = GameState::InitializeBase(gsh, cHandler, cameraRef);

	Resources::ResourceHandler* resHandler = Resources::ResourceHandler::GetInstance();


	// creating the player
	this->m_player1 = Player();
	GraphicsComponent* playerG = m_cHandler->GetGraphicsAnimationComponent();
	playerG->modelID = 2759249725;
	playerG->active = true;
	resHandler->GetModel(playerG->modelID, playerG->modelPtr);
	PhysicsComponent* playerP = m_cHandler->GetPhysicsComponent();
	playerP->PC_entityID = 0;								//Set Entity ID
	playerP->PC_pos = DirectX::XMVectorSet(20, 20, 20, 0);		//Set Position
	playerP->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);//Set Rotation
	playerP->PC_is_Static = TRUE;							//Set IsStatic
	playerP->PC_active = true;								//Set Active
	playerP->PC_mass = 5;
	playerP->PC_BVtype = BV_AABB;
	playerP->PC_OBB.ext[0] = 0.5f;
	playerP->PC_OBB.ext[1] = 0.5f;
	playerP->PC_OBB.ext[2] = 0.5f;
	playerP->PC_AABB.ext[0] = 0.5f;
	playerP->PC_AABB.ext[1] = 0.5f;
	playerP->PC_AABB.ext[2] = 0.5f;
	playerG->worldMatrix = DirectX::XMMatrixIdentity();		//FIX THIS
	
	/*TEMP ANIM STUFF*/
	((GraphicsAnimationComponent*)playerG)->jointCount = playerG->modelPtr->GetSkeleton()->GetSkeletonData()->jointCount;
	
	AnimationComponent* playerAnim1 = nullptr;
	playerAnim1 = m_cHandler->GetAnimationComponent();
	playerAnim1->skeleton = playerG->modelPtr->GetSkeleton();
	playerAnim1->active = 1;
	for (int i = 0; i < ((GraphicsAnimationComponent*)playerG)->jointCount; i++)
	{
		((GraphicsAnimationComponent*)playerG)->finalJointTransforms[i] = DirectX::XMMatrixIdentity();
	}

	if (playerG->modelPtr->GetSkeleton()->GetNumAnimations() > 0)
	{
		int numAnimations = playerG->modelPtr->GetSkeleton()->GetNumAnimations();

		playerAnim1->animation_States = playerG->modelPtr->GetSkeleton()->GetAllAnimations();

		playerAnim1->source_State = playerAnim1->animation_States->at(0)->GetAnimationStateData();
		playerAnim1->source_State->isLooping = true; // TEMP TEST
	}

	this->m_player1.Initialize(0, playerP, playerG, playerAnim1);

	//Player 2
	this->m_player2 = Player();
	playerG = m_cHandler->GetGraphicsAnimationComponent();
	playerG->modelID = 2759249725;
	playerG->active = true;
	resHandler->GetModel(playerG->modelID, playerG->modelPtr);
	playerP = m_cHandler->GetPhysicsComponent();
	playerP->PC_entityID = 1;								//Set Entity ID
															//playerP->PC_pos = DirectX::XMVectorSet(0, -100, 0, 0);		//Set Position
	playerP->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);//Set Rotation
	playerP->PC_is_Static = false;							//Set IsStatic
	playerP->PC_active = true;								//Set Active
	playerP->PC_mass = 1;
	playerP->PC_BVtype = BV_AABB;
	playerP->PC_OBB.ext[0] = 0.5f;
	playerP->PC_OBB.ext[1] = 0.5f;
	playerP->PC_OBB.ext[2] = 0.5f;
	playerP->PC_AABB.ext[0] = 0.5f;
	playerP->PC_AABB.ext[1] = 0.5f;
	playerP->PC_AABB.ext[2] = 0.5f;
	playerG->worldMatrix = DirectX::XMMatrixIdentity();		//FIX THIS
	this->m_player2.Initialize(3, playerP, playerG, playerAnim1);
															/*TEMP ANIM STUFF*/
	((GraphicsAnimationComponent*)playerG)->jointCount = playerG->modelPtr->GetSkeleton()->GetSkeletonData()->jointCount;
	for (int i = 0; i < ((GraphicsAnimationComponent*)playerG)->jointCount; i++)
	{
		((GraphicsAnimationComponent*)playerG)->finalJointTransforms[i] = DirectX::XMMatrixIdentity();
	}

	AnimationComponent* playerAnim2 = nullptr;

	if (playerG->modelPtr->GetSkeleton()->GetNumAnimations() > 0)
	{
		//playerAnim2 = m_cHandler->GetAnimationComponent();

		//int numAnimations = playerG->modelPtr->GetSkeleton()->GetNumAnimations();
		//for (int i = 0; i < numAnimations; i++)
		//{
		//	Resources::Animation* animation = playerG->modelPtr->GetSkeleton()->GetAnimation(i);
		//	int jointIndex = 0;
		//	const Resources::Animation::AnimationJoint* animationJoint = animation->GetJoint(jointIndex);
		//	playerAnim2->Anim_StateData[i].startFrame = animationJoint->keyframes[0].timeValue;
		//	int keyCount = animationJoint->keyframeCount;
		//	playerAnim2->Anim_StateData[i].endFrame = animationJoint->keyframes[keyCount - 1].timeValue;
		//	playerAnim2->Anim_StateData[i].localTime = 0;
		//	playerAnim2->Anim_StateData[i].isLooping = true;
		//	playerAnim2->Anim_StateData[i].animationState = i;
		//}
	}

	this->m_player2.Initialize(3, playerP, playerG, playerAnim2);
	
	//this->m_dynamicEntitys.push_back();
	
	//Ball1
	DynamicEntity* ball = new DynamicEntity();
	GraphicsComponent* ballG = m_cHandler->GetGraphicsComponent();
	ballG->modelID = 1337;
	ballG->active = true;
	resHandler->GetModel(ballG->modelID, ballG->modelPtr);
	PhysicsComponent* ballP = m_cHandler->GetPhysicsComponent();
	ballP->PC_entityID = 2;									//Set Entity ID
	ballP->PC_pos = DirectX::XMVectorSet(-6, 0, -10, 0);		//Set Position
	ballP->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);	//Set Rotation
	ballP->PC_is_Static = false;							//Set IsStatic
	ballP->PC_active = true;								//Set Active
	ballP->PC_BVtype = BV_AABB;
	ballP->PC_AABB.ext[0] = 0.25;
	ballP->PC_AABB.ext[1] = 0.25;
	ballP->PC_AABB.ext[2] = 0.25;
	ballP->PC_mass = 2;
	ballG->worldMatrix = DirectX::XMMatrixIdentity();
	ball->Initialize(2, ballP, ballG);
	this->m_dynamicEntitys.push_back(ball);

	//Ball2
	DynamicEntity* ball2 = new DynamicEntity();
	ballG = m_cHandler->GetGraphicsComponent();
	ballG->modelID = 1337;
	ballG->active = true;
	resHandler->GetModel(ballG->modelID, ballG->modelPtr);
	ballP = m_cHandler->GetPhysicsComponent();
	ballP->PC_entityID = 3;									//Set Entity ID
	ballP->PC_pos = DirectX::XMVectorSet(-3, 0, -10, 0);		//Set Position
	ballP->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);	//Set Rotation
	ballP->PC_is_Static = false;							//Set IsStatic
	ballP->PC_active = true;								//Set Active
	ballP->PC_BVtype = BV_AABB;
	ballP->PC_AABB.ext[0] = 0.5;
	ballP->PC_AABB.ext[1] = 0.5;
	ballP->PC_AABB.ext[2] = 0.5;
	ballP->PC_mass = 2;
	ballG->worldMatrix = DirectX::XMMatrixIdentity();
	ball2->Initialize(3, ballP, ballG);
	this->m_dynamicEntitys.push_back(ball2);

	//Entity* ptr = (Entity*)ball;
	//this->m_player1.SetGrabbed(ball);

	this->m_cHandler->GetPhysicsHandler()->CreateChainLink(2, 1, 5, 0.5);

	StaticEntity* roof = new StaticEntity;
	PhysicsComponent* roofP = m_cHandler->GetPhysicsComponent();
	roofP->PC_pos = DirectX::XMVectorSet(0, 0, 0, 0);
	roofP->PC_is_Static = true;
	roofP->PC_BVtype = BV_Plane;
	roofP->PC_Plane.PC_normal = DirectX::XMVectorSet(0,-1, 0, 0);
	roofP->PC_elasticity = 0;
	roofP->PC_friction = 1.0f;

	this->m_staticEntitys.push_back(roof);

	DynamicEntity* platform = new DynamicEntity();
	GraphicsComponent* platformG = m_cHandler->GetGraphicsComponent();
	platformG->modelID = 1337;
	platformG->active = true;
	resHandler->GetModel(platformG->modelID, platformG->modelPtr);
	PhysicsComponent* platformP = m_cHandler->GetPhysicsComponent();
	platformP->PC_pos = DirectX::XMVectorSet(-3, 1, -40, 0);
	platformP->PC_is_Static = true;
	platformP->PC_AABB.ext[0] = 5;
	platformP->PC_AABB.ext[1] = 0.1f;
	platformP->PC_AABB.ext[2] = 5;
	platformP->PC_elasticity = 0;
	platformP->PC_friction = 1.0f;
	platformG->worldMatrix = DirectX::XMMatrixTranslationFromVector(platformP->PC_pos);
	AIComponent* platformTERMINATOR = m_cHandler->GetAIComponent();
#pragma region AIComp variables
	platformTERMINATOR->AC_triggered = true;
	platformTERMINATOR->AC_speed = 0.15f;
	platformTERMINATOR->AC_position = platformP->PC_pos;
	platformTERMINATOR->AC_pattern = AI_CIRCULAR;
	platformTERMINATOR->AC_nrOfWaypoint = 4;
	platformTERMINATOR->AC_waypoints[0] = platformP->PC_pos;
	platformTERMINATOR->AC_waypoints[1] = DirectX::XMVectorSet(-3, 1, 0, 0);
	platformTERMINATOR->AC_waypoints[2] = DirectX::XMVectorSet(-3, 15, 0, 0);
	platformTERMINATOR->AC_waypoints[3] = DirectX::XMVectorSet(-3, 15, -40, 0);
#pragma endregion
	platform->Initialize(4, platformP, platformG, nullptr, platformTERMINATOR);
	platform->Initialize(3, platformP, platformG, nullptr, platformTERMINATOR);
	platformP->PC_entityID = platform->GetEntityID();
	platformTERMINATOR->AC_entityID = platform->GetEntityID();
	this->m_dynamicEntitys.push_back(platform);

	DynamicEntity* plat = new DynamicEntity();
	GraphicsComponent* platG = m_cHandler->GetGraphicsComponent();
	platG->modelID = 1337;
	platG->active = true;
	resHandler->GetModel(platG->modelID, platG->modelPtr);
	PhysicsComponent* platP = m_cHandler->GetPhysicsComponent();
	platP->PC_pos = DirectX::XMVectorSet(-3, 7, 40, 0);
	platP->PC_is_Static = true;
	platP->PC_AABB.ext[0] = 5;
	platP->PC_AABB.ext[1] = 0.1f;
	platP->PC_AABB.ext[2] = 5;
	platP->PC_elasticity = 0;
	platP->PC_friction = 1.0f;
	platG->worldMatrix = DirectX::XMMatrixTranslationFromVector(platP->PC_pos);
	AIComponent* platA = m_cHandler->GetAIComponent();
#pragma region AIComp variables
	platA->AC_triggered = true;
	platA->AC_speed = 0.15f;
	platA->AC_position = platP->PC_pos;
	platA->AC_pattern = AI_ROUNDTRIP;
	platA->AC_nrOfWaypoint = 4;
	platA->AC_waypoints[0] = platP->PC_pos;
	platA->AC_waypoints[1] = DirectX::XMVectorSet(-3, 7, 0, 0);
	platA->AC_waypoints[2] = DirectX::XMVectorSet(-3, 18, 0, 0);
	platA->AC_waypoints[3] = DirectX::XMVectorSet(-3, 18, 40, 0);
#pragma endregion
	plat->Initialize(5, platP, platG, nullptr, platA);
	platP->PC_entityID = plat->GetEntityID();
	platA->AC_entityID = plat->GetEntityID();
	this->m_dynamicEntitys.push_back(plat);

	//this->m_cameraRef->SetCameraPivot(this->m_player1.GetPhysicsComponent()->PC_pos, 10);
	DirectX::XMVECTOR targetOffset = DirectX::XMVectorSet(0.0, 1.4, 0.0, 0.0);
	
	if (this->m_networkModule->IsHost())
	{
		m_cameraRef->SetCameraPivot(
		&this->m_cHandler->GetPhysicsHandler()->GetDynamicComponentAt(0)->PC_pos,
		targetOffset,
		1.3f
		);
	}
	else // Player 2
	{
		m_cameraRef->SetCameraPivot(
			&this->m_cHandler->GetPhysicsHandler()->GetDynamicComponentAt(1)->PC_pos,
			targetOffset,
			1.3f
		);
	}

	
	this->m_director.Initialize();

	WheelEntity* wheel1 = new WheelEntity();
	DoorEntity* door1 = new DoorEntity();

	//DOOR
	GraphicsComponent* door1G = m_cHandler->GetGraphicsComponent();
	door1G->modelID = 1337;
	door1G->active = true;
	door1G->worldMatrix = DirectX::XMMatrixIdentity();		
	resHandler->GetModel(door1G->modelID, door1G->modelPtr);
	PhysicsComponent* door1P = m_cHandler->GetPhysicsComponent();
	door1P->PC_entityID = 666;								//Set Entity ID
	door1P->PC_pos = DirectX::XMVectorSet(-14.0f, -10.0f, -14.0f, 0.0f);		//Set Position
	door1P->PC_rotation = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);		//Set Rotation
	door1P->PC_is_Static = true;							//Set IsStatic
	door1P->PC_active = true;								//Set Active
	door1P->PC_mass = 5.0f;
	door1P->PC_BVtype = BV_AABB;
	door1P->PC_AABB.ext[0] = 0.5f;
	door1P->PC_AABB.ext[1] = 0.5f;
	door1P->PC_AABB.ext[2] = 0.5f;
	door1->Initialize(666, door1P, door1G, 0.2f);

	//BUTTON
	GraphicsComponent* wheel1G = m_cHandler->GetGraphicsComponent();
	wheel1G->modelID = 1337;
	wheel1G->active = true;
	wheel1G->worldMatrix = DirectX::XMMatrixIdentity();		
	resHandler->GetModel(wheel1G->modelID, wheel1G->modelPtr);
	PhysicsComponent* wheel1P = m_cHandler->GetPhysicsComponent();
	wheel1P->PC_entityID = 616;									//Set Entity ID
	wheel1P->PC_pos = DirectX::XMVectorSet(-8.0f, -10.0f, -19.0f, 0.0f);		//Set Position
	wheel1P->PC_rotation = DirectX::XMVectorSet(0, 0, 0, 0);	//Set Rotation
	wheel1P->PC_is_Static = false;								//Set IsStatic
	wheel1P->PC_active = true;									//Set Active
	wheel1P->PC_gravityInfluence = 1.0f;
	wheel1P->PC_mass = 5;
	wheel1P->PC_BVtype = BV_AABB;
	wheel1P->PC_OBB.ext[0] = 0.5f;
	wheel1P->PC_OBB.ext[1] = 0.5f;
	wheel1P->PC_OBB.ext[2] = 0.5f;
	wheel1P->PC_AABB.ext[0] = 0.5f;
	wheel1P->PC_AABB.ext[1] = 0.5f;
	wheel1P->PC_AABB.ext[2] = 0.5f;
	wheel1->Initialize(616, wheel1P, wheel1G, 2.0f, 0.0f, 1.0f, 2.0f, true, 0.5f, 1.0f);
	wheel1->AddObserver(door1, door1->GetEntityID());
	this->m_wheelEntities.push_back(wheel1);

	this->m_doorEntities.push_back(door1);

	return result;
}

int LevelState::Update(float dt, InputHandler * inputHandler)
{
	int result = 1;
	dt = dt / 1000000;

	this->m_networkModule->Update();

#pragma region 
	if (this->m_networkModule->GetNrOfConnectedClients() != 0)	//Check so we are connected to a client
	{
		//Check for updates for enteties
		this->m_entityPacketList = this->m_networkModule->PacketBuffer_GetEntityPackets();	//This removes the entity packets from the list in NetworkModule

		if (this->m_entityPacketList.size() > 0)
		{
			
			// Apply each packet to the right entity
			std::list<EntityPacket>::iterator itr;
			PhysicsComponent* pp = nullptr;
			for (itr = this->m_entityPacketList.begin(); itr != this->m_entityPacketList.end(); itr++)
			{
				counter++;
				printf("%d\n %d\n", counter, itr->packet_ID);
				if ((int)itr->entityID == 0)
				{
					pp = this->m_player1.GetPhysicsComponent();

					// Update the component
					pp->PC_pos = DirectX::XMLoadFloat3(&itr->newPos);
					pp->PC_rotation = DirectX::XMLoadFloat3(&itr->newRotation);
					pp->PC_velocity = DirectX::XMLoadFloat3(&itr->newVelocity);

					//printf("Player1");
				}
				else if ((int)itr->entityID == 1)
				{
					pp = this->m_player2.GetPhysicsComponent();

					// Update the component
					pp->PC_pos = DirectX::XMLoadFloat3(&itr->newPos);
					pp->PC_rotation = DirectX::XMLoadFloat3(&itr->newRotation);
					pp->PC_velocity = DirectX::XMLoadFloat3(&itr->newVelocity);
				}
				else if ((int)itr->entityID == 2)
				{
					pp = (*this->m_dynamicEntitys.at(0)).GetPhysicsComponent();

					// Update the component
					pp->PC_pos = DirectX::XMLoadFloat3(&itr->newPos);
					pp->PC_rotation = DirectX::XMLoadFloat3(&itr->newRotation);
					pp->PC_velocity = DirectX::XMLoadFloat3(&itr->newVelocity);
				}
				else
				{
					// Find the entity
					std::vector<DynamicEntity*>::iterator Ditr;
					for (Ditr = this->m_dynamicEntitys.begin(); Ditr != this->m_dynamicEntitys.end(); Ditr++) {

						if (itr->entityID == (*Ditr._Ptr)->GetEntityID()) 
						{
							DynamicEntity* ent = (*Ditr._Ptr);	// The entity identified by the ID sent from the other client
							pp = ent->GetPhysicsComponent();

							// Update the component
							pp->PC_pos = DirectX::XMLoadFloat3(&itr->newPos);
							pp->PC_rotation = DirectX::XMLoadFloat3(&itr->newRotation);
							pp->PC_velocity = DirectX::XMLoadFloat3(&itr->newVelocity);
						}

					}
					
				}
			}
		}
		this->m_entityPacketList.clear();	//Clear the list
	}
#pragma endregion Network_update_entities

	float yaw = inputHandler->GetMouseDelta().x;
	float pitch = inputHandler->GetMouseDelta().y;
	float mouseSens = 0.1f * dt;

	if (inputHandler->GetMouseDelta().y || inputHandler->GetMouseDelta().x)
		this->m_cameraRef->RotateCameraPivot(inputHandler->GetMouseDelta().y * mouseSens, inputHandler->GetMouseDelta().x * mouseSens);

	//update player for throw functionallity
	DirectX::XMVECTOR playerLookDir = DirectX::XMVector4Normalize( DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&this->m_cameraRef->GetLookAt()), DirectX::XMLoadFloat3(&this->m_cameraRef->GetCameraPos())));
	DirectX::XMFLOAT3 temp;

	this->m_cameraRef->GetCameraUp(temp);
	
	DirectX::XMVECTOR upDir = DirectX::XMLoadFloat3(&temp);
	DirectX::XMVECTOR rightDir = m_cameraRef->GetRight(); //DirectX::XMVector3Cross(upDir, playerLookDir);
	
#pragma region
	if(this->m_networkModule->IsHost() == true)
	{ 
		Entity* wasGrabbed = this->m_player1.GetGrabbed();

		//Camera
		this->m_player1.SetRightDir(rightDir);
		this->m_player1.SetUpDir(upDir);
		this->m_player1.SetLookDir(playerLookDir);
		this->m_player1.Update(dt, inputHandler);
		//update all dynamic (moving) entities
		Entity* ent = nullptr;

		if (wasGrabbed != this->m_player1.GetGrabbed())
		{
			wasGrabbed->SyncComponents();

			this->m_networkModule->SendGrabPacket(this->m_player1.GetEntityID(), -1);
		}

		for (int i = 0; i < this->m_dynamicEntitys.size(); i++)
		{
			ent = this->m_dynamicEntitys.at(i);
			if (ent == this->m_player2.GetGrabbed())		//Check if the entity is  grabbed by player2, if it is there will be an update packet for it
			{
				ent->SyncComponents();
			}
			else
			{
				ent->Update(dt, inputHandler);
			}
		}
		//Sync other half of the components
		this->m_player2.SyncComponents();

		if (inputHandler->IsKeyPressed(SDL_SCANCODE_G))
		{
			PhysicsComponent* pp = this->m_player1.GetPhysicsComponent();
			PhysicsComponent* epp = this->m_cHandler->GetClosestPhysicsComponent(pp, 3);	//Get the closest component of 2 meters

			if (epp != nullptr)	//If a component was found
			{
				Entity* ent = nullptr;
				for (int i = 0; i < this->m_dynamicEntitys.size(); i++)
				{

					if (this->m_dynamicEntitys.at(i)->GetEntityID() == epp->PC_entityID)	//If the IDs match
					{
						ent = this->m_dynamicEntitys.at(i);
						break;
					}

				}

				if (!ent->IsGrabbed())
				{
					this->m_player1.SetGrabbed(ent);
					this->m_networkModule->SendGrabPacket(this->m_player1.GetEntityID(), ent->GetGrabbed());	//Send the grabbing ID and the grabbed ID
				}
			}

		}
		if (inputHandler->IsKeyPressed(SDL_SCANCODE_H))
		{
			this->m_player1.SetGrabbed(nullptr);
			this->m_networkModule->SendGrabPacket(this->m_player1.GetEntityID(), -1);
		}

		//Check for grabb requests
#pragma region
		this->m_grabPacketList = this->m_networkModule->PacketBuffer_GetGrabPacket();	//This removes the entity packets from the list in NetworkModule

		if (this->m_grabPacketList.size() > 0)
		{
			std::list<GrabPacket>::iterator itr;
			PhysicsComponent* pp = nullptr;
			Entity* ep = nullptr;

			for (itr = this->m_grabPacketList.begin(); itr != this->m_grabPacketList.end(); itr++)
			{
				//Check if we want to grab or drop			
				if (itr->grabbedID >= 0)	//Grab, negativ value is for drop
				{
					//Find the entity we want to grab
					for (int i = 0; i < this->m_dynamicEntitys.size(); i++) 
					{		
						ep = this->m_dynamicEntitys.at(i);				
						if (ep->GetEntityID() == itr->grabbedID)
						{
							//Check if the entity is already grabbed
							if (!ep->IsGrabbed())
							{
								//If it is player2 who want to grab
								if (itr->entityID == this->m_player2.GetEntityID())
								{
									this->m_player2.SetGrabbed(ep);
									//Send the update
									this->m_networkModule->SendGrabPacket(itr->entityID,itr->grabbedID);
								}

							}
							i = this->m_dynamicEntitys.size();
							break;
						}
					}
				}
				else //Drop
				{
					//If it is player2 who want to drop
					if (itr->entityID == this->m_player2.GetEntityID())
					{
						this->m_player2.SetGrabbed(nullptr);
						this->m_networkModule->SendGrabPacket(itr->entityID, itr->grabbedID);
					}
				}
				
			}
		}
		this->m_grabPacketList.clear();
#pragma endregion GrabPacket

		//Aming for player1 (SHOULD BE FOR THE CONTROLED PLAYER)
		if (inputHandler->IsMouseKeyPressed(SDL_BUTTON_RIGHT) && !this->m_player1.GetIsAming())
		{
			this->m_player1.SetAiming(true);
			this->m_cameraRef->SetDistance(2);
		}

		if (inputHandler->IsMouseKeyReleased(SDL_BUTTON_RIGHT) && this->m_player1.GetIsAming())
		{
			this->m_player1.SetAiming(false);
			this->m_cameraRef->SetDistance(10);
		}

		if (this->m_player1.GetIsAming())
		{
			this->m_player1.SetLookDir(this->m_cameraRef->GetDirection());
		}

		//SEND THE UPDATES THAT IS CONTROLED BY PLAYER1
		if (this->m_networkModule->GetNrOfConnectedClients() != 0)	//Player is host and there is connected clients
		{	
			PhysicsComponent* pp = this->m_player1.GetPhysicsComponent();
			

			this->m_networkModule->SendEntityUpdatePacket(0, pp->PC_pos, pp->PC_velocity, pp->PC_rotation);	//Send the update data for only player

			Entity* ent = nullptr;
			for (int i = 0; i < this->m_dynamicEntitys.size(); i++)	//Change start and end with physics packet
			{
				ent = this->m_dynamicEntitys.at(i);
				if (ent != this->m_player2.GetGrabbed())	//If it is not grabbed by player2
				{
					pp = this->m_dynamicEntitys.at(i)->GetPhysicsComponent();
					this->m_networkModule->SendEntityUpdatePacket(pp->PC_entityID, pp->PC_pos, pp->PC_velocity, pp->PC_rotation);	//Send the update data for only player
				}
			}

			
		}

	}
#pragma endregion Host/Player1 Specifics

#pragma region
	if (this->m_networkModule->IsHost() == false)
	{	
		Entity* wasGrabbed = this->m_player2.GetGrabbed();

		this->m_player2.SetRightDir(rightDir);
		this->m_player2.SetUpDir(upDir);
		this->m_player2.SetLookDir(playerLookDir);
		this->m_player2.Update(dt, inputHandler);
		
		//Other half of the components
		this->m_player1.SyncComponents();

		Entity* ent = nullptr;

		if (wasGrabbed != this->m_player2.GetGrabbed())
		{
			wasGrabbed->SyncComponents();
			
			this->m_networkModule->SendGrabPacket(this->m_player2.GetEntityID(), -1);
		}

		for (int i = 0; i < this->m_dynamicEntitys.size(); i++)
		{
			ent = this->m_dynamicEntitys.at(i);
			if (ent == this->m_player2.GetGrabbed())		//Check if the entity is not grabbed, if it is there will be an update packet for it
			{
				ent->Update(dt, inputHandler);
			}
			else
			{
				ent->SyncComponents();
			}
		}
		
		if (inputHandler->IsKeyPressed(SDL_SCANCODE_G))
		{

			PhysicsComponent* pp = this->m_player2.GetPhysicsComponent();
			PhysicsComponent* epp = this->m_cHandler->GetClosestPhysicsComponent(pp, 3);	//Get the closest component of 2 meters

			if (epp != nullptr)	//If a component was found
			{
				Entity* ent = nullptr;
				for (int i = 0; i < this->m_dynamicEntitys.size(); i++)
				{

					if (this->m_dynamicEntitys.at(i)->GetEntityID() == epp->PC_entityID)	//If the IDs match
					{
						ent = this->m_dynamicEntitys.at(i);
						break;
					}

				}

				if (!ent->IsGrabbed())
				{
					this->m_networkModule->SendGrabPacket(this->m_player2.GetEntityID(), ent->GetEntityID());	//Send a request to pick up dynamic entity with ID 2 (ball)
				}
			}
		}
		if (inputHandler->IsKeyPressed(SDL_SCANCODE_H))
		{
			//this->m_player2.SetGrabbed(nullptr);
			this->m_networkModule->SendGrabPacket(this->m_player2.GetEntityID(), -1);	//Send a request to drop the grabed entity (-1 is for drop)
		}

#pragma region

		this->m_grabPacketList = this->m_networkModule->PacketBuffer_GetGrabPacket();

		if (this->m_grabPacketList.size() > 0)
		{
			std::list<GrabPacket>::iterator itr;
			Entity* ep = nullptr;

			for (itr = this->m_grabPacketList.begin(); itr != this->m_grabPacketList.end(); itr++)
			{
				//We know that we can grab the entity recived since it was accepted by the host
				if (itr->grabbedID >= 0) //Grab
				{
					//Find the entity to grab
					for (int i =0 ; i < this->m_dynamicEntitys.size(); i++) 
					{
						ep = this->m_dynamicEntitys.at(i);

						if (ep->GetEntityID() == itr->grabbedID)
						{
							this->m_player2.SetGrabbed(ep);
							break;
						}
						else
						{
							this->m_player1.SetGrabbed(ep);
							break;
						}
						
					}

				}
				else //Drop
				{

					if (itr->entityID == this->m_player2.GetEntityID())
					{
						this->m_player2.SetGrabbed(nullptr);
					}
					else
					{
						this->m_player1.SetGrabbed(nullptr);
					}
					
				}
			}

			this->m_grabPacketList.clear();
		}

#pragma endregion GrabPacket

		//Aming for player1 (SHOULD BE FOR THE CONTROLED PLAYER)
		if (inputHandler->IsMouseKeyPressed(SDL_BUTTON_RIGHT) && !this->m_player2.GetIsAming())
		{
			this->m_player2.SetAiming(true);
			this->m_cameraRef->SetDistance(2);
		}

		if (inputHandler->IsMouseKeyReleased(SDL_BUTTON_RIGHT) && this->m_player2.GetIsAming())
		{
			this->m_player2.SetAiming(false);
			this->m_cameraRef->SetDistance(10);
		}

		if (this->m_player2.GetIsAming())
		{
			this->m_player2.SetLookDir(this->m_cameraRef->GetDirection());
		}

		//SEND THE UPDATES THAT IS CONTROLED BY PLAYER2
		if (this->m_networkModule->GetNrOfConnectedClients() != 0)	//Player is a client has a connection
		{

			PhysicsComponent* pp = this->m_player2.GetPhysicsComponent();
			this->m_networkModule->SendEntityUpdatePacket(pp->PC_entityID, pp->PC_pos, pp->PC_velocity, pp->PC_rotation);	//Send the update data for only player
			
			if (wasGrabbed != nullptr)	//If player2 has grabbed something
			{
				pp = wasGrabbed->GetPhysicsComponent();
				this->m_networkModule->SendEntityUpdatePacket(pp->PC_entityID, pp->PC_pos, pp->PC_velocity, pp->PC_rotation);				
			}
		
		}
	}
	
#pragma endregion Client/Player2 Specifics

	//update all dynamic entities
	for (int i = 0; i < this->m_dynamicEntitys.size(); i++)
	{
		this->m_dynamicEntitys.at(i)->Update(dt, inputHandler);
	}

	//Update all puzzle entities
	//Buttons require input for logical evaluation
	if (inputHandler->IsKeyPressed(SDL_SCANCODE_R))
	{
		for (std::vector<ButtonEntity*>::iterator i = this->m_buttonEntities.begin(); i != this->m_buttonEntities.end(); i++)
		{
			DirectX::XMFLOAT3 playerPos;
			DirectX::XMStoreFloat3(&playerPos, this->m_player1.GetPhysicsComponent()->PC_pos);

			(*i)->CheckPressed(playerPos);
		}
	}
	for (std::vector<ButtonEntity*>::iterator i = this->m_buttonEntities.begin(); i != this->m_buttonEntities.end(); i++)
	{
		DirectX::XMFLOAT3 playerPos;
		DirectX::XMStoreFloat3(&playerPos, this->m_player1.GetPhysicsComponent()->PC_pos);
		(*i)->Update(dt, inputHandler);
	}
	if (inputHandler->IsKeyDown(SDL_SCANCODE_R))
	{

		int increasing = (inputHandler->IsKeyDown(SDL_SCANCODE_LSHIFT)) ? -1 : 1;
		for (std::vector<WheelEntity*>::iterator i = this->m_wheelEntities.begin(); i != this->m_wheelEntities.end(); i++)
		{
			DirectX::XMFLOAT3 playerPos;
			DirectX::XMStoreFloat3(&playerPos, this->m_player1.GetPhysicsComponent()->PC_pos);

			(*i)->CheckPlayerInteraction(playerPos, increasing);
		}
	}
	if (inputHandler->IsKeyReleased(SDL_SCANCODE_R))
	{
		for (std::vector<WheelEntity*>::iterator i = this->m_wheelEntities.begin(); i != this->m_wheelEntities.end(); i++)
		{
			DirectX::XMFLOAT3 playerPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			(*i)->CheckPlayerInteraction(playerPos, 0);
		}
	}
	//Wheels require updates to rotate based on state calculated in CheckPlayerInteraction
	for (std::vector<WheelEntity*>::iterator i = this->m_wheelEntities.begin(); i != this->m_wheelEntities.end(); i++)
	{
		(*i)->Update(dt, inputHandler);
	}
	//Doors require updates to change opening state
	for (std::vector<DoorEntity*>::iterator i = this->m_doorEntities.begin(); i != this->m_doorEntities.end(); i++)
	{
		(*i)->Update(dt, inputHandler);
	}
	//Lock the camera to the player

	// Reactionary level director acts
	this->m_director.Update(dt);

#pragma region
	if (inputHandler->IsKeyPressed(SDL_SCANCODE_J))
	{
		if (this->m_networkModule->GetNrOfConnectedClients() <= 0)	//If the network module is NOT connected to other clients
		{
			if (this->m_networkModule->Join(this->m_ip))				//If we succsefully connected
			{
				printf("Joined client with the ip %s\n", this->m_ip);

				//TEMP SULOTION
				//Move the camera to player 2 since we joined a game 
				DirectX::XMVECTOR targetOffset = DirectX::XMVectorSet(0.0, 1.4, 0.0, 0.0);
				m_cameraRef->SetCameraPivot(
					&this->m_cHandler->GetPhysicsHandler()->GetDynamicComponentAt(1)->PC_pos,
					targetOffset,
					1.3f
				);
			}
			else
			{
				printf("Failed to connect to the client %s\n", this->m_ip);
			}

		}
		else
		{
			printf("Join failed since this module is already connected to other clients\n");
		}
	}
	if (inputHandler->IsKeyPressed(SDL_SCANCODE_K))
	{
		this->m_networkModule->SendFlagPacket(DISCONNECT_REQUEST);
	}
#pragma endregion Network_Key_events
	this->m_cameraRef->Update(dt);
	return result;
}

int LevelState::CreateLevel(LevelData::Level * data)
{
	DirectX::XMVECTOR rot;
	DirectX::XMVECTOR pos;
	rot.m128_f32[3] = 0.0f;	//Set w to 0
	pos.m128_f32[3] = 0.0f;	//Set w to 0

	DirectX::XMMATRIX translate;
	DirectX::XMMATRIX rotate;
	Resources::Model* modelPtr;
	Resources::Status st = Resources::ST_OK;
	Resources::ResourceHandler* resHandler = Resources::ResourceHandler::GetInstance();

	


	m_player1_Spawn = DirectX::XMVectorSet( //Store spawnPoint for player 1
		data->spawns[0].position[0],
		data->spawns[0].position[1],
		data->spawns[0].position[2],
		0);

	m_player2_Spawn = DirectX::XMVectorSet(	//Store spawnPoint for player 2
		data->spawns[1].position[0],
		data->spawns[1].position[1],
		data->spawns[1].position[2],
		0);

	std::vector<DynamicEntity*> aiEntities;

	m_player1.GetPhysicsComponent()->PC_pos = m_player1_Spawn;
	m_player1.GetPhysicsComponent()->PC_pos = DirectX::XMVectorAdd(m_player1_Spawn, DirectX::XMVectorSet(1, 6, 0, 0));

	for (size_t i = 0; i < data->numEntities; i++)
	{
		LevelData::EntityHeader* currEntity = &data->entities[i]; //Current entity
		GraphicsComponent* t_gc;
		Resources::Model * modelPtr;

		AnimationComponent* t_anim = nullptr;

		resHandler->GetModel(currEntity->modelID, modelPtr);

		if (modelPtr->GetSkeleton() != nullptr)
		{
			t_gc = m_cHandler->GetGraphicsAnimationComponent();
			((GraphicsAnimationComponent*)t_gc)->jointCount = modelPtr->GetSkeleton()->GetSkeletonData()->jointCount;
			for (int i = 0; i < modelPtr->GetSkeleton()->GetSkeletonData()->jointCount; i++)
			{
				((GraphicsAnimationComponent*)t_gc)->finalJointTransforms[i] = DirectX::XMMatrixIdentity();
			}

			/*if (modelPtr->GetSkeleton()->GetNumAnimations() > 0)
			{
				t_anim = m_cHandler->GetAnimationComponent();

				int numAnimations = modelPtr->GetSkeleton()->GetNumAnimations();
				for (int i = 0; i < numAnimations; i++)
				{
					Resources::Animation* animation = modelPtr->GetSkeleton()->GetAnimation(i);
					int jointIndex = 0;
					const Resources::Animation::AnimationJoint* animationJoint = animation->GetJoint(jointIndex);
					t_anim->Anim_StateData[i].startFrame = animationJoint->keyframes[0].timeValue;
					int keyCount = animationJoint->keyframeCount;
					t_anim->Anim_StateData[i].endFrame = animationJoint->keyframes[keyCount - 1].timeValue;
					t_anim->Anim_StateData[i].localTime = 0;
					t_anim->Anim_StateData[i].isLooping = true;
				}
			}*/
		}
		else
		{
			t_gc = m_cHandler->GetGraphicsComponent();
		}
		t_gc->modelID			= currEntity->modelID;
		t_gc->active			= true;
		t_gc->modelPtr			= modelPtr; //Get and apply a pointer to the model
		//Create GraphicsComponent
		
		//Create world matrix from data
		memcpy(pos.m128_f32, currEntity->position, sizeof(float) * 3);	  //Convert from POD to DirectX Vector
		memcpy(rot.m128_f32, currEntity->rotation, sizeof(float) * 3);	  //Convert from POD to DirectX Vector
		translate = DirectX::XMMatrixTranslationFromVector(pos);
		DirectX::XMMATRIX rotationMatrixY = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(rot.m128_f32[1]));
		DirectX::XMMATRIX rotationMatrixX = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(rot.m128_f32[0]));
		DirectX::XMMATRIX rotationMatrixZ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rot.m128_f32[2]));
		//Create the rotation matrix
		DirectX::XMMATRIX rotate = DirectX::XMMatrixMultiply(rotationMatrixZ, rotationMatrixX);
		rotate = DirectX::XMMatrixMultiply(rotate, rotationMatrixY);
		//rotate    = DirectX::XMMatrixRotationRollPitchYawFromVector(rot);
		t_gc->worldMatrix = DirectX::XMMatrixMultiply(rotate,translate);
		
		//Create Physics component
		PhysicsComponent* t_pc = m_cHandler->GetPhysicsComponent();
		t_pc->PC_entityID	   = currEntity->EntityID;		//Set Entity ID
		t_pc->PC_pos		   = pos;						//Set Position
		t_pc->PC_rotation	   = rot;						//Set Rotation
		t_pc->PC_is_Static	   = currEntity->isStatic;		//Set IsStatic
		t_pc->PC_active		   = true;						//Set Active
		

		st = Resources::ResourceHandler::GetInstance()->GetModel(currEntity->modelID, modelPtr);

		//get information from file
		t_pc->PC_BVtype = BV_AABB;

		t_pc->PC_AABB.ext[0] = modelPtr->GetOBBData().extension[0];
		t_pc->PC_AABB.ext[1] = modelPtr->GetOBBData().extension[1];
		t_pc->PC_AABB.ext[2] = modelPtr->GetOBBData().extension[2];

		DirectX::XMVECTOR tempRot = DirectX::XMVector3Transform(DirectX::XMVECTOR{ t_pc->PC_AABB.ext[0],
			t_pc->PC_AABB.ext[1] , t_pc->PC_AABB.ext[2] }, rotate);

		t_pc->PC_AABB.ext[0] = abs(tempRot.m128_f32[0]);
		t_pc->PC_AABB.ext[1] = abs(tempRot.m128_f32[1]);
		t_pc->PC_AABB.ext[2] = abs(tempRot.m128_f32[2]);

		/*DirectX::XMMATRIX tempRot = DirectX::XMMatrixTranslationFromVector(DirectX::XMVECTOR{ t_pc->PC_AABB.ext[0],
			t_pc->PC_AABB.ext[1] , t_pc->PC_AABB.ext[2] });
		tempRot = tempRot*rotate;

		t_pc->PC_AABB.ext[0] = abs(tempRot.r[3].m128_f32[0]);
		t_pc->PC_AABB.ext[1] = abs(tempRot.r[3].m128_f32[1]);
		t_pc->PC_AABB.ext[2] = abs(tempRot.r[3].m128_f32[2]);*/

		t_pc->PC_friction = 0.95f;
#ifdef _DEBUG
		if (st != Resources::ST_OK)
			std::cout << "Model could not be found when loading level data,  ID: " << currEntity->modelID << std::endl;
#endif // _DEBUG

		t_pc->PC_OBB = m_ConvertOBB( modelPtr->GetOBBData()); //Convert and insert OBB data
	
		//t_pc->PC_OBB.ort = DirectX::XMMatrixMultiply(t_pc->PC_OBB.ort, rotate);

#pragma region AIComp check
		// Correct check but does not work with current testinglevel
		//if(currEntity->hasAi)
		//	t_pc->PC_is_Static = false;
		// Temporary check to work with both new and old testinglevels
		for (size_t q = 0; q < data->numAI; q++)
		{
			if (currEntity->EntityID == data->aiComponents[q].entityID)
			{
				t_pc->PC_is_Static = false;
			}
		}
#pragma endregion


		if (t_pc->PC_is_Static) {
			StaticEntity* tse = new StaticEntity();
			tse->SetGraphicsComponent(t_gc);
			tse->SetPhysicsComponent(t_pc);
			tse->SetAnimationComponent(t_anim);

			this->m_staticEntitys.push_back(tse); //Push new entity to list
		}
		else {
			
			DynamicEntity* tde = new DynamicEntity();
			tde->SetGraphicsComponent(t_gc);
			tde->SetPhysicsComponent(t_pc);
			tde->SetAnimationComponent(t_anim);

			this->m_dynamicEntitys.push_back(tde); //Push new entity to list
			aiEntities.push_back(tde);// Push entity to initialize AIComp later
		}


	}
	for (size_t A = 0; A < aiEntities.size(); A++)
	{
		for (size_t B = 0; B < data->numAI; B++)
		{
			if (data->aiComponents[B].entityID == aiEntities[A]->GetEntityID())
			{
				aiEntities[A]->GetPhysicsComponent()->PC_is_Static = true;
				AIComponent* temp = m_cHandler->GetAIComponent();
				temp->AC_triggered = true;// Temp: Needed for AIHandler->Update()
				temp->AC_entityID = data->aiComponents[A].entityID;
				temp->AC_time = data->aiComponents[A].time;
				temp->AC_speed = data->aiComponents[A].speed;
				temp->AC_pattern = data->aiComponents[A].pattern;
				temp->AC_nrOfWaypoint = data->aiComponents[A].nrOfWaypoints;
				for (size_t x = 0; x < temp->AC_nrOfWaypoint; x++)
				{
					temp->AC_waypoints[x] = {
						data->aiComponents[A].wayPoints[x][0],
						data->aiComponents[A].wayPoints[x][1],
						data->aiComponents[A].wayPoints[x][2]
					};
				}
				temp->AC_position = temp->AC_waypoints[0];
				aiEntities[A]->SetAIComponent(temp);
			}
		}
	}

	Resources::Model* model = m_player1.GetGraphicComponent()->modelPtr;
	m_player1.GetGraphicComponent()->modelID = 2759249725;
	Resources::ResourceHandler::GetInstance()->GetModel(2759249725, model);
	
	return 1;
}

