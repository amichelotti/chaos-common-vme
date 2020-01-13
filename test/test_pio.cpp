#include <common/vme/caen/caen513_drv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <conffile> <output mask> [write data]\n",argv[0]);



#define CLOSEDEV(_x) \
  _x ## _close(_x ## _handle);

#define OPENDEV(_x)						\
  if(_x ## _addr && (_x ## _handle = _x ## _open(VME_UNIVERSE2_DRIVER,_x ## _addr))){ \
    printf("* " # _x " successfully mapped\n");\
  } else {\
      printf("## cannot map " # _x " address 0x%x\n",_x ## _addr);\
    return -2;\
  }


int main(int argc,char**argv){
 
  char*conf_file;
  FILE* fconf_file;
 
  uint32_t caen513_addr=0,caen965_addr=0,caen792_addr=0,sis3800_addr=0;
  caen513_handle_t caen513_handle=NULL;
  uint32_t write_mask=0x0;
  int32_t pio;
  int cnt;
  if(argc<3){
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

  write_mask=strtoul(argv[2],0,0);
  OPENDEV(caen513);
  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); 

  
  for(cnt=0;cnt<16;cnt++){
    if(write_mask&(1<<cnt)){
      //configure as output
      caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT); 
    } else {
      caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); 
    }

  }

  if(argc==4){
    // write
    uint32_t data=strtoul(argv[3],0,0);
    	caen513_set(caen513_handle,data);
	
  } else {
    //read

    pio=caen513_get(caen513_handle);
    
    printf("read=0x%x\n",pio);
  }
  CLOSEDEV(caen513);
    
  return 0;
}
