/*
  CAEN965 driver
  @release: 0.1
  @author: Andrea Michelotti
*/

#include "caen965_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmewrap.h"
#include <sys/time.h>

#define CAEN965 1
#include "caen_qdc.c"



caen965_handle_t caen965_open(uint32_t address ){
   return caen_qdc_open(address);
}


int32_t caen965_init(caen965_handle_t h,int32_t crate_num,int hwreset){
    return caen_qdc_init(h,crate_num,hwreset);
}

int32_t caen965_setIped(caen965_handle_t h,int32_t ipedval){
   return caen_qdc_setIped(h,ipedval);
}


int32_t caen965_setThreashold(caen965_handle_t h,int16_t lowres,int16_t hires,int channel){
    return  caen_qdc_setThreashold(h,lowres,hires,channel);
}


int32_t caen965_getThreashold(caen965_handle_t h,int16_t* lowres,int16_t* hires,int channel){
    return caen_qdc_getThreashold(h,lowres,hires,channel);
}



uint16_t caen965_getStatus(caen965_handle_t h){
    return caen_qdc_getStatus(h);
}

uint16_t caen965_getBufferStatus(caen965_handle_t h){
  return caen_qdc_getBufferStatus(h);
}

uint32_t caen965_getEventCounter(caen965_handle_t h,int reset){
   return caen_qdc_getEventCounter(h,reset);
}

int32_t caen965_acquire_channels_poll(caen965_handle_t h,uint32_t *lowres,uint32_t*hires,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms){
   return caen_qdc_acquire_channels_poll( h, lowres,hires,start_channel,nchannels,cycle,timeo_ms);
}
#ifdef LABVIEW

caen965_handle_t caen965_LV_open(uint32_t mapped_address,errorStruct *error ){
  return (caen965_handle_t) caen_qdc_LV_open( mapped_address,error );
}


int32_t caen965_LV_close(caen965_handle_t h,errorStruct *error){
   return caen_qdc_LV_close(h,error);
}

int32_t caen965_LV_acquire_channels_poll(caen965_handle_t h,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct*error){
    return caen_qdc_LV_acquire_channels_poll(h,lowres,hires,event_under_run,timeo_ms,error);
}

#endif
