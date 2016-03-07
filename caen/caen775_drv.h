#ifndef __CAEN775_H__
#define __CAEN775_H__
#include <stdint.h>
/**
 * TDC
 * Andrea Michelotti
 * */

#ifdef __cplusplus
extern "C" {
#endif
#define CAEN775_FSR_OFF 1060
/**
 * available modes
 */

typedef enum caen775_modes {
	CAEN775_MEMORY_TEST=0x1,
	CAEN775_ADC_OFFLINE=(1<<1),
	CAEN775_CLEAR_DATA=(1<<2),
	CAEN775_ACCEPT_OVER_RANGE=(1<<3),
	CAEN775_THRESHOLD=(1<<4),
	CAEN775_VALID_DISABLE=(1<<5),
	CAEN775_TEST_ACQ=(1<<6),
	CAEN775_SLIDE_ENABLE=(1<<7),
	CAEN775_TH_RESOLUTION=(1<<8),
	CAEN775_COMMON_STOP=(1<<10),
	CAEN775_AUTO_INCR=(1<<11),
	CAEN775_EMPTY_PROG=(1<<12),
	CAEN775_ALL_TRIGGER=(1<<14)
} caen775_modes_t;




typedef void* caen775_handle_t;

/**
	open the device and return an handle to it
	@param address the vme address of the board
	@return an handle or zero if error
*/
caen775_handle_t caen775_open(uint32_t address);


/**
	close the device
	@param handle to the board
	@return zero if success
*/
int32_t caen775_close(caen775_handle_t handle);


/**
	init device and return
	@param h handle to the board
	@param crate_num crate number associated to this device
	@param swreset make sw reset
	@return zero if success,
*/
int32_t caen775_init(caen775_handle_t h,int32_t crate_num,int swreset);


/**
	enable modes
	@param h handle to the board
	@param modes to be enabled
	@return zero if success, 
*/
int32_t caen775_enable_mode(caen775_handle_t h,caen775_modes_t modes);

/**
	disable modes
	@param h handle to the board
	@param modes to be disabled
	@return zero if success,
*/
int32_t caen775_disable_mode(caen775_handle_t h,caen775_modes_t modes);


/**
	set the threashold (hires,lowres) value for a given channel
	@param h handle to the board
	@param value 8 bit value threashold to compare with the 8MSB of the 12 ADC
	@param kill if true don't memorize channel
	@param channel channel id
	@return zero if success, 
*/
int32_t caen775_setThreashold(caen775_handle_t h,int16_t value,int kill,int channel);

/**
	get the status register of the board
	@param h handle to the board
	@return the status
*/
uint16_t caen775_getStatus(caen775_handle_t h);

/**
	get the event count register
	@param h handle to the board
	@param reset if not zero reset this counter
	@return the counter
*/
uint32_t caen775_getEventCounter(caen775_handle_t h,int reset);

/**
	get the status register of the buffer
	@param h handle to the board
	@return the status
*/
uint16_t caen775_getBufferStatus(caen775_handle_t h);
/**
	acquire the number of channels given
	@param handle to the board
	@param chan_data buffer
	@param timeo_ms timeout in ms
	@return the number of events acquired (0 no events)
*/

int32_t caen775_acquire_channels_poll(caen775_handle_t handle,uint32_t *chan_data,uint64_t *cycle,int timeo_ms);

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
int32_t caen775_LV_close(caen775_handle_t handle,errorStruct* error);

/**
	open the caen265 device and return an handle to it, to be used from LV with a master window already mapped
	@param address the mapped address of the board
	@param error LV error struct
	@return an handle or zero if error
*/
caen775_handle_t caen775_LV_open(uint32_t mapped_address,errorStruct* error);

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
int32_t caen775_LV_acquire_channels_poll(caen775_handle_t handle,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct* error);

#endif

#ifdef __cplusplus
}
#endif

#endif
