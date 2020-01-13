
#include  <vmedrv/sis3153/project_system_define.h>
#include <vmedrv/sis3153/project_interface_define.h>   
#include  <vmedrv/sis3153/vme_interface_class.h>
#include  <vmedrv/sis3153/sis3153ETH_vme_class.h>
#include <sys/types.h>
#include <sys/socket.h>




#include "vmewrap_int.h"
#include <common/debug/core/debug.h>

typedef struct sis3153_eth_priv{
	char ip[64];
	vme_addressing_t addressing;
	vme_access_t access;
	vme_opt_t opt;
	uint32_t add;
	uint32_t size;
} sis3153_eth_priv_t;

static int vme_close_sis3153_eth(vmewrap_int_vme_handle_t handle){

	return 0;
}
static int vme_init_sis3153_eth(vmewrap_int_vme_handle_t handle){

	return 0;

}
static int  map_master_sis3153_eth(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options){
sis3153_eth_priv_t *opt=(sis3153_eth_priv_t*)handle->priv;
	opt->addressing=addressing;
	opt->opt=vme_options;
	opt->access=dw;
	opt->add=add;
	opt->size=size;
	
	return 0;
	
}

static int  map_slave_sis3153_eth(vmewrap_int_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw,vme_opt_t vme_options){
	sis3153_eth_priv_t *opt=(sis3153_eth_priv_t*)handle->priv;
	opt->addressing=addressing;
	opt->opt=vme_options;
	opt->access=dw;
	opt->add=add;
	opt->size=size;
	
	return 0;
}
static int vme_write8_sis3153_eth(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t* data,int sizen){
	int ret=0;

	return ret;

}

static int vme_write32_sis3153_eth(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t* data,int sizen){
	int ret=0;

	return ret;

}
static int vme_write16_sis3153_eth(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t* data,int sizen){
	int ret=0;

	return ret;
}
static int vme_read32_sis3153_eth(vmewrap_int_vme_handle_t  handle,unsigned off,uint32_t *data,int sizen){
	int ret=0;

	return  ret;

}
static int vme_read16_sis3153_eth(vmewrap_int_vme_handle_t  handle,unsigned off,uint16_t *data,int sizen){
	int ret=0;
	
	return ret;
}
static int vme_read8_sis3153_eth(vmewrap_int_vme_handle_t  handle,unsigned off,uint8_t *data,int sizen){
	int ret=0;

	return  ret;
}
typedef std::map<std::string,sis3153eth *> ip2sis_t;
static std::map<std::string,sis3153eth *> sisdev;
int vme_init_driver_sis3153_eth(vmewrap_vme_handle_t handle,void *params){

	vmewrap_int_vme_handle_t p=(vmewrap_int_vme_handle_t)handle;
	sis3153eth *vme_crate;
	const char*ip_addr_string=(const char*)params;
	if(ip_addr_string==NULL){
		DERR("invalid IP address");
		return -4;
	}
	ip2sis_t::iterator found=sisdev.find(ip_addr_string);
	if(found==sisdev.end()){
		sis3153eth(&vme_crate, (char*)ip_addr_string);
		if(vme_crate==NULL){
			DERR("cannot allocate sis controller");
			return -7;
		}
		sisdev[ip_addr_string]=vme_crate;
		char char_messages[128] ;
		unsigned int nof_found_devices ;

	// open Vme Interface device
		int return_code = vme_crate->vmeopen ();  // open Vme interface
		vme_crate->get_vmeopen_messages (char_messages, &nof_found_devices);  // open Vme interface
    	DPRINT("get_vmeopen_messages = %s , number of devices %d \n",char_messages, nof_found_devices);
		if(nof_found_devices<=0){
			DERR("NO DEVICE FOUND");
			return -9;
		}
		p->priv=(void*)calloc(1,sizeof(sis3153_eth_priv_t));

		
	}else {
		DPRINT("found a sis controller @%s 0x%p\n",ip_addr_string,*found);
		vme_crate=found->second;
	}
	
	p->vme_close=vme_close_sis3153_eth;
	p->vme_init=vme_init_sis3153_eth;
	p->map_master=map_master_sis3153_eth;
	p->map_slave=map_slave_sis3153_eth;
	p->vme_write8=vme_write8_sis3153_eth;
	p->vme_write16=vme_write16_sis3153_eth;
	p->vme_write32=vme_write32_sis3153_eth;

	p->vme_read8=vme_read8_sis3153_eth;
	p->vme_read16=vme_read16_sis3153_eth;
	p->vme_read32=vme_read32_sis3153_eth;
	p->bus = (void*)vme_crate;
	return 0;
}
int vme_deinit_driver_sis3153_eth(vmewrap_vme_handle_t handle){
	
	return 0;
}


