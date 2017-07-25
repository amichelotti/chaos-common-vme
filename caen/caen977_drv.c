/*
  CAEN977 driver
  @release: 0.1
  @author: Andrea Michelotti
 */

#include "caen977_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vmewrap.h"
#include <sys/time.h>

#include <common/debug/core/debug.h>

// Register map

typedef struct __vme_handle__ {
	vmewrap_vme_handle_t vme;
	void* mapped_address;
} _caen977_handle_t ;

#define REG_INPUT_SET 0x0
#define REG_INPUT_MASK 0x2
#define REG_INPUT_READ 0x4
#define REG_SINGLEHIT_READ 0x6
#define REG_MULTIHIT_READ 0x8
#define REG_OUTPUT_SET 0xA
#define REG_OUTPUT_MASK 0xC
#define REG_INTERRUPT_MASK 0xE
#define REG_CLEAR_OUTPUT 0x10
#define REG_SINGLEHIT_READ_CLR 0x16
#define REG_MULTIHIT_READ_CLR 0x18
#define REG_TEST_CTRL 0x1A
#define REG_INT_LEVEL 0x20
#define REG_INT_VECTOR 0x22
#define REG_SERIAL_NUMBER 0x24
#define REG_FIRMWARE_REV 0x26
#define REG_CTRL_REG 0x28
#define REG_SW_RES 0x2E

#define VERSION_REG(base) VME_READ_REG16(base,REG_FIRMWARE_REV)
#define BOARD_ID_REG(base) VME_READ_REG16(base,REG_SERIAL_NUMBER)
#define MANUFACTURE_ID_REG(base) 0x0


uint16_t caen977_getOperationMode(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;

	return VME_REG_READ16(handle->vme,REG_CTRL_REG);
}


void caen977_interruptMask(caen977_handle_t h,uint16_t mask){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;

	 VME_REG_WRITE16(handle->vme,REG_INTERRUPT_MASK,mask);
}


void caen977_outputMask(caen977_handle_t h,uint16_t mask){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;

	VME_REG_WRITE16(handle->vme,REG_OUTPUT_MASK,mask);
}

void caen977_inputMask(caen977_handle_t h,uint16_t mask){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	VME_REG_WRITE16(handle->vme,REG_INPUT_MASK,mask);
}

uint16_t caen977_inputRead(caen977_handle_t h,uint16_t level, int readClear){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	uint16_t ret;

	switch(level){
	case 2:
		if(readClear){
			ret = VME_REG_READ16(handle->vme,REG_MULTIHIT_READ_CLR);
		} else {
			ret = VME_REG_READ16(handle->vme,REG_MULTIHIT_READ);
		}
		break;

	case 1:
		if(readClear){
			ret = VME_REG_READ16(handle->vme,REG_SINGLEHIT_READ_CLR);
		} else {
			ret = VME_REG_READ16(handle->vme,REG_SINGLEHIT_READ);
		}
		break;
	default:
		ret = VME_REG_READ16(handle->vme,REG_INPUT_READ);
	}
	return ret;
}

void caen977_outputSet(caen977_handle_t h,uint16_t mask){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	VME_REG_WRITE16(handle->vme,REG_OUTPUT_SET,mask);
}

void caen977_outputClear(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	VME_REG_WRITE16(handle->vme,REG_CLEAR_OUTPUT,0x1);
}

/**
	control the test input channel operation
	@param h handle to the board
	@param mode the test mode operation
*/
void caen977_testCtrlRegister(caen977_handle_t h,uint16_t mode);


caen977_handle_t caen977_open(vme_driver_t vme_driver,uint32_t address ){
	int am,flags;
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
	if(vmewrap_vme_open_master(vme,address,size,VME_ADDRESSING_A32,VME_ACCESS_D16,0)!=0){
		ERR("cannot map vme");
		return NULL;
	}

	mapped_address =  vmewrap_get_linux_add(vme);
	if (0 == mapped_address) {
		ERR("cannot map VME window\n");
		perror("vme_master_window_map");
		return 0;
	}

	_caen977_handle_t* p = (_caen977_handle_t* )malloc(sizeof(_caen977_handle_t));
	if(p==NULL){
		ERR("cannot allocate resources\n");
		vmewrap_vme_close(vme);
		return 0;
	}
	p->vme = vme;
	p->mapped_address = mapped_address;
	boardid=BOARD_ID_REG(mapped_address)&0xFFFF;
	manufactureid=MANUFACTURE_ID_REG(mapped_address)&0xFFFF;
	DPRINT("CAEN977 successfully mapped at @0x%p\n",mapped_address);
	PRINT("CAEN977 Version:0x%x\n",VERSION_REG(mapped_address));
	PRINT("CAEN977 BoardID:0x%x\n",boardid);
	PRINT("CAEN977 Manufacture:0x%x\n",manufactureid);
/*	if(manufactureid!=MANUFACTURE_ID){
		ERR("device not identified expected ManufactureId=0x%x\n",MANUFACTURE_ID);
		caen977_close((caen977_handle_t)p);
		return 0;
	}
*/
	return (caen977_handle_t) p;
}

int32_t caen977_close(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	DPRINT("closing handle @0x%p\n",(void*)h);
	if(handle){
		int ret;
		DPRINT("caen977 sucessfully closed\n");
		ret=vmewrap_vme_close(handle->vme);
		free(handle);
		return ret;
	}

	return -4;
}

void caen977_setChannelMode(caen977_handle_t h,int channel,int mode){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	unsigned off=0x10 +2 *channel;
	VME_WRITE16(handle->vme,off,mode);

	return ;
}

int32_t caen977_getChannelMode(caen977_handle_t h,int channel){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	return REG16(handle->mapped_address,0x10 +2 *channel);

}

void caen977_setStrobe(caen977_handle_t h,int mode){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	REG16(handle->mapped_address,0x6)=mode;
}
int32_t caen977_getStrobe(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	return REG16(handle->mapped_address,0x6);
}

int32_t caen977_get(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	return REG16(handle->mapped_address,0x4);
}
void caen977_set(caen977_handle_t h,int mask){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	//  REG16(handle->mapped_address,0x4)=mask;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x4,mask);
}

void caen977_reset(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;

	VME_WRITE16(handle->vme,REG_SW_RES,1);
}

void caen977_clear(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	//  REG16(handle->mapped_address,0x48)=1;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x48,1);
}

void caen977_clearStrobe(caen977_handle_t h){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	//  REG16(handle->mapped_address,0x44)=1;
	//  msync(handle->mapped_address,0x100);
	VME_WRITE16(handle->vme,0x44,1);
}


int16_t caen977_init(caen977_handle_t h,uint16_t mode){
	_caen977_handle_t* handle = (_caen977_handle_t*)h;
	caen977_reset(h);
	if(mode>=0){

		VME_WRITE16(handle->vme,REG_CTRL_REG,mode);
	}


	return 0;
}


