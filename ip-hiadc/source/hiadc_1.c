/*
 ------------------------------------------------------------------------------------------------
 compile command:
	gcc -Wall -o ./libhiadc.so.1.<version> ./source/hiadc_1.c -I/usr/local/lv70/cintools/ -shared
	
	use make instead
	
 description	reads the IP-HiADC 16 Channel ADC IndustryPack module

 history  v b1.0 02/04/1999 by alessandro: basic routine for a first test.
	  v b2.0 13/03/2001 by alessandro: new error handling, new outputs added for
                            strobeTimeout and strobeOverrun
	  v b2.1 14/03/2001 by alessandro: no reset procedure at the beginning ADC state machine;
                            reset only if "strobe overrun".
	  v b3.0 22/03/2001 by alessandro: code organized as main and functions.
	  v b3.0.1 30/03/2001 by alessandro: no ADC reset procedure if "strobe t.o.".
	  v b3.1 04/04/2001 by alessandro: conversion to true voltage values.
	  v b3.2 04/04/2001 by alessandro: redefined raw 16 bit array as signed int16
                            for a correct bipolar readout. New pointer increment method
                            for VME write buffer operations.
	  v 4.0 29/04/2004 by alessandro&Francesco: input/output array changed to float64.
                           Check of input/output array and internal u16 array size.
	  v 4.1 30/07/2004 by alessandro&Francesco: cast with int16 in readADC for signed acquisition.
	  v 5.0 02/07/2004 by alessandro&Francesco: added new input for ADC range.
			   Fixed bug of ADC "range lost" due to soft reset after strobe o.r. in readADC.
	  v 5.1 16/10/2004 by ale: removed CH1 selection!
                           Now it is done at the end of the init routine "HiADC_initForFullRead.vi"
                           (see Example Code at Pag. 23 of the IP-HiAD User Manual).
                27/04/2005 by alessandro&Francesco: porting to gcc for LINUX.

	  v 1.3 13/01/2012 by Paolo Ciuffetti
			   Rewrite for use as a dinamic lybrary
	  v 1.4 29/08/2013 by Andrea Michelotti
			   use of volatile, unsigned short 
 ------------------------------------------------------------------------------------------------
*/

