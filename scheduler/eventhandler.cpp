#include "definitions.h"

long long int GLOBAL_TIMER = 0;

bool EventCompareFunction::operator() (const Event * left, const Event * right) const //std=c++14 permite uso do auto
{
    return left->time < right->time; 
}

Event::Event(){
    
}

EventHandler::EventHandler() {
    logging.open("EventLog.txt");
}

void EventHandler:: AddEvent(Event * event) {
    EventQueue.push(event);
}

void EventHandler::NextEvent() {
    Event * CurrentEvent = EventQueue.top(); //get the next event 
    EventQueue.pop(); //remove event from queue;

    switch (CurrentEvent->type) {
        case EVENT_CHECKPOINT: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            int taskid = stoi(eventinfo.substr(0, eventinfo.find(' ') - 1));
            std::string machineid = eventinfo.substr(eventinfo.find(' '), eventinfo.size());
            Task * task = scheduler->job->getTaskByID(taskid); 
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            if (machine->status != MACHINE_DOWN) {
                machine->CurrCheckpoint++;
                if (task->LastValidCP >= machine->CurrCheckpoint) { //if the CP already has been done
                    Event * event = new Event();
                    if (machine->CurrCheckpoint < task->NumberOfCheckpoints){ 
                        event->type = EVENT_CHECKPOINT;
                        event->info = task->id + " " + machine->id;
                        event->time = GLOBAL_TIMER + machine->TimeBetweenCP;
                    } else {
                        event->type = EVENT_FINISHTASK;
                        event->info = task->id + " " + machine->id;
                        event->time = machine->StartTime + machine->TimeToComplete;
                    }
                    AddEvent(event);    
                    logging << "event=CHECKPOINT id=" << taskid << " machine=" << machineid << " status=SKIPCP_" << machine->CurrCheckpoint << " time=" << GLOBAL_TIMER << "\n";
                } else { // if this task will do the cp
                    machine->TimeToComplete += task->TimeToCheckpoint;
                    task->LastValidCP = machine->CurrCheckpoint;
                    Event * event = new Event();
                    if (machine->CurrCheckpoint < task->NumberOfCheckpoints){ 
                        event->type = EVENT_CHECKPOINT;
                        event->info = task->id + " " + machine->id;
                        event->time = task->TimeToCheckpoint + GLOBAL_TIMER + machine->TimeBetweenCP;
                    } else {
                        event->type = EVENT_FINISHTASK;
                        event->info = task->id + " " + machine->id;
                        event->time = machine->StartTime + machine->TimeToComplete;
                    }
                    AddEvent(event);
                    logging << "event=CHECKPOINT id=" << taskid << " machine=" << machineid << " status=DONECP_" << machine->CurrCheckpoint << " time=" << GLOBAL_TIMER << "\n";
                }
            }
            break;
        }
        case EVENT_FINISHTASK: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            int taskid = stoi(eventinfo.substr(0, eventinfo.find(' ') - 1));
            std::string machineid = eventinfo.substr(eventinfo.find(' '), eventinfo.size());
            Task * task = scheduler->job->getTaskByID(taskid); 
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            if (machine->status != MACHINE_DOWN) {
                task->status = TASK_COMPLETED;

                scheduler->job->TasksToComplete--;

                //Release the Machines;
                for (int i = 0; i < task->Instances.size(); i++){
                    if (task->Instances[i]->type == MACHINE_NORMAL){
                        task->Instances[i]->status = MACHINE_AVAILABLE;
                        task->Instances[i]->TaskExecuting = NULL;
                        scheduler->job->MachinesAvailable++;
                    } else {
                        for (int j = 0; j < scheduler->job->Machines_S.size(); j++){
                            if (task->Instances[i]->id.compare(scheduler->job->Machines_S[j]->id) == 0){
                                scheduler->job->Machines_S.erase(scheduler->job->Machines_S.begin() + j);
                                break;
                            }   
                        }
                    }
                }

                //Check other Tasks
                for (int i = 0; i < task->dependents.size(); i++){
                    task->dependents[i]->CurrNumberOfDependencies--;
                    if (task->dependents[i]->CurrNumberOfDependencies <= 0){
                        task->dependents[i]->status = TASK_READY;
                        scheduler->P_Queue.push(task->dependents[i]);
                    }
                }

                scheduler->RunNextTasks();    
                logging << "event=FINISHTASK " << "id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
            }
            break;
        }
        case EVENT_TASKSTART: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            int taskid = stoi(eventinfo.substr(0, eventinfo.find(' ') - 1));
            std::string machineid = eventinfo.substr(eventinfo.find(' '), eventinfo.size());
            Task * task = scheduler->job->getTaskByID(taskid); 
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            //Task 
            task->status = TASK_RUNNING;
            task->Instances.push_back(machine);

            //Machine
            machine->status = MACHINE_BUSY;
            machine->TaskExecuting = task;
            scheduler->job->MachinesAvailable--;

            if (task->StartTime == -1){
                task->StartTime = GLOBAL_TIMER;
            }

            if (task->Checkpointable == true) {
                machine->TimeBetweenCP = (task->TaskTime / machine->cp) / task->NumberOfCheckpoints;
                machine->CurrCheckpoint = task->LastValidCP;
                machine->StartTime = GLOBAL_TIMER;
                machine->TimeToComplete = (task->TaskTime / machine->cp) - (machine->TimeBetweenCP * machine->CurrCheckpoint); // Although machines are different, this is a good approximation to define time left
                Event * event = new Event();
                if (machine->CurrCheckpoint < task->NumberOfCheckpoints){ 
                    event->type = EVENT_CHECKPOINT;
                    event->info = task->id + " " + machine->id;
                    event->time = machine->StartTime + machine->TimeBetweenCP;
                } else {
                    event->type = EVENT_FINISHTASK;
                    event->info = task->id + " " + machine->id;
                    event->time = machine->StartTime + machine->TimeToComplete;
                }
                AddEvent(event);
            } else {
                machine->StartTime = GLOBAL_TIMER; 
                machine->TimeToComplete = task->TaskTime / machine->cp;
                Event * event = new Event();
                event->type = EVENT_FINISHTASK;
                event->info = task->id + " " + machineid;
                event->time = machine->StartTime + machine->TimeToComplete;
                AddEvent(event);
            }

            logging << "event=TASKSTART " << "id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
            break;
        }
        case EVENT_FAULT: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            std::string machineid = eventinfo.substr(0, eventinfo.find(' ') - 1);
            int type = stoi(eventinfo.substr(eventinfo.find(' '), eventinfo.size()));
            
            switch (type){
                case FAULT_MACHINEDOWN: {
                    Machine * machine = scheduler->job->getMachineByID(machineid); 
                    machine->status = MACHINE_DOWN;
                    logging << "event=FAULT " << " machine=" << machineid << " type=MACHINEDOWN time=" << GLOBAL_TIMER << "\n";
                    break;
                }
                case FAULT_SENSORS: {
                    Machine * machine = scheduler->job->getMachineByID(machineid); 
                    machine->FaultSensors = true;
                    Event * repair = new Event();
                    repair->type = EVENT_SENSORSREPAIR;
                    repair->info = machineid;
                    repair->time = GLOBAL_TIMER + SENSORS_REPAIR;
                    AddEvent(repair);
                    logging << "event=FAULT " << " machine=" << machineid << " type=SENSORS time=" << GLOBAL_TIMER << "\n";
                    break;
                }
                case FAULT_NETWORK: {
                    Machine * machine = scheduler->job->getMachineByID(machineid); 
                    machine->FaultNetwork = true;
                    Event * repair = new Event();
                    repair->type = EVENT_NETWORKREPAIR;
                    repair->info = machineid;
                    repair->time = GLOBAL_TIMER + NETWORK_REPAIR;
                    AddEvent(repair);
                    logging << "event=FAULT " << " machine=" << machineid << " type=NETWORK time=" << GLOBAL_TIMER << "\n";
                    break;
                }
            }
            break;
        }
        case EVENT_NETWORKREPAIR: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            std::string machineid = eventinfo.substr(0, eventinfo.find(' ') - 1);
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            machine->FaultNetwork = false;
            logging << "event=NETWORKREPAIR " << " machine=" << machineid << " type=NETWORK time=" << GLOBAL_TIMER << "\n";        
            break;
        }
        case EVENT_SENSORSREPAIR: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            std::string machineid = eventinfo.substr(0, eventinfo.find(' ') - 1);
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            machine->FaultSensors = false;
            logging << "event=SENSORSREPAIR " << " machine=" << machineid << " type=SENSORS time=" << GLOBAL_TIMER << "\n";        
            break;
        }
        case EVENT_SYSMONITOR: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            for (int i = 0; i < scheduler->job->Machines.size(); i++){
                Machine * machine = scheduler->job->Machines[i];

                if (machine->FaultNetwork == true){
                    if (machine->NetworkDiscount < 0.5){
                        machine->NetworkDiscount += 0.025;
                        machine->cp -= 0.025;
                        logging << "event=SYSMONITOR " << " machine=" << machine->id << " type=NETWORK_CP_" << machine->cp << " time=" << GLOBAL_TIMER << "\n";        
                    }
                }
                if (machine->FaultSensors == true){
                    if (machine->SensorsDiscount < 0.5){
                        machine->SensorsDiscount += 0.025;
                        machine->cp -= 0.025;
                        logging << "event=SYSMONITOR " << " machine=" << machine->id << " type=SENSORS_CP_" << machine->cp << " time=" << GLOBAL_TIMER << "\n";        
                    }
                }
            }
            
            // Event for next monitoring
            Event * event = new Event();
            event->type = EVENT_SYSMONITOR;
            event->time = GLOBAL_TIMER + MONITOR_TIME; 
            if (scheduler->job->TasksToComplete > 0)
                AddEvent(event);
            break;
        }
        case EVENT_FAULTDETECTOR: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            //We will simplify the Fault Detection here, every 60 seconds, the system will check if any there are new machines down.
            std::vector <Machine *> machines = scheduler->job->Machines;
            for (int i = 0; i < machines.size(); i++){
                if (machines[i]->status == MACHINE_DOWN) {
                    machines[i]->status = MACHINE_TURNEDOFF; //Mark as a verified died machine
                    // Check if we are in the minimum value on standard machines
                    scheduler->job->MachinesUp--;
                    if (scheduler->job->MachinesUp < scheduler->config.A){ // Creates a new machine if below the minimum value
                        Machine * newMachine = new Machine();
                        newMachine->type = MACHINE_NORMAL;
                        newMachine->status = MACHINE_AVAILABLE;
                        newMachine->cs = 1;
                        newMachine->cp = (80 + (rand() % 20)) / 100.0;
                        newMachine->id = "NORMALMACHINE_" + scheduler->job->NextNormal;
                        scheduler->job->Machines.push_back(newMachine);
                        scheduler->job->NextNormal++;
                        scheduler->RunNextTasks();
                    }
                    // About the task
                    Task * task = machines[i]->TaskExecuting;
                    if (task != NULL) { // If task is NULL, means that the machine was idle 
                        int machinesup = 0;
                        for (int j = 0; task->Instances.size(); j++) { // count all machines that are busy (executing the task)
                            if (task->Instances[j]->status == MACHINE_BUSY){
                                machinesup++;
                            }
                        }
                        if (machinesup == 0) { // if there are no more machines executing the task
                            // Reset the task variables
                            task->status = TASK_READY;
                            task->StartTime = -1;
                            task->Instances.clear();
                            // Add to queue again, if has a valid checkpoint, it will be start from the last one
                            scheduler->P_Queue.push(task);     
                            logging << "event=FAULTDETECT " << " machine=" << machines[i]->id << " type=TASKRESCHEDULED_" << task->id << " time=" << GLOBAL_TIMER << "\n";                           
                        }   
                   }
                }
            }

            // Event for the next check
            Event * event = new Event();
            event->type = EVENT_FAULTDETECTOR;
            event->time = GLOBAL_TIMER + FDETECTOR_TIME; 
            if (scheduler->job->TasksToComplete > 0)
                AddEvent(event);
            break;
        }
    }
}

