#include<iostream>
#include<stdio.h>
#include<math.h>
using namespace std;
#define threshold 2     
//-- Threshold is a variable used to decide for the duplication of a task on multiple processor

//-- Main class for Task(process) \
Used to store the DAG for tasks and \
all other information required about task at run time
class task;

//-- Class to store the cost of each child of a task when they are not on the same processor
class childcost
{
    public:
    int pid;    //--- Process id
    float cost; 
    childcost *next;
    task *link;    
    childcost()
    {
      next = NULL;
      pid = 0; 
      cost = 0.0f;
    }
};

//-- Class to store communication cost between various processors
class communication
{ 
  public:
  int from;
  int to;
  float busy_transfer;
  float idle_transfer;
  communication *next;
  communication()
  {
    busy_transfer = 0, idle_transfer = 0, to = from =0;
    next = NULL;
  }
};

//-- Class to store execution cost of each task on each processor
class processorcost
{
  public:
  int pid;
  float executiontime; 
  int procid;
  float idle_energy,busy_energy;
  float extratime;
  processorcost *next;
  processorcost()
  {
   extratime = 0;
   pid = procid = 0; 
   executiontime = 0.0;
   next = NULL;
   busy_energy = 0;
   idle_energy = 0;
 
  }
};

//--- Class to store all the parameters concerned with a particular task
class parameters
{
   public:
   float s_time,e_time;
   int pid;
   float level;
   float est,ect,last,lact;
   task *fplink;    //--- Favourite Predecessor
   task *tlink;
   bool gp;
   parameters *next;
   parameters *prev;
   parameters()
   {
    s_time = e_time = 0.0f;
    prev = NULL;
    gp = false;
    level = 0;est = 0;ect = 0;last = 0;
    fplink = NULL;tlink = NULL;next = NULL;
   }
};

//---   Class to store all the parents of a process
class parentlist
{
  public:
  task *p;
  parentlist *next;
};

//-- Main class for Task(process) \
Used to store the DAG for tasks and \
all other information required about task at run time
class task
{
  public:
  int pid;
  childcost *cc;
  processorcost *pc;
  parameters *obj;
  task *next;
  parentlist *parent;
  task(){
    pid = 0;
    cc = NULL; 
    pc = NULL;
    next =  NULL;
    obj = NULL;
    parent = NULL;
  }
};

//--- Groups as mentioned in the algorithm are stored in this class
class Group
{
  public:
  int gid; 
  int proc_alloc;
  parameters *obj;
  Group *next;
  Group *down;
  Group *up;
  Group() 
  { 
   up = NULL;
   proc_alloc = -1;
   gid = 0;
   obj  = NULL;
   next = NULL;
   down = NULL;
  }
};

//--- Sorts tasks on the basis of their levels
parameters* bubblesort(task *root)
{
  parameters *rootpara=root->obj,*temp = root->obj,*one=NULL,*two,*three;
  if(root->obj->next ==NULL)
  {    
      cout<<"Less no of tasks"<<endl;  
      return rootpara;
  }
  else if(root->obj->next->next == NULL)
  {
     cout<<"Less no of tasks"<<endl;  
     return rootpara;
  }
  int flag = 0; 
  while(flag == 0)
  {
    int x;
    flag = 1;
    one = NULL; two = rootpara, three = rootpara->next; 
    while(three !=NULL)
    { 
          if(two->level > three->level)
          {
          
            if(one!=NULL) 
            {
               one->next = three;
               one = three;
             
            }   
            else
            {  
               one = three;
               rootpara = one;
            }
            two->next = three->next; 
            three->next = two;
            three = two->next;
            flag = 0;
            
          
          }
          else
          {
             if(one ==NULL)
             {
                one = two;
                rootpara = one;
             }
             else
                one  = one ->next;
             two = two -> next;
             three = three -> next;
          } 
                     
    }
  }
  return rootpara;
}

