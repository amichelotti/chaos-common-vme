#include <common/vme/sis/sis3800_drv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <conffile> [init]\n",argv[0]);

#define CLOSEDEV(_x) \
  _x ## _close(_x ## _handle);

#define OPENDEV(_x)						\
  if(_x ## _addr && (_x ## _handle = _x ## _open(_x ## _addr))){ \
    printf("* " # _x " successfully mapped\n");\
  } else {\
      printf("## cannot map " # _x " address 0x%x\n",_x ## _addr);\
    return -2;\
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

  sis3800_handle_t sis3800_handle=NULL;
  int32_t pio;
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



  OPENDEV(sis3800);
  if(argc==3){
    printf("* initialising...\n");
    sis3800_init(sis3800_handle);
  }
  for(cnt=0;cnt<32;cnt++){
    uint32_t data=sis3800_readCounter(sis3800_handle,cnt);
    printf("counter[%d]=%d\n",cnt,data);
  }

  CLOSEDEV(sis3800);
  
  return 0;
}
