#pragma once
#include "Task.h"

namespace TaskHelper
{
	void initialiseHelper();
	int getNextID();
	Task* getTask(int id);
	Task* addTask(Task* const& task, bool root = false);
	void deleteTask(Task* const& task);
	void updateRootTasks();
	const Taskset& getRootTasks();
	const Taskset& getAllTasks();
	const Taskset& getAttackTasks();
}