#include <iostream>
#include <string>
#include "algorithms.h"
#include "SJF.h"

SJF::SJF(std::string name, std::vector<Process> processes, int t_cs, int alpha, int lambda){
    this->name = name;
    this->processes = processes;
    this->t_cs = t_cs;
    this->initial_tau = 1/lambda;
    this->alpha = alpha;
}
void SJF::Start() {
    cout << "time 0ms: Simulator started for " << this->name << " [Q <empty>]" << endl;
}

void SJF::setup() {
    this->currentTime = 0;
    isRemovingProcess = false;
    isLoadingProcess = false;
}
