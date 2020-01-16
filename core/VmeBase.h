/*
 * VmeBase.h
 *
 *  Created on: Oct 25, 2017
 *      Author: michelo
 */

#ifndef CORE_VMEBASE_H_
#define CORE_VMEBASE_H_
//#define DEBUG 1
#include <common/debug/core/debug.h>

#include <common/vme/core/vmewrap.h>
#include <boost/thread.hpp>
#include <sys/ioctl.h>

namespace common {
namespace vme {

class VmeBase {
	vmewrap_vme_handle_t vme;
	vmewrap_window_t window;

	boost::thread m_thread;
	int run;
	void sched_task();
    static int nboard;
    int boardid;
protected:
    uint64_t address;
    uint32_t size;
    vme_addressing_t master_addressing;
    vme_access_t dw;
    vme_opt_t vme_options;
	int unixdev;

public:
	VmeBase();
	// unix mapped device
	int openUnixDev(const char* dev);
	bool isUnixDev();
	virtual int open(vme_driver_t vme_driver,uint64_t address,uint32_t size,vme_addressing_t master_addressing=VME_ADDRESSING_A32,vme_access_t dw=VME_ACCESS_D32, vme_opt_t vme_options=VME_OPT_AM_USER_AM);
	virtual int close();
    int interrupt_enable(int level, int signature,int type=0,void*priv=NULL);
    int interrupt_disable();
	int install_interrupt_handler(int level,int signature);
    int wait_interrupt(int timeo_ms=0);
	int remove_interrupt_handler();
	virtual int interrupt_handler();
    /**
    reset of the board
    */
    virtual int reset();
	virtual ~VmeBase();
	template<typename T>
	int set(uint32_t off,T& data){
		DPRINT("VMESET off 0x%x data=0x%x fd=%d",off,data,getFD());

		return vmewrap_set_reg(window,&data, off,sizeof(T));
	}
	template<typename T>
	int clr(uint32_t off,T& data){
		DPRINT("VMECLR off 0x%x data=0x%x fd=%d",off,data,getFD());

		return vmewrap_clr_reg(window,&data, off,sizeof(T));

	}

	int write(uint32_t off,uint16_t* data,int sizen);
	int read(uint32_t off,uint16_t* data,int sizen);
	int write(uint32_t off,uint32_t* data,int sizen);
	int read(uint32_t off,uint32_t* data,int sizen);
	int write(uint32_t off,uint8_t* data,int sizen);
	int read(uint32_t off,uint8_t* data,int sizen);
	int write(uint32_t off,uint8_t data);
	int read(uint32_t off,uint8_t& data);
	int getFD();


	/*
	 * Write to a specified offset
	 * @param off offset inside the caen device
	 * @param data dat to write
	 */

	int write(uint32_t off,uint16_t data);
	/*
	 * Write to a specified offset
	 * @param off offset inside the caen device
	 * @param data dat to write
	 */
	int write(uint32_t off,uint32_t data);
	/*
	 * read froma a specified offset
	 * @param off offset inside the caen device
	 * @param data reference to data
	 */
	int read(uint32_t off,uint16_t &data);
	/*
	 * read froma a specified offset
	 * @param off offset inside the caen device
	 * @param data reference to data
	 */
	int read(uint32_t off,uint32_t &data);
	/**
	 * Close the vme device, unmap the address
	 * @return 0 on success
	 * */
	uint16_t read16(uint32_t off);
	uint32_t read32(uint32_t off);
	uint8_t read8(uint32_t off);
	int write16(uint32_t off,uint16_t);
	int write32(uint32_t off,uint32_t);
	int write8(uint32_t off,uint8_t);
    int getBoardId(){return boardid;}
};

} /* namespace vme */
} /* namespace common */

#endif /* CORE_VMEBASE_H_ */
