#include <vector>
#include <fstream>
#include "eventhandler.h"

//Faults Definition 
#define FAULT_MACHINEDOWN   1
#define FAULT_SENSORS       2
#define FAULT_NETWORK       3
#define NETWORK_REPAIR      247*60
#define SENSORS_REPAIR      342*60

void ReadFaults(std::string JobString){
    std::ifstream faultsfile(JobString + "_f.cfg");
    std::string line;

    while (getline(faultsfile, line)){
        int time = stoi(line.substr(0, line.find(' ') - 1));
        std::string machine = line.substr(line.find(' '), line.find(' ', line.find(' ')) - 1);
        int type = stoi(line.substr(line.find(' ', line.find(' ')), line.size());

        Event * event = new Event();
        event.type = EVENT_FAULT;
        event.info = machine + " " + type;
        event.time = time;
        EventHandler.AddEvent(event);
    }

}









