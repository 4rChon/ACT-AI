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
	CoalitionHelper::removeCoalition(coalition);

	for (auto &superTask : superTasks)
		superTask->removeSubTask(this);
}


double Task::getCost()
{
	cost = 0.0;

	for (auto &task : subTasks)
		cost += task->getCost();
	if (coalitionID != -1)
		cost += coalition->getCost();

	return cost;
}

void Task::addSubTask(Task* const& task)
{
	Task* const& newTask = TaskHelper::addTask(task);
	subTasks.insert(newTask);
	newTask->addSuperTask(this);
}

void Task::deleteSubTasks()
{
	for (auto &task : subTasks)
		TaskHelper::deleteTask(task);

	subTasks.clear();
}

void Task::updateSubTasks()
{
	//std::cout << taskTree.size() << "\n";
	for (auto &task : subTasks)
	{
		if (!task)
			continue;

		if (task->isComplete())
		{
			subTasks.erase(task);
			TaskHelper::deleteTask(task);
			continue;
		}

		task->updateSubTasks();
		task->update();
	}
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