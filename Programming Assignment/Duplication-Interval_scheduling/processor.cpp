#include "processor.h"
#include <iostream>
#include <algorithm>

/* operator overloading to sort the elements in the timeline vector of the processor in terms of the start times
 */

//bool timeline_node :: operator < (const timeline_node& rhs) const
//{
//	return (this->start_time < rhs.start_time);
//}


/* to get the start_time of the node by insertion-based scheduling policy
 * pre requisite - the processor has atleast one node scheduled
 */
void Generic_Processor :: print_timeline()
{
	int size = timeline.size();
	
	if(size == 0)
	  return;
	
	for(int i=0; i < size;i++)
		cout<<timeline[i].node_id+1<<" [ "<<timeline[i].start_time<<" , "<<timeline[i].finish_time<<" ] ";
}
	
	
