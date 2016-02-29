
#ifndef _VMEWRAPUNIVERSE2_H_

#define _VMEWRAPUNIVERSE2_H_
#include <vmedrv/universe2/vmelib.h>
#define VME_INT_MAP_MASTER(handle,ARGS...) map_master_universe2(handle, ## ARGS)
#define VME_INT_MAP_SLAVE(handle,ARGS...) map_slave_universe2(handle, ## ARGS)
#define VME_INT_INIT(handle) vme_init_universe2(handle)
#define VME_INT_CLOSE(handle) vme_close_universe2(handle)
#define VME_INT_WRITE32(handle,off,data) vme_write32_universe2(handle,off,data)
#define VME_INT_WRITE16(handle,off,data) vme_write16_universe2(handle,off,data)
#define VME_INT_WRITE8(handle,off,data) vme_write8_universe2(handle,off,data)
#define VME_INT_READ32(handle,off,data) vme_read32_universe2(handle,off,data)
#define VME_INT_READ16(handle,off,data) vme_read16_universe2(handle,off,data)
#define VME_INT_READ8(handle,off,data) vme_read8_universe2(handle,off,data)

static VMEBridge vmeb;
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
  
    handle->bus = ( VMEBridge*)&vmeb;
    handle->fd=-1;
    if(initialized==0){    
      initialized=1;
      //      return vmeb.resetDriver();
    }
    return 0;
    
}
static int  map_master_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options){

  VMEBridge* vme=(VMEBridge*)handle->bus;
  int image;

  switch(addressing){
  case 16:
    addressing = VME_ADDRESSING_A16;
    break;
  case 24:
    addressing = VME_ADDRESSING_A24;
    break;
  case 32:
    addressing = VME_ADDRESSING_A32;
    break;
  }

  switch(dw){
  case 16:
    dw = VME_ACCESS_D16;
    break;
  case 8:
    dw = VME_ACCESS_D8;
    break;
  case 32:
    dw = VME_ACCESS_D32;
    break;
  case 64:
    dw = VME_ACCESS_D64;
    break;
  }


  if((addressing!=VME_ADDRESSING_A32) && (addressing !=VME_ADDRESSING_A16) && (addressing !=VME_ADDRESSING_A24)){
    ERR("addressing not implemented %d",addressing);
    return 0;
  }
  if((dw!=VME_ACCESS_D64) && (dw !=VME_ACCESS_D32) && (dw !=VME_ACCESS_D16) && (dw !=VME_ACCESS_D8)){
    ERR("data access not implemented %d",dw);
    return 0;
  }
  image=vme->getImage(add,size,addressing,dw,MASTER);
  if(vme_options){
    vme->setOption(image,vme_options);
  }
  handle->fd=image;
  if (image < 0) {
      ERR("Can't allocate master image vmeadd:0x%x, size:0x%x",add,size);
       return 0;
    }
  handle->mapped_address=  vme->getPciBaseAddr(image);
  if( (int64_t)handle->mapped_address == -1){
     ERR("cannot map vme address");
     return 0;
 } 
 
  DPRINT("Master address mapped at @0x%x size 0x%x, addressing 0x%x dw: 0x%x",handle->mapped_address,size,addressing,dw);
  return 1;
}

static int  map_slave_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw,vme_opt_t vme_options){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  int am=0;
  int image;
  switch(addressing){
  case 16:
    addressing = VME_ADDRESSING_A16;
    break;
  case 24:
    addressing = VME_ADDRESSING_A24;
    break;
  case 32:
    addressing = VME_ADDRESSING_A32;
    break;
  }

  switch(dw){
  case 16:
    dw = VME_ACCESS_D16;
    break;
  case 8:
    dw = VME_ACCESS_D8;
    break;
  case 32:
    dw = VME_ACCESS_D32;
    break;
  case 64:
    dw = VME_ACCESS_D64;
    break;
  }

  if((addressing!=VME_ADDRESSING_A32) && (addressing !=VME_ADDRESSING_A16) && (addressing !=VME_ADDRESSING_A24)){
    ERR("addressing not implemented %d",addressing);
    return 0;
  }
  if((dw!=VME_ACCESS_D64) && (dw !=VME_ACCESS_D32) && (dw !=VME_ACCESS_D16) && (dw !=VME_ACCESS_D8)){
    ERR("data access not implemented %d",dw);
    return 0;
  }


  if(vme_options){
    vme->setOption(image,vme_options);
  }
  image=vme->getImage(add,size,addressing,dw,SLAVE);
  
  if (image < 0) {
      ERR("Can't allocate slave image vmeadd:0x%x, size:0x%x",add,size);
       return 0;
    }
  handle->fd=image;
  handle->mapped_address=  vme->getPciBaseAddr(image);
 if( (int64_t)handle->mapped_address == -1){
     ERR("cannot map vme address");
     return 0;
 } 
 
  DPRINT("slave address mapped at @0x%x size 0x%x",handle->mapped_address,size);
  return 1;
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


#endif


