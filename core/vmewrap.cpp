#include <stdlib.h>
#include <common/debug/core/debug.h>
#include "vmewrap.h"
#include "vmewrap_universe2.h"
#include "vmewrap_caenvme.h"
#include "vmewrap_uservme.h"
#include "vmewrap_sis3153_eth.h"

#define CHECK_DRIVER_NAME(dname,x) \
if(dname==x)return (x ##_DRIVER)

#define CHECK_DRIVER_ID(dname,x) \
if(dname==(x ##_DRIVER))return std::string(x);

vme_driver_t driverNameToId(const std::string&name){
	CHECK_DRIVER_NAME(name,VME_UNIVERSE2);
	CHECK_DRIVER_NAME(name,VME_UNIVERSE);
	CHECK_DRIVER_NAME(name,VME_CAEN1718);
	CHECK_DRIVER_NAME(name,VME_CAEN2718);
	CHECK_DRIVER_NAME(name,VME_LINUX_USER);
	CHECK_DRIVER_NAME(name,VME_SIS3153_ETH);
	return VME_UKNOWN_DRIVER;
}
std::string idToDriverName(vme_driver_t name){
	CHECK_DRIVER_ID(name,VME_UNIVERSE2);
	CHECK_DRIVER_ID(name,VME_UNIVERSE);
	CHECK_DRIVER_ID(name,VME_CAEN1718);
	CHECK_DRIVER_ID(name,VME_CAEN2718);
	CHECK_DRIVER_ID(name,VME_LINUX_USER);
	CHECK_DRIVER_ID(name,VME_SIS3153_ETH);
	return std::string(VME_UKNOWN);
}

vmewrap_vme_handle_t vmewrap_init_driver(const char* driver,void *params){
	return vmewrap_init_driver(driverNameToId(driver),params);

}
vmewrap_vme_handle_t vmewrap_init_driver(vme_driver_t driver,void *params){
	vmewrap_vme_handle_t p;
	p = new __vmewrap_handle__ ();
	if(p==NULL){
		ERR("cannot allocate resources");
		return 0;
	}
	p->type = idToDriverName(driver);
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
	case VME_LINUX_USER_DRIVER:
		if(vme_init_driver_uservme(p)!=0){
			ERR("cannot initialize uservme");
			delete p;
			return 0;
		}
		break;
	case VME_SIS3153_ETH_DRIVER:
		if(vme_init_driver_sis3153_eth(p,params)!=0){
			ERR("cannot initialize sis3153_eth");
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

int vmewrap_deinit_driver(vmewrap_vme_handle_t p){

	int err=0;
	if(p==NULL){
		ERR("bad handle");
		return -1;
	}

	switch(driverNameToId(p->type.c_str())){
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
			return -1;
		}

		break;
	case VME_SIS3153_ETH_DRIVER:
		if(vme_deinit_driver_sis3153_eth(p)!=0){
			ERR("cannot deinitialize sis3153");
			delete p;
			return -1;
		}
		break;
	case VME_LINUX_USER_DRIVER:
		if(vme_deinit_driver_uservme(p)!=0){
			ERR("cannot deinitialize uservme");
			delete p;
			return -1;
		}
		break;
	default:
		err=-2;
		ERR("undefined driver type");
	}

	delete p;

	return err;

}
int vmewrap_getFD(vmewrap_window_t  h){
	if(h==NULL){
		return -1;
	}
	return h->fd;

}
int vmewrap_setFD(vmewrap_window_t  h,int fd){
	if(h==NULL){
		return -1;
	}
	h->fd=fd;
	return h->fd;

}
vmewrap_window_t vmewrap_vme_open_master(vmewrap_vme_handle_t  handle,uint32_t master_add,uint32_t master_size,vme_addressing master_addressing,vme_access dw,vme_opt vme_opts){
	vmewrap_window_t ret;
	if(master_size==0){
		ERR("size must be greater than 0");
		return 0;
	}

	if (  handle->vme_init(handle)) {
		ERR("cannot initialize vme");
		perror("vme_init");
		return 0;
	}

	if(handle->nwindow>=VMEMAXWIN){
		ERR("too many open vme windows %d",handle->nwindow);
		return 0;
	}
		ret=new __vme_window__();
		ret->parent=handle;
		ret->add=master_add;
		ret->addressing=master_addressing;
		ret->master=1;
		ret->access=dw;
		ret->size=master_size;
		ret->master=1;
		ret->opt=vme_opts;
		ret->index=handle->nwindow;
		ret->fd=ret->index; // initialize to index... can be changed
		handle->window[ret->index]=ret;
	DPRINT("Creating new master window id:%d (0x%p) vme addr:0x%x-0x%x vme bus %p, vme wrap handle %p",ret->index,ret,master_add,master_add+master_size,handle->bus,handle);

	if((handle->map_master(handle,master_add,master_size,master_addressing,dw,vme_opts))<0){
		ERR("failed mapping as master");
		delete(ret);
		handle->window[ret->index]=0;
		return 0;
	}
	handle->nwindow++;
	return ret;
}

vmewrap_window_t vmewrap_vme_open_slave(vmewrap_vme_handle_t  handle,uint32_t slave_add,uint32_t slave_size,vme_addressing slave_addressing,vme_access dw,vme_opt vme_opts){
	vmewrap_window_t ret;
	if(slave_size==0){
		return 0;
	}

	if (  handle->vme_init(handle)) {
		ERR("cannot initialize vme");
		perror("vme_init");
		return 0;
	}

	DPRINT("vme init vme handle %p, vme wrap handle %p",handle->bus,handle);
	if(handle->nwindow>=VMEMAXWIN){
		ERR("too many open vme windows %d",handle->nwindow);
		return 0;
	}
	ret=new __vme_window__();
	ret->parent=handle;
	ret->add=slave_add;
	ret->addressing=slave_addressing;
	ret->access=dw;
	ret->size=slave_size;
	ret->master=0;
	ret->opt=vme_opts;
	ret->index=handle->nwindow;
	ret->fd=ret->index; // initialize to index... can be changed
	handle->window[ret->index]=ret;
	DPRINT("Creating new slave window id:%d (0x%p) vme addr:0x%x-0x%x vme bus %p, vme wrap handle %p",ret->index,ret,slave_add,slave_add+slave_size,handle->bus,handle);

	if(handle->map_slave(handle,slave_add,slave_size,slave_addressing,dw,vme_opts)< 0){
		ERR("failed mapping as slave");
		delete (ret);
		handle->window[ret->index]=0;
		return 0;
	}
	
	handle->nwindow++;
	return ret;
}

int vmewrap_vme_close(vmewrap_window_t  handle){
    DPRINT("closing handle 0x%p",handle);
	if(handle){
        handle->parent->map_close(handle);
		handle->parent->window[handle->index]=0;
		if(handle->parent->nwindow>0)handle->parent->nwindow--;
		delete(handle);
		return 0;
	}
	return -4;
}


void* vmewrap_get_linux_add(vmewrap_window_t  handle){
	if(handle){
		DPRINT("returning mapped linux address %p, if zero driver unable to map",handle->mapped_address);
		return handle->mapped_address;
	}
	return 0;
}


int vmewrap_write32(vmewrap_window_t handle,unsigned off,uint32_t* data,int sizen){
	DPRINT("[%s]write32  0x%X+0x%x data0=0x%x ndata:%d, am:%d opt:%d",handle->parent->type.c_str(),handle->add,off,*data,sizen,handle->access,handle->opt);

	return handle->parent->vme_write32(handle,off,data,sizen);

}
int vmewrap_write16(vmewrap_window_t handle,unsigned off,uint16_t* data,int sizen){
	DPRINT("[%s]write16  0x%X+0x%x data0=0x%x ndata:%d, am:%d opt:%d",handle->parent->type.c_str(),handle->add,off,*data,sizen,handle->access,handle->opt);

	return handle->parent->vme_write16(handle,off,data,sizen);

}

int vmewrap_write8(vmewrap_window_t handle,unsigned off,uint8_t* data,int sizen){
	DPRINT("[%s]write8  0x%X+0x%x data0=0x%x ndata:%d, am:%d opt:%d",handle->parent->type.c_str(),handle->add,off,*data,sizen,handle->access,handle->opt);

	return handle->parent->vme_write8(handle,off,data,sizen);
}
int vmewrap_set_reg(vmewrap_window_t handle,void* data, unsigned off,int size){
	return handle->parent->vme_set_reg(handle,off,data,size);
}
int vmewrap_clr_reg(vmewrap_window_t handle,void* data, unsigned off,int size){
	return handle->parent->vme_clr_reg(handle,off,data,size);
}

int vmewrap_read32(vmewrap_window_t handle,unsigned off,uint32_t* data,int sizen){
	int ret;
	DPRINT("[%s]read32  0x%X+0x%x data0=0x%x size:%d, am:%d opt:%d",handle->parent->type.c_str(),handle->add,off,*data,sizen,handle->access,handle->opt);

	ret =handle->parent->vme_read32(handle,off,data,sizen);
	//DPRINT("read32 off 0x%x data=0x%x, ret=%d",off,*data,ret);

	return ret;
}
int vmewrap_read16(vmewrap_window_t handle,unsigned off,uint16_t* data,int sizen){
	int ret;
	ret =handle->parent->vme_read16(handle,off,data,sizen);
	//DPRINT("read16 off 0x%x data=0x%x, ret=%d",off,*data,ret);
	return ret;
}
int vmewrap_read8(vmewrap_window_t handle,unsigned off,uint8_t* data,int sizen){
	int ret;
	ret = handle->parent->vme_read8(handle,off,data,sizen);
	//DPRINT("read8 off 0x%x data=0x%x, ret=%d",off,*data,ret);
	return ret;
}


int vmewrap_interrupt_enable(vmewrap_window_t handle,int level, int signature,int type,void*priv){
	int ret;
	if(handle == NULL){
		DERR("not implemented")
					return -1;
	}
    ret = handle->parent->vme_interrupt_enable(handle,level,signature,type,priv);
	return ret;
}

/**
 * interrupt disable
   		@return 0 on success
 */

int vmewrap_interrupt_disable(vmewrap_window_t handle){
	int ret;
	if(handle == NULL){
		DERR("not implemented")
				return -1;
	}
	ret = handle->parent->vme_interrupt_disable(handle);
	DPRINT("interrupt disable, ret=%d",ret);
	return ret;
}
/**
 * wait for interrupt
 * @param timeo_ms timeout in ms (0 indefinite wait)
 * @return 0 on success
 */
int vmewrap_wait_interrupt(vmewrap_window_t handle,int timeo_ms){
	int ret;
	if(handle == NULL){
		DERR("not implemented")
					return -1;
	}
	ret = handle->parent->vme_wait_interrupt(handle,timeo_ms);

	return ret;
}
uint32_t vmewrap_read_reg32(vmewrap_window_t handle,unsigned off){
	uint32_t ret;
	vmewrap_read32(handle,off,&ret,1);
	return ret;
}
uint16_t vmewrap_read_reg16(vmewrap_window_t handle,unsigned off){
	uint16_t ret;
	vmewrap_read16(handle,off,&ret,1);
	return ret;
}
uint8_t vmewrap_read_reg8(vmewrap_window_t handle,unsigned off){
	uint8_t ret;
	vmewrap_read8(handle,off,&ret,1);
	return ret;
}

int vmewrap_write_reg32(vmewrap_window_t handle,uint32_t data,unsigned off){
	return vmewrap_write32(handle, off,&data,1);
}
int vmewrap_write_reg16(vmewrap_window_t handle,uint16_t data,unsigned off){
	return vmewrap_write16(handle, off,&data,1);

}
int vmewrap_write_reg8(vmewrap_window_t handle,uint8_t data,unsigned off){
	return vmewrap_write8(handle, off,&data,1);


}
