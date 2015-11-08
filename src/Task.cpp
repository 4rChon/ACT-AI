#include "Task.h"
#include "GlobalSets.h"

Task::Task()
{
	this->taskType = NON;
	this->taskName = "Task";
	this->complete = false;
	this->assigned = false;
	this->acting = false;	
	this->age = BWAPI::Broodwar->getFrameCount();
	g_Tasks.push_back(this);
}

void Task::act()
{
	this->acting = true;
}

void Task::update()
{
	int completeCount = 0;
	//std::cout << "Task: Updating\n";
	for (auto task : this->subTasks)
		if (task->isComplete())
			completeCount++;
	
	if (this->subTasks.size() == completeCount)
	{
		//std::cout << "Task Complete\n";
		this->complete = true;
		cleanSubTasks();
	}
}

void Task::setCoalition(Coalition* coalition)
{
	this->coalition = coalition;
}

bool Task::isAssigned() const
{
	return this->assigned;
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
		(*it)->cleanSubTasks();				
		g_Tasks.remove((*it));
		delete (*it);
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