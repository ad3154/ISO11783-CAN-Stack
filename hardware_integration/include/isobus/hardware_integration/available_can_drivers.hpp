//================================================================================================
/// @file available_can_drivers.hpp
///
/// @brief A utility to include all of the available CAN drivers. Should mainly be
/// used for testing purposes.
/// @author Daan Steenbergen
///
/// @copyright 2023 The Open-Agriculture Developers
//================================================================================================
#ifndef AVAILABLE_CAN_DRIVERS_HPP
#define AVAILABLE_CAN_DRIVERS_HPP

#ifdef ISOBUS_SOCKETCAN_AVAILABLE
#include "isobus/hardware_integration/socket_can_interface.hpp"
#endif

#ifdef ISOBUS_WINDOWSPCANBASIC_AVAILABLE
#include "isobus/hardware_integration/pcan_basic_windows_plugin.hpp"
#endif

#ifdef ISOBUS_VIRTUALCAN_AVAILABLE
#include "isobus/hardware_integration/virtual_can_plugin.hpp"
#endif

#ifdef ISOBUS_TWAI_AVAILABLE
#include "isobus/hardware_integration/twai_plugin.hpp"
#endif

#ifdef ISOBUS_MCP2515_AVAILABLE
#include "isobus/hardware_integration/mcp2515_can_interface.hpp"
#endif

#ifdef ISOBUS_WINDOWSINNOMAKERUSB2CAN_AVAILABLE
#include "isobus/hardware_integration/innomaker_usb2can_windows_plugin.hpp"
#endif

#ifdef ISOBUS_MACCANPCAN_AVAILABLE
#include "isobus/hardware_integration/mac_can_pcan_plugin.hpp"
#endif

#ifdef ISOBUS_TOUCAN_AVAILABLE
#include "isobus/hardware_integration/toucan_vscp_canal.hpp"
#endif

#ifdef ISOBUS_SYS_TEC_AVAILABLE
#include "isobus/hardware_integration/sys_tec_windows_plugin.hpp"
#endif

#ifdef ISOBUS_NTCAN_AVAILABLE
#include "isobus/hardware_integration/ntcan_plugin.hpp"
#endif

#ifdef ISOBUS_VSCAN_AVAILABLE
#include "isobus/hardware_integration/vscan_plugin.hpp"
#endif

#endif // AVAILABLE_CAN_DRIVERS_HPP
