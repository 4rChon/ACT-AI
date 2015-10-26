#pragma once
#include <unordered_set>

#include <BWAPI.h>

#include "Composition.h"
#include "Globals.h"
#include "Task.h"
#include "Threatfield.h"
#include "Zone.h"
#include "Attack.h"


namespace std 
{
	template<> struct hash < Agent >
	{
		size_t operator()(const Agent &a) const
		{
			return (51 * (std::hash<int>()(a.getUnit()->getID())));
		}
	};

	template<> struct equal_to < Agent >
	{
		bool operator()(const Agent &lhs, const Agent &rhs) const
		{
			return lhs.getUnit()->getID() == rhs.getUnit()->getID();
		}
	};
}

// Remember not to use "Broodwar" in any global class constructor!

class Core : public BWAPI::AIModule
{
private:
	//Agentset agents;
	int tempBool = 0;
	bool drawGui;
	ThreatField* threatField;
	Attack* attack;

	//Composition attackComp;

public:
	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);
	// Everything below this line is safe to modify.
	void drawRegions();
};
