#include <stdlib.h>
#include <common/debug/core/debug.h>
#include "vmewrap.h"

typedef struct __vme_handle__ {

  void* bus;
  void* master_handle;
  void* slave_handle;
  void* master_mapped_address;
  void* slave_mapped_address;
} * vmewrap_int_vme_handle_t;


#ifdef UNIVERSE2
#include "vmewrap_universe2.h"
#else
#include "vmewrap_universe.h"
#endif



extern "C" {
    
vmewrap_vme_handle_t vmewrap_vme_open(uint32_t master_add,uint32_t master_size,uint32_t master_addressing,uint32_t slave_add,uint32_t slave_size,uint32_t slave_addressing){

 
  vmewrap_int_vme_handle_t p;
  if(master_size==0 && slave_size==0){
    return 0;
  }
  p = (vmewrap_int_vme_handle_t)malloc(sizeof(vmewrap_int_vme_handle_t));
  if(p==NULL){
    ERR("cannot allocate resources\n");
    return 0;
  }
  
  if (VME_INIT(p)) {
    ERR("cannot initialize vme\n");
    perror("vme_init");

    return 0;
  }
 
  DPRINT("vme init handle 0x%x\n",bus_handle);

  if(master_size){
    if(VME_MAP_MASTER(p,master_add,master_size,master_addressing)== 0){
      free(p);
     
      return 0;
    }
  }
  if(slave_size){
    if(VME_MAP_SLAVE(p,slave_add,slave_size,slave_addressing)== 0){
      free(p);
      return 0;
    }
  }

  return (vmewrap_vme_handle_t) p; 
}



int32_t vmewrap_vme_close(vmewrap_vme_handle_t  h){
  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
  DPRINT("closing handle @0x%x\n",(unsigned)h);
  if(handle){
    VME_CLOSE(handle);
    free(handle);
    handle->bus = 0;
    return 0;
  }
  return -4;
}


void* vmewrap_get_vme_master_linux_add(vmewrap_vme_handle_t  h){
  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
  if(handle){
    return handle->master_mapped_address;
  }
  return 0;
}

/**
   
	@param handle
	@return the slave mapped address
*/
void* vmewrap_get_vme_slave_linux_add(vmewrap_vme_handle_t  h){
  vmewrap_int_vme_handle_t handle =  (vmewrap_int_vme_handle_t)h;
  if(handle){
    return handle->slave_mapped_address;
  }
  return 0;
  
}

}