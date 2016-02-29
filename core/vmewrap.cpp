#include <stdlib.h>
#include <common/debug/core/debug.h>
#include "vmewrap.h"

typedef struct __vme_handle__ {

  void* bus;
  void* handle;
  void* mapped_address;
  void* phys_add;
  uint32_t size;
  int fd;
  int master;

} * vmewrap_int_vme_handle_t;


#ifdef UNIVERSE2
#include "vmewrap_universe2.h"
#else
#include "vmewrap_universe.h"
#endif



extern "C" {
    
  vmewrap_vme_handle_t vmewrap_vme_open_master(uint32_t master_add,uint32_t master_size,vme_addressing master_addressing,vme_access dw,vme_opt vme_opts){

 
  vmewrap_int_vme_handle_t p;
  if(master_size==0){
    return 0;
  }

  p = (vmewrap_int_vme_handle_t)calloc(1,sizeof(struct __vme_handle__ ));
  if(p==NULL){
    ERR("cannot allocate resources");
    return 0;
  }
  
  if (VME_INT_INIT(p)) {
    ERR("cannot initialize vme");
    perror("vme_init");

    return 0;
  }
 
  DPRINT("vme init vme handle 0x%x, vme wrap handle 0x%x",p->bus,p);

  p->master=1;
  p->phys_add=(void*)master_add;
  p->size = master_size;
  if(VME_INT_MAP_MASTER(p,master_add,master_size,master_addressing,dw,vme_opts)== 0){
    free(p);
    
    return 0;
  }
  

  return (vmewrap_vme_handle_t) p; 
}

  vmewrap_vme_handle_t vmewrap_vme_open_slave(uint32_t slave_add,uint32_t slave_size,vme_addressing slave_addressing,vme_access dw,vme_opt vme_opts){

 
  vmewrap_int_vme_handle_t p;
  if(slave_size==0){
    return 0;
  }
  p = (vmewrap_int_vme_handle_t)calloc(1,sizeof(struct __vme_handle__ ));
  if(p==NULL){
    ERR("cannot allocate resources");
    return 0;
  }
  
  if (VME_INT_INIT(p)) {
    ERR("cannot initialize vme");
    perror("vme_init");

    return 0;
  }
 
  DPRINT("vme init vme handle 0x%x, vme wrap handle 0x%x",p->bus,p);

  p->phys_add=(void*)slave_add;
  p->size = slave_size;
  if(VME_INT_MAP_SLAVE(p,slave_add,slave_size,slave_addressing,dw,vme_opts)== 0){
      free(p);
      return 0;
  }
  

  return (vmewrap_vme_handle_t) p; 
}



int32_t vmewrap_vme_close(vmewrap_vme_handle_t  h){
  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
  DPRINT("closing handle @0x%x",(unsigned)h);
  if(handle){
    VME_INT_CLOSE(handle);
    free(handle);
    return 0;
  }
  return -4;
}


void* vmewrap_get_linux_add(vmewrap_vme_handle_t  h){
  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
  if(handle){
    return handle->mapped_address;
  }
  return 0;
}


  int vmewrap_write32(vmewrap_vme_handle_t  handle,unsigned off,uint32_t data){

    return VME_INT_WRITE32((vmewrap_int_vme_handle_t)handle,off,data);

  }
  int vmewrap_write16(vmewrap_vme_handle_t  handle,unsigned off,uint16_t data){
    return VME_INT_WRITE16((vmewrap_int_vme_handle_t)handle,off,data);

  }  int vmewrap_write8(vmewrap_vme_handle_t  handle,unsigned off,uint8_t data){
    return VME_INT_WRITE8((vmewrap_int_vme_handle_t)handle,off,data);

  }
  int vmewrap_read32(vmewrap_vme_handle_t  handle,unsigned off,uint32_t* data){
    return VME_INT_READ32((vmewrap_int_vme_handle_t)handle,off,data);
  }
  int vmewrap_read16(vmewrap_vme_handle_t  handle,unsigned off,uint16_t* data){
    return VME_INT_READ16((vmewrap_int_vme_handle_t)handle,off,data);
  }
  int vmewrap_read8(vmewrap_vme_handle_t  handle,unsigned off,uint8_t* data){
    return VME_INT_READ8((vmewrap_int_vme_handle_t)handle,off,data);
  }

}
