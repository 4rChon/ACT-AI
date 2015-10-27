#include "Task.h"
#include "Globals.h"

Task::Task()
{
	taskType = NON;
	taskName = "Task";
	complete = false;
	assigned = false;
	acting = false;
	g_Tasks.push_back(this);
}

void Task::act()
{
	acting = true;
}

void Task::update()
{
	int completeCount = 0;
	//std::cout << "Task: Updating\n";
	for (auto task : subTasks)
		if (task->isComplete())
			completeCount++;
	
	if (subTasks.size() == completeCount)
	{
		//std::cout << "Task Complete\n";
		complete = true;
		cleanSubTasks();
	}
}

void Task::setCoalition(Coalition* coalition)
{
	this->coalition = coalition;
}

bool Task::isAssigned() const
{
	return assigned;
}

bool Task::isComplete() const
{
	return complete;
}

std::list<Task*> Task::getSubTasks() const
{
	//std::cout << "Task: Getting Subtasks\n";
	return subTasks;
}

TaskType Task::getType() const
{
	return taskType;
}

Coalition* Task::getCoalition() const
{
	return coalition;
}

void Task::cleanSubTasks()
{
	for (auto task : this->subTasks)
	{
		task->cleanSubTasks();
		g_Tasks.remove(task);
	}
}

std::string Task::toString() const
{
	std::cout << "Task: Getting Name\n";
	return taskName;
}