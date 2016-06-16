#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <list>
#include <vector>
#include <iostream>

using namespace std;

/* class to store allocation details of a node */

class timeline_node {
		public:
			timeline_node(int _id, int _start, int _finish) : node_id(_id), start_time(_start), finish_time(_finish) {}
			
			int node_id;
			float start_time;
			float finish_time;	

			bool operator < (const timeline_node& rhs) const
			{
				return (this->start_time < rhs.start_time);
			}
};

class Generic_Processor {
	public:
		int processor_id;                 	
		vector<timeline_node > timeline;              // for maintaining holes always in sorted order of timeline_node.start_time
		void print_timeline();
};

#endif
