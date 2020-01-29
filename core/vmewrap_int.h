/*
 * vmewrap_int.h
 *
 *  Created on: Mar 10, 2016
 *      Author: michelo
 */

#ifndef CORE_VMEWRAP_INT_H_
#define CORE_VMEWRAP_INT_H_
#include <stdint.h>
#include <string>
#define VMEMAXWIN 64


typedef enum vme_driver {
	VME_UNIVERSE2_DRIVER,
	VME_UNIVERSE_DRIVER,
	VME_CAEN1718_DRIVER,
	VME_CAEN2718_DRIVER,
	VME_LINUX_USER_DRIVER,
  VME_SIS3153_ETH_DRIVER,
  VME_UKNOWN_DRIVER
} vme_driver_t;

#define VME_UNIVERSE2 "vme_universe2"
#define VME_UNIVERSE "vme_universe"
#define VME_CAEN1718 "vme_caen1718"
#define VME_CAEN2718 "vme_caen2718"
#define VME_LINUX_USER "vme_linux_user"
#define VME_SIS3153_ETH "vme_sis3153_eth"
#define VME_UKNOWN "UKNOWN VME DRIVER"
typedef enum vme_opt {
  VME_OPT_AM_PROG_AM=         0x1, //program access
  VME_OPT_AM_DATA_AM=         0x2, //data access
  VME_OPT_AM_SUPER_AM=        0x4, // supervisor mode
  VME_OPT_AM_USER_AM=     0x8, // not privileged mode
  VME_OPT_BLT_ON=          0x10, // block transfer on
  VME_OPT_BLT_OFF=         0x20, // block transfer off
  VME_OPT_POST_WRITE_EN=   0x40, // post write cycle enable
  VME_OPT_POST_WRITE_DIS=  0x80, // post write cycle disable 
  VME_OPT_PREF_READ_EN =   0x100, // prefetched read cycle enable
  VME_OPT_PREF_READ_DIS=   0x200, // prefetched write cycle disable
  VME_OPT_MBLT_ON=          0x400, // multi block transfer on
  VME_OPT_CBLT_ON=          0x800 // multi chained block transfer on


} vme_opt_t;

typedef enum vme_access {
  VME_ACCESS_D64=    64, //enable 64/32/16/8 bus cycles
  VME_ACCESS_D32=    32, //enable 32/16/8 bus cycles
  VME_ACCESS_D16=    16, // enable 16/8 bus cycles
  VME_ACCESS_D8=     8 // enable 8 bus cycles
} vme_access_t;

typedef enum vme_addressing{
  VME_ADDRESSING_A32=    32, // 32 bit addressing
  VME_ADDRESSING_A24=    24, // 24 bit addressing
  VME_ADDRESSING_A16 =   16 // 16 bit addressing
} vme_addressing_t;

struct __vmewrap_handle__;
typedef struct __vme_window__ {
  struct __vmewrap_handle__*parent;
  vme_addressing_t addressing;
	vme_access_t access;
	vme_opt_t opt;
	uint32_t add;
	uint32_t size;

  int fd; // file descriptor if needed
  int index; // index in the mapped window
  int master; // is a slave or master window
  void* mapped_address; // mapped address
  uint64_t phys_add; //pysical address
  void* priv;
  __vme_window__():parent(NULL),fd(-1),index(0),master(-1),mapped_address(0),phys_add(0),priv(NULL){}
} * vmewrap_window_t;

typedef struct __vmewrap_handle__ {

  void* bus;
  void* priv;//private to the specific driver
  int fd;
  std::string type;
  int nwindow;
  vmewrap_window_t window[VMEMAXWIN];
  int (*vme_init)(struct __vmewrap_handle__ *  handle);
  int (*map_master)(struct __vmewrap_handle__ * handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options);
  int (*map_close)(struct __vme_window__ * handle);

  int (*map_slave)(struct __vmewrap_handle__ * handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options);
  int (*vme_write32)(struct __vme_window__ * handle,unsigned off,uint32_t* data,int sizen);
  int (*vme_set_reg)(struct __vme_window__ * handle,unsigned off,void* data,int sizeb);
  int (*vme_clr_reg)(struct __vme_window__ * handle,unsigned off,void* data,int sizeb);

  int (*vme_write16)(struct __vme_window__ *  handle,unsigned off,uint16_t* data,int sizen);
  int (*vme_write8)(struct __vme_window__ *  handle,unsigned off,uint8_t* data,int sizen);
  int (*vme_read32)(struct __vme_window__ *  handle,unsigned off,uint32_t *data,int sizen);
  int (*vme_read16)(struct __vme_window__ *  handle,unsigned off,uint16_t *data,int sizen);
  int (*vme_read8)(struct __vme_window__ * handle,unsigned off,uint8_t *data,int sizen);
  int (*vme_interrupt_enable)(struct __vme_window__ *  handle,int level, int signature,int type,void*priv);
  int (*vme_interrupt_disable)(struct __vme_window__ *  handle);
  int (*vme_wait_interrupt)(struct __vme_window__ *  handle,int timeo_ms);
__vmewrap_handle__():bus(NULL),priv(NULL),fd(-1),nwindow(0),vme_init(NULL),map_master(NULL),map_close(NULL),map_slave(NULL),vme_interrupt_enable(NULL),vme_interrupt_disable(NULL),vme_wait_interrupt(NULL){
  for(int cnt=0;cnt<VMEMAXWIN;cnt++)window[cnt]=NULL;
}
} * vmewrap_vme_handle_t;

#endif /* CORE_VMEWRAP_INT_H_ */
