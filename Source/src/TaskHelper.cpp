#include "TaskHelper.h"
#include "CoalitionHelper.h"
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
				/*std::cout << "Task : " << task->getName() << "\nNewTask : " << newTask->getName() << "\n";
				std::cout << task->getName().compare(newTask->getName()) << "\n";*/

				if (rootTaskSet.count(task) == 0 
					&& newTask->getType() != TaskType::CRC
					&& task->getName().compare(newTask->getName()) == 0)
				{
					removeTask(newTask);
					return task;
				}
			}
			/*std::cout << "----\n";*/
		}

		fullTaskSet.insert(newTask);
		return newTask;
	}

	void removeTask(Task* task)
	{
		if (rootTaskSet.count(task) > 0)
		{
			std::cout << "Removing " << task->getName().c_str() << " : " << task->getID() << " from root taskset\n";
			rootTaskSet.erase(task);
		}

		if (fullTaskSet.count(task) > 0)
		{
			std::cout << "Removing " << task->getName().c_str() << " : " << task->getID() << " from full taskset\n";
			fullTaskSet.erase(task);
			delete task;
		}		
	}

	void updateRootTasks()
	{		
		if (rootTaskSet.size() > 0)
		{
			for (auto it = rootTaskSet.begin(); it != rootTaskSet.end(); ++it)
				if ((*it)->isComplete())
					removeTask(*it);
				else
					(*it)->updateTaskTree();
		}
	}
}