/*
 * @file vmewrap.h
@author Andrea Michelotti
@date Mar 11, 2016
 * */
#ifndef _VMEWRAP_H_
#define _VMEWRAP_H_
#include <stdint.h>

#define REG32(base,x) *((volatile uint32_t*)(((char*)base)+x))
#define REG16(base,x) *((volatile uint16_t*)(((char*)base)+x))
#define REG8(base,x) *((volatile uint8_t*)(((char*)base)+x))



#define VME_READ16(handle,off,data) vmewrap_read16(handle,(off),data,1)
#define VME_READ32(handle,off,data) vmewrap_read32(handle,(off),data,1)
#define VME_READ8(handle,off,data) vmewrap_read8(handle,(off),data,1)

#define VME_READ_REG16(handle,off) vmewrap_read_reg16(handle,(off))
#define VME_READ_REG32(handle,off) vmewrap_read_reg32(handle,(off))
#define VME_READ_REG8(handle,off) vmewrap_read_reg8(handle,(off))

#define VME_WRITE16(handle,off,val) vmewrap_write_reg16(handle,off,val)
#define VME_WRITE32(handle,off,val) vmewrap_write_reg32(handle,off,val)
#define VME_WRITE8(handle,off,val) vmewrap_write_reg8(handle,off,val)

#define VME_WRITE_REG16(handle,off,val) vmewrap_write16(handle,off,val)
#define VME_WRITE_REG32(handle,off,val)  vmewrap_write32(handle,off,val)
#define VME_WRITE_REG8(handle,off,val) vmewrap_write8(handle,off,val)


#define VME_OPT_CTL_EN		0x80000000
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
/*
#define VME_SET_MASTER 0 // enable master
#define VME_SET_SLAVE  1 // enable slave

#define VME_SET_DMA    9 //enable DMA
*/

#ifdef __cplusplus
extern "C" {
#endif
typedef void* vmewrap_vme_handle_t;
/**
   @return us time
 */
//unsigned long long getUsTime();

typedef enum vme_driver {
	VME_UNIVERSE2_DRIVER,
	VME_UNIVERSE_DRIVER,
	VME_CAEN1718_DRIVER,
	VME_CAEN2718_DRIVER,
	VME_LINUX_USER
} vme_driver_t;
/*
 * Initialize the appropriate driver
 * @param driver the identifier of the driver
 * @return a valid handle on success, zero otherwise
 */
vmewrap_vme_handle_t vmewrap_init_driver(vme_driver_t driver);

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
	@return zero if success
*/
  int vmewrap_vme_open_master(vmewrap_vme_handle_t handle,uint32_t master_add,uint32_t master_size,vme_addressing_t master_addressing,vme_access_t dw, vme_opt_t vme_options);


/**
	open vme master and slave space
	@param handle vme handle
	@param slave_add the vme slave address 
	@param slave_size (0 = no map)
 	@param slave_addressing (16, 32)
	@param dw data width (8,16, 32,64)
	@param vme_options (supervisor...)
	@return zero if success
*/
  int vmewrap_vme_open_slave(vmewrap_vme_handle_t handle,uint32_t slave_add,uint32_t slave_size,vme_addressing_t master_addressing,vme_access_t dw, vme_opt_t vme_options);


/**
	close vme master and slave space
	@param hand
	@return zero if success
*/
int vmewrap_vme_close(vmewrap_vme_handle_t  handle);

/**
   
	@param handle
	@return the mapped address
*/
  void* vmewrap_get_linux_add(vmewrap_vme_handle_t  handle);


  /**
   * interrupt enable
   * @param handle handle to vme
   * @param level interrupt to be sensible
   * @param signature to be enable
   * @param type (optional) specify additional parameters
   * @param priv (optional) private structure
	@return 0 on success
   */
   int vmewrap_interrupt_enable(vmewrap_vme_handle_t  handle,int level, int signature,int type,void*priv);

   /**
      * interrupt disable
   		@return 0 on success
      */

   int vmewrap_interrupt_disable(vmewrap_vme_handle_t  handle);
   /**
       * wait for interrupt
       * @param timeo_ms timeout in ms (0 indefinite wait)
       * @return 0 on success
   */
   int vmewrap_wait_interrupt(vmewrap_vme_handle_t  handle,int timeo_ms);

   int vmewrap_write32(vmewrap_vme_handle_t  handle,unsigned off,uint32_t* data,int sizen);
   int vmewrap_write16(vmewrap_vme_handle_t  handle,unsigned off,uint16_t* data,int sizen);
   int vmewrap_write8(vmewrap_vme_handle_t  handle,unsigned off,uint8_t* data,int sizen);
   int vmewrap_read32(vmewrap_vme_handle_t  handle,unsigned off,uint32_t* data,int sizen);
   int vmewrap_read16(vmewrap_vme_handle_t  handle,unsigned off,uint16_t* data,int sizen);
   int vmewrap_read8(vmewrap_vme_handle_t  handle,unsigned off,uint8_t* data,int sizen);

   uint32_t vmewrap_read_reg32(vmewrap_vme_handle_t  handle,unsigned off);
   uint16_t vmewrap_read_reg16(vmewrap_vme_handle_t  handle,unsigned off);
   uint8_t vmewrap_read_reg8(vmewrap_vme_handle_t  handle,unsigned off);

   int vmewrap_write_reg32(vmewrap_vme_handle_t  handle,uint32_t data, unsigned off);
   int vmewrap_set_reg(vmewrap_vme_handle_t  handle,void* data, unsigned off,int size);
   int vmewrap_clr_reg(vmewrap_vme_handle_t  handle,void* data, unsigned off,int size);

   int vmewrap_write_reg16(vmewrap_vme_handle_t  handle,uint16_t data,unsigned off);
   int vmewrap_write_reg8(vmewrap_vme_handle_t  handle,uint8_t data,unsigned off);
   int vmewrap_getFD(vmewrap_vme_handle_t  handle);
   int vmewrap_setFD(vmewrap_vme_handle_t  handle,int fd);

#ifdef __cplusplus

}
#endif
#endif


