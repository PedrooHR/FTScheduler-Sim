#include "definitions.h"

Machine::Machine() {

}

Task::Task() {

}

Task * Job::getTaskByID(int taskid){
    for (int i = 0; i < G.size(); i++){
        if (G[i]->id == taskid){
            return G[i];
            break;
        }
    }
}

Machine * Job::getMachineByID(std::string machineid){
    for (int i = 0; i < Machines.size(); i++){
        if (Machines[i]->id.compare(machineid) == 0){
            return Machines[i];
            break;
        }
    }
}

void Job::ReadGraph(std::string JobString){
    std::string datapath = JobString + ".txt";
    std::string deppath = JobString + ".cfg";
    FILE * taskdata, * taskdep;
    
    taskdata = fopen(datapath.c_str(), "r+");
    taskdep = fopen(deppath.c_str(), "r+");

    int NumberOfTasks;
    fscanf(taskdata, "%d\n", &NumberOfTasks);
    TasksToComplete = NumberOfTasks;

    //Create all tasks
    for (int i = 0; i < NumberOfTasks; i++) {
        Task * task = new Task();
        long int output;
        float time;
        char name[50];

        fscanf(taskdata, "%d %s %f %li %li\n", &task->id, name, &time, &task->S, &output);

        task->TaskTime = (int) ceil(time);

        task->status = TASK_NOTREADY;
        task->PendingRed = false;
        task->Checkpointable = false;
        task->LastValidCP = 0;
        task->StartTime = -1;
        task->Tries = 0;
        task->dependencies.clear();
        task->dependents.clear();
        task->Instances.clear();

        //Checkpoint info
        task->TimeToCheckpoint = std::max((task->S / HDD_WRITE_SPEED) * 2, (long int) MINIMUM_CP_TIME); 
        int number_cp = ((task->TaskTime / task->TimeToCheckpoint)) * CP_INT_MULTIPLIER; 
        task->NumberOfCheckpoints = number_cp;//std::min(25, number_cp);

        G.push_back(task);
    }

    for (int i = 0; i < NumberOfTasks; i++){
        int number_dependencies;
        fscanf(taskdep, "%d", &number_dependencies);

        G[i]->CurrNumberOfDependencies = number_dependencies;

        for (int j = 0; j < number_dependencies; j++){
            int tid;
            fscanf(taskdep, "%d", &tid);
            
            G[i]->dependencies.push_back(G[tid]);
            /* this is particularity of our method */
            G[tid]->dependents.push_back(G[i]);
        }
    }

    fclose(taskdata);
    fclose(taskdep);
}

void Job::ReadMachines(std::string JobString){
    std::string machinespath = JobString + "_m.txt";
    FILE * machinefile;

    machinefile = fopen(machinespath.c_str(), "r+");

    int number_of_machines;
    fscanf(machinefile, "%d", &number_of_machines);

    for (int i = 0; i < number_of_machines; i++){
        Machine * machine = new Machine();
        
        fscanf(machinefile, "%f", &machine->cp);
        machine->id = "NORMALMACHINE_" + std::to_string(i);
        machine->type = MACHINE_NORMAL;
        machine->status = MACHINE_AVAILABLE;
        machine->StartTime = 0;
        machine->cs = 1.0;
        machine->TimeToComplete = 0; 
        machine->CurrCheckpoint = 0;
        machine->TimeBetweenCP = 0;
        machine->TaskExecuting = NULL;
        machine->FaultSensors = false;
        machine->SensorsDiscount = 0.0;
        machine->FaultNetwork = false;
        machine->NetworkDiscount = 0.0; 

        Machines.push_back(machine);
    }

    MachinesAvailable = number_of_machines;
    NextSpot = 0;
    NextNormal = number_of_machines;

    fclose(machinefile);
}

Job::Job(std::string JobString)
{
    ReadGraph(JobString);
    ReadMachines(JobString);
}