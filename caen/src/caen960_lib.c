/*
  CAEN965 driver
  @release: 0.1
  @author: Andrea Michelotti
*/
// clear data buffer each acquisition AMI 28/10/13
#include "caen960_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DEBUG
#define DPRINT(str,ARGS...) printf("* %s:" str,__FUNCTION__,##ARGS)
#else
#define DPRINT(str,ARGS...) 
#endif
#define PRINT(str,ARGS...) printf("*" str,##ARGS)

#define ERR(str,ARGS...) printf("# %s:" str,__FUNCTION__,##ARGS)

#include <vme/vme.h>
#include <vme/vme_api.h>
#include <sys/time.h>

// Register map
#define REG32(base,x) *((volatile uint32_t*)(base+x))
#define REG16(base,x) *((volatile uint16_t*)(base+x))
#define FW_REVISION_REG(base) REG16(base,0x1000)
#define BOARD_ID_REG(base) REG16(base,0x803A)
#define BOARD_ID_LSB_REG(base) REG16(base,0x803E)
#define BOARD_ID_MSB_REG(base) REG16(base,0x8036)
#define OUI_REG(base) REG16(base,0x802A)

#define THRS_CHANNEL_REG(base,chan,lo) REG16(base,0x1080 + 4*chan + 2*lo)
#define GEO_ADDRESS_REG(base) REG16(base,0x1002)
#define BITSET_REG(base) REG16(base,0x1006)
#define BITCLR_REG(base) REG16(base,0x1008)
#define STATUS_REG(base) REG16(base,0x100E)
#define CTRL_REG(base) REG16(base,0x1010)
#define SSRESET_REG(base) REG16(base,0x1016)
#define EVT_TRG_REG(base) REG16(base,0x1020)
#define STATUS2_REG(base) REG16(base,0x1022)
#define EVT_CNT_LOW_REG(base) REG16(base,0x1024)
#define EVT_CNT_HI_REG(base) REG16(base,0x1026)
#define EVT_CNTRESET_REG(base) REG16(base,0x1040)
#define IPED_REG(base) REG16(base,0x1060)
#define AAD_REG(base) REG16(base,0x1070)
#define BAD_REG(base) REG16(base,0x1072)
#define CRATE_SEL_REG(base) REG16(base,0x103C)
#define SLIDE_CONST_REG(base) REG16(base,0x106A)

#define BITSET2_REG(base) REG16(base,0x1032)
#define BITCLR2_REG(base) REG16(base,0x1034)

/* BITSET1 */
#define SOFTRESET_BIT (1<<7)

/*** BITSET/CLR 2  */
#define TEST_MEM_BIT 0x1 /* test memory, enable direct writes */
#define OFFLINE_BIT 0x2 /* adc offine */
#define CLEARDATA_BIT 0x4 /* clear data (data reset signal), write&read pointers */
#define OVERRANGE_EN_BIT 0x8 /*no overflow suppression*/
#define LOWTHR_EN_BIT 0x10 /* no zero suppression */
#define TESTAQ_BIT 0x40 /* acquisition test mode, the data are taken from an internal fifo */
#define SLIDE_EN_BIT 0x80 /* slide scale enable*/
#define STEP_TH_BIT 0x100 /* set the zero suppression resolution*/
#define AUTOINCR_BIT 0x800 /* enable read pointer autoincrement */
#define EMPTY_EN_BIT 0x1000 /* write the header and EOB when there are not accepted data*/
#define SLIDESUB_EN_BIT 0x2000 /* allow to change oepration mode */
#define ALLTRG_EN_BIT 0x4000 /* enable the event counter to increment also to not accepted trigger*/ 
/****** */

typedef struct _header {
  uint32_t unused:8;
  uint32_t cnt:6;
  uint32_t pad:2;
  uint32_t crate:8;
  uint32_t signature:3;
  uint32_t geo:5;
} header_t;

typedef struct _data {
  uint32_t adc:12;
  uint32_t ov:1;
  uint32_t un:1;
  uint32_t pad:2;
  uint32_t rg:1;
  uint32_t channel:4;
  uint32_t pad2:3;
  uint32_t signature:3;
  uint32_t geo:5;
} data_t;

