#ifndef _VMEWRAPUNIVERSE2_H_

#define _VMEWRAPUNIVERSE2_H_
#include <vmedrv/universe2/vmelib.h>
#define VME_MAP_MASTER(handle,ARGS...) map_master_universe2(handle, ## ARGS)
#define VME_MAP_SLAVE(handle,ARGS...) map_slave_universe2(handle, ## ARGS)
#define VME_INIT(handle) vme_init_universe2(handle)
#define VME_CLOSE(handle) vme_close_universe2(handle)



static int vme_close_universe2(vmewrap_int_vme_handle_t handle){
    if(handle && handle->bus){
        VMEBridge* vme=(VMEBridge*)handle->bus;
        delete vme;
    }
    return 0;
}
static int vme_init_universe2(vmewrap_int_vme_handle_t handle){
    VMEBridge* vme=new  VMEBridge();
    handle->bus = ( VMEBridge*)vme;
    if(vme)
        return 0;
    return -1;
    
}
static int  map_master_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing){
  int am;
  VMEBridge* vme=(VMEBridge*)handle->bus;
  int image;
  if(addressing==32)
    am = A32;
  else if(addressing==16){
    am = A16;
  } else {
    ERR("master addressing not implemented %d\n",addressing);
    return 0;
  }

  image=vme->getImage(add,size,am,D32,MASTER);
  
  if (image < 0) {
      ERR("Can't allocate master image vmeadd:0x%x, size:0x%x\n",add,size);
       return 0;
    }
 handle->master_mapped_address=  vme->getPciBaseAddr(image);
 if( (int64_t)handle->master_mapped_address == -1){
     ERR("cannot map vme address");
     return 0;
 } 
 
  DPRINT("Master address mapped at @0x%x size 0x%x\n",handle->master_mapped_address,size);
  return 1;
}

static int  map_slave_universe2(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing){
   VMEBridge* vme=(VMEBridge*)handle->bus;
   int am=0;
  int image;
  if(addressing==32)
    am = A32;
  else if(addressing==16){
    am = A16;
  } else {
    ERR("slave addressing not implemented %d\n",addressing);
    return 0;
  }

  image=vme->getImage(add,size,am,D32,SLAVE);
  
  if (image < 0) {
      ERR("Can't allocate slave image vmeadd:0x%x, size:0x%x\n",add,size);
       return 0;
    }
 handle->slave_mapped_address=  vme->getPciBaseAddr(image);
 if( (int64_t)handle->slave_mapped_address == -1){
     ERR("cannot map vme address");
     return 0;
 } 
 
  DPRINT("slave address mapped at @0x%x size 0x%x\n",handle->slave_mapped_addres,size);
  return 1;
}

#endif


