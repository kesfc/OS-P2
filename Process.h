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
        int terminatedTime;
        Process();
        Process(char process_name, int Arrival_time, const int Burst_number, const int Burst_remaining, bool isIObound);
        void add_burst(int burst_time, int io_time);
        void free_self();
        bool isCompleted();
        int getCurrentBurst();
        int getCurrentIOBurst();
};

#endif // PROGRESS_H