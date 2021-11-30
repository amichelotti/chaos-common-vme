
#include <vmedrv/sis3153/project_system_define.h>
#include <vmedrv/sis3153/project_interface_define.h>
#include <vmedrv/sis3153/vme_interface_class.h>
#include <vmedrv/sis3153/sis3153ETH_vme_class.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <map>
#include "vmewrap.h"

#include "vmewrap_int.h"

typedef struct sis3153_eth_priv
{
	char ip[64];
	sis3153eth *vme_crate;
	vme_addressing_t addressing;
	vme_access_t access;
	vme_opt_t opt;
	uint32_t add;
	uint32_t size;
} sis3153_eth_priv_t;

static int vme_close_sis3153_eth(vmewrap_window_t handle)
{
	DPRINT("close handle %p \n", handle);

	if (handle->priv)
	{
		free(handle->priv);
		handle->priv = NULL;
	}
	return 0;
}
static int vme_init_sis3153_eth(vmewrap_vme_handle_t handle)
{

	return 0;
}
static int map_master_sis3153_eth(vmewrap_vme_handle_t handle, uint32_t add, uint32_t size, vme_addressing_t addressing, vme_access_t dw, vme_opt_t vme_options)
{
	return 0;
}

static int map_slave_sis3153_eth(vmewrap_vme_handle_t handle, uint32_t add, uint32_t size, vme_addressing_t addressing, vme_access_t dw, vme_opt_t vme_options)
{
	return 0;
}
static int vme_write8_sis3153_eth(vmewrap_window_t handle, unsigned off, uint8_t *data, int sizen)
{
	uint32_t ret = 0;

	sis3153eth *vme_crate = (sis3153eth *)handle->parent->bus;

	if (handle->addressing == VME_ADDRESSING_A32)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A32D8_write(handle->add + off, data[cnt]);
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A24)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A24D8_write(handle->add + off, data[cnt]);
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A16)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A16D8_write(handle->add + off, data[cnt]);
		}
	}
	else
	{
		DERR("unsupported address space");
		return -1;
	}
	return ret;
}

