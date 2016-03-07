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

int CaenBase::open(uint64_t address ){
  void* mapped_address;
  int size = 0x10000;
  int boardid,manufactureid;
  vmewrap_vme_handle_t vme;
  DPRINT(" opening vme device at @0x%x",address);
  vme = vmewrap_vme_open_master(address,size,VME_ADDRESSING_A32,VME_ACCESS_D32,(vme_opt_t)0);
  if(vme==NULL) {
	  ERR("cannot open vme at address @0x%x",address);
	  return -1;
  }

  mapped_address =  vmewrap_get_linux_add(vme);
  if (0 == mapped_address) {
    ERR("cannot map VME window");
    perror("vme_master_window_map");
    return -2;
  }

  handle = (_caen_common_handle_t*)calloc(1,sizeof(_caen_common_handle_t));
  if(handle==NULL){
    ERR("cannot allocate resources");
    vmewrap_vme_close(vme);
    return -3;
  }
  handle->vme = vme;
  handle->mapped_address = mapped_address;
  handle->cycle = 0;
  boardid=((BOARD_ID_REG(mapped_address)&0xFF)<<8) | (BOARD_ID_LSB_REG(mapped_address)&0xFF);
  manufactureid=OUI_REG(mapped_address)&0xFF;
    handle->boardid=boardid;
    handle->manufactureid=manufactureid;
    handle->version=BOARD_VERSION_REG(mapped_address);

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
    SSRESET_REG(handle->mapped_address) = 1;
    BITSET_REG(handle->mapped_address) = SOFTRESET_BIT;
    sleep(1);
    BITCLR_REG(handle->mapped_address) = SOFTRESET_BIT;

  }



  CRATE_SEL_REG(handle->mapped_address)=crate_num;
  BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT; // clear data reset
  BITCLR2_REG(handle->mapped_address)=TEST_MEM_BIT|OFFLINE_BIT|TESTAQ_BIT|CLEARDATA_BIT;

  //  BITSET2_REG(handle->mapped_address)=ALLTRG_EN_BIT/*|OVERRANGE_EN_BIT|LOWTHR_EN_BIT*/|EMPTY_EN_BIT|AUTOINCR_BIT;
  BITSET2_REG(handle->mapped_address)=ALLTRG_EN_BIT|EMPTY_EN_BIT|AUTOINCR_BIT|OVERRANGE_EN_BIT|LOWTHR_EN_BIT;
  EVT_CNTRESET_REG(handle->mapped_address)=0;
}


void CaenBase::setThreashold(uint16_t chan,uint16_t value){
  DPRINT("%s setting threshold chan %d = 0x%x",board.c_str(),chan,value);

    BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT|OVERRANGE_EN_BIT|LOWTHR_EN_BIT;

    REG16(handle->mapped_address,THRS_CHANNEL_OFF + (2*chan))=value;
}

uint16_t CaenBase::getThreashold(uint16_t chan){
  DPRINT("%s getting threshold chan %d",board.c_str(),chan);

    return REG16(handle->mapped_address,THRS_CHANNEL_OFF + (2*chan));
}


uint16_t CaenBase::getStatus(){
   uint16_t ret;
  ret = STATUS_REG(handle->mapped_address);
  DPRINT("%s get status 0x%x",board.c_str(),ret);
  return ret;
}

uint16_t CaenBase::getBufferStatus(){
   uint16_t ret;
  ret = STATUS2_REG(handle->mapped_address);
  DPRINT("%s get buffer status 0x%x",board.c_str(),ret);
  return ret;
}


 uint32_t CaenBase::getEventCounter(bool reset){
  uint32_t ret;

  ret = EVT_CNT_LOW_REG(handle->mapped_address)|EVT_CNT_HI_REG(handle->mapped_address)<<16;
  if(reset){
    EVT_CNTRESET_REG(handle->mapped_address)=0;
    handle->event_counter=0;
  }
  return ret;
}

 void CaenBase::resetEventBuffer(){
	 BITSET2_REG(handle->mapped_address)=CLEARDATA_BIT;
	 BITCLR2_REG(handle->mapped_address)=CLEARDATA_BIT;
 }

 int32_t CaenBase::waitEvent(int timeo_ms){
	 uint16_t status;
	 uint64_t diff=0,now=common::debug::getUsTime();
	 uint32_t endm=timeo_ms*1000;
	 do{
	    // get the status from the caen_qdc
	    status = STATUS_REG(handle->mapped_address);
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
			 a.data  = REG32(handle->mapped_address,0);
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
    while((ret<max_size)&&((status =  STATUS2_REG(handle->mapped_address))&CAEN_QDC_STATUS_BUFFER_EMPTY)==0){
    	buffer[ret++]= REG32(handle->mapped_address,0);
    }
    return ret;
}
uint16_t CaenBase::acquireChannels(uint32_t* channels,uint32_t *event){
	// search header
	 common_header_t a;
	 common_footer_t b;

	 common_data_t buf;

	 uint16_t nchan,ret_channels=searchEvent();
	 nchan=ret_channels;
	 while(nchan--){
		 buf.data=  REG32(handle->mapped_address,0);
		 if(buf.ddata.signature==0){
			 channels[buf.ddata.channel] =buf.ddata.adc;
		 }
	 }
	 b.data  = REG32(handle->mapped_address,0);
	 if(b.ddata.signature==4){
		 handle->cycle+= abs(b.ddata.ev_counter-handle->event_counter);
		 handle->event_counter =b.ddata.ev_counter;
		 *event = b.ddata.ev_counter;
		 return ret_channels;
	 }
	 ERR("not found end signature, found %d into word 0x%x",b.ddata.signature,b.data);
	 return 0;
}
uint16_t CaenBase::acquireChannels(uint16_t* channels,uint32_t *event){
	// search header
	 common_header_t a;
	 common_footer_t b;

	 common_data_t buf;

	 uint16_t ret_channels=searchEvent();
	 while(a.ddata.cnt--){
		 buf.data=  REG32(handle->mapped_address,0);
		 if(buf.ddata.signature==0){
			 channels[buf.ddata.channel] =buf.ddata.adc;
		 }
	 }
	 b.data  = REG32(handle->mapped_address,0);
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
	BITSET2_REG(handle->mapped_address)=mode;
}

void CaenBase::clrMode(caen_modes_t mode){
	BITCLR2_REG(handle->mapped_address)=mode;
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
