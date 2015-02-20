#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG
#define DPRINT(str,ARGS...) printf("[%llu] %s:" str,getUsTime(), __FUNCTION__, ##ARGS)
#else
#define DPRINT(str,ARGS...) 
#endif
#define PRINT(str,ARGS...) printf("*" str,##ARGS)
#define TPRINT(str,ARGS...) printf("[%llu]" str,getUsTime(),##ARGS)

#define ERR(str,ARGS...) printf("[%llu]# %s:" str,getUsTime(),__FUNCTION__,##ARGS)

#define REG32(base,x) *((volatile uint32_t*)(base+x))
#define REG16(base,x) *((volatile uint16_t*)(base+x))
#define REG8(base,x) *((volatile uint8_t*)(base+x))
#define READ16(base,off) REG16(base,off)
#define READ32(base,off) REG32(base,off)
#define READ8(base,off) REG8(base,off)

#define WRITE16(base,off,val) REG16(base,off)=val
#define WRITE32(base,off,val) REG32(base,off)=val
#define WRITE8(base,off,val) REG8(base,off)=val


typedef void* dd_vme_handle_t;
/**
   @return us time
 */
unsigned long long getUsTime();



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
dd_vme_handle_t dd_vme_open(uint32_t master_add,uint32_t master_size,uint32_t master_addressing,uint32_t slave_add,uint32_t slave_size,uint32_t slave_addressing);


/**
	close vme master and slave space
	@param hand
	@return zero if success
*/
int32_t dd_vme_close(dd_vme_handle_t  handle);

/**
   
	@param handle
	@return the master mapped address
*/
uint32_t dd_get_vme_master_linux_add(dd_vme_handle_t  handle);

/**
   
	@param handle
	@return the slave mapped address
*/
uint32_t dd_get_vme_slave_linux_add(dd_vme_handle_t  handle);



