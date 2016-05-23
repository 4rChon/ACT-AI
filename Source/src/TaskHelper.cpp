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
		static Taskset attackTasks;
		static int nextID;
	}

	void initialiseHelper()
	{
		nextID = 0;
		
		/* optional opening */
		//CreateUnit* createUnit = new CreateUnit(...);
		//addTask(createUnit, true);
	}

	int getNextID()
	{
		return ++nextID;
	}

	Task* getTask(int id)
	{
		for (auto &task : fullTaskSet)
		{
			if (task->getID() == id)
				return task;
		}
		return nullptr;
	}

	const Taskset& getRootTasks()
	{
		return rootTaskSet;
	}

	const Taskset& getAllTasks()
	{
		return fullTaskSet;
	}

	const Taskset& getAttackTasks()
	{
		return attackTasks;
	}

	Task* addTask(Task* const& newTask, bool root)
	{
		//check whether similar task has been added
		if (root)
			rootTaskSet.insert(newTask);
		else
		{
			for (auto &task : fullTaskSet)
			{
				if (rootTaskSet.count(task) == 0 && 
					(newTask->getType() == STR || newTask->getType() == SUR || newTask->getType() == CRU)
					&& task->getName().compare(newTask->getName()) == 0)
				{
					deleteTask(newTask);
					return task;
				}
			}
		}

		if (newTask->getType() == ATT)
			attackTasks.insert(newTask);

		fullTaskSet.insert(newTask);
		return newTask;
	}

	void deleteTask(Task* const& task)
	{
		if (!task)
			return;

		attackTasks.erase(task);
		fullTaskSet.erase(task);

		delete task;
	}

	void updateRootTasks()
	{
		for (auto &taskIt = rootTaskSet.begin(); taskIt != rootTaskSet.end();)
		{
			(*taskIt)->printDebugInfo("Iterating");
			if ((*taskIt)->isComplete())
			{
				(*taskIt)->printDebugInfo("\n\tDeleting");
				deleteTask(*taskIt);
				taskIt = rootTaskSet.erase(taskIt);
				continue;
			}

			(*taskIt)->printDebugInfo("\n\tUpdating");
			(*taskIt)->updateSubTasks();
			(*taskIt)->update();
			++taskIt;
		}
	}
}