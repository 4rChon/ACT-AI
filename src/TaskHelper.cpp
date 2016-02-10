#include "..\include\TaskHelper.h"

namespace TaskHelper
{
	namespace
	{
		static Taskset taskSet;
		static int nextID;
	}

	void initialiseHelper()
	{
		nextID = 0;
	}

	int getNextID()
	{
		return ++nextID;
	}

	void addTask(Task* task)
	{
		taskSet.insert(task);
	}

	void removeTask(Task* task)
	{
		taskSet.erase(task);
		delete task;
	}
}