#include "extcode.h"
#include <sys/time.h>
/* typedefs */
typedef struct {
  long dimSize;
  double arg1[1];
} TD1;
typedef TD1 **TD1Hdl;

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define DPRINT(str,ARGS...) printf("[%llu] %s:" str,getUsTime(), __FUNCTION__, ##ARGS)

static unsigned long long getUsTime(){
 struct timeval tv;
 unsigned long long ret;
 static unsigned long long first_call=0;
 gettimeofday(&tv,NULL);
 ret = ((unsigned long long)tv.tv_sec) * 1000000LL;
 ret+= tv.tv_usec;
 if(first_call==0){
 	first_call = ret;
}
 return ret-first_call;
}
#else
#define DPRINT(str,ARGS...) 
#endif
#define PRINT(str,ARGS...) printf("*" str,##ARGS)

#define ERR(str,ARGS...) printf("# %s:" str,__FUNCTION__,##ARGS)

/* local definitions */
#define	SCSR 0x40		/* Software Command and Status Register */
#define CR   0x42		/* Control Register */
#define SER  0x50		/* Strobe Enable Register Register */
//#define RDH1 0x00		/* Digitize Channel #1 (Held Mode) */
//#define RDT1 0x20		/* Digitize Channel #1 (Track Mode) */
#define GH_MASK 0x0f00		/* Mask for bits 8-11 (Group Is Held) of SCSR */
#define GS_MASK 0xf000		/* Mask for bits 12-15 (Group Was Strobed) of SCSR */
//#define STROBETIMEOUT	"strobeTimeout   "
//#define STROBEOVERRUN	"strobeOverrun   "
#define MAXWAIT_DEFAULT 2500000	/* max number of loops for strobe detection (~ 5 second on VMIC 7807) */
#define CHANNUM 16		/* number of ADC channels to be acquired */

#define READ16(base,off) (*(volatile unsigned short*)(base+ off))
#define WRITE16(base,off,val) (*(volatile unsigned short*)(base+ off))=val

void initADC(unsigned long baseAddress){
	WRITE16(baseAddress,CR,0x16); // reset IP, 10V scale, Sign extend
	WRITE16(baseAddress,SCSR,0x20); // reset state machine
	WRITE16(baseAddress,SER,0xAA); // external strobe
	DPRINT("- Initialize SER=0x%x CR=0x%x status=0x%x\n",READ16(baseAddress,SER),READ16(baseAddress,CR),READ16(baseAddress,SCSR));
}
/* funcions */
void readADC(unsigned long *baseAddress, long *maxWait, TD1Hdl rawData, long *to, long *or)
{
	static int initialize=0;
#ifdef DEBUG
  static unsigned long long last_call_time=0;
#endif
	unsigned long baseA=*baseAddress;
	/*
    -----------------------------------------------------------------------------
    Reads all the 16 channels of the IP-HiADC module located at "baseAddress".
    A polling loop keeps reading the 4 "Group Was Strobed" bits and terminates
    when all the 4 bits are set or the loop exceedes maxWait (t.o. condition).
    If no t.o. occourred the 16 channels are read and put back into tracking mode.
    Then it checks the Strobe Overrun and in case resets the ADC state machine.
    -----------------------------------------------------------------------------
  */

  unsigned short status;
  long i;
  if(initialize==0){
  	initialize=1;
  	initADC(baseA);
  }
  *to = 0;
  *or = 0;
  if (*maxWait <= 0)
    *maxWait = MAXWAIT_DEFAULT;
  i = 0;
  
  DPRINT("- call time %Lu us\n",getUsTime()-last_call_time);
#ifdef DEBUG  
  last_call_time = getUsTime();
#endif
/* WRITE16(baseA,0,0);
	WRITE16(baseA,0,2);
WRITE16(baseA,0,4);
WRITE16(baseA,0,6);
*/

  do {
  	status = READ16(baseA,SCSR);
  	i++;
  	if(status!=0)
	  	DPRINT("status 0x%x\n",status);
  } while (((status & GH_MASK)!=GH_MASK) && (i <= *maxWait) && ((status & GS_MASK) == 0));
  
  *to = (i > *maxWait);
  *or = (status & GS_MASK)?1:0;
  DPRINT("- loop exit status %x, loops=%d, time spent in loop:%llu us, timeo:%d, overrun:%x\n",status,i,getUsTime()-last_call_time,*to,*or);
  if(status & GH_MASK){
  	// something in latch the channels
	(*rawData)->arg1[1]  = READ16(baseA,0x2);
	(*rawData)->arg1[2]  = READ16(baseA,0x4);
	(*rawData)->arg1[3]  = READ16(baseA,0x6);
	(*rawData)->arg1[4]  = READ16(baseA,0x8);
	(*rawData)->arg1[5]  = READ16(baseA,0xA);
	(*rawData)->arg1[6]  = READ16(baseA,0xC);
	(*rawData)->arg1[7]  = READ16(baseA,0xE);
	(*rawData)->arg1[8]  = READ16(baseA,0x10);
	(*rawData)->arg1[9]  = READ16(baseA,0x12);
	(*rawData)->arg1[10]  = READ16(baseA,0x14);
	(*rawData)->arg1[11]  = READ16(baseA,0x16);
    (*rawData)->arg1[12] = READ16(baseA,0x38); // group 1 track
    (*rawData)->arg1[13] = READ16(baseA,0x3A); // group 2 track
    (*rawData)->arg1[14] = READ16(baseA,0x3C); // group 3 track
	(*rawData)->arg1[15] = READ16(baseA,0x3E); // group 4 track
	(*rawData)->arg1[0]  = READ16(baseA,0x0);
	
#ifdef DEBUG
	for(i=0;i<16;i++){
		DPRINT("- channel %d = %.5f, status 0x%x\n",i, (*rawData)->arg1[i],READ16(baseA,SCSR));
	}
#endif

  } 
 	//if(*or || *to){
  	
  	/* ADC reset: all SH in tracking */
  	WRITE16(baseA,SCSR,0x20);
  //}
}

void readADC_dummy(unsigned long *baseAddress, long *maxWait, TD1Hdl rawData, long *to, long *or)
{
  /*
    -----------------------------------------------------------------------------
    Just for test.
    -----------------------------------------------------------------------------
  */
  long i;
  
  *to = 0;
  *or = 0;
  for (i = 0; i < CHANNUM; i++){
    (*rawData)->arg1[i] = i;
  }
  return;
}

void convertADC(TD1Hdl data, long *range)
{
  long i;
  double rawToVolt;
  
  if (*range == 0)
    rawToVolt = 5.0 / 2048.0;
  else if (*range == 1) 
    rawToVolt = 10.0 / 2048.0;
  else
    rawToVolt = 5.0 / 2048.0;
  
  for (i = 0; i < CHANNUM; i++) {
    (*data)->arg1[i] = (*data)->arg1[i] * rawToVolt;
  }
  return;
}

