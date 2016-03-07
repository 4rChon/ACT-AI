#include "Task.h"
#include "CoalitionHelper.h"
#include "TaskHelper.h"
#include "ArmyHelper.h"
#include "EconHelper.h"
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
	//debug = true;
	taskType = NON;
}

Task::~Task()
{
	printDebugInfo("DELETE", true);
	if (taskType != CRC && taskType != STR && taskType != SUR && taskType != NON && coalition != nullptr)
	{
		CoalitionHelper::removeCoalition(coalition);
		coalition = nullptr;
		coalitionID = -1;
	}

	for (auto &superTaskIt = superTasks.begin(); superTaskIt != superTasks.end(); ++superTaskIt)
	{
		printDebugInfo("\n\tRemoving sub task: " + taskName + " \n\tfrom super task: " + (*superTaskIt)->getName(), true);
		(*superTaskIt)->getSubTasks().erase(this);
	}

	superTasks.clear();
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

Taskset& Task::getSuperTasks()
{
	return superTasks;
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
	auto newTask = TaskHelper::addTask(task);
	subTasks.insert(newTask);
	newTask->addSuperTask(this);
}

void Task::addSuperTask(Task* task)
{
	superTasks.insert(task);
}

void Task::cleanSubTasks()
{
	for (auto &taskIt = subTasks.begin(); taskIt != subTasks.end();)
	{
		(*taskIt)->getSuperTasks().erase(this);
		printDebugInfo("\n\tRemoving sub task: " + (*taskIt)->getName() + " \n\tfrom super task: " + this->getName(), true);	
		TaskHelper::removeTask(*taskIt++);
	}
	subTasks.clear();
}

void Task::updateTaskTree()
{	
	if (subTasks.size() > 0)
	{		
		for (auto &it = subTasks.begin(); it != subTasks.end();)
		{
			if (!(*it)->isComplete())
				(*it++)->updateTaskTree();
			else
				TaskHelper::removeTask(*it++);
		}
	}
	update();
}

void Task::succeed()
{
	complete = true;
	profit = 1.0;
	printDebugInfo("Success!", true);

	if (taskType == ATT)
	{
		ArmyHelper::defend();
		ArmyHelper::updateTargetPriority();
	}

	if (taskType == SCO)
	{
		ArmyHelper::stopScouting();
	}

	if (taskType == EXP)
	{
		EconHelper::doneExpanding();
	}
	cleanSubTasks();
}

void Task::fail()
{		
	complete = true;
	profit = 0.0;
	printDebugInfo("Failure!", true);
	
	if (taskType == ATT)
	{
		ArmyHelper::defend();
	}

	if (taskType == SCO)
	{
		ArmyHelper::stopScouting();
	}

	if (taskType == EXP)
	{
		EconHelper::doneExpanding();
	}
	cleanSubTasks();
}

void Task::printDebugInfo(std::string info, bool forceShow)
{
	if(debug || forceShow)
		std::cout << taskName << " : " << taskID << " : " << info << "\n";
}