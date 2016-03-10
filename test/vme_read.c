
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmewrap.h"
#define USAGE printf("%s <driver type 0=universe2,1=caen> <vme base address> <addressing 16/24/32> <data width 8/16/32> <address off> <read size>\n",argv[0]);

int main(int argc,char**argv){
  unsigned long address=0;
  unsigned addressing=32;
  unsigned off=0;
  int size=4;
  int cnt;
  int dw;
  int type;
  vmewrap_vme_handle_t handle;
  void*ptr;
  FILE*out;
  if(argc<7){
    USAGE;
    return 1;
  }
  type =strtoul(argv[1],0,0);
  address=strtoul(argv[2],0,0);
  addressing=strtoul(argv[3],0,0);
  dw=strtoul(argv[4],0,0);
  off=strtoul(argv[5],0,0);
  size=strtoul(argv[6],0,0);
  handle=vmewrap_init_driver(type);

  if(handle==NULL){
    printf("## cannot initialize driver type 0x%x\n",type);
    return -1;
  }
  if(vmewrap_vme_open_master(handle,address,0x1000000,addressing,32,0)!=0){
	  printf("## cannot map address 0x%x\n",address);
	  return -2;
  }
  ptr=vmewrap_get_linux_add(handle);
  if(ptr==NULL){
      printf("## cannot remap address 0x%x\n",address);
    return -1;
  }
  for(cnt=0;cnt<size;cnt++){
    int add;
    if(dw==16){
      uint16_t data;
      vmewrap_read16(handle,off,&data);
      printf("READ16[0x%x]=0x%x\n",off,data);
    } else if(dw==8){
      uint8_t data;
      vmewrap_read8(handle,off,&data);
      printf("READ8[0x%x]=0x%x\n",off,data);
    } else {
      uint32_t data;
      vmewrap_read32(handle,off,&data);
      printf("READ32[0x%x]=0x%x\n",off,data);
    }
  }
  vmewrap_vme_close(handle);
  return 0;
}
