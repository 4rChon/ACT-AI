#include "..\include\TaskHelper.h"

namespace TaskHelper
{
	namespace
	{
		static Taskset rootTaskSet;
		static Taskset fullTaskSet;
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

	Taskset getRootTasks()
	{
		return rootTaskSet;
	}

	Taskset getAllTasks()
	{
		return fullTaskSet;
	}

	void addTask(Task* task, bool root)
	{
		if (root)
			rootTaskSet.insert(task);
		fullTaskSet.insert(task);
	}

	void removeTask(Task* task)
	{		
		rootTaskSet.erase(task);
		fullTaskSet.erase(task);
		delete task;
	}

	void updateTaskTree(Task* task)
	{
		if (task->getSubTasks().size() > 0)
		{
			for (auto t : task->getSubTasks())
				updateTaskTree(t);
			if (task->isComplete())
				removeTask(task);
		}
		task->update();
	}

	void updateRootTasks()
	{
		for (auto &task : rootTaskSet)
			updateTaskTree(task);
	}
}