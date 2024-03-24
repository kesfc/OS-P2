#include "Process.h"
class Process{
    public:
        char process_name;
        int add_Que_time;
        int burst_number;
        int burst_remaining;
        int* burst_time;
        int* io_time;
        Process(){
            this->process_name = 'A';
            this->add_Que_time = 0;
            this->burst_number = 0;
            this->burst_remaining = 0;
            this->burst_time = new int[0];
            this->io_time = new int[0];
        }
        Process(char process_name, int Arrival_time, int Burst_number, int Burst_remaining){
            this->process_name = process_name;
            this->add_Que_time = Arrival_time;
            this->burst_number = Burst_number;
            this->burst_remaining = Burst_remaining;
            this->burst_time = new int[Burst_number];
            this->io_time = new int[Burst_number];
        }
        void add_burst(int burst_time, int io_time){
            this->burst_time[this->burst_number] = burst_time;
            this->io_time[this->burst_number] = io_time;
            this->burst_number++;
        }
};