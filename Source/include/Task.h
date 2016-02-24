#pragma once
#include <BWAPI.h>
#include <list>
#include <string>

#include "Coalition.h"

typedef BWAPI::SetContainer < Task*, std::hash<void*>> Taskset;

enum TaskType
{
	NON,
	DEF,
	ATT,
	CRC,
	CRU,
	EXP,
	RES,
	STR,
	SUR,
	SCO
};

class Task
{
protected:
	std::string taskName; //debug purposes
	Taskset subTasks;
	bool assigned;
	bool complete;
	bool acting;
	bool debug;
	int creationFrame;
	int taskID;
	double cost;
	double profit;
	bool techSatisfied;
	bool unitSatisfied;
	Coalition* coalition;
	int coalitionID;
	TaskType taskType;
public:
	//constructors and destructors
	Task();
	virtual ~Task();

	//setters
	void setCoalition(Coalition* coalition);
	void setUnitSatisfied(bool unitSatisfied);
	void setTechSatisfied(bool techSatisfied);
	

	//getters
	bool isUnitSatisfied() const;
	bool isTechSatisfied() const;
	virtual bool Task::isComplete() const;
	TaskType getType() const;
	int getID() const;
	std::string getName() const;
	Taskset& getSubTasks();
	virtual Coalition* getCoalition() const;
	virtual double getCost();
	virtual double getProfit();
	void addSubTask(Task* task);

	//-
	virtual void assign() = 0;
	virtual void act() = 0;
	virtual void update() = 0;
	void cleanSubTasks();
	void updateTaskTree();

	virtual void succeed();
	virtual void fail();

	void printDebugInfo(std::string info, bool forceShow = false);
};