void print(task *root)
{
  task *iter = root;
  childcost *ccit;
  processorcost *pcit;
  while(iter != NULL)
  {
    cout<<"Task Pid is : "<<iter->pid<<" level : "<<iter->obj->level<<endl;
    pcit = iter->pc;
    cout<<" Processor Cost "<<endl;
    while(pcit!=NULL)
    { 
       cout<< pcit->pid<<" "<<pcit->executiontime<<" "<<pcit->procid<<" Extratime "<<pcit->extratime<<endl; 
       pcit = pcit -> next;
    }
    ccit = iter->cc;
    cout<<" ChildCost "<<endl;
    while(ccit!=NULL)
    {  
     cout<<ccit->pid<<" "<<ccit->cost<<endl;
     ccit = ccit->next;
    }
    iter = iter->next;
  }

}

//--- Compute level of task
float Level(task *root,int processor)
{ 
 
 processorcost * temp;
 if(root->cc == NULL)
 {
   temp = root->pc; 
   while(temp->procid!=processor)
     temp = temp->next;
   root->obj->level = temp->executiontime;
   return temp->executiontime;
 } 
 childcost *tempcc = root->cc;
 float max=0,temp1;
 while(tempcc!=NULL)
 {
  temp1 =  Level(tempcc->link,processor);
  if(temp1>max)
    max = temp1;
  tempcc = tempcc->next;
 }
  temp = root->pc;
  while(temp->procid!=processor)
     temp = temp->next;
  
  max += temp->executiontime;
  root->obj->level = max;
  return max;
}

//--- Compute favourite predecessor of each task
void fp(task *root)
{
  if(root->parent == NULL)
  {
      root->obj->fplink = NULL;
      return;
  }
   parentlist *pl = root->parent;
   task *maxtask = pl->p;
   childcost *ch = maxtask->cc;
   while(ch->pid != root->pid)
     ch = ch->next;
   float max_cost = ch->cost;
   while(pl!=NULL)
   {  
     ch = pl->p->cc;
     while(ch->pid !=root->pid)
          ch = ch->next;
     if(pl->p->obj->ect + ch->cost > maxtask->obj->ect + max_cost)
     {
       max_cost = ch->cost; 
       maxtask = pl->p;
     } 
     pl = pl->next;
   }
  
   root->obj->fplink = maxtask;
  cout<<" Process "<<root->pid<<" |-> FP : "<<maxtask->pid<<endl;
}

//--- Compute est and ect of each task
void estect(task *root,int processor)
{
   processorcost *temp;
   if(root->parent == NULL)
   {
       root->obj->est = 0;
           temp = temp->next;
       root->obj->ect = temp->executiontime; 
  }
  else
  {
     int min = 999999,max  = 0;
     parentlist *tempout = root->parent,*tempin = root->parent;
     while(tempout!=NULL)
     {   
       max = 0,min = 999999; 
       tempin = root->parent;
       while(tempin !=NULL)
       {
          if(tempin->p->pid != tempout->p->pid)
          {
             childcost *cch = tempin->p->cc;
             while(cch->pid != root->pid)
                  cch = cch->next;
             if(max < tempin->p->obj->ect+cch->cost)
                  max  = tempin->p->obj->ect+cch->cost;   
          }
            tempin = tempin->next;
       }
       if(max < tempout->p->obj->ect)
            max = tempout->p->obj->ect;
       
       if(min > max)
          min = max;
       tempout = tempout->next;
     }

     root->obj->est = min; 
     temp = root->pc;
     while(temp->procid!=processor)
          temp = temp->next;
     root->obj->ect = min + temp->executiontime;
   
  }
   cout<<" Process "<<root->pid<<" |->  ECT : "<<root->obj->ect<<", EST : "<<root->obj->est<<endl;
}

