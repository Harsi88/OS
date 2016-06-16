#include <ilcp/cp.h>
#include "schedule.h"

//#define CP_ENERGY

ILOSTLBEGIN

typedef IloArray<IloNumVarArray> IloNumVarArrayArray;
typedef IloArray<IloNumArray> IloNumArrayArray;
typedef IloArray<IloNumVarArrayArray> IloNumVarArray3D;

bool schedule::solveNoDup(float timeLimit, float mipgap, int algo, int mipemph) {
	bool status;
	IloEnv env;
	try {
		IloModel 	model(env);
		IloCP 		cp(env);

		// ******************** Get the input *************************************
		// Declaring Parameters
		IloInt numJobs = this->nJobs, numPE = this->nPE, M = this->makespan, l, m, w; //w = PE; l,m = Jobs Indexes
		IloInt numPaths = this->nPaths;
		IloNumArrayArray Edges(env, numJobs), ExecutionCost(env, numJobs);

#ifdef CP_ENERGY
		IloNumVarArray			Ebusy(env,numPE,0,M*M,ILOFLOAT), Eidle(env,numPE,0,M*M,ILOFLOAT);
		IloNumVarArray			EPbusy(env,numPaths,0,M*M,ILOFLOAT), EPidle(env,numPaths,0,M*M,ILOFLOAT);
		IloNumVar				Etotal;
#endif

		// Fill In Values for jobs, pe, edges and execution cost by parsing
		// Adjacency list for the Task Graph
		for(l = 0; l < numJobs; ++l) {
			Edges[l] = IloNumArray(env, numJobs);
			for(m = 0; m < numJobs; ++m) {
				if (this->edges[l].count(m) > 0)
					Edges[l][m] = this->edges[l][m];
				else
					Edges[l][m] = 0;
			}
		}

		for(l = 0; l < numJobs; ++l) {
			ExecutionCost[l] = IloNumArray(env, numPE);
			for(w = 0; w < numPE; ++w) {
//				ExecutionCost[l][w] = this->execution_cost[l][w];
				ExecutionCost[l][w] = this->execution_cost[l][w];
			}
		}
		
		// ******************** Input Ends *************************************

		IloIntervalVarArray2 machines = IloIntervalVarArray2(env, numPE);
		for (w = 0; w < numPE; w++)
		    machines[w] = IloIntervalVarArray(env);
		
		char name[128];

		IloIntervalVarArray jobs = IloIntervalVarArray(env, nJobs);

		IloIntExprArray ends(env);
		IloIntVarArray ends_min(env, nJobs);

		for (l = 0; l < nJobs; l++) {
			jobs[l] = IloIntervalVar(env);
		    jobs[l].setStartMin(0);

		    jobs[l].setEndMax(M);
		    sprintf(name, "Job%ld",l);
			jobs[l].setName(name);

		    ends.add(IloEndOf(jobs[l]));

		    IloIntervalVarArray jobm(env, numPE);

		    for (w=0; w<numPE; w++) {
		      sprintf(name, "Alt%ld_%ld_C%ld",l,w,ExecutionCost[l][w]);
		      jobm[w] = IloIntervalVar(env, ExecutionCost[l][w]);
		      jobm[w].setName(name);
		      jobm[w].setOptional();
		      machines[w].add(jobm[w]);
		    }
			model.add(IloAlternative(env, jobs[l], jobm));
		}

// 		Energy of executing jobs
//		IloNumExprArray etotal(env);
		for (w = 0; w < numPE; w++) {
			model.add(IloNoOverlap(env, machines[w]));

#ifdef CP_ENERGY
/*			IloExpr excost(env);
			for(l = 0; l < numJobs; ++l) {
				excost +=  ExecutionCost[l][w] * IloPresenceOf(env, machines[w][l]);
			}
			model.add(Ebusy[w] == excost * E_busy[w]);
			model.add(Eidle[w] == (IloMax(ends) - excost) * E_idle[w]);
			etotal.add(Ebusy[w] + Eidle[w]);
			excost.end();*/
#endif
		}

		// Add the precedence constraints
		for (l = 0; l < numJobs; ++l)
			for (m = 0; m < numJobs; ++m)
				if(l!=m && Edges[l][m]!=0)
//			for(IloInt e = 0; e < nEdges; ++e)
					for(w = 0; w < numPE; ++w) {
//						l = source[e]; m = dest[e];
//						model.add(IloEndBeforeStart(env, jobs[l], jobs[m]));
						for(IloInt w1=0;w1 < numPE; w1++)
							if(w == w1)
								model.add(IloEndBeforeStart(env, machines[w][l], machines[w][m]));
							else {
								model.add(IloEndBeforeStart(env, machines[w][l], machines[w1][m],Edges[l][m]));
#ifdef CP_ENERGY
//								IloNum p;
//								model.add(IloPresenceOf(env, machines[w][l])+IloPresenceOf(env, machines[w1][m]) - 2*EONP[e][p]>=0);
//								model.add(IloPresenceOf(env, machines[w][l])+IloPresenceOf(env, machines[w1][m]) - 2*EONP[e][p]<=0);
#endif
							}
					}

	    IloObjective objective = IloMinimize(env,IloMax(ends));
	    model.add(objective);
//	    cp.setParameter(IloCP::BranchLimit, 1000000);
	    cp.setParameter(IloCP::TimeLimit, timeLimit);
	    cp.setParameter(IloCP::RelativeOptimalityTolerance, 0.1);


	    cp.extract(model);
	    if (cp.solve()) {
			cp.out() << endl;
 			cp.out() << "Solution status = " << cp.getStatus() << endl;
        	cp.out() << "Solution value  = " << cp.getObjValue() << endl;
//        	cp.out() << "Makespan : " << cp.getObjValue() << std::endl;

	      this->makespan = cp.getObjValue();
			for(l = 0; l < numJobs; ++l) {
				cout<<"Job "<<l+1<<" ";
				for(w = 0; w < numPE; ++w) {
					this->allocation[l][w] =  cp.isPresent(machines[w][l]);
					cout<<cp.isPresent(machines[w][l])<<" ";
					if (this->allocation[l][w] == 1)
					{
						this->start_time[l][w] = cp.getStart(machines[w][l]);
						this->finish_time[l][w] = cp.getEnd(machines[w][l]);
					}
				}
				cout<<"\n";
			}
	    } else {
	      cp.out() << "No solution found."  << std::endl;
	    }
	}
	catch(IloException &e) {
		cerr << "Concert Exception caught: " << e << endl;
	}
	return true;
}

