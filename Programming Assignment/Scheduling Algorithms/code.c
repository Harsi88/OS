#include<stdio.h>
#include<stdlib.h>
typedef int bool;
#define true 1
#define false 0
#define context_switch 0.5

//--- Structure to hold process information

typedef struct process
{  
    float arr_time;     //--- Arrival time for a process
    float exe_time;     //--- Time required by CPU to execute this process
    float interrupted_time;   //-- Time when process was interrupted latest in time.
    float start_time;
    float exeb_time;
    struct process *prev,*next;
}process;

//--- Structure to hold performance information

typedef struct performance
{
    float av_wait_time;
    float av_turnaround_time;
}performance;

//--- List of methods

void print(process* );
void sort(process* );
performance* FCFS(process* );
performance* SRTF(process* );
performance* RR(process* , float );

//--- Method to print the list

void print(process *head)
{
    struct process *cur = head->next;
    printf("---%p---\n",head);
    while(cur!=NULL)
    {
        printf("---%f---%f---%p---%p---%p---\n",cur->arr_time,cur->exe_time,cur,cur->prev,cur->next);
        cur = cur->next;
    }
    return;
}

//--- Method to sort the list of processes on the basis of their arrival time

void sort(process *head)
{
    bool flag = false; //--- Variable to check if list is sorted
    struct process *cur = head->next, *prv, *nxt, *nxt2nxt;   //--- Pointer to list elements
    while(!flag)    //--- while list is not sorted
    {
        //--- Make flag true
        flag = true;
        while(cur->next->next!=NULL)    //--- until we reach the end of the list 
        {
            if(cur->arr_time > cur->next->arr_time)     //--- In case of inversion
            {
                //---Swap
                prv = cur->prev;
                nxt = cur->next;
                nxt2nxt = cur->next->next;
                cur->next = nxt2nxt;
                cur->prev = nxt;
                nxt->next = cur;
                nxt->prev = prv;
                prv->next = nxt;
                nxt2nxt->prev = cur;
                flag = false;       //---make flag false
            }
            else cur = cur->next;//--- move cur forward
        }
        cur = head->next;   //--- taking current pointer to start point again
    }
    return;
}

//--- Method to calculate performance of FCFS scheduling algorithm

performance* FCFS(process *head)
{
    process *cur = head->next;
    performance* perf = (performance*)malloc(sizeof(performance));
    perf->av_wait_time = 0;
    perf->av_turnaround_time = 0;
    float time = cur->arr_time, numProcess=0;
    while(cur!=NULL)
    {
        perf->av_wait_time += time - cur->arr_time;         //--- Adding waiting time for every process
        perf->av_turnaround_time += cur->exe_time + time - cur->arr_time;   //--- Adding Turnaround time for every process
        numProcess++;       //--- Counting number of processes
        time += cur->exe_time;  //--- Simulating time
        cur = cur->next;    
    }
    perf->av_wait_time /= numProcess;       //--- Calculate average waiting time
    perf->av_turnaround_time /= numProcess; //--- Calculate average turnaround time
    return perf;
}

//--- Method to calculate performance of SRTF scheduling algorihtm

