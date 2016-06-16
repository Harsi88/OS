#include <iostream>
#include "schedule.h"
#include <assert.h>
#include <cstdlib>

ostream& operator<<(ostream& output, const schedule& sch) {
	output << endl << "Task Schedule" << endl;
	output << "Number of Jobs = " << sch.nJobs << ", Number of Processing Elements = " << sch.nPE << endl;
	output << "Number of Edges = " << sch.nEdges << endl;
	output << "Makespan = " << sch.makespan << endl;

//	map<int, map<int, bool > > alloc(sch.allocation);
//	map<int, map<int, float > > s(sch.start_time), f(sch.finish_time);
////	map<int, Generic_Processor > processor (sch.processor_list);
//
//	output << "JobId -> Allocation" << endl;
//	for(int i = 0; i < sch.nJobs; ++i) {
//		output << i+1 << " -> ";
//		for(int m = 0; m < sch.nPE; ++m) {
//			output << alloc[i][m] << " ";
//		}
//		output << endl;
//	}
//
//	output << endl << "JobId -> Start Time" << endl;
//	for(int i = 0; i < sch.nJobs; ++i) {
//			output << i+1 << " -> ";
//			for(int m = 0; m < sch.nPE; ++m) {
//				if (alloc[i][m]) {
//					s[i][m] = s[i][m] < 1 ? 0 : s[i][m];
//					output << s[i][m] << " ";
//				}
//				else
//					output << "-" << " ";
//			}
//			output << endl;
//		}
//
//	output << endl << "JobId -> Finish Time" << endl;
//		for(int i = 0; i < sch.nJobs; ++i) {
//				output << i+1 << " -> ";
//				for(int m = 0; m < sch.nPE; ++m) {
//					if (alloc[i][m])
//						output << f[i][m] << " ";
//					else
//						output << "-" << " ";
//				}
//				output << endl;
//			}
	return output;
}

void schedule::prepare_timeline() {
//	cout<<"Processor List\n";
	for(int m=0; m < nPE; ++m) {
		for(int i = 0; i < nJobs; ++i)
			if (allocation[i][m]) {
				Generic_Processor processor = processor_list[m];
				processor.timeline.push_back(*(new timeline_node(i, start_time[i][m], finish_time[i][m])));
				sort(processor.timeline.begin(), processor.timeline.end());
				processor_list.erase(m);
				processor_list[m] = processor;
			}
		cout<<"Proc "<<m<<" ";
		processor_list[m].print_timeline();
		cout<<"\n";
	}
}

void schedule::PrintNewGraph(float cCCR,int rCCR, float avgEC) {
//float rCCR;
	float avgCC=0;
//	cout<<"Please enter the rCCR "; cin>>rCCR;
//	cout<<"Please enter the rCCR "; cin>>rCCR;

	float marginCCR = 0.5 - cCCR;
	marginCCR = (marginCCR * avgEC)/nEdges;
	srand (time(NULL));

	cout<<"HyperPeiord 8000\nNo_of_Tasks 1\nNo_of_Processor "<<nPE<<endl;
	cout<<"t_id 0\nt_rtime 0\nt_period 8000\nt_dline 100\nt_nst "<<nJobs<<endl;

	for (int i = 0; i < nJobs; ++i) {
		cout<<"st"<<i+1;
		int pe_rand = rand() % nPE;
		for (int j = 0; j < nPE; ++j) {
			int id = (j+pe_rand) % nPE;
			cout <<" "<<(int)execution_cost[i][id];
		}
		cout<<endl;
	}
	for (int i = 0; i < nJobs; ++i)
		for (int j = 0; j < nJobs; ++j)
			if(i!=j && edges[i][j]!=0) {
				avgCC += edges[i][j]+marginCCR;
				cout<<"Link_from "<<i<<" "<<j<<" "<<(int)(edges[i][j]+marginCCR)<<endl;
			}
	cout<<"task_end -1"<<endl;
	avgCC = avgCC/(avgEC);
	cout<<"#Old CCR = "<<cCCR<<"\n";
	cout<<"#New CCR = "<<avgCC;
}

