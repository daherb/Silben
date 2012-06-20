/* defines and prototypes for the PVM library */
#include <pvm3.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include "message.h"
#include "matcher.hpp"

#define CORE_COUNT 2  // Number of Cores of a host
#define SYLLABLE_LIST_FILE (char *) "silbenliste.txt"

int main(int argc, char* argv[])
{
  // Own TID
  int mytid=pvm_mytid();

  // Lookup master-id from PVM
  int master_id;

  // Example sum
  unsigned long int sum1=0;
  unsigned long int sum2=0;
  unsigned long int oldsum=0;

  // Size of packages
  int data_size;

  // Timeout for Message Receive
  struct timeval tmout;
  tmout.tv_usec=100;
  tmout.tv_sec=0;

  // End of Data
  bool eod=false;

  // Error in operation
  bool err=false;

  pvm_lookup((char *)"master",0,&master_id);
  // Spawn if neccessary
  if (strcmp(argv[1],"1")==0)
    {
      // get local hostname
      char hostname[64];
      int pvm_tid=pvm_tidtohost(mytid);
      int hostcount;
      struct pvmhostinfo *hostp;
      pvm_config(&hostcount,NULL,&hostp);
      for (int ct = 0; ct < hostcount; ct++)
	{
	  if (hostp[ct].hi_tid==pvm_tid)
	    strcpy(hostname,hostp[ct].hi_name);
	}
      // Parameters for new spawn
      char *params[2]={(char *) "0", NULL};
      char *slave=(char *) "matcher_slave";
      int tids[CORE_COUNT];
      int spawncount=pvm_spawn(slave,params,PvmTaskHost,(char *) hostname, CORE_COUNT-1,tids);
    }

  // Read the frequency list
  init_matcher(SYLLABLE_LIST_FILE);

  // Send READY
  int buff=pvm_initsend(PvmDataDefault);
  pvm_pkint(&mytid,1,1);
  pvm_send(master_id,MSG_SLAVE_READY);

  while ((!eod)&&(!err))
    {
      // End on EOD
      if(pvm_probe(master_id,MSG_MASTER_EOD))
	{
	  pvm_recv(master_id,MSG_MASTER_EOD);
	  eod=true;
	}
      // Get data
      else
	{
	  // Data received
	  if (pvm_trecv(master_id,MSG_MASTER_DATA,&tmout)>0)
	    {
	      pvm_upkint(&data_size,1,1);
	      // Unpack data and work
	      /* to fill */
	      char word[64];
	      unsigned long int count;
	      for(int ct=0;ct<data_size;ct++)
		{
		  pvm_upkstr(word);
		  pvm_upkulong(&count,1,1);
		  int ret=match(new Pstring(word),count);
		  if (ret==-1)
		    {
		      buff=pvm_initsend(PvmDataDefault);
		      pvm_pkint(&mytid,1,1);
		      pvm_pkint(&ret,1,1);
		      pvm_send(master_id,MSG_SLAVE_ERROR);
		      err=true;
		    }
		}
	      if (!err)
		{
		  // Ready again
		  buff=pvm_initsend(PvmDataDefault);
		  pvm_pkint(&mytid,1,1);
		  pvm_send(master_id,MSG_SLAVE_READY);
		}
	    }
	}
    }
  if(!err)
    {
      // When finished send Result
      buff=pvm_initsend(PvmDataDefault);
      // Pack my id
      pvm_pkint(&mytid,1,1);
      // Pack Result element count
      data_size=1;
      pvm_pkint(&data_size,1,1);
      // Pack Result
      /* to fill */
      pvm_pkulong(&sum1,1,1);
      pvm_pkulong(&sum2,1,1);
      pvm_send(master_id,MSG_SLAVE_RESULT);
    }
  pvm_recv(master_id,MSG_MASTER_EXIT);
  pvm_exit();
  return 0;
}
