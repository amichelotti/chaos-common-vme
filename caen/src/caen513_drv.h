#ifndef __CAEN513_H__
#define __CAEN513_H__
#include <stdint.h>
#define V513_CHANMODE_OUTPUT    0
#define V513_CHANMODE_INPUT     1
#define V513_CHANMODE_NEG       0
#define V513_CHANMODE_POS       2
#define V513_CHANMODE_IGLITCHED 0
#define V513_CHANMODE_INORMAL   4
#define V513_CHANMODE_TRANSP    0
#define V513_CHANMODE_EXTSTROBE 8

#define V513_STROBE_POLARITY_POS 0
#define V513_STROBE_POLARITY_NEG 1
#define V513_STROBE_ENABLE_INT   2
#define V513_STROBE_DISABLE_INT  0

#define MANUFACTURE_ID 0x832
#define NCHANNELS 16


 
typedef void* caen513_handle_t;

/**
	open the caen513 device and return an handle to it
	@param address the vme address of the board
	@return an handle or zero if error
*/
caen513_handle_t caen513_open(uint32_t address);


/**
	close the caen513 device
	@param handle to the board
	@return zero if success
*/
int32_t caen513_close(caen513_handle_t handle);


/**
	init 513 device and return
	@param h handle to the board
	@param defmode default mode of all channels in case of reset
	@return zero if success, 
*/
int32_t caen513_init(caen513_handle_t h,int defmode);



/**
	set the channel mode of a given channel
	@param h handle to the board
	@param channel channel id
	@param mode mode (I/O,Positive/Negative,Glitched/Normal,Trasparent/ExtStrobe)

*/
void caen513_setChannelMode(caen513_handle_t h,int channel,int mode);


/**
	get the channel mode of a given channel
	@param h handle to the board
	@param channel channel id
	@return the mode, 
*/

int32_t caen513_getChannelMode(caen513_handle_t h,int channel);


/**
	interrupt mask register, mask the channels 
	@param h handle to the board
	@param mask mask of channels
*/

void caen513_interruptMask(caen513_handle_t h,int mask);



/**
	set strobe
	@param h handle to the board
	@param mode

*/
void caen513_setStrobe(caen513_handle_t h,int mode);

/**
	get strobe
	@param h handle to the board
	@return the strobe status
*/
int32_t caen513_getStrobe(caen513_handle_t h);

/**
	get input pins
	@param h handle to the board
	@return the mask of input pins
*/
int32_t caen513_get(caen513_handle_t h);
/**
	set output pins
	@param h handle to the board
	@param mask the output pis
*/
void caen513_set(caen513_handle_t h,int mask);

/**
	reset the board
	@param h handle to the board

*/
void caen513_reset(caen513_handle_t h);

/**
   clear input register
	@param h handle to the board

*/
void caen513_clear(caen513_handle_t h);

/**
   clear strobe
	@param h handle to the board

*/
void caen513_clearStrobe(caen513_handle_t h);



/**
	get the status register of the board
	@param h handle to the board
	@return the status
*/
uint16_t caen513_getStatus(caen513_handle_t h);

/**
	get the event count register
	@param h handle to the board
	@param reset if not zero reset this counter
	@return the counter
*/
uint32_t caen513_getEventCounter(caen513_handle_t h,int reset);

/**
	get the status register of the buffer
	@param h handle to the board
	@return the status
*/
uint16_t caen513_getBufferStatus(caen513_handle_t h);


#endif
