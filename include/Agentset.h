#pragma once

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>
#include "Agent.h"

class Agentset : public BWAPI::SetContainer < Agent*, std::hash<void*>> {};