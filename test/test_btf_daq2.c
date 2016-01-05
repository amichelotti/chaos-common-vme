#include "caen965_drv.h"
#include "caen792_drv.h"
#include "caen513_drv.h"
#include "sis3800_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <conffile> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);

#define ENABLE_VETO 0x0
#define DISABLE_VETO 0xffff

#define CLOSEDEV(_x) \
  _x ## _close(_x ## _handle);

#define OPENDEV(_x)						\
  if(_x ## _addr && (_x ## _handle = _x ## _open(_x ## _addr))){ \
    printf("* " # _x " successfully mapped\n");\
  } else {\
      printf("## cannot map " # _x " address 0x%x\n",_x ## _addr);\
    return -2;\
  }

void dump_channels(FILE*o,uint32_t *chan,uint64_t cycle,int channels){
    int cnt;
    fprintf(o,"-\n");
    for(cnt=0;cnt<channels;cnt++){
        fprintf(o,"- %lld [%d] = 0x%x\n",cycle,cnt,chan[cnt]);
    }
}

int main(int argc,char**argv){
  unsigned long address=0;
  void* caen;
  char*conf_file;
  FILE* fconf_file;
  int ret;
  uint32_t acquire_cycles=0;
  uint32_t acquire_timeo=0;
  int timeo_counter=0;
  uint32_t low[16],hi[16],ch[32],counters[32];
  uint32_t caen513_addr=0,caen965_addr=0,caen792_addr=0,sis3800_addr=0;
  uint64_t loop=0,cycle0=0,cycle1=0,loop_time_start=0;
  caen965_handle_t caen965_handle=NULL;
  caen792_handle_t caen792_handle=NULL;
  caen513_handle_t caen513_handle=NULL;
  sis3800_handle_t sis3800_handle=NULL;
  int32_t pio;
  int is965=1;
  int cnt;
  uint32_t counter_before,counter_after;
  uint64_t lost=0;
    FILE*out;
  if(argc<2){
    USAGE;
    return 1;
  }
  conf_file=argv[1];
  fconf_file=fopen(conf_file,"r");
  if(fconf_file==NULL){
    printf("## cannot open configuration file \"%s\"\n",conf_file);
    return -1;
  }
  while(!feof(fconf_file)){
    char stringa[256];
    fgets(stringa,sizeof(stringa),fconf_file);
    if(sscanf(stringa,"pio:%x",&caen513_addr)==1){
      printf("* pio(caen 513) address 0x%x\n",caen513_addr);
    }
    if(sscanf(stringa,"scaler:%x",&sis3800_addr)==1){
      printf("* scaler (sis3800) address 0x%x\n",sis3800_addr);
    }
    if(sscanf(stringa,"qdc965:%x",&caen965_addr)==1){
      printf("* qdc965 (caen965) address 0x%x\n",caen965_addr);
    }
    if(sscanf(stringa,"qdc792:%x",&caen792_addr)==1){
      printf("* qdc792 (caen792) address 0x%x\n",caen792_addr);
    }
  }
  fclose(fconf_file);
  out=fopen("btf_daq.txt","w");

  OPENDEV(caen965);
  sleep(1);
  OPENDEV(caen792);
  sleep(1);
  OPENDEV(sis3800);
  sleep(1);
  OPENDEV(caen513);
  sleep(1);
  //  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); 
  caen513_reset(caen513_handle);
  //  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT); 
  //  
  //caen513_init(caen513_handle,1); //use board defaults
  /*
  for(cnt=8;cnt<16;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); // 15 trigger in
  */
  for(cnt=0;cnt<16;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT); 
  




  caen965_init(caen965_handle,0,1);
  caen792_init(caen792_handle,0,1);
  sis3800_init(sis3800_handle);

  //resetTM(caen513_handle);
  caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF  
  
  while(1){
    loop_time_start=getUsTime();
	
    printf("acquiring 965\n");
    counter_before=sis3800_readCounter(sis3800_handle,30);
    ret = caen965_acquire_channels_poll(caen965_handle,low,hi,0,16,&cycle0,0);
    //caen513_set(caen513_handle,ENABLE_VETO); // SW veto ON

    //    dump_channels(out,low,cycle0,ret);
    // dump_channels(out,hi,cycle0,ret);
    printf("acquire 792\n",pio);
    ret = caen792_acquire_channels_poll(caen792_handle,ch,0,16,&cycle1,0);
    
    //    dump_channels(out,ch,cycle1,ret);
    counter_after=sis3800_readCounter(sis3800_handle,30);
    counters[0] = counter_after;
    counters[1] = sis3800_readCounter(sis3800_handle,31);
    //    dump_channels(out,counters,cycle1,2);
    if(counter_after>counter_before)
      lost+=(counter_after-counter_before-1);
    printf("%llu, %llu, %llu (before:%d,after:%d, lost:%lld) end acquire %f ms\n",loop,cycle0,cycle1,counter_before,counter_after,lost,1.0*(getUsTime()-loop_time_start)/1000.0);

    //    caen513_reset(caen513_handle);
    //    caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF
    //caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF


    
    loop++;
  }
  
  CLOSEDEV(caen513);
  CLOSEDEV(caen965);
  CLOSEDEV(caen792);
  CLOSEDEV(sis3800);
  
  return 0;
}
