/*
 CAEN792 driver
 @release: 0.1
 @author: Andrea Michelotti
 */

#include "caen792_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmewrap.h"
#include <sys/time.h>

#define CAEN792 1
#include "caen_common.c"



caen792_handle_t caen792_open(uint32_t address ){
    return caen_common_open(address);
}
int32_t caen792_close(caen792_handle_t h){
    return caen_common_close(h);
}


int32_t caen792_init(caen792_handle_t h,int32_t crate_num,int hwreset){
    return caen_common_init(h,crate_num,hwreset);
}

int32_t caen792_setIped(caen792_handle_t h,int32_t ipedval){
    return caen_common_setIped(h,ipedval);
}


int32_t caen792_setThreashold(caen792_handle_t h,int16_t res,int channel){
    return  caen_common_setThreashold(h,res,0,channel);
}


int32_t caen792_getThreashold(caen792_handle_t h,int16_t* res,int channel){
    return caen_common_getThreashold(h,res,0,channel);
}



uint16_t caen792_getStatus(caen792_handle_t h){
    return caen_common_getStatus(h);
}

uint16_t caen792_getBufferStatus(caen792_handle_t h){
    return caen_common_getBufferStatus(h);
}

uint32_t caen792_getEventCounter(caen792_handle_t h,int reset){
    return caen_common_getEventCounter(h,reset);
}

int32_t caen792_acquire_channels_poll(caen792_handle_t h,uint32_t *res,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms){
    return caen_common_acquire_channels_poll( h, res,0,start_channel,nchannels,cycle,timeo_ms);
}
#ifdef LABVIEW

caen792_handle_t caen792_LV_open(uint32_t mapped_address,errorStruct *error ){
    return (caen792_handle_t) caen_common_LV_open( mapped_address,error );
}


int32_t caen792_LV_close(caen792_handle_t h,errorStruct *error){
    return caen_common_LV_close(h,error);
}

int32_t caen792_LV_acquire_channels_poll(caen792_handle_t h,void *res,int32_t* event_under_run,int timeo_ms,errorStruct*error){
    return caen_common_LV_acquire_channels_poll(h,res,event_under_run,timeo_ms,error);
}

#endif
