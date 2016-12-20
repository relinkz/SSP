#include "Progression.h"

Progression* Progression::m_instance = nullptr;

Progression::Progression()
{
	this->m_currentLevel = 0;
	this->m_currentCheckpoint = 0;
	this->m_unlockedLevels = 0;
}

Progression::~Progression()
{

}

bool Progression::WriteToFile(std::string filename)
{
	std::ofstream saveFile;

	saveFile.open("..\\Debug\\Saves\\" + filename + ".txt");

	if (!saveFile.is_open()) {
		return false;
	}
	else
	{

		saveFile << "Allhuakbar" << "\r\n";
		saveFile.close();
	}
	return true;
}

bool Progression::ReadFromFile(std::string filename)
{
	return false;
}
