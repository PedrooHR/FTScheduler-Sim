#include <cstdio>
#include <iostream>
#include "scheduler.h"
#include <string>

using namespace std;

#define SIMULATING_STEP 20 //20 segundos

string mpath;

void resetLogs(){
    FILE * p;

    string local_path = mpath + "MachinesLog.txt";
    p = fopen(local_path.c_str(), "w");
    fclose(p);

    local_path = mpath + "TasksLog.txt";
    p = fopen(local_path.c_str(), "w");
    fclose(p);

    local_path = mpath + "RunningTasks.txt";
    p = fopen(local_path.c_str(), "w");
    fclose(p);

    local_path = mpath + "MainLog.txt";
    p = fopen(local_path.c_str(), "w");
    fclose(p);

    local_path = mpath + "MainLogA.txt";
    p = fopen(local_path.c_str(), "w");
    fclose(p);
}


int main(int argc, char * argv[]){
    Scheduler * scheduler = new Scheduler(60); //a cada minuto
    //Set up things
    if (argc == 1){
        mpath = "";
        scheduler->createJob("25machinesF.cfg", "100.cfg", "100m.cfg", 7200, 360, 10000, mpath);
    } else if (argc == 8) {
        mpath.append(argv[7]);
        scheduler->createJob(argv[1], argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), mpath);
    } else {
        printf("invalid arguments\n");
        exit(0);
    }

    resetLogs();

    scheduler->logJob(GLOBAL_TIMER);
    std::string local_path = mpath + "RunningTasks.txt";
    scheduler->p = fopen(local_path.c_str(), "a+");
    int n_tasks = scheduler->getRemainingTasks();
    while (scheduler->getRemainingTasks() > 0)
    {
        scheduler->heartBeat();
        //printf("Remaining: %d\n", scheduler->getRemainingTasks());

        scheduler->getReadyToQueueTasks(); //see if are new tasks to put in queue
        scheduler->assignMachines(); //assign machines to execute tasks in queue
        scheduler->checkCompletedTasks(); //check if any task was completed;
        scheduler->checkCheckpointing();

        //logging
        scheduler->logRunningTasks();

        //scheduler->logJob(GLOBAL_TIMER);
        GLOBAL_TIMER += SIMULATING_STEP;
    }

    fclose(scheduler->p);

    scheduler->logJob(GLOBAL_TIMER);
    local_path = mpath + "MainLog.txt";
    FILE * p = fopen(local_path.c_str(), "a+");

    int n_machines = scheduler->getNMachines();
    fprintf(p, "%f\n%ld\n", TOTAL_TASK_TIME / (1.0 * n_machines), GLOBAL_TIMER);

    fclose(p);

    local_path = "Resultados/Compilado.csv";
    p = fopen(local_path.c_str(), "a+");
    fprintf(p, "Ideal Time; %d Tasks; %d; %f\nTime Spent; %d Tasks; %d; %ld\n", n_tasks, n_machines, TOTAL_TASK_TIME / (1.0 * n_machines), n_tasks, n_machines, GLOBAL_TIMER);
    fclose(p);

    return 0;
}
