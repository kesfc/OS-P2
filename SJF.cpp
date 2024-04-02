#include <iostream>
#include <string>
#include "algorithms.h"
#include "SJF.h"

SJF::SJF(std::string name, std::vector<Process> processes, int t_cs, int alpha, int lambda) : Algo(name, processes, t_cs) {
    this->alpha = alpha;
    this->initial_tau = 1.0 / lambda;
}
void SJF::Start() {
    cout << "time 0ms: Simulator started for " << this->name << " [Q <empty>]" << endl;
}

