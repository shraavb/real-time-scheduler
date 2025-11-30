#include "part_1_skeleton.h"
#include <iostream>
#include <vector>

// INCOMPLETE FUNCTIONS:
//      void simulate(string policy, int num_tasks, Task * tasks, int start_time, int max_sim_time, bool kill_late_tasks)
//      findlcm(int arr[], int n)
//      any other functions you may require

int main(){
    string policy = "";
    int num_tasks = 0;
    int S, P, E, D;
    int sim_start_time = -1;
    int max_sim_time = -1;
    bool kill_late_tasks;

    // read user inputs
    // YOU MAY NOT ALTER INPUT PROMPTS

    while( (policy != "RM") && (policy != "EDF") ){
        cout << "Select policy (RM or EDF): ";
        cin >> policy;
    }

    while(num_tasks <= 0){
        cout << "Input number of tasks: ";
        cin >> num_tasks;
    }

    Task tasks[num_tasks];
    int periods[num_tasks];
    for(int i = 0; i < num_tasks; i++){
        P = 0; D = 1;
        while(P != D){
            cout << "Input task " << i+1 << " as \"S P E D\": ";
            cin >> S >> P >> E >> D;
        }

        tasks[i] = {(i+1), S, P, E, D, 0};
        periods[i] = P;
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
        cout << "Terminate tasks that miss their deadlines (Y or N)? ";
        cin >> input;
    }
    if(input == "Y") kill_late_tasks = true;
    else kill_late_tasks = false;

    // calculate maximum execution time if necessary
    if(max_sim_time == 0){
        max_sim_time = findlcm(periods, num_tasks);
    }

    cout << endl;

    // simulate task executions
    simulate(policy, num_tasks, tasks, sim_start_time, max_sim_time, kill_late_tasks);

    return 0;
}

// Simulates task executions. Prints list of task executions.
// parameters:
//      policy - RM or EDF
//      num_tasks - number of tasks
//      tasks - reference to array of Task structs
//      max_sim_time - maximum simulation time in ms
//      kill_late_tasks - flag indicating whether to terminate tasks that have missed their deadline (true) or not (false)
void simulate(string policy, int num_tasks, Task * tasks, int start_time, int max_sim_time, bool kill_late_tasks){

    // COMPLETE THIS FUNCTION TO SATISFY THE SPECIFICATIONS

    int current_time = start_time;
    int active_sim_time = 0;
    int num_deadline_misses = 0;
    Job current_job;
    int current_task_id, current_job_id;

    // maintain list of active jobs (task instantiations) waiting to be scheduled for execution
    vector<Job> jobs;

    // while loop steps through time
    while (current_time < start_time + max_sim_time){
        cout << "------------------------------------------------" << endl;
        cout << "Time: " + to_string(current_time) << endl;

        if(!jobs.empty()){
            // select a job
            current_job = jobs[0];
            current_task_id = current_job.task_id;
            current_job_id = current_job.job_id;

            cout << "Scheduler: picking task " + to_string(current_task_id) << endl;

            // "run" job for 1 ms
            current_job.current_execution_time++;
            cout << "Task " + to_string(current_task_id) + ", Job " + to_string(current_job_id) +
                    ": starts executing " + to_string(current_job.current_execution_time) + " of " + to_string(current_job.max_execution_time) << endl;
            
            active_sim_time++;
        }
        else{
            cout << "No job is waiting" << endl;
        }

        current_time++;
    }

    // print stats
    cout << "------------------------------------------------" << endl;
    cout << endl << "Deadline misses: " << to_string(num_deadline_misses) << endl;
    cout << "CPU utilization: " << to_string(float(active_sim_time)/float(max_sim_time)) << endl;
}

// calculates least common multiple of n integers
// parameters:
//      array of n integers
//      length of array
// returns:
//      least common multiple of integers in array
ll findlcm(int arr[], int n)
{

    // YOUR IMPLEMENTATION HERE

    return 0;
}