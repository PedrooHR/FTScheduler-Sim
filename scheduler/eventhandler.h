#include <scheduler.h>
#include <vector>
#include <queue>
#include <fstream>

extern long long int GLOBAL_TIMER;

//Events Definition
#define EVENT_CHECKPOINT    1
#define EVENT_FINISHTASK    2  
#define EVENT_TASKSTART     3 
#define EVENT_FAULT         4
#define EVENT_NETWORKREPAIR 5
#define EVENT_SENSORSREPAIR 6
#define EVENT_SYSMONITOR    7

class Event {
public:
    int type;
    std::string info; 
    int time;
    Event();
} ;

bool EventCompareFunction(Event * left, Event * right) { 
    return left->time < right->time; 
}

class EventHandler {
public:
    std::ofstream logging("EventLog.txt");
    std::priority_queue<int, std::vector<Event *>, EventCompareFunction> EventQueue; 

    void AddEvent(Event * event);
    void NextEvent();
    
    EventHandler();
};














