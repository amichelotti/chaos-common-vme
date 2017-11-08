#include <stdlib.h>
#include <common/debug/core/debug.h>
#include "vmewrap.h"
#include "vmewrap_int.h"

#include "vmewrap_universe2.h"
#include "vmewrap_caenvme.h"
#include "vmewrap_uservme.h"


extern "C" {

vmewrap_vme_handle_t vmewrap_init_driver(vme_driver_t driver){
	vmewrap_int_vme_handle_t p;
	p = (vmewrap_int_vme_handle_t)calloc(1,sizeof(struct __vme_handle__ ));
	if(p==NULL){
		ERR("cannot allocate resources");
		return 0;
	}
	p->type = driver;
	switch(driver){
	case VME_UNIVERSE2_DRIVER:
		//case VME_UNIVERSE_DRIVER:

		DPRINT("initializing UNIVERSE2 driver");
		if(vme_init_driver_universe2(p)!=0){
			ERR("cannot initialize universe2");
			delete p;
			return 0;
		}
		break;
	case VME_CAEN1718_DRIVER:
	case VME_CAEN2718_DRIVER:
		if(vme_init_driver_caenvme(p)!=0){
			ERR("cannot initialize caenvme");
			delete p;
			return 0;
		}
		break;
	case VME_LINUX_USER:
		if(vme_init_driver_uservme(p)!=0){
			ERR("cannot initialize uservme");
			delete p;
			return 0;
		}
		break;
	default:
		ERR("uknown driver");
		delete p;
		return 0;
	}

	return p;

}

int vmewrap_deinit_driver(vmewrap_vme_handle_t driver){
	vmewrap_int_vme_handle_t p;
	p = (vmewrap_int_vme_handle_t)driver;
	int err=0;
	if(p==NULL){
		ERR("bad handle");
		return -1;
	}

	switch(p->type){
	case VME_UNIVERSE2_DRIVER:
	case VME_UNIVERSE_DRIVER:

		DPRINT("deinitializing UNIVERSE driver");
		if(vme_deinit_driver_universe2(p)!=0){
			ERR("cannot deinitialize universe2");

			err= -1;
		}
		break;
	case VME_CAEN1718_DRIVER:
	case VME_CAEN2718_DRIVER:
		if(vme_deinit_driver_caenvme(p)!=0){
			ERR("cannot deinitialize caenvme");
			delete p;
			return 0;
		}
		break;
	default:
		err=-2;
		ERR("undefined driver type");
	}

	delete p;

	return err;

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

	DPRINT("vme init vme handle %p, vme wrap handle %p",handle->bus,h);

	handle->master=1;
	handle->phys_add=(uint64_t)master_add;
	handle->size = master_size;
	if(handle->map_master(handle,master_add,master_size,master_addressing,dw,vme_opts)< 0){
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

	DPRINT("vme init vme handle %p, vme wrap handle %p",handle->bus,h);

	handle->master=0;
	handle->phys_add=(uint64_t)slave_add;
	handle->size = slave_size;
	if(handle->map_slave(handle,slave_add,slave_size,slave_addressing,dw,vme_opts)< 0){
		ERR("failed mapping as slave");
		return -3;
	}


	return 0;
}



int vmewrap_vme_close(vmewrap_vme_handle_t  h){
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
    DPRINT("closing handle 0x%p",h);
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
		DPRINT("returning mapped linux address %p, if zero driver unable to map",handle->mapped_address);
		return handle->mapped_address;
	}
	return 0;
}


int vmewrap_write32(vmewrap_vme_handle_t  h,unsigned off,uint32_t* data,int sizen){
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	return handle->vme_write32((vmewrap_int_vme_handle_t)handle,off,data,sizen);

}
int vmewrap_write16(vmewrap_vme_handle_t  h,unsigned off,uint16_t* data,int sizen){
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	return handle->vme_write16((vmewrap_int_vme_handle_t)handle,off,data,sizen);

}

int vmewrap_write8(vmewrap_vme_handle_t  h,unsigned off,uint8_t* data,int sizen){
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	return handle->vme_write8((vmewrap_int_vme_handle_t)handle,off,data,sizen);

}
int vmewrap_read32(vmewrap_vme_handle_t  h,unsigned off,uint32_t* data,int sizen){
	int ret;
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	ret =handle->vme_read32((vmewrap_int_vme_handle_t)handle,off,data,sizen);
	DPRINT("read32 off 0x%x data=0x%x, ret=%d",off,*data,ret);

	return ret;
}
int vmewrap_read16(vmewrap_vme_handle_t  h,unsigned off,uint16_t* data,int sizen){
	int ret;
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	ret =handle->vme_read16((vmewrap_int_vme_handle_t)handle,off,data,sizen);
	DPRINT("read16 off 0x%x data=0x%x, ret=%d",off,*data,ret);
	return ret;
}
int vmewrap_read8(vmewrap_vme_handle_t  h,unsigned off,uint8_t* data,int sizen){
	int ret;
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	ret = handle->vme_read8((vmewrap_int_vme_handle_t)handle,off,data,sizen);
	DPRINT("read8 off 0x%x data=0x%x, ret=%d",off,*data,ret);
	return ret;
}


int vmewrap_interrupt_enable(vmewrap_vme_handle_t  h,int level, int signature){
	int ret;
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	if(handle == NULL){
		DERR("not implemented")
					return -1;
	}
	ret = handle->vme_interrupt_enable((vmewrap_int_vme_handle_t)handle,level,signature);
	return ret;
}

/**
 * interrupt disable
   		@return 0 on success
 */

int vmewrap_interrupt_disable(vmewrap_vme_handle_t  h){
	int ret;
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	if(handle == NULL){
		DERR("not implemented")
				return -1;
	}
	ret = handle->vme_interrupt_disable((vmewrap_int_vme_handle_t)handle);
	DPRINT("interrupt disable, ret=%d",ret);
	return ret;
}
/**
 * wait for interrupt
 * @param timeo_ms timeout in ms (0 indefinite wait)
 * @return 0 on success
 */
int vmewrap_wait_interrupt(vmewrap_vme_handle_t  h,int timeo_ms){
	int ret;
	vmewrap_int_vme_handle_t handle = (vmewrap_int_vme_handle_t)h;
	if(handle == NULL){
		DERR("not implemented")
					return -1;
	}
	ret = handle->vme_wait_interrupt((vmewrap_int_vme_handle_t)handle,timeo_ms);

	return ret;
}
uint32_t vmewrap_read_reg32(vmewrap_vme_handle_t  handle,unsigned off){
	uint32_t ret;
	vmewrap_read32(handle,off,&ret,1);
	return ret;
}
uint16_t vmewrap_read_reg16(vmewrap_vme_handle_t  handle,unsigned off){
	uint16_t ret;
	vmewrap_read16(handle,off,&ret,1);
	return ret;
}
uint8_t vmewrap_read_reg8(vmewrap_vme_handle_t  handle,unsigned off){
	uint8_t ret;
	vmewrap_read8(handle,off,&ret,1);
	return ret;
}

int vmewrap_write_reg32(vmewrap_vme_handle_t  handle,uint32_t data,unsigned off){
	DPRINT("write32 off 0x%x data=0x%x",off,data);

	return vmewrap_write32(handle, off,&data,1);
}
int vmewrap_write_reg16(vmewrap_vme_handle_t  handle,uint16_t data,unsigned off){
	DPRINT("write16 off 0x%x data=0x%x",off,data);

	return vmewrap_write16(handle, off,&data,1);

}
int vmewrap_write_reg8(vmewrap_vme_handle_t  handle,uint8_t data,unsigned off){
	DPRINT("write8 off 0x%x data=0x%x",off,data);

	return vmewrap_write8(handle, off,&data,1);


}
}
