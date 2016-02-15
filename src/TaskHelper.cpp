#include "..\include\TaskHelper.h"
#include <string>

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

	bool addTask(Task* newTask, bool root)
	{
		//check whether similar task has been added
		if (root)
			rootTaskSet.insert(newTask);
		else
		{
			for (auto &task : fullTaskSet)
			{
				std::cout << "Task : " << task->getName() << "\nNewTask : " << newTask->getName() << "\n";
				std::cout << task->getName().compare(newTask->getName()) << "\n";
				if(newTask->getName().compare("Coalition(Composition, Task*)") != 0
					&& task->getName().compare(newTask->getName()) == 0)
					return true;
			}
			std::cout << "----\n";
		}

		fullTaskSet.insert(newTask);
		return true;
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