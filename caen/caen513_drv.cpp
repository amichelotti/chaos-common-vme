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

#include <common/debug/core/debug.h>

// Register map

#define VERSION_REG(base) REG16(base,0xFE)
#define BOARD_ID_REG(base) REG16(base,0xFA)
#define MANUFACTURE_ID_REG(base) REG16(base,0xFC)
typedef struct __vme_handle__ {
	vmewrap_vme_handle_t vme;
	void* mapped_address;
} _caen513_handle_t ;




caen513_handle_t caen513_open(vme_driver_t vme_driver,uint32_t address ){
	void* mapped_address;
	int size = 0x10000;
	int boardid,manufactureid;
	vmewrap_vme_handle_t vme;
	DPRINT("opening vme device at @0x%x\n",address);
	vme = vmewrap_init_driver(vme_driver);
	if(vme==NULL){
		ERR("cannot initialize VME driver %d",vme_driver);

		return NULL;
	}
	if(vmewrap_vme_open_master(vme,address,size,VME_ADDRESSING_A32,VME_ACCESS_D32,VME_OPT_AM_SUPER_AM)!=0){
		ERR("cannot map vme");
		return NULL;
	}

	mapped_address =  vmewrap_get_linux_add(vme);
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
	boardid=BOARD_ID_REG(mapped_address)&0xFFFF;
	manufactureid=MANUFACTURE_ID_REG(mapped_address)&0xFFFF;
	DPRINT("CAEN513 successfully mapped at @%p\n",mapped_address);
	PRINT("CAEN513 Version:0x%x\n",VERSION_REG(mapped_address));
	PRINT("CAEN513 BoardID:0x%x\n",boardid);
	PRINT("CAEN513 Manufacture:0x%x\n",manufactureid);
	if(manufactureid!=V513_MANUFACTURE_ID){
		ERR("device not identified expected ManufactureId=0x%x\n",V513_MANUFACTURE_ID);
		caen513_close((caen513_handle_t)p);
		return 0;
	}

	return (caen513_handle_t) p;
}

int32_t caen513_close(caen513_handle_t h){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	DPRINT("closing handle @%p\n",h);
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
	unsigned off=0x10 +2 *channel;
	VME_WRITE16(handle->vme,off,mode);

	return ;
}

int32_t caen513_getChannelMode(caen513_handle_t h,int channel){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	return REG16(handle->mapped_address,0x10 +2 *channel);

}
void caen513_interruptMask(caen513_handle_t h,int mask){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	VME_WRITE16(handle->vme,0x8,mask);

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
	//  REG16(handle->mapped_address,0x4)=mask;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x4,mask);
}

void caen513_reset(caen513_handle_t h){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	//  REG16(handle->mapped_address,0x42)=1;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x42,1);
}

void caen513_clear(caen513_handle_t h){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	//  REG16(handle->mapped_address,0x48)=1;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x48,1);
}

void caen513_clearStrobe(caen513_handle_t h){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	//  REG16(handle->mapped_address,0x44)=1;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x44,1);
}


int32_t caen513_init(caen513_handle_t h,int mode){
	_caen513_handle_t* handle = (_caen513_handle_t*)h;
	if(mode>=0){
		//    REG16(handle->mapped_address,0x46)=mode;
		//    msync(handle->mapped_address,0x100);
		VME_WRITE16(handle->vme,0x46,mode);
	}

	//  caen513_reset(h);
	//  caen513_clear(h);
	return 0;
}


