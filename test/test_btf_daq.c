#include "caen965_drv.h"
#include "caen792_drv.h"
#include "caen513_drv.h"
#include "sis3800_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <conffile> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);

#define CLOSEDEV(_x) \
  _x ## close(_x ## _handle);

#define OPENDEV(_x)						\
  if(_x ## _addr && (_x ## _handle = _x ## _open(_x ## _addr))){ \
    printf("* " # _x " successfully mapped\n");\
  } else {\
      printf("## cannot map " # _x " address 0x%x\n",_x ## _addr);\
    return -2;\
  }

void dump_channels(FILE*o,uint32_t *chan,uint64_t cycle,int channels){
    int cnt;
    fprintf(o,"\n");
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
  uint32_t low[16],hi[16],ch[32],counters[32];
  uint32_t caen513_addr=0,caen965_addr=0,caen792_addr=0,sis3800_addr=0;
  uint64_t cycle0=0,cycle1=0,loop_time_start=0;
  caen965_handle_t caen965_handle=NULL;
  caen792_handle_t caen792_handle=NULL;
  caen513_handle_t caen513_handle=NULL;
  sis3800_handle_t sis3800_handle=NULL;

  int is965=1;
  int cnt;
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
    if(fscanf(fconf_file,"pio:%x",&caen513_addr)==1){
      printf("* pio(caen 513) address 0x%x\n",caen513_addr);
    }
    if(fscanf(fconf_file,"scaler:%x",&sis3800_addr)==1){
      printf("* scaler (sis3800) address 0x%x\n",sis3800_addr);
    }
    if(fscanf(fconf_file,"qdc965:%x",&caen965_addr)==1){
      printf("* qdc965 (caen965) address 0x%x\n",caen965_addr);
    }
    if(fscanf(fconf_file,"qdc792:%x",&caen792_addr)==1){
      printf("* qdc792 (caen792) address 0x%x\n",caen792_addr);
    }
  }
  fclose(fconf_file);
  out=fopen("btf_daq.txt","w");
  OPENDEV(caen513);
  OPENDEV(caen965);
  OPENDEV(caen792);
  OPENDEV(sis3800);
  caen513_init(caen513_handle,V513_CHANMODE_OUTPUT); //default all output
  caen965_init(caen965_handle,0,1);
  caen792_init(caen792_handle,0,1);
  sis3800_init(sis3800_handle);
  caen513_setChannelMode(caen513_handle,15, V513_CHANMODE_INPUT); // trigger in
  caen513_setChannelMode(caen513_handle,0, V513_CHANMODE_OUTPUT); // veto out

  while(1){
    loop_time_start=getUsTime();
    while((caen513_get(caen513_handle)&0x8000)==0); // wait trigger 
    caen513_set(caen513_handle,1); //assert veto
    ret = caen965_acquire_channels_poll(caen965_handle,low,hi,0,16,&cycle0,0);
    if(ret){
      dump_channels(out,low,cycle0,ret);
      dump_channels(out,hi,cycle0,ret);
    }
    ret = caen792_acquire_channels_poll(caen792_handle,ch,0,32,&cycle1,0);
    if(ret){
      dump_channels(out,ch,cycle1,ret);
    }
    counters[0] = sis3800_readCounter(sis3800_handle,30);
    counters[1] = sis3800_readCounter(sis3800_handle,31);
    dump_channels(out,counters,cycle1,2);
    printf("%llu, %llu end acquire %llu us\n",cycle0,cycle1,getUsTime()-loop_time_start);
    caen513_set(caen513_handle,0); //de-assert veto
  }
  
  CLOSEDEV(caen513);
  CLOSEDEV(caen965);
  CLOSEDEV(caen792);
  CLOSEDEV(sis3800);
  
  return 0;
}
