#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <cmath>
#include <string>
#include "job.h"

#define MONITOR_TIME    5*60

typedef struct {
    int Mspots = 0;  //Number of Spot Machines
    int A = 40, B = 50; //Bounds of normal Machines
    int R = 10000; // Input Threshold, maximum size a task can be checkpointed 
    int Theta = 2; // Theta Input, maximum number of redundancies
    float Gamma = 0.75; // Reliability Threshold   
} Config;

class PQCompare
{
public:
    bool operator() (const Task * left, const Task * right) const //std=c++14 permite uso do auto
    {
        return left->DoD < right->Dod;
    }
};

class Scheduler
{
public:
    std::vector <Task *> RunningTasks;
    std::priority_queue<int, std::vector<Task *>, PQCompare> P_Queue;
    Job * job;
    Config config;

    int GetDependents(Task * curr_node, int Mark);
    void Initialize();
    std::vector <Machine *> Provide(int Number);
    int assign(Task * NextTask, std::vector <Machine *> machines);
    std::vector <Machine *> getNextMachine(int Order);
    std::vector <Machine *> getNextMachine(int Order, int MaxInstances);
    void CheckPendingRedundancy();
    void CheckRedundancy();
    void CheckTask(Task * NextTask);
    void RunNextTasks();
    void StartScheduler();

    Scheduler();
};

#endif // SCHEDULER_H_INCLUDED