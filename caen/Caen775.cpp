/*
 * Caen775.cpp
 *
 *  Created on: Mar 4, 2016
 *      Author: michelo
 */

#include "Caen775.h"
#include <common/vme/caen/caen775_drv.h>
namespace common {
namespace vme {
namespace caen {

Caen775::Caen775() {
	// TODO Auto-generated constructor stub

}

Caen775::~Caen775() {
	// TODO Auto-generated destructor stub
}
void Caen775::setFSR(uint16_t data){
	write(CAEN775_FSR_OFF,data);
}
uint16_t Caen775::getFSR(){
	uint16_t data;
	read(CAEN775_FSR_OFF,data);
	return data;
}
} /* namespace caen */
} /* namespace vme */
} /* namespace common */
