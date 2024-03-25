#include "Process.h"
#include <iostream>
#include <string>

Process::Process(){
    this->process_name = 'A';
    this->add_Que_time = 0;
    this->burst_number = 0;
    this->burst_remaining = 0;
    this->burst_time = new int[1];
    this->io_time = new int[1];
}

Process::Process(char process_name, int Arrival_time, const int Burst_number, const int Burst_remaining){
    this->process_name = process_name;
    this->add_Que_time = Arrival_time;
    this->burst_number = 0;
    this->burst_remaining = Burst_remaining;
    this->burst_time = new int[Burst_number];
    this->io_time = new int[Burst_number];
}

void Process::add_burst(int burst_time, int io_time){
    this->burst_time[this->burst_number] = burst_time;
    this->io_time[this->burst_number] = io_time;
    this->burst_number++;
}