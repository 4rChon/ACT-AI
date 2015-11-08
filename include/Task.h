#pragma once
#include <BWAPI.h>
#include <list>
#include <string>

#include "Coalition.h"
#include "TaskType.h"

class Task
{
protected:
	std::string taskName; //debug purposes
	std::list<Task*> subTasks;
	TaskType taskType;
	bool assigned;
	bool complete;
	bool acting;
	int age;
	double cost;
	Coalition* coalition;	
public:
	//constructors
	Task();
	
	//setters
	virtual void setCoalition(Coalition* coalition);
	virtual void setAssigned(bool assigned);
	virtual void setActing(bool acting);
	virtual void setComplete(bool complete);

	//getters
	virtual bool isAssigned() const;
	virtual bool isActing() const;
	virtual bool isComplete() const;
	virtual std::list<Task*> getSubTasks() const;
	virtual TaskType getType() const;
	virtual Coalition* getCoalition() const;
	virtual double getCost();

	//-
	virtual void assign() = 0;
	virtual void act();
	virtual void update();
	virtual void cleanSubTasks();
	virtual void addSubTask(Task* task);
	virtual std::string toString() const;

};