#include "Process.h"
#include <iostream>
#include <string>

Process::Process(){
    this->process_name = 'A';
    this->arrival_time = 0;
    this->burst_number = 0;
    this->burst_remaining = 0;
    this->cpu_bursts = new int[1];
    this->io_bursts = new int[1];
}

Process::Process(char process_name, int Arrival_time, const int Burst_number, const int Burst_remaining, bool isIObound, double lambda){
    this->process_name = process_name;
    this->arrival_time = Arrival_time;
    this->burst_number = 0;
    this->burst_remaining = Burst_remaining;
    this->cpu_bursts = new int[Burst_number];
    this->io_bursts = new int[Burst_number];
    this->remain_cpu_bursts = new int[Burst_number];
    this->isCpuBound = !isIObound;
    this->tau = 1.0/lambda;
}

void Process::add_burst(int burst_time, int io_time){
    this->cpu_bursts[this->burst_number] = burst_time;
    this->io_bursts[this->burst_number] = io_time;
    this->remain_cpu_bursts[this->burst_number] = burst_time;
    this->burst_number++;
}

void Process::add_burst(int burst_time) {
    this->cpu_bursts[this->burst_number] = burst_time;
    this->burst_number++;
}

void Process::free_self(){
    delete[] cpu_bursts;
    delete[] io_bursts;
}

bool Process::isCompleted() {
    
    return (burst_remaining == 0);
}

int Process::getCurrentBurst() {
    return cpu_bursts[burst_number - burst_remaining];
}

int Process::getCurrentIOBurst() {
    return io_bursts[burst_number - burst_remaining - 1];
}