typedef struct _eob {
  uint32_t ev_counter:24;
  uint32_t signature:3;
  uint32_t geo:5;
} eob_t;

typedef struct __evt_buffer {
    
  union {
    header_t h;
    data_t d;
    eob_t e;
    uint32_t data;
  };
} evt_buffer_t;
/** 
    private date,
    handle
*/
typedef struct __vme_handle__ {
  vme_bus_handle_t bus;
  vme_master_handle_t handle;
  uint32_t mapped_address;
  uint32_t event_counter;
  uint64_t cycle;
} vme_handle_t;




caen960_handle_t caen960_open(uint32_t address ){
  int am,flags;
  vme_bus_handle_t bus_handle;
  vme_master_handle_t handle;
  uint32_t mapped_address;
  int size = 0x10000;
  int boardid,manufactureid;
  DPRINT("opening vme device at @0x%x\n",address);
  if (vme_init(&bus_handle)) {
    ERR("cannot initialize vme\n");
    perror("vme_init");

    return 0;
  }
 
  am = VME_A32SD;
    
  flags = VME_CTL_PWEN;
  if (vme_master_window_create(bus_handle, &handle, address, am, size,
			       flags, NULL)) {
    ERR("cannot create master window\n");
    perror("vme_master_window_create");
    return 0;
  }

  mapped_address =  (uint32_t)vme_master_window_map(bus_handle, handle, 0);
  if (0 == mapped_address) {
    ERR("cannot map VME window\n");
    perror("vme_master_window_map");
    return 0;
  }

  vme_handle_t *p = malloc(sizeof(vme_handle_t));
  if(p==NULL){
    ERR("cannot allocate resources\n");
    vme_master_window_unmap(bus_handle, handle);
    vme_master_window_release(bus_handle, handle);
    vme_term(bus_handle);
    return 0;
  }
  p->bus = bus_handle;
  p->handle = handle;
  p->mapped_address = mapped_address;
  p->cycle = 0;
  boardid=BOARD_ID_REG(mapped_address)&0xFF;
  manufactureid=OUI_REG(mapped_address)&0xFF;
  DPRINT("caen960 successfully mapped at @0x%x\n",mapped_address);
  PRINT("CV 965 FW:0x%x\n",FW_REVISION_REG(mapped_address));
  PRINT("CV 965 BoardID:0x%x\n",boardid);
  PRINT("CV 965 Manufacture:0x%x\n",manufactureid);
  if(boardid!=BOARD_ID || manufactureid!=MANUFACTURE_ID){
    ERR("device not identified expected BoardId=0x%x ManufactureId=0x%x\n",BOARD_ID,MANUFACTURE_ID);
    caen960_close((caen960_handle_t)p);
    return 0;
  }
  
  return (caen960_handle_t) p; 
}

int32_t caen960_close(caen960_handle_t h){
  vme_handle_t* handle = h;
  DPRINT("closing handle @0x%x\n",(unsigned)h);
  if(handle){
    if (vme_master_window_unmap(handle->bus, handle->handle)) {
     
      ERR("Error unmapping the window\n");
      vme_master_window_release(handle->bus, handle->handle);
      vme_term(handle->bus);
      free(handle);
      return -1;
    }
        
    if (vme_master_window_release(handle->bus, handle->handle)) {
      ERR("Error releasing the window\n");
      vme_term(handle->bus);
      return -2;
    }
        
    if (vme_term(handle->bus)) {
      ERR("Error terminating VME\n");
      return -3;
    }
 
    DPRINT("caen960 sucessfully closed\n");
    free(handle);
    return 0;
  }
  return -4;
}