//--- Compute last and lact of each process
void lastlact(task *root, int processor)
{
   processorcost *temppc = root->pc;
   if(root->cc == NULL)
   {
      root->obj->lact = root->obj->ect; 
      while(temppc->procid !=processor)
         temppc = temppc->next;  
      root->obj->last = root->obj->lact - temppc->executiontime;
      return; 
   }
   float minnotfp = 999999,minfp = 999999,totalmin = 999999;
   childcost *ch = root->cc;
   while(ch!=NULL)
   {   
      lastlact(ch->link,processor);
      if(ch->link->obj->fplink->pid == root->pid)
      {
         if(minfp > ch->link->obj->last)
            minfp = ch->link->obj->last; 
      }
      else
      {
        if(minnotfp > ch->link->obj->last - ch->cost)
           minnotfp = ch->link->obj->last - ch->cost;
      }
      ch = ch->next;
   } 
   if(minnotfp < minfp)
      totalmin = minnotfp;
   else 
      totalmin = minfp;
  
   root->obj->lact = totalmin; 
   temppc = root->pc; 
   while(temppc->procid !=processor)
         temppc = temppc->next; 
   root->obj->last = totalmin - temppc->executiontime; 
}

//--- Method to group tasks on the basis of their levels
Group* Grouping(parameters *rootpara,communication *rootcom)
{
  Group *root,*temp = new Group(),*tmp;
  parameters *tempp = rootpara,*tmpp;
  
  int i = 1;
  while(tempp !=NULL)
  { 
      if(tempp->fplink ==NULL)
      {
        tempp = NULL;
        continue;
      }
      if(!tempp->gp)
      {
        // tempp->gp = true;
         
         if(i == 1) 
         {
            root = temp;
            temp->gid = i;
            temp->obj = tempp; 
            tmpp = tempp;
            tmp = temp;
          // cout<<" temp gid :"<<tempp->tlink->pid<<endl; 
            while(tmpp ->fplink !=NULL)
            {
              //cout<<tempp->fplink->pid<<endl;
                tmpp = tmpp->fplink->obj;
                if(!tmpp->gp)
                { 
                    tmpp->gp = true;
                    tmp->down = new Group();
                    tmp = tmp->down;
                    tmp->gid = i;
                    tmp->obj = tmpp;
                    tmp->next =NULL;
                  //cout<<" tmp -> gid "<<tmpp->fplink->pid<<endl;
                }   
                
            }  
         }         
         else
         {
           
           tempp->gp = true;
           temp->next = new Group(); 
           temp = temp->next;
           temp->gid = i;
           temp->obj = tempp;
           tmpp = tempp;
           tmp = temp; 
          // cout<<" temp  else gid :"<<tempp->tlink->pid<<endl; 
           while(tmpp ->fplink !=NULL)
           {
                  tmpp = tmpp->fplink->obj;
                if(!tmpp->gp)
                {  
                  
                    tmpp->gp = true;
                    tmp->down = new Group();
                    tmp = tmp->down;
                    tmp->gid = i;
                    tmp->obj = tmpp;
                    tmp->next =NULL;
                  //  cout<<" tmp -> gid "<<tmpp->tlink->pid<<endl;
                }   
                else
                {
                    //duplication wala code;
                    task *u ,*v = tmp->obj->tlink;
                    u = v->obj->fplink;
                    childcost *tmpcc = u->cc;
                    while(tmpcc->pid !=v->pid)
                          tmpcc = tmpcc->next;
                    if(v->obj->last - u->obj->lact < tmpcc->cost) 
                    {
                        // duplicate
                        communication *tempcom = rootcom;
                        float avg = 0;
                        int i = 0;
                        while(tempcom!=NULL)
                        { 
                           i++;
                           avg+=tempcom->busy_transfer;
                           tempcom = tempcom->next;
                        }
                         float el = avg/(i*1.0);
                         float en = 0;
                         processorcost *tpc = u->pc;
                         while(tpc->next!=NULL)
                              tpc = tpc->next;
                         en = tpc->executiontime;
                         if(fabs(en-el) < threshold)
                         {
                          tmp->down = new Group();
                          tmp = tmp->down;
                          tmp->gid = i;
                          tmp->obj = u->obj;
                          tmp->next =NULL;
                         }
                         else
                         {
                            while(u!=NULL)
                            { 
                                 processorcost *ctu = u->pc,*ctv = v->pc;
                                 while(ctu->next!=NULL)
                                 {    
                                      ctv->extratime+=ctu->executiontime;
                                      ctv = ctv->next;
                                      ctu = ctu->next;
                                 }
                                 u = u->obj->fplink; 
                            }
                         }
                    }
                    
                }
           }
         }
          i++;
      }    
      tempp = tempp->next;
  }
  return root;
}


