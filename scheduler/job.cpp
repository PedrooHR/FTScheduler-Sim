#include "job.h"

/* Machine Part */
Machine::Machine() {

}

/* Task Part */

Task::Task() {

}

/* Job methods  */
//aux method to calculate dependents

Task * getTaskByID(int taskid){
    for (int i = 0; i < G->size(); i++){
        if (G[i]->id == taskid){
            return G[i];
            break;
        }
    }
}

Task * getTaskByID(std::string machineid){
    for (int i = 0; i < Machines->size(); i++){
        if (Machines[i]->id.compare(machineid)){
            return Machines[i];
            break;
        }
    }
}

void Job::ReadGraph(std::string JobString){
    std::string datapath = JobString + ".txt";
    std::string deppath = JobString + ".cfg"
    FILE * taskdata, taskdep;
    
    taskdata = fopen(datapath.c_str(), "r+");
    taskdep = fopen(deppath.c_str(), "r+");

    int NumberOfTasks;
    fscanf(taskdata, "%d", &NumberOfTasks);
    TasksToComplete = NumberOfTasks;

    //Create all tasks
    for (int i = 0; i < NumberOfTasks; i++) {
        Task * task = new Task();
        int output;
        char name[];

        fscanf(taskdata, "%d", &task->id);
        fscanf(taskdata, "%s", &name;
        fscanf(taskdata, "%d", &task->TaskTime);
        fscanf(taskdata, "%d", &task->S);
        fscanf(taskdata, "%d", &output);

        task->status = TASK_NOTREADY;
        task->PendingRed = false;
        task->Checkpointable = false;
        task->LastValidCP = 0;
        task->Dependencies.clear();
        task->dependents.clear();
        task->Instances.clear();

        //Checkpoint info
        task->TimeToCheckpoint = task->S / HDD_WRITE_SPEED;
        task->NumberOfCheckpoints = task->TaskTime / task->TimeToCheckpoint;

        G->push_back(task);
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
    std::string machinespath = JobString + "_m.txt"
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

    fclose(machinefile);
}

Job::Job(std::string JobString)
{
    ReadGraph(JobString);
    ReadMachines(JobString);
}