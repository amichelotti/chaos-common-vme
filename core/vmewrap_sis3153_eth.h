/*
 *
 * @file vmewrap_sis3153_eth.h
 * @author Andrea Michelotti
 * @date Gen 13, 2020
 */
#ifndef _VMEWRAPSIS3153_H_
#define _VMEWRAPSIS3153_H_
#include "vmewrap.h"
/*
 * initialize the driver structure
 * @return 0 on success
 */
int vme_init_driver_sis3153_eth(vmewrap_vme_handle_t handle,void*params);
/*
 * deinitialize the driver structure
 * @return 0 on success
 */
int vme_deinit_driver_sis3153_eth(vmewrap_vme_handle_t handle);


#endif


