#include <iostream>
#include <fstream>
#include <cstdlib>
#include "schedule.h"
#define TYPE 0

using namespace std;

int main(int argc, char **argv) {
	schedule sch;
	int time = -1;
	float relmipgap = -1;
	int	algo = 0, mipemph = 0;
	switch(argc) {
		case 5:
			mipemph = atoi(argv[4]);
		case 4:
			algo = atoi(argv[3]);
		case 3:
			relmipgap = atof(argv[2]);
		case 2:
			time = atoi(argv[1]);
	}

//	sch.getInputFromTGFF_with_contention();
	sch.getInputFromTGFF(cin,mipemph);
	
	ifstream file("input.heft");
	if (!file.is_open()) {
		sch.generateInitialSchedule();
		cout << "Generating Initial Input Schedule by assigning all the tasks to the same processor; which gives the minimum makespan" << endl;
	}
	else {
		sch.setInitialSchedule(file);
		cout << "Generating Initial Input Schedule from the file input.heft" << endl;
	}
	cout << "Printing Default Initial Schedule for debugging" << endl;
	cout << sch << endl;

	if (sch.validateSchedule())
		cout << "Feasible Schedule" << endl;
	else
		cout << "Infeasible Schedule" << endl;

	sch.find_overlap_jobs();

	bool stop = false;
	while(!stop)
		stop = sch.solveCPDup(time, relmipgap, algo, mipemph);

	cout << "-----------------------" << endl;
	cout << "Edge List -------------" << endl;
	if (sch.validateSchedule() && sch.validateScheduleOverlap())
		cout << "Feasible Schedule" << endl;
	else
		cout << "Infeasible Schedule" << endl;
//	cout << sch;
	cerr << "Makespan "<<sch.getMakespan()<<endl;

	sch.prepare_timeline();

	return 0;
}
