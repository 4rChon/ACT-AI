#include "Task.h"
#include "CoalitionHelper.h"
#include "TaskHelper.h"
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
	debug = true;
	taskType = NON;
}

Task::~Task()
{
	printDebugInfo("DELETE", true);
	cleanSubTasks();
	if (taskType != CRC && taskType != NON && coalition)
	{
		CoalitionHelper::removeCoalition(coalition);
		coalition = nullptr;
	}
	//TaskHelper::getAllTasks().erase(this);
	//TaskHelper::removeTask(this);
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

TaskType Task::getType() const
{
	return taskType;
}

int Task::getID() const
{
	return taskID;
}

std::string Task::getName() const
{
	return taskName;
}

Taskset& Task::getSubTasks()
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

void Task::addSubTask(Task* task)
{
	subTasks.insert(TaskHelper::addTask(task));
}

void Task::cleanSubTasks()
{
	for (auto it = subTasks.begin(); it != subTasks.end(); ++it)
		TaskHelper::removeTask(*it);
	subTasks.clear();
}

void Task::updateTaskTree()
{	
	if (subTasks.size() > 0)
	{
		for (auto it = subTasks.begin(); it != subTasks.end(); ++it)
		{
			if (!(*it)->isComplete())
				(*it)->updateTaskTree();
			else
				TaskHelper::removeTask(*it);
		}
	}
	update();
}

void Task::succeed()
{
	complete = true;
	profit = 1.0;
	printDebugInfo("Success!", true);
	
	if(taskType != CRC)
		CoalitionHelper::removeCoalition(coalition);
	//TaskHelper::removeTask(this);
}

void Task::fail()
{	
	complete = true;
	profit = 0.0;
	printDebugInfo("Failure!", true);

	if (taskType != CRC)
		CoalitionHelper::removeCoalition(coalition);
	//TaskHelper::removeTask(this);
}

void Task::printDebugInfo(std::string info, bool forceShow)
{
	if(debug || forceShow)
		std::cout << taskName << " : " << taskID << " : " << info << "\n";
}