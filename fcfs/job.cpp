#include "job.h"

std::string path;
long int TOTAL_TASK_TIME = 0;

/* Machine Part */
std::string Machine::getId()
{
    return id;
}
void Machine::setId(std::string Id)
{
    id = Id;
}

int Machine::getStatus()
{
    return status;
}
void Machine::setStatus(int Status)
{
    status = Status;
}

float Machine::getEfficiency()
{
    return efficiency;
}
void Machine::setEfficiency(float Efficiency)
{
    efficiency = Efficiency;
}

long int Machine::getStartedTime()
{
    return started_time;
}
void Machine::setStartedTime(long int StartTime)
{
    started_time = StartTime;
}

int Machine::getTimeToComplete()
{
    return time_to_complete;
}
void Machine::setTimeToComplete(int TimeToComplete)
{
    time_to_complete = TimeToComplete;
}

int Machine::getCheckpointSpeed()
{
    return checkpoint_speed;
}
void Machine::setCheckpointSpeed(int cp_speed)
{
    checkpoint_speed = cp_speed;
}

Machine::Machine(std::string Id, float Efficiency, int cp_speed)
{
    this->efficiency = Efficiency;
    this->status = 1;
    this->time_to_complete = 0;
    this->started_time = 0;
    this->id = Id;
    this->checkpoint_speed = cp_speed;
}

/* Task Part */
int Task::getId()
{
    return id;
}
void Task::setId(int id_l)
{
    id = id_l;
}

int Task::getStatus()
{
    return status;
}
void Task::setStatus(int Status)
{
    status = Status;
}

int Task::getCompleteTime()
{
    return complete_time;
}
void Task::setCompleteTime(int Time)
{
    complete_time = Time;
}

int Task::getNumberDependents() const
{
    return number_dependents;
}
void Task::setNumberDependents(int dependents)
{
    number_dependents = dependents;
}

int Task::getNumberDependencies()
{
    return number_dependencies;
}
void Task::setNumberDependencies(int dependencies)
{
    number_dependencies = dependencies;
}

int Task::getUsedMem()
{
    return used_mem;
}
void Task::setUsedMem(int mem)
{
    used_mem = mem;
}

int Task::getLastValidCP()
{
    return last_valid_cp;
}
void Task::setLastValidCP(int cp)
{
    last_valid_cp = cp;
}

void Task::addCheckpoint(float cp_time)
{
    checkpoints.push_back(cp_time);
}
std::vector <float> Task::getCheckpoints()
{
    return checkpoints;
}

void Task::addDependency(Task * dependency)
{
    dependencies.push_back(dependency);
}
std::vector <Task *> Task::getDependencies()
{
    return dependencies;
}

void Task::addDependent(Task * dependent)
{
    dependents.push_back(dependent);
}
std::vector <Task *> Task::getDependents()
{
    return dependents;
}

Task::Task(int Id, int Dependencies, int BaseTime, int Desv, int Mem){
    id = Id;
    number_dependencies = Dependencies;
    status = -1;
    number_dependents = 0;
    auxDependentMapping = 0;
    last_valid_cp = -1;
    used_mem = Mem;
    complete_time = BaseTime + Desv;
    //standard checkpoints
    checkpoints.push_back(0.25);
    checkpoints.push_back(0.5);
    checkpoints.push_back(0.75);
    TOTAL_TASK_TIME += complete_time;
}

Task::Task(int Id, int BaseTime, int Desv, int Mem){
    id = Id;
    number_dependencies = 0;
    status = -1;
    number_dependents = 0;
    auxDependentMapping = 0;
    last_valid_cp = -1;
    used_mem = Mem;
    complete_time = BaseTime + Desv;
    //standard checkpoints
    checkpoints.push_back(0.25);
    checkpoints.push_back(0.5);
    checkpoints.push_back(0.75);
    TOTAL_TASK_TIME += complete_time;
}

/* Job methods  */

void Job::createGraph(const char * xpath, int BaseTime, int DesvTime, int BaseMem){
    FILE * p;

    p = fopen(xpath, "r+");
    if (p == NULL){
        printf("Dependency graph file doesn't exist\n");
        exit(0);
    }
    int number_of_tasks;
    fscanf(p, "%d", &number_of_tasks);
    tasks_to_complete = number_of_tasks;
    srand(time(NULL));
    for (int i = 0; i < number_of_tasks; i++){
        //create the task
        int Mem = BaseMem + rand() % (BaseMem / 2);
        int timec = (-DesvTime) + rand() % (2*DesvTime);
        Task * new_Task = new Task(i, BaseTime, timec, Mem);
        dependency_graph.push_back(new_Task);
    }

    for (int i = 0; i < number_of_tasks; i++){
        int number_dependencies;
        fscanf(p, "%d", &number_dependencies);

        //printf("%d\n", number_dependencies);
        dependency_graph[i]->setNumberDependencies(number_dependencies);

        for (int j = 0; j < number_dependencies; j++){
            int tid;
            fscanf(p, "%d", &tid);
            //tid--;
            dependency_graph[i]->addDependency(dependency_graph[tid]);
            /* this is particularity of our method */
            dependency_graph[tid]->addDependent(dependency_graph[i]);
            dependency_graph[tid]->setNumberDependents(dependency_graph[tid]->getNumberDependents() + 1);
        }
    }

    fclose(p);
}
void Job::completeTask(Task * task_completed)
{
    task_completed->setStatus(3);
    tasks_to_complete--;
}
void Job::setReadyTasksToQueue(Task * task_completed) //not need to check all tasks to see
{
    std::vector <Task *> task_completed_dependents = task_completed->getDependents();
    int dependents_size = task_completed_dependents.size();
    for (int i = 0; i < dependents_size; i++){
        task_completed_dependents[i]->setNumberDependencies(task_completed_dependents[i]->getNumberDependencies() - 1);
        if (task_completed_dependents[i]->getNumberDependencies() == 0 && task_completed_dependents[i]->getStatus() == -1){
            task_completed_dependents[i]->setStatus(0); //waiting on queue
            tasks_ready_to_queue.push_back(task_completed_dependents[i]);
        }
    }
}
void Job::setReadyTasksToQueue() //
{
    for (unsigned int i = 0; i < dependency_graph.size(); i++){
        if (dependency_graph[i]->getNumberDependencies() == 0){
            dependency_graph[i]->setStatus(0); //waiting on queue
            tasks_ready_to_queue.push_back(dependency_graph[i]);
        }
    }
}
Task * Job::getNextReadyTask()
{
    if (tasks_ready_to_queue.empty()){
        return NULL;
    } else {
        Task * next_task = tasks_ready_to_queue.back();
        next_task->setStatus(1);
        tasks_ready_to_queue.pop_back();
        return next_task;
    }
}

