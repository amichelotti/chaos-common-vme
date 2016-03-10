#include <stdlib.h>
#include <common/debug/core/debug.h>
#include "vmewrap.h"
#include "vmewrap_int.h"

#include "vmewrap_universe2.h"



extern "C" {
    
vmewrap_vme_handle_t vmewrap_init_driver(vme_driver_t driver){
	vmewrap_int_vme_handle_t p;
	p = (vmewrap_int_vme_handle_t)calloc(1,sizeof(struct __vme_handle__ ));
	if(p==NULL){
		    ERR("cannot allocate resources");
		    return 0;
	}
	switch(driver){
	case VME_UNIVERSE2_DRIVER:
	case VME_UNIVERSE_DRIVER:
		if(vme_init_driver_universe2(p)!=0){
		    ERR("cannot initialize universe2");
			delete p;
			return 0;
		}
		break;
	default:
		delete p;
		return 0;
	}

	return p;

}

  int vmewrap_vme_open_master(vmewrap_vme_handle_t  h,uint32_t master_add,uint32_t master_size,vme_addressing master_addressing,vme_access dw,vme_opt vme_opts){
  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
 
  if(master_size==0){
    return -1;
  }

  if (  handle->vme_init(handle)) {
    ERR("cannot initialize vme");
    perror("vme_init");
    return -2;
  }
 
  DPRINT("vme init vme handle 0x%x, vme wrap handle 0x%x",handle->bus,h);

  handle->master=1;
  handle->phys_add=(void*)master_add;
  handle->size = master_size;
  if(handle->map_master(handle,master_add,master_size,master_addressing,dw,vme_opts)== 0){
    free(handle);
    ERR("failed mapping as master");
    return -3;
  }
  

  return 0;
}

  int vmewrap_vme_open_slave(vmewrap_vme_handle_t  h,uint32_t slave_add,uint32_t slave_size,vme_addressing slave_addressing,vme_access dw,vme_opt vme_opts){

 
 vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
  if(slave_size==0){
    return 0;
  }

  if (  handle->vme_init(handle)) {
    ERR("cannot initialize vme");
    perror("vme_init");
    return -2;
  }
 
  DPRINT("vme init vme handle 0x%x, vme wrap handle 0x%x",handle->bus,h);

  handle->master=0;
  handle->phys_add=(void*)slave_add;
  handle->size = slave_size;
  if(handle->map_slave(handle,slave_add,slave_size,slave_addressing,dw,vme_opts)== 0){
    free(handle);
    ERR("failed mapping as slave");
    return -3;
  }
  

  return 0;
}



int vmewrap_vme_close(vmewrap_vme_handle_t  h){
  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
  DPRINT("closing handle @0x%x",(unsigned)h);
  if(handle){
    handle->vme_close(handle);
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


  int vmewrap_write32(vmewrap_vme_handle_t  h,unsigned off,uint32_t data){
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    return handle->vme_write32((vmewrap_int_vme_handle_t)handle,off,data);

  }
  int vmewrap_write16(vmewrap_vme_handle_t  h,unsigned off,uint16_t data){
	  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    return handle->vme_write16((vmewrap_int_vme_handle_t)handle,off,data);

  }  int vmewrap_write8(vmewrap_vme_handle_t  h,unsigned off,uint8_t data){
	  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    return handle->vme_write8((vmewrap_int_vme_handle_t)handle,off,data);

  }
  int vmewrap_read32(vmewrap_vme_handle_t  h,unsigned off,uint32_t* data){
	  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    return handle->vme_read32((vmewrap_int_vme_handle_t)handle,off,data);
  }
  int vmewrap_read16(vmewrap_vme_handle_t  h,unsigned off,uint16_t* data){
	  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    return handle->vme_read16((vmewrap_int_vme_handle_t)handle,off,data);
  }
  int vmewrap_read8(vmewrap_vme_handle_t  h,unsigned off,uint8_t* data){
	  vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    return handle->vme_read8((vmewrap_int_vme_handle_t)handle,off,data);
  }

}
