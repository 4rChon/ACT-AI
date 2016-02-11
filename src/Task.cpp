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
	return this->taskName;
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
	return taskName;
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