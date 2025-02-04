//================================================================================================
/// @file isobus_virtual_terminal_server_managed_working_set.cpp
///
/// @brief Defines a class that manages a VT server's active working sets.
/// @author Adrian Del Grosso
///
/// @copyright 2023 Adrian Del Grosso
//================================================================================================
#include "isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp"

#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/utility/to_string.hpp"

#include <cstring>

namespace isobus
{
	VirtualTerminalServerManagedWorkingSet::VirtualTerminalServerManagedWorkingSet()
	{
		CANStackLogger::info("[WS]: New VT Server Object Created with no associated control function");
	}

	VirtualTerminalServerManagedWorkingSet::VirtualTerminalServerManagedWorkingSet(std::shared_ptr<ControlFunction> associatedControlFunction) :
	  workingSetControlFunction(associatedControlFunction)
	{
		if (nullptr != associatedControlFunction)
		{
			CANStackLogger::info("[WS]: New VT Server Object Created for CF " + to_string(static_cast<int>(associatedControlFunction->get_NAME().get_full_name())));
		}
		else
		{
			CANStackLogger::info("[WS]: New VT Server Object Created with no associated control function");
		}
	}

	void VirtualTerminalServerManagedWorkingSet::start_parsing_thread()
	{
		if (nullptr == objectPoolProcessingThread)
		{
			objectPoolProcessingThread.reset(new std::thread([this]() { worker_thread_function(); }));
		}
	}

	void VirtualTerminalServerManagedWorkingSet::join_parsing_thread()
	{
		if ((nullptr != objectPoolProcessingThread) && (objectPoolProcessingThread->joinable()))
		{
			objectPoolProcessingThread->join();
			objectPoolProcessingThread = nullptr;
			set_object_pool_processing_state(ObjectPoolProcessingThreadState::Joined);
		}
	}

	bool VirtualTerminalServerManagedWorkingSet::get_any_object_pools() const
	{
		return (!iopFilesRawData.empty());
	}

	VirtualTerminalServerManagedWorkingSet::ObjectPoolProcessingThreadState VirtualTerminalServerManagedWorkingSet::get_object_pool_processing_state()
	{
		const std::lock_guard<std::mutex> lock(managedWorkingSetMutex);
		return processingState;
	}

	std::shared_ptr<ControlFunction> VirtualTerminalServerManagedWorkingSet::get_control_function() const
	{
		return workingSetControlFunction;
	}

	std::uint32_t VirtualTerminalServerManagedWorkingSet::get_working_set_maintenance_message_timestamp_ms() const
	{
		return workingSetMaintenanceMessageTimestamp_ms;
	}

	void VirtualTerminalServerManagedWorkingSet::set_working_set_maintenance_message_timestamp_ms(std::uint32_t value)
	{
		workingSetMaintenanceMessageTimestamp_ms = value;
	}

	void VirtualTerminalServerManagedWorkingSet::save_callback_handle(isobus::EventCallbackHandle callbackHandle)
	{
		callbackHandles.push_back(callbackHandle);
	}

	void VirtualTerminalServerManagedWorkingSet::clear_callback_handles()
	{
		callbackHandles.clear();
	}

	bool VirtualTerminalServerManagedWorkingSet::get_was_object_pool_loaded_from_non_volatile_memory() const
	{
		return wasLoadedFromNonVolatileMemory;
	}

	void VirtualTerminalServerManagedWorkingSet::set_was_object_pool_loaded_from_non_volatile_memory(bool value, CANLibBadge<VirtualTerminalServer>)
	{
		wasLoadedFromNonVolatileMemory = value;
	}

	void VirtualTerminalServerManagedWorkingSet::set_object_focus(std::uint16_t objectID)
	{
		focusedObject = objectID;
	}

	std::uint16_t VirtualTerminalServerManagedWorkingSet::get_object_focus() const
	{
		return focusedObject;
	}