int32_t caen960_init(caen960_handle_t h,int32_t crate_num,int hwreset){
  vme_handle_t* handle = h;
  DPRINT("intialiazing @0x%x\n",(uint32_t)h); 
  int cnt;
  if(hwreset){
    SSRESET_REG(handle->mapped_address) = 1;
    BITSET_REG(handle->mapped_address) = SOFTRESET_BIT;
    sleep(1);
    BITCLR_REG(handle->mapped_address) = SOFTRESET_BIT;
    
  }
  for(cnt=0;cnt<NCHANNELS;cnt++){
    THRS_CHANNEL_REG(handle->mapped_address,cnt,1)= 0;
    THRS_CHANNEL_REG(handle->mapped_address,cnt,0)= 0;
  }
  CRATE_SEL_REG(handle->mapped_address)=crate_num;
  BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT; // clear data reset
  BITCLR2_REG(handle->mapped_address)=TEST_MEM_BIT|OFFLINE_BIT|TESTAQ_BIT|CLEARDATA_BIT;

  //  BITSET2_REG(handle->mapped_address)=ALLTRG_EN_BIT/*|OVERRANGE_EN_BIT|LOWTHR_EN_BIT*/|EMPTY_EN_BIT|AUTOINCR_BIT;
  BITSET2_REG(handle->mapped_address)=ALLTRG_EN_BIT|EMPTY_EN_BIT|AUTOINCR_BIT|OVERRANGE_EN_BIT|LOWTHR_EN_BIT;
  return 0;
}

int32_t caen960_setIped(caen960_handle_t h,int32_t ipedval){
  vme_handle_t* handle = h;
  DPRINT("setting ipedval=x%x\n",ipedval);
  IPED_REG(handle->mapped_address)=ipedval;
  return 0;
}


int32_t caen960_setThreashold(caen960_handle_t h,int16_t lowres,int16_t hires,int channel){
  vme_handle_t* handle = h;
  if((channel<NCHANNELS) && (channel>=0)){
    DPRINT("setting threshold channel %d hires=x%x lores=x%x \n",channel,lowres,hires);
    BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT|OVERRANGE_EN_BIT|LOWTHR_EN_BIT;
    THRS_CHANNEL_REG(handle->mapped_address,channel,1)= lowres&0xff;
    THRS_CHANNEL_REG(handle->mapped_address,channel,0)= hires&0xff;
    return 0;
  }
  return -1;
}


int32_t caen960_getThreashold(caen960_handle_t h,int16_t* lowres,int16_t* hires,int channel){
    vme_handle_t* handle = h;
    if((channel<NCHANNELS) && (channel>=0)){
      *lowres = THRS_CHANNEL_REG(handle->mapped_address,channel,1)&0xff;
      *hires = THRS_CHANNEL_REG(handle->mapped_address,channel,0)&0xff;
      return 0;
    }
    return -1;
}



uint16_t caen960_getStatus(caen960_handle_t h){
  uint16_t ret;
  vme_handle_t* handle = h;
  ret = STATUS_REG(handle->mapped_address);
  DPRINT("Get Status 0x%x\n",ret);
  return ret;
}

uint16_t caen960_getBufferStatus(caen960_handle_t h){
  uint16_t ret;
  vme_handle_t* handle = h;
  ret = STATUS2_REG(handle->mapped_address);
  return ret;
}

