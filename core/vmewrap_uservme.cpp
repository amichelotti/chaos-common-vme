
//#define DEBUG 1
#include "vmewrap.h"
#include <poll.h>
#include "vme_user.h"
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <sstream>
#include <cstring>
#ifdef __linux__
#include <byteswap.h>
#else

static inline unsigned short __bswap_16(unsigned short x) {
  return (x>>8) | (x<<8);
}

static inline unsigned int __bswap_32(unsigned int x) {
  return (__bswap_16(x&0xffff)<<16) | (__bswap_16(x>>16));
}

static inline unsigned long long bswap_64(unsigned long long x) {
  return (((unsigned long long)__bswap_32(x&0xffffffffull))<<32) |
    (__bswap_32(x>>32));
}

#endif
/* Resource Type */
/// from linux/vme.h

/* VME Address Spaces */
#define VME_A16		0x1
#define VME_A24		0x2
#define	VME_A32		0x4
#define VME_A64		0x8
#define VME_CRCSR	0x10
#define VME_USER1	0x20
#define VME_USER2	0x40
#define VME_USER3	0x80
#define VME_USER4	0x100

#define VME_A16_MAX	0x10000ULL
#define VME_A24_MAX	0x1000000ULL
#define VME_A32_MAX	0x100000000ULL
#define VME_A64_MAX	0x10000000000000000ULL
#define VME_CRCSR_MAX	0x1000000ULL


/* VME Cycle Types */
#define VME_SCT		0x1
#define VME_BLT		0x2
#define VME_MBLT	0x4
#define VME_2eVME	0x8
#define VME_2eSST	0x10
#define VME_2eSSTB	0x20

#define VME_2eSST160	0x100
#define VME_2eSST267	0x200
#define VME_2eSST320	0x400

#define	VME_SUPER	0x1000
#define	VME_USER	0x2000
#define	VME_PROG	0x4000
#define	VME_DATA	0x8000

/* VME Data Widths */
#define VME_D8		0x1
#define VME_D16		0x2
#define VME_D32		0x4
#define VME_D64		0x8

/* Arbitration Scheduling Modes */
#define VME_R_ROBIN_MODE	0x1
#define VME_PRIORITY_MODE	0x2

#define VME_DMA_PATTERN			(1<<0)
#define VME_DMA_PCI			(1<<1)
#define VME_DMA_VME			(1<<2)

#define VME_DMA_PATTERN_BYTE		(1<<0)
#define VME_DMA_PATTERN_WORD		(1<<1)
#define VME_DMA_PATTERN_INCREMENT	(1<<2)

#define VME_DMA_VME_TO_MEM		(1<<0)
#define VME_DMA_MEM_TO_VME		(1<<1)
#define VME_DMA_VME_TO_VME		(1<<2)
#define VME_DMA_MEM_TO_MEM		(1<<3)
#define VME_DMA_PATTERN_TO_VME		(1<<4)
#define VME_DMA_PATTERN_TO_MEM		(1<<5)

class BusAllocator {
    int masters,slaves;
    // return negative if error
    std::map<int,std::string> fd2dev;
public:
    BusAllocator():masters(0),slaves(0){};


    int getMasters(){return masters;}
    int getSlaves(){return slaves;}

    std::string getDevice(int fd){
        return fd2dev[fd];
    }
    int allocateMaster(){
        std::stringstream ss;
        ss<<"/dev/bus/vme/m"<<masters;
        int ret=open(ss.str().c_str(),O_RDWR);
        if(ret>0){
            DPRINT("allocating master dev \"%s\" fd=%d",ss.str().c_str(),ret);

            fd2dev[ret]=ss.str();
            masters++;
        } else {
            DERR("cannot open master dev \"%s\" fd=%d",ss.str().c_str(),ret);
        }
        return ret;
    }
    int allocateSlave(){
        std::stringstream ss;
        ss<<"/dev/bus/vme/s"<<slaves;
        int ret=open(ss.str().c_str(),O_RDWR);
        if(ret>0){
            DPRINT("allocating slave dev \"%s\" fd=%d",ss.str().c_str(),ret);

            fd2dev[ret]=ss.str();
            slaves++;
        } else{
            DERR("cannot open slave dev \"%s\" fd=%d",ss.str().c_str(),ret);
        }
        return ret;
    }
    int deallocate(int fd){
        std::map<int,std::string>::iterator i= fd2dev.find(fd);
        if(i !=fd2dev.end()){
            DPRINT("deallocating fd %d, dev \"%s\"",fd,i->second.c_str());

            close(fd);
            fd2dev.erase(i);
            return fd;
        }
        DERR("cannot deallocate fd=%d (not found)",fd);

        return -1;
    }
};

