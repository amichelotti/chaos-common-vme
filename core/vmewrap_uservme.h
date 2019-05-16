/*
 *
 * @file vmewrap_uservme.h
 * @author Andrea Michelotti
 * @date October 21, 2017
 * wrapper for vme_user kernel driver
 */
#ifndef _VMEWRAPUSERVME_H_
#define _VMEWRAPUSERVME_H_
#include <common/vme/core/vmewrap.h>
/*
 * initialize the driver structure
 * @return 0 on success
 */
int vme_init_driver_uservme(vmewrap_vme_handle_t handle);
/*
 * deinitialize the driver structure
 * @return 0 on success
 */
int vme_deinit_driver_uservme(vmewrap_vme_handle_t handle);


#endif