void Job::createMachinePool(const char * xpath){
    FILE * p;

    p = fopen(xpath, "r+");
    if (p == NULL){
        printf("Machine pool file doesn't exist\n");
        exit(0);
    }
    int number_of_machines;
    fscanf(p, "%d", &number_of_machines);

    for (int i = 0; i < number_of_machines; i++){
        std::string m_id = "Machine_" + std::to_string(i);

        float m_eff;
        int cp_speed;

        fscanf(p, "%f %d", &m_eff, &cp_speed);

        Machine * new_Machine = new Machine(m_id, m_eff, cp_speed);

        machine_pool.push_back(new_Machine);
    }

    fclose(p);
}

void Job::createFaults(const char * xpath){
    FILE * p;

    p = fopen(xpath, "r+");
    if (p == NULL){
        printf("Faults file doesn't exist\n");
        exit(0);
    }

    int time, machine;

    while (fscanf(p, "%d %d", &time, &machine) != EOF)
        faults.push_back(std::pair <int, int> (time, machine));

    fault_count = 0;
    fclose(p);
}

Machine * Job::getNextReadyMachine()
{
    Machine * newmachine = NULL;
    for (unsigned int i = 0; i < machine_pool.size(); i++){
        if (machine_pool[i]->getStatus() == 1) // available
        {
            newmachine = machine_pool[i];
            break;
        }
    }
    return newmachine;
}
void Job::releaseMachine(Machine * machine_r)
{
    machine_r->setStatus(1);
    machine_r->setTimeToComplete(0);
    machine_r->setStartedTime(0);
}

std::vector <Machine *> Job::getMachines()
{
    return machine_pool;
}

std::vector <std::pair <int, int> > Job::getFaults()
{
    return faults;
}

int Job::getTasksToComplete()
{
    return tasks_to_complete;
}

void Job::logMachines(long int GLOBAL_TIMER)
{
    FILE * p;

    std::string local_path = path + "MachinesLog.txt";
    p = fopen(local_path.c_str(), "a+");
    fprintf(p, "GLOBALTIME -> Machine_XX -- St  -- StartedTim -- CompTim -- MacEff \n");
    for (unsigned int i = 0; i < machine_pool.size(); i++){
        char buffer[100];

        sprintf(buffer, "%10ld -> %10s -- %3d -- %10ld -- %7d -- %1.4f ", GLOBAL_TIMER, machine_pool[i]->getId().c_str(), machine_pool[i]->getStatus(), machine_pool[i]->getStartedTime(),
                                                        machine_pool[i]->getTimeToComplete(), machine_pool[i]->getEfficiency());
        fprintf(p, "%s\n", buffer);
    }

    fclose(p);
}

void Job::logTasks(long int GLOBAL_TIMER)
{
    FILE * p;

    std::string local_path = path + "TasksLog.txt";
    p = fopen(local_path.c_str(), "a+");
    fprintf(p, "GLOBALTIME -> ID  -- St  -- D-> -- D<- -- LCP -- TimeToComplete\n");
    for (unsigned int i = 0; i < dependency_graph.size(); i++){
        char buffer[100];
        sprintf(buffer, "%10ld -> %3d -- %3d -- %3d -- %3d -- %3d -- %7d", GLOBAL_TIMER, dependency_graph[i]->getId(), dependency_graph[i]->getStatus(), dependency_graph[i]->getNumberDependencies(), dependency_graph[i]->getNumberDependents(), dependency_graph[i]->getLastValidCP(), dependency_graph[i]->getCompleteTime());
        fprintf(p, "%s\n", buffer);
    }

    fclose(p);
}

Job::Job(const char * machine_path, const char * tasks_path,  const char * faults_path, int BaseTime, int DesvTime, int BaseMem, std::string lpath)
{
    createMachinePool(machine_path);
    createFaults(faults_path);
    createGraph(tasks_path, BaseTime, DesvTime, BaseMem);
    path = lpath;
}

