#pragma once

#include "Coalition.h"
#include "Composition.h"
#include "Task.h"
#include "Coalitionset.h"

class CoalitionManager
{
private:
	Coalitionset coalitionSet;
	static CoalitionManager* instance;
	int openCoalitions;
public:
	//constructors and destructors
	CoalitionManager();
	~CoalitionManager();

	//getters
	static CoalitionManager* getInstance();
	Coalition* getCoalition(int coalitionID);
	Coalitionset& getCoalitionset();
	int getOpenCount();

	//-	
	int addCoalition(Composition composition, Task* task);
	void removeCoalition(int id);
	void removeCoalition(Coalition* coalition);
	Coalitionset::iterator removeCoalition(Coalitionset::iterator coalition);
	//void activateCoalition(Coalition* coalition);
	//void disbandCoalition(Coalition* coalition);
};