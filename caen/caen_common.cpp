/*
  CAEN_QDC common driver
  @release: 0.1
  @author: Andrea Michelotti
  @date: 24/2/2015
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "caen_common.h"
#include <sys/time.h>

#include <common/debug/core/debug.h>
static int32_t caen_common_close(void* h){
    _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
    DPRINT(DEV_BOARD " closing handle @%p",h);
    if(handle){
        int ret;
        DPRINT(DEV_BOARD " caen_qdc sucessfully closed");
        ret=vmewrap_vme_close(handle->vme);
        free(handle);
        return ret;
    }
    
    return -4;
}

static void* caen_common_open(vmewrap_vme_handle_t vme,uint32_t address ){
  void* mapped_address;
  int size = 0x10000;
  int boardid,manufactureid;
  vmewrap_window_t window;
  if((window=vmewrap_vme_open_master(vme,address,size,VME_ADDRESSING_A32,VME_ACCESS_D32,( vme_opt)VME_OPT_BLT_ON))==0){
  		ERR("cannot map vme");
  		return NULL;
  	}

  mapped_address =  vmewrap_get_linux_add(window);
 
  _caen_common_handle_t *p =( _caen_common_handle_t *) calloc(1,sizeof(_caen_common_handle_t));
  if(p==NULL){
    ERR("cannot allocate resources");
    vmewrap_vme_close(window);
    return 0;
  }
  p->vme = window;
  p->mapped_address = mapped_address;
  p->cycle = 0;
  boardid=BOARD_ID_REG(window)&0xFF;
  manufactureid=OUI_REG(window)&0xFF;
    p->boardid=boardid;
    p->manufactureid=manufactureid;
    p->version=FW_REVISION_REG(window);
  if(mapped_address){
    DPRINT(DEV_BOARD " caen_qdc successfully mapped at @%p",mapped_address);
  }
  PRINT(DEV_BOARD " FW:0x%x",p->version);
  PRINT(DEV_BOARD " BoardID:0x%x",boardid);
  PRINT(DEV_BOARD " ManufactureID:0x%x",manufactureid);
  if(boardid!=BOARD_ID || manufactureid!=MANUFACTURE_ID){
    ERR("device not identified expected BoardId=0x%x ManufactureId=0x%x",BOARD_ID,MANUFACTURE_ID);
    caen_common_close((void*)p);
    return 0;
  }
  
  return (void*) p; 
}



static int32_t caen_common_init(void* h,int32_t crate_num,int hwreset){
  _caen_common_handle_t* handle =( _caen_common_handle_t*) h;
  DPRINT(DEV_BOARD " intialiazing @%p",h);
  int cnt;
  if(hwreset){
    VME_WRITE16(handle->vme,SSRESET_OFF,1);
    VME_WRITE16(handle->vme,BITSET_OFF,SOFTRESET_BIT);
    sleep(1);
    VME_WRITE16(handle->vme,BITCLR_OFF,SOFTRESET_BIT);    
  }
  for(cnt=0;cnt<NCHANNELS;cnt++){
    //    DPRINT(DEV_BOARD " clearing threashold %d",cnt);
    //    THRS_CHANNEL_REG(handle->mapped_address,cnt,0)= 0;
    VME_WRITE16(handle->vme,THRS_CHANNEL_OFF+(4*cnt),0);
    //    msync(handle->mapped_address+off,4);
    //    THRS_CHANNEL_REG(handle->mapped_address,cnt,1)= 0;
    VME_WRITE16(handle->vme,THRS_CHANNEL_OFF+(4*cnt)+2,0);
    //    msync(handle->mapped_address+off+4,4);
  }
  
  VME_WRITE16(handle->vme,CRATE_SEL_OFF,crate_num);    
  VME_WRITE16(handle->vme,BITSET2_OFF,CLEARDATA_BIT|ALLTRG_EN_BIT|EMPTY_EN_BIT|AUTOINCR_BIT|OVERRANGE_EN_BIT|LOWTHR_EN_BIT);// clear data reset
  VME_WRITE16(handle->vme,BITCLR2_OFF,TEST_MEM_BIT|OFFLINE_BIT|TESTAQ_BIT|CLEARDATA_BIT);

  return 0;
}

static int32_t caen_common_setIped(void* h,int32_t ipedval){
  _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
  DPRINT(DEV_BOARD " setting ipedval=x%x",ipedval);
  VME_WRITE16(handle->vme,IPED_OFF,ipedval);    
  return 0;
}


static int32_t caen_common_setThreashold(void* h,int16_t lowres,int16_t hires,int channel){
  _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
  if((channel<NCHANNELS) && (channel>=0)){
    DPRINT(DEV_BOARD " setting threshold channel %d hires=x%x lores=x%x ",channel,lowres,hires);
    VME_WRITE16(handle->vme,BITSET2_OFF,CLEARDATA_BIT|OVERRANGE_EN_BIT|LOWTHR_EN_BIT);// clear data reset

#ifdef CAEN965
      VME_WRITE16(handle->vme,THRS_CHANNEL_OFF+4*channel,hires&0xff);// clear data reset
      VME_WRITE16(handle->vme,THRS_CHANNEL_OFF+2+4*channel,lowres&0xff);// clear data reset

#else
      VME_WRITE16(handle->vme,THRS_CHANNEL_OFF+4*channel,lowres&0xff);// clear data reset

#endif
    return 0;
  }
  return -1;
}


static int32_t caen_common_getThreashold(void* h,int16_t* lowres,int16_t* hires,int channel){
    _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
    if((channel<NCHANNELS) && (channel>=0)){
#ifdef CAEN792
        *lowres = THRS_CHANNEL_REG(handle->vme,channel,0)&0xff;
#else
      *lowres = THRS_CHANNEL_REG(handle->vme,channel,1)&0xff;
      *hires = THRS_CHANNEL_REG(handle->vme,channel,0)&0xff;
#endif
      return 0;
    }
    return -1;
}



static uint16_t caen_common_getStatus(void* h){
  uint16_t ret;
  _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
  ret = STATUS_REG(handle->vme);
  DPRINT(DEV_BOARD " Get Status 0x%x",ret);
  return ret;
}

static uint16_t caen_common_getBufferStatus(void* h){
  uint16_t ret;
  _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
  ret = STATUS2_REG(handle->vme);
  return ret;
}

static uint32_t caen_common_getEventCounter(void* h,int reset){
  uint32_t ret;
  _caen_common_handle_t* handle = (_caen_common_handle_t*)h;
  //  ret = EVT_CNT_REG(handle->mapped_address);
  ret = EVT_CNT_LOW_REG(handle->vme)|EVT_CNT_HI_REG(handle->vme)<<16;
  if(reset){
    VME_WRITE16(handle->vme,EVT_CNT_LOW_OFF,0);
    VME_WRITE16(handle->vme,EVT_CNT_HI_OFF,0);

    handle->event_counter=0;
  }
  return ret;
}
static uint32_t search_header(_caen_common_handle_t* handle){
  evt_buffer_t a;
  uint32_t ret=0;
  a.data  = REG32(handle->vme,0);
  if(a.h.signature==2){
    ret = a.h.cnt;
    DPRINT(DEV_BOARD " New Header %u channels acquired",ret);
  }
  return ret;
}
static uint32_t search_eoe(_caen_common_handle_t* handle){
  evt_buffer_t a;
  uint32_t ret=0;
  a.data  = REG32(handle->vme,0);
  if(a.e.signature==4){
    DPRINT(DEV_BOARD " EOE found event counter = %u",a.e.ev_counter);
    ret= a.e.ev_counter;
  }
  return ret;
}
//return the channels acquired 
static int acquire_event_channels(void* h,uint32_t *lowres,uint32_t*hires,int start_chan,int max_chan,uint32_t*counter){
  _caen_common_handle_t* handle =(_caen_common_handle_t*) h;
  int cnt=0;
  evt_buffer_t a;
  int nchannels_acquired=0;

  int nchan;
  nchan = search_header(handle);
  while(cnt<nchan){
    //    a.data=  REG32(handle->mapped_address,0);
    VME_READ32(handle->vme,0,&a.data);
    if(a.d.signature==0){
      // data signature
      if((a.d.channel<max_chan)&&(a.d.channel>=start_chan)){
	uint32_t data;
	if(	a.d.un){
		data = 0;
	} else if(a.d.ov){
		data = 0xFFF;
	} else {
		data = a.d.adc;
	}
#ifdef CAEN965
	if(a.d.rg==1){
	  
	  lowres[a.d.channel] = data;
	} else {
	  hires[a.d.channel] = data;
	}


#else
	lowres[a.d.channel] = data;
#endif
	nchannels_acquired++;
        
	DPRINT(DEV_BOARD " [%.2d] acquiring channel %.2d=0x%.3x, acquired %.2d :Underflow:%d Overflow:%d", cnt,a.d.channel,data,nchannels_acquired,a.d.un,a.d.ov);
	
      }
    } else if(a.d.signature==4){
    
      DPRINT(DEV_BOARD " [%d] EOE found event counter = %d",cnt,a.e.ev_counter);
      *counter=a.e.ev_counter;
      break;
    } else if(a.d.signature==6){
      DPRINT(DEV_BOARD " [%d] Not valid Data found event counter",cnt);
    }
    cnt++;
  }
  search_eoe(handle);
  return nchannels_acquired;
}

static int32_t caen_common_acquire_channels_poll(void* h,uint32_t *lowres,uint32_t*hires,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms){
  int ret = 0;
  _caen_common_handle_t* handle =(_caen_common_handle_t*) h;
  uint16_t status=0;
  uint32_t counter;
  uint32_t events;
  int cnt;
  struct timeval tv,start;
  suseconds_t diff=0;
  tv.tv_usec = 0;

  if(timeo_ms>0){
    gettimeofday(&start,NULL);
  }
  // AMI 7/11/2013
  for(cnt=0;cnt<nchannels;cnt++){
   lowres[cnt]=0;
#ifdef CAEN965
   hires[cnt]=0;
#endif
  }
  //
  do{
    // get the status from the caen_qdc
    VME_READ16(handle->vme,STATUS_OFF,&status);
    //    status = STATUS_REG(handle->mapped_address);
    //usleep(100);
    if(timeo_ms>0){
      gettimeofday(&tv,NULL);
      if(tv.tv_usec<start.tv_usec){
	diff = (tv.tv_sec-1 - start.tv_sec)*1000000 +1000000+tv.tv_usec - start.tv_usec;
      } else {
	diff = (tv.tv_sec- start.tv_sec)*1000000 + tv.tv_usec - start.tv_usec;
      }

    }
  } while(((status&CAEN_QDC_STATUS_DREADY)==0)&&((diff)<=(timeo_ms*1000)));
if((timeo_ms>0) && (diff>(timeo_ms*1000))){
    return -100;
  }
//  counter = EVT_CNT_LOW_REG(handle->vme)|(EVT_CNT_HI_REG(handle->vme)<<16);
  DPRINT(DEV_BOARD " counter events %u, events %d, totcycle %lu",counter,events,handle->cycle);
  if(status&CAEN_QDC_STATUS_DREADY){
    ret = acquire_event_channels(handle,lowres,hires,start_channel,nchannels,&counter);
    events = (counter > handle->event_counter)?(counter - handle->event_counter):0;
  
    handle->cycle+= events;
    handle->event_counter =counter;
  
  }
  *cycle = handle->cycle;
  //BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT;//clear data buffer AMI 28/10/13
  //BITCLR2_REG(handle->mapped_address)=CLEARDATA_BIT;//clear data buffer AMI 28/10/13
  //  msync(handle->mapped_address+0x1000,0x100);
  VME_WRITE16(handle->vme,BITSET2_OFF,CLEARDATA_BIT);
  VME_WRITE16(handle->vme,BITCLR2_OFF,CLEARDATA_BIT);

 return ret;
}
#ifdef LABVIEW
static void checkI32Array(LV_vector_uint32_t**  array, long *numOfComp, errorStruct *error)
{
MgErr   err;

if (!error->status && ((*array)->dim != *numOfComp)) {
        if ((err = NumericArrayResize(uL,1L,(UHandle *)&array,*numOfComp))) {
                LStrPrintf (error->source, "Call Library Function: rawMemAccess, function: checkI32Array");
                error->code = 5000;
                error->status = LVTRUE;
        } else {
                (*array)->dim = *numOfComp;
        }
}
return;
}


static void* caen_common_LV_open(uint32_t mapped_address,errorStruct *error ){
  int boardid,manufactureid;
  _caen_common_handle_t *p;
  if(error->status)
  	return 0;
  	
  p = malloc(sizeof(_caen_common_handle_t));
  if(p==NULL){
  	error->status = 2;
  	return 0;
  }
  p->bus = 0;
  p->handle = 0;
  p->mapped_address = mapped_address;
  p->cycle = 0;
  DPRINT(DEV_BOARD " setting caen_qdc Linux address to @0x%x",mapped_address);

  boardid=BOARD_ID_REG(mapped_address)&0xFF;
  manufactureid=OUI_REG(mapped_address)&0xFF;
  DPRINT(DEV_BOARD " caen_qdc successfully mapped at @0x%x",mapped_address);
  PRINT(DEV_BOARD " FW:0x%x",FW_REVISION_REG(mapped_address));
  PRINT(DEV_BOARD " BoardID:0x%x",boardid);
  PRINT(DEV_BOARD " ManufactureID:0x%x",manufactureid);
  if(boardid!=BOARD_ID || manufactureid!=MANUFACTURE_ID){
    ERR("device not identified expected BoardId=0x%x ManufactureId=0x%x",BOARD_ID,MANUFACTURE_ID);
    error->status = 2;
    return 0;
  }
  
  return (void*) p; 
}


static int32_t caen_common_LV_close(void* h,errorStruct *error){
 DPRINT(DEV_BOARD " LV close 0x@%x",(uint32_t)h);
  if(h){
    free(h);
  } 
  return 0;
}

static int32_t caen_common_LV_acquire_channels_poll(void* h,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct*error){
  LV_vector_uint32_t** lv_lowres=(LV_vector_uint32_t**)lowres;
  LV_vector_uint32_t** lv_hires=(LV_vector_uint32_t**)hires;
  uint64_t counter;
  int32_t ret=0;
  _caen_common_handle_t* handle = h;
  long numOfComp=NCHANNELS;
  if(error->status){
  	return 0;
  }
    #ifndef CAEN792
 // DPRINT(DEV_BOARD " lowres (%d,0x%x), hires (%d,0x%x)",(*lv_lowres)->dim,(*lv_lowres)->arg,(*lv_hires)->dim,(*lv_hires)->arg);
  if(((*lv_lowres)->dim<NCHANNELS)||((*lv_lowres)->arg==NULL)){
  #if 0
    void * newp = realloc((*lv_lowres)->arg,NCHANNELS*sizeof(uint32_t));
    if(newp){
      uint32_t* tmp=(uint32_t*)&(*lv_lowres)->arg;
      DPRINT(DEV_BOARD " reallocating LV low buffer from %d elements to %d @0x%x",(*lv_lowres)->dim,NCHANNELS,(uint32_t)newp);
      *tmp = (uint32_t)newp;
      //(*lv_lowres)->arg = (uint32_t* )newp;
      (*lv_lowres)->dim = NCHANNELS;
    }
  #else
  numOfComp=NCHANNELS;
  DPRINT(DEV_BOARD " reallocating LV low buffer (@0x%x) from %d elements to %d",(uint32_t)(*lv_lowres)->arg,(*lv_lowres)->dim,NCHANNELS);
  checkI32Array(lv_lowres,&numOfComp,error);
  if(error->status){
  	ERR("Resizing array @0x%x",(uint32_t)lv_lowres);
  	return 0;
  }
  (*lv_lowres)->dim = NCHANNELS;
  #endif
  
  }

    
  if(((*lv_hires)->dim<NCHANNELS)||((*lv_hires)->arg==NULL)){
  #if 0 
    void * newp = realloc((*lv_hires)->arg,NCHANNELS*sizeof(uint32_t));
    if(newp){
      uint32_t* tmp=(uint32_t*)&(*lv_hires)->arg;
      DPRINT(DEV_BOARD " reallocating LV hi buffer from %d elements to %d",(*lv_hires)->dim,NCHANNELS,(uint32_t)newp);
    //  (*lv_hires)->arg = (uint32_t*)newp;
      *tmp= (uint32_t)newp;
      (*lv_hires)->dim = NCHANNELS;
    }
    #else
    numOfComp=NCHANNELS;
    DPRINT(DEV_BOARD " reallocating LV hi buffer (@0x%x) from %d elements to %d",(uint32_t)(*lv_hires)->arg,(*lv_hires)->dim,NCHANNELS);
  	checkI32Array(lv_hires,&numOfComp,error);
  	if(error->status){
  		ERR("Resizing array @0x%x",(uint32_t)lv_hires);
  		return 0;
  	}
  	(*lv_hires)->dim = NCHANNELS;
  #endif
  }
   
  *event_under_run =-1;
  if((*lv_lowres)->arg && (*lv_hires)->arg){
    uint64_t old_counter = handle->cycle;

    ret =caen_common_acquire_channels_poll(h,(uint32_t *)(*lv_lowres)->arg,(uint32_t*)(*lv_hires)->arg,0,NCHANNELS,&counter,timeo_ms); 
    *event_under_run = (handle->cycle - old_counter) -1;
    DPRINT(DEV_BOARD " [x%x]acquired %d channels, event underrun %d",handle->mapped_address,ret,*event_under_run);

  } else {
    ERR("INVALID INPUT POINTERS lowres:@x%x,hires:@x%x",(unsigned)(*lv_lowres)->arg,(unsigned)(*lv_hires)->arg);
    return -1;
  }
#else
    //TODO
#endif

  return ret;
}

#endif
