#include "CaenPioBase.h"
#include <common/debug/core/debug.h>

namespace common{
namespace vme {
namespace caen {

CaenPioBase::CaenPioBase()
{

}
/**
 * Open the Caen Device, map the address
 * @param vme_driver vme driver to use
 * @param address vme address of the board
 * @return 0 on success
 * */
int CaenPioBase::open(vme_driver_t vme_driver,uint64_t address){
          if(::common::vme::VmeBase::open(vme_driver,address, 0x10000,VME_ADDRESSING_A32,VME_ACCESS_D16,VME_OPT_AM_USER_AM)!=0){
              ERR("cannot map vme");
              return -3;
          }

          return 0;
}
int CaenPioBase::reset(){
    write16(0x2E,0);
       return 0;
}

int CaenPioBase::out(uint32_t outmask){
    return write16(0xA,outmask);

}
int CaenPioBase::in(uint32_t& stat){
    stat=read16(0x54);
    return 0;
}
int CaenPioBase::interrupt_enable(int signature,uint32_t inmask){
    int ret;
    int ivl=8-getBoardId();
    ret= VmeBase::interrupt_enable(ivl,signature);

    write16(0xC,inmask);// program mask interrupt all channels (use just test channel)
    write16(0x20,ivl);// program interrupt level
    return 0;
}
int CaenPioBase::interrupt_disable(){
    int ret;
    write16(0x20,0);// program interrupt level
    ret= VmeBase::interrupt_disable();
    return ret;
}
}}}
