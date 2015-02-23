/*
  CAEN513 driver
  @release: 0.1
  @author: Andrea Michelotti
*/

#include "caen513_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmewrap.h"
#include <sys/time.h>

// Register map

#define FW_REVISION_REG(base) REG16(base,0x1000)
#define BOARD_ID_REG(base) REG16(base,0x803A)
#define BOARD_ID_LSB_REG(base) REG16(base,0x803E)
#define BOARD_ID_MSB_REG(base) REG16(base,0x8036)

typedef struct __vme_handle__ {
 vmewrap_vme_handle_t vme;
  uint32_t mapped_address;
 } _caen513_handle_t ;




caen513_handle_t caen513_open(uint32_t address ){
  int am,flags;
  uint32_t mapped_address;
  int size = 0x10000;
  int boardid,manufactureid;
  vmewrap_vme_handle_t vme;
  DPRINT("opening vme device at @0x%x\n",address);
  vme = vmewrap_vme_open(address,size,32,0,0,0);
  if(vme==NULL) return NULL;

  mapped_address =  vmewrap_get_vme_master_linux_add(vme);
  if (0 == mapped_address) {
    ERR("cannot map VME window\n");
    perror("vme_master_window_map");
    return 0;
  }

  _caen513_handle_t* p = (_caen513_handle_t* )malloc(sizeof(_caen513_handle_t));
  if(p==NULL){
    ERR("cannot allocate resources\n");
    vmewrap_vme_close(vme);
    return 0;
  }
  p->vme = vme;
  p->mapped_address = mapped_address;
  p->cycle = 0;
  boardid=BOARD_ID_REG(mapped_address)&0xFF;
  manufactureid=OUI_REG(mapped_address)&0xFF;
  DPRINT("caen513 successfully mapped at @0x%x\n",mapped_address);
  PRINT("CV 965 FW:0x%x\n",FW_REVISION_REG(mapped_address));
  PRINT("CV 965 BoardID:0x%x\n",boardid);
  PRINT("CV 965 Manufacture:0x%x\n",manufactureid);
  if(boardid!=BOARD_ID || manufactureid!=MANUFACTURE_ID){
    ERR("device not identified expected BoardId=0x%x ManufactureId=0x%x\n",BOARD_ID,MANUFACTURE_ID);
    caen513_close((caen513_handle_t)p);
    return 0;
  }
  
  return (caen513_handle_t) p; 
}

int32_t caen513_close(caen513_handle_t h){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  DPRINT("closing handle @0x%x\n",(unsigned)h);
  if(handle){
    int ret;
    DPRINT("caen513 sucessfully closed\n");
    ret=vmewrap_vme_close(handle->vme);
    free(handle);
    return ret;
  }
  
  return -4;
}

void caen513_setChannelMode(caen513_handle_t h,int channel,int mode){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x10 +2 *channel)=mode;
  return ;
}

int32_t caen513_getChannelMode(caen513_handle_t h,int channel){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  return REG16(handle->mapped_address,0x10 +2 *channel);

}
void caen513_interruptMask(caen513_handle_t h,int mask){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x8)=mask;
}

void caen513_setStrobe(caen513_handle_t h,int mode){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x6)=mode;
}
int32_t caen513_getStrobe(caen513_handle_t h){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  return REG16(handle->mapped_address,0x6);
}

int32_t caen513_get(caen513_handle_t h){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  return REG16(handle->mapped_address,0x4);
}
void caen513_set(caen513_handle_t h,int mask){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x4)=mask;
}

void caen513_reset(caen513_handle_t h){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x42)=1;
}

void caen513_clear(caen513_handle_t h){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x48)=1;
}

void caen513_clearStrobe(caen513_handle_t h){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x44)=1;
}


int32_t caen513_init(caen513_handle_t h,int mode){
  _caen513_handle_t* handle = (_caen513_handle_t*)h;
  REG16(handle->mapped_address,0x46)=mode;
  caen513_reset(h);
  return 0;
}


