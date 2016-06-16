#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct request{
    int cylinder;
    struct request *next;
}request;

void print(request *list){
    request *temp = list;
    while(temp!=NULL){
        printf("%d ",temp->cylinder);
        temp = temp->next;
    }
    printf("\n");
}

int fcfs(request *list,int cylinders,int currentposition){
    request *temp=list;
    int current = currentposition;
    int distance=0;
    while(temp!=NULL){
        distance+=(abs(current-temp->cylinder));
        current = temp->cylinder;
        temp = temp->next;
    }
    return distance;
}

int sstf(request *list,int cylinders,int currentposition){
    request *temp=list,*prev,*mini,*prevmini;
    int current = currentposition;
    int distance = 0,min=999999999;
    if(list==NULL) return 0;
    while(temp!=NULL){
        temp = list;
        while(temp!=NULL){
            if(abs(temp->cylinder-current)<min){
                mini = temp;
                min = abs(temp->cylinder-current);
                prevmini = prev;
            }
            prev = temp;
            temp = temp->next;
        }
        //---- deleting request
        if(mini==list){ 
            list = list->next;
            distance = abs(mini->cylinder-current);
            current = mini->cylinder;
            mini->next = NULL;
        }
        else{
            prevmini->next = mini->next;
            distance = abs(mini->cylinder-current);
            current = mini->cylinder;
            mini->next = NULL;
        }
    }
//    printf("%d :",distance);
//    print(list);
    return distance+sstf(list,cylinders,current);
}

int scan(request *list,int cylinders,int currentposition){
    request *temp=list,*prev,*mini,*prevmini;
    int current = currentposition;
    int distance = 0,min=999999999;
    if(list==NULL) return 0;
    while(temp!=NULL){
        temp = list;
        mini=NULL;
        while(temp!=NULL){
            if( ((temp->cylinder-current)<0) && (abs(temp->cylinder-current)<min) ){
                mini = temp;
                min = abs(temp->cylinder-current);
                prevmini = prev;
            }
            prev = temp;
            temp = temp->next;
        }
        if(mini == NULL){
            temp = list;
            while(temp!=NULL){
                if( ((temp->cylinder-current)>0) && (abs(temp->cylinder-current)<min) ){
                    mini = temp;
                    min = abs(temp->cylinder-current);
                    prevmini = prev;
                }
                prev = temp;
                temp = temp->next;
            }            
        }
        //---- deleting request
        if(mini==list){ 
            list = list->next;
            distance = abs(mini->cylinder-current);
            current = mini->cylinder;
            mini->next = NULL;
        }
        else{
            prevmini->next = mini->next;
            distance = abs(mini->cylinder-current);
            current = mini->cylinder;
            mini->next = NULL;
        }
    }
//    printf("%d :",distance);
//    print(list);
    return distance+scan(list,cylinders,current);
}

void main(int argc,char *argv){
    int cylinders,currentposition,input;
    FILE *fp = fopen("input.txt","r");
    if(fp==NULL){ 
        printf("INPUT FILE NOT FOUND\n");    
        return;
    }
    FILE *fpo = fopen("output.txt","w");
    fscanf(fp,"%d",&cylinders);
    fscanf(fp,"%d",&currentposition);
    struct request *list = malloc(sizeof(request));
    struct request *list_copy = malloc(sizeof(request));
    struct request *temp = list,*prev;
    struct request *temp_copy = list_copy,*prev_copy;
    int count = 0,min = 999999999,flag=0;
    while(fscanf(fp,"%d",&input)==1){
        if((count++)>1){
            prev = temp;
            prev_copy = temp_copy;
        }
        if(input<min) min = input;
        if(input>currentposition) flag=1;
        temp->cylinder = input;
        temp_copy->cylinder = input;
        temp->next = malloc(sizeof(request));
        temp_copy->next = malloc(sizeof(request));
        temp = temp->next;
        temp_copy = temp_copy->next;
    }
    prev->next = NULL;
    prev_copy->next = NULL;
//    print(list);
//    print(list_copy);
    int d;
    d = fcfs(list,cylinders,currentposition);
    fprintf(fpo,"FCFS: %d\n",d);
    d = sstf(list,cylinders,currentposition);
    fprintf(fpo,"SSTF: %d\n",d);
    d = scan(list_copy,cylinders,currentposition)+2*min*flag;
    fprintf(fpo,"SCAN: %d\n",d);
}
