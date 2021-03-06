#include <common/vme/caen/caen775_drv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s  <driver type 0=universe2,1=caen> <caen775 vme address> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);


void dump_channels(FILE*o,uint32_t *chan,uint64_t cycle,int channels){
    int cnt;
    for(cnt=0;cnt<channels;cnt++){
        fprintf(o,"- %ld [%d] = 0x%x\n",cycle,cnt,chan[cnt]);
    }
}

int main(int argc,char**argv){
  unsigned long address=0;
  void* caen;
  uint32_t acquire_cycles=0;
  uint32_t acquire_timeo=0;
  uint32_t ch[32];
  int type;
  uint64_t cycle=0;
  int is965=1;
  int cnt;
    FILE*out;
  if(argc<3){
    USAGE;
    return 1;
  }
  type = strtoul(argv[1],0,0);

  address = strtoul(argv[2],0,0);
  if(argc>3){
    acquire_cycles = strtoul(argv[3],0,0);
    printf("* number of acquisitions %d\n",acquire_cycles);
  }

  if(argc>4){
    acquire_timeo = strtoul(argv[4],0,0);
    printf("* timeout set to %d ms\n",acquire_timeo);
  }
  printf("* opening caen at address @0x%lx\n",address);
  vmewrap_vme_handle_t vme= vmewrap_init_driver(VME_UNIVERSE2_DRIVER);

  caen = caen775_open(vme,address);
    
  out=fopen("caen_data.out","w");
    if(out){
        printf("* opened caen_data.out\n");
        
    } else {
        printf("## cannot open caen_data.out for write\n");
        return -1;
    }
    
  if(caen){
      int stat,bufstat;
      caen775_init(caen,1,1);
      caen775_disable_mode(caen,CAEN775_COMMON_STOP);
      stat=caen775_getStatus(caen);
       bufstat=caen775_getBufferStatus(caen);
        fprintf(out,"===CAEN stat :0x%x buf stat:0x%x",stat,bufstat);

    cnt = acquire_cycles;

    printf("* [%d event] Status 0x%x, Buffer status 0x%x, acquiring ... \n",caen775_getEventCounter(caen,0),stat ,bufstat);
    while((acquire_cycles==0) || (cnt>=0)){
      int ret;
      ret = caen775_acquire_channels_poll(caen,ch,&cycle,acquire_timeo);
      dump_channels(out,ch,cycle,sizeof(ch));


      printf("* acquired %d channels, events:%lld\n",ret,cycle);
      cnt--;
    }
      caen775_close(caen);

} else {
    printf("## cannot open caen device\n");
    return -1;
  }

    fclose(out);
    return 0;
}
