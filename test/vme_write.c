
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vmewrap/vmewrap.h>
#define USAGE printf("%s <vme base address> <addressing 16/32> <address off> <data0> [data1] ...\n",argv[0]);

int main(int argc,char**argv){
  unsigned long address=0;
  unsigned addressing=32;
  unsigned off=0;
  int size=4;
  vmewrap_vme_handle_t handle;
  unsigned* data=0;
  int cnt;
  int ndati;
  void*ptr;
    FILE*out;
  if(argc<5){
    USAGE;
    return 1;
  }
  address=strtoul(argv[1],0,0);
  addressing=strtoul(argv[2],0,0);
  off=strtoul(argv[3],0,0);
  ndati=(argc-4);
  data= (unsigned*)malloc(ndati*sizeof(unsigned));
  for(cnt=0;cnt<ndati;cnt++){
    data[cnt]=strtoul(argv[4+cnt],0,0);
  }
  handle= vmewrap_vme_open(address,0x1000000,addressing,0,0,0);
  if(handle==NULL){
    printf("## cannot map address 0x%x\n",address);
    return -1;
  }
  ptr=vmewrap_get_vme_master_linux_add(handle);
  if(ptr==NULL){
      printf("## cannot remap address 0x%x\n",address);
    return -1;
  }
  for( cnt=0;cnt<ndati;cnt++){
    int add;
    if(addressing==16){
      add=off+(cnt<<1);
      WRITE16(ptr,add,data[cnt]);
      printf("WRITE16[0x%x]=0x%x\n",add,data[cnt]);
    } else {
      add=off+(cnt<<2);
      WRITE32(ptr,add,data[cnt]);
      printf("WRITE32[0x%x]=0x%x\n",add,data[cnt]);
    }
  }
  return 0;
}