uint32_t caen960_getEventCounter(caen960_handle_t h,int reset){
  uint32_t ret;
  vme_handle_t* handle = h;
  //  ret = EVT_CNT_REG(handle->mapped_address);
  ret = EVT_CNT_LOW_REG(handle->mapped_address)|EVT_CNT_HI_REG(handle->mapped_address)<<16;
  if(reset){
    EVT_CNTRESET_REG(handle->mapped_address)=0;
    handle->event_counter=0;
  }
  return ret;
}
static uint32_t search_header(vme_handle_t* handle){
  evt_buffer_t a;
  uint32_t ret=0;
  a.data  = REG32(handle->mapped_address,0);
  if(a.h.signature==2){
    ret = a.h.cnt;
    DPRINT("New Header %u channels acquired\n",ret);
  }
  return ret;
}
static uint32_t search_eoe(vme_handle_t* handle){
  evt_buffer_t a;
  uint32_t ret=0;
  a.data  = REG32(handle->mapped_address,0);
  if(a.e.signature==4){
    DPRINT("EOE found event counter = %u\n",a.e.ev_counter);
    ret= a.e.ev_counter;
  }
  return ret;
}
//return the channels acquired 
static int acquire_event_channels(caen960_handle_t h,uint32_t *lowres,uint32_t*hires,int start_chan,int max_chan){
  vme_handle_t* handle = h;
  int cnt=0;
  evt_buffer_t a;
  int nchannels_acquired=0;

  int nchan;
  nchan = search_header(handle);
  while(cnt<nchan){
    a.data=  REG32(handle->mapped_address,0);
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
	if(a.d.rg==1){
	  
	  lowres[a.d.channel] = data;
	} else {
	  hires[a.d.channel] = data;
	}
	nchannels_acquired++;
	DPRINT("[%.2d] acquiring %3s channel %.2d=0x%.3x, acquired %.2d :Underflow:%d Overflow:%d\n", cnt,a.d.rg?"LOW":"HI",a.d.channel,data,nchannels_acquired,a.d.un,a.d.ov);
	
      }
    } else if(a.d.signature==4){
    
      DPRINT("[%d] EOE found event counter = %d\n",cnt,a.e.ev_counter);
      break;
    } else if(a.d.signature==6){
      DPRINT("[%d] Not valid Data found event counter\n",cnt);
    }
    cnt++;
  }
  search_eoe(handle);
  return nchannels_acquired;
}

int32_t caen960_acquire_channels_poll(caen960_handle_t h,uint32_t *lowres,uint32_t*hires,int start_channel,int nchannels,uint64_t *cycle,int timeo_ms){
  int ret = 0;
  vme_handle_t* handle = h;
  uint32_t status;
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
   hires[cnt]=0;
  }
  //
  do{
    // get the status from the caen960
    status = STATUS_REG(handle->mapped_address);
    //usleep(100);
    if(timeo_ms>0){
      gettimeofday(&tv,NULL);
      if(tv.tv_usec<start.tv_usec){
	diff = (tv.tv_sec-1 - start.tv_sec)*1000000 +1000000+tv.tv_usec - start.tv_usec;
      } else {
	diff = (tv.tv_sec- start.tv_sec)*1000000 + tv.tv_usec - start.tv_usec;
      }

    }
  } while(((status&CAEN960_STATUS_DREADY)==0)&&((diff)<=(timeo_ms*1000)));

  counter = EVT_CNT_LOW_REG(handle->mapped_address)|EVT_CNT_HI_REG(handle->mapped_address)<<16;
  events = abs(counter - handle->event_counter);

  handle->cycle+= events;
  handle->event_counter =counter;
  // DPRINT("counter events %u, events %d, totcycle %Ld\n",counter,events,handle->cycle);
  if(status&CAEN960_STATUS_DREADY){
    ret = acquire_event_channels(handle,lowres,hires,start_channel,nchannels);
  }
  *cycle = handle->cycle;
  BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT;//clear data buffer AMI 28/10/13
  BITCLR2_REG(handle->mapped_address)=CLEARDATA_BIT;//clear data buffer AMI 28/10/13
 return ret;
}
#ifdef LABVIEW
void checkI32Array(LV_vector_uint32_t**  array, long *numOfComp, errorStruct *error)
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


caen960_handle_t caen960_LV_open(uint32_t mapped_address,errorStruct *error ){
  int boardid,manufactureid;
  vme_handle_t *p;
  if(error->status)
  	return 0;
  	
  p = malloc(sizeof(vme_handle_t));
  if(p==NULL){
  	error->status = 2;
  	return 0;
  }
  p->bus = 0;
  p->handle = 0;
  p->mapped_address = mapped_address;
  p->cycle = 0;
  DPRINT("setting caen960 Linux address to @0x%x\n",mapped_address);

  boardid=BOARD_ID_REG(mapped_address)&0xFF;
  manufactureid=OUI_REG(mapped_address)&0xFF;
  DPRINT("caen960 successfully mapped at @0x%x\n",mapped_address);
  PRINT("CV 965 FW:0x%x\n",FW_REVISION_REG(mapped_address));
  PRINT("CV 965 BoardID:0x%x\n",boardid);
  PRINT("CV 965 Manufacture:0x%x\n",manufactureid);
  if(boardid!=BOARD_ID || manufactureid!=MANUFACTURE_ID){
    ERR("device not identified expected BoardId=0x%x ManufactureId=0x%x\n",BOARD_ID,MANUFACTURE_ID);
    error->status = 2;
    return 0;
  }
  
  return (caen960_handle_t) p; 
}


