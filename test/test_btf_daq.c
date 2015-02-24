#include "caen965_drv.h"
#include "caen792_drv.h"
#include "caen513_drv.h"
#include "sis3800_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <conffile> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);

#define OPENDEV(_x)						\
  if(_x ## _addr && (handle_## _x= _x ## _open(_x ## _addr))){ \
    printf("* " # _x " successfully mapped\n");\
  } else {\
      printf("## cannot map " # _x " address 0x%x\n",_x ## _addr);\
    return -2;\
  }

void dump_channels(FILE*o,uint32_t *chan,uint64_t cycle,int channels){
    int cnt;
    for(cnt=0;cnt<channels;cnt++){
        fprintf(o,"- %lld [%d] = 0x%x\n",cycle,cnt,chan[cnt]);
    }
}

int main(int argc,char**argv){
  unsigned long address=0;
  void* caen;
  char*conf_file;
  FILE* fconf_file;
  uint32_t acquire_cycles=0;
  uint32_t acquire_timeo=0;
  uint32_t low[16],hi[16],ch[32],counters[32];
  uint32_t caen513_addr=0,caen965_addr=0,caen792_addr=0,sis3800_addr=0;
  uint64_t cycle=0;
  caen965_handle_t handle_caen965=NULL;
  caen792_handle_t handle_caen792=NULL;
  caen513_handle_t handle_caen513=NULL;
  sis3800_handle_t handle_sis3800=NULL;

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
  OPENDEV(caen513);
  OPENDEV(caen965);
  OPENDEV(caen792);
  OPENDEV(sis3800);
  /*
    if(caen513_addr && (handle_pio=caen513_open(caen513_addr))){
    printf("* pio successfully mapped\n");
    } else {
    printf("## cannot map pio address 0x%x\n",caen513_addr);
    return -2;
    }
    if(caen965_addr && (handle_qdc9=caen965_open(caen965_addr))){
    printf("* qdc965 successfully mapped\n");
    } else {
    printf("## cannot map pio address 0x%x\n",caen965_addr);
    return -2;
    }
  */
  return 0;
}
