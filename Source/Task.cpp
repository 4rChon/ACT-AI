#include "Task.h"
#include "Globals.h"

Task::Task()
{
	taskType = NON;
	taskName = "Task";
	complete = false;
	assigned = false;
	acting = false;
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
		subTasks.clear();
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

void Task::cleanSubTasks(std::list<Task*> subTasks)
{
	for (auto task : subTasks)
	{
		cleanSubTasks(task->getSubTasks());
		g_Tasks.remove(task);
	}
}

std::string Task::toString() const
{
	std::cout << "Task: Getting Name\n";
	return taskName;
}