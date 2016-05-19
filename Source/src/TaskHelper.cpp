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
		
		/* opening */
		//CreateUnit* createUnit = new CreateUnit(BWAPI::UnitTypes::Terran_Barracks, 1);
		//addTask(createUnit, true);
	}

	int getNextID()
	{
		return ++nextID;
	}

	Task * getTask(int id)
	{
		for (auto& task : fullTaskSet)
		{
			if (task->getID() == id)
				return task;
		}
		return nullptr;
	}

	Taskset& getRootTasks()
	{
		return rootTaskSet;
	}

	Taskset& getAllTasks()
	{
		return fullTaskSet;
	}

	Taskset& getAttackTasks()
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
					(newTask->getType() == STR || newTask->getType() == SUR)
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
		//task = nullptr;
	}

	void updateRootTasks()
	{
		for (auto &task : rootTaskSet)
		{
			if (task->isComplete())
			{
				rootTaskSet.erase(task);
				deleteTask(task);
				continue;
			}

			task->updateSubTasks();
			task->update();
		}
	}
}