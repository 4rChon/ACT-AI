#pragma once
#include "CoalitionHelper.h"
#include "DesireHelper.h"

namespace CoalitionHelper
{
	namespace
	{
		static Coalitionset coalitionSet;
		static int nextID;
	}
	
	void initialiseHelper()
	{
		nextID = 0;
	}

	const Coalitionset& getCoalitionset()
	{
		return coalitionSet;
	}

	int getNextID()
	{
		return ++nextID;
	}
	
	Coalition* addCoalition(Composition composition, Task* task)
	{
		Coalition* coalition = new Coalition(composition, task);
		coalitionSet.insert(coalition);
		return coalition;
	}

	void removeCoalition(Coalition* const& coalition)
	{
		if (coalition == nullptr)
			return;

		if (coalitionSet.count(coalition) > 0)
		{
			coalitionSet.erase(coalition);
			delete coalition;
		}
	}
};