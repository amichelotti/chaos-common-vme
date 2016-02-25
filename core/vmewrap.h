#ifndef _VMEWRAP_H_
#define _VMEWRAP_H_
#include <stdint.h>

#define REG32(base,x) *((volatile uint32_t*)(((char*)base)+x))
#define REG16(base,x) *((volatile uint16_t*)(((char*)base)+x))
#define REG8(base,x) *((volatile uint8_t*)(((char*)base)+x))

#define VME_READ16(handle,off,data) vmewrap_read16(handle,(off),data)
#define VME_READ32(handle,off,data) vmewrap_read32(handle,(off),data)
#define VME_READ8(handle,off,data) vmewrap_read8(handle,(off),data)

#define VME_WRITE16(handle,off,val) vmewrap_write16(handle,off,val)
#define VME_WRITE32(handle,off,val) vmewrap_write32(handle,off,val)
#define VME_WRITE8(handle,off,val) vmewrap_write8(handle,off,val)

#ifdef __cplusplus
extern "C" {
#endif
typedef void* vmewrap_vme_handle_t;
/**
   @return us time
 */
//unsigned long long getUsTime();



/**
	open vme master and slave space
	@param master_add the vme master address 
	@param master_size (0=no map)
	@param master_addressing (16, 32)
	@param vme_options (supervisor...)
	@return an handle or zero if error
*/
vmewrap_vme_handle_t vmewrap_vme_open_master(uint32_t master_add,uint32_t master_size,uint32_t master_addressing,uint32_t vme_options);


/**
	open vme master and slave space
	@param slave_add the vme slave address 
	@param slave_size (0 = no map)
 	@param slave_addressing (16, 32)
	@param vme_options (supervisor...)
	@return an handle or zero if error
*/
  vmewrap_vme_handle_t vmewrap_vme_open_slave(uint32_t slave_add,uint32_t slave_size,uint32_t slave_addressing,uint32_t vme_options);


/**
	close vme master and slave space
	@param hand
	@return zero if success
*/
int32_t vmewrap_vme_close(vmewrap_vme_handle_t  handle);

/**
   
	@param handle
	@return the mapped address
*/
  void* vmewrap_get_linux_add(vmewrap_vme_handle_t  handle);


  /**
   */
  int vmewrap_write32(vmewrap_vme_handle_t  handle,unsigned off,uint32_t data);
  int vmewrap_write16(vmewrap_vme_handle_t  handle,unsigned off,uint16_t data);
  int vmewrap_write8(vmewrap_vme_handle_t  handle,unsigned off,uint8_t data);
  int vmewrap_read32(vmewrap_vme_handle_t  handle,unsigned off,uint32_t* data);
  int vmewrap_read16(vmewrap_vme_handle_t  handle,unsigned off,uint16_t* data);
  int vmewrap_read8(vmewrap_vme_handle_t  handle,unsigned off,uint8_t* data);


#ifdef __cplusplus

}
#endif
#endif


