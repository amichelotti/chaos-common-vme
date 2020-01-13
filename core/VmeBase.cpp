/*
 * VmeBase.cpp
 *
 *  Created on: Oct 25, 2017
 *      Author: michelo
 */

#include "VmeBase.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

namespace common {
namespace vme {
int VmeBase::nboard=0;
bool VmeBase::isUnixDev(){return (unixdev>0);};

VmeBase::VmeBase():vme(NULL),run(0),address(0),size(0),unixdev(-1) {
	// TODO Auto-generated constructor stub
    boardid=++nboard;
}


int VmeBase::getFD(){
	if(unixdev<0)
		return vmewrap_getFD(vme);

	return unixdev;
	
}
int VmeBase::openUnixDev(const char*dev){
	int fd = ::open(dev,O_RDWR);
	if(fd<0){
			DERR("Opening unix dev \"%s\" fd=%d",dev,unixdev);

		return fd;
	}
	if(vme==NULL){
		vme = vmewrap_init_driver(VME_LINUX_USER_DRIVER);
	}
	vmewrap_setFD(vme,fd);
	unixdev=fd;
	DPRINT("Opening unix dev \"%s\" fd=%d",dev,unixdev);

	return 0;
}

int VmeBase::open(vme_driver_t vme_driver,uint64_t address_,uint32_t size_,vme_addressing_t master_addressing_,vme_access_t dw_, vme_opt_t vme_options_){
	run=0;
	unixdev=-1;

	if(vme==NULL){
		vme = vmewrap_init_driver(vme_driver);
	}

	if(vme==NULL){
		ERR("cannot initialize VME driver %d",vme_driver);

		return -2;
	}
    if(vmewrap_vme_open_master(vme,address_,size_,master_addressing_,dw_,vme_options_)!=0){
		ERR("cannot map vme");
		return -3;
	}
    address=address_;
    size=size_;
    master_addressing=master_addressing_;
    dw=dw_;
    vme_options=vme_options_;
	return 0;
}

int VmeBase::write(uint32_t off,uint16_t* data,int sizen){
	if(unixdev>0){
		::lseek(unixdev,off,SEEK_SET);
		DPRINT("unix write at off=0x%x data=0x%x fd=%d",off,*data,unixdev);

		return ::write(unixdev,(void*)data,sizeof(uint16_t)*sizen);
	}
	DPRINT("vme write at off=0x%x data=0x%x fd=%d",off,*data,getFD());
	return vmewrap_write16(vme,off,data,sizen);
}
int VmeBase::read(uint32_t off,uint16_t* data,int sizen){
	if(unixdev>0){
		::lseek(unixdev,off,SEEK_SET);
		return ::read(unixdev,(void*)data,sizeof(uint16_t)*sizen);
	}
	return vmewrap_read16(vme,off,data,sizen);

}
int VmeBase::write(uint32_t off,uint32_t* data,int sizen){
	if(unixdev>0){
		::lseek(unixdev,off,SEEK_SET);
		return ::write(unixdev,(void*)data,sizeof(uint32_t)*sizen);
	}
	return vmewrap_write32(vme,off,data,sizen);

}
int VmeBase::read(uint32_t off,uint32_t* data,int sizen){
	if(unixdev>0){
		::lseek(unixdev,off,SEEK_SET);
		return ::read(unixdev,(void*)data,sizeof(uint32_t)*sizen);
	}
	return vmewrap_read32(vme,off,data,sizen);

}
int VmeBase::write(uint32_t off,uint8_t* data,int sizen){
	if(unixdev>0){
		::lseek(unixdev,off,SEEK_SET);
		return ::write(unixdev,(void*)data,sizen);
	}
	return vmewrap_write8(vme,off,data,sizen);

}
int VmeBase::read(uint32_t off,uint8_t* data,int sizen){
	if(unixdev>0){
		::lseek(unixdev,off,SEEK_SET);
		return ::read(unixdev,(void*)data,sizen);
	}
	return vmewrap_read8(vme,off,data,sizen);


}


int VmeBase::write(uint32_t off,uint16_t data){
	if(unixdev>0){
		return this->write(off, &data,  1);
	} 
	 return vmewrap_write_reg16(vme, data,  off);
}

int VmeBase::write(uint32_t off,uint32_t data){
	if(unixdev>0){
		return this->write(off, &data,  1);
	} 
	 return vmewrap_write_reg32(vme, data,  off);

}
int VmeBase::write(uint32_t off,uint8_t data){
	if(unixdev>0){
		return this->write(off, &data,  1);
	} 
	 return vmewrap_write_reg8(vme, data,  off);

}
int VmeBase::read(uint32_t off,uint8_t &data){
	if(unixdev>0){
		return this->read(off,&data,1);
	}
	data = vmewrap_read_reg8(vme,  off);
    return 0;

}
int VmeBase::read(uint32_t off,uint16_t &data){
	if(unixdev>0){
		return this->read(off,&data,1);
	}
	data = vmewrap_read_reg16(vme,  off);
    return 0;
}
uint16_t VmeBase::read16(uint32_t off){
	if(unixdev>0){
		uint16_t data;
		this->read(off,&data,1);
		return data;
	}
	uint16_t ret=vmewrap_read_reg16(vme,  off);
    return ret;

}
uint32_t VmeBase::read32(uint32_t off){
	if(unixdev>0){
		uint32_t data;
		this->read(off,&data,1);
		return data;
	} 
	uint32_t ret=vmewrap_read_reg32(vme,  off);
    return ret;
}

int VmeBase::write16(uint32_t off,uint16_t data){
	if(unixdev>0){
		return this->write(off, &data,  1);
	}
	return vmewrap_write_reg16(vme, data,  off);

	
}
int VmeBase::write32(uint32_t off,uint32_t data){
		if(unixdev>0){

			return this->write(off, &data,  1);
		}
	return vmewrap_write_reg32(vme, data,  off);

}
int VmeBase::write8(uint32_t off,uint8_t data){
			if(unixdev>0){

		return this->write(off, &data,  1);
			}
	return vmewrap_write_reg8(vme, data,  off);

}
uint8_t VmeBase::read8(uint32_t off){
	uint8_t data;
	this->read(off,&data,1);
	return data;

}
int VmeBase::read(uint32_t off,uint32_t &data){
		return this->read(off,&data,1);

}

int VmeBase::interrupt_enable(int level, int signature,int type,void*priv){
    return vmewrap_interrupt_enable(vme, level,  signature,type,priv);

}
int VmeBase::interrupt_disable(){
  return vmewrap_interrupt_disable(vme);
}
int VmeBase::wait_interrupt(int timeo){
    return vmewrap_wait_interrupt(vme,timeo);
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
	if(unixdev>0){
		::close(unixdev);
		return 0;
	}
    if(vme){
        int ret= vmewrap_vme_close(vme);
        vme=NULL;
        return ret;
    }
    return 0;
}
int VmeBase::reset(){
    return 0;
}


VmeBase::~VmeBase(){
	close();
    nboard--;

}
} /* namespace vme */
} /* namespace common */
