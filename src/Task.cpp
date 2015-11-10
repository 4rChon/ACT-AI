#include "Task.h"
#include "GlobalSets.h"

Task::Task()
{
	this->taskType = NON;
	this->taskName = "Task";
	this->complete = false;
	this->assigned = false;
	this->acting = false;
	this->coalition = nullptr;
	this->age = BWAPI::Broodwar->getFrameCount();
	g_Tasks.push_back(this);
}

Task::~Task()
{
	this->cleanSubTasks();
	g_Coalitions.erase(this->coalition);
	this->coalition = nullptr;
	g_Tasks.remove(this);
}

void Task::setCoalition(Coalition* coalition)
{
	this->coalition = coalition;
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
	return this->assigned;
}

bool Task::isActing() const
{
	return this->acting;
}

bool Task::isComplete() const
{
	return this->complete;
}

std::list<Task*> Task::getSubTasks() const
{
	//std::cout << "Task: Getting Subtasks\n";
	return this->subTasks;
}

TaskType Task::getType() const
{
	return this->taskType;
}

Coalition* Task::getCoalition() const
{
	return this->coalition;
}

double Task::getCost()
{
	this->cost = 0;
	
	for (auto task : this->subTasks)
		this->cost += task->getCost();
	
	return this->cost;
}

void Task::cleanSubTasks()
{
	std::list<Task*>::iterator it = this->subTasks.begin();
	while (it != this->subTasks.end())
	{		
		delete(*it);
		(*it) = nullptr;
		it = this->subTasks.erase(it);
	}
}

void Task::addSubTask(Task* task)
{
	this->subTasks.push_back(task);
}

std::string Task::toString() const
{
	std::cout << "Task: Getting Name\n";
	return this->taskName;
}