#include "ddlib.h"
#include <sys/time.h>
#include <vme/vme.h>
#include <vme/vme_api.h>
#include <stdlib.h>
typedef struct __vme_handle__ {
  vme_bus_handle_t bus;
  vme_master_handle_t master_handle;
  vme_slave_handle_t slave_handle;
  uint32_t master_mapped_address;
  uint32_t slave_mapped_address;
} * dd_int_vme_handle_t;


unsigned long long getUsTime(){
 struct timeval tv;
 unsigned long long ret;
 static unsigned long long first_call=0;
 gettimeofday(&tv,NULL);
 ret = ((unsigned long long)tv.tv_sec) * 1000000LL;
 ret+= tv.tv_usec;
 if(first_call==0){
 	first_call = ret;
}
 return ret-first_call;
}


static int  map_master(dd_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing){
  int am,flags;
  vme_bus_handle_t bus_handle;
  uint32_t mapped_address;
  bus_handle = handle->bus;

  if(addressing==32)
    am = VME_A32SD;
  else if(addressing==16){
    am = VME_A16S;
  } else {
    ERR("master addressing not implemented %d\n",addressing);
    return 0;
  }


  flags = VME_CTL_PWEN;
  if (vme_master_window_create(bus_handle, &handle->master_handle, add, am,size,
			       flags, NULL)) {
    ERR("cannot create master window bus_handle 0x%x add= 0x%x size = 0x%x am=%d \n",bus_handle,add,size,am);
    perror("vme_master_window_create");
    vme_term(bus_handle);
    handle->bus = 0;
    return 0;
  }
  
  mapped_address =  (uint32_t)vme_master_window_map(bus_handle,handle->master_handle, 0);
  if (0 == mapped_address) {
    ERR("cannot map VME master window\n");
    perror("vme_master_window_map");
    vme_master_window_release(bus_handle, handle->master_handle);

    vme_term(bus_handle);
    handle->bus = 0;
    handle->master_handle = 0;
    return 0;
  }
  handle->master_mapped_address= mapped_address;
  DPRINT("Master address mapped at @0x%x size %d\n",mapped_address,size);
  return 1;
}

static int  map_slave(dd_int_vme_handle_t handle,uint32_t add,uint32_t size,uint32_t addressing){
  int am,flags;
  vme_bus_handle_t bus_handle;
  uint32_t mapped_address;


  bus_handle = handle->bus;

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
  if (vme_slave_window_create(bus_handle, &handle->slave_handle, add, am,size,
			       flags, NULL)) {
    ERR("cannot create slave window\n");
    perror("vme_slave_window_create");
    vme_term(bus_handle);
    handle->bus = 0;
    return 0;
  }
  
  mapped_address =  (uint32_t)vme_slave_window_map(bus_handle, handle->slave_handle, 0);
  if (0 == mapped_address) {
    ERR("cannot map VME master window\n");
    perror("vme_master_window_map");
    vme_slave_window_release(bus_handle, handle->slave_handle);

    vme_term(bus_handle);
    handle->bus = 0;
    handle->slave_handle = 0;
    return 0;
  }
  handle->slave_mapped_address= mapped_address;
  DPRINT("Slave address mapped at @0x%x size %d\n",mapped_address,size);
  return 1;
}

dd_vme_handle_t dd_vme_open(uint32_t master_add,uint32_t master_size,uint32_t master_addressing,uint32_t slave_add,uint32_t slave_size,uint32_t slave_addressing){

  vme_bus_handle_t bus_handle;
  dd_int_vme_handle_t p;
  if(master_size==0 && slave_size==0){
    return 0;
  }
  if (vme_init(&bus_handle)) {
    ERR("cannot initialize vme\n");
    perror("vme_init");

    return 0;
  }
  p = (dd_int_vme_handle_t)malloc(sizeof(dd_int_vme_handle_t));
  if(p==NULL){
    ERR("cannot allocate resources\n");
    vme_term(bus_handle);
    return 0;
  }
  DPRINT("vme init handle 0x%x\n",bus_handle);
  p->bus = bus_handle;

  if(master_size){
    if(map_master(p,master_add,master_size,master_addressing)== 0){
      free(p);
     
      return 0;
    }
  }
  if(slave_size){
    if(map_slave(p,slave_add,slave_size,slave_addressing)== 0){
      free(p);
      return 0;
    }
  }

  
  return (dd_vme_handle_t) p; 

}



int32_t dd_vme_close(dd_vme_handle_t  h){
  dd_int_vme_handle_t handle = h;
  DPRINT("closing handle @0x%x\n",(unsigned)h);
  if(handle){
    if(handle->master_handle){
      if (vme_master_window_unmap(handle->bus, handle->master_handle)) {
	
		ERR("Error unmapping the window\n");
		vme_master_window_release(handle->bus, handle->master_handle);
      }
      
        
     /* if (vme_master_window_release(handle->bus, handle->master_handle)) {
			ERR("Error releasing the window\n");
      }*/
      handle->master_handle = 0;
    }
    
    if(handle->slave_handle){
      if (vme_slave_window_unmap(handle->bus, handle->slave_handle)) {
	
	ERR("Error unmapping the window\n");
	vme_slave_window_release(handle->bus, handle->slave_handle);

      }
      
        
      if (vme_slave_window_release(handle->bus, handle->slave_handle)) {
	ERR("Error releasing the window\n");
      }
      handle->slave_handle = 0;
    }
    
    if (vme_term(handle->bus)) {
      ERR("Error terminating VME\n");
      return -3;
    }
 
    DPRINT("vme sucessfully closed\n");
    free(handle);
    handle->bus = 0;
    return 0;
  }
  return -4;
}


uint32_t dd_get_vme_master_linux_add(dd_vme_handle_t  h){
  dd_int_vme_handle_t handle = h;
  if(handle){
    return handle->master_mapped_address;
  }
  return 0;
}

/**
   
	@param handle
	@return the slave mapped address
*/
uint32_t dd_get_vme_slave_linux_add(dd_vme_handle_t  h){
  dd_int_vme_handle_t handle = h;
  if(handle){
    return handle->slave_mapped_address;
  }
  return 0;
  
}

