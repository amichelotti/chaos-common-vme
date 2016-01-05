#include "sis3800_drv.h"
#include "vmewrap.h"
#include <stdlib.h>
#include <common/debug/core/debug.h>
#define DEV_BOARD "SIS3800"
#define BOARD_IDENTIFICATION_REG(base) REG32(base,0x4)

typedef struct __vme_handle__ {
    vmewrap_vme_handle_t vme;
    void* mapped_address;
    uint64_t cycle;
    int boardid;
    int version;
} _sis_handle_t ;

sis3800_handle_t sis3800_open(uint32_t address){
    void* mapped_address;
    int size = 0x10000;
    int boardid,manufactureid;
    vmewrap_vme_handle_t vme;
    DPRINT(DEV_BOARD " opening vme device at @0x%x\n",address);
    vme = vmewrap_vme_open_master(address,size,32,0);
    if(vme==NULL){
        return NULL;
    }
    
    mapped_address =  vmewrap_get_linux_add(vme);
    if (0 == mapped_address) {
        ERR("cannot map VME window\n");
        perror("vme_master_window_map");
        return 0;
    }
    
    _sis_handle_t *p = ( _sis_handle_t *)malloc(sizeof(_sis_handle_t));
    if(p==NULL){
        ERR("cannot allocate resources\n");
        vmewrap_vme_close(vme);
        return 0;
    }
    p->vme = vme;
    p->mapped_address = mapped_address;
    p->cycle = 0;
    boardid=BOARD_IDENTIFICATION_REG(mapped_address)>>12;
    p->version=boardid&0xF;
    p->boardid=boardid>>4;
    DPRINT(DEV_BOARD " successfully mapped at @0x%x\n",mapped_address);
    PRINT(DEV_BOARD " Version:0x%x\n",p->version);
    PRINT(DEV_BOARD " BoardID:0x%x\n",p->boardid);
    return (void*) p;
}


int32_t sis3800_close(sis3800_handle_t h){
     _sis_handle_t* handle = h;
    DPRINT(DEV_BOARD " closing handle @0x%x\n",(unsigned)h);
    if(handle){
        int ret;
        DPRINT(DEV_BOARD " caen_qdc sucessfully closed\n");
        ret=vmewrap_vme_close(handle->vme);
        free(handle);
        return ret;
    }
    
    return -4;

}


int32_t sis3800_init(sis3800_handle_t h){
    sis3800_reset(h);
    sis3800_clear(h);
    sis3800_enable(h,1);
    return 0;
}

uint32_t sis3800_readnclear(sis3800_handle_t h,int countern){
    _sis_handle_t* handle = h;

    return REG32(handle->mapped_address,0x300+(countern<<2));
}

uint32_t sis3800_readCounter(sis3800_handle_t h,int countern){
    _sis_handle_t* handle = h;
    
    return REG32(handle->mapped_address,0x280+(countern<<2));
}

void sis3800_disableCounter(sis3800_handle_t h,int32_t counter){
    _sis_handle_t* handle = h;
     REG32(handle->mapped_address,0xC)=counter;
}

uint32_t sis3800_getOverflows(sis3800_handle_t h){
    uint32_t res=0;
    _sis_handle_t* handle = h;
    
    res|=(REG32(handle->mapped_address,0x380)>>24);
    res|=(REG32(handle->mapped_address,0x3A0)>>24)<<8;
    res|=(REG32(handle->mapped_address,0x3C0)>>24)<<16;
    res|=(REG32(handle->mapped_address,0x3E0)>>24)<<24;
    return res;

}

void sis3800_enable(sis3800_handle_t h,int enable){
      _sis_handle_t* handle = h;
    if(enable>0){
        REG32(handle->mapped_address,0x28)=1;
    } else{
        REG32(handle->mapped_address,0x2C)=1;

    }
}


void sis3800_clear(sis3800_handle_t h){
      _sis_handle_t* handle = h;
    REG32(handle->mapped_address,0x20)=1;

}
void sis3800_reset(sis3800_handle_t h){
      _sis_handle_t* handle = h;
    REG32(handle->mapped_address,0x60)=1;

}
