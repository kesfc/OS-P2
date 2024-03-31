
#include <fstream>
#include <iostream>
#include <queue>
#include "process.h"
#include "algorithms.h"
using namespace std;

class FCFS : public Algo{
public:
	FCFS(string& name, vector<Process> processes) : Algo(name, processes) {}
};

