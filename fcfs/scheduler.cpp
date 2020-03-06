#include "definitions.h"

Scheduler::Scheduler(EventHandler * handler, int mode, int CPTIME) {
    eventhandler = handler;
    Mode = mode;
    CPRed = CPTIME;
}

void Scheduler::Initialize() {
    long long int TotalTime = 0;//Auxiliary to see task total time

    for (int i = 0; i < job->G.size(); i++) {
        if (job->G[i]->dependencies.size() == 0){
            job->G[i]->status = TASK_READY;
            ReadyQueue.push(job->G[i]);
        }
        TotalTime += job->G[i]->TaskTime;
    }
    //eventhandler->logging.open("EventLog.txt", std::ofstream::app);
    eventhandler->logging << "SumTaskTimes=" << TotalTime << "\n";
    //eventhandler->logging.close();
}

void Scheduler::assign(Task * NextTask, std::vector <Machine *> machines) { 
    if (NextTask->status == TASK_COMPLETED)
        return;

    for (int i = 0; i < machines.size(); i++){
        //Task 
        NextTask->status = TASK_RUNNING;
        NextTask->Instances.push_back(machines[i]);

        //Machine
        machines[i]->status = MACHINE_BUSY;
        machines[i]->TaskExecuting = NextTask;
        job->MachinesAvailable--;
        
        Event * event = new Event();
        event->type = EVENT_TASKSTART;
        event->info = std::to_string(NextTask->id) + " " + machines[i]->id + " ";
        event->time = GLOBAL_TIMER;
        eventhandler->AddEvent(event);
    }
}

std::vector <Machine *> Scheduler::getNextMachine(int Quantity) { // Try to get MaxInstances machines
    std::vector <Machine *> machines;
  
    for (int i = 0; i < job->Machines.size(); i++) {
        if (job->Machines[i]->status == MACHINE_AVAILABLE){
            machines.push_back(job->Machines[i]);
            if (machines.size() >= Quantity)
                break;
        }
    }

    return machines;
}

void Scheduler::RunNextTasks() {

    while (ReadyQueue.empty() == false && job->MachinesAvailable > 0) {
        Task * NextTask = ReadyQueue.front(); // get first task in the queue
        ReadyQueue.pop(); // Remove the task from queue
        
        // Get the available machines
        std::vector <Machine *> machines;
        if (Mode == REDUNDANCY_MODE){
            machines = getNextMachine(CPRed);
        } else {
            machines = getNextMachine(1);  //Only one machine.
        } 
        
        assign(NextTask, machines); // Assign machines to the task
    } 
}

void Scheduler::StartScheduler() {
    Initialize();    // Get First Ready tasks to queue;
    RunNextTasks();  // Run First set of tasks

    //Add The First Fault Detection Event
    Event * detector = new Event();
    detector->type = EVENT_FAULTDETECTOR;
    detector->time = 0;
    eventhandler->AddEvent(detector);
}