
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vmewrap/vmewrap.h>
#define USAGE printf("%s <vme base address> <addressing 16/32> <address off> <read size>\n",argv[0]);

int main(int argc,char**argv){
  unsigned long address=0;
  unsigned addressing=32;
  unsigned off=0;
  int size=4;
  int cnt;
  vmewrap_vme_handle_t handle;
  void*ptr;
  FILE*out;
  if(argc<5){
    USAGE;
    return 1;
  }
  address=strtoul(argv[1],0,0);
  addressing=strtoul(argv[2],0,0);
  off=strtoul(argv[3],0,0);
  size=strtoul(argv[4],0,0);
  handle= vmewrap_vme_open_master(address,0x1000000,32,0);
  if(handle==NULL){
    printf("## cannot map address 0x%x\n",address);
    return -1;
  }
  ptr=vmewrap_get_linux_add(handle);
  if(ptr==NULL){
      printf("## cannot remap address 0x%x\n",address);
    return -1;
  }
  for(cnt=0;cnt<size;cnt++){
    int add;
    if(addressing==16){
      add=off+(cnt<<1);
      printf("READ16[0x%x]=0x%x\n",add,REG16(ptr,add));
    } else {
      add=off+(cnt<<2);
      printf("READ32[0x%x]=0x%x\n",add,REG32(ptr,add));
    }
  }
  vmewrap_vme_close(handle);
  return 0;
}
