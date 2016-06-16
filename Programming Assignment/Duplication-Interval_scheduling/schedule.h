#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__
#include <vector>
#include <iostream>
#include <map>
#include <stack>
#include <set>
#include <algorithm>
#include "processor.h"

using namespace std;

#define CPN 	1
#define IBN 	2
#define OBN 	3

class schedule {
	friend ostream& operator<<(ostream& output, const schedule& sch);

	private:
		int nPE, nJobs, nEdges, nDeadl;
		float makespan;
		int M;
		int nLinks,nPaths;
		int MaxPath;

		//Processor List
		map<int, Generic_Processor > processor_list;

		//variables related to clustering
		int		Phase;
		int 	ClusSize;
		map<int, int>		Job;
		map<int,bool>		InCl;

		//store the variable values from previous phases
		int		_nEdges;
		//following variables are filled from each phase and used as constraints in next phase
		//initialised in getinputfromtgff_contention
		map<int, int>				_LID;	//current indices of Link arrays
		map<int, map<int, float> >  _FTL;	//Fixed Finish times of Links
		map<int, map<int, float> >  _STL;	//Fixed Start Times of Links

		map<int, map<int, map<int, bool> > >	OPPL;		//whether two paths have a common link in them

		// b level, t level and cp
		map<int, float>		blevel,tlevel;

		map<int, float>		bandwidth;
		map<int, map <int, set<int > > > P2P_Paths;	//Processor to Processor paths with there IDs
		map<int, vector<int > > Path;				//Path contains links in order, represent a route
		map<int, int>	PSource;					//Each Path source and destination
		map<int, int>	PEnd;

		map<int, set<int> >			Path_Switch;	//switches in a path - yet to write code for this

		// Variables related to energy constraints - each processor has e(idle) and e(busy)

		map<int, float> 	E_idle,E_busy,E_Link;		//per processor,per processor, per link

		map<int, map<int, float> > El_idle, El_busy;

		//deadlines related parameters
		map<int, int>		deadl_st;		//subtask
		map<int, float>		deadl;			//deadline
		map<int, bool>		deadl_h;		//deadline is hard or soft 1-hard 0-soft

		map<int, map<int, float > > execution_cost;
		map<int, set<int > > predecessor_list, successor_list;

		map<int, map<int, float > > edges;
		map<int,int>	source,dest;
		map<int,float>	cost;

		map<int, map<int, bool > > allocation;
		map<int, map<int, float > > start_time, finish_time;
		map<int, float > st_nodup, ft_nodup;
		vector<int> topological_sorted;
		map<int, map<int, bool> > ol;
		map<int, map<int, bool> > ol_sib;

//		vector<int> CPN, IBN, OBN;
		map<int,int>	CPList;

		bool sort_nodes_topological();
		void b_level(int id);	//evaluate blevel of a node
		void t_level(int id);	//evaluate tlevel of a node

	public:
		void find_overlap_jobs();
		void find_overlap_jobs_siblings();
		void getInputFromTGFF(istream& input = cin,int ClSize = 8);
		void getInputFromTGFF_with_contention(istream& input = cin,int ClSize = 8);
		void setInitialSchedule(istream& input);
		void generateInitialSchedule();
		bool validateSchedule();
		bool validateScheduleOverlap();
		bool solveArmin(float, int nCopies = -1, float mipgap = -1, int algo = 0, int mipemph = 0);
		bool solveGeneral(float, int nCopies = -1, float mipgap = -1, int algo = 0, int mipemph = 0);
		bool solveGeneralclus(float, int nCopies = -1, float mipgap = -1, int algo = 0, int mipemph = 0);
		bool solveRestricted(float, int nCopies = -1, float mipgap = -1, int algo = 0, int mipemph = 0);
		bool solveNoDup(float, float mipgap = -1, int algo = 0, int mipemph = 0);
		bool solveCPDup(float, float mipgap = -1, int algo = 0, int mipemph = 0);
		bool solveNoDupClus(float, float mipgap = -1, int algo = 0, int mipemph = 50);
		bool solveSingleDuplication(float, float mipgap = -1);		
		float getMakespan();
		bool  Cluster1(int &);
		bool  Cluster2(int &);
		float avg_computation (int node_id);
		void  critical_path(); //find critical path of the DAG also set nodes to CPN, IBN and OBN
		void SetIBN(int id);
		bool LinkinPath(int p,int l);
		void prepare_timeline();
		void PrintNewGraph(float cCCR,int, float avgEC);
};

#endif
