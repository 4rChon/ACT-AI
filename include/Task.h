#pragma once
#include <BWAPI.h>
#include <list>
#include <string>

#include "Coalition.h"

typedef BWAPI::SetContainer < Task*, std::hash<void*>> Taskset;

class Task
{
protected:
	std::string taskName; //debug purposes
	Taskset subTasks;
	bool assigned;
	bool complete;
	bool acting;
	int creationFrame;
	int taskID;
	double cost;
	double profit;
	Coalition* coalition;
	int coalitionID;
public:
	//constructors and destructors
	Task();
	virtual ~Task();

	//setters
	virtual void setCoalition(Coalition* coalition);
	virtual void setAssigned(bool assigned);
	virtual void setActing(bool acting);
	virtual void setComplete(bool complete);

	//getters
	virtual bool isAssigned() const;
	virtual bool isActing() const;
	virtual bool isComplete() const;
	virtual int getID() const;
	virtual Taskset getSubTasks() const;
	virtual Coalition* getCoalition() const;
	virtual double getCost();
	virtual double getProfit();

	//-
	virtual void assign() = 0;
	virtual void act() = 0;
	virtual void update() = 0;
	virtual void cleanSubTasks();
	virtual void addSubTask(Task* task);
	virtual std::string toString() const;

};