//================================================================================================
/// @file ntcan_plugin.cpp
///
/// @brief An interface for using a ESD NTCAN driver.
/// @attention Use of the NTCAN driver is governed in part by their license, and requires you
/// to install their driver first, which in-turn requires you to agree to their terms and conditions.
/// @author Alex "Y_Less" Cole
/// @author Daan Steenbergen
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================

#include "isobus/hardware_integration/ntcan_plugin.hpp"
#include "isobus/isobus/can_stack_logger.hpp"

#include <chrono>
#include <thread>

namespace isobus
{
	NTCANPlugin::NTCANPlugin(int channel, int baudrate) :
	  net(channel),
	  baudrate(baudrate)
	{
	}

	bool NTCANPlugin::get_is_valid() const
	{
		return (NTCAN_SUCCESS == openResult) && (NTCAN_NO_HANDLE != handle);
	}

	void NTCANPlugin::close()
	{
		canClose(handle);
		handle = NTCAN_NO_HANDLE;
	}

	void NTCANPlugin::open()
	{
		if (NTCAN_NO_HANDLE != handle)
		{
			isobus::CANStackLogger::error("[NTCAN]: Attempting to open a connection that is already open");
			return;
		}
		std::uint32_t mode = 0;
		std::int32_t txQueueSize = 8;
		std::int32_t rxQueueSize = 8;
		std::int32_t txTimeOut = 100;
		std::int32_t rxTimeOut = 1000;

		openResult = canOpen(net, mode, txQueueSize, rxQueueSize, txTimeOut, rxTimeOut, &handle);

		if (NTCAN_SUCCESS != openResult)
		{
			isobus::CANStackLogger::error("[NTCAN]: Error trying to open the connection");
			return;
		}

		CAN_IF_STATUS status{ 0 };

		openResult = canSetBaudrate(handle, baudrate);
		if (NTCAN_SUCCESS != openResult)
		{
			isobus::CANStackLogger::error("[NTCAN]: Error trying to set the baudrate");
			close();
			return;
		}

		openResult = canStatus(handle, &status);
		if (NTCAN_SUCCESS != openResult)
		{
			isobus::CANStackLogger::error("[NTCAN]: Error trying to get the status");
			close();
			return;
		}

		if (NTCAN_FEATURE_TIMESTAMP == (status.features & NTCAN_FEATURE_TIMESTAMP))
		{
			std::uint64_t timestamp = 0;
			openResult = canIoctl(handle, NTCAN_IOCTL_GET_TIMESTAMP_FREQ, &timestampFreq);
			if (NTCAN_SUCCESS == openResult)
			{
				openResult = canIoctl(handle, NTCAN_IOCTL_GET_TIMESTAMP, &timestamp);
			}
			if (NTCAN_SUCCESS == openResult)
			{
				auto now = std::chrono::system_clock::now();
				auto unix = now.time_since_epoch();
				long long millis = std::chrono::duration_cast<std::chrono::microseconds>(unix).count();
				timestampOffset = millis - timestamp;
			}
		}

		std::int32_t ids = (1 << 11);
		openResult = canIdRegionAdd(handle, 0, &ids);
		if (NTCAN_SUCCESS == openResult && ids != (1 << 11))
		{
			openResult = NTCAN_INSUFFICIENT_RESOURCES;
			isobus::CANStackLogger::error("[NTCAN]: Error trying to add the standard ID region");
			close();
			return;
		}

		ids = (1 << 29);
		openResult = canIdRegionAdd(handle, NTCAN_20B_BASE, &ids);
		if (NTCAN_SUCCESS == openResult && ids != (1 << 29))
		{
			openResult = NTCAN_INSUFFICIENT_RESOURCES;
			isobus::CANStackLogger::error("[NTCAN]: Error trying to add the extended ID region");
			close();
			return;
		}
	}

	bool NTCANPlugin::read_frame(isobus::CANMessageFrame &canFrame)
	{
		NTCAN_RESULT result;
		CMSG_T msgCanMessage{ 0 };
		bool retVal = false;
		std::int32_t count = 1;

		result = canReadT(handle, &msgCanMessage, &count, nullptr);

		if (NTCAN_SUCCESS == result && 1 == count)
		{
			canFrame.dataLength = msgCanMessage.len;
			memcpy(canFrame.data, msgCanMessage.data, msgCanMessage.len);
			canFrame.identifier = (msgCanMessage.id & ((1 << 29) - 1));
			canFrame.isExtendedFrame = (NTCAN_20B_BASE == (msgCanMessage.id & NTCAN_20B_BASE));
			canFrame.timestamp_us = msgCanMessage.timestamp * 1000000 / timestampFreq + timestampOffset;
			retVal = true;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		return retVal;
	}

	bool NTCANPlugin::write_frame(const isobus::CANMessageFrame &canFrame)
	{
		NTCAN_RESULT result;
		CMSG_T msgCanMessage{ 0 };
		std::int32_t count = 1;

		msgCanMessage.id = canFrame.isExtendedFrame ? (canFrame.identifier | NTCAN_20B_BASE) : canFrame.identifier;
		msgCanMessage.len = canFrame.dataLength;
		memcpy(msgCanMessage.data, canFrame.data, canFrame.dataLength);

		result = canWriteT(handle, &msgCanMessage, &count, nullptr);

		return (NTCAN_SUCCESS == result && 1 == count);
	}

	bool NTCANPlugin::reconfigure(int channel, int baudrate)
	{
		bool retVal = false;

		if (!get_is_valid())
		{
			net = channel;
			this->baudrate = baudrate;
			retVal = true;
		}
		return retVal;
	}
} // namespace isobus
