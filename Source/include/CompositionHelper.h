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
	const std::vector<UsedComposition>& getCompositionSet();

	//helpers
	void loadCompositions();
	void saveComposition(Coalition* const& coalition);
	void saveComposition(UsedComposition usedComposition);
	void saveCompositions();
};