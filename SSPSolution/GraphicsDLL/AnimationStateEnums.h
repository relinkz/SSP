#ifndef GRAPHICSDLL_ANIMATIONS_ANIMATIONSTATEENUMS_H
#define GRAPHICSDLL_ANIMATIONS_ANIMATIONSTATEENUMS_H

#ifdef GRAPHICSDLL_EXPORTS
#define GRAPHICSDLL_API __declspec(dllexport)
#else
#define GRAPHICSDLL_API __declspec(dllimport)
#endif

enum AnimationStates : int
{
	ANIMATION_ERROR = -1,
	PLAYER_IDLE = 0, //Temp place right now because the order of indexing is strange for idle and walk. 
	PLAYER_RUN_FORWARD,
	PLAYER_RUN_BACKWARD,
	PLAYER_RUN_LEFT,
	PLAYER_RUN_RIGHT,
	PLAYER_PICKUP,
	PLAYER_THROW, 
	LEVEL_DEACTIVATED = 0,
	LEVEL_ACTIVATED,
	DOOR_OPENING = 0, 
	DOOR_CLOSING 
};

enum Blending
{
	NO_TRANSITION = 0,
	SMOOTH_TRANSITION = 1
	//FROZEN_TRANSITION	=	2
};

#endif