static BusAllocator bus;
static int vme_close_uservme(vmewrap_window_t handle){
    DPRINT("closing vme handle 0x%p fd:%d",handle,handle->fd);

    if(handle && (handle->fd>0)){
        int ret;
        DPRINT("closing vme fd %d",handle->fd);
        ret=bus.deallocate(handle->fd);
        handle->fd=0;
        return ret;
    }


    return -1;
}
static int vme_init_uservme(vmewrap_vme_handle_t handle){
    DPRINT("init vme library handle 0x%p",handle);

    return 0;

}
static int configure_vme_params(vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options, int &uni_addressing,int & uni_dw,int &uni_access){
    switch(addressing){
    case VME_ADDRESSING_A16:
        uni_addressing = VME_A16;
        break;
    case VME_ADDRESSING_A24:
        uni_addressing = VME_A24;
        break;
    case VME_ADDRESSING_A32:
        uni_addressing = VME_A32;
        break;
    default:
        ERR("addressing not implemented %d",addressing);
        return -1;
    }

    switch(dw){
    case VME_ACCESS_D16:
        uni_dw = VME_D16 ;
        break;
    case VME_ACCESS_D8:
        uni_dw = VME_D8 ;
        break;
    case VME_ACCESS_D32:
        uni_dw = VME_D32;
        break;
    case VME_ACCESS_D64:
        uni_dw = VME_D64;
        break;
    default:
        ERR("data access not implemented %d",dw);
        return -2;
    }
    uni_access=0;
    if(vme_options&VME_OPT_AM_PROG_AM){
        uni_access |=VME_PROG;
    }
    if(vme_options&VME_OPT_AM_DATA_AM){
        uni_access |=VME_DATA;
    }
    if(vme_options&VME_OPT_AM_USER_AM){
        uni_access |=VME_USER;
    }
    if(vme_options&VME_OPT_BLT_ON){
        uni_access |=VME_BLT;
    }
    if(vme_options&VME_OPT_AM_SUPER_AM){
        uni_access |=VME_SUPER;

    }
    if(vme_options&VME_OPT_MBLT_ON){
        uni_access |=VME_MBLT;
    }
    return 0;
}

static int  map_master_uservme(vmewrap_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw, vme_opt_t vme_options){
    struct vme_master master;
    int retval;
    int myfd;
    int uni_access=0,uni_dw=0,uni_addressing=0;

    if((retval=configure_vme_params(addressing,dw,vme_options,uni_addressing,uni_dw,uni_access))<0){
        return retval;
    }
    myfd=bus.allocateMaster();
    handle->fd=myfd;
    handle->window[handle->nwindow]->fd=myfd;
    if(myfd<0){
        ERR("cannot open a new master %d handle 0x%p fd:%d",myfd,handle,handle->fd);
        return -2;
    }




    master.enable = 1;
    master.vme_addr = add;
    master.size = size;
    master.aspace = uni_addressing;
    master.cycle = uni_access;
    master.dwidth =uni_dw;

    retval = ioctl(myfd, VME_SET_MASTER, &master);

    if (retval != 0) {
        DERR("[%s] vme add= 0x%x, size 0x%x , addressing 0x%x, cycle 0x%x, data width 0x%x,FD:%d retval=%d\n",bus.getDevice(myfd).c_str(),add,size, uni_addressing,uni_access,uni_dw,myfd,retval);
        perror("ERROR: Failed to configure window");
        return -10;
    }

    // DPRINT("Master address mapped at @0x%p size 0x%x, addressing %d (0x%x) dw %d (0x%x)",handle->mapped_address,size,addressing,uni_addressing,dw,uni_dw);
    DPRINT("Master VME address 0x%x size: %d , am =0x%x dw=0x%x mapped on %s device.",add,size,vme_options,dw,bus.getDevice(myfd).c_str());
    return 0;
}

