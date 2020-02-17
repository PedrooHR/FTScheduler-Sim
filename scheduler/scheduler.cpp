#include "scheduler.h"

Scheduler::Scheduler()
{

}

int Scheduler::GetDependents(Task * curr_node, int Mark){
    if (curr_node->auxDependentMapping >= Mark){
        return 0;
    } else {
        curr_node->auxDependentMapping = Mark; //marca o nodo
        int dependents = 1;

        for (unsigned int i = 0; i < curr_node->getDependents().size(); i++){
            dependents += getTotalDependents( curr_node->getDependents()[i], Mark);
        }

        return dependents;
    }
}

void Scheduler::Initialize() {
    for (int i = 0; i < job->G.size(); i++) {
        int total = getDependents(job->G[i], i);
        job->G[i]->DoD = total;
        if (job->G[i]->dependencies.size() == 0){
            job->G[i]->status = TASK_READY;
            P_Queue.push(job->G[i]);
        }
    }
}

std::vector <Machine *> Scheduler::Provide(int Number) { //Provides Spot Machines
    std::vector <Machine *> machines;
    srand(time(NULL));
    for (int i = 0; i < Number && job->Machines_S.size() < config.Mspots; i++){
        Machine * newMachine = new Machine();
        newMachine->type = MACHINE_SPOT;
        newMachine->status = MACHINE_AVAILABLE;
        newMachine->cs = 1;
        newMachine->cp = (80 + (rand() % 20)) / 100.0;
        newMachine->id = "SPOTMACHINE_" + job->NextSpot;

        job->Machines_S.push_back(newMachine);
        job->NextSpot++;
        machines.push_back(newMachine);
    }
    return machines;
}

int Scheduler::assign(Task * NextTask, std::vector <Machine *> machines) {
    int pending = config.Theta - NextTask->Instances.size();
    if (NextTask->Checkpointable == true) {
        for (int i = 0; i < machines->size(); i++){
            Event * event = new Event();
            event->type = EVENT_TASKSTART;
            event->info = NextTask->id + " " + machines[i]->id;
            if (machines[i].type == MACHINE_SPOT) {
                event->time = GLOBAL_TIMER + SPOT_DELAY;
            } else {
                event->time = GLOBAL_TIMER;
            }
            EventHandler.AddEvent(event);
            pend--;
        }
    }
    return pend;
}

//Machine Sorting Auxiliary Functions
bool SortByMRLP (Machine * left, Machine * right) {
    if (left->cs > right->cs) {
        return true;
    } else if (left->cs < right->cs) {
        return false;
    } else {
        if (left->cp < right->cp) {
            true;
        } else {
            false;
        }
    }
}

bool SortByMRMP (Machine * left, Machine * right) {
    if (left->cs > right->cs) {
        return true;
    } else if (left->cs < right->cs) {
        return false;
    } else {
        if (left->cp < right->cp) {
            false;
        } else {
            true;
        }
    }
}

std::vector <Machine *> Scheduler::getNextMachine(int Order) { //Get only One Machine
    std::vector <Machine *> machines;
    switch (Order){
        case MRLP:
            sort(job->Machines.begin(), job->Machines.end(), SortByMRLP);
            break;
        case MRMP:
            sort(job->Machines.begin(), job->Machines.end(), SortByMRMP);
            break;
    }
    for (int i = 0; i < job->Machines.size(); i++) {
        if (job->Machines[i].status == MACHINE_AVAILABLE){
            machines.push_back(job->Machines[i]);
            break;
        }
    }
    return machines;
}

std::vector <Machine *> Scheduler::getNextMachine(int Order, int MaxInstances) { //Try to get MaxInstances machines
    std::vector <Machine *> machines;
    switch (Order){
        case MRLP:
            sort(job->Machines.begin(), job->Machines.end(), SortByMRLP);
            break;
        case MRMP:
            sort(job->Machines.begin(), job->Machines.end(), SortByMRMP);
            break;
    }
    for (int i = 0; i < job->Machines.size(); i++) {
        if (job->Machines[i].status == MACHINE_AVAILABLE){
            machines.push_back(job->Machines[i]);
            if (machines.size() >= MaxInstances)
                break;
        }
    }
    return machines;
}

//Tasks Sorting Auxiliary Functions
bool SortByStartTime (Task * left, Task * right) {
    return left->StartTime < right->StartTime;
}

bool SortByInstancesSize (Task * left, Task * right) {
    return left->Instances.size() < right->Instances.size();
}

void Scheduler::CheckPendingRedundancy() {
    //Sort Running Tasks by StartTime
    sort(RunningTasks.begin(), RunningTasks.end(), SortByStartTime);
    for (int i = 0; i < RunningTasks.size(); i++){
        if (RunningTasks[i].PendingRed == true){            
            if (job->MachinesAvailable == 0){
                break;
            }
            std::vector <Machine *> machines = getNextMachine(MRLP);
            assign(RunningTasks[i], machines);
            if (RunningTasks[i].Instances.size() >= config.Theta){
                RunningTasks[i].PendingRed = false;
            }
        }
    }
}

void Scheduler::CheckRedundancy() {
    //Sort Running Tasks by Instances Size
    sort(RunningTasks.begin(), RunningTasks.end(), SortByInstancesSize);
    int i = 0, stop = 0;
    while (job->MachinesAvailable > 0 && stop < RunningTasks.size()){
        stop = 1;
        if (RunningTasks[i].Instances < config.Theta){
            std::vector <Machine *> machines = getNextMachine(MRLP);
            assign(RunningTasks[i], machines);
        } else {
            stop += 1;
        }
        i = i % RunningTasks.size();
    }
}

void Scheduler::CheckTask(Task * NextTask) {
    if (NextTask.S < config.R){
        NextTask.Checkpointable = true;
        std::vector <Machine *> machines = getNextMachine(MRLP);
        assign(NextTask, Machine);
    } else {
        NextTask.Checkpointable = false;
        std::vector <Machine *> Instances = getNextMachine(MRMP, config.Theta);
        if (Instances[0].cs < config.Gamma) {
            CheckTask(P_Queue.pop());
            P_Queue.push(NextTask);
            return;
        } else {
            int pend = assign(NextTask, Instances);
            if (pend > 0){
                if (job->Machines_S < config.Mspots) {
                    Instances = Provide(Config.Theta);
                    pend = assign(NextTask, Instances); //Have to discount providing time
                }
                if (pend > 0){
                    NextTask.PendingRed = true;
                }
            }
        }
    }
    RunningTasks.push(NextTask);
}

void Scheduler::RunNextTasks() {
    CheckPendingRedundancy();
    while (P_Queue.empty() == false && job.MachinesAvailable > 0) {
        Task * NextTask = P_Queue.pop();
        CheckTask(NextTask);
    } 
    if (job->MachinesAvailable > 0) {
        CheckRedundancy();
    }
}

void Scheduler::StartScheduler() {
    Initialize();
    RunNextTasks();

    //Add The First Monitor Event
    Event * monitor = new Event();
    monitor.type = EVENT_SYSMONITOR;
    monitor.time = 0;
    EventHandler.AddEvent(monitor);
}