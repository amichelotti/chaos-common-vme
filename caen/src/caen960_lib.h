#ifndef __CAEN265_H__
#define __CAEN265_H__
#include <stdint.h>

#define BOARD_ID 0x3
#define MANUFACTURE_ID 0x40
#define NCHANNELS 16
/* 
   status bits
*/
#define CAEN960_STATUS_DREADY 0x1 /* data ready */
#define CAEN960_STATUS_GDREADY 0x2 /* global data ready, at least one board has data ready */

#define CAEN960_STATUS_BUSY 0x4 /* board busy */
#define CAEN960_STATUS_GBUSY 0x8 /* global busy */

#define CAEN960_STATUS_PURGE 0x20 /* board purged */
#define CAEN960_STATUS_EVRDY 0x100 /* event ready */

/* 
   buffer status bits
*/
#define CAEN960_STATUS_BUFFER_EMPTY 0x2
#define CAEN960_STATUS_BUFFER_FULL 0x4


typedef void* caen960_handle_t;

/**
	open the caen265 device and return an handle to it
	@param address the vme address of the board
	@return an handle or zero if error
*/
caen960_handle_t caen960_open(uint32_t address);


/**
	close the caen265 device
	@param handle to the board
	@return zero if success
*/
int32_t caen960_close(caen960_handle_t handle);


/**
	init 960 device and return
	@param h handle to the board
	@param crate_num crate number associated to this device
	@param swreset if nonzero produce a Software Reset
	@return zero if success, 
*/
int32_t caen960_init(caen960_handle_t h,int32_t crate_num,int swreset);

/**
	set the Iped value
	@param h handle to the board
	@param ipedval iped value
	@return zero if success, 
*/
int32_t caen960_setIped(caen960_handle_t h,int32_t ipedval);


/**
	set the threashold (hires,lowres) value for a given channel
	@param h handle to the board
	@param lowres 8 bit value
	@param hires 8 bit value
	@param channel channel id
	@return zero if success, 
*/
int32_t caen960_setThreashold(caen960_handle_t h,int16_t lowres,int16_t hires,int channel);

/**
	get the threashold (hires,lowres) value for a given channel
	@param h handle to the board
	@param lowres 8 bit value
	@param hires 8 bit value
	@param channel channel id
	@return zero if success, 
*/
int32_t caen960_getThreashold(caen960_handle_t h,int16_t* lowres,int16_t* hires,int channel);


/**
	get the status register of the board
	@param h handle to the board
	@return the status
*/
uint16_t caen960_getStatus(caen960_handle_t h);

/**
	get the event count register
	@param h handle to the board
	@param reset if not zero reset this counter
	@return the counter
*/
uint32_t caen960_getEventCounter(caen960_handle_t h,int reset);

/**
	get the status register of the buffer
	@param h handle to the board
	@return the status
*/
uint16_t caen960_getBufferStatus(caen960_handle_t h);
/**
	acquire the number of channels given
	@param handle to the board
	@param lowres buffer (0:900pC)
	@param hires buffer (0:100pC)
	@param start_channel start channel to acquire
	@param nchannels number of channels to acquire
	@param timeo_ms timeout in ms
	@return the number of events acquired (0 no events)
*/

int32_t caen960_acquire_channels_poll(caen960_handle_t handle,uint32_t *lowres,uint32_t*hires,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms);

#ifdef LABVIEW
#include "extcode.h"
typedef struct {
        LVBoolean status;
        long code;
        LStrHandle source;
        } errorStruct;

/***
    LV data
 */
typedef struct{
  uint32_t dim;
  uint32_t arg[1];
} LV_vector_uint32_t;

/**
	close the caen265 device, to be used from LV 
	@param handle to the board
	@param error LV error struct
	@return zero if success
*/
int32_t caen960_LV_close(caen960_handle_t handle,errorStruct* error);

/**
	open the caen265 device and return an handle to it, to be used from LV with a master window already mapped
	@param address the mapped address of the board
	@param error LV error struct
	@return an handle or zero if error
*/
caen960_handle_t caen960_LV_open(uint32_t mapped_address,errorStruct* error);

/**
	acquire all the channels, to be used from LV
	@param handle to the board
	@param lowres buffer (0:900pC)
	@param hires buffer (0:100pC)
	@param event_under_run returns the difference from last and current acquire cycle -1 (0= no event under run, >0 = underrun, <0 no acquisition)
	@param timeo_ms timeout in ms
	@param error LV error struct
	@return the number of events acquired (0 no events, negative error)
*/
int32_t caen960_LV_acquire_channels_poll(caen960_handle_t handle,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct* error);

#endif

#endif