	void VirtualTerminalServerManagedWorkingSet::set_auxiliary_input_maintenance_timestamp_ms(std::uint32_t value)
	{
		auxiliaryInputMaintenanceMessageTimestamp_ms = value;
	}

	std::uint32_t VirtualTerminalServerManagedWorkingSet::get_auxiliary_input_maintenance_timestamp_ms() const
	{
		return auxiliaryInputMaintenanceMessageTimestamp_ms;
	}

	void VirtualTerminalServerManagedWorkingSet::request_deletion()
	{
		workingSetDeletionRequested = true;
	}

	bool VirtualTerminalServerManagedWorkingSet::is_deletion_requested() const
	{
		return workingSetDeletionRequested;
	}

	void VirtualTerminalServerManagedWorkingSet::set_iop_size(std::uint32_t newIopSize)
	{
		const std::lock_guard<std::mutex> lock(managedWorkingSetMutex);
		objectPoolTransferInProgress = true;
		iopSize = newIopSize;
	}

	float VirtualTerminalServerManagedWorkingSet::iop_load_percentage() const
	{
		if (processingState != ObjectPoolProcessingThreadState::None || transferredIopSize > iopSize)
		{
			return 100.0f;
		}

		// if IOP transfer is not completed check if there is an ongoing IOP transfer to us
		auto sessions = CANNetworkManager::CANNetwork.get_active_transport_protocol_sessions(0);
		auto currentTransferredIopSize = transferredIopSize;
		for (const auto &session : sessions)
		{
			if (session->get_source()->get_address() == get_control_function()->get_address() &&
			    (static_cast<std::uint32_t>(CANLibParameterGroupNumber::ECUtoVirtualTerminal) == session->get_parameter_group_number()) &&
			    (session->get_data().size() >= 1) &&
			    (session->get_data().get_byte(0) == 0x11)) // ObjectPoolTransferMessage
			{
				currentTransferredIopSize += session->get_total_bytes_transferred();
			}
		}

		if (currentTransferredIopSize > iopSize)
		{
			return 100.0f;
		}

		return (currentTransferredIopSize / (float)iopSize) * 100.0f;
	}

	void VirtualTerminalServerManagedWorkingSet::set_object_pool_processing_state(ObjectPoolProcessingThreadState value)
	{
		const std::lock_guard<std::mutex> lock(managedWorkingSetMutex);
		processingState = value;
		if (value != ObjectPoolProcessingThreadState::None)
		{
			objectPoolTransferInProgress = false;
		}
	}

	void VirtualTerminalServerManagedWorkingSet::worker_thread_function()
	{
		if (!iopFilesRawData.empty())
		{
			bool lSuccess = true;

			set_object_pool_processing_state(ObjectPoolProcessingThreadState::Running);
			CANStackLogger::info("[WS]: Beginning parsing of object pool. This pool has " +
			                     isobus::to_string(static_cast<int>(iopFilesRawData.size())) +
			                     " IOP components.");
			for (std::size_t i = 0; i < iopFilesRawData.size(); i++)
			{
				if (!parse_iop_into_objects(iopFilesRawData[i].data(), static_cast<std::uint32_t>(iopFilesRawData[i].size())))
				{
					lSuccess = false;
					break;
				}
			}

			if (lSuccess)
			{
				CANStackLogger::info("[WS]: Object pool successfully parsed.");
				set_object_pool_processing_state(ObjectPoolProcessingThreadState::Success);
			}
			else
			{
				CANStackLogger::error("[WS]: Object pool failed to be parsed.");
				set_object_pool_processing_state(ObjectPoolProcessingThreadState::Fail);
			}
		}
		else
		{
			CANStackLogger::error("[WS]: Object pool failed to be parsed.");
			set_object_pool_processing_state(ObjectPoolProcessingThreadState::Fail);
		}
	}

	bool VirtualTerminalServerManagedWorkingSet::is_object_pool_transfer_in_progress() const
	{
		return iop_load_percentage() != 0.0f;
	}

} // namespace isobus