class processor_timing_list
{
  public:
  int pid;
  int processorid;
  float starttime;
  float endtime;
  processor_timing_list *next;
  processor_timing_list *down;
  processor_timing_list()
  {
    processorid = -1;
    starttime = endtime = 0.0f;
    pid = 0;
    next = down = NULL;
  }  
};


//---- Allocating group to processors on the basis of energy
void gp_alloc(task *roottask,Group *root, int noofprocessors, parameters *rootpara)
{
     Group *tempgp = root;
     int i = 1,j = 1; float sum;
     float minsum = 999999;
     while(tempgp !=NULL)
     {   
        i = 1, minsum = 999999;
        while(i<=noofprocessors)
        {
            Group *downgp = tempgp;
            sum = 0;
            processorcost *pcost;
            while(downgp!=NULL)
            {
        	pcost = downgp->obj->tlink->pc;
        	processorcost *rcost = roottask->pc;
                j = 1; 
                while(j<i)
        	{
                        rcost = rcost->next;
           		pcost = pcost->next;
           		j++;
        	}     
                //cout<< " Execution Time "<<pcost->executiontime<<" busy "<<rcost->busy_energy<<" ";
        	sum+=(pcost->executiontime*rcost->busy_energy);
                downgp = downgp->down;
                //cout<<sum<<" ; ";
            }
	    //cout << endl;
            //cout<<i<< " "<<sum<<endl;
            if(sum<minsum)
            {
              minsum = sum; 
              tempgp->proc_alloc  = pcost->procid;
            }
            i++;
        }
        cout<<endl;
        tempgp = tempgp -> next;
        
     }  
     // reverse_paralist(rootpara);
     
     rootpara->prev = NULL;
     parameters *temppara1 = rootpara;
     parameters *prevpara1 = temppara1;  
     temppara1 = temppara1->next;
     while(temppara1!=NULL)
     {
       temppara1->prev = prevpara1;
       prevpara1 = temppara1; 
       temppara1 = temppara1->next;
     }
     
     //////////////////////////////
     
     parameters *temppara = rootpara;
     while(temppara->next != NULL)
         temppara = temppara->next;         // Last is the begining process.
     
         
     processor_timing_list *rootptl, *tempptl,*prevptl = NULL;
     for(int i = 0;i<noofprocessors;i++)
     {
       tempptl = new processor_timing_list();
       tempptl->processorid = i;
       tempptl->pid = -1;
       if(prevptl !=NULL)
       prevptl->next = tempptl; 
       prevptl = tempptl;
       tempptl->next = tempptl->down = NULL;
       if(i == 0)
          rootptl = tempptl;
     }
     int ak = 0;
     while(temppara!= NULL)        // For each process in order .. 
     { 
       
        // Find its processor id : 
        int proc_id;
        Group *tempgproot = root;
        while(tempgproot!=NULL)
        {  
           Group *temporary = tempgproot;
           while(temporary!=NULL)
           {    
              if(temporary->obj->pid == temppara->pid)
                 proc_id = tempgproot->proc_alloc;
              temporary = temporary -> down;
           }
           tempgproot = tempgproot->next;
        }
        
           
       // Allocating time to corresponding processor.
       tempptl = rootptl;
       while(tempptl->processorid != proc_id)
             tempptl = tempptl->next;
       processorcost *pctemp = temppara->tlink->pc;
       while(pctemp->procid != proc_id)
          pctemp = pctemp->next;
//       pctemp->procid = (pctemp->procid+1)%noofprocessors;   
       if(tempptl->pid !=-1)
       {
         while(tempptl->down!=NULL)
           tempptl = tempptl->down; 
         tempptl->down = new processor_timing_list();
         if(temppara->s_time > tempptl->endtime)
            tempptl->down->starttime = temppara->s_time;
         else   
            tempptl->down->starttime = tempptl->endtime;
         tempptl = tempptl->down;
         tempptl->pid = temppara -> pid;
         
       }
       else
        tempptl->pid = temppara->pid;
               
           
        
        // End time :
        float finishing_time;
        tempptl->endtime = tempptl->starttime + pctemp->executiontime;        
        finishing_time = tempptl->endtime;
        // children starttime :
        childcost *chcost = temppara->tlink->cc;
        while(chcost!=NULL)
        {
             // Find child :
             parameters *anothertemppara = temppara;
             while(anothertemppara->pid != chcost->pid)
                 anothertemppara = anothertemppara->prev;
             
             // Change its start time :    
             if(anothertemppara->s_time < finishing_time)
                anothertemppara->s_time = finishing_time;     
             chcost = chcost->next;
        }
       

       // Main loop last line :
        temppara = temppara->prev;
     }
     
     
     //Printing PTLs : 
      tempptl = rootptl;
      int ij = 0;
      while(tempptl !=NULL){
      
      cout<< " Processor No. "<<ij++<<endl;
      processor_timing_list *tempptl1 = tempptl;
      while(tempptl1!=NULL)
      {
         cout<<" Pid :  "<<tempptl1->pid<<" start time : "<<tempptl1->starttime<<" End time : "<<tempptl1->endtime<<endl;
         tempptl1 = tempptl1->down;
      }
      cout<<endl;
      tempptl = tempptl->next;
      }
}

