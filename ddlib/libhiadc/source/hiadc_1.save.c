/*
 ------------------------------------------------------------------------------------------------
 compile command:
	gcc -Wall -o ./libhiadc.so.1.<version> ./source/hiadc_1.c -I/usr/local/lv70/cintools/ -shared

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
 ------------------------------------------------------------------------------------------------
*/

#include "extcode.h"

/* typedefs */
typedef struct {
  long dimSize;
  double arg1[1];
} TD1;
typedef TD1 **TD1Hdl;

/* local definitions */
#define	SCSR 0x40		/* Software Command and Status Register */
//#define CR   0x42		/* Control Register */
//#define SER  0x50		/* Strobe Enable Register Register */
//#define RDH1 0x00		/* Digitize Channel #1 (Held Mode) */
//#define RDT1 0x20		/* Digitize Channel #1 (Track Mode) */
#define GH_MASK 0x0f00		/* Mask for bits 8-11 (Group Is Held) of SCSR */
#define GS_MASK 0xf000		/* Mask for bits 12-15 (Group Was Strobed) of SCSR */
//#define STROBETIMEOUT	"strobeTimeout   "
//#define STROBEOVERRUN	"strobeOverrun   "
#define MAXWAIT_DEFAULT 2500000	/* max number of loops for strobe detection (~ 5 second on VMIC 7807) */
#define CHANNUM 16		/* number of ADC channels to be acquired */

/* funcions */
void readADC(unsigned long *baseAddress, long *maxWait, TD1Hdl rawData, long *to, long *or)
{
  /*
    -----------------------------------------------------------------------------
    Reads all the 16 channels of the IP-HiADC module located at "baseAddress".
    A polling loop keeps reading the 4 "Group Was Strobed" bits and terminates
    when all the 4 bits are set or the loop exceedes maxWait (t.o. condition).
    If no t.o. occourred the 16 channels are read and put back into tracking mode.
    Then it checks the Strobe Overrun and in case resets the ADC state machine.
    -----------------------------------------------------------------------------
  */

  short *localPtr;     /* with localPtr we access the VME memory */
  long i;

  *to = 0;
  *or = 0;
  if (*maxWait <= 0)
    *maxWait = MAXWAIT_DEFAULT;
  i = 0;

  localPtr = (short *)(*baseAddress + SCSR);
  while (((*localPtr & GH_MASK) != GH_MASK) && (i <= *maxWait)){  /* Wait Strobe with timeout */
    i++;
  }

  if (i > *maxWait) {
    *to = 1;
  }
  else {
    localPtr = (unsigned short*)(*baseAddress + 0x02);	/* Get Ch 01, Start Ch #02 */
    (*rawData)->arg1[0] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x04);	/* Get Ch 02, Start Ch #03 */
    (*rawData)->arg1[1] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x06);	/* Get Ch 03, Start Ch #04 */
    (*rawData)->arg1[2] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x08);	/* Get Ch 04, Start Ch #05 */
    (*rawData)->arg1[3] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x0A);	/* Get Ch 05, Start Ch #06 */
    (*rawData)->arg1[4] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x0C);	/* Get Ch 06, Start Ch #07 */
    (*rawData)->arg1[5] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x0E);	/* Get Ch 07, Start Ch #08 */
    (*rawData)->arg1[6] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x10);	/* Get Ch 08, Start Ch #09 */
    (*rawData)->arg1[7] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x12);	/* Get Ch 09, Start Ch #10 */
    (*rawData)->arg1[8] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x14);	/* Get Ch 10, Start Ch #11 */
    (*rawData)->arg1[9] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x16);	/* Get Ch 11, Start Ch #12 */
    (*rawData)->arg1[10] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x18);	/* Get Ch 12, Start Ch #13 */
    (*rawData)->arg1[11] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x3A);	/* Get Ch 13, Start Ch #14 */
    (*rawData)->arg1[12] = (int16) *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x3C);	/* Get Ch 14, Start Ch #15 */
    (*rawData)->arg1[13] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x3E);	/* Get Ch 15, Start Ch #16 */
    (*rawData)->arg1[14] = *localPtr;
    localPtr = (unsigned short*)(*baseAddress + 0x20);	/* Get Ch 16, Start Ch #01 */
    (*rawData)->arg1[15] = *localPtr;

  }

  /* test strobe overrun conditon */
  localPtr = (short *)(*baseAddress + SCSR);
  
  if ((*localPtr & GS_MASK) != 0) {
    *or = 1;
    *localPtr = 0x20;					/* ADC reset: all SH in tracking */
  }
  return;
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

