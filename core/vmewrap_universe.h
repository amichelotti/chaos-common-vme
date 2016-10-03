/*
 *
@file vmewrap_universe.h
@author Andrea Michelotti
@date Mar 11, 2016
 * */

#ifndef _VMEWRAPUNIVERSE_H_

#define _VMEWRAPUNIVERSE_H_
#include <vmedrv/universe/vme.h>
#include <vmedrv/universe/vme_api.h>
#define VME_MAP_MASTER(handle,ARGS...) map_master_universe(handle, ## ARGS)
#define VME_MAP_SLAVE(handle,ARGS...) map_slave_universe(handle, ## ARGS)
#define VME_INIT(handle) vme_init_universe(handle)
#define VME_CLOSE(handle) vme_close_universe(handle)




static int vme_close_universe(vmewrap_int_vme_handle_t handle){
    vme_bus_handle_t bus_handle=( vme_bus_handle_t)handle->bus;
    if(handle->master_handle){
      if (vme_master_window_unmap(bus_handle,(vme_master_handle_t) handle->master_handle)) {
	
		ERR("Error unmapping the window\n");
		vme_master_window_release(bus_handle, (vme_master_handle_t) handle->master_handle);
      }
      
        
     /* if (vme_master_window_release(handle->bus, handle->master_handle)) {
			ERR("Error releasing the window\n");
      }*/
      handle->master_handle = 0;
    }
    
    if(handle->slave_handle){
      if (vme_slave_window_unmap(bus_handle,(vme_slave_handle_t) handle->slave_handle)) {
	
	ERR("Error unmapping the window\n");
	vme_slave_window_release(bus_handle, (vme_slave_handle_t)handle->slave_handle);

      }
      
        
      if (vme_slave_window_release(bus_handle,(vme_slave_handle_t) handle->slave_handle)) {
	ERR("Error releasing the window\n");
      }
      handle->slave_handle = 0;
    }
    
    if (vme_term(bus_handle)) {
      ERR("Error terminating VME\n");
      return -3;
    }
 
    DPRINT("vme sucessfully closed\n");
    return 0;
}
static int vme_init_universe(vmewrap_int_vme_handle_t handle){
    vme_bus_handle_t bus_handle;
    int ret= vme_init(&bus_handle);
    handle->bus = (void*)bus_handle;
    return ret;
    
}
static int  map_master_universe(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing){
  int am,flags;
  vme_bus_handle_t bus_handle;
  void* mapped_address;
  bus_handle = (vme_bus_handle_t)handle->bus;

  if(addressing==32)
    am = VME_A32SD;
  else if(addressing==16){
    am = VME_A16S;
  } else {
    ERR("master addressing not implemented %d\n",addressing);
    return 0;
  }


  flags = VME_CTL_PWEN;
  if (vme_master_window_create(bus_handle, (vme_master_handle_t*)&handle->master_handle, add, am,size,
			       flags, NULL)) {
    ERR("cannot create master window bus_handle 0x%x add= 0x%x size = 0x%x am=%d \n",bus_handle,add,size,am);
    perror("vme_master_window_create");
    vme_term(bus_handle);
    handle->bus = 0;
    return 0;
  }
  
  mapped_address =  vme_master_window_map(bus_handle,(vme_master_handle_t)handle->master_handle, 0);
  if (0 == mapped_address) {
    ERR("cannot map VME master window\n");
    perror("vme_master_window_map");
    vme_master_window_release(bus_handle, (vme_master_handle_t)handle->master_handle);

    vme_term(bus_handle);
    handle->bus = 0;
    handle->master_handle = 0;
    return 0;
  }
  handle->master_mapped_address= mapped_address;
  DPRINT("Master address mapped at @0x%x size %d\n",mapped_address,size);
  return 1;
}

static int  map_slave_universe(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing){
  int am,flags;
  vme_bus_handle_t bus_handle;
  void* mapped_address;


  bus_handle = (vme_bus_handle_t)handle->bus;

  if(addressing==32)
    am = VME_A32;
  else if(addressing==16){
    am = VME_A16;
  } else if(addressing==24){
    am = VME_A24;
  }else {
    ERR("slave addressing not implemented %d\n",addressing);
    return 0;
  }
  

  flags =  VME_CTL_PWEN | VME_CTL_PREN;
  if (vme_slave_window_create(bus_handle, (vme_slave_handle_t*)&handle->slave_handle, add, am,size,
			       flags, NULL)) {
    ERR("cannot create slave window\n");
    perror("vme_slave_window_create");
    vme_term(bus_handle);
    handle->bus = 0;
    return 0;
  }
  
  mapped_address =  vme_slave_window_map(bus_handle,  (vme_slave_handle_t)handle->slave_handle, 0);
  if (0 == mapped_address) {
    ERR("cannot map VME master window\n");
    perror("vme_master_window_map");
    vme_slave_window_release(bus_handle, (vme_slave_handle_t)handle->slave_handle);

    vme_term(bus_handle);
    handle->bus = 0;
    handle->slave_handle = 0;
    return 0;
  }
  handle->slave_mapped_address= mapped_address;
  DPRINT("Slave address mapped at @0x%x size %d\n",mapped_address,size);
  return 1;
}

#endif


