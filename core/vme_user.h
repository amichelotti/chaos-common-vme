#ifndef _VME_USER_H_
#define _VME_USER_H_

#define VME_USER_BUS_MAX	1
#ifndef __KERNEL__
#include <stdint.h>
#define __packed __attribute__((packed))
typedef uint32_t __u32;
typedef uint64_t __u64;
typedef uint8_t __u8;

#endif
/*
 * VMEbus Master Window Configuration Structure
 */
struct vme_master {
	__u32 enable;		/* State of Window */
	__u64 vme_addr;		/* Starting Address on the VMEbus */
	__u64 size;		/* Window Size */
	__u32 aspace;		/* Address Space */
	__u32 cycle;		/* Cycle properties */
	__u32 dwidth;		/* Maximum Data Width */
#if 0
	char prefetchenable;		/* Prefetch Read Enable State */
	int prefetchsize;		/* Prefetch Read Size (Cache Lines) */
	char wrpostenable;		/* Write Post State */
#endif
} __packed;


/*
 * IOCTL Commands and structures
 */

/* Magic number for use in ioctls */
#define VME_IOC_MAGIC 0xAE


/* VMEbus Slave Window Configuration Structure */
struct vme_slave {
	__u32 enable;		/* State of Window */
	__u64 vme_addr;		/* Starting Address on the VMEbus */
	__u64 size;		/* Window Size */
	__u32 aspace;		/* Address Space */
	__u32 cycle;		/* Cycle properties */
#if 0
	char wrpostenable;		/* Write Post State */
	char rmwlock;			/* Lock PCI during RMW Cycles */
	char data64bitcapable;		/* non-VMEbus capable of 64-bit Data */
#endif
} __packed;

struct vme_irq_id {
	__u8 level;
	__u8 statid;
};

struct vme_irq_handle {
  __u8 statid;
  __u8 level; 
};

struct vme_irq_handle_caen {
    __u8 statid;
    __u8 level;
    __u8 channels;
    void*meb; // the user must provide a heap buffer, where the meb is filled
    void*events; // the user must provide valid pointer to fill with the nenvents
};

struct vme_user_drv {
  long base;
  long size;
  int irq_vector;
  int irq_level;
  struct vme_dev *vdev;
  struct vme_resource *window;

};

#define VME_GET_SLAVE _IOR(VME_IOC_MAGIC, 1, struct vme_slave)
#define VME_SET_SLAVE _IOW(VME_IOC_MAGIC, 2, struct vme_slave)
#define VME_GET_MASTER _IOR(VME_IOC_MAGIC, 3, struct vme_master)
#define VME_SET_MASTER _IOW(VME_IOC_MAGIC, 4, struct vme_master)
#define VME_IRQ_GEN _IOW(VME_IOC_MAGIC, 5, struct vme_irq_id)
#define VME_IRQ_HANDLE _IOW(VME_IOC_MAGIC, 6, struct vme_irq_handle)
#define VME_IRQ_REMOVE _IO(VME_IOC_MAGIC, 7)

#define VME_IRQ_HANDLE_CAENDAQ _IOW(VME_IOC_MAGIC, 8, struct vme_irq_handle_caen)
#define VME_IRQ_HANDLE_CAENDAQ_COMBINE _IOW(VME_IOC_MAGIC, 9, struct vme_irq_handle_caen)


#endif /* _VME_USER_H_ */

