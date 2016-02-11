#include "..\include\Task.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\TaskHelper.h"

Task::Task()
{
	taskID = TaskHelper::getNextID();
	taskName = "Task";
	complete = false;
	assigned = false;
	acting = false;
	coalitionID = -1;
	coalition = nullptr;
	creationFrame = BWAPI::Broodwar->getFrameCount();
	cost = 0.0;
	profit = 0.0;
	TaskHelper::addTask(this, false);
}

Task::~Task()
{
	std::cout << "~" << taskName.c_str() << "\n";
	cleanSubTasks();	
	delete coalition;
	coalition = nullptr;
	TaskHelper::removeTask(this);
}

void Task::setCoalition(Coalition* coalition)
{
	this->coalition = coalition;
	coalitionID = coalition->getID();
}

bool Task::isComplete() const
{
	return complete;
}

int Task::getID() const
{
	return taskID;
}

std::string Task::getName() const
{
	return taskName;
}

Taskset Task::getSubTasks() const
{
	return subTasks;
}

Coalition* Task::getCoalition() const
{
	return coalition;
}

double Task::getCost()
{
	cost = 0.0;

	for (auto task : subTasks)
		cost += task->getCost();
	if (coalitionID != -1)
		cost += coalition->getCost();

	return this->cost;
}

double Task::getProfit()
{
	return profit;
}

void Task::cleanSubTasks()
{
	for(auto &task : subTasks)
		TaskHelper::removeTask(task);
	subTasks.clear();
}

void Task::succeed()
{
	complete = true;
	profit = 1.0;
	std::cout << taskID << " : " << taskName << " : Success!\n";

	cleanSubTasks();
}

void Task::fail()
{	
	complete = true;
	profit = 0.0;
	std::cout << taskID << " : " << taskName << " : Failure!\n";

	cleanSubTasks();
}

void Task::updateTaskTree()
{
	//std::cout << taskName.c_str() << " : " << taskID << " : UpdateTaskTree\n";
	if (subTasks.size() > 0)
	{
		//std::cout << subTasks.size() << "\n";
		for (auto it = subTasks.begin(); it != subTasks.end();)
		{
			//std::cout << (*it)->getName().c_str() << " : " << (*it)->getID() << "\n";
			if ((*it)->isComplete())
			{
				auto tempIt = subTasks.erase(it);
				delete (*it);
				it = tempIt;
			}
			else
			{
				(*it)->updateTaskTree();
				++it;
			}
		}
	}
	update();
	//std::cout << taskName.c_str() << " : " << taskID << " : UpdateTaskTree End\n";
}