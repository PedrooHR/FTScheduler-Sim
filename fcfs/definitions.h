#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <string>
#include <cmath>
#include <time.h>
#include <bits/stdc++.h> 

//////////////////////////////////////////////////////////
// DEFINES 
//////////////////////////////////////////////////////////

//System definitions
#define FDETECTOR_TIME      1*60        // time in seconds to the system detects if there are machines with fault
#define CHECKPOINT_MODE     0           // Defines that checkpoint mode is activated
#define REDUNDANCY_MODE     1           // Defines redundancy redundancy mode is activated
#define DEFAULT_MODE        2           // No FT Mode activated
#define TASK_SCALING_TIME   1           // defines a factor to scale tasks time (hypothesis)
#define TASK_SCALING_SIZE   1           // defines a factor to scale tasks size (hypothesis)

//Machines Status Definition
#define MACHINE_DOWN        0           // when a machine fails
#define MACHINE_TURNEDOFF   1           // nn case we already now that machine failed
#define MACHINE_AVAILABLE   2           // when a machine is available
#define MACHINE_BUSY        3           // when a machine is executing a task

//Checkpoint related Definitions
#define MINIMUM_CP_TIME     10          // defines the minimum tima a checkpoint needs to be done
#define HDD_WRITE_SPEED     40          // defines the HDD speed for checkpoints in MB/s, value determined according to AWS 
                                        // general purpose HDD for writing not frequently large files (checkpoints) with less cost
#define DATA_TRANSFER_RATE  640         // data transfer rate https://aws.amazon.com/pt/blogs/aws/the-floodgates-are-open-increased-network-bandwidth-for-ec2-instances/                                        

//Tasks Status Definition
#define TASK_NOTREADY       0           // when a task still have dependencies
#define TASK_READY          1           // when a task has no more dependencies and can be executed
#define TASK_RUNNING        2           // when a task is running
#define TASK_COMPLETED      3           // when a task is successfully completed

//Events Definition
#define EVENT_CHECKPOINT    1           // when a machine will start to execute a checkpoint 
#define EVENT_FINISHTASK    2           // when a machine finishes a task
#define EVENT_TASKSTART     3           // when a machine will start a task, receving the data
#define EVENT_FAULT         4           // when a fault occurs
#define EVENT_NETWORKREPAIR 5           // when the network fault will be recovered (this not affect FCFS Sim)
#define EVENT_SENSORSREPAIR 6           // when the sensors fault will be recovered (this not affect FCFS Sim)
#define EVENT_FAULTDETECTOR 7           // periodically event that detects when a fault has occurred
#define EVENT_STARTCOMPUTE  8           // starting to compute after receiving data
#define EVENT_SENDDATA      9           // sends final data to head node
#define EVENT_FINISHCHECKPOINT 10           // finish checkpoint process


//Faults Definition 
#define FAULT_MACHINEDOWN   1           // when a fault occurs and machine goes down
#define FAULT_SENSORS       2           // when a change in sensors occurs on a machine
#define FAULT_NETWORK       3           // when a change in network occurs on a machine
#define NETWORK_REPAIR      247*60      // avg time till repair network, accordingly to article
#define SENSORS_REPAIR      342*60      // avg time till repair sensors, accordingly to article


//////////////////////////////////////////////////////////
// FOWARD CLASSES DEFINITION 
//////////////////////////////////////////////////////////

class Event;
class EventCompareFunction;
class Scheduler;
class Machine;
class Task;
class Job;
class EventHandler;

//////////////////////////////////////////////////////////
// ADDITIONAL STRUCTURES AND FUNCTIONS DEFINITION 
//////////////////////////////////////////////////////////

typedef struct {
    int A = 40, B = 50;         //Bounds of Machines
} Config;

class Event {
public:
    int type;
    std::string info; 
    int time;
    Event();
};

class EventCompareFunction
{
public:
    bool operator() (const Event * left, const Event * right) const;
};

//////////////////////////////////////////////////////////
// CLASSES PROTOTYPES 
//////////////////////////////////////////////////////////

class Scheduler
{
public:
    std::vector <Task *> RunningTasks;
    std::queue <Task *> ReadyQueue;
    Job * job;
    Config config;
    EventHandler * eventhandler;
    int CPRed; // flex variable, can be CP interval or number of Redundancy 
    int Mode; // Checkpoint or Redundancy

    void Initialize();
    void assign(Task * NextTask, std::vector <Machine *> machines);
    std::vector <Machine *> getNextMachine(int Quantity);
    int CheckTask(Task * NextTask);
    void RunNextTasks();
    void StartScheduler();

    Scheduler(EventHandler * handler, int Mode, int value);
};

class Machine {
public:
    //Unchageable variables
    std::string id; //can be any string
    float cp;
    //Changeable variables
    int status;
    long long int StartTime;
    int TimeToComplete; //in second
    Task * TaskExecuting;
    int TimeBetweenCP;
    long int CurrCheckpoint;
    long int ComputedTime;

    //Machines Constructors
    Machine();
};

/* Task takes all information about tasks */
class Task {
public:
    //Unchangeable variables
    int id;
    int TaskTime; //in second
    std::vector <Task *> dependencies;
    std::vector <Task *> dependents; //only the direct ones
    int TimeToCheckpoint;
    long int S;
    //Changeable variables
    int status; 
    int CurrNumberOfDependencies;
    int LastValidCP; //last valid checkpoint
    std::vector <Machine *> Instances;
    long long int StartTime;
    
    //Task Constructors
    Task();
};

/* maintain machine and tasks, and methods relative to scheduling */
class Job {
public:
    std::vector <Task *> G; //Dependency Graph
    std::vector <Machine *> Machines;
    int TasksToComplete;
    int MachinesAvailable;
    int NextNormal;

    Task * getTaskByID(int taskid);
    Machine * getMachineByID(std::string machineid);

    void ReadGraph(std::string JobString);
    void ReadMachines(std::string JobString);
    
    Job(std::string JobString);
};

class EventHandler {
public:
    std::ofstream logging, compiled;
    std::priority_queue<int, std::vector<Event *>, EventCompareFunction> EventQueue; 
    Scheduler * scheduler;

    void AddEvent(Event * event);
    void NextEvent();
    
    EventHandler();
};

//////////////////////////////////////////////////////////
// ADDITIONAL FUNCTION AND VARIABLES DEFINITION 
//////////////////////////////////////////////////////////

void ReadFaults(std::string JobString, EventHandler * eventhandler);
extern long long int GLOBAL_TIMER;
