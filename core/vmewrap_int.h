/*
 * vmewrap_int.h
 *
 *  Created on: Mar 10, 2016
 *      Author: michelo
 */

#ifndef CORE_VMEWRAP_INT_H_
#define CORE_VMEWRAP_INT_H_

#include "vmewrap.h"


typedef struct __vme_handle__ {

  void* bus;
  void* handle;
  void* mapped_address;
  uint64_t phys_add;
  void* priv;//private to the specific driver
  uint32_t size;
  int fd;
  int master;
  vme_driver_t type;
  int (*vme_init)(struct __vme_handle__ *  handle);
  int (*map_master)(struct __vme_handle__ * handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options);
  int (*map_slave)(struct __vme_handle__ * handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options);
  int (*vme_write32)(struct __vme_handle__ * handle,unsigned off,uint32_t* data,int sizen);
  int (*vme_set_reg)(struct __vme_handle__ * handle,unsigned off,void* data,int sizeb);
  int (*vme_clr_reg)(struct __vme_handle__ * handle,unsigned off,void* data,int sizeb);

  int (*vme_write16)(struct __vme_handle__ *  handle,unsigned off,uint16_t* data,int sizen);
  int (*vme_write8)(struct __vme_handle__ *  handle,unsigned off,uint8_t* data,int sizen);
  int (*vme_read32)(struct __vme_handle__ *  handle,unsigned off,uint32_t *data,int sizen);
  int (*vme_read16)(struct __vme_handle__ *  handle,unsigned off,uint16_t *data,int sizen);
  int (*vme_read8)(struct __vme_handle__ * handle,unsigned off,uint8_t *data,int sizen);
  int (*vme_close)(struct __vme_handle__ *  handle);
  int (*vme_interrupt_enable)(struct __vme_handle__ *  handle,int level, int signature,int type,void*priv);
  int (*vme_interrupt_disable)(struct __vme_handle__ *  handle);
  int (*vme_wait_interrupt)(struct __vme_handle__ *  handle,int timeo_ms);

} * vmewrap_int_vme_handle_t;



#endif /* CORE_VMEWRAP_INT_H_ */
