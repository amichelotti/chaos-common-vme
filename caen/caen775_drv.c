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


int32_t caen775_init(caen775_handle_t h,int32_t crate_num,int hwreset){
    return caen_common_init(h,crate_num,hwreset);
}

int32_t caen775_close(caen775_handle_t h){
    return caen_common_close(h);
}

int32_t caen775_setIped(caen775_handle_t h,int32_t ipedval){
   return caen_common_setIped(h,ipedval);
}


int32_t caen775_setThreashold(caen775_handle_t h,int16_t lowres,int16_t hires,int channel){
    return  caen_common_setThreashold(h,lowres,hires,channel);
}


int32_t caen775_getThreashold(caen775_handle_t h,int16_t* lowres,int16_t* hires,int channel){
    return caen_common_getThreashold(h,lowres,hires,channel);
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

int32_t caen775_acquire_channels_poll(caen775_handle_t h,uint32_t *lowres,uint32_t*hires,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms){
   return caen_common_acquire_channels_poll( h, lowres,hires,start_channel,nchannels,cycle,timeo_ms);
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
