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
#include "caen_common.cpp"



caen775_handle_t caen775_open(vmewrap_vme_handle_t vme_driver,uint32_t address ){
	   return caen_common_open(vme_driver,address);
	}


int32_t caen775_init(caen775_handle_t h,int32_t crate_num,int swreset){
	_caen_common_handle_t* handle =(_caen_common_handle_t*) h;

	DPRINT(DEV_BOARD " intialiazing @%p",h);
	if(swreset){
	  VME_WRITE_REG16(handle->vme,BITSET_OFF,SOFTRESET_BIT);
	}
	VME_WRITE_REG16(handle->vme,CRATE_SEL_OFF,crate_num);
	VME_WRITE_REG16(handle->vme,BITCLR_OFF,0xff);// clear soft reset, berr and sel addr
	//  BITSET2_REG(handle->mapped_address)=ALLTRG_EN_BIT/*|OVERRANGE_EN_BIT|LOWTHR_EN_BIT*/|EMPTY_EN_BIT|AUTOINCR_BIT;
	VME_WRITE_REG16(handle->vme,BITSET2_OFF,CAEN775_ALL_TRIGGER|CAEN775_AUTO_INCR|CAEN775_ACCEPT_OVER_RANGE|CAEN775_CLEAR_DATA);
	VME_WRITE_REG16(handle->vme,BITCLR2_OFF,CAEN775_MEMORY_TEST|CAEN775_ADC_OFFLINE|CAEN775_TEST_ACQ|CAEN775_CLEAR_DATA);

	return 0;
}

int32_t caen775_close(caen775_handle_t h){
    return caen_common_close(h);
}


int32_t caen775_setThreashold(caen775_handle_t h,int16_t value,int kill,int channel){
	_caen_common_handle_t* handle = (_caen_common_handle_t*)h;

	if((channel<NCHANNELS) && (channel>=0)){
	    DPRINT(DEV_BOARD " setting threshold channel %d to x%x ",channel,value);
		VME_WRITE_REG16(handle->vme,BITSET2_OFF,CAEN775_CLEAR_DATA|CAEN775_THRESHOLD);
		VME_WRITE_REG16(handle->vme,THRS_CHANNEL_OFF+4*channel,(value&0xff)| ((kill)?0x100:0));
	}
	return 0;
}




int32_t caen775_enable_mode(caen775_handle_t h,caen775_modes_t modes){
  	_caen_common_handle_t* handle = (_caen_common_handle_t*)h;
	VME_WRITE_REG16(handle->vme,BITSET2_OFF,modes);

	return 0;
}

int32_t caen775_disable_mode(caen775_handle_t h,caen775_modes_t modes){
  _caen_common_handle_t* handle =(_caen_common_handle_t*) h;
  	VME_WRITE_REG16(handle->vme,BITCLR2_OFF,modes);

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
