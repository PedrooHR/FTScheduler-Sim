#include <cstdio>
#include <iostream>
#include <string>
#include "definitions.h"


using namespace std;

int main(int argc, char * argv[]){
    string JobString;
    
    if (argc == 3){
        JobString.assign(argv[1]);
    } else {
        printf("invalid arguments - Specify the JobString and an integer to choose RFactor (1 to 4)\n");
        exit(0);
    }

    //Object Constructors
    EventHandler * eventhandler = new EventHandler();
    Job * job = new Job(JobString);
    Scheduler * scheduler = new Scheduler(std::stoi(argv[2]), eventhandler);


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
