
#ifndef _VMEWRAPUNIVERSE2_H_

#define _VMEWRAPUNIVERSE2_H_
#include <vmedrv/universe2/vmelib.h>
#include "vmewrap_int.h"
#include <common/debug/core/debug.h>
static VMEBridge* vmeb=NULL;
static int initialized=0;
static int vme_close_universe2(vmewrap_int_vme_handle_t handle){
    if(handle && handle->bus){
        VMEBridge* vme=(VMEBridge*)handle->bus;
	if(handle->fd>0){
	  vme->releaseImage(handle->fd);
	}

	//	DPRINT("delete vme bridge 0x%x",vme);
	//        delete vme;
    }
    return 0;
}
static int vme_init_universe2(vmewrap_int_vme_handle_t handle){
	if(vmeb==NULL){
		ERR("init driver must be done before");
		return -1;
	}
    handle->bus = ( VMEBridge*)vmeb;
    handle->fd=-1;
    if(initialized==0){    
      initialized=1;
      DPRINT("initialized driver bus:0x%x",handle->bus);

      //      return vmeb.resetDriver();
    }
    return 0;
    
}
static int  map_master_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options){

  VMEBridge* vme=(VMEBridge*)handle->bus;
  int image;
  int uni_addressing,uni_dw;
  switch(addressing){
  case VME_ADDRESSING_A16:
    uni_addressing = A16;
    break;
  case VME_ADDRESSING_A24:
    uni_addressing = A24;
    break;
  case VME_ADDRESSING_A32:
    uni_addressing = A32;
    break;
  default:
	ERR("addressing not implemented %d",addressing);
	return -1;
  }

  switch(dw){
  case VME_ACCESS_D16:
    uni_dw = D16 ;
    break;
  case VME_ACCESS_D8:
    uni_dw = D8 ;
    break;
  case VME_ACCESS_D32:
    uni_dw = D32;
    break;
  case VME_ACCESS_D64:
    uni_dw = D64;
    break;
  default:
	    ERR("data access not implemented %d",dw);
	    return -2;
  }

  image=vme->getImage(add,size,uni_addressing,uni_dw,MASTER);
  if(vme_options){
    vme->setOption(image,vme_options);
  }
  handle->fd=image;
  if (image < 0) {
      ERR("Can't allocate master image vmeadd:0x%x, size:0x%x",add,size);
       return -3;
    }
  handle->mapped_address=  vme->getPciBaseAddr(image);
  if( (int64_t)handle->mapped_address == -1){
     ERR("cannot map vme master address");
     return -4;
 } 
 
  DPRINT("Master address mapped at @0x%x size 0x%x, addressing %d (0x%x) dw %d (0x%x)",handle->mapped_address,size,addressing,uni_addressing,dw,uni_dw);
  return 0;
}

static int  map_slave_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw,vme_opt_t vme_options){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  int am=0;
  int image;
  int uni_addressing,uni_dw;
   switch(addressing){
   case VME_ADDRESSING_A16:
     uni_addressing = A16;
     break;
   case VME_ADDRESSING_A24:
     uni_addressing = A24;
     break;
   case VME_ADDRESSING_A32:
     uni_addressing = A32;
     break;
   default:
 	ERR("addressing not implemented %d",addressing);
 	return -1;
   }

   switch(dw){
   case VME_ACCESS_D16:
     uni_dw = D16 ;
     break;
   case VME_ACCESS_D8:
     uni_dw = D8 ;
     break;
   case VME_ACCESS_D32:
     uni_dw = D32;
     break;
   case VME_ACCESS_D64:
     uni_dw = D64;
     break;
   default:
 	    ERR("data access not implemented %d",dw);
 	    return -2;
   }

  if(vme_options){
    vme->setOption(image,vme_options);
  }
  image=vme->getImage(add,size,uni_addressing,uni_dw,SLAVE);
  
  if (image < 0) {
      ERR("Can't allocate slave image vmeadd:0x%x, size:0x%x",add,size);
       return -3;
    }
  handle->fd=image;
  handle->mapped_address=  vme->getPciBaseAddr(image);
 if( (int64_t)handle->mapped_address == -1){
     ERR("cannot map vme slave address");
     return -4;
 } 
 
  DPRINT("slave address mapped at @0x%x size 0x%x",handle->mapped_address,size);
  return 0;
}
static int vme_write8_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
#ifdef DONT_USE_MAP
  return vme->wb(handle->fd,(uint32_t)handle->phys_add + off,data);
#else
  REG8(handle->mapped_address,off)=data;
  return 0;
#endif
}

static int vme_write32_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
#ifdef DONT_USE_MAP
  return vme->wl(handle->fd,(uint32_t)handle->phys_add + off,data);
#else
  REG32(handle->mapped_address,off)=data;
  return 0;
#endif
}
static int vme_write16_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
#ifdef DONT_USE_MAP
  return vme->ww(handle->fd,(uint32_t)handle->phys_add + off,data);
#else
  REG16(handle->mapped_address,off)=data;
  return 0;

#endif  
}
static int vme_read32_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t *data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
#ifdef DONT_USE_MAP
  return vme->rl(handle->fd,(uint32_t)handle->phys_add + off,data);
#else
  *data =REG32(handle->mapped_address,off);
  return 1;
#endif
}
static int vme_read16_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t *data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
#ifdef DONT_USE_MAP
  return vme->rw(handle->fd,(uint32_t)handle->phys_add + off,data);
#else
  *data =REG16(handle->mapped_address,off);
  return 1;
#endif
}
static int vme_read8_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t *data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
#ifdef DONT_USE_MAP
  return vme->rb(handle->fd,(uint32_t)handle->phys_add + off,data);
#else
  *data = REG8(handle->mapped_address,off);
  return  1;
#endif
}

int vme_init_driver_universe2(vmewrap_vme_handle_t handle){
	vmewrap_int_vme_handle_t p=(vmewrap_int_vme_handle_t)handle;
	if(vmeb==NULL){
			vmeb = new VMEBridge();
			DPRINT("allocated a new vme universe2 bridge 0x%x",(void*)vmeb);
		} else {
			DPRINT("reusing vme universe2 bridge 0x%x",(void*)vmeb);
	}
	p->vme_close=vme_close_universe2;
	p->vme_init=vme_init_universe2;
	p->map_master=map_master_universe2;
	p->map_slave=map_slave_universe2;
	p->vme_write8=vme_write8_universe2;
	p->vme_write16=vme_write16_universe2;
	p->vme_write32=vme_write32_universe2;

	p->vme_read8=vme_read8_universe2;
	p->vme_read16=vme_read16_universe2;
	p->vme_read32=vme_read32_universe2;
	return 0;
}
int vme_deinit_driver_universe2(vmewrap_vme_handle_t handle){
	vmewrap_int_vme_handle_t p=(vmewrap_int_vme_handle_t)handle;
	DPRINT("deallocating universe2 driver 0x%x",(void*)vmeb);
	int ret=vme_close_universe2(p);
	if(vmeb){
		delete vmeb;
		vmeb=NULL;
	}
	return ret;
}
#endif

