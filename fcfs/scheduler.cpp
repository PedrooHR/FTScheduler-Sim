#include "scheduler.h"

long int GLOBAL_TIMER = 0;
std::string spath;

void Scheduler::createJob(const char *machinePath, const char * tasksPath, const char * faultsPath, int BaseTime, int DesvTime, int BaseMem, std::string lpath)
{
    job = new Job(machinePath, tasksPath, faultsPath, BaseTime, DesvTime, BaseMem, lpath);
    job->setReadyTasksToQueue(); //inital set of tasks;
    spath = lpath;
}

void Scheduler::getReadyToQueueTasks()
{
    Task * next_task = job->getNextReadyTask();
    while (next_task){
        pqueue.push(next_task);
        next_task = job->getNextReadyTask();
    }
}

void Scheduler::assignMachines()
{
    while(!pqueue.empty()) {
        Machine * nextmachine = job->getNextReadyMachine();

        if (nextmachine == NULL) {
            break;
        } else {
            RunningTask * next_rtask = new RunningTask();
            next_rtask->task = pqueue.front();
            next_rtask->task->setStatus(2);

            float fraction = (pqueue.front()->getLastValidCP() == -1) ? 1.0 : 1.0 - pqueue.front()->getCheckpoints()[pqueue.front()->getLastValidCP() - 1]; // exclude time before last valid checkpoint
            int cp_cost = (pqueue.front()->getLastValidCP() >= 0) ? ceil(pqueue.front()->getUsedMem() / (1.0 * nextmachine->getCheckpointSpeed())) : 0; //if loading a checkpoint include time to checkpoint
            int total_time = ceil((pqueue.front()->getCompleteTime() * fraction) / nextmachine->getEfficiency() ) + cp_cost; //include time to load a checkpoint
            nextmachine->setTimeToComplete(total_time);
            nextmachine->setStartedTime(GLOBAL_TIMER);
            nextmachine->setStatus(2);
            next_rtask->machine = nextmachine;

            running_tasks.push_back(next_rtask);
            pqueue.pop();
        }
    }
}

void Scheduler::checkCompletedTasks()
{
    unsigned int i = 0;
    while (i < running_tasks.size()) {
        if ((GLOBAL_TIMER - running_tasks[i]->machine->getStartedTime()) >= running_tasks[i]->machine->getTimeToComplete()) { //Global time - time started >= Time to complete
            //task has completed
            job->completeTask(running_tasks[i]->task);
            job->setReadyTasksToQueue(running_tasks[i]->task);
            job->releaseMachine(running_tasks[i]->machine); //release the machine

            running_tasks.erase(running_tasks.begin() + i);
        } else {
            i++;
        }

    }
}

void Scheduler::checkCheckpointing()
{
    for (unsigned int i = 0; i < running_tasks.size(); i++){
        int last_cp = running_tasks[i]->task->getLastValidCP();
        if (last_cp < (int) running_tasks[i]->task->getCheckpoints().size()){
            if ((GLOBAL_TIMER - running_tasks[i]->machine->getStartedTime()) > (running_tasks[i]->task->getCheckpoints()[last_cp + 1] * running_tasks[i]->machine->getTimeToComplete())) {
                running_tasks[i]->task->setLastValidCP(last_cp + 1);
                int time_to_cp = ceil(running_tasks[i]->task->getUsedMem() / (1.0 *  running_tasks[i]->machine->getCheckpointSpeed()));
                running_tasks[i]->machine->setTimeToComplete(running_tasks[i]->machine->getTimeToComplete() + time_to_cp);
            }
        }
    }
}

int Scheduler::getRemainingTasks()
{
    return job->getTasksToComplete();
}

void Scheduler::heartBeat()
{
    //this part is associated with heartbeat but does not compose heartbeat
    if (job->fault_count < ((int) job->getFaults().size() * 0.7) ){
        if (job->getFaults()[job->fault_count].first < GLOBAL_TIMER) {
            std::string local_path = spath + "MainLogA.txt";
            FILE * x = fopen(local_path.c_str(), "a+");
            char buffer[100];
            sprintf(buffer, "%10ld -> %10s Fault ", GLOBAL_TIMER, job->getMachines()[job->getFaults()[job->fault_count].second]->getId().c_str());
            fprintf(x, "%s\n", buffer);
            fclose(x);
            job->getMachines()[job->getFaults()[job->fault_count].second]->setStatus(0);
            job->fault_count++;
        }
    }

    //here heartbeat starts
    if (GLOBAL_TIMER % heartbeat_step == 0) {
        unsigned int i = 0;
        while (i < running_tasks.size()){
            if (running_tasks[i]->machine->getStatus() == 0){
                std::string local_path = spath + "MainLog.txt";
                FILE * q = fopen(local_path.c_str(), "a+");
                char buffer[100];
                sprintf(buffer, "%10ld -> %10s Fault ", GLOBAL_TIMER, running_tasks[i]->machine->getId().c_str());
                fprintf(q, "%s\n", buffer);
                fclose(q);

                running_tasks[i]->task->setStatus(1); //send to queue again
                pqueue.push(running_tasks[i]->task); //added queue
                running_tasks.erase(running_tasks.begin() + i);
            } else {
                i++;
            }
        }
    }
}


int Scheduler::getNMachines(){
    return job->getMachines().size();
}

void Scheduler::logRunningTasks()
{
   // FILE * p;

    //p = fopen("RunningTasks.txt", "a+");

    fprintf(p, "GLOBALTIME -> ID  -- Machines Assigned\n");
    for (unsigned int i = 0; i < running_tasks.size(); i++) {
        fprintf(p, "%10ld -> %d -- ", GLOBAL_TIMER, running_tasks[i]->task->getId());
        fprintf(p, "%s (S: %ld , CT: %d) ", running_tasks[i]->machine->getId().c_str(), running_tasks[i]->machine->getStartedTime(), running_tasks[i]->machine->getTimeToComplete());
        fprintf(p, "\n");
    }

    //fclose(p);
}

void Scheduler::logJob(long int GLOBAL_TIMER)
{
    job->logMachines(GLOBAL_TIMER);
    job->logTasks(GLOBAL_TIMER);
}

Scheduler::Scheduler(int hb_time)
{
    heartbeat_step = hb_time;
}
