#ifndef CAENPIOBASE_H
#define CAENPIOBASE_H
#include <common/vme/core/VmeBase.h>

namespace common{
namespace vme {
namespace caen {

class CaenPioBase : public VmeBase
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
    int out(uint32_t outmask);
    int in(uint32_t& stat);
    int interrupt_enable(int signature,uint32_t inmask);
    int interrupt_disable();
};
}}}

#endif // CAENPIOBASE_H
