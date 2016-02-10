#pragma once

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>
#include "ResourceDepot.h"

class Baseset : public BWAPI::SetContainer < ResourceDepot*, std::hash<void*>> {};