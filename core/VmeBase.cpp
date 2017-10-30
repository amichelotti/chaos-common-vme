/*
 * VmeBase.cpp
 *
 *  Created on: Oct 25, 2017
 *      Author: michelo
 */

#include "VmeBase.h"
#include <common/debug/core/debug.h>
namespace common {
namespace vme {

VmeBase::VmeBase():vme(NULL),run(0) {
	// TODO Auto-generated constructor stub

}



int VmeBase::open(vme_driver_t vme_driver,uint64_t address,uint32_t size,vme_addressing_t master_addressing,vme_access_t dw, vme_opt_t vme_options){
	run=0;
	if(vme==NULL){
		vme = vmewrap_init_driver(vme_driver);
	}

	if(vme==NULL){
		ERR("cannot initialize VME driver %d",vme_driver);

		return -2;
	}
	if(vmewrap_vme_open_master(vme,address,size,master_addressing,dw,vme_options)!=0){
		ERR("cannot map vme");
		return -3;
	}
	return 0;
}

int VmeBase::write(uint32_t off,uint16_t* data,int sizen){
	return vmewrap_write16(vme,off,data,sizen);
}
int VmeBase::read(uint32_t off,uint16_t* data,int sizen){
	return vmewrap_read16(vme,off,data,sizen);

}
int VmeBase::write(uint32_t off,uint32_t* data,int sizen){
	return vmewrap_write32(vme,off,data,sizen);

}
int VmeBase::read(uint32_t off,uint32_t* data,int sizen){
	return vmewrap_read32(vme,off,data,sizen);

}
int VmeBase::write(uint32_t off,uint8_t* data,int sizen){
	return vmewrap_write8(vme,off,data,sizen);

}
int VmeBase::read(uint32_t off,uint8_t* data,int sizen){
	return vmewrap_read8(vme,off,data,sizen);


}


int VmeBase::write(uint32_t off,uint16_t data){
	return vmewrap_write_reg16(vme, data,  off);
}

int VmeBase::write(uint32_t off,uint32_t data){
	return vmewrap_write_reg32(vme, data,  off);

}
int VmeBase::write(uint32_t off,uint8_t data){
	return vmewrap_write_reg8(vme, data,  off);

}
int VmeBase::read(uint32_t off,uint8_t &data){
	data = vmewrap_read_reg8(vme,  off);
	return 0;
}
int VmeBase::read(uint32_t off,uint16_t &data){
	data = vmewrap_read_reg16(vme,  off);
	return 0;
}
uint16_t VmeBase::read16(uint32_t off){
	uint16_t ret=vmewrap_read_reg16(vme,  off);
	return ret;
}
uint32_t VmeBase::read32(uint32_t off){
	uint32_t ret=vmewrap_read_reg32(vme,  off);
	return ret;

}

int VmeBase::write16(uint32_t off,uint16_t data){
	return vmewrap_write_reg16(vme, data,  off);
}
int VmeBase::write32(uint32_t off,uint32_t data){
	return vmewrap_write_reg32(vme, data,  off);
}
int VmeBase::write8(uint32_t off,uint8_t data){
	return vmewrap_write_reg8(vme, data,  off);
}
uint8_t VmeBase::read8(uint32_t off){
	uint8_t ret=vmewrap_read_reg8(vme,  off);
		return ret;
}
int VmeBase::read(uint32_t off,uint32_t &data){
	data = vmewrap_read_reg32(vme,  off);
	return 0;
}

int VmeBase::interrupt_enable(int level, int signature){
	return 0;
}
int VmeBase::interrupt_disable(){
	return 0;
}
void VmeBase::sched_task(){
	DPRINT("Interrupt Scheduler Started");

	while(run){
		 vmewrap_wait_interrupt(vme,0);
		 interrupt_handler();
	}
	DPRINT("Interrupt Scheduler Ended");

}
int VmeBase::install_interrupt_handler(int level,int signature){
	if(run==0){
			run =1;
			m_thread=boost::thread(&VmeBase::sched_task,this);
	}
	return 0;
}
int VmeBase::remove_interrupt_handler(){
	run=0;
	m_thread.join();
	return 0;
}
int VmeBase::interrupt_handler(){
	return 0;
}
int VmeBase::close(){
	run=0;
	int ret= vmewrap_vme_close(vme);
	vme=NULL;
	return ret;
}
VmeBase::~VmeBase(){
	close();
}
} /* namespace vme */
} /* namespace common */
