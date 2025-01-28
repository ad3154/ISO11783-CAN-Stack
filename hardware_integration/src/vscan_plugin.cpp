//================================================================================================
/// @file vscan_plugin.cpp
///
/// @brief An interface for using a VSCOM VSCAN driver.
/// @attention Use of the VSCAN driver is governed in part by their license, and requires you
/// to install their driver first, which in-turn requires you to agree to their terms and conditions.
/// @author Daan Steenbergen
///
/// @copyright 2025 The Open-Agriculture Developers
//================================================================================================

#include "isobus/hardware_integration/vscan_plugin.hpp"
#include "isobus/isobus/can_stack_logger.hpp"

#include <chrono>
#include <cstring>
#include <thread>

namespace isobus
{
	VSCANPlugin::VSCANPlugin(const std::string &channel, void *baudrate) :
	  channel(channel),
	  baudrate(baudrate)
	{
	}

	bool VSCANPlugin::get_is_valid() const
	{
		return (VSCAN_ERR_OK == status) && (handle > 0);
	}

	void VSCANPlugin::close()
	{
		VSCAN_Close(handle);
		handle = 0;
	}

	void VSCANPlugin::open()
	{
		if (get_is_valid())
		{
			LOG_ERROR("[VSCAN]: Attempting to open a connection that is already open");
			return;
		}

		VSCAN_API_VERSION version;
		status = VSCAN_Ioctl(0, VSCAN_IOCTL_GET_API_VERSION, &version);
		if (status != VSCAN_ERR_OK)
		{
			LOG_ERROR("[VSCAN] Failed to get API version: %s, trying to continue anyway", parse_error_from_status(status).c_str());
		}

		LOG_DEBUG("[VSCAN] API Version %d.%d.%d", version.Major, version.Minor, version.SubMinor);

		// We create a buffer to guarantee the content to be non-const
		std::vector<char> channelBuffer(channel.begin(), channel.end());
		channelBuffer.push_back('\0');
		handle = VSCAN_Open(channelBuffer.data(), VSCAN_MODE_NORMAL);
		if (handle <= 0)
		{
			LOG_ERROR("[VSCAN]: Error trying to open the connection: %s", parse_error_from_status(handle).c_str());
			return;
		}

		status = VSCAN_Ioctl(handle, VSCAN_IOCTL_SET_SPEED, baudrate);
		if (VSCAN_ERR_OK != status)
		{
			LOG_ERROR("[VSCAN]: Error trying to set the baudrate: %s", parse_error_from_status(status).c_str());
			close();
			return;
		}

		status = VSCAN_Ioctl(handle, VSCAN_IOCTL_SET_BLOCKING_READ, VSCAN_IOCTL_ON);
		if (VSCAN_ERR_OK != status)
		{
			LOG_ERROR("[VSCAN]: Error trying to set blocking read mode: %s", parse_error_from_status(status).c_str());
			close();
			return;
		}
	}

	bool VSCANPlugin::read_frame(CANMessageFrame &canFrame)
	{
		VSCAN_MSG message;
		DWORD rv;
		bool retVal = false;

		status = VSCAN_Read(handle, &message, 1, &rv);

		if (VSCAN_ERR_OK == status && rv)
		{
			canFrame.dataLength = message.Size;
			memcpy(canFrame.data, message.Data, message.Size);
			canFrame.identifier = message.Id;
			canFrame.isExtendedFrame = (message.Flags & VSCAN_FLAGS_EXTENDED);
			retVal = true;
		}
		else
		{
			LOG_ERROR("[VSCAN]: Error trying to read a frame: %s, closing connection", parse_error_from_status(status).c_str());
			close();
		}

		return retVal;
	}

	bool VSCANPlugin::write_frame(const CANMessageFrame &canFrame)
	{
		VSCAN_MSG message;
		DWORD rv;
		bool retVal = false;

		message.Id = canFrame.identifier;
		message.Size = canFrame.dataLength;
		memcpy(message.Data, canFrame.data, message.Size);
		message.Flags = canFrame.isExtendedFrame ? VSCAN_FLAGS_EXTENDED : VSCAN_FLAGS_STANDARD;

		status = VSCAN_Write(handle, &message, 1, &rv);

		if (VSCAN_ERR_OK == status && rv)
		{
			status = VSCAN_Flush(handle);
			if (VSCAN_ERR_OK == status)
			{
				retVal = true;
			}
			else
			{
				LOG_ERROR("[VSCAN]: Error trying to flush the write buffer: %s, closing connection", parse_error_from_status(status).c_str());
				close();
			}
		}
		else
		{
			LOG_ERROR("[VSCAN]: Error trying to write a frame: %s, closing connection", parse_error_from_status(status).c_str());
			close();
		}

		return retVal;
	}

	bool VSCANPlugin::reconfigure(const std::string &channel, void *baudrate)
	{
		bool retVal = false;

		if (!get_is_valid())
		{
			this->channel = channel;
			this->baudrate = baudrate;
			retVal = true;
		}
		return retVal;
	}

	std::string VSCANPlugin::parse_error_from_status(VSCAN_STATUS status)
	{
		// Arbitrary buffer size, should be enough for most error messages
		size_t bufferSize = 256;
		std::vector<char> errorBuffer(bufferSize, 0);

		VSCAN_GetErrorString(status, errorBuffer.data(), bufferSize);

		// Ensure the string is null-terminated, just in case
		errorBuffer[bufferSize - 1] = '\0';

		return std::string(errorBuffer.data());
	}
}
// namespace isobus
