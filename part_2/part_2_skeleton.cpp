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

// Global synchronization variables
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t task_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t scheduler_cv = PTHREAD_COND_INITIALIZER;
int current_id = -1; // ID of task that should execute
bool should_execute = false; // Flag indicating a task should execute
bool execution_done = false; // Flag indicating task finished its execution slice
bool shutdown_threads = false; // Flag to shutdown all threads

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

        pthread_mutex_lock(&global_mutex);

        // Step 1: Remove completed jobs
        for (int i = job_list->size() - 1; i >= 0; i--) {
            if ((*job_list)[i].executed >= (*job_list)[i].max_execution_time) {
                job_list->erase(job_list->begin() + i);
            }
        }

        // Step 2: Check for deadline misses
        vector<int> missed_indices;
        for (int i = 0; i < job_list->size(); i++) {
            if ((*job_list)[i].deadline == time_) {
                cout << "Task " << (*job_list)[i].task_id << ", Job " << (*job_list)[i].current_period
                     << ": missed deadline" << endl;
                num_deadline_misses++;
                if (kill_late_tasks) {
                    missed_indices.push_back(i);
                }
            }
        }

        // Remove jobs that missed deadlines
        for (int i = missed_indices.size() - 1; i >= 0; i--) {
            job_list->erase(job_list->begin() + missed_indices[i]);
        }

        // Step 3: Create new jobs for tasks that should start at this time
        for (int i = 0; i < task_list->size(); i++) {
            Task* task = (*task_list)[i];
            if (time_ >= task->start_time) {
                int time_since_start = time_ - task->start_time;
                if (time_since_start % task->period == 0) {
                    int priority = (policy == RM) ? task->period : (time_ + task->period);
                    Task_Control new_job(task->id, task->counter, priority, time_, task->period, task->execution_time);
                    job_list->push_back(new_job);
                    task->counter++;
                }
            }
        }

        // Step 4: Sort jobs based on priority policy
        if (policy == RM) {
            // Rate Monotonic: shorter period = higher priority (lower value)
            sort(job_list->begin(), job_list->end(), [](const Task_Control& a, const Task_Control& b) {
                if (a.priority != b.priority) {
                    return a.priority < b.priority;
                }
                return a.job_id < b.job_id;
            });
        } else if (policy == EDF) {
            // Earliest Deadline First: earlier deadline = higher priority (lower value)
            sort(job_list->begin(), job_list->end(), [](const Task_Control& a, const Task_Control& b) {
                if (a.deadline != b.deadline) {
                    return a.deadline < b.deadline;
                }
                return a.job_id < b.job_id;
            });
        }

        pthread_mutex_unlock(&global_mutex);

        // if job list is empty, don't schedule anything
        if (job_list->empty()){
            cout <<"No job is waiting" <<endl;
            cout << "------------------------------------------------" << endl;
            time_++;
            continue;
        }
        active_sim_time++;

        // choose highest priority task
        pthread_mutex_lock(&global_mutex);
        Task_Control& current_job = job_list->front();
        cout << "Scheduler: picking task " << current_job.task_id <<endl;
        cout << "Task " << current_job.task_id << ", Job "<< current_job.current_period \
            << ": starts executing " << current_job.executed+1 << " of " << current_job.max_execution_time << endl;

        // start the correct job (wake up thread)
        current_id = current_job.task_id;
        should_execute = true;
        execution_done = false;
        pthread_cond_broadcast(&task_cv);
        pthread_mutex_unlock(&global_mutex);

        // sleep until it's time to select new thread (1 decisecond)
        nanosleep(&req,NULL);

        // wait for task to complete execution
        pthread_mutex_lock(&global_mutex);
        while (!execution_done) {
            pthread_cond_wait(&scheduler_cv, &global_mutex);
        }

        // Update job execution time
        current_job.executed++;
        pthread_mutex_unlock(&global_mutex);

        time_++;
        cout << "------------------------------------------------" << endl;
    }

    // Shutdown all threads
    pthread_mutex_lock(&global_mutex);
    shutdown_threads = true;
    pthread_cond_broadcast(&task_cv);
    pthread_mutex_unlock(&global_mutex);

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
        pthread_mutex_lock(&global_mutex);

        // Wait until it's this task's turn to execute or shutdown
        while (!shutdown_threads && !(should_execute && current_id == this->id)) {
            pthread_cond_wait(&task_cv, &global_mutex);
        }

        // Check if we should shutdown
        if (shutdown_threads) {
            pthread_mutex_unlock(&global_mutex);
            break;
        }

        // Execute the task
        cout << "Execute current task " << this->id << endl;

        // Signal that execution is done
        execution_done = true;
        should_execute = false;
        pthread_cond_signal(&scheduler_cv);

        pthread_mutex_unlock(&global_mutex);
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