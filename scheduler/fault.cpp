#include "definitions.h"

void ReadFaults(std::string JobString, EventHandler * eventhandler){
    std::ifstream faultsfile(JobString + "_f.cfg");
    std::string line;

    while (getline(faultsfile, line)){
        int time = stoi(line.substr(0, line.find(' ') - 1));
        std::string machine = line.substr(line.find(' '), line.find(' ', line.find(' ')) - 1);
        std::string type = line.substr(line.find(' ', line.find(' ')), line.size());

        Event * event = new Event();
        event->type = EVENT_FAULT;
        event->info = machine + " " + type;
        event->time = time;
        eventhandler->AddEvent(event);
    }

}
