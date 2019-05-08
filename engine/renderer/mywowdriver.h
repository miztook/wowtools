#pragma once

#include "mywow.h"

IVideoDriver* mywow_InitDriver(const SWindowInfo& wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode);