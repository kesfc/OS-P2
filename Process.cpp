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

Process::Process(char process_name, int Arrival_time, int Burst_number, int Burst_remaining){
        std::cout << "burst_number = " << Burst_number << std::endl;
    this->process_name = process_name;
        std::cout << "burst_number = " << Burst_number << std::endl;
    this->add_Que_time = Arrival_time;
    this->burst_number = Burst_number;
        std::cout << "burst_number = " << Burst_number << std::endl;
    this->burst_remaining = Burst_remaining;
        std::cout << "burst_number = " << Burst_number << std::endl;
    this->burst_time = new int[Burst_number];
    this->io_time = new int[Burst_number];
}

void Process::add_burst(int burst_time, int io_time){
    this->burst_time[this->burst_number] = burst_time;
    this->io_time[this->burst_number] = io_time;
    this->burst_number++;
}