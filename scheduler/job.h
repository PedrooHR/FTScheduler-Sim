#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <string>

//Machines Order Definition
#define MRLP    1               // sort machines by more reliability and then by less processing
#define MRMP    2               // sort machines by more reliability and then by more processing

//Machines Status Definition
#define MACHINE_DOWN        0   // when a machine fails
#define MACHINE_TURNEDOFF   1   // nn case we already now that machine failed
#define MACHINE_AVAILABLE   2   // when a machine is available
#define MACHINE_BUSY        3   // when a machine is executing a task

//Machines Type Definition
#define MACHINE_NORMAL  1       // defines that the machine is standard
#define MACHINE_SPOT    2       // defines that the machines is spot
#define SPOT_DELAY      60      // defines the time to provide a spot machine (in seconds)

//Machines HDD Speed
#define HDD_WRITE_SPEED 500     // defines the HDD speed for checkpoints in MB/s, value determined according to AWS 
                                // general purpose HDD for writing not frequently large files (checkpoints) with less cost

//Tasks Status Definition
#define TASK_NOTREADY   0       // when a task still have dependencies
#define TASK_READY      1       // when a task has no more dependencies and can be executed
#define TASK_RUNNING    2       // when a task is running
#define TASK_COMPLETED  3       // when a task is successfully completed

// Foward Declaration
class Machine;
class Task;

/* Machines info */
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
    int S; //Size of Task Input
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
    int MachinesAvailable;
    int NextSpot;
    int NextNormal;
    int TasksToComplete;
    int MachinesUp;

    Task * getTaskByID(int taskid);
    Machine * getMachineByID(std::string machineid);

    void ReadGraph(std::string JobString);
    void ReadMachines(std::string JobString);
    
    Job(std::string JobString);
};
