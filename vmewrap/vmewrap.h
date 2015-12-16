#ifndef _VMEWRAP_H_
#define _VMEWRAP_H_


#define REG32(base,x) *((volatile uint32_t*)(((char*)base)+x))
#define REG16(base,x) *((volatile uint16_t*)(((char*)base)+x))
#define REG8(base,x) *((volatile uint8_t*)(((char*)base)+x))
#define READ16(base,off) REG16(base,off)
#define READ32(base,off) REG32(base,off)
#define READ8(base,off) REG8(base,off)

#define WRITE16(base,off,val) REG16(base,off)=val
#define WRITE32(base,off,val) REG32(base,off)=val
#define WRITE8(base,off,val) REG8(base,off)=val

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
	@param slave_add the vme slave address 
	@param slave_size (0 = no map)
 	@param slave_addressing (16, 32)
	@return an handle or zero if error
*/
vmewrap_vme_handle_t vmewrap_vme_open(uint32_t master_add,uint32_t master_size,uint32_t master_addressing,uint32_t slave_add,uint32_t slave_size,uint32_t slave_addressing);


/**
	close vme master and slave space
	@param hand
	@return zero if success
*/
int32_t vmewrap_vme_close(vmewrap_vme_handle_t  handle);

/**
   
	@param handle
	@return the master mapped address
*/
void* vmewrap_get_vme_master_linux_add(vmewrap_vme_handle_t  handle);

/**
   
	@param handle
	@return the slave mapped address
*/
void* vmewrap_get_vme_slave_linux_add(vmewrap_vme_handle_t  handle);
#ifdef __cplusplus

}
#endif
#endif


