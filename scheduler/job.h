#ifndef JOB_H_INCLUDED
#define JOB_H_INCLUDED

/* Job includes all files related to the job, like the dependecy graph, the machines configuration and the probabilistic configuration */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>
#include <string>

extern long int TOTAL_TASK_TIME;
extern long int TOTAL_REDUNDANCY;

//Machines Order Definition
#define MRLP    1
#define MRMP    2

//Machines Status Definition
#define MACHINE_DOWN        1
#define MACHINE_AVAILABLE   2
#define MACHINE_BUSY        3

//Machines Type Definition
#define MACHINE_NORMAL  1
#define MACHINE_SPOT    2   
#define SPOT_DELAY      60

//Machines HDD Speed
#define HDD_WRITE_SPEED 500

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

//Tasks Status Definition
#define TASK_NOTREADY   0
#define TASK_READY      1
#define TASK_RUNNING    2
#define TASK_COMPLETED  3 

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
    std::vector <Machines *> Instances;
    
    //Task Constructors
    Task();
};

/* maintain machine and tasks, and methods relative to scheduling */
class Job {
public:
    std::vector <Task *> G; //Dependency Graph
    std::vector <Machine *> Machines;
    std::vector <Machine *> Machines_S;
    std::vector <std::pair <int, int> > faults;
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


#endif // JOB_H_INCLUDED