void schedule::getInputFromTGFF(istream& input,int ClSize) {
	float t;
	input >> t >> t >> nPE;
	input >> t >> t >> t >> t >> nJobs;
	float avgEC=0,avgCC=0, cCCR;
	nEdges=0;

	for (int i = 0; i < nJobs; ++i) {
		for (int j = 0; j < nPE; ++j) {
			input >> execution_cost[i][j];
		//	execution_cost[i][j] /= 10;
			avgEC += execution_cost[i][j];
		}
	}

	avgEC = avgEC/nPE;

	int a, b;
	float c;
	input >> a;
	while (a != -1) {
		input >> b >> c;
		predecessor_list[b].insert(a);
		successor_list[a].insert(b);
		edges[a][b] = c;
		avgCC += c; nEdges++;
		input >> a;
	}
//	avgCC = avgCC;
	cCCR = avgCC/avgEC;

//	PrintNewGraph(cCCR,ClSize,avgEC);
	critical_path();
	Phase = 1;
	ClusSize = ClSize;
	_nEdges = 0;
}

void schedule::getInputFromTGFF_with_contention(istream& input, int ClSize) {
	float t;
	input >> t >> t >> nPE;
	input >> t >> t >> t >> t >> nJobs;

	for (int i = 0; i < nJobs; ++i)
		for (int j = 0; j < nPE; ++j)
			input >> execution_cost[i][j];

	nEdges=0;nDeadl=0;
	int a, b; bool d;
	float c;float eli,elb;
	input >> a;
	while (a != -1) {
		nEdges++;
		input >> b >> c;
		predecessor_list[b].insert(a);
		successor_list[a].insert(b);
		edges[a][b] = c;
		input >> a;
	}

	input >> a;
	while (a != -1) {
		input >> c >> d;
		deadl_st[nDeadl] = a;
		deadl[nDeadl] = c;
		deadl_h[nDeadl] = d;
		input >> a;
		nDeadl++;
	}

	for (int j = 0; j < nPE; ++j)
		input >> E_idle[j] >> E_busy[j];

	input >> a;

	while (a != -1) {
		input >> b >> eli >> elb;
		El_idle[a][b] = eli; El_idle[b][a] = El_idle[a][b];
		El_busy[a][b] = elb; El_busy[b][a] = El_busy[a][b];
		input >> a;
	}

	input >> nLinks;

	for (int i = 0; i < nLinks; ++i)
		input >> bandwidth[i] >> E_Link[i];

	input >> nPaths;
	int source,dest;
	MaxPath = 0;
	for (int i = 0; i < nPaths; ++i) {
			int count=0;
			input >> a;
			source = a;
			input >> a;
			dest = a;
			input >> a;
			while (a != -1) {
				count++;
				Path[i].insert(Path[i].end(),a);
				input >> a;
			}
			MaxPath = MaxPath < count ? count : MaxPath;
			P2P_Paths[source][dest].insert(i);
			PSource[i]=source;
			PEnd[i] = dest;
	}
	for(int p = 0; p < nPaths; ++p) {
		vector<int>::iterator it;
		for(it = Path[p].begin()+Path[p].size()-1; it >= Path[p].begin(); --it) {
			cout<<*it<<" ";
		}
		cout<<"\n";
	}

	for (int j = 0; j < nPE; ++j)
			cout << E_idle[j] <<" "<< E_busy[j]<<"\n";

	// Initialise processor list
	for(int i=0;i<nPE;i++) {
		Generic_Processor * procr = new Generic_Processor();
		procr->processor_id = i;
		processor_list[i] = (*procr);
	}

	//initialise parameters related to clustering
	Phase = 1;
	ClusSize = ClSize;
	_nEdges = 0;

	// Find which Paths share a link
	for(int l=0;l<nLinks;l++)
		for(int p=0;p<nPaths;p++)
			for(int q=0;q<nPaths;q++) {
					if(LinkinPath(p,l) && LinkinPath(q,l)) {
						OPPL[p][q][l] = true;
//						cout<<"Path "<<p<<" Path "<<q<<" Link "<<l<<"\n";
					}
					else
						OPPL[p][q][l] = false;
			}

	for(int i=0;i<nLinks;i++)
		_LID[i] = 0;
}

bool schedule::LinkinPath(int p,int l) {
	vector<int>::iterator itl;

	for(itl = Path[p].begin();itl != Path[p].end(); ++itl) {
		if(*itl == l) return true;
	}
	return false;
}

