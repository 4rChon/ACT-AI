#pragma once

#include "Coalition.h"
#include "Composition.h"
#include "Task.h"

namespace CompositionHelper
{
	struct UsedComposition
	{
		Composition composition;
		TaskType taskType;
		int activationFrame;
		double fitness;
		int useCount;
	};

	void initialiseHelper();

	//getters
	Composition getComposition(TaskType taskType);
	Composition getCounters(BWAPI::UnitType enemyUnitType);
	//UsedComposition filterByCounter(BWAPI::SetContainer<BWAPI::UnitType> unitTypes);
	const std::vector<UsedComposition>& getCompositionSet();
	/*const std::vector<UsedComposition>& getWorkingSet();*/

	//helpers
	void loadCompositions();
	void saveComposition(Coalition* coalition);
	void saveComposition(UsedComposition usedComposition);
	void saveCompositions();
};