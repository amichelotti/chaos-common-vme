
#include <common/vme/caen/caen965_drv.h>
#include <common/vme/caen/caen792_drv.h>
#include <common/vme/caen/caen513_drv.h>
#include <common/vme/sis/sis3800_drv.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USAGE printf("%s <conffile> [#acquire cycles] [## timeout acquiring ms ]\n",argv[0]);

#define ENABLE_VETO 0x7
#define DISABLE_VETO 0x0

#define CLOSEDEV(_x) \
  _x ## close(_x ## _handle);

#define OPENDEV(_x)						\
  if(_x ## _addr && (_x ## _handle = _x ## _open(VME_UNIVERSE2_DRIVER,_x ## _addr))){ \
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

// the PIO doesn't set the line reliably
// an output is feedbacked into the input to be sure is reset
static void resetTM(caen513_handle_t caen513_handle){
  unsigned pio;
  int timeo_counter=0;
    
  caen513_clear(caen513_handle);
  caen513_set(caen513_handle,0x7); // reset veto flip flop 
  while(((pio=caen513_get(caen513_handle))&0x4000)==0){
    //caen513_clear(caen513_handle);

    if(timeo_counter%1000 == 0){
      printf("waiting ACK 1 :0x%x\n",pio);
    }
    timeo_counter++;
    caen513_set(caen513_handle,0x7); // reset veto flip flop 
  }
  caen513_clear(caen513_handle);
  caen513_set(caen513_handle,DISABLE_VETO); // reset veto flip flop 
  timeo_counter=0;
  while(((pio=caen513_get(caen513_handle))&0x4000)!=0){

    if(timeo_counter%1000 == 0){
      printf("waiting ACK 0 :0x%x\n",pio);
    }
    timeo_counter++;
    caen513_set(caen513_handle,0x0); // reset veto flip flop 
  }
}

int main(int argc,char**argv){
  
  char*conf_file;
  FILE* fconf_file;
  int ret;
  
  int timeo_counter=0;
  uint32_t low[16],hi[16],ch[32],counters[32];
  uint32_t caen513_addr=0,caen965_addr=0,caen792_addr=0,sis3800_addr=0;
  uint64_t loop=0,cycle0=0,cycle1=0,loop_time_start=0;
  caen965_handle_t caen965_handle=NULL;
  caen792_handle_t caen792_handle=NULL;
  caen513_handle_t caen513_handle=NULL;
  sis3800_handle_t sis3800_handle=NULL;
  int32_t pio;
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
  out=fopen("btf_daq.txt","w");

  OPENDEV(caen965);
  OPENDEV(caen792);
  OPENDEV(sis3800);
  OPENDEV(caen513);
  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); 
  //caen513_init(caen513_handle,1); //use board defaults
  
  for(cnt=8;cnt<16;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); // 15 trigger in
  
  for(cnt=0;cnt<8;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT); 

#ifdef CHECK_PIO
  caen513_setChannelMode(caen513_handle,14, V513_CHANMODE_NEG|V513_CHANMODE_INORMAL|V513_CHANMODE_INPUT); // 14 feedback reset
#endif


#ifdef HW_VETO

  caen513_set(caen513_handle,DISABLE_VETO); // reset veto flip flop 
#warning "HW TRIGGER 2 TM"
#else
  caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF
#endif
  caen965_init(caen965_handle,0,1);
  caen792_init(caen792_handle,0,1);
  sis3800_init(sis3800_handle);

  caen513_clear(caen513_handle);
  //resetTM(caen513_handle);
  
  while(1){
    loop_time_start=getUsTime();
    while(((pio=caen513_get(caen513_handle))&0x8000)==0){

      if((timeo_counter%100000) == 0){
	printf("waiting trigger pio:0x%x\n",pio);
#ifdef CHECK_PIO
	caen513_set(caen513_handle,0x0); // reset veto flip flop 
#endif
	
      }
      timeo_counter++;
    } // wait trigger 

#ifndef HW_VETO
    caen513_set(caen513_handle,ENABLE_VETO); //assert veto

#endif
    //    caen513_clear(caen513_handle);
    
    pio=caen513_get(caen513_handle);
    printf("start acquire 0x%x 965\n",pio);
    ret = caen965_acquire_channels_poll(caen965_handle,low,hi,0,16,&cycle0,0);
    
    dump_channels(out,low,cycle0,ret);
    dump_channels(out,hi,cycle0,ret);
    pio=caen513_get(caen513_handle);
    printf("start acquire 0x%x 792\n",pio);
    ret = caen792_acquire_channels_poll(caen792_handle,ch,0,16,&cycle1,0);
    
    dump_channels(out,ch,cycle1,ret);
    
    printf("start acquire 0x%x sis\n",pio);
    counters[0] = sis3800_readCounter(sis3800_handle,30);
    counters[1] = sis3800_readCounter(sis3800_handle,31);
    dump_channels(out,counters,cycle1,2);
    printf("%llu, %llu, %llu (%d,%d) end acquire %f ms\n",loop,cycle0,cycle1,counters[0],counters[1],1.0*(getUsTime()-loop_time_start)/1000.0);

#ifdef HW_VETO

    caen513_set(caen513_handle,0x7); // reset veto flip flop 
    caen513_set(caen513_handle,0x7); // reset veto flip flop 

    caen513_set(caen513_handle,0x0); //
    caen513_set(caen513_handle,0x0); //
#else
    caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF
#endif

    //resetTM(caen513_handle);
    loop++;
  }
  
  CLOSEDEV(caen513);
  CLOSEDEV(caen965);
  CLOSEDEV(caen792);
  CLOSEDEV(sis3800);
  
  return 0;
}