bool schedule::solveCPDup(float timeLimit, float mipgap, int algo, int mipemph) {
	bool status;
	IloEnv env;
	try {
		IloModel 	model(env);
		IloCP 		cp(env);

		// ******************** Get the input *************************************
		// Declaring Parameters
		IloInt numJobs = this->nJobs, numPE = this->nPE, M = 2*this->makespan, l, m, w, lv, levels = 2; //w = PE; l,m = Jobs Indexes
		IloInt numPaths = this->nPaths;
		IloNumArrayArray Edges(env, numJobs), ExecutionCost(env, numJobs);

		// Fill In Values for jobs, pe, edges and execution cost by parsing
		// Adjacency list for the Task Graph
		for(l = 0; l < numJobs; ++l) {
			Edges[l] = IloNumArray(env, numJobs);
			for(m = 0; m < numJobs; ++m) {
				if (this->edges[l].count(m) > 0)
					Edges[l][m] = this->edges[l][m];
				else
					Edges[l][m] = 0;
			}
		}

		for(l = 0; l < numJobs; ++l) {
			ExecutionCost[l] = IloNumArray(env, numPE);
			for(w = 0; w < numPE; ++w) {
				ExecutionCost[l][w] = IloNumArray(env, levels);
				for(lv = 0; lv < levels; lv++)
				{
					ExecutionCost[l][w][lv] = this->execution_cost[l][w]/(2^lv);					
				}

			}
		}

		// ******************** Input Ends *************************************

		IloIntervalVarArray2 machines = IloIntervalVarArray2(env, numPE);
		for (w = 0; w < numPE; w++)
		    machines[w] = IloIntervalVarArray(env);

		char name[128];
		char name1[128];


		IloIntervalVarArray jobs = IloIntervalVarArray(env, 2*nJobs);
	//	IloIntervalVarArray jobs_copy = IloIntervalVarArray(env, nJobs);
		IloIntExprArray ends(env);
		IloIntExprArray ends1(env);
		IloIntVarArray ends_min(env, nJobs);

		for (l = 0; l < nJobs; l++) {
                    jobs[l] = IloIntervalVar(env);
		    jobs[l].setStartMin(0);

		    jobs[l].setEndMax(M);
		    sprintf(name, "Job%ld",l);
			jobs[l].setName(name);

		    ends.add(IloEndOf(jobs[l]));

		    IloIntervalVarArray jobm(env, numPE*levels);

		    for (lv=0; lv<levels; lv++){
		    for (w=0; w<numPE; w++) {
		      sprintf(name, "Alt%ld_%ld_C%ld",l,w,ExecutionCost[l][w][lv]);
		      jobm[w] = IloIntervalVar(env, ExecutionCost[l][w][lv]);
		      jobm[w].setName(name);
		      jobm[w].setOptional();
		      machines[w].add(jobm[w]);
		    }
		    }
		    for (w=0; w<numPE; w++) {
		    	model.add(IloAlternative(env, jobs[l], jobm));
		    }
		}

		for (l = nJobs; l < 2*nJobs; l++) {
			jobs[l] = IloIntervalVar(env);
		    jobs[l].setStartMin(0);

		    jobs[l].setEndMax(M);
		    sprintf(name1, "Job_copy%ld",l);
			jobs[l].setName(name1);

		    ends1.add(IloEndOf(jobs[l]));

		    IloIntervalVarArray jobm(env, numPE);

		    for (lv=0; lv<levels; lv++)
		    for (w=0; w<numPE; w++) {
		    {
		      sprintf(name1, "Alt%ld_%ld_C%ld",l,w,ExecutionCost[l-nJobs][w][lv]);
		      jobm[w] = IloIntervalVar(env, ExecutionCost[l-nJobs][w][lv]);
		      jobm[w].setName(name1);
		      jobm[w].setOptional();
		      machines[w].add(jobm[w]);
		    }
		    }
			model.add(IloAlternative(env, jobs[l], jobm));
		}

		for (w = 0; w < numPE; w++) {
			model.add(IloNoOverlap(env, machines[w]));
		}
		
		for(l=0;l<nJobs;l++){
			for(w=0;w<numPE; w++){
			for(lv=0;lv<levels;lv++)
			{
				model.add(IloIfThen(env,IloPresenceOf(env,machines[w][l][lv]),IloNot(IloPresenceOf(env,machines[w][l+nJobs][lv]))));
				model.add(IloIfThen(env,IloPresenceOf(env,machines[w][l][lv]),IloNot(IloPresenceOf(env,machines[w][l][(lv+1)%levels]))));
				model.add(IloIfThen(env,IloPresenceOf(env,machines[w][l+nJobs][lv]),IloNot(IloPresenceOf(env,machines[w][l+nJobs][(lv+1)%levels]))));
			}
			} 
		}

		// Add the precedence constraints
		for (l = 0; l < numJobs; ++l)
			for (m = 0; m < numJobs; ++m)
				if(l!=m && Edges[l][m]!=0)
					for(w = 0; w < numPE; ++w) {
						for(IloInt w1=0;w1 < numPE; w1++)
                        for(lv=0; lv<levels; lv++)
                        {
							if(w == w1)
							{
								model.add(IloEndBeforeStart(env, machines[w][l][lv], machines[w][m][lv]));
								model.add(IloEndBeforeStart(env, machines[w][l+nJobs][lv], machines[w][m][lv]));
								model.add(IloEndBeforeStart(env, machines[w][l][lv], machines[w][m+nJobs][lv]));
								model.add(IloEndBeforeStart(env, machines[w][l+nJobs][lv], machines[w][m+nJobs][lv]));
							}
							else {
								model.add(IloEndBeforeStart(env, machines[w][l][lv], machines[w1][m][lv],Edges[l][m]));
								model.add(IloEndBeforeStart(env, machines[w][l+nJobs], machines[w1][m][lv],Edges[l][m]));
								model.add(IloEndBeforeStart(env, machines[w][l][lv], machines[w1][m+nJobs][lv],Edges[l][m]));
								model.add(IloEndBeforeStart(env, machines[w][l+nJobs][lv], machines[w1][m+nJobs][lv],Edges[l][m]));
						    }
						}
					}

/*--------------------

		IloIntervalVarArray jobs1 = IloIntervalVarArray(env, nJobs);

		
		IloIntVarArray ends_min1(env, nJobs);

		

		for (w = 0; w < numPE; w++) {
			model.add(IloNoOverlap(env, machines[w]));
		}

		for (l = 0; l < numJobs; ++l)
			for (m = 0; m < numJobs; ++m)
				if(l!=m && Edges[l][m]!=0)
					for(w = 0; w < numPE; ++w) {
						for(IloInt w1=0;w1 < numPE; w1++)
							if(w == w1)
								model.add(IloEndBeforeStart(env, machines[w][l], machines[w][m]));
							else {
								model.add(IloEndBeforeStart(env, machines[w][l], machines[w1][m],Edges[l][m]));
							}
					}

*///-------------------


	    IloObjective objective = IloMinimize(env,IloMax(ends)+IloMax(ends1));
	    model.add(objective);
//	    cp.setParameter(IloCP::BranchLimit, 1000000);
	    cp.setParameter(IloCP::TimeLimit, timeLimit);
	    cp.setParameter(IloCP::RelativeOptimalityTolerance, 0.1);


	    cp.extract(model);
	    if (cp.solve()) {
			cp.out() << endl;
 			cp.out() << "Solution status = " << cp.getStatus() << endl;
        	cp.out() << "Solution value  = " << cp.getObjValue() << endl;
//        	cp.out() << "Makespan : " << cp.getObjValue() << std::endl;
	      this->makespan = cp.getObjValue();
			for(l = 0; l < numJobs; ++l) {
				cout<<"Job "<<l+1<<" ";
				for(w = 0; w < numPE; ++w) {
					this->allocation[l][w] =  cp.isPresent(machines[w][l]);
					cout<<cp.isPresent(machines[w][l])<<" ";
					if (this->allocation[l][w] == 1)
					{
						this->start_time[l][w] = cp.getStart(machines[w][l]);
						this->finish_time[l][w] = cp.getEnd(machines[w][l]);
						cout << "[ "<<cp.getStart(machines[w][l]) << "  " << cp.getEnd(machines[w][l]) << " ] ";
					}
				}
				cout<<"\n";
			}
			cout << "---------------------------" << endl;
			for(l=numJobs;l<2*numJobs;l++){
			    cout<<"Job_copy "<<l+1-numJobs<<" ";
				for(w=0;w<numPE;w++){
					cout <<  "---";
				    cout << cp.isPresent(machines[w][l]) << " ";
					if(cp.isPresent(machines[w][l])==1){
						cout << "[ "<<cp.getStart(machines[w][l]) << "  " << cp.getEnd(machines[w][l]) << " ] ";
					}
				}
				cout << endl;
			}
			
			for (l = 0; l < numJobs; ++l)
				for (m = 0; m < numJobs; ++m)
					if(l!=m && Edges[l][m]!=0)
						cout << l+1 <<" " << m+1 << " " << Edges[l][m] << endl;			
			
	    } else {
	      cp.out() << "No solution found."  << std::endl;
	    }
	}
	catch(IloException &e) {
		cerr << "Concert Exception caught: " << e << endl;
	}
	
	return true;
}
//IloIntervalVarArray jobs = IloIntervalVarArray(env, nJobs);
//
//IloIntExprArray ends(env);
//IloIntVarArray ends_min(env, nJobs);
//
//for (l = 0; l < nJobs; l++) {
////			IloIntervalVar job(env);
////			cout<<jobs<<"\n";
//	jobs[l] = IloIntervalVar(env);
//    jobs[l].setStartMin(0);
//
//    jobs[l].setEndMax(M);
//    sprintf(name, "Job%ld",l);
//	jobs[l].setName(name);
//
//    ends.add(IloEndOf(jobs[l]));
//
//
//    IloIntervalVarArray jobm(env, numPE);
//    IloIntExprArray endsmin(env);
//
//
//    for (w=0; w<numPE; w++) {
//      sprintf(name, "Alt%ld_%ld_C%ld",l,w,ExecutionCost[l][w]);
//      jobm[w] = IloIntervalVar(env, ExecutionCost[l][w]);
//      jobm[w].setName(name);
//      jobm[w].setOptional();
//      machines[w].add(jobm[w]);
//      endsmin.add(IloEndOf(jobm[w]));
//    }
//
////		    model.add(ends_min[l] == IloMin(endsmin));
//    endsmin.end();
//
////		    model.add(IloAlternative(env, jobs[l], jobm));
//    model.add(IloSpan(env, jobs[l], jobm));
//
//}



//		IloIntervalVarArray jobs = IloIntervalVarArray(env, nJobs);
//
//		IloIntExprArray ends(env);
//
//		IloIntVarArray ends_min(env, nJobs);
//
//		for (l = 0; l < nJobs; l++) {
////			jobs[l] = IloIntervalVar(env);
////		    jobs[l].setStartMin(0);
////
////		    jobs[l].setEndMax(M);
////		    sprintf(name, "Job%ld",l);
////			jobs[l].setName(name);
//
//		    IloIntervalVarArray jobm(env, numPE);
//		    IloIntExprArray present(env);
//
//
//		    for (w=0; w<numPE; w++) {
//		      sprintf(name, "Alt%ld_%ld_C%ld",l,w,ExecutionCost[l][w]);
//		      jobm[w] = IloIntervalVar(env, ExecutionCost[l][w]);
//		      jobm[w].setName(name);
//		      jobm[w].setOptional();
//		      machines[w].add(jobm[w]);
//		      present.add(IloPresenceOf(env,jobm[w]));
//		      ends.add(IloEndOf(jobm[w]));
//		    }
//
//		    model.add(IloSum(present) == 1);
//
//		    present.end();
//
////		    model.add(IloSpan(env, jobs[l], jobm));
//
//		}








