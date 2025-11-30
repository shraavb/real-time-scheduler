
#include <vector>
#include <pthread.h>
#include <atomic>
#include <unistd.h>
#include <queue>
#include <time.h>

using namespace std;

extern int current_id;

enum Policy{
    RM = 0,
    EDF = 1
};

// define task control block for scheduling queue. represents a "job"
struct Task_Control{
    int task_id;
    int job_id;
    int priority;
    int arrival_time;
    int period;
    int current_period;
    int max_execution_time;
    int executed;
    int deadline;
    Task_Control(int,int,int,int,int,int);
};

// represents a single task
class Task{
    public:
        // define S,P,E,D for task
        int start_time;
        int period;
        int execution_time;
        int deadline;
        pthread_t thread_; // thread associated with task
        bool pause_  = false; // bool for whether job is running or not
        int counter = 1; // number of jobs that have been instantiated
        int id;
        Task(int,int,int,int,int);

        ~Task();
        void execute();
};

class Scheduler{
    public: 
        vector<Task*> task_table; // list of tasks 
        vector<Task_Control> scheduling_queue; // list of active jobs waiting to be scheduled
        Scheduler();
        ~Scheduler();
    private:
        int deadline_miss;
};

void* helper_thread(void *args);

void create_new_task(int S, int P, int E, int D);