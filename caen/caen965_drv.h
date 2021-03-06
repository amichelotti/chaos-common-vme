#ifndef __CAEN965_H__
#define __CAEN965_H__
#include <stdint.h>
/**
 * QDC
 * Andrea Michelotti
 * */
#include <common/vme/core/vmewrap.h>

typedef void* caen965_handle_t;

/**
	open the caen265 device and return an handle to it
	@param vme_driver id of the driver vme to be used
	@param address the vme address of the board
	@return an handle or zero if error
*/
caen965_handle_t caen965_open(vmewrap_vme_handle_t vme_driver,uint32_t address);


/**
	close the caen265 device
	@param handle to the board
	@return zero if success
*/
int32_t caen965_close(caen965_handle_t handle);


/**
	init 965 device and return
	@param h handle to the board
	@param crate_num crate number associated to this device
	@param swreset if nonzero produce a Software Reset
	@return zero if success, 
*/
int32_t caen965_init(caen965_handle_t h,int32_t crate_num,int swreset);

/**
	set the Iped value
	@param h handle to the board
	@param ipedval iped value
	@return zero if success, 
*/
int32_t caen965_setIped(caen965_handle_t h,int32_t ipedval);


/**
	set the threashold (hires,lowres) value for a given channel
	@param h handle to the board
	@param lowres 8 bit value
	@param hires 8 bit value
	@param channel channel id
	@return zero if success, 
*/
int32_t caen965_setThreashold(caen965_handle_t h,int16_t lowres,int16_t hires,int channel);

/**
	get the threashold (hires,lowres) value for a given channel
	@param h handle to the board
	@param lowres 8 bit value
	@param hires 8 bit value
	@param channel channel id
	@return zero if success, 
*/
int32_t caen965_getThreashold(caen965_handle_t h,int16_t* lowres,int16_t* hires,int channel);


/**
	get the status register of the board
	@param h handle to the board
	@return the status
*/
uint16_t caen965_getStatus(caen965_handle_t h);

/**
	get the event count register
	@param h handle to the board
	@param reset if not zero reset this counter
	@return the counter
*/
uint32_t caen965_getEventCounter(caen965_handle_t h,int reset);

/**
	get the status register of the buffer
	@param h handle to the board
	@return the status
*/
uint16_t caen965_getBufferStatus(caen965_handle_t h);
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

int32_t caen965_acquire_channels_poll(caen965_handle_t handle,uint32_t *lowres,uint32_t*hires,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms);

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
int32_t caen965_LV_close(caen965_handle_t handle,errorStruct* error);

/**
	open the caen265 device and return an handle to it, to be used from LV with a master window already mapped
	@param address the mapped address of the board
	@param error LV error struct
	@return an handle or zero if error
*/
caen965_handle_t caen965_LV_open(uint32_t mapped_address,errorStruct* error);

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
int32_t caen965_LV_acquire_channels_poll(caen965_handle_t handle,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct* error);

#endif

#endif
