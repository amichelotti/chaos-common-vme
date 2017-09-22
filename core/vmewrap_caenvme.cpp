/*
 @file vmewrap_caenvme.cpp
@author Andrea Michelotti
@date Mar 11, 2016
 */
#include "vmewrap_int.h"
#include <common/debug/core/debug.h>
#include <unistd.h>

#define LINUX
#define LIBRARY "libCAENVME.so"
#include <vmedrv/CAENVMELib/CAENVMElib.h>
#include <vmedrv/CAENVMELib/CAENVMEtypes.h>
#include <vmedrv/CAENVMELib/CAENVMEoslib.h>
#include <dlfcn.h>
typedef	CAENVME_API  (*CAENVME_Init_t)(CVBoardTypes BdType, short Link, short BdNum, int32_t *Handle);
typedef CAENVME_API (*CAENVME_ReadRegister_t)(int32_t Handle, CVRegisters Reg, unsigned int *Data);
typedef CAENVME_API (*CAENVME_WriteRegister_t)(int32_t Handle, CVRegisters Reg, unsigned int Data);
typedef CAENVME_API (*CAENVME_ReadCycle_t)(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
typedef CAENVME_API (*CAENVME_WriteCycle_t)(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
typedef	CAENVME_API (*CAENVME_SystemReset_t)(int32_t Handle);
typedef CAENVME_API (*CAENVME_End_t)(int32_t Handle);
typedef struct _caen_vme {
	CAENVME_API  (*CAENVME_Init)(CVBoardTypes BdType, short Link, short BdNum, int32_t *Handle);
	void (*CAENVME_DecodeError)(CVErrorCodes Code);
	void (*CAENVME_SWRelease)(char *SwRel);
	CAENVME_API (*CAENVME_BoardFWRelease)(int32_t Handle, char *FWRel);
	CAENVME_API (*CAENVME_DriverRelease)(int32_t Handle, char *Rel);
	CAENVME_API (*CAENVME_DeviceReset)(int32_t dev);
	CAENVME_API (*CAENVME_End)(int32_t Handle);
	CAENVME_API (*CAENVME_ReadCycle)(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
	CAENVME_API (*CAENVME_RMWCycle)(int32_t Handle, uint32_t Address,  void *Data,CVAddressModifier AM, CVDataWidth DW);
	CAENVME_API (*CAENVME_WriteCycle)(int32_t Handle, uint32_t Address, void *Data,CVAddressModifier AM, CVDataWidth DW);
	CAENVME_API (*CAENVME_MultiRead)(int32_t Handle, uint32_t *Addrs, uint32_t *Buffer,int NCycles, CVAddressModifier *AMs, CVDataWidth *DWs, CVErrorCodes *ECs);
	CAENVME_API (*CAENVME_MultiWrite)(int32_t Handle, uint32_t *Addrs, uint32_t *Buffer,int NCycles, CVAddressModifier *AMs, CVDataWidth *DWs, CVErrorCodes *ECs);
	CAENVME_API (*CAENVME_LTReadCycle)(int32_t Handle, uint32_t Address, void *Buffer,int Size, CVAddressModifier AM, CVDataWidth DW, int *count);
	CAENVME_API (*CAENVME_FIFOBLTReadCycle)(int32_t Handle, uint32_t Address, void *Buffer,int Size, CVAddressModifier AM, CVDataWidth DW, int *count);
	CAENVME_API (*CAENVME_MBLTReadCycle)(int32_t Handle, uint32_t Address, void *Buffer,int Size, CVAddressModifier AM, int *count);
	CAENVME_API (*CAENVME_FIFOMBLTReadCycle)(int32_t Handle, uint32_t Address, void *Buffer,
			int Size, CVAddressModifier AM, int *count);
	CAENVME_API  (*CAENVME_BLTWriteCycle)(int32_t Handle, uint32_t Address, void *Buffer,
			int size, CVAddressModifier AM, CVDataWidth DW, int *count);

	CAENVME_API  (*CAENVME_FIFOBLTWriteCycle)(int32_t Handle, uint32_t Address, void *Buffer,
			int size, CVAddressModifier AM, CVDataWidth DW, int *count);

	CAENVME_API (*CAENVME_MBLTWriteCycle)(int32_t Handle, uint32_t Address, void *Buffer,
			int size, CVAddressModifier AM, int *count);


	CAENVME_API (*CAENVME_FIFOMBLTWriteCycle)(int32_t Handle, uint32_t Address, void *Buffer,
			int size, CVAddressModifier AM, int *count);


	CAENVME_API (*CAENVME_ADOCycle)(int32_t Handle, uint32_t Address, CVAddressModifier AM);
	CAENVME_API (*CAENVME_ADOHCycle)(int32_t Handle, uint32_t Address, CVAddressModifier AM);
	CAENVME_API (*CAENVME_IACKCycle)(int32_t Handle, CVIRQLevels Level, void *Vector, CVDataWidth DW);
	CAENVME_API (*CAENVME_IRQCheck)(int32_t Handle, CAEN_BYTE *Mask);
	CAENVME_API (*CAENVME_IRQEnable)(int32_t Handle, uint32_t Mask);
	CAENVME_API (*CAENVME_IRQDisable)(int32_t Handle, uint32_t Mask);
	CAENVME_API (*CAENVME_IRQWait)(int32_t Handle, uint32_t Mask, uint32_t Timeout);
	CAENVME_API (*CAENVME_SetPulserConf)(int32_t Handle, CVPulserSelect PulSel, unsigned char Period,
			unsigned char Width, CVTimeUnits Unit, unsigned char PulseNo,
			CVIOSources Start, CVIOSources Reset);
	CAENVME_API (*CAENVME_ReadRegister)(int32_t Handle, CVRegisters Reg, unsigned int *Data);
	CAENVME_API (*CAENVME_WriteRegister)(int32_t Handle, CVRegisters Reg, unsigned int Data);
	CAENVME_API (*CAENVME_BLTReadAsync)(int32_t Handle, uint32_t Address, void *Buffer,
			int Size, CVAddressModifier AM, CVDataWidth DW);
	CAENVME_API (*CAENVME_BLTReadWait)(int32_t Handle, int *Count);
	CAENVME_API (*CAENVME_SystemReset)(int32_t Handle);
	void *libhandle;
	CVAddressModifier am;

} caen_vme_t;
#define INIT_LIB(name)				\
  caen->name = (name ##_t)dlsym(lib,# name);					\
		if(caen->name == NULL){ ERR("Error loaging \"%s\" :%s",# name, dlerror());free(caen);return -2;} else {DPRINT("load " # name);}


static int caenvme_deinit(vmewrap_int_vme_handle_t handle) {
	caen_vme_t* caen_handle;
	if(handle==NULL) return -3;
	caen_handle=(caen_vme_t*)handle->priv;
	caen_handle->CAENVME_SystemReset(handle->fd);
	caen_handle->CAENVME_End(handle->fd);

	return 0;
}

static int  map_master_caenvme(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

	handle->phys_add=(uint64_t)add;
	handle->size = size;

	switch(addressing){
	case VME_ADDRESSING_A16:
		switch(vme_options){
		case VME_OPT_AM_SUPER_AM:
			caen_handle->am=cvA16_S;
			break;
		default:
			caen_handle->am=cvA16_U;
		}
		break;
		case VME_ADDRESSING_A24:
			switch(vme_options){
			case (VME_OPT_AM_SUPER_AM| VME_OPT_AM_DATA_AM):
					  caen_handle->am=cvA24_S_DATA;
			break;
			case (VME_OPT_AM_SUPER_AM| VME_OPT_AM_PROG_AM):
		  					caen_handle->am=cvA24_S_PGM;
			break;
			case (VME_OPT_AM_SUPER_AM| VME_OPT_BLT_ON):
		  				caen_handle->am=cvA24_S_MBLT;
			break;
			case (VME_OPT_AM_NON_PRIV_AM| VME_OPT_AM_DATA_AM):
		  					  caen_handle->am=cvA24_U_DATA;
			break;
			case (VME_OPT_AM_NON_PRIV_AM| VME_OPT_AM_PROG_AM):
		  		  					caen_handle->am=cvA24_U_PGM;
			break;
			case (VME_OPT_AM_NON_PRIV_AM| VME_OPT_BLT_ON):
		  		  				caen_handle->am=cvA24_U_BLT;
			break;
			default:
				caen_handle->am=cvA24_U_DATA;
			break;
			}
			break;

			case VME_ADDRESSING_A32:
				switch(vme_options){
				case (VME_OPT_AM_SUPER_AM| VME_OPT_AM_DATA_AM):
							  caen_handle->am=cvA32_S_DATA;
				break;
				case (VME_OPT_AM_SUPER_AM| VME_OPT_AM_PROG_AM):
				  					caen_handle->am=cvA32_S_PGM;
				break;
				case (VME_OPT_AM_SUPER_AM| VME_OPT_BLT_ON):
				  				caen_handle->am=cvA32_S_MBLT;
				break;
				case (VME_OPT_AM_NON_PRIV_AM| VME_OPT_AM_DATA_AM):
				  					  caen_handle->am=cvA32_U_DATA;
				break;
				case (VME_OPT_AM_NON_PRIV_AM| VME_OPT_AM_PROG_AM):
				  		  					caen_handle->am=cvA24_U_PGM;
				break;
				case (VME_OPT_AM_NON_PRIV_AM| VME_OPT_BLT_ON):
				  		  				caen_handle->am=cvA32_U_BLT;
				break;
				default:
					caen_handle->am=cvA32_U_DATA;
				}

				break;

				default:
					ERR("addressing not implemented %d",addressing);
					return -1;
	}

	return 0;

}



static int caenvme_init(vmewrap_int_vme_handle_t handle){

	int status, caenst;
	CVBoardTypes   VMEBoard;
	short          Link, Device;
	int BHandle=0;
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

	unsigned int DataShort=0;
	//  VMEBoard = cvV1718;
	switch (handle->type){
	case VME_CAEN1718_DRIVER:
		VMEBoard = cvV1718;
		break;
	case VME_CAEN2718_DRIVER:
		VMEBoard = cvV2718;
		break;
	default:
		ERR("unsupported %d CAENVME board",handle->type);
		return -1;
	}

	Device = 0;
	Link = 0;


	if( caen_handle->CAENVME_Init(VMEBoard, Link, Device, &BHandle) != cvSuccess ){
		ERR("Error opening the device");
		return -2;
	} else{
		DPRINT("CAENVME initialized");
	}

	caen_handle->CAENVME_SystemReset(BHandle);
	usleep(10000);

	status = 1;
	caenst = caen_handle->CAENVME_ReadRegister(BHandle, cvStatusReg, &DataShort);
	status *= (1-caenst);
	if(status==1){
		DPRINT("CaenVME Status reg \n Bridge is Controlled = %i, USB speed = %i, ",(DataShort & 0x2)>>1,(DataShort & 0x8000)>>16);
	}

	handle->fd = BHandle;

	return 0;
}



static int vme_write8_caenvme(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t data){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;
	if(handle==NULL) return -100;
	  DPRINT(" addr=0x%x, am = 0x%x dw=8 ptr=@0x%x",off,caen_handle->am,data);
	  return caen_handle->CAENVME_WriteCycle(handle->fd, (uint32_t)handle->phys_add + off, &data,caen_handle->am, cvD8);
}

static int vme_write32_caenvme(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t data){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

	if(handle==NULL) return -100;
		  DPRINT(" addr=0x%x, am = 0x%x dw=32 ptr=@0x%x",off,caen_handle->am,data);
		  return caen_handle->CAENVME_WriteCycle(handle->fd, (uint32_t)handle->phys_add + off, &data,caen_handle->am, cvD32);

}
static int vme_write16_caenvme(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t data){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

		if(handle==NULL) return -100;
			  DPRINT(" addr=0x%x, am = 0x%x dw=16 ptr=@0x%x",off,caen_handle->am,data);
			  return caen_handle->CAENVME_WriteCycle(handle->fd, (uint32_t)handle->phys_add + off, &data,caen_handle->am, cvD16);

}
static int vme_read32_caenvme(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t *data){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

			if(handle==NULL) return -100;
				  DPRINT(" addr=0x%x, am = 0x%x dw=32 ptr=@%p",off,caen_handle->am,data);
				  return caen_handle->CAENVME_ReadCycle(handle->fd, (uint32_t)handle->phys_add + off, data,caen_handle->am, cvD32);
}
static int vme_read16_caenvme(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t *data){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

				if(handle==NULL) return -100;
					  DPRINT(" addr=0x%x, am = 0x%x dw=16 ptr=@%p\n",off,caen_handle->am,data);
					  return caen_handle->CAENVME_ReadCycle(handle->fd, (uint32_t)handle->phys_add + off, data,caen_handle->am, cvD16);
}
static int vme_read8_caenvme(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t *data){
	caen_vme_t* caen_handle=(caen_vme_t*)handle->priv;

				if(handle==NULL) return -100;
					  DPRINT(" addr=0x%x, am = 0x%x dw=8 ptr=@%p",off,caen_handle->am,data);
					  return caen_handle->CAENVME_ReadCycle(handle->fd, (uint32_t)handle->phys_add + off, data,caen_handle->am, cvD8);
}

int vme_init_driver_caenvme(vmewrap_vme_handle_t handle){
	vmewrap_int_vme_handle_t p=(vmewrap_int_vme_handle_t)handle;
	caen_vme_t *caen;
	void *lib;
	DPRINT("try to open " LIBRARY);
	lib = dlopen(LIBRARY, RTLD_NOW | RTLD_GLOBAL );
	if (!handle) {
		ERR("Error opening " LIBRARY " :%s",dlerror());

		return -1;
	}

	dlerror();    /* Clear any existing error */
	caen = (caen_vme_t*) calloc(1,sizeof(caen_vme_t));
	if(caen==NULL){
		ERR("cannot allocate internal caen handle");
		return -2;
	}
	caen->libhandle=lib;
	INIT_LIB(CAENVME_Init);
	INIT_LIB(CAENVME_ReadRegister);
	INIT_LIB(CAENVME_ReadCycle);
	INIT_LIB(CAENVME_WriteCycle);
	INIT_LIB(CAENVME_SystemReset);
	INIT_LIB(CAENVME_End);
	p->vme_init=caenvme_init;
	p->vme_close=caenvme_deinit;
	p->map_master=map_master_caenvme;
	p->map_slave=NULL;
	p->vme_read16 = vme_read16_caenvme;
	p->vme_read32 = vme_read32_caenvme;
	p->vme_read8 = vme_read8_caenvme;
	p->vme_write16 = vme_write16_caenvme;
		p->vme_write32 = vme_write32_caenvme;
		p->vme_write8 = vme_write8_caenvme;
	p->priv = caen;
	return 0;
}
int vme_deinit_driver_caenvme(vmewrap_vme_handle_t handle){
	vmewrap_int_vme_handle_t p=(vmewrap_int_vme_handle_t)handle;
	if(p==NULL){
		ERR("invalid handle");
		return -1;
	}
	caen_vme_t *caen=(caen_vme_t *)p->priv;
	dlclose(caen->libhandle);
	delete caen;
	p->priv=NULL;
	return 0;
}




