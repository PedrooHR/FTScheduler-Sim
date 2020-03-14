#include <cstdio>
#include <iostream>
#include <string>
#include "definitions.h"


using namespace std;

int main(int argc, char * argv[]){
    string JobString;
    int mode;
    int value = 0; //cp interval or max redundancies

    if (argc == 2){
        JobString.assign(argv[1]);
        mode = 2;
    } else if (argc == 4) {
        JobString.assign(argv[1]);
        value = std::stoi(argv[3]);
        mode = std::stoi(argv[2]);
    } else {
        printf("invalid arguments - Specify the JobString, 1 if using redundancy or 0 if cp, and an integer to choose RFactor (1 to 4)\n");
        exit(0);
    }

    //Object Constructors
    EventHandler * eventhandler = new EventHandler();
    Job * job = new Job(JobString);
    Scheduler * scheduler = new Scheduler(eventhandler, mode, value);
    scheduler->config.A = job->MachinesAvailable;
    scheduler->config.B = job->MachinesAvailable;
    //First Steps
    ReadFaults(JobString, eventhandler); // Add All Event Faults
    eventhandler->scheduler = scheduler;
    scheduler->job = job;
    scheduler->StartScheduler(); // Start The Scheduler -- Also adds first Monitor event

    while (eventhandler->EventQueue.empty() == false){
        eventhandler->NextEvent();
    }
    eventhandler->logging.close();

    return 0;
}
