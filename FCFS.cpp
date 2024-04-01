
#include <fstream>
#include <iostream>
#include <queue>
#include "Process.h"
#include "algorithms.h"
using namespace std;

class FCFS : public Algo{
public:
	FCFS(string& name, vector<Process> processes, int t_cs) : Algo(name, processes, t_cs) {}
};

