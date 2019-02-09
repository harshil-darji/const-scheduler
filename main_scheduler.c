#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

#include "queue.h"

struct process{

	unsigned int pid;
	signed int priority;
	int timeslice;
	int exec_time_reqd;

	struct process *next;
};	

//Global Variables
int pid_cnt=20;		
FILE* fp;
struct process *execute;
struct Queue *blockedQueue;
int base_priority = 120;	

//Function declarations 
int push_front(struct process **head);
void output( struct process **head );
void display(struct process **set, size_t n);
void scheduler(struct process **head, int itr);
void priority_finisher(struct process **head, int itr);
void access_proc(struct process **set );
int dynamic_priority_assigner(int bonus);
int bonus_decider();
int allot_timeslice(int priority);


struct Queue *q; 

int bitmap[40];


int push_front( struct process **head )
{
    struct process *new_process = malloc(sizeof(struct process));

    int success = new_process != NULL;    

    int bonus=rand()%10;

    if(success)
    {
		new_process->pid            = ++pid_cnt;						//sequentially asign PIDs to process	
		new_process->priority       = dynamic_priority_assigner(bonus);	//set as per given formula		
		new_process->exec_time_reqd = rand()%150;
		new_process->timeslice      = allot_timeslice(new_process->priority);
        new_process->next           = *head;
        *head = new_process;
    }

    return success;
}


void output( struct process **head )
{
    struct process *current = NULL;
	
	int bonus=rand()%10;

    for( current =*head; current != NULL; current = current->next )
    {
		printf(" %d->",current->pid);
        //printf(" %d,",current->priority);

	}	  
	printf(" NULL");
		
}

void display( struct process **set, size_t n )
{
	printf("\nActive run queue PIDs : \n");
    for ( size_t i = 0; i < n; i++ )
    {
		printf("%d ",i+100);
        output( set++ );
        putchar( '\n' );
    }
	printf("\nThese are the processes that come in order of their priorities...");
}

void scheduler(struct process **head, int itr)
{
	execute = malloc(sizeof(struct process));
 	
	blockedQueue = createQueue(); 
					
    struct process *current = NULL;
	
    for( current =*head; current != NULL; current = current->next )
    {
		printf("PID : %d ", current->pid);
		printf("Exec time : %d ", current->exec_time_reqd);
		printf("Timeslice : %d \n",current->timeslice);

		//Execution goin on
		execute->pid            = current->pid;					
		execute->priority       = current->priority;			
		execute->exec_time_reqd = current->exec_time_reqd;
		execute->timeslice      = current->timeslice;
		execute->next           = NULL;

		fprintf(fp,"PID %d running...",current->pid);
		//Sleep(5000);
			
		if(current->exec_time_reqd <= current->timeslice){
		
			fprintf(fp," PID %d done in %d millisecs.\n",current->pid, current->exec_time_reqd);
			
		}
		else
		{
			execute->exec_time_reqd = current->exec_time_reqd - current->timeslice;	
			fprintf(fp,"  PID %d still got to run %d millisecs.\n", execute->pid, execute->exec_time_reqd);
			fprintf(fp,"  PID %d added to blocked Queue.\n", execute->pid);
			enQueue(blockedQueue, execute->pid); 	
		}

	}	  
	printf(" NULL");
	
}

void priority_finisher(struct process **head, int itr)
{
	struct process* current = NULL;
	struct QNode *n = NULL;

	do{
	
	n = deQueue(blockedQueue); 

	if(n!=NULL)
	{
		for( current =*head; current != NULL; current = current->next )
		{
			if(n->key == current->pid)
			{
				
				if(current->exec_time_reqd <= current->timeslice)
				{
					fprintf(fp,"  PID %d done in %d millisecs.\n",current->pid, current->exec_time_reqd);
					fprintf(fp,"  PID %d removed from blocked Queue.\n", current->pid);
				}
				else
				{
					current->exec_time_reqd = current->exec_time_reqd - current->timeslice;	
					fprintf(fp,"   PID %d still got to run %d millisecs.\n", current->pid, current->exec_time_reqd);
					fprintf(fp,"   PID %d already in blocked Queue.\n", current->pid);

					while(current->exec_time_reqd > 0)
					{
						current->exec_time_reqd = 0;
						fprintf(fp,"   PID %d done and removed from blocked Queue.\n",current->pid, current->exec_time_reqd);
					}	
				}	
			}	
		}
	}

	}while(n!=NULL);

}

void access_proc(struct process **set )	//using bitmap
{
    int itr=0;

	while(itr!=40)
	{
		if(bitmap[itr]==1)
		{
			scheduler( set+itr , itr);
			priority_finisher( set+itr, itr );
			fprintf(fp,"All processes with priority %d executed successfully.\n\n",itr+100);
			printf("\n");	
		}
		itr++;
	}

}

int dynamic_priority_assigner(int bonus)	//Time-Sharing tasks
{
	//dynamic priority = MAX( 100, min(base_priority-bonus+5,139) )	

	int heuristic = (( (base_priority-bonus+5) < 139 )? (base_priority-bonus+5):139);
	int dynamic_priority = (heuristic>100)? heuristic:100;

	return dynamic_priority;
}

int bonus_decider()
{
	int bonus=rand()%10;
	return bonus;
}

int allot_timeslice(int priority)
{
	int timeslice;
	
	//more priority implies more timeslice to complete process...
	if(priority < 120)
		timeslice = (140-priority)*20;		//in millisecs
	else
		timeslice = (140-priority)*5;		//in millisecs

	return timeslice;	
}


#define N 40

int main()
{
	
	srand(time(NULL));
	
	fp = fopen("exec_process_log.txt","w+");


	struct process * active_run[N] = { 0 };

	struct process * expired_run[N] = { 0 };


	int i,proc_count=50,dp,bonus;

	for(i=0;i<N;i++)
	{
		bitmap[i] = 0;	//no process anywhere
	}
	printf("\n\n");
	
	for(i=0;i<proc_count;i++)
	{
		bonus = bonus_decider();
		dp = dynamic_priority_assigner(bonus);		
		push_front( &active_run[139-dp] );	
		bitmap[139-dp] = 1;
	}

	printf("Bitmap : ");
	for(i=0;i<N;i++)
	{
		printf("%d ",bitmap[i]);	
		fprintf(fp," %d ",bitmap[i]);	//prints to file
	}
	printf("\n\n");
	fprintf(fp,"\n\n");


	display( active_run, sizeof( active_run ) / sizeof( *active_run ) );

	printf("\n\n");

	access_proc( active_run );


	printf("\n\n");



	fclose(fp);
	return 0;
}
