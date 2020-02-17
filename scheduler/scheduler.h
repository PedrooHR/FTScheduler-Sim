#pragma once

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <cmath>
#include <string>
#include <algorithm>
#include "eventhandler.h"
#include "job.h"

//System definitions
#define MONITOR_TIME    5*60    // time in seconds to the system monitor sensors and network
#define FDETECTOR_TIME  1*60    // time in seconds to the system detects if there are machines with fault

//R threshold definitions
#define R_MEAN          1       // calculates R threshold by the mean of all tasks' datainputs size
#define R_MEDIAN        2       // calculates R threshold by the median of all tasks' datainputs size
#define R_LQUARTILE     3       // calculates R threshold by the first quartile of all tasks' datainputs size
#define R_HQUARTILE     4       // calculates R threshold by the third quartile of all tasks' datainputs size

typedef struct {
    int Mspots = 0;             //Number of Spot Machines
    int A = 40, B = 50;         //Bounds of normal Machines
    float R = 10000;            // Input Threshold, maximum size a task can be checkpointed 
    int Theta = 2;              // Theta Input, maximum number of redundancies
    float Gamma = 0.75;         // Reliability Threshold   
} Config;

class PQCompare
{
public:
    bool operator() (const Task * left, const Task * right) const //std=c++14 permite uso do auto
    {
        return left->DoD < right->DoD;
    }
};

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
    void CheckTask(Task * NextTask);
    void RunNextTasks();
    void RCalc(int factor);
    void StartScheduler();

    Scheduler(int factor, EventHandler * handler);
};
