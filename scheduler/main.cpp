#include <cstdio>
#include <iostream>
#include "scheduler.h"
#include "eventhandler.h"
#include "fault.h"
#include <string>

using namespace std;

int main(int argc, char * argv[]){
    string JobString;
    
    if (argc == 2){
        JobString.assign(argv[1]);
    } else {
        printf("invalid arguments\n");
        exit(0);
    }

    Scheduler * scheduler = new Scheduler();
    EventHandler * eventhandler = new EventHandler();
    Job * job = new Job(JobString);

    //First Steps
    ReadFaults(JobString); // Add All Event Faults
    scheduler->StartScheduler(); // Start The Scheduler -- Also adds first Monitor event

    while (eventhandler->EventQueue.empty() == false){
        eventhandler->NextEvent();
    }

    return 0;
}
