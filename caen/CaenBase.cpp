/*
 * CaenBase.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: michelo
 */

#include "CaenBase.h"
#include <common/debug/core/debug.h>
#include <common/vme/core/vmewrap.h>
using namespace common::vme::caen;

CaenBase::CaenBase() {
	// TODO Auto-generated constructor stub

}

CaenBase::~CaenBase() {
	// TODO Auto-generated destructor stub
}

int CaenBase::close(){
	DPRINT("%s closing handle @0x%x",board.c_str(),(unsigned)handle);
	if(handle){
		int ret;
		ret=vmewrap_vme_close(handle->vme);
		free(handle);
		return ret;
	}

	return -4;
}

int CaenBase::open(vme_driver_t vme_driver,uint64_t address ){
	void* mapped_address;
	int size = 0x10000;
	int boardid,manufactureid;
	vmewrap_vme_handle_t vme;
	DPRINT(" opening vme device at @0x%x",address);
	vme = vmewrap_init_driver(vme_driver);
	if(vme==NULL){
		ERR("cannot initialize VME driver %d",vme_driver);

		return -2;
	}
	if(vmewrap_vme_open_master(vme,address,size,VME_ADDRESSING_A32,VME_ACCESS_D32,(vme_opt_t)0)!=0){
		ERR("cannot map vme");
		return -3;
	}

	mapped_address =  vmewrap_get_linux_add(vme);
	if (0 == mapped_address) {
		DERR("cannot map VME window to address space");
	}

	handle = (_caen_common_handle_t*)calloc(1,sizeof(_caen_common_handle_t));

	if(handle==NULL){
		ERR("cannot allocate resources");
		vmewrap_vme_close(vme);
		return -3;
	}
	DPRINT("allocated caen handle 0x%x",handle);
	handle->vme = vme;
	handle->mapped_address = mapped_address;
	handle->cycle = 0;
	boardid =((VME_READ_REG16(vme,BOARD_ID_OFF)&0xFF)<<8) | VME_READ_REG16(vme,BOARD_ID_LSB_OFF)&0xFF;

	manufactureid=VME_READ_REG16(vme,OUI_OFF)&0xFF;
	handle->boardid=boardid;
	handle->manufactureid=manufactureid;
	handle->version=VME_READ_REG16(vme,BOARD_VERSION_OFF);

	channels=0;
	if(manufactureid==MANUFACTURE_ID){
		switch(boardid){
		case 0x0307:
			if(((handle->version)&0xF0)==0xE0){
				board.assign("CAEN775N");
				channels=16;
			} else {
				board.assign("CAEN775");
				channels=32;
			}
			break;

		case 0x3C5:
			board.assign("CAEN965");
			channels=16;
			break;
		case 0x318:
			board.assign("CAEN792");
			channels=32;
			break;

		}
	}
	if(channels==0){
		ERR("unrecognized board id=0x%x manufacture id=0x%x, FW=0x%x",boardid,manufactureid, handle->version);
		return -3;
	}
	DPRINT("found board \"%s\" manufacture id 0x%x boardid:0x%x, version 0x%x nchannels %d",board.c_str(),manufactureid,boardid,handle->version,channels);
	return 0;
}



void CaenBase::init(uint32_t crate_num,int hwreset){
	DPRINT("%s intialiazing @0x%x",board.c_str(),(uint32_t)handle);
	int cnt;
	if(hwreset){
		VME_WRITE16(handle->vme,SSRESET_OFF,1);
		VME_WRITE16(handle->vme,BITSET_OFF,SOFTRESET_BIT);
		sleep(1);
		VME_WRITE16(handle->vme,BITCLR_OFF,SOFTRESET_BIT);
	}


	VME_WRITE16(handle->vme,CRATE_SEL_OFF,crate_num);
	VME_WRITE16(handle->vme,BITSET2_OFF,CAEN_COMMON_STOP|CAEN_CLEAR_DATA|CAEN_EMPTY_PROG|CAEN_AUTO_INCR|CAEN_ACCEPT_OVER_RANGE|CAEN_VALID_DISABLE|CAEN_THRESHOLD_DISABLE);
	VME_WRITE16(handle->vme,BITCLR2_OFF,CAEN_ALL_TRIGGER|CAEN_MEMORY_TEST|CAEN_ADC_OFFLINE|CAEN_TEST_ACQ|CAEN_CLEAR_DATA);

	resetEventBuffer();
	resetEventCounter();
}

void CaenBase::resetEventCounter(){
	VME_WRITE32(handle->vme,EVT_CNTRESET_OFF,0);

}
void CaenBase::setThreashold(uint16_t chan,uint16_t value){
	DPRINT("%s setting threshold chan %d = 0x%x",board.c_str(),chan,value);

	VME_WRITE16(handle->vme,BITCLR2_OFF,CAEN_VALID_DISABLE|CAEN_THRESHOLD_DISABLE);
	VME_WRITE16(handle->vme,THRS_CHANNEL_OFF + (2*chan),value);
}

uint16_t CaenBase::getThreashold(uint16_t chan){
	DPRINT("%s getting threshold chan %d",board.c_str(),chan);

	return VME_READ_REG16(handle->vme,THRS_CHANNEL_OFF + (2*chan));
}


