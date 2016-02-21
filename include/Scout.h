#pragma once
#include "Task.h"
#include "MapHelper.h"

class Scout : public Task
{
private:
	MapHelper::Zone* target;
public:
	Scout(MapHelper::Zone* target);
	void createCoalition();
	void assign();
	void act();
	void update();
};