#ifndef PROGRESS_H
#define PROGRESS_H

class Process{
    public:
        bool isCpuBound;
        char process_name;
        int arrival_time;
        int burst_number;
        int burst_remaining;
        int* cpu_bursts;
        int* io_bursts;
        int* remain_cpu_bursts;
        int terminatedTime;
        int tau;
        int burst_time_left = -1;
        int burst_start_time = -1;

        int turnAroundTime = 0;
        int waitTime = 0;
        int switchCount = 0;
        int preemptionCount = 0;

        Process();
        Process(char process_name, int Arrival_time, const int Burst_number, const int Burst_remaining, bool isIObound, double lambda);
        void add_burst(int burst_time, int io_time);
        void add_burst(int burst_time);
        void free_self();
        bool isCompleted();
        int getCurrentBurst();
        int getCurrentIOBurst();
        void onStart(int startTime);
};

#endif // PROGRESS_H