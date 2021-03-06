
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmewrap.h"
#define USAGE printf("%s <driver type 0=universe2,1=caen,4=vme_user, <ip if sis>> <vme base address> <vme size> <addressing 16/24/32> <data width 8/16/32> <address off> <data0> [data1] ...\n",argv[0]);

int main(int argc,char**argv){
  unsigned long address=0;
  unsigned addressing=32;
  unsigned vme_size=0x1000;
  unsigned off=0;
  vmewrap_vme_handle_t handle;
  vmewrap_window_t window;

  unsigned* data=0;
  int cnt;
  int ndati;
  int dw=0;
  void*ptr;
  int type;
  if(argc<8){
    USAGE;
    return 1;
  }
	if(strchr(argv[1],'.')){
		// if contains characters assume IP -> sis
		type=(int)VME_SIS3153_ETH_DRIVER;
	} else {
		type=strtoul(argv[1],0,0);
	}
  address=strtoul(argv[2],0,0);
  vme_size=strtoul(argv[3],0,0);
  addressing=strtoul(argv[4],0,0);
  dw=strtoul(argv[5],0,0);
  off=strtoul(argv[6],0,0);
  ndati=(argc-7);
  data= (unsigned*)malloc(ndati*sizeof(unsigned));
  for(cnt=0;cnt<ndati;cnt++){
	if(dw==16){
		uint16_t *ptr=(uint16_t *)data;
		ptr[cnt]=strtoul(argv[7+cnt],0,0);
	} else if(dw==32){
		data[cnt]=strtoul(argv[7+cnt],0,0);
	} else if(dw==8){
		uint8_t *ptr=(uint8_t *)data;
		ptr[cnt]=strtoul(argv[7+cnt],0,0);
	}

  }
  handle=vmewrap_init_driver((vme_driver_t)type,argv[1]);

   if(handle==NULL){
     printf("## cannot initialize driver type 0x%x\n",type);
     return -1;
   }
   int am=VME_OPT_AM_USER_AM|VME_OPT_AM_DATA_AM;
   if((window=vmewrap_vme_open_master(handle,address,vme_size,(vme_addressing_t)addressing,(vme_access_t)dw,(vme_opt_t)am))==0){
 	  printf("## cannot map address 0x%lx\n",address);
 	  return -2;
   }
 /* ptr=vmewrap_get_linux_add(handle);
  if(ptr==NULL){
      printf("%% cannot remap address 0x%lx to linux space\n",address);

  }
*/
    if(dw==16){

      vmewrap_write16(window,off,(uint16_t*)data,ndati);
    } else if(dw==8){

      vmewrap_write8(window,off,(uint8_t*)data,ndati);
    } else {
      vmewrap_write32(window,off,(uint32_t*)data,ndati);
    }
    free(data);
  return 0;
}
