#pragma once
#include "Task.h"
#include "Zone.h"

class Scout : public Task
{
private:
	Zone* target;
public:
	Scout(Zone* target);
	void assign();
	void act();
	void update();
};