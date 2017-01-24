#pragma once
#include <DirectXMath.h>
#include "../GraphicsDLL/GraphicsHandler.h"
#include "../AIDLL/AIComponent.h"
#include "LevelHeaders.h"

enum ContainerType
{
	MODEL,
	CHECKPOINT,
	BUTTON,
	LEVER,
	WHEEL,
	LIGHT,
	AIWAYPOINT,
	DOOR,
	MAGNET,
	PRESSUREPLATE,

	NUM_TYPES
};
struct Container
{
	unsigned int	  internalID;
	DirectX::XMVECTOR position;		// Total v�rde. 
	DirectX::XMVECTOR rotation;		// Total v�rde. 
	GraphicsComponent component;
	AIComponent*	  aiComponent = nullptr;
	bool			  isDirty      = false;
	bool			  isStatic     = true;

	ContainerType type;
	Container() {}
	Container(const Container &obj) {  // copy constructor
	
		this->internalID	= obj.internalID	;
		this->position		= obj.position		;
		this->rotation		= obj.rotation		;
		this->component		= obj.component		;
		this->aiComponent	= obj.aiComponent	;
		this->isDirty		= obj.isDirty		;
		this->isStatic		= obj.isStatic		;
	}
	Container& operator=(Container const& obj)
	{
	
		this->internalID  = obj.internalID;
		this->position	  = obj.position;
		this->rotation	  = obj.rotation;
		this->component   = obj.component;
		this->aiComponent = obj.aiComponent;
		this->isDirty	  = obj.isDirty;
		this->isStatic	  = obj.isStatic;

		return *this;
	}

	};

struct Button : Container
{
	float interactionDistance;
	float resetTime; // Seconds
};

struct CheckpointContainer : Container
{
	unsigned int internalID;
	OBB obb;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR rotation;
	DirectX::XMVECTOR scale;


	LevelData::CheckpointHeader checkpointHeader;
	CheckpointContainer()
	{
		internalID = UINT_MAX;
		position = { 0.0,0.0,0.0 };
		rotation = { 0.0,0.0,0.0 };
		scale = { 1.0, 1.0, 1.0 };

		obb.ort.r[0] = { 1.0f,0.0f,0.0f };
		obb.ort.r[1] = { 0.0f,1.0f,0.0f };
		obb.ort.r[2] = { 0.0f,0.0f,1.0f };
		obb.ext[0] = 0.5f;
		obb.ext[1] = 0.5f;
		obb.ext[2] = 0.5f;
	}
};

struct SelectionLists
{
	std::vector<Resources::Model*>* modelPtr;
	std::vector<CheckpointContainer*>* checkpointPtr;
	//std::vector<Lights*>* lightsPtr;
};