performance* SRTF(process* head)
{
    process *cur = head->next, *ready_queue = (process*)malloc(sizeof(process));
    performance *perf = (performance*)malloc(sizeof(process));
    perf->av_wait_time = perf->av_turnaround_time = 0;
    float time = cur->arr_time, num_process=0;
    ready_queue->arr_time = ready_queue->exe_time = 0;
    ready_queue->prev = NULL;
    process *qit = ready_queue;         //--- Ready Queue iterator
    while(cur!=NULL || ready_queue->next!=NULL)
    {
        //--- Insert current process to ready-queue at its appropriate place on its remaining execution time
        process *temp,*running;
        bool inserted = false;      //--- Check whether insertion of new process in ready-queue is completed or not
        while(!inserted && cur!=NULL && time==cur->arr_time)
        {
            if(qit->exe_time > cur->exe_time)   //--- Inserting in-between
            {
                temp = (process*)malloc(sizeof(process));
                temp->arr_time = cur->arr_time;
                temp->exe_time = cur->exe_time;
                temp->interrupted_time = cur->interrupted_time;
                temp->start_time = cur->start_time;
                temp->prev = qit->prev;
                temp->prev->next = temp;
                temp->next = qit;
                qit->prev = temp;
                inserted = true;
                qit = ready_queue;
                num_process += 1;
                cur = cur->next;
            }
            if(qit->exe_time <= cur->exe_time && qit->next==NULL)  //--- Inserting at the end
            {
                temp = (process*)malloc(sizeof(process));
                temp->arr_time = cur->arr_time;
                temp->exe_time = cur->exe_time;
                temp->interrupted_time = cur->interrupted_time;
                temp->start_time = cur->start_time;
                temp->next = NULL;
                temp->prev = qit;
                qit->next = temp;
                inserted = true;
                qit = ready_queue;
                num_process += 1;
                cur = cur->next;
            }
            else
            {
                qit = qit->next;
            }
        }
        qit = ready_queue;
//        print(ready_queue);
        //--- Allot first process in the ready-queue to the processor and let it\
                run until it finishes or some new process arrives at ready-queue
        
        if(running==NULL && qit->next==NULL)
        {
//            printf("1\n");
            time = cur->arr_time;
        }                
        else if(running==NULL && qit->next!=NULL)
        {
//            printf("2\n");
            running = (process*)malloc(sizeof(process));
            running = qit->next;
            if(qit->next->next!=NULL) qit->next->next->prev = qit;
            qit->next = qit->next->next;
            running->start_time = time;
            running->prev = running->next = NULL;
            perf->av_wait_time += time - running->interrupted_time;
            if(cur==NULL || running->exe_time+time < cur->arr_time)
            {
//                printf("2.1\n");
                time = running->start_time + running->exe_time;
                perf->av_turnaround_time += time - running->arr_time;
                running = NULL;
            }
            else
            {
//                printf("2.2\n");
                time = cur->arr_time;
            }
        }
        else if(running!=NULL && qit->next==NULL)
        {
//            printf("3\n");
            if(running->exe_time+time < cur->arr_time)
            {
                time = running->start_time+running->exe_time;
                perf->av_turnaround_time += time - running->arr_time;
                running = NULL;
            }
            else
            {
                time = cur->arr_time;
            }
        }
        else    //--- running!=NULL
        {
//            printf("4\n");
            if((qit->next->exe_time<(running->exe_time + running->interrupted_time - time)))
            {
//                printf("4.1\n");
                // Context-switch
                time += context_switch;
                running->exe_time = running->exe_time - (time - running->start_time);
                running->interrupted_time = time;
                bool inserted = false;      //--- Check whether insertion of new process in ready-queue is completed or not
                while(!inserted)
                {
                    if(qit->exe_time > cur->exe_time)   //--- Inserting in-between
                    {
                        temp = (process*)malloc(sizeof(process));
                        temp->arr_time = running->arr_time;
                        temp->exe_time = running->exe_time;
                        temp->interrupted_time = running->interrupted_time;
                        temp->start_time = running->start_time;                        
                        temp->prev = qit->prev;
                        temp->prev->next = temp;
                        temp->next = qit;
                        qit->prev = temp;
                        inserted = true;
                        qit = ready_queue;
                        running = NULL;
                    }
                    if(qit->exe_time <= cur->exe_time && qit->next==NULL)  //--- Inserting at the end
                    {
                        temp = (process*)malloc(sizeof(process));
                        temp->arr_time = running->arr_time;
                        temp->exe_time = running->exe_time;
                        temp->interrupted_time = running->interrupted_time;
                        temp->start_time = running->start_time;                        
                        temp->next = NULL;
                        temp->prev = qit;
                        qit->next = temp;
                        inserted = true;
                        qit = ready_queue;
                        running = NULL;
                    }
                    else
                    {
                        qit = qit->next;
                    }
                }
                qit = ready_queue;
                running = (process*)malloc(sizeof(process));
                running = qit->next;
                running->start_time = time;
                qit->next->next->prev = qit;
                qit->next = qit->next->next;
                running->prev = running->next = NULL;
                perf->av_wait_time += time - running->interrupted_time;
            }
            else
            {
//                printf("4.2\n");
                // No context switch
                if(cur==NULL || running->exe_time+time < cur->arr_time)
                {
//                    printf("4.2.1\n");
                    time = running->start_time + running->exe_time;
                    perf->av_turnaround_time += time - running->arr_time;
                    running = NULL;
                }
                else
                {
//                    printf("4.2.2\n");
                    time = cur->arr_time;
                }
            }
        }
        
///        if(running!=NULL) printf("running %f %f %f\n",running->exe_time,running->start_time,running->interrupted_time);
 //           printf("time %f\n",time);
//            print(ready_queue);
//            printf("perf values---%f---%f\n",perf->av_wait_time,perf->av_turnaround_time);
//            printf("\n");
        //--- If new Process arrives at ready queue than-- \
                If new process will take less time than the remanining time\
                of current process than make a context switch otherwise let the\
                the current process run and put new process in ready queue.\
                While making a context switch keep in mind to change the remaining\
                execution time of previously running process.
        
        //--- Note that---> ready-queue is maintained in this way that processes are \
                sorted in ascending order of their remaining execution time
    }
    perf->av_wait_time /= num_process;       //--- Calculate average waiting time
    perf->av_turnaround_time /= num_process; //--- Calculate average turnaround time  
    return perf;
}

//--- round robin method

