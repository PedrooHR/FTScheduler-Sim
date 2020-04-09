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
                //first step to start a CP is to define what was calculated up to this point
                int ComputedTime = GLOBAL_TIMER - machine->StartTime; //actual time - last time machine started the computing
                machine->ComputedTime += ComputedTime; // add the last computed time to the total computed time
                machine->TimeToComplete -= ComputedTime; // discount computed time from the time remaining

                //there are two options, machine will do or not do the CP
                Event * event = new Event();
                event->type = EVENT_FINISHCHECKPOINT;
                event->info = std::to_string(task->id) + " " + machineid + " ";
                //to CP, computed time need to be over than last valid CP by at least the time it will spend doing the CP
                if ((machine->ComputedTime * machine->cp + task->TimeToCheckpoint) < task->LastValidCP) {
                    event->time = GLOBAL_TIMER + 1; // so task will not make the CP, 1 is just to guarantee
                    logging << "event=CHECKPOINT id=" << taskid << " machine=" << machineid << " status=SKIPCP_" << machine->ComputedTime * machine->cp << " time=" << GLOBAL_TIMER << "\n";
                } else {
                    event->time = GLOBAL_TIMER + task->TimeToCheckpoint; // if doing the CP, wait for the time to CP
                    logging << "event=CHECKPOINT id=" << taskid << " machine=" << machineid << " status=DONECP_" << machine->ComputedTime * machine->cp << " time=" << GLOBAL_TIMER << "\n";
                }

                AddEvent(event);
            }
            break;
        }
        case EVENT_FINISHCHECKPOINT: {
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
                //first step is to mark the checkpoint as done
                if (machine->ComputedTime * machine->cp > task->LastValidCP) { // if the curr machine time is higher than last valid task cp
                    task->LastValidCP = machine->ComputedTime * machine->cp;
                }
                //calculate next interval
                int TimeToNextCP = sqrt(task->TimeToCheckpoint * (task->TimeToCheckpoint + (task->TimeToCheckpoint + LAZY_MTBF) / LAZY_ERROR)); //sqrt(b² + b*y/e + M*b/e) where b = y
                int LazyTimeToNextCP = std::max(TimeToNextCP * pow((GLOBAL_TIMER - scheduler->LastFailure) / TimeToNextCP, 1 - LAZY_K_FACTOR), (double)TimeToNextCP); // I've included max(oci, lazy) for when Global - Last Failure are close to 0
                machine->StartTime = GLOBAL_TIMER;
                machine->TimeBetweenCP = LazyTimeToNextCP;
                machine->CurrCheckpoint = machine->ComputedTime * machine->cp;

                // start computing again
                machine->StartTime = GLOBAL_TIMER;

                if (machine->TimeBetweenCP < machine->TimeToComplete){  // if task will do an CP
                    //©reate the next CP Event 
                    Event * event = new Event();
                    event->type = EVENT_CHECKPOINT;
                    event->info = std::to_string(task->id) + " " + machine->id + " ";
                    event->time = machine->StartTime + machine->TimeBetweenCP;   
                    AddEvent(event);
                } else { // if task will finish
                    Event * event = new Event();
                    event->type = EVENT_SENDDATA;
                    event->info = std::to_string(task->id) + " " + machine->id + " ";
                    event->time = machine->StartTime + machine->TimeToComplete;   
                    AddEvent(event);
                }

                logging << "event=FINISHCHECKPOINT id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
            }
            break;
        }
        case EVENT_SENDDATA: {
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
                //at this point we will finish the task and send the data to head node
                
                //calculate transfer rate
                int TransferTime = task->S / DATA_TRANSFER_RATE;

                Event * event = new Event();
                event->type = EVENT_FINISHTASK;
                event->info = std::to_string(task->id) + " " + machineid + " ";
                event->time = GLOBAL_TIMER + TransferTime;
                AddEvent(event);

                logging << "event=SENDDATA " << "id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
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
                if (scheduler->job->TasksToComplete % 5000 == 0)
                    std::cout << "Tasks to go: " << scheduler->job->TasksToComplete << "\n";
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
                    if (task->dependents[i]->CurrNumberOfDependencies <= 0 && task->dependents[i]->status == TASK_NOTREADY){
                        task->dependents[i]->status = TASK_READY;
                        scheduler->P_Queue.push(task->dependents[i]);
                    }
                }
            
                scheduler->RunNextTasks(); 

                logging << "event=FINISHTASK " << "id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
            }
            break;
        }
        case EVENT_STARTCOMPUTE: {
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
                machine->StartTime = GLOBAL_TIMER; // start the computing 

                //check how much time task have left (less than task time if it was checkpointed before)
                int TimeLeft = task->TaskTime - task->LastValidCP; 
                machine->TimeToComplete = TimeLeft / machine->cp; // define the estimated compute time for completion, based on machine computing coeff

                // CP interval, if exists was calculed before
                if (task->Checkpointable == true && scheduler->RFactor != R_RESTARTING) { // if task is checkpointable
                    //calculate the next interval based on Lazy CP
                    int TimeToNextCP = sqrt(task->TimeToCheckpoint * (task->TimeToCheckpoint + (task->TimeToCheckpoint + LAZY_MTBF) / LAZY_ERROR)); //sqrt(b² + b*y/e + M*b/e) where b = y
                    int LazyTimeToNextCP = round(std::max(TimeToNextCP * pow((GLOBAL_TIMER - scheduler->LastFailure) / TimeToNextCP, 1 - LAZY_K_FACTOR), (double)TimeToNextCP)); // I've included max(oci, lazy) for when Global - Last Failure are close to 0
                    machine->CurrCheckpoint = task->LastValidCP; // attribute to the machine the current checkpoint of the task
                    machine->TimeBetweenCP = LazyTimeToNextCP; // define next cp interval
                    machine->ComputedTime = task->LastValidCP / machine->cp; // mark as computed time the checkpointed part of task (if it exists)
                    machine->TimeToComplete -= machine->ComputedTime; //discount if the task is already did some computation

                    if (machine->TimeBetweenCP < machine->TimeToComplete){  // if task will do an CP
                        //©reate the next CP Event 
                        Event * event = new Event();
                        event->type = EVENT_CHECKPOINT;
                        event->info = std::to_string(task->id) + " " + machine->id + " ";
                        event->time = GLOBAL_TIMER + machine->TimeBetweenCP;   
                        AddEvent(event);
                    } else { // if no CP
                        Event * event = new Event();
                        event->type = EVENT_SENDDATA;
                        event->info = std::to_string(task->id) + " " + machine->id + " ";
                        event->time = machine->StartTime + machine->TimeToComplete;   
                        AddEvent(event);
                    }
                } else { // If the task isn't checkpointable
                    Event * event = new Event();
                    event->type = EVENT_SENDDATA;
                    event->info = std::to_string(task->id) + " " + machineid + " ";
                    event->time = machine->StartTime + machine->TimeToComplete;
                    AddEvent(event);
                }
            }

            logging << "event=STARTCOMPUTE " << "id=" << taskid << " machine=" << machineid << " time=" << GLOBAL_TIMER << "\n";
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

                // Calculate the time the task will be receiving the data
                int TransferTime = task->S / DATA_TRANSFER_RATE;

                Event * event = new Event();                
                event->type = EVENT_STARTCOMPUTE;
                event->info = std::to_string(task->id) + " " + machine->id + " ";
                event->time = GLOBAL_TIMER + TransferTime;
                AddEvent(event);

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
                    logging << "event=FAULT machine=" << machineid << " type=NETWORK time=" << GLOBAL_TIMER << "\n";
                    break;
                }
            }
            break;
        }
        case EVENT_NETWORKREPAIR: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string machineid = CurrentEvent->info;
            Machine * machine = scheduler->job->getMachineByID(machineid); 

            machine->FaultNetwork = false;
            logging << "event=NETWORKREPAIR " << " machine=" << machineid << " type=NETWORK time=" << GLOBAL_TIMER << "\n";
            break;
        }
        case EVENT_SENSORSREPAIR: {
            //Define the new GlobalTime
            GLOBAL_TIMER = CurrentEvent->time;

            std::string machineid = CurrentEvent->info;
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
                        machine->cs -= 0.025;
                        logging << "event=SYSMONITOR " << " machine=" << machine->id << " type=NETWORK_CS_" << machine->cs << " time=" << GLOBAL_TIMER << "\n";        
                    }
                }
                if (machine->FaultSensors == true){
                    if (machine->SensorsDiscount < 0.5){
                        machine->SensorsDiscount += 0.025;
                        machine->cs -= 0.025;
                        logging << "event=SYSMONITOR " << " machine=" << machine->id << " type=SENSORS_CS_" << machine->cs << " time=" << GLOBAL_TIMER << "\n";        
                    }
                }
            }
            
            // Event for next monitoring
            Event * event = new Event();
            event->type = EVENT_SYSMONITOR;
            event->time = CurrentEvent->time + MONITOR_TIME; 
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
                    scheduler->LastFailure = GLOBAL_TIMER;
                    machines[i]->status = MACHINE_TURNEDOFF; //Mark as a verified died machine
                    if (machines[i]->TaskExecuting == NULL)
                        scheduler->job->MachinesAvailable--;
                    // Check if we are in the minimum value on standard machines
                    if (scheduler->job->MachinesAvailable < scheduler->config.A){ // Creates a new machine if below the minimum value
                        Machine * newMachine = new Machine();
                        
                        newMachine->id = "NORMALMACHINE_" + std::to_string(scheduler->job->NextNormal);
                        newMachine->type = MACHINE_NORMAL;
                        newMachine->status = MACHINE_AVAILABLE;
                        newMachine->StartTime = 0;
                        newMachine->cs = 1.0;
                        newMachine->cp = (80 + (rand() % 20)) / 100.0;
                        newMachine->TimeToComplete = 0; 
                        newMachine->CurrCheckpoint = 0;
                        newMachine->TimeBetweenCP = 0;
                        newMachine->TaskExecuting = NULL;
                        newMachine->FaultSensors = false;
                        newMachine->SensorsDiscount = 0.0;
                        newMachine->FaultNetwork = false;
                        newMachine->NetworkDiscount = 0.0; 
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
                            scheduler->P_Queue.push(task);     
                            scheduler->RunNextTasks();
                            logging << "event=FAULTDETECT " << " machine=" << machines[i]->id << " type=TASKRESCHEDULED_" << task->id << " time=" << GLOBAL_TIMER << "\n";                           
                        } else {
                            if (machinesup < scheduler->config.Theta && task->Checkpointable == false){
                                task->PendingRed = true;
                            }
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
    scheduler->RunNextTasks();
}

