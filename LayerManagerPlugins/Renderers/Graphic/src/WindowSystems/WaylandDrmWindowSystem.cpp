/***************************************************************************
*
* Copyright 2010, 2011 BMW Car IT GmbH
* Copyright (C) 2011 DENSO CORPORATION and Robert Bosch Car Multimedia Gmbh
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*        http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*
* THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
* FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
* SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
* RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
* CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
* CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
****************************************************************************/

#include "WindowSystems/WaylandDrmWindowSystem.h"
#include "Log.h"
#include <linux/fb.h>
#include <fcntl.h>
#include <iomanip>

#include <gbm.h>
#include <libudev.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

static const char default_seat[] = "seat0";

WaylandDrmWindowSystem::WaylandDrmWindowSystem(const char* displayname, int width, int height, Scene* pScene)
: WaylandBaseWindowSystem(displayname, width, height, pScene)
, m_fdDev(-1), m_gbm(NULL)
{
    LOG_DEBUG("WaylandDrmWindowSystem", "creating WaylandDrmWindowSystem width:" << width << " height:" << height);
}

WaylandDrmWindowSystem::~WaylandDrmWindowSystem()
{
	if (m_gbm != NULL)
		gbm_device_destroy(m_gbm);
	if (m_fdDev >= 0)
		drmDropMaster(m_fdDev);
}

bool WaylandDrmWindowSystem::initGraphicSystem()
{
	graphicSystem->setBaseWindowSystem(this);
    return graphicSystem->init((void*)m_gbm, (void*)NULL);
}

bool WaylandDrmWindowSystem::createNativeContext()
{
	struct udev* udev;
	struct udev_device *device, *drm_device;
	struct udev_enumerate* e;
	struct udev_list_entry* entry;
	const char *path, *device_seat;
	const char *seat = default_seat;

	udev = udev_new();
	if (udev == NULL)
	{
		LOG_ERROR("WaylandDrmWindowSystem", "failed to initialize udev context.");
		return false;
	}

	e = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(e, "drm");
	udev_enumerate_add_match_sysname(e, "card[0-9]*");

	udev_enumerate_scan_devices(e);
	drm_device = NULL;
	udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e))
	{
		path = udev_list_entry_get_name(entry);
		device = udev_device_new_from_syspath(udev, path);
		device_seat = udev_device_get_property_value(device, "ID_SEAT");
		if (!device_seat)
			device_seat = default_seat;
		if (strcmp(device_seat, seat) == 0)
		{
			drm_device = device;
			break;
		}
		udev_device_unref(device);
	}

	if (drm_device == NULL)
	{
		LOG_ERROR("WaylandDrmWindowSystem", "no drm device found");
		return false;
	}

	const char* filename;

	filename = udev_device_get_devnode(drm_device);
	m_fdDev = open(filename, O_RDWR | O_CLOEXEC);
	if (m_fdDev < 0)
	{
        LOG_ERROR("WaylandDrmWindowSystem", "failed to open device");
        return false;
	}

	LOG_DEBUG("WaylandDrmWindowSystem", "Device name: " << filename << " fd: " << m_fdDev);

	m_gbm = gbm_create_device(m_fdDev);
	if (m_gbm == NULL)
	{
        LOG_ERROR("WaylandDrmWindowSystem", "failed to create gbm device");
		return false;
	}

	udev_device_unref(drm_device);

    LOG_DEBUG("WaylandDrmWindowSystem", "SUCCESS:create gbm device");
    return true;
}
