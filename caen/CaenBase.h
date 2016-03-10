/*
 * CaenBase.h
 *
 *  Created on: Mar 3, 2016
 *      Author: michelo
 */

#ifndef CAEN_CAENBASE_H_
#define CAEN_CAENBASE_H_
#define CAEN_GENERIC
#include "caen_common.h"
#include <string>
#include <common/vme/core/vmewrap.h>

namespace common{
namespace vme {
namespace caen {


class CaenBase {

protected:
	_caen_common_handle_t* handle;
	std::string board;
	int channels;

	//return the number of channels acquired;
	uint16_t searchEvent();
public:
	CaenBase();
	virtual ~CaenBase();
	/**
	 * Open the Caen Device, map the address
	 * @param vme_driver vme driver to use
	 * @param address vme address of the board
	 * @return 0 on success
	 * */
	int open(vme_driver_t vme_driver,uint64_t address);
	/**
	 * Close the caen device, unmap the address
	 * @return 0 on success
	 * */
	int close();
	/**
	 * Initialize the board
	 * @param crate_num sets a number to SW discriminate boards
	 **/
	void init(uint32_t crate_num,int hwreset);
	/**
	 * Set threashold of the channels
	 * @param chan channel number
	 * @param value value of the threashold
	 **/
	void setThreashold(uint16_t chan,uint16_t value);
	/**
	 * Get threashold of the channels
	 * @param chan channel number
	 * @return value of the threashold
	 **/
	uint16_t getThreashold(uint16_t chan);

	/*** Get board status

		@return value of the status
	 **/
	uint16_t getStatus();

	/*** Get buffer status

		@return value of the buffer status
	 **/
	uint16_t getBufferStatus();
	/**
	 * Get Event counter
	 * @param reset reset the event counter
	 */
	uint32_t getEventCounter(bool reset);
	/*
	 * Wait for an event for a given number of ms
	 * @param timeo_ms timeout in ms
	 * @return 0 if success
	 */
	virtual int32_t waitEvent(int timeo_ms);
	/**
		 * Acquire single event
		 * @param[out] channels output buffer
		 * @param[out] event tag
		 * @return number of acquired channels
		 */
	virtual uint16_t acquireChannels(uint32_t* channels,uint32_t *event);

	/**
	 * Acquire single event
	 * @param[out] channels output buffer
	 * @param[out] event tag
	 * @return number of acquired channels
	 */
	uint16_t acquireChannels(uint16_t* channels,uint32_t *event);
	/**
	 *  Acquire All the acquired events
	 * @param[out] buffer output buffer
	 * @param[int] max_size max buffer size
	 * @return acquired buffer size
	 */
	uint32_t acquireBuffer(uint32_t* buffer,uint32_t max_size);
	/*
	 * * Reset Event buffer
	 */
	void resetEventBuffer();
	/*
	 * Set operational mode
	 * @param mode bitfield of operational modes to enable
	 */
	void setMode(caen_modes_t mode);
	/*
	 * Clear operational mode
	 * 	 * @param mode bitfield of operational modes to disable
	 *
	 */
	void clrMode(caen_modes_t mode);
	/*
	 * Write to a specified offset
	 * @param off offset inside the caen device
	 * @param data dat to write
	 */

	void write(uint16_t off,uint16_t data);
	/*
		 * Write to a specified offset
		 * @param off offset inside the caen device
		 * @param data dat to write
		 */
	void write(uint16_t off,uint32_t data);
	/*
			 * read froma a specified offset
			 * @param off offset inside the caen device
			 * @param data reference to data
			 */
	void read(uint16_t off,uint16_t &data);
	/*
				 * read froma a specified offset
				 * @param off offset inside the caen device
				 * @param data reference to data
				 */
	void read(uint16_t off,uint32_t &data);

	uint16_t getNumberOfChannels(){return channels;}
	std::string & getBoard(){return board;}

};
}}}
#endif /* CAEN_CAENBASE_H_ */
