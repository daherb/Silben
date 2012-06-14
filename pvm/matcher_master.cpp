/* defines and prototypes for the PVM library */
#include <pvm3.h>
#include <cstddef>
#include <cstdio>
#include <errno.h>
#include <limits.h>
#include "message.h"
#include "pair.hpp"

#define HOST_COUNT 2       // Number of Hosts in the Cluster
#define FREQ_LIST_FILE (char *) "frequenzliste.txt"

// Array of Word-Count-Pairs
Pair *freq_list[3250000];

// Number of Entries in Frequency List
unsigned long int freq_count;

// Read the syllables from a file into the vector
void read_freq_list(char *filename)
{
  // Reset counter
  freq_count=0;
  FILE *f=fopen(filename,"r");
  // Words should not be longer than 64 characters
  char line[64];
  unsigned long int count;
  while(fscanf(f,"%s\t%lu\n",line,&count)>0)
    {
      Pair *p=new Pair(Pstring(line));
      p->count=count;
      // freq_list.push(p);
      freq_list[freq_count]=p;
      freq_count++;
    }
}

int main(int argc, char* argv[])
{
  // Timeout for Message Receive
  struct timeval tmout;
  tmout.tv_usec=100;
  tmout.tv_sec=0;
  
  // Size for Data Packets  
  int data_size;

  // Numer of running tasks
  int curcount;

  // Check if all data sent
  bool finished;

  // TID of host to send data
  int slave_tid;

  // Data Buffer for outgoing messages
  int buff;

  // Read the frequency list
  read_freq_list(FREQ_LIST_FILE);

  // Total number of Data Elements
  int data_count=freq_count;

  // Current possition in the array
  int current_data=0;

  // Store master-id in PVM
  {
    int master_id=pvm_mytid();
    pvm_delete((char *) "master",0);
    pvm_insert((char *)"master",0,master_id);
    // Parameters for Spawn
    char *params[2]={(char *) "1", NULL};
    char *slave=(char *) "matcher_slave";
    int tids[HOST_COUNT];
    int spawncount=pvm_spawn(slave,params,PvmTaskDefault,NULL,HOST_COUNT,tids);
  }

  // Get number of current tasks
  pvm_tasks(0,&curcount,NULL);
  
  finished=false;
  do
    {
      // Check for error
      if (pvm_probe(-1,MSG_SLAVE_ERROR))
	{
	  char error[32];
	  int err_no=0;
	  pvm_recv(-1,MSG_SLAVE_ERROR);
	  pvm_upkint(&slave_tid,1,1);
	  pvm_upkint(&err_no,1,1);
	  printf("Fehler in Slave %d Code %d\n",slave_tid,err_no);
	}
      // Send Data or End-Of-Data
      else if(pvm_trecv(-1,MSG_SLAVE_READY,&tmout)>0)
	{
	  pvm_upkint(&slave_tid,1,1);		
	  // No more data
	  if (finished)
	    {
	      pvm_send(slave_tid,MSG_MASTER_EOD);
	    }
	  // Send data packet
	  else
	    {
	      buff=pvm_initsend(PvmDataDefault);

	      if (data_count>=PACK_SIZE)
		data_size=PACK_SIZE;
	      else
		data_size=data_count;
	      data_count-=data_size;
	      pvm_pkint(&data_size,1,1);
	      for(int ct=0;ct<data_size;ct++)
		{
		  // Store the data in the buffer
		  if (current_data<=freq_count)
		    {
		      pvm_pkstr(freq_list[current_data]->word.getValue());
		      pvm_pkulong(&(freq_list[current_data]->count),1,1);
		      current_data++;
		    }
		}
	      pvm_send(slave_tid,MSG_MASTER_DATA);
	      printf("Send %d sets of %d to %d\n",data_size,data_count,slave_tid);
	      if (data_count==0) 
		finished=true;
	    }
	}
      // Receive Result
      else if (pvm_probe(-1,MSG_SLAVE_RESULT))
	{
	  if (pvm_trecv(-1,MSG_SLAVE_RESULT,&tmout)>0)
	    {
	      pvm_upkint(&slave_tid,1,1);
	      // Number of elements
	      pvm_upkint(&data_size,1,1);	    
	      for (int ct=0;ct<data_size;ct++)
		{
		  unsigned long res1=0;
		  unsigned long res2=0;
		  // Read the result data
		  /* to fill */
		  // pvm_upkulong(&res1,1,1);
		  // pvm_upkulong(&res2,1,1);
		  // printf("Result %llu\n",(unsigned long long int)res2*ULONG_MAX+res1);
		}
	      pvm_send(slave_tid,MSG_MASTER_EXIT);
	    }
	}
      pvm_tasks(0,&curcount,NULL);
    }
  while(curcount>2);
  pvm_exit();
  return 0;
}
