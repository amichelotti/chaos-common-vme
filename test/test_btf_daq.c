#include "caen965_drv.h"
#include "caen792_drv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <965|792> <caen960 vme address> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);


void dump_channels(FILE*o,uint32_t *chan,uint64_t cycle,int channels){
    int cnt;
    for(cnt=0;cnt<channels;cnt++){
        fprintf(o,"- %lld [%d] = 0x%x\n",cycle,cnt,chan[cnt]);
    }
}

int main(int argc,char**argv){
  unsigned long address=0;
  void* caen;
  uint32_t acquire_cycles=0;
  uint32_t acquire_timeo=0;
  uint32_t low[16],hi[16],ch[32];
    
  uint64_t cycle=0;
  int is965=1;
  int cnt;
    FILE*out;
  if(argc<3){
    USAGE;
    return 1;
  }
    if(strstr(argv[1],"792")){
        is965=0;
    } else if(!strstr(argv[1],"965")){
        printf("## incalid QDC device %s\n",argv[1]);
        return -2;
    }
  address = strtoul(argv[2],0,0);
  if(argc>3){
    acquire_cycles = strtoul(argv[2],0,0);
    printf("* number of acquisitions %d\n",acquire_cycles);
  }

  if(argc>4){
    acquire_timeo = strtoul(argv[3],0,0);
    printf("* timeout set to %d ms\n",acquire_timeo);
  }
  printf("* opening caen at address @0x%lx\n",address);
    if(is965){
        caen = caen965_open(address);
    } else {
        caen = caen792_open(address);
    }
    
    out=fopen("caen_data.out","w");
    if(out){
        printf("* opened caen_data.out\n");
        
    } else {
        printf("## cannot open caen_data.out for write\n");
        return -1;
    }
    
  if(caen){
      int stat,bufstat;
      if(is965){
          caen965_init(caen,1,1);
          stat=caen965_getStatus(caen);
          bufstat=caen965_getBufferStatus(caen);
          fprintf(out,"===CAEN 965 stat :0x%x buf stat:0x%x",stat,bufstat);
      } else {
          caen792_init(caen,1,1);
          stat=caen792_getStatus(caen);
          bufstat=caen792_getBufferStatus(caen);
          fprintf(out,"===CAEN 792 stat :0x%x buf stat:0x%x",stat,bufstat);


      }
    cnt = acquire_cycles;

    printf("* Status 0x%x, Buffer status 0x%x, acquiring ... \n",stat ,bufstat);
    while((acquire_cycles==0) || (cnt>=0)){
      int ret;
       if(is965){
           ret = caen965_acquire_channels_poll(caen,low,hi,0,16,&cycle,acquire_timeo);
           dump_channels(out,low,cycle,ret);
           dump_channels(out,hi,cycle,ret);
       } else {
           ret = caen792_acquire_channels_poll(caen,ch,0,32,&cycle,acquire_timeo);
           dump_channels(out,ch,cycle,ret);

       }
      printf("* acquired %d channels, events:%lld\n",ret,cycle);
      cnt--;
    }
    if(is965){
        caen965_close(caen);
    } else {
        caen792_close(caen);

    }
  } else {
    printf("## cannot open caen960 device\n");
    return -1;
  }

    fclose(out);
    return 0;
}
