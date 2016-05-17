#pragma once
#include "Task.h"

namespace TaskHelper
{
	void initialiseHelper();
	int getNextID();
	Task* addTask(Task* task, bool root = false);
	void removeTask(Task* task);
	void deleteTaskTree(Taskset & taskTree);
	void updateTaskTree(Taskset & taskTree);
	void updateRootTasks();
	Taskset& getRootTasks();
	Taskset& getAllTasks();
}