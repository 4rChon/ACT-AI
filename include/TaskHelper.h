#pragma once
#include "Task.h"

namespace TaskHelper
{
	int getNextID();
	void addTask(Task* task);
	void removeTask(Task* task);
}