void schedule::generateInitialSchedule() {
	float minCost[nPE];
	int min = 0;

	for (int i = 0; i < nPE; ++i) 
		minCost[i] = 0;

	for (int i = 0; i < nJobs; ++i) {
		for (int j = 0; j < nPE; ++j) {
			minCost[j] += execution_cost[i][j];
		}
	}

	for (int i = 0; i < nPE; ++i) {
		if (minCost[min] > minCost[i])
			min = i;
	}

	makespan = minCost[min];

	for (int i = 0; i < nJobs; ++i) {
		for (int m = 0; m < nPE; ++m) {
			if (m == min)
				allocation[i][m] = 1;
			else
				allocation[i][m] = 0;
		}
	}

	if (sort_nodes_topological()) {
		vector<int>::iterator it;
		float preend = 0;
		for(it = topological_sorted.begin(); it != topological_sorted.end(); ++it) {
			start_time[*it][min] = preend;
			finish_time[*it][min] = preend + execution_cost[*it][min];
			preend = finish_time[*it][min];
		}

		if (preend != makespan) 
			cout << "Mismatch" << " " << preend << " " << makespan << endl;
	}
	M = makespan + 100;
}

float schedule :: avg_computation (int node_id)
{
	float avg = 0.0;
	int size_p = nPE;
	for(int i= 0; i < size_p; i++)
	{
		avg += execution_cost[node_id][i];
	}
	avg /= size_p;

	return avg;
}

void schedule::
critical_path() {
	//initialize nodes blevel and tlevel to -1
	map<int, set<int > > next(successor_list), prev(predecessor_list);

	for(int i=0;i<nJobs;i++) {
		blevel[i] = -1; tlevel[i] = -1;
	}

	//recursively evaluate blevels and tlevels of all the nodes
	for(int i=0;i<nJobs;i++) {
		if(successor_list[i].size() == 0)	//leaf node evaluate tlevel
			t_level(i);
		if(predecessor_list[i].size() == 0)	//root node evaluate blevel
			b_level(i);
	}
	//find nodes in CPN, which have same CP(blevel+tlevel)
	float CP = -1;
	for(int i=0;i<nJobs;i++) {
//		cout<< blevel[i] + tlevel[i] <<"\n";
		CP = CP < blevel[i] + tlevel[i] ? blevel[i] + tlevel[i] : CP;
	}

//	cout<<"CP = "<<CP<<"\n";
	vector<int>::iterator it,fnd;
	sort_nodes_topological();

	for(it = topological_sorted.begin(); it != topological_sorted.end(); ++it) {
		if(CP - blevel[*it] - tlevel[*it] <= 0.001)
			CPList[*it] = CPN;
	}

	for(int i=0;i<nJobs;i++)
		if(CPList[i]==CPN) {
			set <int>::iterator itp;
			for(itp=prev[i].begin() ; itp!=prev[i].end() ; itp++)
				SetIBN(*itp);
		}

	for(int i=0;i<nJobs;i++) {
		if(CPList[i] != CPN && CPList[i] != IBN)
			CPList[i] = OBN;

//		cout<<i<<" "<<CPList[i]<<"\n";
	}

	//prepare clusters from the new list

	int id = 0;

//	vector<int>::iterator it;
//	sort_nodes_topological();

	//Add CPN and IBN according to topological ordering in the cluster list
	for(it = topological_sorted.begin(); it != topological_sorted.end(); ++it) {
		if(CPList[*it] == CPN || CPList[*it] == IBN) {
			Job[id] = *it;
			InCl[id] = false;
			id++;
		}
	}
	//Add remaining OBN according to topological ordering in the cluster list
	for(it = topological_sorted.begin(); it != topological_sorted.end(); ++it) {
		if(CPList[*it] == OBN) {
			Job[id] = *it;
			InCl[id] = false;
			id++;
		}
	}

//	for(int i=0;i<nJobs;i++) {
//		cout<<i<<" "<<Job[i]<<"\n";
//	}

	assert(id==nJobs);
}

void schedule::
SetIBN(int id) {
	map<int, set<int > > prev(predecessor_list);

	if(CPList[id] == CPN || CPList[id] == IBN)
		return;

	CPList[id] = IBN;

	set <int>::iterator itp;
	for(itp=prev[id].begin(); itp!=prev[id].end(); itp++)
		SetIBN(*itp);
}

