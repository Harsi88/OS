#include<stdio.h>
#include<stdlib.h>

//--- Structure to save the input reference string
typedef struct reference_string{
    int page_number;    
    struct reference_string *next;
}reference_string;

int findarray(int array[],int page_num,int frames){
    int i=0;
    while(i<frames){
        if(array[i]==page_num) return i;
        i++;
    }
    return -1;
}

//------------------------------------------------------------------

void print(reference_string *list){
    reference_string *temp = list;
    while(temp!=NULL){
        printf("%d ",temp->page_number);
        temp = temp->next;
    }
    printf("\n");
}

void printt(int a[],int length){
    int i=0;
    while(i<length){
        printf("%d ",a[i]);
        i++;
    }
    printf("\n");
}

int fifo(reference_string *list,int frames){
    reference_string *temp; 
    int frame[frames];
    int i=0;
    for(i=0;i<frames;i++)   frame[i] = -1;
    int page_fault=0,tobereplaced=0;
    temp = list;
    while(temp != NULL){
        if(findarray(frame,temp->page_number,frames)==-1){
            page_fault++;
            frame[tobereplaced++]=temp->page_number;
            tobereplaced = tobereplaced%frames;
        }
        else{
            
        }
        temp=temp->next;
//        printf("frames:%d: ",tobereplaced);
//        printt(frame,frames);
    }
    return page_fault;
}

int optimal(reference_string *list,int frames){
    reference_string *temp; 
    int frame[frames],fut[frames],time[frames];
    int i=0;
    for(i=0;i<frames;i++)   frame[i] = -1;
    for(i=0;i<frames;i++)   time[i] = 0;
    int page_fault=0,count=0,tobereplaced=0,futnumber,hit,num9,maxdistance,maxtime;
    temp = list;
    while(temp != NULL){
        for(i=0;i<frames;i++)   time[i] += 1;
        if(findarray(frame,temp->page_number,frames)==-1){
            page_fault++;
            reference_string *tmp = temp->next;
            tobereplaced=0;
            for(i=0;i<frames;i++){
                futnumber = 1;
                hit = 0;
                tmp = temp->next;
                while(tmp!=NULL){
                    futnumber++;
                    if(tmp->page_number==frame[i]){
                        fut[i] = futnumber;
                        hit = 1;
                        break;
                    }
                    tmp = tmp->next;
                }
                if(hit==0) fut[i] = 999999999;
            }
            num9=0;
            maxdistance=0;
            for(i=0;i<frames;i++){
                if(fut[i]==999999999){
                    num9++;
                    maxdistance = 999999999;
                    tobereplaced = i;
                }
                else if(fut[i]>maxdistance){
                    maxdistance = fut[i];
                    tobereplaced = i;
                }
            }
            if(num9>1){
                //--- Apply fifo to them
                maxtime=0;
                for(i=0;i<frames;i++){
                    if(fut[i]==999999999){
                        if(frame[i]==-1){
                            tobereplaced = i;
                            break;
                        }
                        if(time[i]>maxtime){
                            maxtime = time[i];
                            tobereplaced = i;
                        }
                    }
                }
                frame[tobereplaced]=temp->page_number;
                time[tobereplaced] = 0;
            }
            else{
                frame[tobereplaced]=temp->page_number;
                time[tobereplaced] = 0;
            }
        }
        else{
            time[findarray(frame,temp->page_number,frames)] = 0;
        }
        temp=temp->next;
/*      printf("frames:%d: ",count++);
        printt(frame,frames);
*/
    }
    return page_fault;
}

int lu(reference_string *list,int frames){
    reference_string *temp; 
    int frame[frames],used[frames],time[frames];
    int i=0,nummin,minused,min,maxtime;
    for(i=0;i<frames;i++)   frame[i] = -1;
    for(i=0;i<frames;i++)   used[i] = 0;
    for(i=0;i<frames;i++)   time[i] = 0;
    int page_fault=0,tobereplaced=0;
    temp = list;
    while(temp != NULL){
        for(i=0;i<frames;i++) time[i]++;
        if(findarray(frame,temp->page_number,frames)==-1){
            page_fault++;
            nummin=0;
            minused=0;
            min=1;
            for(i=0;i<frames;i++){
                if(used[i]<min){
                    min=used[i];
                    nummin = 1;
                    minused = i;
                }
                else if(used[i]==min) nummin++;
            }
            if(nummin>1){
                //--- Apply FIFO replacement policy
                maxtime = 0;
                for(i=0;i<frames;i++){
                    if(used[i]==min){
                        if(frame[i]==-1){
                            minused = i;
                            break;
                        }
                        if(maxtime<time[i]){
                            minused = i;
                            maxtime = time[i];
                        }
                        
                    }
                }
                tobereplaced = minused;   
            }
            else if(nummin=1){
                tobereplaced = minused;
            }
            frame[tobereplaced]=temp->page_number;
            time[tobereplaced] = 0;
            used[tobereplaced]=1;
            
        }
        else{
            used[findarray(frame,temp->page_number,frames)]+=1;
        }
        temp=temp->next;
    }
    return page_fault;
}
 
int lru(reference_string *list,int frames){
    reference_string *temp; 
    int frame[frames],used[frames],time[frames];
    int i=0,nummin,minused,min,maxtime;
    for(i=0;i<frames;i++)   frame[i] = -1;
    for(i=0;i<frames;i++)   used[i] = 0;
    for(i=0;i<frames;i++)   time[i] = 0;
    int page_fault=0,tobereplaced=0;
    temp = list;
    while(temp != NULL){
        for(i=0;i<frames;i++) time[i]++;
        if(findarray(frame,temp->page_number,frames)==-1){
            page_fault++;
            tobereplaced=0;
            maxtime = 0;
            for(i=0;i<frames;i++){
                if(frame[i]==-1){
                    tobereplaced = i;
                    break;
                }
                if(maxtime<time[i]){
                    tobereplaced = i;
                    maxtime = time[i];
                }
            }
                
            frame[tobereplaced]=temp->page_number;
            time[tobereplaced] = 0;
            used[tobereplaced]=1;
            
        }
        else{
            time[findarray(frame,temp->page_number,frames)]=0;
        }
        temp=temp->next;
//        printt(frame,frames);
//        printf("\n");
    }
    return page_fault;
}


void main(){
    //--- Taking input
    int input,frames;
    FILE *fp = fopen("input.txt","r");
    if(fp==NULL){ 
        printf("INPUT FILE NOT FOUND\n");    
        return;
    }
    FILE *fpo = fopen("output.txt","w");
    struct reference_string *list,*temp;
    list = malloc(sizeof(reference_string));
    temp = malloc(sizeof(reference_string));
    temp=list;
    int count=0;
    struct reference_string *prev;
    while(fscanf(fp,"%d",&input)==1){
        count++;
        if(count==2){
            prev = list;
        }
        else if(count>2){
            prev = prev->next;
        }
        temp->page_number = input;
        temp->next = malloc(sizeof(reference_string));
        temp = temp->next;
    }
    temp = prev;
    prev = prev->next;
    frames = prev->page_number;
    temp->next = NULL;
//    print(list);
//    printf("%d\n",prev->page_number);
    int d;
    d = fifo(list,frames);
    fprintf(fpo,"fifo: %d\n",d);
    d = optimal(list,frames);
    fprintf(fpo,"optimal: %d\n",d);
    d = lru(list,frames);
    fprintf(fpo,"lru: %d\n",d);
}
