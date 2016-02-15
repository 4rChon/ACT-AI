#include "..\include\Task.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\TaskHelper.h"
#include <string>

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
	debug = false;
	//TaskHelper::addTask(this, false);
}

Task::~Task()
{
	printDebugInfo("DELETE");
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

void Task::setUnitSatisfied(bool unitSatisfied)
{
	this->unitSatisfied = unitSatisfied;
}

void Task::setTechSatisfied(bool techSatisfied)
{
	this->techSatisfied = techSatisfied;
}

bool Task::isUnitSatisfied() const
{
	return unitSatisfied;
}

bool Task::isTechSatisfied() const
{
	return techSatisfied;
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
	if (subTasks.size() > 0)
	{
		for (auto it = subTasks.begin(); it != subTasks.end();)
		{
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
}

void Task::printDebugInfo(std::string info)
{
	if(debug)
		std::cout << getName().c_str() << " : " << taskID << " : " << info << "\n";
}