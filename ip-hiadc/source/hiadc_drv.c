/*
 ------------------------------------------------------------------------------------------------
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

#include <sys/time.h>
#include "vmewrap.h"
/* local definitions */
#define	SCSR 0x40		/* Software Command and Status Register */
#define CR   0x42		/* Control Register */
#define SER  0x50		/* Strobe Enable Register Register */

#define GH_MASK 0x0f00		/* Mask for bits 8-11 (Group Is Held) of SCSR */
#define GS_MASK 0xf000		/* Mask for bits 12-15 (Group Was Strobed) of SCSR */

#define MAXWAIT_DEFAULT 2500000	/* max number of loops for strobe detection (~ 5 second on VMIC 7807) */
#define CHANNUM 16		/* number of ADC channels to be acquired */

void hiadc_init(unsigned long baseAddress){
	WRITE16(baseAddress,CR,0x16); // reset IP, 10V scale, Sign extend
	WRITE16(baseAddress,SCSR,0x20); // reset state machine
	WRITE16(baseAddress,SER,0xAA); // external strobe
	DPRINT("- Initialize SER=0x%x CR=0x%x status=0x%x\n",READ16(baseAddress,SER),READ16(baseAddress,CR),READ16(baseAddress,SCSR));
}

void hiadc_acquire(unsigned long baseA, unsigned short* channels, int time_ms,long *to, long *or){
  static unsigned long long last_call_time;
  unsigned short status;
  int i=0;
  *to = 0;
  *or = 0;
  
  
  DPRINT("- call time %Lu us\n",getUsTime()-last_call_time);
  last_call_time = getUsTime();
  do {
  	status = READ16(baseA,SCSR);
  	i++;
  	if(status!=0)
	  DPRINT("status 0x%x\n",status);
	*to = (time_ms >(getUsTime()-last_call_time))?1:0;
  } while (((status & GH_MASK)!=GH_MASK) && (*to==0) && ((status & GS_MASK) == 0));
  
  *or = (status & GS_MASK)?1:0;
  DPRINT("- loop exit status %x, loops=%d, time spent in loop:%llu us, timeo:%d, overrun:%x\n",status,i,getUsTime()-last_call_time,*to,*or);
  if(status & GH_MASK){
    // something in latch the channels
    channels[1]  = READ16(baseA,0x2);
    channels[2]  = READ16(baseA,0x4);
    channels[3]  = READ16(baseA,0x6);
    channels[4]  = READ16(baseA,0x8);
    channels[5]  = READ16(baseA,0xA);
    channels[6]  = READ16(baseA,0xC);
    channels[7]  = READ16(baseA,0xE);
    channels[8]  = READ16(baseA,0x10);
    channels[9]  = READ16(baseA,0x12);
    channels[10]  = READ16(baseA,0x14);
    channels[11]  = READ16(baseA,0x16);
    channels[12] = READ16(baseA,0x38); // group 1 track
    channels[13] = READ16(baseA,0x3A); // group 2 track
    channels[14] = READ16(baseA,0x3C); // group 3 track
    channels[15] = READ16(baseA,0x3E); // group 4 track
    channels[0]  = READ16(baseA,0x0);
    
#ifdef DEBUG
    for(i=0;i<16;i++){
      DPRINT("- channel %d = %.5f, status 0x%x\n",i, channels[i],READ16(baseA,SCSR));
    }
#endif
    
  } 
 	//if(*or || *to){
  	
  	/* ADC reset: all SH in tracking */
  	WRITE16(baseA,SCSR,0x20);
  //}
}
