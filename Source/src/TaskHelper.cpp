#include "TaskHelper.h"
#include "CoalitionHelper.h"
#include "CreateUnit.h"
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

	Taskset& getRootTasks()
	{
		return rootTaskSet;
	}

	Taskset& getAllTasks()
	{
		return fullTaskSet;
	}

	Task* addTask(Task* newTask, bool root)
	{
		//check whether similar task has been added
		if (root)
			rootTaskSet.insert(newTask);
		else
		{
			for (auto &task : fullTaskSet)
			{
				if (rootTaskSet.count(task) == 0 && 
					(newTask->getType() == STR 
					|| newTask->getType() == SUR)
					&& task->getName().compare(newTask->getName()) == 0)
				{
					removeTask(newTask);
					return task;
				}
			}
		}

		fullTaskSet.insert(newTask);
		return newTask;
	}

	void removeTask(Task* task)
	{
		if (fullTaskSet.count(task) > 0)
		{
			fullTaskSet.erase(task);
			delete task;
		}
	}

	void updateRootTasks()
	{		
		if (rootTaskSet.size() > 0)
		{
			for (auto it = rootTaskSet.begin(); it != rootTaskSet.end();)
				if ((*it)->isComplete())
				{					
					removeTask(*it);
					it = rootTaskSet.erase(it);
				}
				else
				{
					(*it)->updateTaskTree();
					++it;
				}
		}
	}
}