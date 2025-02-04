//================================================================================================
/// @file isobus_virtual_terminal_server_managed_working_set.hpp
///
/// @brief Defines a managed working set for a VT server.
/// @author Adrian Del Grosso
///
/// @copyright 2023 Adrian Del Grosso
//================================================================================================
#ifndef ISOBUS_VIRTUAL_TERMINAL_MANAGED_WORKING_SET_HPP
#define ISOBUS_VIRTUAL_TERMINAL_MANAGED_WORKING_SET_HPP

#include <array>
#include <map>
#include <mutex>
#include <thread>

#include "isobus/isobus/can_badge.hpp"
#include "isobus/isobus/can_control_function.hpp"
#include "isobus/isobus/isobus_virtual_terminal_objects.hpp"
#include "isobus/isobus/isobus_virtual_terminal_working_set_base.hpp"
#include "isobus/utility/event_dispatcher.hpp"

namespace isobus
{
	class VirtualTerminalServer;

	/// @brief Defines a managed working set.
	/// @details This class is meant to be used as the basis for a VT server.
	/// It keeps track of one active object pool.
	class VirtualTerminalServerManagedWorkingSet : public VirtualTerminalWorkingSetBase
	{
	public:
		/// @brief Enumerates the states of the processing thread for the object pool
		enum class ObjectPoolProcessingThreadState
		{
			None, ///< Thread has never been started for this working set
			Running, ///< We are currently parsing the object pool
			Success, ///< We have finished parsing the pool successfully and need to respond to the working set
			Fail, ///< The object pool is bad and we need to respond to the working set
			Joined ///< We have sent our response to the working set master and are done parsing
		};

		/// @brief Default constructor
		VirtualTerminalServerManagedWorkingSet();

		/// @brief Constructor that takes a control function to associate with this working set
		/// @param[in] associatedControlFunction The control function to associate with this working set
		VirtualTerminalServerManagedWorkingSet(std::shared_ptr<ControlFunction> associatedControlFunction);

		/// @brief Destructor
		~VirtualTerminalServerManagedWorkingSet() = default;

		/// @brief Starts a thread to parse the received object pool files
		void start_parsing_thread();

		/// @brief Joins the parsing thread
		void join_parsing_thread();

		/// @brief Returns if any object pools are being managed for this working set master
		/// @returns true if at least 1 object pool has been received for this working set master, otherwise false
		bool get_any_object_pools() const;

		/// @brief Returns the state of object pool processing, useful when parsing the object pool
		/// on its own thread.
		/// @returns The state of object pool processing
		ObjectPoolProcessingThreadState get_object_pool_processing_state();

		/// @brief Returns the control function that is the working set master
		/// @returns The control function that is the working set master
		std::shared_ptr<ControlFunction> get_control_function() const;

		/// @brief Returns the working set maintenance message timestamp
		/// @returns The working set maintenance message timestamp in milliseconds
		std::uint32_t get_working_set_maintenance_message_timestamp_ms() const;

		/// @brief Sets the timestamp for when we sent the maintenance message timestamp
		/// @param[in] value New timestamp value in milliseconds
		void set_working_set_maintenance_message_timestamp_ms(std::uint32_t value);

		/// @brief Saves an event callback handle for the lifetime of this object
		/// which is useful for keeping track of callback lifetimes in a VT server
		/// @param[in] callbackHandle The event callback handle to save
		void save_callback_handle(isobus::EventCallbackHandle callbackHandle);

		/// @brief Clears all event callback handles for the this working set
		/// which is useful if you want to stop drawing this working set
		void clear_callback_handles();

		/// @brief Tells the server where this pool originated from.
		/// @returns True if this pool was loaded via a Load Version Command, otherwise false (transferred normally)
		bool get_was_object_pool_loaded_from_non_volatile_memory() const;

		/// @brief Tells the server where this pool originated from.
		/// @param[in] value True if this pool was loaded via a Load Version Command, otherwise false (transferred normally)
		void set_was_object_pool_loaded_from_non_volatile_memory(bool value, CANLibBadge<VirtualTerminalServer>);

		/// @brief Sets the object ID of the currently focused object
		/// @param[in] objectID The object ID to set as the focused object
		void set_object_focus(std::uint16_t objectID);

		/// @brief Returns the object ID of the currently focused object
		/// @returns The object ID of the currently focused object
		std::uint16_t get_object_focus() const;

		/// @brief Sets the timestamp for when we received the last auxiliary input maintenance message
		/// from the client.
		/// @param[in] value New timestamp value in milliseconds
		void set_auxiliary_input_maintenance_timestamp_ms(std::uint32_t value);

		/// @brief Returns the timestamp for when we received the last auxiliary input maintenance message
		/// from the client.
		/// @returns The timestamp for when we received the last auxiliary input maintenance message
		std::uint32_t get_auxiliary_input_maintenance_timestamp_ms() const;

		/// @brief Marks the working set for deletion/deactivation by the server.
		/// The server will call this when object pool deletion is requested for this working set
		/// by the appropriate working set master.
		void request_deletion();

		/// @brief Returns if the server has marked this working set for deletion
		/// @returns true if the working set should be deleted, otherwise false
		bool is_deletion_requested() const;

		/// @brief Set the IOP size used for download percentage calculations
		/// @param[in] newIopSize IOP size in bytes
		void set_iop_size(std::uint32_t newIopSize);

		/// @brief Function to retrive the IOP load progress
		/// @returns state of the IOP loading in percentage (0-100.0). Returns 0 if the IOP size is not set.
		float iop_load_percentage() const;

		/// @brief Function to check the IOP loading state
		/// @returns returns true if the IOP size is known but the transfer is not finished
		bool is_object_pool_transfer_in_progress() const;

	private:
		/// @brief Sets the object pool processing state to a new value
		/// @param[in] value The new state of processing the object pool
		void set_object_pool_processing_state(ObjectPoolProcessingThreadState value);

		/// @brief The object pool processing thread will execute this function when it runs
		void worker_thread_function();

		std::unique_ptr<std::thread> objectPoolProcessingThread = nullptr; ///< A thread to process the object pool with, since that can be fairly time consuming.
		std::shared_ptr<ControlFunction> workingSetControlFunction = nullptr; ///< Stores the control function associated with this working set
		std::vector<isobus::EventCallbackHandle> callbackHandles; ///< A convenient way to associate callback handles to a working set
		ObjectPoolProcessingThreadState processingState = ObjectPoolProcessingThreadState::None; ///< Stores the state of processing the object pool
		std::uint32_t workingSetMaintenanceMessageTimestamp_ms = 0; ///< A timestamp (in ms) to track sending of the maintenance message
		std::uint32_t auxiliaryInputMaintenanceMessageTimestamp_ms = 0; ///< A timestamp (in ms) to track if/when the working set sent an auxiliary input maintenance message
		std::uint16_t focusedObject = NULL_OBJECT_ID; ///< Stores the object ID of the currently focused object
		bool wasLoadedFromNonVolatileMemory = false; ///< Used to tell the server how this object pool was obtained
		bool workingSetDeletionRequested = false; ///< Used to tell the server to delete this working set
		bool objectPoolTransferInProgress = false; ///< Variable to store the IOP download state
	};
} // namespace isobus

#endif // ISOBUS_VIRTUAL_TERMINAL_MANAGED_WORKING_SET_HPP
