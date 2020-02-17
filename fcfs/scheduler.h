#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <cmath>
#include "job.h"
#include <string>

extern long int GLOBAL_TIMER;

class RunningTask
{
public:
    Task * task;
    Machine * machine;
    RunningTask(){}
};


class Scheduler
{
private:
    std::vector <RunningTask *> running_tasks;
    std::queue<Task *> pqueue;
    Job * job;
    int unsigned_tasks;
    int heartbeat_step;
public:
    FILE * p;

    void createJob(const char *machinePath, const char * tasksPath, const char * faultsPath, int BaseTime, int DesvTime, int BaseMem, std::string lpath);
    void getReadyToQueueTasks();
    void assignMachines();
    void checkCompletedTasks();
    void checkCheckpointing();
    int getRemainingTasks();
    void heartBeat();
    int getNMachines();

    void logRunningTasks();
    void logJob(long int GLOBAL_TIMER);

    Scheduler(int hb_time);
};


#endif // SCHEDULER_H_INCLUDED