static int vme_write32_sis3153_eth(vmewrap_window_t handle, unsigned off, uint32_t *data, int sizen)
{
	uint32_t ret = 0;

	sis3153eth *vme_crate = (sis3153eth *)handle->parent->bus;

	if (handle->addressing == VME_ADDRESSING_A32)
	{
		if (sizen == 1)
		{
			ret += vme_crate->vme_A32D32_write(handle->add + off, *data);
			return ret;
		}
		if (handle->opt & VME_OPT_BLT_ON)
		{
			vme_crate->vme_A32BLT32_write(handle->add + off, data, sizen, &ret);
		}
		else
		{
			vme_crate->vme_A32DMA_D32_write(handle->add + off, data, sizen, &ret);
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A24)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A24D32_write(handle->add + off, data[cnt]);
		}
	}
	else
	{
		DERR("unsupported address space");
		return -1;
	}
	return ret;
}
static int vme_write16_sis3153_eth(vmewrap_window_t handle, unsigned off, uint16_t *data, int sizen)
{
	uint32_t ret = 0;

	sis3153eth *vme_crate = (sis3153eth *)handle->parent->bus;

	if (handle->addressing == VME_ADDRESSING_A32)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A32D16_write(handle->add + off, data[cnt]);
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A24)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A24D16_write(handle->add + off, data[cnt]);
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A16)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += vme_crate->vme_A16D16_write(handle->add + off, data[cnt]);
		}
	}
	else
	{
		DERR("unsupported address space");
		return -1;
	}
	return ret;
}
static int vme_read32_sis3153_eth(vmewrap_window_t handle, unsigned off, uint32_t *data, int sizen)
{
	uint32_t ret = 0;

	sis3153eth *vme_crate = (sis3153eth *)handle->parent->bus;

	if (handle->addressing == VME_ADDRESSING_A32)
	{
		if (sizen == 1)
		{
			ret += (vme_crate->vme_A32D32_read(handle->add + off, data) == 0) ? 1 : 0;
			return ret;
		}
		if (handle->opt & VME_OPT_BLT_ON)
		{
			vme_crate->vme_A32BLT32_read(handle->add + off, data, sizen, &ret);
		}
		else
		{
			vme_crate->vme_A32DMA_D32_read(handle->add + off, data, sizen, &ret);
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A24)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A24D32_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else
	{
		DERR("unsupported address space");
		return -1;
	}
	return ret;
}
static int vme_read16_sis3153_eth(vmewrap_window_t handle, unsigned off, uint16_t *data, int sizen)
{
	uint32_t ret = 0;

	sis3153eth *vme_crate = (sis3153eth *)handle->parent->bus;

	if (handle->addressing == VME_ADDRESSING_A32)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A32D16_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A24)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A24D16_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A16)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A16D16_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else
	{
		DERR("unsupported address space");
		return -1;
	}
	return ret;
}
static int vme_read8_sis3153_eth(vmewrap_window_t handle, unsigned off, uint8_t *data, int sizen)
{
	uint32_t ret = 0;

	sis3153eth *vme_crate = (sis3153eth *)handle->parent->bus;

	if (handle->addressing == VME_ADDRESSING_A32)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A32D8_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A24)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A24D8_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else if (handle->addressing == VME_ADDRESSING_A16)
	{
		for (int cnt = 0; cnt < sizen; cnt++)
		{
			ret += (vme_crate->vme_A16D8_read(handle->add + off, &data[cnt]) == 0) ? 1 : 0;
		}
	}
	else
	{
		DERR("unsupported address space");
		return -1;
	}
	return ret;
}
typedef std::map<std::string, sis3153eth *> ip2sis_t;
static ip2sis_t sisdev;
int vme_init_driver_sis3153_eth(vmewrap_vme_handle_t p, void *params)
{

	sis3153eth *vme_crate;
	const char *ip_addr_string = (const char *)params;
	if (ip_addr_string == NULL)
	{
		DERR("invalid IP address");
		return -4;
	}
	ip2sis_t::iterator found = sisdev.find(ip_addr_string);
	if (found == sisdev.end())
	{
		sis3153eth(&vme_crate, (char *)ip_addr_string);
		if (vme_crate == NULL)
		{
			DERR("cannot allocate sis controller");
			return -7;
		}
		sisdev[ip_addr_string] = vme_crate;
		char char_messages[128];
		unsigned int nof_found_devices;

		// open Vme Interface device
		int return_code = vme_crate->vmeopen();								// open Vme interface
		vme_crate->get_vmeopen_messages(char_messages, &nof_found_devices); // open Vme interface
		DPRINT("get_vmeopen_messages = %s , number of devices %d \n", char_messages, nof_found_devices);
		if (nof_found_devices <= 0)
		{
			DERR("NO DEVICE FOUND");
			return -9;
		}
	}
	else
	{
		DPRINT("found a sis controller @%s 0x%p\n", ip_addr_string, found->second);
		vme_crate = found->second;
	}
	// every init create a difference space for base address and addressing.
	p->map_close = vme_close_sis3153_eth;
	p->vme_init = vme_init_sis3153_eth;
	p->map_master = map_master_sis3153_eth;
	p->map_slave = map_slave_sis3153_eth;
	p->vme_write8 = vme_write8_sis3153_eth;
	p->vme_write16 = vme_write16_sis3153_eth;
	p->vme_write32 = vme_write32_sis3153_eth;

	p->vme_read8 = vme_read8_sis3153_eth;
	p->vme_read16 = vme_read16_sis3153_eth;
	p->vme_read32 = vme_read32_sis3153_eth;
	p->bus = (void *)vme_crate;

	return 0;
}
int vme_deinit_driver_sis3153_eth(vmewrap_vme_handle_t p)
{

	sis3153eth *vme_crate = (sis3153eth *)p->bus;
	DPRINT("DEINIT handle %p crate %p\n", p, vme_crate);
	for (ip2sis_t::iterator i = sisdev.begin(); i != sisdev.end(); i++)
	{
		PRINT("looking %s crate %p\n", i->first.c_str(), i->second);

		if (i->second == vme_crate)
		{
			PRINT("erasing %s crate\n", i->first.c_str());

			sisdev.erase(i);
			break;
		}
	}
	if (vme_crate != NULL)
	{
		for (int cnt = 0; cnt < p->nwindow; cnt++)
		{
			if (p->window[cnt])
			{
				vme_close_sis3153_eth(p->window[cnt]);
			}
		}
		vme_crate->vmeclose();

		delete vme_crate;
	}

	return 0;
}