static int  map_slave_uservme(vmewrap_vme_handle_t handle,uint32_t add,uint32_t size,vme_addressing_t addressing,vme_access_t dw,vme_opt_t vme_options){
    struct vme_slave slave;
    int retval;
    int myfd;
    int uni_access=0,uni_dw=0,uni_addressing=0;

    if((retval=configure_vme_params(addressing,dw,vme_options,uni_addressing,uni_dw,uni_access))<0){
        return retval;
    }
    myfd=bus.allocateMaster();
    handle->fd=myfd;

    if(myfd<0){
        ERR("cannot open a new master handle 0x%p fd:%d",handle,handle->fd);
        return -2;
    }



    slave.enable = 1;
    slave.vme_addr = add;
    slave.size = size;
    slave.aspace = uni_addressing;
    slave.cycle = uni_access;
    //	slave.width =uni_dw;

    retval = ioctl(myfd, VME_SET_SLAVE, &slave);

    if (retval != 0) {
        printf("retval=%d\n", retval);
        perror("ERROR: Failed to configure window");
        return -10;
    }

    // DPRINT("Master address mapped at @0x%p size 0x%x, addressing %d (0x%x) dw %d (0x%x)",handle->mapped_address,size,addressing,uni_addressing,dw,uni_dw);
    DPRINT("Slave VME address 0x%x size: %d , am =0x%x dw=0x%x mapped on %s device",add,size,vme_options,dw,bus.getDevice(myfd).c_str());
    return 0;
}
static int vme_write8_uservme(vmewrap_window_t  handle,unsigned off,uint8_t* data,int sizen){
    int fd=handle->fd;
    int ret=0;
    lseek(fd,off,SEEK_SET);
    return write(fd,(void*)data,sizen);

}

static int vme_write32_uservme(vmewrap_window_t  handle,unsigned off,uint32_t* data,int sizen){

    int fd=handle->fd;

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint32_t tmp=data;
#else
    for(int cnt=0;cnt<sizen;cnt++){
        uint32_t tmp=__bswap_32 (data[cnt]);
        data[cnt]=tmp;
    }
#endif
    lseek(fd,off,SEEK_SET);

    return write(fd,data,sizen*sizeof(uint32_t));
}
static int vme_write16_uservme(vmewrap_window_t  handle,unsigned off,uint16_t* data,int sizen){
    int fd=handle->fd;

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint16_t tmp=data;
#else
    for(int cnt=0;cnt<sizen;cnt++){
        uint16_t tmp=__bswap_16 (data[cnt]);
        data[cnt]=tmp;
    }
#endif
    lseek(fd,off,SEEK_SET);

    return write(fd,data,sizen*sizeof(uint16_t));
}
static int vme_read32_uservme(vmewrap_window_t  handle,unsigned off,uint32_t *data,int sizen){
    int fd=handle->fd;
    uint32_t tmp=0;
    lseek(fd,off,SEEK_SET);

    int ret=read(fd,data,sizen*sizeof(uint32_t));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#else
    for(int cnt=0;cnt<sizen;cnt++){
        tmp=__bswap_32 (data[cnt]);
        data[cnt]=tmp;
    }
#endif
    return ret;
}

static int vme_read16_uservme(vmewrap_window_t  handle,unsigned off,uint16_t *data,int sizen){
    int fd=handle->fd;
    uint16_t tmp=0;
    lseek(fd,off,SEEK_SET);
    int ret=read(fd,data,sizen*sizeof(uint16_t));
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#else
    for(int cnt=0;cnt<sizen;cnt++){
        tmp=__bswap_16 (data[cnt]);
        data[cnt]=tmp;
    }
#endif
    return ret;
}
static int vme_read8_uservme(vmewrap_window_t  handle,unsigned off,uint8_t *data,int sizen){
    int fd=handle->fd;
    lseek(fd,off,SEEK_SET);

    return read(fd,data,sizen*sizeof(uint8_t));
}

