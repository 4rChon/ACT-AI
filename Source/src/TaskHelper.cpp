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
		
		/* opening */
		//CreateUnit* createUnit = new CreateUnit(BWAPI::UnitTypes::Terran_Barracks, 1);
		//addTask(createUnit, true);
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
			task = nullptr;
		}
	}

	void updateRootTasks()
	{		
		updateTaskTree(rootTaskSet);
	}

	void deleteTaskTree(Taskset &taskTree)
	{
		for (auto &task : taskTree)
		{
			auto superTasks = task->getSuperTasks();
			for (auto &superTask : superTasks)
			{
				superTask->getSubTasks().erase(task);
			}
		
			removeTask(task);
			taskTree.erase(task);
		}
	}

	void updateTaskTree(Taskset &taskTree)
	{
		if (taskTree.size() == 0)
			return;

		for (auto &task : taskTree)
		{
			if (task->isComplete())
			{
				taskTree.erase(task);
				removeTask(task);
				continue;
			}

			updateTaskTree(task->getSubTasks());
			task->update();
		}
	}
}