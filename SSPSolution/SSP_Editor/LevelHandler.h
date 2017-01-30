#ifndef SSPEDITOR_LEVELHANDLER_LEVELHANDLER_H
#define SSPEDITOR_LEVELHANDLER_LEVELHANDLER_H
#include <fstream>
#include "LevelHeaders.h"
#include "Level.h"
#include <qfiledialog.h>
#include <DataHandler.h>

class LevelHandler
{
	enum Operation {
		LOAD,
		SAVE
	};
private:
	Level m_currentLevel;
	QString filePath = "//DESKTOP-BOKNO6D/server/Assets/bbf files/Levels/";
	LevelHandler();
	
public:
	~LevelHandler();
	static LevelHandler* GetInstance();

	LevelData::LevelStatus ExportLevelFile();
	LevelData::LevelStatus ImportLevelFile();
	LevelData::LevelStatus NewLevel();
	Level* GetCurrentLevel() { return &this->m_currentLevel; };
private:
	std::string GetFilePathAndName(Operation flag);

#pragma region Save Functions
	LevelData::MainLevelHeader GetMainHeader();
	LevelData::LevelStatus GetEntityData(char* dataPtr);
	LevelData::LevelStatus GetResourceData(char* dataPtr);
	LevelData::LevelStatus GetSpawnData(char* dataPtr);
	LevelData::LevelStatus GetAiData(char* dataPtr);
	LevelData::LevelStatus GetCheckpointData(char* dataPtr);
	LevelData::LevelStatus GetButtonData(char* dataPtr);
	LevelData::LevelStatus GetDoorData(char* dataPtr);
#pragma endregion

#pragma region Load Functions
	LevelData::LevelStatus LoadEntities(LevelData::EntityHeader* dataPtr, size_t numEntities);
	LevelData::LevelStatus LoadAiComponents(LevelData::AiHeader* dataPtr, size_t numComponents);
	LevelData::LevelStatus LoadCheckpointComponents(LevelData::CheckpointHeader* dataPtr, size_t numComponents);
	LevelData::LevelStatus LoadTriggerComponents(LevelData::ButtonHeader* dataPtr, size_t numComponents);
	LevelData::LevelStatus LoadTriggerComponents(LevelData::DoorHeader* dataPtr, size_t numComponents);
#pragma endregion
 };

#endif