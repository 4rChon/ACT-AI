#pragma once
#include "Task.h"

namespace TaskHelper
{
	void initialiseHelper();
	int getNextID();
	Task* getTask(int id);
	Task* addTask(Task* task, bool root = false);
	void deleteTask(Task * task);
	void updateRootTasks();
	Taskset& getRootTasks();
	Taskset& getAllTasks();
	Taskset& getAttackTasks();
}