int32_t caen960_LV_close(caen960_handle_t h,errorStruct *error){
 DPRINT("LV close 0x@%x\n",(uint32_t)h);
  if(h){
    free(h);
  } 
  return 0;
}

int32_t caen960_LV_acquire_channels_poll(caen960_handle_t h,void *lowres,void*hires,int32_t* event_under_run,int timeo_ms,errorStruct*error){
  LV_vector_uint32_t** lv_lowres=(LV_vector_uint32_t**)lowres;
  LV_vector_uint32_t** lv_hires=(LV_vector_uint32_t**)hires;
  uint64_t counter;
  int32_t ret=0;
  vme_handle_t* handle = h;
  long numOfComp=NCHANNELS;
  if(error->status){
  	return 0;
  }
 // DPRINT("lowres (%d,0x%x), hires (%d,0x%x)\n",(*lv_lowres)->dim,(*lv_lowres)->arg,(*lv_hires)->dim,(*lv_hires)->arg);
  if(((*lv_lowres)->dim<NCHANNELS)||((*lv_lowres)->arg==NULL)){
  #if 0
    void * newp = realloc((*lv_lowres)->arg,NCHANNELS*sizeof(uint32_t));
    if(newp){
      uint32_t* tmp=(uint32_t*)&(*lv_lowres)->arg;
      DPRINT("reallocating LV low buffer from %d elements to %d @0x%x\n",(*lv_lowres)->dim,NCHANNELS,(uint32_t)newp);
      *tmp = (uint32_t)newp;
      //(*lv_lowres)->arg = (uint32_t* )newp;
      (*lv_lowres)->dim = NCHANNELS;
    }
  #else
  numOfComp=NCHANNELS;
  DPRINT("reallocating LV low buffer (@0x%x) from %d elements to %d\n",(uint32_t)(*lv_lowres)->arg,(*lv_lowres)->dim,NCHANNELS);
  checkI32Array(lv_lowres,&numOfComp,error);
  if(error->status){
  	ERR("Resizing array @0x%x\n",(uint32_t)lv_lowres);
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
      DPRINT("reallocating LV hi buffer from %d elements to %d\n",(*lv_hires)->dim,NCHANNELS,(uint32_t)newp);
    //  (*lv_hires)->arg = (uint32_t*)newp;
      *tmp= (uint32_t)newp;
      (*lv_hires)->dim = NCHANNELS;
    }
    #else
    numOfComp=NCHANNELS;
    DPRINT("reallocating LV hi buffer (@0x%x) from %d elements to %d\n",(uint32_t)(*lv_hires)->arg,(*lv_hires)->dim,NCHANNELS);
  	checkI32Array(lv_hires,&numOfComp,error);
  	if(error->status){
  		ERR("Resizing array @0x%x\n",(uint32_t)lv_hires);
  		return 0;
  	}
  	(*lv_hires)->dim = NCHANNELS;
  #endif
  }
  *event_under_run =-1;
  if((*lv_lowres)->arg && (*lv_hires)->arg){
    uint64_t old_counter = handle->cycle;

    ret =caen960_acquire_channels_poll(h,(uint32_t *)(*lv_lowres)->arg,(uint32_t*)(*lv_hires)->arg,0,NCHANNELS,&counter,timeo_ms); 
    *event_under_run = (handle->cycle - old_counter) -1;
    DPRINT("[x%x]acquired %d channels, event underrun %d\n",handle->mapped_address,ret,*event_under_run);

  } else {
    ERR("INVALID INPUT POINTERS lowres:@x%x,hires:@x%x\n",(unsigned)(*lv_lowres)->arg,(unsigned)(*lv_hires)->arg);
    return -1;
  }
  return ret;
}

#endif
