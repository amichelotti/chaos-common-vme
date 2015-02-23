#include "caen960_drv.h"
#include <stdio.h>
#include <stdlib.h>

#define USAGE printf("%s <caen960 vme address> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);
int main(int argc,char**argv){
  unsigned long address=0;
  caen960_handle_t caen;
  uint32_t acquire_cycles=0;
  uint32_t acquire_timeo=0;
  uint32_t low[16],hi[16];
  uint64_t cycle=0;
  int cnt;
  if(argc<2){
    USAGE;
    return 1;
  }
  address = strtoul(argv[1],0,0);
  if(argc>2){
    acquire_cycles = strtoul(argv[2],0,0);
    printf("* number of acquisitions %d\n",acquire_cycles);
  }

  if(argc>3){
    acquire_timeo = strtoul(argv[3],0,0);
    printf("* timeout set to %d ms\n",acquire_timeo);
  }
  printf("* opening caen at address @0x%lx\n",address);
  caen = caen960_open(address);
  if(caen){
    caen960_init(caen,1,1);
    cnt = acquire_cycles;

    printf("* Status 0x%x, Buffer status 0x%x, acquiring ... \n", caen960_getStatus(caen),caen960_getBufferStatus(caen));
    while((acquire_cycles==0) || (cnt>=0)){
      int ret;
      
      ret = caen960_acquire_channels_poll(caen,low,hi,0,16,&cycle,acquire_timeo);
      printf("* acquired %d channels, events:%Ld\n",ret,cycle);
      cnt--;
    }
    caen960_close(caen);
  } else {
    printf("## cannot open caen960 device\n");
    return -1;
  }

    return 0;
}
