#include <string>

using namespace std;

typedef long long int ll;

// describes a task
struct Task{
    int task_id; // i
    int start; // start time (ms)
    int period; // (ms)
    int max_execution_time; // (ms)
    int deadline; // relative deadline (ms)
    int num_jobs_created; // number of instantiations of task that have been created. initially 0
};

// describes an active instantiation of a task
struct Job{
    int task_id; // i
    int job_id; // j
    int current_execution_time; // (ms)
    int max_execution_time; // (ms)
    int period; // (ms)
    int exact_deadline; // exact deadline (ms). calculated from start of period
};

void simulate(string policy, int num_tasks, Task * tasks, int start_time, int max_sim_time, bool kill_late_tasks);

ll findlcm(int arr[], int n);
