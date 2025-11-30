#include "part_2_skeleton.h"
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <algorithm>
#include <numeric>

// INCOMPLETE FUNCTIONS:
//      void Task::execute()
//      int main(void)
//      any other functions you may require

vector<Task*> *task_list;
vector<Task_Control> *job_list;

int main(void){
    timespec req{0, 100000000};

    Scheduler scheduler;
    task_list = &scheduler.task_table;
    job_list = &scheduler.scheduling_queue;
    int time_;

    // read inputs from user, and construct list of tasks from them
    string policy_choice = "";
    int policy = 0;
    int num_tasks = 0;
    int S, P, E, D;
    int sim_start_time = -1;
    int max_sim_time = -1;
    bool kill_late_tasks;
    
    while( (policy_choice != "RM") && (policy_choice != "EDF") ){
        cout << "Select policy (RM or EDF): ";
        cin >> policy_choice;
    }

    if (policy_choice == "EDF") policy = 1;
    else policy = 0;

    while(num_tasks <= 0){
        cout << "Input number of tasks: ";
        cin >> num_tasks;
    }

    for(int i = 0; i < num_tasks; i++){
        P = 0; D = 1;
        while(P != D){
            cout << "Input task " << i+1 << " as \"S P E D\": ";
            cin >> S >> P >> E >> D;
        }
        create_new_task(S,P,E,D);
    }

    while(sim_start_time < 0){
        cout << "Input simulator starting time (ms): ";
        cin >> sim_start_time;
    }

    while(max_sim_time < 0){
        cout << "Input maximum simulation time (ms): ";
        cin >> max_sim_time;
    }

    string input = "";
    while( (input != "Y") && (input != "N") ){
        cout << "Terminate tasks that miss their deadlines? ";
        cin >> input;
    }

    if(input == "Y") kill_late_tasks = true;
    else kill_late_tasks = false;

    cout << endl;

    // begin scheduling loop
    time_ = sim_start_time;
    int active_sim_time = 0;
    int num_deadline_misses = 0;

    cout << "------------------------------------------------" << endl;
    while (time_ < sim_start_time + max_sim_time){

        cout << "Time: " << time_ << endl;

        // if job list is empty, don't schedule anything
        if (job_list->empty()){
            cout <<"No job is waiting" <<endl;
            cout << "------------------------------------------------" << endl;
            time_++;
            continue;
        }
        active_sim_time++;

        // choose highest priority task
        Task_Control& current_job = job_list->front();
        cout << "Scheduler: picking task " << current_job.task_id <<endl;
        cout << "Task " << current_job.task_id << ", Job "<< current_job.current_period \
            << ": starts executing " << current_job.executed+1 << " of " << current_job.max_execution_time << endl;
        
        // start the correct job (wake up thread)

        // sleep until it's time to select new thread (1 decisecond)
        nanosleep(&req,NULL);

        // stop execution of the most recent job

        time_++;
        cout << "------------------------------------------------" << endl;
    }

    cout << endl;
    cout << "Deadline misses: " << num_deadline_misses << endl;
    cout << "CPU utilization: " << to_string(float(active_sim_time)/float(max_sim_time)) << endl;

}

/* Task class functions */

// constructor
Task::Task(int S, int P, int E, int D, int num){
    start_time = S;
    period = P;
    execution_time = E;
    deadline = D;
    id = num;
    pthread_create(&thread_, NULL, &helper_thread, (void *)this);
}

// destructor
Task::~Task(){
    pthread_join(this->thread_,NULL);
}

// function executed by each task
void Task::execute(){
    while(true){
        // cout << "Execute current task " << this->id << endl;
    }
}

/* Task_Control class functions */

// constructor
Task_Control::Task_Control(int task_id,int job_id, int priority, int arrival_time, int period, int max_execution_time){
    this->task_id = task_id;
    this->job_id = job_id;
    this->priority = priority;
    this->arrival_time = arrival_time;
    this->period = period;
    this->current_period = job_id;
    this->max_execution_time = max_execution_time;
    this->deadline = arrival_time + period;
    this->executed = 0;
}

/* Scheduler class functions */

// constructor
Scheduler::Scheduler(){
    deadline_miss = 0;
}

// destructor
Scheduler::~Scheduler(){
    task_table.clear();
}

/* HELPER FUNCTIONS */

// runs function that thread needs to execute
void* helper_thread(void *args){
    ((Task*)args)->execute();
    return NULL;
}

// create new tasks
void create_new_task(int S, int P, int E, int D){
    Task* new_task = new Task(S,P,E,D,task_list->size()+1);
    task_list->push_back(new_task);
}