
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmewrap.h"
#define USAGE printf("%s <vme base address> <addressing 16/24/32> <data width 8/16/32> <address off> <data0> [data1] ...\n",argv[0]);

int main(int argc,char**argv){
  unsigned long address=0;
  unsigned addressing=32;
  unsigned off=0;
  int size=4;
  vmewrap_vme_handle_t handle;
  unsigned* data=0;
  int cnt;
  int ndati;
  int dw=0;
  void*ptr;
    FILE*out;
  if(argc<6){
    USAGE;
    return 1;
  }
  address=strtoul(argv[1],0,0);
  addressing=strtoul(argv[2],0,0);
  dw=strtoul(argv[3],0,0);
  off=strtoul(argv[4],0,0);
  ndati=(argc-5);
  data= (unsigned*)malloc(ndati*sizeof(unsigned));
  for(cnt=0;cnt<ndati;cnt++){
    data[cnt]=strtoul(argv[5+cnt],0,0);
  }
  handle= vmewrap_vme_open_master(address,0x1000000,addressing,32,0);
  if(handle==NULL){
    printf("## cannot map address 0x%x\n",address);
    return -1;
  }
  ptr=vmewrap_get_linux_add(handle);
  if(ptr==NULL){
      printf("## cannot remap address 0x%x\n",address);
    return -1;
  }
  for(cnt=0;cnt<ndati;cnt++){
    if(dw==16){

      uint16_t dd=data[cnt];
      vmewrap_write16(handle,off,dd);
      printf("WRITE16[0x%x]=0x%x\n",off,dd);
    } else if(dw==8){

      uint8_t dd=data[cnt];
      vmewrap_write8(handle,off,dd);
      printf("WRITE8[0x%x]=0x%x\n",off,dd);
    } else {
      uint16_t dd=data[cnt];
      vmewrap_write32(handle,off,dd);
      printf("WRITE32[0x%x]=0x%x\n",off,dd);
    }
  }
  return 0;
}
