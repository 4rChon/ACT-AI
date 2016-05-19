#pragma once
#include <BWAPI.h>
#include <list>
#include <string>

#include "Zone.h"
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
	std::string taskName;
	Taskset subTasks;
	Taskset superTasks;
	bool assigned;
	bool complete;
	bool acting;
	bool debug;
	int creationFrame;
	int taskID;
	double cost;
	double profit;
	Coalition* coalition;
	int coalitionID;
	TaskType taskType;
	Zone* target;
public:
	//constructors and destructors
	Task();
	virtual ~Task();	

	//setters
	void setDebug(bool debug)				{ this->debug = debug; }
	void setCoalition(Coalition* coalition)
	{
		this->coalition = coalition;
		coalitionID = coalition->getID();
	}

	//getters
	Zone* getTarget()						{ return target; }
	virtual bool Task::isComplete() const	{ return complete; }
	TaskType getType() const				{ return taskType; }
	int getID() const						{ return taskID; }
	std::string getName() const				{ return taskName; }
	Taskset& getSubTasks()					{ return subTasks; }
	Taskset& getSuperTasks()				{ return superTasks; }
	virtual Coalition* getCoalition() const { return coalition; }
	virtual double getCost();
	
	virtual double getProfit()				{ return profit; }
	void addSubTask(Task* const& task);
	void removeSubTask(Task* const& task)	{ subTasks.erase(task); }
	void addSuperTask(Task* const& task)	{ superTasks.insert(task); }

	//-
	virtual void createCoalition();
	virtual void assign() = 0;
	virtual void act() = 0;
	virtual void update() = 0;

	void deleteSubTasks();
	void updateSubTasks();
	
	virtual void succeed();
	virtual void fail();

	void printDebugInfo(std::string info, bool forceShow = false);
	std::string getTypeString();
};