#include "part_1_skeleton.h"
#include <iostream>
#include <vector>
#include <algorithm>

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

    int current_time = start_time;
    int active_sim_time = 0;
    int num_deadline_misses = 0;

    // maintain list of active jobs (task instantiations) waiting to be scheduled for execution
    vector<Job> jobs;

    // while loop steps through time
    while (current_time < start_time + max_sim_time){
        cout << "------------------------------------------------" << endl;
        cout << "Time: " + to_string(current_time) << endl;

        // Step 1: Remove completed jobs first
        for (int i = jobs.size() - 1; i >= 0; i--) {
            if (jobs[i].current_execution_time >= jobs[i].max_execution_time) {
                jobs.erase(jobs.begin() + i);
            }
        }

        // Step 2: Check for deadline misses
        vector<int> missed_indices;
        for (int i = 0; i < jobs.size(); i++) {
            // Deadline occurs at beginning of time step
            if (jobs[i].exact_deadline == current_time) {
                cout << "Task " + to_string(jobs[i].task_id) + ", Job " + to_string(jobs[i].job_id) +
                        ": missed deadline" << endl;
                num_deadline_misses++;
                if (kill_late_tasks) {
                    missed_indices.push_back(i);
                }
            }
        }

        // Remove jobs that missed deadlines (if kill_late_tasks is true)
        // Remove in reverse order to avoid index shifting issues
        for (int i = missed_indices.size() - 1; i >= 0; i--) {
            jobs.erase(jobs.begin() + missed_indices[i]);
        }

        // Step 3: Create new jobs for tasks that should start at this time
        for (int i = 0; i < num_tasks; i++) {
            // Check if this task should release a job at current_time
            if (current_time >= tasks[i].start) {
                int time_since_start = current_time - tasks[i].start;
                // Check if this is a period boundary (start time or multiple of period)
                if (time_since_start % tasks[i].period == 0) {
                    tasks[i].num_jobs_created++;
                    Job new_job = {
                        tasks[i].task_id,
                        tasks[i].num_jobs_created,
                        0, // current_execution_time starts at 0
                        tasks[i].max_execution_time,
                        tasks[i].period,
                        current_time + tasks[i].period // exact deadline
                    };
                    jobs.push_back(new_job);
                }
            }
        }

        // Step 4: Sort jobs based on priority policy
        if (policy == "RM") {
            // Rate Monotonic: shorter period = higher priority
            sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) {
                if (a.period != b.period) {
                    return a.period < b.period;
                }
                // Tie-breaker: earlier job_id
                return a.job_id < b.job_id;
            });
        } else if (policy == "EDF") {
            // Earliest Deadline First: earlier absolute deadline = higher priority
            sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) {
                if (a.exact_deadline != b.exact_deadline) {
                    return a.exact_deadline < b.exact_deadline;
                }
                // Tie-breaker: earlier job_id
                return a.job_id < b.job_id;
            });
        }

        // Step 5: Execute highest priority job (if available)
        if (!jobs.empty()) {
            Job& current_job = jobs[0];

            cout << "Scheduler: picking task " + to_string(current_job.task_id) << endl;

            // "run" job for 1 ms
            current_job.current_execution_time++;
            cout << "Task " + to_string(current_job.task_id) + ", Job " + to_string(current_job.job_id) +
                    ": starts executing " + to_string(current_job.current_execution_time) + " of " +
                    to_string(current_job.max_execution_time) << endl;

            active_sim_time++;
        } else {
            cout << "No job is waiting" << endl;
        }

        current_time++;
    }

    // print stats
    cout << "------------------------------------------------" << endl;
    cout << endl << "Deadline misses: " << to_string(num_deadline_misses) << endl;
    cout << "CPU utilization: " << to_string(float(active_sim_time)/float(max_sim_time)) << endl;
}

// helper function to calculate GCD
ll gcd(ll a, ll b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

// calculates least common multiple of n integers
// parameters:
//      array of n integers
//      length of array
// returns:
//      least common multiple of integers in array
ll findlcm(int arr[], int n)
{
    if (n == 0) return 0;

    ll result = arr[0];
    for (int i = 1; i < n; i++) {
        result = (result * arr[i]) / gcd(result, arr[i]);
    }
    return result;
}