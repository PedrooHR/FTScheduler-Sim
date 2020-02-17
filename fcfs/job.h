#ifndef JOB_H_INCLUDED
#define JOB_H_INCLUDED

/* Job includes all files related to the job, like the dependecy graph, the machines configuration and the probabilistic configuration */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>

extern long int TOTAL_TASK_TIME;

/* Task takes all information about tasks */
class Task {
private:
    int id;
    int status; //-1 = not queued; 0 = waiting to queue; 1 = queued; 2 = running; 3 = completed
    int complete_time; //in second
    int number_dependencies;
    int number_dependents;
    int used_mem;
    int last_valid_cp; //last valid checkpoint
    std::vector <float> checkpoints;
    std::vector <Task *> dependencies;
    std::vector <Task *> dependents; //only the direct ones

public:
    int auxDependentMapping;

    int getId();
    void setId(int id_l);

    int getStatus();
    void setStatus(int Status);

    int getCompleteTime();
    void setCompleteTime(int time);

    int getNumberDependents() const;
    void setNumberDependents(int dependents);

    int getNumberDependencies();
    void setNumberDependencies(int dependencies);

    int getUsedMem();
    void setUsedMem(int mem);

    int getLastValidCP();
    void setLastValidCP(int cp);

    void addCheckpoint(float cp_time);
    std::vector <float> getCheckpoints();

    void addDependency(Task * dependency);
    std::vector <Task *> getDependencies();

    void addDependent(Task * dependent);
    std::vector <Task *> getDependents();

    Task(int Id, int Dependencies, int BaseTime, int Desv, int Mem);
    Task(int Id, int BaseTime, int Desv, int Mem);
};

/* Machines info */
class Machine {
private:
    std::string id; //can be any string
    int status; //0 = down, 1 = available, 2 = busy
    float efficiency;
    int checkpoint_speed;

    long long int started_time;
    int time_to_complete; //in second
public:
    std::string getId();
    void setId(std::string Id);

    int getStatus();
    void setStatus(int Status);

    float getEfficiency();
    void setEfficiency(float Efficiency);

    long int getStartedTime();
    void setStartedTime(long int StartTime);

    int getTimeToComplete();
    void setTimeToComplete(int TimeToComplete);

    int getCheckpointSpeed();
    void setCheckpointSpeed(int cp_speed);

    Machine(std::string id, float Efficiency, int cp_speed);
};

/* maintain machine and tasks, and methods relative to scheduling */
class Job {
private:
    std::vector <Task *> dependency_graph;
    std::vector <Task *> tasks_ready_to_queue;
    std::vector <Machine *> machine_pool;
    std::vector <std::pair <int, int> > faults;

    int tasks_to_complete;
public:
    int fault_count;

    void createGraph(const char * path, int BaseTime, int DesvTime, int BaseMem);
    void completeTask(Task * task_completed);
    void setReadyTasksToQueue(Task * task_completed);
    void setReadyTasksToQueue();
    Task * getNextReadyTask();

    void createMachinePool(const char * path); //pass_id representa se o id esta ou não no arquivo de conf
    void createFaults(const char * xpath); //pass_id representa se o id esta ou não no arquivo de conf
    Machine * getNextReadyMachine();
    void releaseMachine(Machine * machine_r);
    std::vector <Machine *> getMachines();
    std::vector <std::pair <int, int> > getFaults();

    int getTasksToComplete();

    void logMachines(long int GLOBAL_TIMER);
    void logTasks(long int GLOBAL_TIMER);

    Job(const char * machine_path, const char * tasks_path, const char * faults_path, int BaseTime, int DesvTime, int BaseMem, std::string lpath);
};


#endif // JOB_H_INCLUDED