uint16_t CaenBase::getStatus(){
	uint16_t ret;
	ret =  VME_READ_REG16(handle->vme,STATUS_OFF);
	DPRINT("%s get status 0x%x",board.c_str(),ret);
	return ret;
}

uint16_t CaenBase::getBufferStatus(){
	uint16_t ret;
	ret =  VME_READ_REG16(handle->vme,STATUS2_OFF);

	DPRINT("%s get buffer status 0x%x",board.c_str(),ret);
	return ret;
}


uint32_t CaenBase::getEventCounter(bool reset){
	uint32_t ret;

	ret = VME_READ_REG16(handle->vme,EVT_CNT_LOW_OFF)|(VME_READ_REG16(handle->vme,EVT_CNT_HI_OFF)<<16);
	if(reset){
		resetEventCounter();
		handle->event_counter=0;
	}
	return ret;
}

void CaenBase::resetEventBuffer(){
	VME_WRITE16(handle->vme,BITSET2_OFF,CLEARDATA_BIT);
	VME_WRITE16(handle->vme,BITCLR2_OFF,CLEARDATA_BIT);
}

int32_t CaenBase::waitEvent(int timeo_ms){
	uint16_t status;
	uint64_t diff=0,now=common::debug::getUsTime();
	uint32_t endm=timeo_ms*1000;
	do{
		// get the status from the caen_qdc
		status = VME_READ_REG16(handle->vme,STATUS_OFF);
		//usleep(100);
		if(endm>0){
			diff=common::debug::getUsTime() - now;
		}
	} while(((status&CAEN_QDC_STATUS_DREADY)==0)&&((diff)<=endm));
	if(diff>endm){
		ERR("timeout expired %Lu us",diff);
		return -1;
	}
	return 0;
}
uint16_t CaenBase::searchEvent(){
	common_header_t a;
	do{
		a.data  = VME_READ_REG32(handle->vme,0);
		if(a.ddata.signature!=2){
			DPRINT("found event %d not start event",a.ddata.signature);
		}
	} while(a.ddata.signature!=2);
	DPRINT("found event %d channels acquired",a.ddata.cnt);
	return a.ddata.cnt;
}

uint32_t CaenBase::acquireBuffer(uint32_t* buffer,uint32_t max_size){
	uint32_t ret=0;
	uint16_t status;
	while((ret<max_size)&&((status =  (VME_READ_REG32(handle->vme,STATUS2_OFF)&CAEN_QDC_STATUS_BUFFER_EMPTY))==0)){
		buffer[ret++]= REG32(handle->mapped_address,0);
	}
	return ret;
}
uint16_t CaenBase::acquireChannels(uint32_t* channel,uint32_t *event){
	// search header
	common_header_t a;
	common_footer_t b;

	common_data_t buf;
	uint16_t nchan,ret_channels=searchEvent();
	int ch;
	nchan=ret_channels;
	while(nchan--){
		buf.data=  VME_READ_REG32(handle->vme,0);
		if(buf.ddata.signature==0){
			if(channels==16)
				ch=buf.ddata.channel>>1;
			else
				ch=buf.ddata.channel;

			channel[ch] =buf.ddata.adc;
		}
	}
	b.data  = VME_READ_REG32(handle->vme,0);
	if(b.ddata.signature==4){
		handle->cycle+= abs(b.ddata.ev_counter-handle->event_counter);
		handle->event_counter =b.ddata.ev_counter;
		*event = b.ddata.ev_counter;
		return ret_channels;
	}
	ERR("not found end signature, found %d into word 0x%x",b.ddata.signature,b.data);
	return 0;
}
uint16_t CaenBase::acquireChannels(uint16_t* channel,uint32_t *event){
	// search header
	common_header_t a;
	common_footer_t b;
	int ch;
	common_data_t buf;

	uint16_t nchan,ret_channels=searchEvent();
	nchan=ret_channels;

	while(nchan--){
		buf.data=  REG32(handle->mapped_address,0);
		//	DPRINT("signature %d channel %d val 0x%x",buf.ddata.signature,buf.ddata.channel,buf.ddata.adc)

		if(buf.ddata.signature==0){
			if(channels==16)
				ch=buf.ddata.channel>>1;
			else
				ch=buf.ddata.channel;

			channel[ch] =buf.ddata.adc;
		}
	}
	b.data  = VME_READ_REG32(handle->vme,0);
	if(b.ddata.signature==4){
		handle->cycle+= abs(b.ddata.ev_counter-handle->event_counter);
		handle->event_counter =b.ddata.ev_counter;
		*event = b.ddata.ev_counter;
		return ret_channels;
	}
	ERR("not found end signature found %d into 0x%x",b.ddata.signature,b.data);
	return 0;

}


void CaenBase::setMode(caen_modes_t mode){
	VME_WRITE16(handle->vme,BITSET2_OFF,mode);
}

void CaenBase::clrMode(caen_modes_t mode){
	VME_WRITE16(handle->vme,BITCLR2_OFF,mode);
}

void CaenBase::write(uint16_t off,uint16_t data){
	vmewrap_write16(handle,off,data);
}
void CaenBase::read(uint16_t off,uint16_t &data){
	vmewrap_read16(handle,off,&data);

}
void CaenBase::write(uint16_t off,uint32_t data){
	vmewrap_write32(handle,off,data);
}
void CaenBase::read(uint16_t off,uint32_t &data){
	vmewrap_read32(handle,off,&data);
}
