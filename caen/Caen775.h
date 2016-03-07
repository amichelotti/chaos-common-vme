/*
 * Caen775.h
 *
 *  Created on: Mar 4, 2016
 *      Author: michelo
 */

#ifndef CAEN_CAEN775_H_
#define CAEN_CAEN775_H_

#include "CaenBase.h"

namespace common {
namespace vme {
namespace caen {

class Caen775: public CaenBase {
public:
	Caen775();
	virtual ~Caen775();
	/*
	 * Set Full Scale Range
	 */
	void setFSR(uint16_t);
	uint16_t getFSR();

};
#define CAEN775_FSR
} /* namespace caen */
} /* namespace vme */
} /* namespace common */

#endif /* CAEN_CAEN775_H_ */
