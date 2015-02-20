#include "ddlib.h"
#include "hiadc_drv.h"
#include <stdlib.h>
#define USAGE printf("%s <master address> <slave address>\n",v[0]);
int main(int c,char**v){
  dd_vme_handle_t handle;
  uint32_t master_add,slave_add;
  uint32_t master_mapped;
  if(c!=3){
    USAGE;
    return -1;
  }
  master_add = strtoul(v[1],0,0);
  slave_add = strtoul(v[2],0,0);

  handle= dd_vme_open(master_add,0x1000000,16,0,0,0);
  if(handle){
    long to,or,i;
    unsigned short arr[16];
    unsigned long cycle=0;

    master_mapped = dd_get_vme_master_linux_add(handle);
    hiadc_init(master_mapped);
    do{
      hiadc_acquire(master_mapped,arr,1000,&to,&or);
      cycle++;
      printf("[%llu] ==== cycle %d ====\n",getUsTime(),cycle);
      for(i=0;i<16;i++){
	printf("%.4x ",arr[i]);
      }
      printf("\n");
    } while(to == 0 && or==0);
      dd_vme_close(handle);
  }
  
    return 0;
}
