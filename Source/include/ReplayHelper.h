#pragma once
#include "BWAPI.h"
#include "CompositionHelper.h"

namespace ReplayHelper
{
	void initialiseHelper();
	void getEnemyUnitsAtFrame(int frame);
	void loadFlaggedCompositions();
	void evolveFlaggedCompositions();
	void evolveComposition(CompositionHelper::UsedComposition composition);
}