#include<stdio.h>
#include<stdlib.h>

//--- Structure to save the input reference string
typedef struct reference_string{
    int page_number;    
    struct reference_string *next;
}reference_string;

 
int find(reference_string *frame,int page_num){
    reference_string *temp=frame;
    while(temp!=NULL){
        if(temp->page_number==page_num) return 1;
        temp = temp->next;
    }
    return 0;
}

int findarray(int array[],int page_num,int frames){
    int i=0;
    while(i<frames){
        if(array[i]==page_num) return 1;
        i++;
    }
    return 0;
}

reference_string* insertfront(reference_string *list,int page_num){
/*    reference_string *temp;
    temp = malloc(sizeof(reference_string));
    temp->page_number = page_num;
    temp->next = list;
    return temp;*/
    list->page_number = page_num;
}

reference_string* insertlast(reference_string *list,int page_num){
    reference_string *temp;
    temp = list;
    while(temp->page_number!=-1){ 
        if(temp->next==NULL) break;   
        temp = temp->next;
    }    
    temp->page_number = page_num;
/*    temp->next = malloc(sizeof(reference_string));
    temp = temp->next;
    temp->page_number = page_num;
*/    
    return list;
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
/*    reference_string *frame = malloc(sizeof(reference_string));
    int i=0;
    temp = frame;
    frame->page_number = -1;
    while( (++i) < frames){ 
        temp = temp->next = malloc(sizeof(reference_string));
        temp->page_number = -1;
    }*/
    int page_fault=0,count=0;
    temp = list;
    while(temp != NULL){
        if(findarray(frame,temp->page_number,frames)==0){
            page_fault++;
            frame[count++]=temp->page_number;
            count = count%frames;
        }
        else{
            
        }
        temp=temp->next;
        printf("frames:%d: ",count);
        printt(frame,frames);
    }
    return page_fault;
}

void main(){
    //--- Taking input
    int input,frames;
    struct reference_string *list,*temp;
    list = malloc(sizeof(reference_string));
    temp = malloc(sizeof(reference_string));
    temp=list;
    int count=0;
    struct reference_string *prev;
    while(scanf("%d",&input)==1){
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
//    print(list);
    temp = prev;
    prev = prev->next;
    frames = prev->page_number;
    temp->next = NULL;
    print(list);
    printf("%d\n",prev->page_number);
/*    list = insertfront(list,4);
    print(list);
    list = insertlast(list,3);
    print(list);
    list = insertfront(list,1);
    print(list);*/
    int d = fifo(list,frames);
    printf("Hello\n%d\n",d);
}
