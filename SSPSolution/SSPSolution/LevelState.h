#ifndef SSPAPPLICATION_GAMESTATES_LEVELSTATE_H
#define SSPAPPLICATION_GAMESTATES_LEVELSTATE_H
#include "GameState.h"
class LevelState :
	public GameState
{
private:
public:
	LevelState();
	virtual ~LevelState();

	int Initialize(GameStateHandler* gsh);
	int Update(float dt, InputHandler * inputHandler);
private:
};

#endif