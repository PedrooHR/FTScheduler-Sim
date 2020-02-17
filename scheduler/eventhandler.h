#pragma once

#include "scheduler.h"
#include <vector>
#include <queue>
#include <fstream>

extern long long int GLOBAL_TIMER;

//Events Definition
#define EVENT_CHECKPOINT    1   // when a machine will execute a checkpoint 
#define EVENT_FINISHTASK    2   // when a machine finishes a task
#define EVENT_TASKSTART     3   // when a machine will start a task
#define EVENT_FAULT         4   // when a fault occurs
#define EVENT_NETWORKREPAIR 5   // when the network fault will be recovered
#define EVENT_SENSORSREPAIR 6   // when the sensors fault will be recovered
#define EVENT_SYSMONITOR    7   // periodically event to monitor system network and sensors
#define EVENT_FAULTDETECTOR 8   // periodically event that detects when a fault has occurred

class Event {
public:
    int type;
    std::string info; 
    int time;
    Event();
} ;

class EventCompareFunction
{
public:
    bool operator() (const Event * left, const Event * right) const //std=c++14 permite uso do auto
    {
        return left->time < right->time; 
    }
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

