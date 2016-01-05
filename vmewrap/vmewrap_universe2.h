
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
static int  map_master_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing,uint32_t vme_opt){
  int am;
  VMEBridge* vme=(VMEBridge*)handle->bus;
  int image;
  if(addressing==32)
    am = A32;
  else if(addressing==16){
    am = A16;
  } else if(addressing==24){
    am = A24;
  } else {
    ERR("master addressing not implemented %d\n",addressing);
    return 0;
  }

  image=vme->getImage(add,size,am,D32,MASTER);
  //  vme->setOption(image,SUPER_AM);
  handle->fd=image;
  if (image < 0) {
      ERR("Can't allocate master image vmeadd:0x%x, size:0x%x\n",add,size);
       return 0;
    }
  handle->mapped_address=  vme->getPciBaseAddr(image);
  if( (int64_t)handle->mapped_address == -1){
     ERR("cannot map vme address");
     return 0;
 } 
 
  DPRINT("Master address mapped at @0x%x size 0x%x\n",handle->mapped_address,size);
  return 1;
}

static int  map_slave_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing,uint32_t vme_opt){
   VMEBridge* vme=(VMEBridge*)handle->bus;
   int am=0;
  int image;
  if(addressing==32)
    am = A32;
  else if(addressing==16){
    am = A16;
  } else if(addressing==24){
    am = A24;
  } else {
    ERR("slave addressing not implemented %d\n",addressing);
    return 0;
  }

  image=vme->getImage(add,size,am,D32,SLAVE);
  
  if (image < 0) {
      ERR("Can't allocate slave image vmeadd:0x%x, size:0x%x\n",add,size);
       return 0;
    }
  handle->fd=image;
  handle->mapped_address=  vme->getPciBaseAddr(image);
 if( (int64_t)handle->mapped_address == -1){
     ERR("cannot map vme address");
     return 0;
 } 
 
  DPRINT("slave address mapped at @0x%x size 0x%x\n",handle->mapped_address,size);
  return 1;
}
static int vme_write8_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  return vme->wb(handle->fd,(uint32_t)handle->phys_add + off,data);
}

static int vme_write32_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  return vme->wl(handle->fd,(uint32_t)handle->phys_add + off,data);
}
static int vme_write16_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  return vme->ww(handle->fd,(uint32_t)handle->phys_add + off,data);
}
static int vme_read32_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t *data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  return vme->rl(handle->fd,(uint32_t)handle->phys_add + off,data);
}
static int vme_read16_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t *data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  return vme->rw(handle->fd,(uint32_t)handle->phys_add + off,data);
}
static int vme_read8_universe2(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t *data){
  VMEBridge* vme=(VMEBridge*)handle->bus;
  return vme->rb(handle->fd,(uint32_t)handle->phys_add + off,data);
}


#endif


