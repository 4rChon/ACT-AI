#pragma once
#include "BWAPI.h"
#include "CompositionHelper.h"

namespace ReplayHelper
{
	void initialiseHelper();
	void loadFlaggedCompositions();
	void evolveFlaggedCompositions();
	void evolveComposition(CompositionHelper::UsedComposition composition);
	Composition mutateComposition(Composition usedComp);
}