int main()
{
  int noofprocessors,nooftasks,hyperperiod,no_tasks;
  scanf("%d",&hyperperiod);
  scanf("%d",&no_tasks);
  scanf("%d",&noofprocessors);
  int tid,trtime,tperiod,tdline;
  scanf("%d",&tid);
  scanf("%d",&trtime);
  scanf("%d",&tperiod);
  scanf("%d",&tdline);
  scanf("%d",&nooftasks);
  task *root_task= NULL,*temp = new task();
  processorcost *root_pc,*temp_pc = new processorcost();
  childcost *root_cc,*temp_cc = new childcost();
  parameters *root_para,*temp_para = new parameters();
  for(int i = 0;i<nooftasks;i++)
  {  
     if(i == 0){
       root_task = temp;
       root_para = temp_para; }
     temp_para->pid = i;
     temp_para->tlink = temp;
     temp->obj = temp_para;
     if(i==nooftasks-1)
        temp_para->next = NULL;
     else{ 
     temp_para->next = new parameters();
     temp_para = temp_para->next;}

     temp->pid = i;
     temp_pc = new processorcost();
     float sumexec = 0;
     for( int j = 0;j<noofprocessors;j++)
     {
       if(j == 0)
             root_pc = temp_pc;
       
        temp_pc->pid = i;
         temp_pc->procid = j;
          scanf("%f",&temp_pc->executiontime); 
          sumexec+=temp_pc->executiontime;
          if(j == noofprocessors-1)
          { 
            temp_pc->next = new processorcost();
            temp_pc = temp_pc->next; 
            temp_pc->procid = -1; 
            temp_pc->executiontime = sumexec / noofprocessors;
           // cout<<sumexec / noofprocessors<<" Average "<<endl;
            temp_pc->pid = i;
            temp_pc->next = NULL;
          }
          else{
          temp_pc->next = new processorcost(); 
          temp_pc = temp_pc->next;}

     }
     temp->pc = root_pc;
     if(i == nooftasks-1)
       temp->next = NULL;
     else{  
     temp->next = new task();
     temp = temp->next; }
      
  }   

  int sr,dest;
  float cost;  
  scanf("%d",&sr);
  temp = root_task;
  while(sr!=-1)
  {
    temp_cc = new childcost();
    scanf("%d",&dest);
    scanf("%f",&cost);
    
    temp_cc->pid = dest;
    temp_cc->cost = cost;
    task *tmp = root_task;
    while(tmp->pid != dest)
       tmp = tmp->next;
    temp_cc->link = tmp;

    temp_cc->next = NULL;
    if(temp->pid>sr)
      temp = root_task;
    while(temp->pid!=sr)
    {
      temp = temp->next;
      if(temp == NULL)
        break;
    }
    if(temp == NULL)
       continue; 
    if(temp->cc == NULL)
       temp->cc = temp_cc;
    else 
    {
      temp_cc->next = temp->cc;
      temp->cc = temp_cc; 
    }
    parentlist *temppl;
    temppl = new parentlist();
    temppl->p = temp;
    if(tmp->parent == NULL)
      tmp->parent = temppl;
    else{
     temppl->next = tmp->parent;
     tmp->parent = temppl;
    }


    temp_cc = NULL;
    scanf("%d",&sr);
  }
  temp_cc = NULL; 


// Only processors of root_task has idle and busy values correctly fed.  
  temp = root_task; 
  processorcost *pcpc = temp->pc;
  while(pcpc->next !=NULL)
  {
      scanf("%f",&pcpc->idle_energy);
      scanf("%f",&pcpc->busy_energy);
      pcpc = pcpc->next;
  } 
  

// For all tasks, calculated Level :  
  Level(root_task,-1);
 
// For all tasks, calculated ect and est :    
  temp = root_task; 
  while( temp!=NULL)
  {
    estect(temp,-1);
    temp = temp->next;
  }
  cout<<endl; 
 // For all tasks, calculated FP :
  temp = root_task;
  while(temp!=NULL)
  {
    fp(temp);  
    temp = temp->next;
  }
 //Calculated Lact and Last :
   lastlact(root_task,-1);  
  temp = root_task; 
  cout<<endl; 
  while(temp!=NULL)
  { 
    cout<<" Process " << temp->pid<< " |-> " <<" Lact : "<<temp->obj->lact<<", Last : "<<temp->obj->last<<endl;
    temp = temp->next;
  } 
  root_para = bubblesort(root_task);
  
  communication *comroot = new communication();
  communication *tempcom = comroot,*prevcom;
  int from,to;
  scanf("%d",&from);
  while(from!=-1)
  {
    scanf("%d",&to);
    tempcom->from = from;
    tempcom->to = to;
    scanf("%f",&tempcom->idle_transfer);
    scanf("%f",&tempcom->busy_transfer);
    tempcom->next = new communication();
    prevcom = tempcom;
    tempcom = tempcom->next;
    scanf("%d",&from);
   
  }  
  prevcom->next = NULL;

  Group *rootgp = Grouping(root_para,comroot);
  Group *tmpgp = rootgp,*downgp;
  int i = 1;
  cout<<endl;
  while(tmpgp!=NULL)
  {  
       downgp = tmpgp;
       cout<<" Group No. : "<<i++<<" |-> ";
       while(downgp!=NULL)
       {
             cout<<" "<<downgp->obj->tlink->pid;
             downgp = downgp->down;
       }
       cout<<endl;
       tmpgp = tmpgp->next;
  }

   gp_alloc(root_task,rootgp,noofprocessors,root_para);
  //print(root_task);
   /*Group *tempgp = rootgp;
   while(tempgp!=NULL)
   { 
         cout<<" Group -> proc_alloc "<<tempgp->proc_alloc<<endl;
         tempgp = tempgp->next;
   }*/

}