void schedule::
b_level(int id) {

	map<int, set<int > > next(successor_list), prev(predecessor_list);

	float st_b,temp_b=0;

	//Ending condition -> check if node id has no children then process it
	if(successor_list[id].size() == 0) {
	    blevel[id] = avg_computation(id);
	    return;
	}

	set<int>::iterator it;
	for(it = next[id].begin(); it != next[id].end(); ++it) {
        if(blevel[*it] <= -1) b_level(*it);			//recursively calculate the level for *it
        st_b = blevel[*it] + avg_computation(id) + edges[id][*it];
        temp_b = (temp_b < st_b) ? st_b : temp_b;
    }
	blevel[id] = temp_b;
}

void schedule::
t_level(int id) {

	map<int, set<int > > next(successor_list), prev(predecessor_list);

	float st_t,temp_t=0;

	//Ending condition -> check if node id has no parent then process it
	if(predecessor_list[id].size() == 0) {
	    tlevel[id] = 0;
	    return;
	}

	set<int>::iterator it;
	for(it = prev[id].begin(); it != prev[id].end(); ++it) {
		if(tlevel[*it] <= -1) t_level(*it);			//recursively calculate the level for *it
        st_t = tlevel[*it] + avg_computation(*it) + edges[*it][id];
        temp_t = (temp_t < st_t) ? st_t : temp_t;
    }
	tlevel[id] = temp_t;
}


bool schedule::sort_nodes_topological() {
	if (topological_sorted.size() != 0)
		return true;

	map<int, set<int > > next(successor_list), prev(predecessor_list);

	stack<int> _strtset;
	for(int i = 0; i < nJobs; ++i) {			//multiple root nodes
		if (predecessor_list[i].size() == 0)
			_strtset.push(i);
	}

	int _temp;
	set<int>::iterator it, fnd;

	while (!_strtset.empty()) {
		_temp = _strtset.top();
		_strtset.pop();
		topological_sorted.push_back(_temp);		//node added to topological list
		for(it = next[_temp].begin(); it != next[_temp].end(); ++it) {  //successors of a node just processed
			fnd = prev[*it].find(_temp);
			if (fnd != prev[*it].end())
				prev[*it].erase(fnd);
			next[_temp].erase(it);

			if (prev[*it].size() == 0) {
				_strtset.push(*it);
			}
		}	
	}

	for(int i = 0; i < nJobs; ++i) {
		if (next[i].size() > 0 || prev[i].size() > 0) {
			topological_sorted.clear();
			return false;
		}
	}	

	return true;
}

void schedule::find_overlap_jobs() {

	map<int, set<int > > next(successor_list), prev(predecessor_list);

	for(int i = 0; i < nJobs; ++i)
		for(int j = 0; j < nJobs; ++j)
			ol[i][j] = true;

	for(int i = 0; i < nJobs; ++i) {		//for each job
		stack<int> _strtset;
		if (successor_list[i].size() != 0){		//process successors if any
			int _temp;
			set<int>::iterator it;
			for(it = next[i].begin(); it != next[i].end(); ++it)
				_strtset.push(*it);
			while (!_strtset.empty()) {
				_temp = _strtset.top();
				_strtset.pop();
				ol[i][_temp] = false;		//cancel overlap between jobs which are in precedence
//				ol[_temp][i] = false;
				for(it = next[_temp].begin(); it != next[_temp].end(); ++it)  //add successors of a successor in stack
					_strtset.push(*it);
			}
		}
		if (predecessor_list[i].size() != 0){
			int _temp;
			set<int>::iterator it;
			for(it = prev[i].begin(); it != prev[i].end(); ++it)
				_strtset.push(*it);
			while (!_strtset.empty()) {
				_temp = _strtset.top();
				_strtset.pop();
				ol[i][_temp] = false;		//cancel overlap between jobs which are in precedence
//				ol[_temp][i] = false;
				for(it = prev[_temp].begin(); it != prev[_temp].end(); ++it)  //add successors of a successor in stack
					_strtset.push(*it);
			}
		}
	}
}

