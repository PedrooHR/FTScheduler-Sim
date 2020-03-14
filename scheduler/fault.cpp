#include "definitions.h"

void ReadFaults(std::string JobString, EventHandler * eventhandler){
    std::ifstream faultsfile(JobString + "_f.cfg");
    /* Temporary  std::ifstream faultsfile("faults.cfg"); */
    std::string line;

    while (getline(faultsfile, line)){
        int prev = 0, curr = line.find(' ');
        int time = std::stoi(line.substr(0, curr));
        prev = curr + 1; curr = line.find(' ', prev);
        std::string machine = "NORMALMACHINE_" + line.substr(prev, curr - prev);
        prev = curr + 1; curr = line.find(' ', prev);
        std::string type = line.substr(prev, curr - prev);
        Event * event = new Event();
        event->type = EVENT_FAULT;
        event->info = machine + " " + type + " ";
        event->time = time;
        eventhandler->AddEvent(event);
    }

}
