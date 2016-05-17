#include "Task.h"
#include "CoalitionHelper.h"
#include "CompositionHelper.h"
#include "TaskHelper.h"
#include "ArmyHelper.h"
#include "EconHelper.h"
#include "CreateCoalition.h"

Task::Task()
	: taskID(TaskHelper::getNextID())
	, taskName("Task")
	, taskType(NON)
	, complete(false)
	, assigned(false)
	, acting(false)
	, coalitionID(-1)
	, coalition(nullptr)
	, creationFrame(BWAPI::Broodwar->getFrameCount())
	, cost(0.0)
	, profit(0.0)
	, target(nullptr)
	, debug(false)
{
}

Task::~Task()
{
	printDebugInfo("DELETE");
	if (taskType != CRC && taskType != STR && taskType != SUR && taskType != NON && coalition != nullptr)
	{
		CoalitionHelper::removeCoalition(coalition);
	}

	for (auto &superTaskIt = superTasks.begin(); superTaskIt != superTasks.end(); ++superTaskIt)
	{
		printDebugInfo("\n\tRemoving sub task: " + taskName + " \n\tfrom super task: " + (*superTaskIt)->getName());
		(*superTaskIt)->getSubTasks().erase(this);
	}
}

void Task::setDebug(bool debug)
{
	this->debug = debug;
}

void Task::setCoalition(Coalition* coalition)
{
	this->coalition = coalition;
	coalitionID = coalition->getID();
}

Zone* Task::getTarget()
{
	return target;
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

	return cost;
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

void Task::deleteSubTasks()
{
	TaskHelper::deleteTaskTree(subTasks);
}

void Task::createCoalition()
{
	Composition c = CompositionHelper::getComposition(this);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
}

void Task::succeed()
{
	complete = true;
	profit = 1.0;
	printDebugInfo("Success!", true);
	
	deleteSubTasks();
}

void Task::fail()
{		
	complete = true;
	profit = 0.0;
	printDebugInfo("Failure!", true);

	if (taskType == SCO)
	{
		ArmyHelper::stopScouting();
	}	
	
	deleteSubTasks();
}

void Task::printDebugInfo(std::string info, bool forceShow)
{
	if(debug || forceShow)
		std::cout << taskName << " : " << taskID << " : " << info << "\n";
}

std::string Task::getTypeString()
{
	switch (taskType)
	{
	case DEF:
		return "DEF";
	case ATT:
		return "ATT";
	case CRC:
		return "CRC";
	case CRU:
		return "CRU";
	case EXP:
		return "EXP";
	case RES:
		return "RES";
	case STR:
		return "STR";
	case SUR:
		return "SUR";
	case SCO:
		return "SCO";
	default:
		return "NON";
	}
}