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
		std::cout << "Removing " << task->getName().c_str() << " from root taskset\n";
		rootTaskSet.erase(task);
		std::cout << "Removing " << task->getName().c_str() << " from full taskset\n";
		fullTaskSet.erase(task);		
		/*std::cout << "Deleting " << task->getName().c_str() << "\n";
		delete task;*/
	}

	void updateRootTasks()
	{
		for (auto &task : rootTaskSet)
			if (task->isComplete())
				delete task;
			else
				task->updateTaskTree();
	}
}