#ifndef __CAEN977_H__
#define __CAEN977_H__
/* 16 Channel I/O Register (Status A
 * Rev 1.0
 * Andrea Michelotti
 */
#include <stdint.h>
/* control register 0x26 */
#define V977_CTRL_REGISTER_MULTHIT_PATTM_MODE    0x1
#define V977_CTRL_REGISTER_GATE_MASKED    0x2
#define V977_CTRL_REGISTER_OUTPUT_MASKED    0x4
/* test control register 0x1A */

#define V977_TSTCTRL_REGISTER_CLEAR    0x1
/*does not produce output bit */
#define V977_TSTCTRL_REGISTER_MASK    0x2
/*does not send state to or logic*/
#define V977_TSTCTRL_REGISTER_ORMASK    0x4
/*interrupt mask register */
#define V977_TSTCTRL_REGISTER_INTMASK    0x8
/*status of of test channel */
#define V977_TSTCTRL_REGISTER_STATUS    0x10

#define NCHANNELS 16
#include <common/vme/core/vmewrap.h>

typedef void* caen977_handle_t;

/**
	open the caen977 device and return an handle to it
		@param vme_driver id of the driver vme to be used

	@param address the vme address of the board
	@return an handle or zero if error
*/
caen977_handle_t caen977_open(vme_driver_t vme_driver,uint32_t address);


/**
	close the caen977 device
	@param handle to the board
	@return zero if success
*/
int32_t caen977_close(caen977_handle_t handle);


/**
	initialize device and return
	@param h handle to the board
	@param defmode default mode
	@return zero if success, 
*/
int16_t caen977_init(caen977_handle_t h,uint16_t defmode);





/**
	get the operation mode of a given channel
	@param h handle to the board
	@return the operational mode,
*/

uint16_t caen977_getOperationMode(caen977_handle_t h);


/**
	interrupt mask register, mask the channels interrupt lines
	@param h handle to the board
	@param mask bitwise mask of channel interrupts
*/

void caen977_interruptMask(caen977_handle_t h,uint16_t mask);



/**
	Outputs channel to be masked
	@param h handle to the board
	@param mask bitwise mask
*/
void caen977_outputMask(caen977_handle_t h,uint16_t mask);

/**
	input channel to be masked
	@param h handle to the board
	@param mask bitwise mask
*/
void caen977_inputMask(caen977_handle_t h,uint16_t mask);

/**
	channel input read (level=0 => the input connector logic regardless the mask input, level=1 => read the single hit register, level=2 => read the second hit (if the operational mode is set accordly)
	@param h handle to the board
	@param level determines what input source to read (level=0 => the input connector logic regardless the mask input, level=1 => read the single hit register, level=2 => read the second hit (if the operational mode is set accordly)
	@param readClear if true for level 1 or 2 determines the reset of all flip flops that retains the data of that level
	@return the bitwised status of the channels
*/
uint16_t caen977_inputRead(caen977_handle_t h,uint16_t level, int readClear);

/**
	Outputs Set mask, set to 1 regardless the corresponding input connector
	@param h handle to the board
	@param mask set mask
*/
void caen977_outputSet(caen977_handle_t h,uint16_t mask);

/**
	clear all outputs
	@param h handle to the board

*/
void caen977_outputClear(caen977_handle_t h);

/**
	control the test input channel operation
	@param h handle to the board
	@param mode the test mode operation
*/
void caen977_testCtrlRegister(caen977_handle_t h,uint16_t mode);

/**
	reset the board
	@param h handle to the board

*/
void caen977_reset(caen977_handle_t h);


#endif
