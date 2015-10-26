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
	Coalition* coalition;	
public:
	//constructors
	Task();
	
	//setters
	virtual void setCoalition(Coalition* coalition);

	//getters
	virtual bool isAssigned() const;
	virtual bool isComplete() const;
	virtual std::list<Task*> getSubTasks() const;
	virtual TaskType getType() const;
	virtual Coalition* getCoalition() const;

	//-
	virtual void assign() = 0;
	virtual void act() = 0;
	virtual void update();
	virtual void cleanSubTasks(std::list<Task*> subTasks);
	virtual std::string toString() const;

};