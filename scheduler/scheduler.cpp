//#include "scheduler.h"
#include "definitions.h"

bool PQCompare::operator() (const Task * left, const Task * right) const {
    return left->DoD < right->DoD;
}

Scheduler::Scheduler(int factor, EventHandler * handler) {
    RFactor = factor;
    eventhandler = handler;
}

int Scheduler::GetDependents(Task * curr_node, int Mark){
    if (curr_node->auxDependentMapping >= Mark){
        return 0;
    } else {
        curr_node->auxDependentMapping = Mark; //marca o nodo
        int dependents = 1;

        for (unsigned int i = 0; i < curr_node->dependents.size(); i++){
            dependents += GetDependents(curr_node->dependents[i], Mark);
        }

        return dependents;
    }
}

void Scheduler::Initialize() {
    for (int i = 0; i < job->G.size(); i++) {
        int total = GetDependents(job->G[i], i);
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
        for (int i = 0; i < machines.size(); i++){
            Event * event = new Event();
            event->type = EVENT_TASKSTART;
            event->info = NextTask->id + " " + machines[i]->id;
            if (machines[i]->type == MACHINE_SPOT) {
                event->time = GLOBAL_TIMER + SPOT_DELAY;
            } else {
                event->time = GLOBAL_TIMER;
            }
            eventhandler->AddEvent(event);
            pending--;
        }
    }
    return pending;
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
        case MRLP: {
            std::sort(job->Machines.begin(), job->Machines.end(), SortByMRLP);
            break;
        }
        case MRMP: {
            std::sort(job->Machines.begin(), job->Machines.end(), SortByMRMP);
            break;
        }
    }
    for (int i = 0; i < job->Machines.size(); i++) {
        if (job->Machines[i]->status == MACHINE_AVAILABLE){
            machines.push_back(job->Machines[i]);
            break;
        }
    }
    return machines;
}

std::vector <Machine *> Scheduler::getNextMachine(int Order, int MaxInstances) { //Try to get MaxInstances machines
    std::vector <Machine *> machines;
    switch (Order){
        case MRLP: {
            std::sort(job->Machines.begin(), job->Machines.end(), SortByMRLP);
            break;
        }
        case MRMP: {
            std::sort(job->Machines.begin(), job->Machines.end(), SortByMRMP);
            break;
        }
    }
    for (int i = 0; i < job->Machines.size(); i++) {
        if (job->Machines[i]->status == MACHINE_AVAILABLE){
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
    std::sort(RunningTasks.begin(), RunningTasks.end(), SortByStartTime);
    for (int i = 0; i < RunningTasks.size(); i++){
        if (RunningTasks[i]->PendingRed == true){            
            if (job->MachinesAvailable == 0){
                break;
            }
            std::vector <Machine *> machines = getNextMachine(MRLP);
            assign(RunningTasks[i], machines);
            if (RunningTasks[i]->Instances.size() >= config.Theta){
                RunningTasks[i]->PendingRed = false;
            }
        }
    }
}

void Scheduler::CheckRedundancy() {
    //Sort Running Tasks by Instances Size
    std::sort(RunningTasks.begin(), RunningTasks.end(), SortByInstancesSize);
    int i = 0, stop = 0;
    while (job->MachinesAvailable > 0 && stop < RunningTasks.size()){
        stop = 1;
        if (RunningTasks[i]->Instances.size() < config.Theta){
            std::vector <Machine *> machines = getNextMachine(MRLP);
            assign(RunningTasks[i], machines);
        } else {
            stop += 1;
        }
        i = i % RunningTasks.size();
    }
}

void Scheduler::CheckTask(Task * NextTask) {
    if (NextTask->S < config.R){
        NextTask->Checkpointable = true;
        std::vector <Machine *> machines = getNextMachine(MRLP);
        assign(NextTask, machines);
    } else {
        NextTask->Checkpointable = false;
        std::vector <Machine *> Instances = getNextMachine(MRMP, config.Theta);
        if (Instances[0]->cs < config.Gamma) {
            Task * subsequenttask = P_Queue.top(); //get the next task
            P_Queue.pop(); // remove next task from queue
            CheckTask(subsequenttask);
            P_Queue.push(NextTask); // add this task to queue again
            return;
        } else {
            int pend = assign(NextTask, Instances);
            if (pend > 0){
                if (job->Machines_S.size() < config.Mspots) {
                    Instances = Provide(config.Theta);
                    pend = assign(NextTask, Instances); //Have to discount providing time
                }
                if (pend > 0){
                    NextTask->PendingRed = true;
                }
            }
        }
    }
    RunningTasks.push_back(NextTask);
}

void Scheduler::RunNextTasks() {
    CheckPendingRedundancy();
    while (P_Queue.empty() == false && job->MachinesAvailable > 0) {
        Task * NextTask = P_Queue.top(); // get first task in the queue
        P_Queue.pop(); // Remove the task from queue
        CheckTask(NextTask);
    } 
    if (job->MachinesAvailable > 0) {
        CheckRedundancy();
    }
}

// Auxiliary Sort Compare Function
bool SortByS (Task * left, Task * right) {
    return left->S < right->S;
}

// Auxiliary Function to determine R threshold
void Scheduler::RCalc(int factor){
    std::vector <Task *> localG = job->G;
    std::sort(localG.begin(), localG.end(), SortByS);

    int NumberOfTasks = localG.size();
    long long int Sum = 0;

    for (int i = 0; i < NumberOfTasks; i++){
        Sum += localG[i]->S;
    }

    switch (factor){
        case R_MEAN: {
            config.R = Sum / (1.0 * NumberOfTasks);
            break;
        }
        case R_MEDIAN: {
            if (NumberOfTasks % 2 == 0){
                config.R = (localG[(NumberOfTasks / 2) - 1]->S + localG[NumberOfTasks / 2]->S) / 2.0;
            } else {
                config.R = float(localG[NumberOfTasks / 2]->S);
            }
            break;
        }
        case R_LQUARTILE: {
            int half = NumberOfTasks / 2;
            
            if (half % 2 == 0){
                config.R = (localG[(half / 2) - 1]->S + localG[half / 2]->S) / 2.0;
            } else {
                config.R = float(localG[half / 2]->S);
            }
            break;
        }
        case R_HQUARTILE: {
            int half = NumberOfTasks / 2;
            
            if (half % 2 == 0){
                config.R = (localG[(NumberOfTasks - 1) - (half / 2)]->S + localG[NumberOfTasks - (half / 2)]->S) / 2.0;
            } else {
                config.R = float(localG[(NumberOfTasks - 1) - (half / 2)]->S);
            }
            break;
        }
    }
}

void Scheduler::StartScheduler() {
    Initialize();
    RunNextTasks();

    //Add The First Monitor Event
    Event * monitor = new Event();
    monitor->type = EVENT_SYSMONITOR;
    monitor->time = 0;
    eventhandler->AddEvent(monitor);

    //Call to the auxiliary Function to determine R threshold
    RCalc(RFactor);
}