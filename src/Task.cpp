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
	TaskHelper::addTask(this);
}

Task::~Task()
{
	cleanSubTasks();	
	CoalitionHelper::removeCoalition(coalition);
	delete coalition;
	coalition = nullptr;
	TaskHelper::removeTask(this);
}

void Task::setCoalition(Coalition* coalition)
{
	this->coalition = coalition;
	coalitionID = coalition->getID();
}

void Task::setAssigned(bool assigned)
{
	this->assigned = assigned;
}

void Task::setActing(bool acting)
{
	this->acting = acting;
}

void Task::setComplete(bool complete)
{
	this->complete = complete;
}

bool Task::isAssigned() const
{
	return assigned;
}

bool Task::isActing() const
{
	return acting;
}

bool Task::isComplete() const
{
	return complete;
}

int Task::getID() const
{
	return taskID;
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
	auto &it = subTasks.begin();
	while (it != subTasks.end())
	{
		it = subTasks.erase(it);
		delete(*it);
		//(*it) = nullptr;
	}
	//subTasks.resize(0);
}

void Task::addSubTask(Task* task)
{
	subTasks.insert(task);
}

std::string Task::toString() const
{
	std::cout << "Task: Getting Name\n";
	return this->taskName;
}