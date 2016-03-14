/*
 * @file vmewrap_caenvme.h
 * @author Andrea Michelotti
 * @date Mar 11, 2016
*/

#ifndef _VMEWRAPCAENLIB_H_
#define _VMEWRAPCAENLIB_H_
#include <common/vme/core/vmewrap.h>
/*
 * initialize the driver structure
 * @return 0 on success
 */
int vme_init_driver_caenvme(vmewrap_vme_handle_t handle);
/*
 * deinitialize the driver structure
 * @return 0 on success
 */
int vme_deinit_driver_caenvme(vmewrap_vme_handle_t handle);


#endif


