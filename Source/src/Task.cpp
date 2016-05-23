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

	deleteSubTasks();
	removeSuperTasks();
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
	for (auto &taskIt = subTasks.begin(); taskIt != subTasks.end(); ++taskIt)
		TaskHelper::deleteTask(*taskIt);		

	subTasks.clear();
}

void Task::updateSubTasks()
{
	printDebugInfo("Updating Sub Tasks");
	for (auto &taskIt = subTasks.begin(); taskIt != subTasks.end(); ++taskIt)
	{				
		if ((*taskIt)->isComplete())
		{
			TaskHelper::deleteTask(*taskIt);
			continue;
		}

		(*taskIt)->updateSubTasks();
		(*taskIt)->update();
	}
}

void Task::removeSuperTasks()
{
	for (auto &taskIt = superTasks.begin(); taskIt != superTasks.end(); ++taskIt)
		(*taskIt)->removeSubTask(this);

	superTasks.clear();
}

void Task::createCoalition()
{
	printDebugInfo("Create Coalition");
	Composition c = CompositionHelper::getComposition(taskType);
	printDebugInfo("Composition Obtained");
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	printDebugInfo("Create Coalition Task Created");
	addSubTask(createCoalition);
	printDebugInfo("Create Coalition Subtask Added");
}

void Task::succeed()
{
	complete = true;
	profit = 1.0;
	printDebugInfo("Success!", true);	
}

void Task::fail()
{		
	complete = true;
	profit = 0.0;
	printDebugInfo("Failure!", true);

	if (taskType == SCO)
		ArmyHelper::stopScouting();
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