static int vme_set_uservme(vmewrap_window_t  handle,unsigned off,void *data,int sizeb){
    int fd=handle->fd;

    struct pack{
        uint32_t off;
        uint32_t data;
        uint8_t sizeb;
    } d={off,0,(uint8_t)sizeb};
    memcpy(&d.data,data,sizeb);

    int  ret=ioctl(fd, VME_SET_REG, &d);
        DPRINT("vme set off %d data:0x%x fd:%d, ret=0x%x cmd=0x%lx",d.off,d.data,handle->fd,ret,VME_SET_REG);

    return ret;
}
static int vme_clr_uservme(vmewrap_window_t  handle,unsigned off,void *data,int sizeb){
    int fd=handle->fd;

    struct pack{
        uint32_t off;
        uint32_t data;
        uint8_t sizeb;
    } d={off,0,(uint8_t)sizeb};

    memcpy(&d.data,data,sizeb);
    DPRINT("vme clr off %d data:0x%x fd:%d",d.off,d.data,handle->fd);

    int  ret=ioctl(fd, VME_CLR_REG, &d);
    return ret;
}
int vme_interrupt_enable_uservme(vmewrap_window_t  handle,int level, int signature,int type,void*priv){
    int fd=handle->fd;
    struct vme_irq_handle irq_req;
    int ret;
    irq_req.statid=signature;
    irq_req.level=level;
    if((type!=0)&& priv){
      std::memcpy(priv,&irq_req,sizeof(struct vme_irq_handle));
        ret=ioctl(fd, type, priv);
    } else {
        ret=ioctl(fd, VME_IRQ_HANDLE, &irq_req);
    }

    DPRINT("[%d] interrupt enable 0x%x ivr 0x%x, ret=%d",fd,level,signature,ret);

    return ret;
}
int vme_interrupt_disable_uservme(vmewrap_window_t  handle){
    int fd=handle->fd;
    int ret=ioctl(fd,VME_IRQ_REMOVE,0);
    DPRINT("[%d] interrupt disable ret=%d",fd,ret);

    return ret;
}

int vme_wait_interrupt_uservme(vmewrap_window_t  handle,int timeo_ms){
    struct pollfd pol;
    int fd=handle->fd;

    int ret;
    int t=-1;
    pol.fd=fd;
    pol.events=POLLIN | POLLHUP;

    DPRINT("[%d] waiting interrupt timeo %d",fd,timeo_ms);
    if(timeo_ms!=0){
        t=timeo_ms;
    }
    ret = poll(&pol,1,t);
    DPRINT("[%d] exiting wait interrupt ret %d",fd,ret);

    return ret;

}
int vme_init_driver_uservme(vmewrap_vme_handle_t p){
    DPRINT("Initializing library");
    p->map_close=vme_close_uservme;
    p->vme_init=vme_init_uservme;
    p->map_master=map_master_uservme;
    p->map_slave=map_slave_uservme;
    p->vme_write8=vme_write8_uservme;
    p->vme_write16=vme_write16_uservme;
    p->vme_write32=vme_write32_uservme;
    p->vme_interrupt_enable=vme_interrupt_enable_uservme;
    p->vme_interrupt_disable=vme_interrupt_disable_uservme;
    p->vme_wait_interrupt=vme_wait_interrupt_uservme;
    p->vme_read8=vme_read8_uservme;
    p->vme_read16=vme_read16_uservme;
    p->vme_read32=vme_read32_uservme;
    p->vme_set_reg=vme_set_uservme;
    p->vme_clr_reg=vme_clr_uservme;

    return 0;
}
int vme_deinit_driver_uservme(vmewrap_vme_handle_t p){
    DPRINT("DeInitializing library");
    for(int cnt=0;cnt<p->nwindow;cnt++){
        if(p->window[cnt])vme_close_uservme(p->window[cnt]);

    }
    return 0;
}


