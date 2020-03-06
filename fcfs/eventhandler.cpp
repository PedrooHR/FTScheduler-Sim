#include "definitions.h"

long long int GLOBAL_TIMER = 0;

bool EventCompareFunction::operator() (const Event * left, const Event * right) const //std=c++14 permite uso do auto
{
    return left->time > right->time; 
}

Event::Event(){
    
}

EventHandler::EventHandler() {
    logging.open("EventLog.txt");
    logging.close();
    logging.open("EventLog.txt");
}

void EventHandler::AddEvent(Event * event) {
    EventQueue.push(event);
}

void EventHandler::NextEvent() {
    Event * CurrentEvent = EventQueue.top(); //get the next event 
    EventQueue.pop(); //remove event from queue;

    if (scheduler->job->TasksToComplete <= 0){
        EventQueue = std::priority_queue<int, std::vector<Event *>, EventCompareFunction>();
        compiled.open("compiled.txt", std::ofstream::app);
        compiled << GLOBAL_TIMER << " ";
        compiled.close();
        logging.close();
        return;
    }
    
    switch (CurrentEvent->type) {
        case EVENT_CHECKPOINT: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            int prev = 0; int curr = eventinfo.find(' ');
            int taskid = std::stoi(eventinfo.substr(prev, curr - prev));
            prev = curr + 1; curr = eventinfo.find(' ', prev);
            std::string machineid = eventinfo.substr(prev, curr - prev);
            Task * task = scheduler->job->getTaskByID(taskid); 
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            if (machine->status == MACHINE_BUSY && machine->TaskExecuting->id == task->id) {
                task->LastValidCP++;
                machine->TimeToComplete += task->TimeToCheckpoint; // Add the time to make the CP to the total execution time
                
                Event * event = new Event();
                if ((machine->TimeToComplete - (GLOBAL_TIMER + machine->StartTime)) > scheduler->CPRed){ // lets count the CP interval only after finishing this CP
                    event->type = EVENT_CHECKPOINT;
                    event->info = std::to_string(task->id) + " " + machine->id + " ";
                    event->time = task->TimeToCheckpoint + GLOBAL_TIMER + scheduler->CPRed;
                } else {
                    event->type = EVENT_FINISHTASK;
                    event->info = std::to_string(task->id) + " " + machine->id + " ";
                    event->time = machine->StartTime + machine->TimeToComplete;
                }
                AddEvent(event);
                logging << "event=CHECKPOINT id=" << taskid << " machine=" << machineid << " status=DONECP_" << task->LastValidCP << " time=" << GLOBAL_TIMER << "\n";
            }
            break;
        }
        case EVENT_FINISHTASK: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            int prev = 0; int curr = eventinfo.find(' ');
            int taskid = std::stoi(eventinfo.substr(prev, curr - prev));
            prev = curr + 1; curr = eventinfo.find(' ', prev);
            std::string machineid = eventinfo.substr(prev, curr - prev);
            Task * task = scheduler->job->getTaskByID(taskid); 
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            if (machine->status == MACHINE_BUSY && machine->TaskExecuting->id == task->id) {
                
                task->status = TASK_COMPLETED;
            
                for (int i = 0; i < scheduler->RunningTasks.size(); i++){ //Remove The Already Completed Tasks From Running Ones
                    if (scheduler->RunningTasks[i]->status == TASK_COMPLETED){
                        scheduler->RunningTasks.erase(scheduler->RunningTasks.begin() + i);
                    }
                }

                scheduler->job->TasksToComplete--;
                std::cout << "Tasks to go: " << scheduler->job->TasksToComplete << "\n";
                //Release the Machines;
                for (int i = 0; i < task->Instances.size(); i++){
                    task->Instances[i]->status = MACHINE_AVAILABLE;
                    task->Instances[i]->TaskExecuting = NULL;
                    scheduler->job->MachinesAvailable++;
                }

                //Check other Tasks
                for (int i = 0; i < task->dependents.size(); i++){
                    task->dependents[i]->CurrNumberOfDependencies--;
                    if (task->dependents[i]->CurrNumberOfDependencies <= 0 && task->dependents[i]->status == TASK_NOTREADY){
                        task->dependents[i]->status = TASK_READY;
                        scheduler->ReadyQueue.push(task->dependents[i]);
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
            int prev = 0; int curr = eventinfo.find(' ');
            int taskid = std::stoi(eventinfo.substr(prev, curr - prev));
            prev = curr + 1; curr = eventinfo.find(' ', prev);
            std::string machineid = eventinfo.substr(prev, curr - prev);
            Task * task = scheduler->job->getTaskByID(taskid); 
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            if (machine->status == MACHINE_BUSY && machine->TaskExecuting->id == task->id){

                if (task->StartTime == -1){
                    task->StartTime = GLOBAL_TIMER;
                }

                if (scheduler->Mode == CHECKPOINT_MODE) {
                    machine->StartTime = GLOBAL_TIMER;
                    machine->TimeToComplete = (task->TaskTime / machine->cp) - (scheduler->CPRed * task->LastValidCP); // Although machines are different, this is a good approximation to define time left
                    Event * event = new Event();
                    if (machine->TimeToComplete > scheduler->CPRed){
                        event->type = EVENT_CHECKPOINT;
                        event->info = std::to_string(task->id) + " " + machine->id + " ";
                        event->time = machine->StartTime + scheduler->CPRed;   
                    } else {
                        event->type = EVENT_FINISHTASK;
                        event->info = std::to_string(task->id) + " " + machine->id + " ";
                        event->time = machine->StartTime + machine->TimeToComplete;
                    }
                    AddEvent(event);
                } else {
                    machine->StartTime = GLOBAL_TIMER; 
                    machine->TimeToComplete = task->TaskTime / machine->cp;
                    Event * event = new Event();
                    event->type = EVENT_FINISHTASK;
                    event->info = std::to_string(task->id) + " " + machineid + " ";
                    event->time = machine->StartTime + machine->TimeToComplete;
                    AddEvent(event);
                }

                logging << "event=TASKSTART " << "id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
            }
            break;
        }
        case EVENT_FAULT: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string eventinfo = CurrentEvent->info;
            int prev = 0; int curr = eventinfo.find(' ');
            std::string machineid = eventinfo.substr(prev, curr - prev);
            prev = curr + 1; curr = eventinfo.find(' ', prev);
            int type = std::stoi(eventinfo.substr(prev, curr - prev));
            
            Machine * machine = scheduler->job->getMachineByID(machineid);

            if (machine == NULL){ // System Hasn't discovered to launch a new one, this event should occur after FDETECTOR_TIME
                Event * event = new Event();
                event->type = CurrentEvent->type;
                event->info = CurrentEvent->info;
                event->time = CurrentEvent->time + FDETECTOR_TIME; 
                AddEvent(event);
                break;              
            }

            switch (type){
                case FAULT_MACHINEDOWN: {
                    machine->status = MACHINE_DOWN;
                    logging << "event=FAULT " << " machine=" << machineid << " type=MACHINEDOWN time=" << GLOBAL_TIMER << "\n";
                    break;
                }
                case FAULT_SENSORS: {
                    /*Event * repair = new Event();
                    repair->type = EVENT_SENSORSREPAIR;
                    repair->info = machineid;
                    repair->time = GLOBAL_TIMER + SENSORS_REPAIR;
                    AddEvent(repair);*/
                    logging << "event=FAULT " << " machine=" << machineid << " type=SENSORS time=" << GLOBAL_TIMER << "\n";
                    break;
                }
                case FAULT_NETWORK: {
                    /*Event * repair = new Event();
                    repair->type = EVENT_NETWORKREPAIR;
                    repair->info = machineid;
                    repair->time = GLOBAL_TIMER + NETWORK_REPAIR;
                    AddEvent(repair);*/
                    logging << "event=FAULT machine=" << machineid << " type=NETWORK time=" << GLOBAL_TIMER << "\n";
                    break;
                }
            }
            break;
        }
        case EVENT_NETWORKREPAIR: {  // This should not affect FCFS att all
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;
            std::string eventinfo = CurrentEvent->info;
            int prev = 0; int curr = eventinfo.find(' ');
            std::string machineid = eventinfo.substr(prev, curr - prev);

            logging << "event=NETWORKREPAIR " << " machine=" << machineid << " type=NETWORK time=" << GLOBAL_TIMER << "\n";   
            break;
        }
        case EVENT_SENSORSREPAIR: { // This should not affect FCFS att all
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;
            std::string eventinfo = CurrentEvent->info;
            int prev = 0; int curr = eventinfo.find(' ');
            std::string machineid = eventinfo.substr(prev, curr - prev);

            logging << "event=SENSORSREPAIR " << " machine=" << machineid << " type=SENSORS time=" << GLOBAL_TIMER << "\n";     
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
                    if (machines[i]->TaskExecuting == NULL)
                        scheduler->job->MachinesAvailable--;

                    // Check if we are in the minimum value on standard machines
                    if (scheduler->job->MachinesAvailable < scheduler->config.A){ // Creates a new machine if below the minimum value
                        Machine * newMachine = new Machine();
                        
                        newMachine->id = "MACHINE_" + std::to_string(scheduler->job->NextNormal);
                        newMachine->status = MACHINE_AVAILABLE;
                        newMachine->StartTime = 0;
                        newMachine->cp = (80 + (rand() % 20)) / 100.0;
                        newMachine->TimeToComplete = 0; 
                        newMachine->TaskExecuting = NULL;
                        scheduler->job->Machines.push_back(newMachine);
                        scheduler->job->NextNormal++;
                        scheduler->job->MachinesAvailable++;
                        scheduler->RunNextTasks();
                    }

                    // About the task
                    Task * task = machines[i]->TaskExecuting;
                    if (task != NULL) { // If task is NULL, means that the machine was idle
                        int machinesup = 0;
                        for (int j = 0; j < task->Instances.size(); j++) { // count all machines that are busy (executing the task)
                            if (task->Instances[j]->status == MACHINE_BUSY){
                                machinesup++;
                            }
                        }
                        if (machinesup == 0) { // if there are no more machines executing the task
                            // Reset the task variables
                            task->status = TASK_READY;
                            task->StartTime = -1;
                            task->Instances.resize(0);
                            // Add to queue again, if has a valid checkpoint, it will be start from the last one
                            scheduler->ReadyQueue.push(task);     
                            scheduler->RunNextTasks();
                            logging << "event=FAULTDETECT " << " machine=" << machines[i]->id << " type=TASKRESCHEDULED_" << task->id << " time=" << GLOBAL_TIMER << "\n";                           
                        } 
                   }
                }
            }

            // Event for the next check
            Event * event = new Event();
            event->type = EVENT_FAULTDETECTOR;
            event->time = CurrentEvent->time + FDETECTOR_TIME; 
            if (scheduler->job->TasksToComplete > 0)
                AddEvent(event);
            break;
        }
    } 
}

