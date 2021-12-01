/*
 * @file vmewrap.h
@author Andrea Michelotti
@date Mar 11, 2016
 * */
#ifndef _VMEWRAP_H_
#define _VMEWRAP_H_
#include <stdio.h>
#include <stdint.h>
#include "vmewrap_int.h"

#define MM_REG32(base,x) *((volatile uint32_t*)(((char*)base)+x))
#define MM_REG16(base,x) *((volatile uint16_t*)(((char*)base)+x))
#define MM_REG8(base,x) *((volatile uint8_t*)(((char*)base)+x))


#define VME_READ16(handle,off,data) vmewrap_read16(handle,(off),data,1)
#define VME_READ32(handle,off,data) vmewrap_read32(handle,(off),data,1)
#define VME_READ8(handle,off,data) vmewrap_read8(handle,(off),data,1)

#define VME_READ_REG16(handle,off) vmewrap_read_reg16(handle,(off))
#define VME_READ_REG32(handle,off) vmewrap_read_reg32(handle,(off))
#define VME_READ_REG8(handle,off) vmewrap_read_reg8(handle,(off))

#define VME_WRITE16(handle,off,val) vmewrap_write_reg16(handle,val,off)
#define VME_WRITE32(handle,off,val) vmewrap_write_reg32(handle,val,off)
#define VME_WRITE8(handle,off,val) vmewrap_write_reg8(handle,val,off)

#define VME_WRITE_REG16(handle,off,val) {uint16_t tmp=val;vmewrap_write16(handle,off,&tmp,1);}
#define VME_WRITE_REG32(handle,off,val) {uint32_t tmp=val; vmewrap_write32(handle,off,&tmp,1);}
#define VME_WRITE_REG8(handle,off,val) {uint8_t tmp=val;vmewrap_write8(handle,off,&tmp,1);}

#define VME_OPT_CTL_EN		0x80000000

#ifdef CHAOS
#include <common/debug/core/debug.h>
#else
#include <stdio.h>
#define DPRINT(str,...) printf("\033[38;5;148m%s\033[39m :" str "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__)
#define DERR(str,...) printf("# \033[38;5;148m%s\033[39m :" str "\n",__PRETTY_FUNCTION__,##__VA_ARGS__)
#define ERR DERR
#define PRINT DPRINT
#endif
/*
#define VME_SET_MASTER 0 // enable master
#define VME_SET_SLAVE  1 // enable slave

#define VME_SET_DMA    9 //enable DMA
*/



/**
   @return us time
 */
//unsigned long long getUsTime();


/*
 * Initialize the appropriate driver
 * @param driver the identifier of the driver
 * @param params specific parameters
 * @return a valid handle on success, zero otherwise
 */
vmewrap_vme_handle_t vmewrap_init_driver(vme_driver_t driver,void*params=0);


/*
 * Initialize the appropriate driver
 * @param driver the identifier of the driver
 * @return a valid handle on success, zero otherwise
 */
vmewrap_vme_handle_t vmewrap_init_driver(const char* drv_name,void*params=0);

/*
 * DeInitialize the appropriate driver
 * @param driver the handler of the driver
 * @return zero  on success otherwise
 */
int vmewrap_deinit_driver(vmewrap_vme_handle_t driver);
/**
	open vme master and slave space
	@param handle vme handle
	@param master_add the vme master address 
	@param master_size (0=no map)
	@param master_addressing (16, 32, 24)
	@param dw data width (8,16, 32,64)
	@param vme_options (supervisor...)
	@return zero if error, a valid window handle otherwise.
*/
  vmewrap_window_t vmewrap_vme_open_master(vmewrap_vme_handle_t handle,uint32_t master_add,uint32_t master_size,vme_addressing_t master_addressing,vme_access_t dw, vme_opt_t vme_options);


/**
	open vme master and slave space
	@param handle vme handle
	@param slave_add the vme slave address 
	@param slave_size (0 = no map)
 	@param slave_addressing (16, 32)
	@param dw data width (8,16, 32,64)
	@param vme_options (supervisor...)
	@return  zero if error, a valid window handle otherwise.
*/
  vmewrap_window_t vmewrap_vme_open_slave(vmewrap_vme_handle_t handle,uint32_t slave_add,uint32_t slave_size,vme_addressing_t master_addressing,vme_access_t dw, vme_opt_t vme_options);



/**
	close the specified window
	@param hand
	@return zero if success
*/
int vmewrap_vme_close(vmewrap_window_t  handle);
/**
   
	@param handle
	@return the mapped address
*/
  void* vmewrap_get_linux_add(vmewrap_window_t  handle);


  /**
   * interrupt enable
   * @param handle handle to vme
   * @param level interrupt to be sensible
   * @param signature to be enable
   * @param type (optional) specify additional parameters
   * @param priv (optional) private structure
	@return 0 on success
   */
   int vmewrap_interrupt_enable(vmewrap_window_t  handle,int level, int signature,int type,void*priv);

   /**
      * interrupt disable
   		@return 0 on success
      */

   int vmewrap_interrupt_disable(vmewrap_window_t  handle);
   /**
       * wait for interrupt
       * @param timeo_ms timeout in ms (0 indefinite wait)
       * @return 0 on success
   */
   int vmewrap_wait_interrupt(vmewrap_window_t  handle,int timeo_ms);

   int vmewrap_write32(vmewrap_window_t  handle,unsigned off,uint32_t* data,int sizen);
   int vmewrap_write16(vmewrap_window_t  handle,unsigned off,uint16_t* data,int sizen);
   int vmewrap_write8(vmewrap_window_t  handle,unsigned off,uint8_t* data,int sizen);
   int vmewrap_read32(vmewrap_window_t  handle,unsigned off,uint32_t* data,int sizen);
   int vmewrap_read16(vmewrap_window_t  handle,unsigned off,uint16_t* data,int sizen);
   int vmewrap_read8(vmewrap_window_t  handle,unsigned off,uint8_t* data,int sizen);

   uint32_t vmewrap_read_reg32(vmewrap_window_t  handle,unsigned off);
   uint16_t vmewrap_read_reg16(vmewrap_window_t  handle,unsigned off);
   uint8_t vmewrap_read_reg8(vmewrap_window_t  handle,unsigned off);

   int vmewrap_write_reg32(vmewrap_window_t  handle,uint32_t data, unsigned off);
   int vmewrap_set_reg(vmewrap_window_t  handle,void* data, unsigned off,int size);
   int vmewrap_clr_reg(vmewrap_window_t  handle,void* data, unsigned off,int size);

   int vmewrap_write_reg16(vmewrap_window_t  handle,uint16_t data,unsigned off);
   int vmewrap_write_reg8(vmewrap_window_t  handle,uint8_t data,unsigned off);
   int vmewrap_getFD(vmewrap_window_t  handle);
   int vmewrap_setFD(vmewrap_window_t  handle,int fd);

  vme_driver_t driverNameToId(const std::string& name);
  std::string idToDriverName(vme_driver_t name);


#define REG32(base,x) vmewrap_read_reg32(base,x)
#define REG16(base,x) vmewrap_read_reg16(base,x)
#define REG8(base,x) vmewrap_read_reg8(base,x)

#endif


