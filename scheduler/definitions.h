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

//////////////////////////////////////////////////////////
// DEFINES 
//////////////////////////////////////////////////////////

//System definitions
#define MONITOR_TIME    5*60            // time in seconds to the system monitor sensors and network
#define FDETECTOR_TIME  1*60            // time in seconds to the system detects if there are machines with fault

//R threshold definitions
#define R_MEAN          1               // calculates R threshold by the mean of all tasks' datainputs size
#define R_MEDIAN        2               // calculates R threshold by the median of all tasks' datainputs size
#define R_LQUARTILE     3               // calculates R threshold by the first quartile of all tasks' datainputs size
#define R_HQUARTILE     4               // calculates R threshold by the third quartile of all tasks' datainputs size

//Machines Order Definition
#define MRLP    1                       // sort machines by more reliability and then by less processing
#define MRMP    2                       // sort machines by more reliability and then by more processing

//Machines Status Definition
#define MACHINE_DOWN        0           // when a machine fails
#define MACHINE_TURNEDOFF   1           // nn case we already now that machine failed
#define MACHINE_AVAILABLE   2           // when a machine is available
#define MACHINE_BUSY        3           // when a machine is executing a task

//Machines Type Definition
#define MACHINE_NORMAL  1               // defines that the machine is standard
#define MACHINE_SPOT    2               // defines that the machines is spot
#define SPOT_DELAY      60              // defines the time to provide a spot machine (in seconds)

//Checkpoint related Definitions
#define MINIMUM_CP_TIME     10          // defines the minimum tima a checkpoint needs to be done
#define MAXIMUM_NUMBER_CP   25          // defines a maximum value a task will be checkpointed
#define HDD_WRITE_SPEED     500         // defines the HDD speed for checkpoints in MB/s, value determined according to AWS 
                                        // general purpose HDD for writing not frequently large files (checkpoints) with less cost

//Tasks Status Definition
#define TASK_NOTREADY   0               // when a task still have dependencies
#define TASK_READY      1               // when a task has no more dependencies and can be executed
#define TASK_RUNNING    2               // when a task is running
#define TASK_COMPLETED  3               // when a task is successfully completed

//Events Definition
#define EVENT_CHECKPOINT    1           // when a machine will execute a checkpoint 
#define EVENT_FINISHTASK    2           // when a machine finishes a task
#define EVENT_TASKSTART     3           // when a machine will start a task
#define EVENT_FAULT         4           // when a fault occurs
#define EVENT_NETWORKREPAIR 5           // when the network fault will be recovered
#define EVENT_SENSORSREPAIR 6           // when the sensors fault will be recovered
#define EVENT_SYSMONITOR    7           // periodically event to monitor system network and sensors
#define EVENT_FAULTDETECTOR 8           // periodically event that detects when a fault has occurred

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
class PQCompare;
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
    int Mspots = 0;             //Number of Spot Machines
    int A = 40, B = 50;         //Bounds of normal Machines
    double R = 10000;           // Input Threshold, maximum size a task can be checkpointed 
    int Theta = 2;              // Theta Input, maximum number of redundancies
    float Gamma = 0.75;         // Reliability Threshold   
} Config;

class Event {
public:
    int type;
    std::string info; 
    int time;
    Event();
};

class PQCompare
{
public:
    bool operator() (const Task * left, const Task * right) const; 
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
    std::priority_queue<int, std::vector<Task *>, PQCompare> P_Queue;
    Job * job;
    Config config;
    int RFactor;
    EventHandler * eventhandler;

    int GetDependents(Task * curr_node, int Mark);
    void Initialize();
    std::vector <Machine *> Provide(int Number);
    int assign(Task * NextTask, std::vector <Machine *> machines);
    std::vector <Machine *> getNextMachine(int Order);
    std::vector <Machine *> getNextMachine(int Order, int MaxInstances);
    void CheckPendingRedundancy();
    void CheckRedundancy();
    int CheckTask(Task * NextTask);
    void RunNextTasks();
    void RCalc(int factor);
    void StartScheduler();

    Scheduler(int factor, EventHandler * handler);
};

class Machine {
public:
    //Unchageable variables
    std::string id; //can be any string
    float cp; //Computing Performance Coefficient
    int type;
    //Changeable variables
    int status;
    long long int StartTime;
    float cs; //Reliability Coefficient
    int TimeToComplete; //in second
    int CurrCheckpoint;
    int TimeBetweenCP;
    Task * TaskExecuting;
    bool FaultSensors;
    float SensorsDiscount;
    bool FaultNetwork;
    float NetworkDiscount; 

    //Machines Constructors
    Machine();
};

/* Task takes all information about tasks */
class Task {
public:
    //Unchangeable variables
    int id;
    int TaskTime; //in second
    int DoD; //Degre of Dependency
    long int S; //Size of Task Input
    int TimeToCheckpoint;
    int NumberOfCheckpoints;
    std::vector <Task *> dependencies;
    std::vector <Task *> dependents; //only the direct ones
    //Changeable variables
    int status; 
    int CurrNumberOfDependencies;
    bool PendingRed;
    bool Checkpointable; //True if task will do checkpoint
    int LastValidCP; //last valid checkpoint
    int auxDependentMapping; // Auxiliary variable for DoD Calculus
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
    std::vector <Machine *> Machines_S;
    int TasksToComplete;
    int MachinesAvailable;
    int NextSpot;
    int NextNormal;

    Task * getTaskByID(int taskid);
    Machine * getMachineByID(std::string machineid);

    void ReadGraph(std::string JobString);
    void ReadMachines(std::string JobString);
    
    Job(std::string JobString);
};

class EventHandler {
public:
    std::ofstream logging;
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
