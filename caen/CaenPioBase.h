#ifndef CAENPIOBASE_H
#define CAENPIOBASE_H
#include <common/vme/core/VmeBase.h>

namespace common{
namespace vme {
namespace caen {

class CaenPioBase : public ::common::vme::VmeBase
{
public:
    CaenPioBase();
    /**
     * Open the Caen Device, map the address
     * @param vme_driver vme driver to use
     * @param address vme address of the board
     * @return 0 on success
     * */
    int open(vme_driver_t vme_driver,uint64_t address);
    int reset();
    
    uint16_t out(uint16_t outmask);
    int in(uint32_t& stat);
    
    // set output pins
    // @outmask is the mask of the bits we want to set.
    // @return the resulting content of the output register
        
    uint16_t set(uint16_t outmask=0xffff);

    // clear output pins
    // @outmask is the mask of the bits we want to clear.
    // @return the resulting content of the output register
    uint16_t clr(uint16_t outmask=0xffff);
    // inhibit front input 
    int maskin(uint32_t outmask);
    // inhibit flip flop output
    int maskout(uint32_t outmask);
    int interrupt_enable(int signature,uint32_t inmask);
    int interrupt_disable();
};
}}}

#endif // CAENPIOBASE_H
