#pragma once
#include "Task.h"
#include "MapHelper.h"

class Scout : public Task
{
public:
	Scout(MapHelper::Zone* target);
	void createCoalition();
	void assign();
	void act();
	void update();
	void succeed();
};