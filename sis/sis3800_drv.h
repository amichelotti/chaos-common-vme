#ifndef __SIS3800_H__
#define __SIS3800_H__
#include <stdint.h>
#include "../core/vmewrap.h"

typedef void* sis3800_handle_t;

/**
 open the sis3800 device and return an handle to it
 @param vme_driver id of the driver vme to be used
 @param address the vme address of the board
 @return an handle or zero if error
 */
sis3800_handle_t sis3800_open(vmewrap_vme_handle_t vme_driver,uint32_t address);


/**
 close the sis3800 device
 @param handle to the board
 @return zero if success
 */
int32_t sis3800_close(sis3800_handle_t handle);


/**
 init sis3800 device and return
 @param h handle to the board
 @return zero if success,
 */
int32_t sis3800_init(sis3800_handle_t h);

/**
 read a the specified counter and clear it
 @param h handle to the board
 @param[in] countern, counter to be read
 @return the counter,
 */

uint32_t sis3800_readnclear(sis3800_handle_t h,int countern);

/**
 read a specific counter
 @param h handle to the board
 @param[in] countern, counter to be read

 @return the counter,
 */
uint32_t sis3800_readCounter(sis3800_handle_t h,int countern);

/**
 read a specific counter
 @param h handle to the board
 @param[in] countern, counter to be read

 @return the counter,
 */
uint32_t sis3800_readCounter(sis3800_handle_t h,uint32_t* count,int countern);

/**
 disable counters
 @param h handle to the board
 @param[in] counter bit mask of the counters to be disabled
 */
void sis3800_disableCounter(sis3800_handle_t h,int32_t counter);

/**
 read overflow of counters
 @param h handle to the board
 @return a bit mask that represents the counters in overflow
 */
uint32_t sis3800_getOverflows(sis3800_handle_t h);

/**
 enable/disable all counters
 @param h handle to the board
 @param enable >0 enable all counters, otherwise disable
 */
void sis3800_enable(sis3800_handle_t h,int enable);

/**
 clear all counters
 @param h handle to the board
 */
void sis3800_clear(sis3800_handle_t h);
/**
 reset
 @param h handle to the board
 */
void sis3800_reset(sis3800_handle_t h);
#define SIS3800_COUNTER0_OFF 0x280
#define SIS3800_COUNTER0_RDCLR_OFF 0x300

#endif