void enq(process* q_head,process* node)
{
    process *cur = q_head->next;
    q_head->next = node;
    node->prev = q_head;
    node->next = cur;
    if(cur!=NULL) cur->prev = node;
    return;
}

process* deq(process* q_head)
{
    process *cur = q_head->next;
    if(cur==NULL) return NULL;
    while(cur->next!=NULL) cur = cur->next;
    cur->prev->next = NULL;
    cur->prev = NULL;
    return cur;
}

bool notEmpty(process* q_head)
{
    if(q_head->next==NULL) return false;
    else return true;
}

performance* RR(process *head, float quanta)
{
    process* cur = head->next,*q = (process*)malloc(sizeof(process)),*temp,*running;
    q->prev = q->next = NULL;
    performance *perf = (performance*)malloc(sizeof(process));
    perf->av_wait_time = perf->av_turnaround_time = 0;
    float time = cur->arr_time, num_process=0;
    while(cur->arr_time <= time)
    {
    temp = NULL;
    temp = (process*)malloc(sizeof(process));
    temp->arr_time = cur->arr_time;
    temp->exe_time = cur->exe_time;
    temp->interrupted_time = cur->interrupted_time;
    temp->start_time = cur->start_time;
    temp->prev = temp->next = NULL;
    enq(q,temp);
    cur = cur->next;
    num_process++;
    }
    while(cur!=NULL || notEmpty(q))
    {
        
        running = (process*)malloc(sizeof(process));
        running = deq(q);
  
        if(running == NULL)
        {
                time = cur->arr_time;
        }
        else
        {
        if(running->exe_time <= quanta)
        {
            time += running->exe_time;
            perf->av_wait_time += (time - running->arr_time - running->exeb_time);
            perf->av_turnaround_time += time + running->arr_time;
            if(!notEmpty && time <= cur->arr_time)
            {
                time = cur->arr_time;
            }
        }
        else
        {
            time += quanta;
            printf("%f\n",quanta);
            if(running!=NULL) running->exe_time -= quanta;
            temp = NULL;
            temp = (process*)malloc(sizeof(process));
            temp->arr_time = running->arr_time;
            temp->exe_time = running->exe_time;
            temp->interrupted_time = running->interrupted_time;
            temp->start_time = running->start_time;
            temp->prev = temp->next = NULL;
            enq(q,temp);
            time += context_switch;
        }
        }
        while(1)
        {
            if(cur==NULL || cur->arr_time > time ) break;
            temp = NULL;
            temp = (process*)malloc(sizeof(process));
            temp->arr_time = cur->arr_time;
            temp->exe_time = cur->exe_time;
            temp->interrupted_time = cur->interrupted_time;
            temp->start_time = cur->start_time;
            temp->prev = temp->next = NULL;
            enq(q,temp);
            if(cur!=NULL) cur = cur->next;
            num_process++;
            if(cur==NULL) break;
        }
        printf("%f %f\n",time,num_process);
    }
    perf->av_turnaround_time /= num_process;
    perf->av_wait_time /= num_process;
    return perf;
}

//--- Main Method

int main(int argc, char* argv[])
{   
    if(argc < 2)    printf("Error: Input File Not Found!!\n");      //--- Checking if input file is given
    else
    {
        struct process *head = (process*)malloc(sizeof(process));   //--- Pointer to list of all the processes
        head->prev = head->next = NULL;                              
        head->next = (process*)malloc(sizeof(process));
        struct process *cur = head->next;                           //--- Pointer to current element in the list
        cur->next = NULL;
        cur->prev = head;
        
        //--- Taking input
        
        FILE *fp = fopen(argv[1],"r");
        while(fscanf(fp,"%f",&cur->arr_time)==1)
        {
            fscanf(fp,"%f",&cur->exe_time);
            cur->exeb_time = cur->exe_time;
            cur->start_time = cur->interrupted_time = cur->arr_time;
            cur->next = (process*)malloc(sizeof(process));
            cur->next->prev = cur;
//            printf("%f %f %p\n",cur->arr_time,cur->exe_time,cur->next);
            cur = cur->next;
//            int d;
//            scanf("%d",&d);
        }
        //--- Deleting last unnecessary node created in the list
        cur = cur->prev;
        cur->next->prev = NULL;
        cur->next = NULL;
        
        //--- Input Taken
        
        sort(head);
//        print(head);
        
//        FILE *op = fopen("output.txt","w+");
        performance *perf = FCFS(head);
//        fprintf(op,"%f %f \n",perf->av_wait_time,perf->av_turnaround_time);
        perf = SRTF(head);        
//        fprintf(op,"%f %f \n",perf->av_wait_time,perf->av_turnaround_time);
        perf = RR(head,50);        
        printf("%f %f\n",perf->av_wait_time,perf->av_turnaround_time);
    }
}
