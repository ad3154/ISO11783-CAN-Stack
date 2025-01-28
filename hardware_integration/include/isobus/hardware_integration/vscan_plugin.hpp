//================================================================================================
/// @file vscan_plugin.hpp
///
/// @brief An interface for using a VSCOM VSCAN driver.
/// @attention Use of the VSCAN driver is governed in part by their license, and requires you
/// to install their driver first, which in-turn requires you to agree to their terms and conditions.
/// @author Daan Steenbergen
///
/// @copyright 2025 The Open-Agriculture Developers
//================================================================================================
#ifndef VSCAN_PLUGIN_HPP
#define VSCAN_PLUGIN_HPP

#include <string>
#include "vs_can_api.h"

#include "isobus/hardware_integration/can_hardware_plugin.hpp"
#include "isobus/isobus/can_hardware_abstraction.hpp"
#include "isobus/isobus/can_message_frame.hpp"

namespace isobus
{
	/// @brief A CAN Driver for VSCOM VSCAN Devices
	class VSCANPlugin : public CANHardwarePlugin
	{
	public:
		/// @brief Constructor for the VSCOM VSCAN CAN driver
		/// @param[in] channel The COM port or IP address of the VSCAN device to use.
		/// @param[in] baudrate The baudrate to use for the CAN connection.
		VSCANPlugin(const std::string &channel, void *baudrate = VSCAN_SPEED_250K);

		/// @brief The destructor for VSCANPlugin
		virtual ~VSCANPlugin() = default;

		/// @brief Returns if the connection with the hardware is valid
		/// @returns `true` if connected, `false` if not connected
		bool get_is_valid() const override;

		/// @brief Closes the connection to the hardware
		void close() override;

		/// @brief Connects to the hardware you specified in the constructor's channel argument
		void open() override;

		/// @brief Returns a frame from the hardware (synchronous), or `false` if no frame can be read.
		/// @param[in, out] canFrame The CAN frame that was read
		/// @returns `true` if a CAN frame was read, otherwise `false`
		bool read_frame(isobus::CANMessageFrame &canFrame) override;

		/// @brief Writes a frame to the bus (synchronous)
		/// @param[in] canFrame The frame to write to the bus
		/// @returns `true` if the frame was written, otherwise `false`
		bool write_frame(const isobus::CANMessageFrame &canFrame) override;

		/// @brief Changes previously set configuration parameters. Only works if the device is not open.
		/// @param[in] channel The COM port or IP address of the VSCAN device to use.
		/// @param[in] baudrate The baudrate to use for the CAN connection.
		/// @returns True if the configuration was changed, otherwise false (if the device is open false will be returned)
		bool reconfigure(const std::string &channel, void *baudrate = VSCAN_SPEED_250K);

	private:
		/// @brief Parses the error from the status code
		/// @param[in] status The status code to parse
		/// @returns The error message
		static std::string parse_error_from_status(VSCAN_STATUS status);

		std::string channel; ///< The COM port or IP address of the VSCAN device to use.
		void *baudrate; ///< The baudrate to use for the CAN connection.
		VSCAN_HANDLE handle; ///< The handle as defined in the NTCAN driver API
		VSCAN_STATUS status = VSCAN_ERR_OK; ///< Stores the result of the call to begin CAN communication. Used for is_valid check later.
	};
}

#endif // NTCAN_PLUGIN_HPP