void schedule::find_overlap_jobs_siblings() {

	map<int, set<int > > next(successor_list), prev(predecessor_list);

	for(int i = 0; i < nJobs; ++i)
		for(int j = 0; j < nJobs; ++j)
			ol_sib[i][j] = false;

	for(int i = 0; i < nJobs; ++i)		//for each job
		if (successor_list[i].size() > 1){		//process successors if any
			int _temp;
			set<int>::iterator it1,it2;
			for(it1 = next[i].begin(); it1 != next[i].end(); ++it1){
				for(it2 = next[i].begin(); it2 != next[i].end(); ++it2)
					if(*it1 != *it2)
						ol_sib[*it1][*it2] = true;
			}
		}
}

bool schedule::validateSchedule() {
	if (!sort_nodes_topological()) {
		return false;
	}

	vector<int>::iterator it;
	set<int>::iterator sit;

	bool flag=true;		//used for the jobs that have wrong finish time and are skipped for precedence check
	for(it = topological_sorted.begin(); it != topological_sorted.end(); ++it) {
		for(int m = 0; m < nPE; ++m) {
			if (allocation[*it][m] == 1) {
				if (finish_time[*it][m] < start_time[*it][m] + execution_cost[*it][m]) {
					cout<<"Finish time of "<<*it<<" on " << m+1 << " Incorrect\n";
					allocation[*it][m] == 0;
					flag = false;
//					return false;
				}
				if(flag)	//only check if job have valid finish time
				for(sit = predecessor_list[*it].begin(); sit != predecessor_list[*it].end(); ++sit) {
					bool pass = false;
					if ((allocation[*sit][m] == 1) && (start_time[*it][m] >= finish_time[*sit][m])) {
						pass = true;
					}
					else {
						for(int k = 0; k < nPE; ++k) {
							if (k != m && allocation[*sit][k] == 1) {
								if (start_time[*it][m] >= finish_time[*sit][k] + edges[*sit][*it]) {
									if(!pass)
										cout<<"Edge from "<<*sit+1<<" "<<k<<" "<<" - "<<*it+1<<" "<<m<<" "<<edges[*sit][*it]<<"\n";
									pass = true;
								}
							}
						}
					}

					if (!pass) {
						cout<<"Precedence between "<<*sit<<" and " << *it << " not meeting\n";
						return false;
					}
				}
				flag = true;
			}
		}
	}

	for(int i = 0; i < nJobs; ++i) {
		int _cnt = 0;
		for(int m = 0; m < nPE; ++m) {
			if (allocation[i][m] == 1)
				_cnt++;
		}
		if (_cnt < 1) {
			cout<<"No Copy of Job "<<i+1<<endl;
			return false;
		}
	}

	return true;
}

bool schedule::validateScheduleOverlap() {
//	if (!sort_nodes_topological()) {
//		return false;
//	}
	vector<int>::iterator it;
	set<int>::iterator sit;

	bool feas=true;

	for(it = topological_sorted.begin(); it != topological_sorted.end(); ++it)
		for(int m = 0; m < nPE; ++m)
			if (allocation[*it][m] == 1)
				for(int i = 0; i < nJobs; ++i)
					if(i!=*it && allocation[i][m])
						if((finish_time[i][m] - start_time[*it][m] > 0) && (finish_time[*it][m] - start_time[i][m] > 0)) {
							feas = false;
							cout<<"Overlap of "<<*it<<" and "<<i<<" on "<<m<<"\n";
							//update the overlap matrix
							ol[i][*it] = true;
							ol[*it][i] = true;
						}

	return feas;
}

float schedule::getMakespan()
{
	return this->makespan;
}

void schedule::setInitialSchedule(istream& input)
{
	for(int i = 0; i < this->nJobs; ++i) {
		for(int j = 0; j < this->nPE; ++j) {
			input >> this->start_time[i][j];
		}
	}

	for(int i = 0; i < this->nJobs; ++i) {
		for(int j = 0; j < this->nPE; ++j) {
			input >> this->finish_time[i][j];
		}
	}

	for(int i = 0; i < this->nJobs; ++i) {
		for(int j = 0; j < this->nPE; ++j) {
			if (this->finish_time[i][j] > this->start_time[i][j])
				this->allocation[i][j] = 1;
			else
				this->allocation[i][j] = 0;
		}
	}

	float makespan = 0;
	for(int i = 0; i < this->nJobs; ++i) {
		for(int j = 0; j < this->nPE; ++j) {
			if (makespan < this->finish_time[i][j])
				makespan = this->finish_time[i][j];
		}
	}
	this->makespan = makespan;
}
