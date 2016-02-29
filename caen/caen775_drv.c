/*
  CAEN965 driver
  @release: 0.1
  @author: Andrea Michelotti
*/

#include "caen775_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmewrap.h"
#include <sys/time.h>

#define CAEN775 1
#include "caen_common.c"



caen775_handle_t caen775_open(uint32_t address ){
   return caen_common_open(address);
}


int32_t caen775_init(caen775_handle_t h,int32_t crate_num,int swreset){
	_caen_common_handle_t* handle = h;

	DPRINT(DEV_BOARD " intialiazing @0x%x",(uint32_t)h);
	if(swreset){
	  BITSET_REG(handle->mapped_address) = SOFTRESET_BIT;
	}
	CRATE_SEL_REG(handle->mapped_address)=crate_num;


	BITCLR_REG(handle->mapped_address) = 0xff; // clear soft reset, berr and sel addr
	//  BITSET2_REG(handle->mapped_address)=ALLTRG_EN_BIT/*|OVERRANGE_EN_BIT|LOWTHR_EN_BIT*/|EMPTY_EN_BIT|AUTOINCR_BIT;
	BITSET2_REG(handle->mapped_address)=CAEN775_ALL_TRIGGER|CAEN775_AUTO_INCR|CAEN775_ACCEPT_OVER_RANGE|CAEN775_CLEAR_DATA; // clear data reset;
	BITCLR2_REG(handle->mapped_address)=CAEN775_MEMORY_TEST|CAEN775_ADC_OFFLINE|CAEN775_TEST_ACQ|CAEN775_CLEAR_DATA; // clear data reset;;
	return 0;
}

int32_t caen775_close(caen775_handle_t h){
    return caen_common_close(h);
}


int32_t caen775_setThreashold(caen775_handle_t h,int16_t value,int kill,int channel){
	_caen_common_handle_t* handle = h;

	if((channel<NCHANNELS) && (channel>=0)){
	    DPRINT(DEV_BOARD " setting threshold channel %d to x%x ",channel,value);
	    BITSET2_REG(handle->mapped_address)=CAEN775_CLEAR_DATA|CAEN775_THRESHOLD;

	      THRS_CHANNEL_REG(handle->mapped_address,channel,0)= (value&0xff)| ((kill)?0x100:0);
	}
	return 0;
}




int32_t caen775_enable_mode(caen775_handle_t h,caen775_modes_t modes){
  	_caen_common_handle_t* handle = h;

	BITSET2_REG(handle->mapped_address)=modes;
	return 0;
}

int32_t caen775_disable_mode(caen775_handle_t h,caen775_modes_t modes){
  _caen_common_handle_t* handle = h;
  	BITCLR2_REG(handle->mapped_address)=modes;
	return 0;
}


uint16_t caen775_getStatus(caen775_handle_t h){
    return caen_common_getStatus(h);
}

uint16_t caen775_getBufferStatus(caen775_handle_t h){
  return caen_common_getBufferStatus(h);
}

uint32_t caen775_getEventCounter(caen775_handle_t h,int reset){
   return caen_common_getEventCounter(h,reset);
}

int32_t caen775_acquire_channels_poll(caen775_handle_t h,uint32_t *lowres,uint64_t *cycle,int timeo_ms){
   return caen_common_acquire_channels_poll( h, lowres,0,0,NCHANNELS,cycle,timeo_ms);
}
#ifdef LABVIEW

caen775_handle_t caen775_LV_open(uint32_t mapped_address,errorStruct *error ){
  return (caen775_handle_t) caen_common_LV_open( mapped_address,error );
}


int32_t caen775_LV_close(caen775_handle_t h,errorStruct *error){
   return caen_common_LV_close(h,error);
}

int32_t caen775_LV_acquire_channels_poll(caen775_handle_t h,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct*error){
    return caen_common_LV_acquire_channels_poll(h,lowres,hires,event_under_run,timeo_ms,error);
}

#endif
