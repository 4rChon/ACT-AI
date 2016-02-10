#pragma once

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>
#include "Worker.h"

class Workerset : public BWAPI::SetContainer < Worker*, std::hash<void*>> {};