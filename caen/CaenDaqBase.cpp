/*
 * CaenDaqBase.cpp
 *
 *  Created on: Mar 3, 2016
 *      Author: michelo
 */

#include <common/debug/core/debug.h>
#include "CaenDaqBase.h"
using namespace common::vme::caen;
CaenDaqBase::CaenDaqBase() {
}

CaenDaqBase::~CaenDaqBase() {

}

int CaenDaqBase::close(){
	DPRINT("%s closing ",board.c_str());

	return ::common::vme::VmeBase::close();

}
int16_t CaenDaqBase::getMode(){
	 return read16( BITSET2_OFF);
}

int CaenDaqBase::open(vme_driver_t vme_driver,uint64_t address ){
	int size = 0x10000;
	int boardid,manufactureid;
	DPRINT(" opening vme device at @0x%llx",address);

	if(::common::vme::VmeBase::open(vme_driver,address,size,VME_ADDRESSING_A32,VME_ACCESS_D32,VME_OPT_AM_USER_AM)!=0){
		ERR("cannot map vme");
		return -3;
	}

	cycle = 0;
	boardid =((read16(BOARD_ID_OFF)&0xFF)<<8) | read16(BOARD_ID_LSB_OFF)&0xFF;

	manufactureid=read16(OUI_OFF)&0xFF;
	boardid=boardid;
	manufactureid=manufactureid;
	version=read16(BOARD_VERSION_OFF);

	channels=0;
	if(manufactureid==MANUFACTURE_ID){
		switch(boardid){
		case 0x0307:
			if(((version)&0xF0)==0xE0){
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
		ERR("unrecognized board id=0x%x manufacture id=0x%x, FW=0x%x",boardid,manufactureid, version);
		return -3;
	}

	DPRINT("found board \"%s\" manufacture id 0x%x boardid:0x%x, version 0x%x nchannels %d",board.c_str(),manufactureid,boardid,version,channels);
	return 0;
}



void CaenDaqBase::init(uint32_t crate_num,int hwreset){
	DPRINT("%s intialiazing",board.c_str());
	if(hwreset){
		write16(SSRESET_OFF,1);
		write16(BITSET_OFF,SOFTRESET_BIT);
		sleep(1);
		write16(BITCLR_OFF,SOFTRESET_BIT);
	}


	write16(CRATE_SEL_OFF,crate_num);
	write16(BITSET2_OFF,CAEN_COMMON_STOP|CAEN_CLEAR_DATA|CAEN_EMPTY_PROG|CAEN_AUTO_INCR|CAEN_ACCEPT_OVER_RANGE|CAEN_VALID_DISABLE|CAEN_THRESHOLD_DISABLE);
	write16(BITCLR2_OFF,CAEN_ALL_TRIGGER|CAEN_MEMORY_TEST|CAEN_ADC_OFFLINE|CAEN_TEST_ACQ|CAEN_CLEAR_DATA);

	resetEventBuffer();
	resetEventCounter();
}

void CaenDaqBase::resetEventCounter(){
    write16(EVT_CNTRESET_OFF,0);

}
void CaenDaqBase::setThreashold(uint16_t chan,uint16_t value){
	DPRINT("%s setting threshold chan %d = 0x%x",board.c_str(),chan,value);

	write16(BITCLR2_OFF,CAEN_VALID_DISABLE|CAEN_THRESHOLD_DISABLE);
	write16(THRS_CHANNEL_OFF + (2*chan),value);
}

uint16_t CaenDaqBase::getThreashold(uint16_t chan){
	DPRINT("%s getting threshold chan %d",board.c_str(),chan);

	return read16(THRS_CHANNEL_OFF + (2*chan));
}


uint16_t CaenDaqBase::getStatus(){
	uint16_t ret;
	ret =  read16(STATUS_OFF);
	DPRINT("%s get status 0x%x",board.c_str(),ret);
	return ret;
}

uint16_t CaenDaqBase::getBufferStatus(){
	uint16_t ret;
	ret =  read16(STATUS2_OFF);

	DPRINT("%s get buffer status 0x%x",board.c_str(),ret);
	return ret;
}


uint32_t CaenDaqBase::getEventCounter(bool reset){
	uint32_t ret;

	ret = read16(EVT_CNT_LOW_OFF)|(read16(EVT_CNT_HI_OFF)<<16);
	if(reset){
		resetEventCounter();
		event_counter=0;
	}
	return ret;
}

void CaenDaqBase::resetEventBuffer(){
	write16(BITSET2_OFF,CLEARDATA_BIT);
	write16(BITCLR2_OFF,CLEARDATA_BIT);
}

int32_t CaenDaqBase::waitEvent(int timeo_ms){
	uint16_t status;
	uint64_t diff=0,now=common::debug::getUsTime();
	uint32_t endm=timeo_ms*1000;
	do{
		// get the status from the caen_qdc
		status = read16(STATUS_OFF);
		//usleep(100);
		if(endm>0){
			diff=common::debug::getUsTime() - now;
		}
	} while(((status&CAEN_QDC_STATUS_DREADY)==0)&&((diff)<=endm));
	if(diff>endm){
		ERR("timeout expired %llu us",diff);
		return -1;
	}
	return 0;
}
uint16_t CaenDaqBase::searchEvent(){
	common_header_t a;
	do{
		a.data  =read32(0);
		if(a.ddata.signature!=2){
			DPRINT("found event %d not start event",a.ddata.signature);
		}
	} while(a.ddata.signature!=2);
	DPRINT("found event %d channels acquired",a.ddata.cnt);
	return a.ddata.cnt;
}

uint32_t CaenDaqBase::acquireBuffer(uint32_t* buffer,uint32_t max_size){
	uint32_t ret=0;
	uint16_t status;
	while((ret<max_size)&&((status =  (read32(STATUS2_OFF)&CAEN_QDC_STATUS_BUFFER_EMPTY))==0)){
		buffer[ret++]= read32(0);
	}
	return ret;
}
uint16_t CaenDaqBase::acquireChannels(uint32_t* channel,uint32_t *event){
	// search header
	common_footer_t b;

	common_data_t buf;
	uint16_t nchan,ret_channels=searchEvent();
	int ch;
	nchan=ret_channels;
	while(nchan--){
		buf.data=  read32(0);
		if(buf.ddata.signature==0){
			if(channels==16)
				ch=buf.ddata.channel>>1;
			else
				ch=buf.ddata.channel;

			channel[ch] =buf.ddata.adc;
		}
	}
	b.data  =read32(0);
	if(b.ddata.signature==4){
        cycle+= (b.ddata.ev_counter>event_counter)?(b.ddata.ev_counter-event_counter):0;
		event_counter =b.ddata.ev_counter;
		*event = b.ddata.ev_counter;
		return ret_channels;
	}
	ERR("not found end signature, found %d into word 0x%x",b.ddata.signature,b.data);
	return 0;
}


uint16_t CaenDaqBase::acquireChannels(uint16_t* channel,uint32_t *event){
	// search header
	common_footer_t b;
	int ch;
	common_data_t buf;

	uint16_t nchan,ret_channels=searchEvent();
	nchan=ret_channels;

	while(nchan--){
		buf.data=  read32(0);
		//	DPRINT("signature %d channel %d val 0x%x",buf.ddata.signature,buf.ddata.channel,buf.ddata.adc)

		if(buf.ddata.signature==0){
			if(channels==16)
				ch=buf.ddata.channel>>1;
			else
				ch=buf.ddata.channel;

			channel[ch] =buf.ddata.adc;
		}
	}
	b.data  =read32(0);
	if(b.ddata.signature==4){
        cycle+= (b.ddata.ev_counter>event_counter)?(b.ddata.ev_counter-event_counter): 0;
		event_counter =b.ddata.ev_counter;
		*event = b.ddata.ev_counter;
		return ret_channels;
	}
	ERR("not found end signature found %d into 0x%x",b.ddata.signature,b.data);
	return 0;

}


void CaenDaqBase::setMode(caen_modes_t mode){
	write16(BITSET2_OFF,mode);
}

void CaenDaqBase::clrMode(caen_modes_t mode){
	write16(BITCLR2_OFF,mode);
}

int CaenDaqBase::interrupt_enable(int meb_lenght){
    int ret;
    // initialize the IVR with the most significant byte of the address
    int ivr=(address>>24);
    if((meb_lenght<0) && (meb_lenght>32) ){
        DERR("invalid meb lenght %d",meb_lenght);
        return -2;
    }
    if(ivr==0){
        DERR("invalid signature");

        return -3;
    }
    ret= VmeBase::interrupt_enable(8-getBoardId(),ivr);
    write16(IVR_STATUS,ivr);
    write16(EVT_TRG_OFF,meb_lenght);
    write16(IVR_LEVEL,8-getBoardId());
    return ret;

}
int CaenDaqBase::interrupt_disable(){
    write16(EVT_TRG_OFF,0);
    write16(IVR_LEVEL,0);

    return VmeBase::interrupt_disable();

}
