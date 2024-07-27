//================================================================================================
/// @file isobus_task_controller_client.hpp
///
/// @brief A class to manage a client connection to a ISOBUS field computer's task controller
/// @author Adrian Del Grosso
///
/// @copyright 2022 The Open-Agriculture Developers
//================================================================================================
#ifndef ISOBUS_TASK_CONTROLLER_CLIENT_HPP
#define ISOBUS_TASK_CONTROLLER_CLIENT_HPP

#include "isobus/isobus/can_internal_control_function.hpp"
#include "isobus/isobus/can_partnered_control_function.hpp"
#include "isobus/isobus/isobus_device_descriptor_object_pool.hpp"
#include "isobus/isobus/isobus_language_command_interface.hpp"
#include "isobus/utility/processing_flags.hpp"

#include <list>
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
#include <thread>
#endif

namespace isobus
{
	/// @brief A class to manage a client connection to a ISOBUS field computer's task controller or data logger
	class TaskControllerClient
	{
	public:
		/// @brief Enumerates the different internal state machine states
		enum class StateMachineState
		{
			Disconnected, ///< Not communicating with the TC
			WaitForStartUpDelay, ///< Client is waiting for the mandatory 6s startup delay
			WaitForServerStatusMessage, ///< Client is waiting to identify the TC via reception of a valid status message
			SendWorkingSetMaster, ///< Client initiating communication with TC by sending the working set master message
			SendStatusMessage, ///< Enables sending the status message
			RequestVersion, ///< Requests the TC version and related data from the TC
			WaitForRequestVersionResponse, ///< Waiting for the TC to respond to a request for its version
			WaitForRequestVersionFromServer, ///< Waiting to see if the TC will request our version (optional)
			SendRequestVersionResponse, ///< Sending our response to the TC's request for out version information
			RequestLanguage, ///< Client is requesting the language command PGN from the TC
			WaitForLanguageResponse, ///< Waiting for a response to our request for the language command PGN
			ProcessDDOP, ///< Client is processing the DDOP into a binary DDOP and validating object IDs in the pool
			RequestStructureLabel, ///< Client is requesting the DDOP structure label that the TC has (if any)
			WaitForStructureLabelResponse, ///< Client is waiting for the TC to respond to our request for its structure label
			RequestLocalizationLabel, ///< Client is requesting the DDOP localization label the TC has for us (if any)
			WaitForLocalizationLabelResponse, ///< Waiting for a response to our request for the localization label from the TC
			SendDeleteObjectPool, ///< Client is sending a request to the TC to delete its current copy of our object pool
			WaitForDeleteObjectPoolResponse, ///< Waiting for a response to our request to delete our object pool off the TC
			SendRequestTransferObjectPool, ///< Client is requesting to transfer the DDOP to the TC
			WaitForRequestTransferObjectPoolResponse, ///< Waiting for a response to our request to transfer the DDOP to the TC
			BeginTransferDDOP, ///< Client is initiating the DDOP transfer
			WaitForDDOPTransfer, ///< The DDOP transfer in ongoing. Client is waiting for a callback from the transport layer.
			WaitForObjectPoolTransferResponse, ///< DDOP has transferred. Waiting for a response to our object pool transfer.
			SendObjectPoolActivate, ///< Client is sending the activate object pool message
			WaitForObjectPoolActivateResponse, ///< Client is waiting for a response to its request to activate the object pool
			Connected, ///< TC is connected
			DeactivateObjectPool, ///< Client is shutting down and is therefore sending the deactivate object pool message
			WaitForObjectPoolDeactivateResponse ///< Client is waiting for a response to the deactivate object pool message
		};

		/// @brief Enumerates the different task controller versions
		enum class Version : std::uint8_t
		{
			DraftInternationalStandard = 0, ///< The version of the DIS (draft International Standard).
			FinalDraftInternationalStandardFirstEdition = 1, ///< The version of the FDIS.1 (final draft International Standard, first edition).
			FirstPublishedEdition = 2, ///< The version of the FDIS.2 and the first edition published as an International Standard.
			SecondEditionDraft = 3, ///< The version of the second edition published as a draft International Standard(E2.DIS).
			SecondPublishedEdition = 4, ///< The version of the second edition published as the final draft International Standard(E2.FDIS) and as the International Standard(E2.IS)
			Unknown = 0xFF
		};

		/// @brief Enumerates the bits stored in our version data that we send to the TC when handshaking
		enum class ServerOptions : std::uint8_t
		{
			SupportsDocumentation = 0x01,
			SupportsTCGEOWithoutPositionBasedControl = 0x02,
			SupportsTCGEOWithPositionBasedControl = 0x04,
			SupportsPeerControlAssignment = 0x08,
			SupportsImplementSectionControlFunctionality = 0x10,
			ReservedOption1 = 0x20,
			ReservedOption2 = 0x40,
			ReservedOption3 = 0x80
		};

		/// @brief Used to describe the triggers to set up by default when the
		/// the TC server requests the default process data from the client.
		struct DefaultProcessDataSettings
		{
			std::int32_t timeTriggerInterval_ms = 0; ///< The time interval for sending the data element specified by the data dictionary identifier.
			std::int32_t distanceTriggerInterval_mm = 0; ///< The distance interval for sending the data element specified by the data dictionary identifier.
			std::int32_t minimumWithinThreshold = 0; ///< The value of this data element is sent to the TC or DL when the value is higher than the threshold value.
			std::int32_t maximumWithinThreshold = 0; ///< The value of this data element is sent to the TC or DL when the value is lower than the threshold value.
			std::int32_t changeThreshold = 0; ///< The value of this data element is sent to the TC or DL when the value change is higher than or equal to the change threshold since last transmission.
			bool enableTimeTrigger = false; ///< Enable the time trigger
			bool enableDistanceTrigger = false; ///< Enable the distance trigger
			bool enableMinimumWithinThresholdTrigger = false; ///< Enable the minimum within threshold trigger
			bool enableMaximumWithinThresholdTrigger = false; ///< Enable the maximum within threshold trigger
			bool enableChangeThresholdTrigger = false; ///< Enable the change threshold trigger
		};

		/// @brief A callback for handling a value request command from the TC
		using RequestValueCommandCallback = bool (*)(std::uint16_t elementNumber,
		                                             std::uint16_t DDI,
		                                             std::int32_t &processVariableValue,
		                                             void *parentPointer);

		/// @brief A callback for handling a default process data request from the TC.
		/// This callback is used to set up the default process data settings for a process data variable
		/// when the TC requests the default process data from the client. When this callback is called, you should
		/// edit the content of the `returnedSettings` parameter to set up the triggers you want to use by default for
		/// this process data variable.
		using DefaultProcessDataRequestedCallback = bool (*)(std::uint16_t elementNumber,
		                                                     std::uint16_t DDI,
		                                                     DefaultProcessDataSettings &returnedSettings,
		                                                     void *parentPointer);

		/// @brief A callback for handling a set value command from the TC
		using ValueCommandCallback = bool (*)(std::uint16_t elementNumber,
		                                      std::uint16_t DDI,
		                                      std::int32_t processVariableValue,
		                                      void *parentPointer);

		/// @brief The constructor for a TaskControllerClient
		/// @param[in] partner The TC server control function
		/// @param[in] clientSource The internal control function to communicate from
		/// @param[in] primaryVT Pointer to our primary VT. This is optional (can be nullptr), but should be provided if possible to provide the best compatibility to TC < version 4.
		TaskControllerClient(std::shared_ptr<PartneredControlFunction> partner, std::shared_ptr<InternalControlFunction> clientSource, std::shared_ptr<PartneredControlFunction> primaryVT);

		/// @brief Destructor for the client
		~TaskControllerClient();

		// Setup Functions
		/// @brief This function starts the state machine. Call this once you have created your DDOP, set up the client capabilities, and are ready to connect.
		/// @param[in] spawnThread The client will start a thread to manage itself if this parameter is true. Otherwise you must update it cyclically
		/// by calling the `update` function.
		void initialize(bool spawnThread);

		/// @brief This function adds a callback that will be called when the TC requests the default process data from the client.
		/// When starting a task, the task controller will often send a request for the default process data from the client.
		/// When the stack receives those messages, it will call each callback you've added add with this function until one returns true.
		/// When a callback returns true, the stack will use the settings provided by that callback to set up the triggers for the process data variable.
		/// The stack will then send the process data to the TC, and set up the triggers for the process data variable as requested by the callback.
		/// @note The TC may overwrite your desired trigger settings if it wants to. The values you set here are just defaults.
		/// @param[in] callback The callback to add
		/// @param[in] parentPointer A generic context variable that will be passed into the associated callback when it gets called
		void add_default_process_data_requested_callback(DefaultProcessDataRequestedCallback callback, void *parentPointer);

		/// @brief This adds a callback that will be called when the TC requests the value of one of your variables.
		/// @details The task controller will often send a request for the value of a process data variable.
		/// When the stack receives those messages, it will call this callback to request the value from your
		/// application. You must provide the value at that time for the associated process data variable identified
		/// by its element number and DDI.
		/// @param[in] callback The callback to add
		/// @param[in] parentPointer A generic context variable that will be passed into the associated callback when it gets called
		void add_request_value_callback(RequestValueCommandCallback callback, void *parentPointer);

		/// @brief Adds a callback that will be called when the TC commands a new value for one of your variables.
		/// @details The task controller will often send a command to set one of your process data variables to a new value.
		/// This callback will get called when that happens, and you will need to set the variable to the commanded value in your
		/// application.
		/// @param[in] callback The callback to add
		/// @param[in] parentPointer A generic context variable that will be passed into the associated callback when it gets called
		void add_value_command_callback(ValueCommandCallback callback, void *parentPointer);

		/// @brief Removes the specified callback from the list of default process data requested callbacks
		/// @param[in] callback The callback to remove
		/// @param[in] parentPointer parent pointer associated to the callback being removed
		void remove_default_process_data_requested_callback(DefaultProcessDataRequestedCallback callback, void *parentPointer);

		/// @brief Removes the specified callback from the list of value request callbacks
		/// @param[in] callback The callback to remove
		/// @param[in] parentPointer parent pointer associated to the callback being removed
		void remove_request_value_callback(RequestValueCommandCallback callback, void *parentPointer);

		/// @brief Removes the specified callback from the list of value command callbacks
		/// @param[in] callback The callback to remove
		/// @param[in] parentPointer parent pointer associated to the callback being removed
		void remove_value_command_callback(ValueCommandCallback callback, void *parentPointer);

		/// @brief A convenient way to set all client options at once instead of calling the individual setters
		/// @details This function sets up the parameters that the client will report to the TC server.
		/// These parameters should be tailored to your specific application.
		/// @note This version of the configure function takes a DeviceDescriptorObjectPool.
		/// The other versions of the configure function take various other kinds of DDOP.
		/// @param[in] DDOP The device descriptor object pool to upload to the TC
		/// @param[in] maxNumberBoomsSupported Configures the max number of booms the client supports
		/// @param[in] maxNumberSectionsSupported Configures the max number of sections supported by the client for section control
		/// @param[in] maxNumberChannelsSupportedForPositionBasedControl Configures the max number of channels supported by the client for position based control
		/// @param[in] reportToTCSupportsDocumentation Denotes if your app supports documentation
		/// @param[in] reportToTCSupportsTCGEOWithoutPositionBasedControl Denotes if your app supports TC-GEO without position based control
		/// @param[in] reportToTCSupportsTCGEOWithPositionBasedControl Denotes if your app supports TC-GEO with position based control
		/// @param[in] reportToTCSupportsPeerControlAssignment Denotes if your app supports peer control assignment
		/// @param[in] reportToTCSupportsImplementSectionControl Denotes if your app supports implement section control
		void configure(std::shared_ptr<DeviceDescriptorObjectPool> DDOP,
		               std::uint8_t maxNumberBoomsSupported,
		               std::uint8_t maxNumberSectionsSupported,
		               std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
		               bool reportToTCSupportsDocumentation,
		               bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
		               bool reportToTCSupportsTCGEOWithPositionBasedControl,
		               bool reportToTCSupportsPeerControlAssignment,
		               bool reportToTCSupportsImplementSectionControl);

		/// @brief A convenient way to set all client options at once instead of calling the individual setters
		/// @details This function sets up the parameters that the client will report to the TC server.
		/// These parameters should be tailored to your specific application.
		/// @note This version of the configure function takes a pointer to an array of bytes.
		/// The other versions of the configure function take various other kinds of DDOP.
		/// @param[in] binaryDDOP The device descriptor object pool to upload to the TC
		/// @param[in] DDOPSize The number of bytes in the binary DDOP that will be uploaded
		/// @param[in] maxNumberBoomsSupported Configures the max number of booms the client supports
		/// @param[in] maxNumberSectionsSupported Configures the max number of sections supported by the client for section control
		/// @param[in] maxNumberChannelsSupportedForPositionBasedControl Configures the max number of channels supported by the client for position based control
		/// @param[in] reportToTCSupportsDocumentation Denotes if your app supports documentation
		/// @param[in] reportToTCSupportsTCGEOWithoutPositionBasedControl Denotes if your app supports TC-GEO without position based control
		/// @param[in] reportToTCSupportsTCGEOWithPositionBasedControl Denotes if your app supports TC-GEO with position based control
		/// @param[in] reportToTCSupportsPeerControlAssignment Denotes if your app supports peer control assignment
		/// @param[in] reportToTCSupportsImplementSectionControl Denotes if your app supports implement section control
		void configure(std::uint8_t const *binaryDDOP,
		               std::uint32_t DDOPSize,
		               std::uint8_t maxNumberBoomsSupported,
		               std::uint8_t maxNumberSectionsSupported,
		               std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
		               bool reportToTCSupportsDocumentation,
		               bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
		               bool reportToTCSupportsTCGEOWithPositionBasedControl,
		               bool reportToTCSupportsPeerControlAssignment,
		               bool reportToTCSupportsImplementSectionControl);

		/// @brief A convenient way to set all client options at once instead of calling the individual setters
		/// @details This function sets up the parameters that the client will report to the TC server.
		/// These parameters should be tailored to your specific application.
		/// @note This version of the configure function takes a vector of bytes, and stores a copy of it.
		/// The other versions of the configure function take various other kinds of DDOP.
		/// @param[in] binaryDDOP The device descriptor object pool to upload to the TC
		/// @param[in] maxNumberBoomsSupported Configures the max number of booms the client supports
		/// @param[in] maxNumberSectionsSupported Configures the max number of sections supported by the client for section control
		/// @param[in] maxNumberChannelsSupportedForPositionBasedControl Configures the max number of channels supported by the client for position based control
		/// @param[in] reportToTCSupportsDocumentation Denotes if your app supports documentation
		/// @param[in] reportToTCSupportsTCGEOWithoutPositionBasedControl Denotes if your app supports TC-GEO without position based control
		/// @param[in] reportToTCSupportsTCGEOWithPositionBasedControl Denotes if your app supports TC-GEO with position based control
		/// @param[in] reportToTCSupportsPeerControlAssignment Denotes if your app supports peer control assignment
		/// @param[in] reportToTCSupportsImplementSectionControl Denotes if your app supports implement section control
		void configure(std::shared_ptr<std::vector<std::uint8_t>> binaryDDOP,
		               std::uint8_t maxNumberBoomsSupported,
		               std::uint8_t maxNumberSectionsSupported,
		               std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
		               bool reportToTCSupportsDocumentation,
		               bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
		               bool reportToTCSupportsTCGEOWithPositionBasedControl,
		               bool reportToTCSupportsPeerControlAssignment,
		               bool reportToTCSupportsImplementSectionControl);

		/// @brief Calling this function will reset the task controller client's connection
		/// with the TC server, and cause it to reconnect after a short delay.
		void restart();

		// Calling this will stop the worker thread if it exists
		/// @brief Terminates the client and joins the worker thread if applicable
		void terminate();

		/// @brief Returns the internal control function being used by the interface to send messages
		/// @returns The internal control function being used by the interface to send messages
		std::shared_ptr<InternalControlFunction> get_internal_control_function() const;

		/// @brief Returns the control function of the TC server with which this TC client communicates.
		/// @returns The partner control function for the TC server
		std::shared_ptr<PartneredControlFunction> get_partner_control_function() const;

		/// @brief Returns the previously configured number of booms supported by the client
		/// @returns The previously configured number of booms supported by the client
		std::uint8_t get_number_booms_supported() const;

		/// @brief Returns the previously configured number of section supported by the client
		/// @returns The previously configured number of booms supported by the client
		std::uint8_t get_number_sections_supported() const;

		/// @brief Returns the previously configured number of channels supported for position based control
		/// @returns The previously configured number of channels supported for position based control
		std::uint8_t get_number_channels_supported_for_position_based_control() const;

		/// @brief Returns if the client has been configured to report that it supports documentation to the TC
		/// @returns `true` if the client has been configured to report that it supports documentation, otherwise `false`
		bool get_supports_documentation() const;

		/// @brief Returns if the client has been configured to report that it supports TC-GEO without position based control to the TC
		/// @returns `true` if the client has been configured to report that it supports TC-GEO without position based control, otherwise `false`
		bool get_supports_tcgeo_without_position_based_control() const;

		/// @brief Returns if the client has been configured to report that it supports TC-GEO with position based control to the TC
		/// @returns `true` if the client has been configured to report that it supports TC-GEO with position based control, otherwise `false`
		bool get_supports_tcgeo_with_position_based_control() const;

		/// @brief Returns if the client has been configured to report that it supports peer control assignment to the TC
		/// @returns `true` if the client has been configured to report that it supports peer control assignment, otherwise `false`
		bool get_supports_peer_control_assignment() const;

		/// @brief Returns if the client has been configured to report that it supports implement section control to the TC
		/// @returns `true` if the client has been configured to report that it supports implement section control, otherwise `false`
		bool get_supports_implement_section_control() const;

		/// @brief Returns if the client has been initialized
		/// @note This does not mean that the client is connected to the TC server
		/// @returns true if the client has been initialized
		bool get_is_initialized() const;

		/// @brief Check whether the client is connected to the TC server
		/// @returns true if connected, false otherwise
		bool get_is_connected() const;

		/// @brief Returns if a task is active as indicated by the TC
		/// @attention Some TCs will report they are always in a task rather than properly reporting this.
		/// For example, John Deere TCs have a bad habit of doing this.
		/// Use caution before relying on the TC's task status.
		/// @returns `true` if the TC is connected and the TC is reporting it is in a task, otherwise `false`
		bool get_is_task_active() const;

		/// @brief Returns the current state machine state
		/// @returns The current internal state machine state
		StateMachineState get_state() const;

		/// @brief Returns the number of booms that the connected TC supports for section control
		/// @returns Number of booms that the connected TC supports for section control
		std::uint8_t get_connected_tc_number_booms_supported() const;

		/// @brief Returns the number of sections that the connected TC supports for section control
		/// @returns Number of sections that the connected TC supports for section control
		std::uint8_t get_connected_tc_number_sections_supported() const;

		/// @brief Returns the number of channels that the connected TC supports for position control
		/// @returns Number of channels that the connected TC supports for position control
		std::uint8_t get_connected_tc_number_channels_supported() const;

		/// @brief Returns the maximum boot time in seconds reported by the connected TC
		/// @returns Maximum boot time (seconds) reported by the connected TC, or 0xFF if that info is not available.
		std::uint8_t get_connected_tc_max_boot_time() const;

		/// @brief Returns if the connected TC supports a certain option
		/// @param[in] option The option to check against
		/// @returns `true` if the option was reported as "supported" by the TC, otherwise `false`
		bool get_connected_tc_option_supported(ServerOptions option) const;

		/// @brief Returns the version of the connected task controller
		/// @returns The version reported by the connected task controller
		Version get_connected_tc_version() const;

		/// @brief Tells the TC client that a value was changed or the TC client needs to command
		/// a value to the TC server.
		/// @details If you provide on-change triggers in your DDOP, this is how you can request the TC client
		/// to update the TC server on the current value of your process data variables.
		/// @param[in] elementNumber The element number of the process data variable that changed
		/// @param[in] DDI The DDI of the process data variable that changed
		void on_value_changed_trigger(std::uint16_t elementNumber, std::uint16_t DDI);

		/// @brief Sends a broadcast request to TCs to identify themselves.
		/// @details Upon receipt of this message, the TC shall display, for a period of 3 s, the TC Number
		/// @returns `true` if the message was sent, otherwise `false`
		bool request_task_controller_identification() const;

		/// @brief If the TC client is connected to a TC, calling this function will
		/// cause the TC client interface to delete the currently active DDOP, re-upload it,
		/// then reactivate it using the pool passed into the parameter of this function.
		/// This process is faster than restarting the whole interface, and you have to
		/// call it if you change certain things in your DDOP at runtime after the DDOP has already been activated.
		/// @param[in] binaryDDOP The updated device descriptor object pool to upload to the TC
		/// @returns true if the interface accepted the command to re-upload the pool, or false if the command cannot be handled right now
		bool reupload_device_descriptor_object_pool(std::shared_ptr<std::vector<std::uint8_t>> binaryDDOP);

		/// @brief If the TC client is connected to a TC, calling this function will
		/// cause the TC client interface to delete the currently active DDOP, re-upload it,
		/// then reactivate it using the pool passed into the parameter of this function.
		/// This process is faster than restarting the whole interface, and you have to
		/// call it if you change certain things in your DDOP at runtime after the DDOP has already been activated.
		/// @param[in] binaryDDOP The updated device descriptor object pool to upload to the TC
		/// @param[in] DDOPSize The number of bytes in the binary DDOP that will be uploaded
		/// @returns true if the interface accepted the command to re-upload the pool, or false if the command cannot be handled right now
		bool reupload_device_descriptor_object_pool(std::uint8_t const *binaryDDOP, std::uint32_t DDOPSize);

		/// @brief If the TC client is connected to a TC, calling this function will
		/// cause the TC client interface to delete the currently active DDOP, re-upload it,
		/// then reactivate it using the pool passed into the parameter of this function.
		/// This process is faster than restarting the whole interface, and you have to
		/// call it if you change certain things in your DDOP at runtime after the DDOP has already been activated.
		/// @param[in] DDOP The updated device descriptor object pool to upload to the TC
		/// @returns true if the interface accepted the command to re-upload the pool, or false if the command cannot be handled right now
		bool reupload_device_descriptor_object_pool(std::shared_ptr<DeviceDescriptorObjectPool> DDOP);

		/// @brief If your application has any distance triggers set up in the DDOP, you can call this function
		/// to update the distance that the TC client uses to determine if it should send a process data value.
		/// This should be the total distance driven by the vehicle since the application started. Not the difference between the last call and this call!
		/// @param[in] distance The total, absolute distance in millimeters the vehicle has driven
		void set_distance(std::uint32_t distance);

		/// @brief The cyclic update function for this interface.
		/// @note This function may be called by the TC worker thread if you called
		/// initialize with a parameter of `true`, otherwise you must call it
		/// yourself at some interval.
		void update();

		/// @brief Used to determine the language and unit systems in use by the TC server
		LanguageCommandInterface languageCommandInterface;

	protected:
		/// @brief Enumerates the different Process Data commands from ISO11783-10 Table B.1
		enum class ProcessDataCommands : std::uint8_t
		{
			TechnicalCapabilities = 0x00, ///< Used for determining the technical capabilities of a TC, DL, or client.
			DeviceDescriptor = 0x01, ///< Subcommand for the transfer and management of device descriptors
			RequestValue = 0x02, ///< The value of the data entity specified by the data dictionary identifier is requested.
			Value = 0x03, ///< This command is used both to answer a request value command and to set the value of a process data entity.
			MeasurementTimeInterval = 0x04, ///< The process data value is the time interval for sending the data element specified by the data dictionary identifier.
			MeasurementDistanceInterval = 0x05, ///< The process data value is the distance interval for sending the data element specified by the data dictionary identifier.
			MeasurementMinimumWithinThreshold = 0x06, ///< The client has to send the value	of this data element to the TC or DL when the value is higher than the threshold value
			MeasurementMaximumWithinThreshold = 0x07, ///< The client has to send the value of this data element to the TC or DL when the value is lower than the threshold value.
			MeasurementChangeThreshold = 0x08, ///< The client has to send the value of this data element to the TC or DL when the value change is higher than or equal to the change threshold since last transmission.
			PeerControlAssignment = 0x09, ///< This message is used to establish a connection between a	setpoint value source and a setpoint value user
			SetValueAndAcknowledge = 0x0A, ///< This command is used to set the value of a process data entity and request a reception acknowledgment from the recipient
			Reserved1 = 0x0B, ///< Reserved.
			Reserved2 = 0x0C, ///< Reserved.
			ProcessDataAcknowledge = 0x0D, ///< Message is a Process Data Acknowledge (PDACK).
			StatusMessage = 0x0E, ///< Message is a Task Controller Status message
			ClientTask = 0x0F ///< Sent by the client
		};

		/// @brief Enumerates the subcommands within the technical data message group
		enum class TechnicalDataMessageCommands : std::uint8_t
		{
			ParameterRequestVersion = 0x00, ///< The Request Version message allows the TC, DL, and the client to determine the ISO 11783-10 version of the implementation
			ParameterVersion = 0x01, ///< The Version message is sent in response to the request version message and contains the ISO 11783-10 version information of the TC, DL, or client implementation
			IdentifyTaskController = 0x02 ///< Upon receipt of this message, the TC shall display, for a period of 3 s, the TC Number.
		};

		/// @brief Enumerates the subcommands within the device descriptor command message group
		enum class DeviceDescriptorCommands : std::uint8_t
		{
			RequestStructureLabel = 0x00, ///< Allows the client to determine the availability of the requested	device descriptor structure
			StructureLabel = 0x01, ///< The Structure Label message is sent by the TC or DL to inform the client about the availability of the requested version of the device descriptor structure
			RequestLocalizationLabel = 0x02, ///< Allows the client to determine the availability of the requested device descriptor localization
			LocalizationLabel = 0x03, ///< Sent by the TC or DL to inform the client about the availability of the requested localization version of the device descriptor
			RequestObjectPoolTransfer = 0x04, ///< The Request Object-pool Transfer message allows the client to determine whether it is allowed to transfer(part of) the device descriptor object pool to the TC
			RequestObjectPoolTransferResponse = 0x05, ///< Sent in response to Request Object-pool Transfer message
			ObjectPoolTransfer = 0x06, ///< Enables the client to transfer (part of) the device descriptor object pool to the TC
			ObjectPoolTransferResponse = 0x07, ///< Response to an object pool transfer
			ObjectPoolActivateDeactivate = 0x08, ///< sent by a client to complete its connection procedure to a TC or DL or to disconnect from a TC or DL.
			ObjectPoolActivateDeactivateResponse = 0x09, ///< sent by a client to complete its connection procedure to a TC or DL or to disconnect from a TC or DL.
			ObjectPoolDelete = 0x0A, ///< This is a message to delete the device descriptor object pool for the client that sends this message.
			ObjectPoolDeleteResponse = 0x0B, ///< TC response to a Object-pool Delete message
			ChangeDesignator = 0x0C, ///< This message is used to update the designator of an object.
			ChangeDesignatorResponse = 0x0D ///< Sent in response to Change Designator message
		};

		/// @brief Stores data related to requests and commands from the TC
		struct ProcessDataCallbackInfo
		{
			/// @brief Allows easy comparison of callback data
			/// @param obj the object to compare against
			/// @returns true if the ddi and element numbers of the provided objects match, otherwise false
			bool operator==(const ProcessDataCallbackInfo &obj) const;
			std::int32_t processDataValue; ///< The value of the value set command
			std::int32_t lastValue; ///< Used for measurement commands to store timestamp or previous values
			std::uint16_t elementNumber; ///< The element number for the command
			std::uint16_t ddi; ///< The DDI for the command
			bool ackRequested; ///< Stores if the TC used the mux that also requires a PDACK
			bool thresholdPassed; ///< Used when the structure is being used to track measurement command thresholds to know if the threshold has been passed
		};

		/// @brief The data callback passed to the network manger's send function for the transport layer messages
		/// @details We upload the data with callbacks to avoid making yet another complete copy of the pool to
		/// accommodate the multiplexer that needs to get passed to the transport layer message's first byte.
		/// @param[in] callbackIndex The number of times the callback has been called
		/// @param[in] bytesOffset The byte offset at which to get pool data
		/// @param[in] numberOfBytesNeeded The number of bytes the protocol needs to send another frame (usually 7)
		/// @param[out] chunkBuffer A pointer through which the data should be returned to the protocol
		/// @param[in] parentPointer A context variable that is passed back through the callback
		/// @returns true if the data was successfully returned via the callback
		static bool process_internal_object_pool_upload_callback(std::uint32_t callbackIndex,
		                                                         std::uint32_t bytesOffset,
		                                                         std::uint32_t numberOfBytesNeeded,
		                                                         std::uint8_t *chunkBuffer,
		                                                         void *parentPointer);

		/// @brief Adds a measurement change threshold to the queue of maintained triggers, checks for duplicates.
		/// @param[in] info The information to add to the queue
		void add_measurement_change_threshold(ProcessDataCallbackInfo &info);

		/// @brief Adds a measurement distance interval to the queue of maintained triggers, checks for duplicates.
		/// @param[in] info The information to add to the queue
		void add_measurement_distance_interval(ProcessDataCallbackInfo &info);

		/// @brief Adds a measurement time interval to the queue of maintained triggers, checks for duplicates.
		/// @param[in] info The information to add to the queue
		void add_measurement_time_interval(ProcessDataCallbackInfo &info);

		/// @brief Adds a measurement max threshold to the queue of maintained triggers, checks for duplicates.
		/// @param[in] info The information to add to the queue
		void add_measurement_maximum_threshold(ProcessDataCallbackInfo &info);

		/// @brief Adds a measurement minimum threshold to the queue of maintained triggers, checks for duplicates.
		/// @param[in] info The information to add to the queue
		void add_measurement_minimum_threshold(ProcessDataCallbackInfo &info);

		/// @brief Clears all queued TC commands and responses
		void clear_queues();

		/// @brief Checks if a DDOP was provided via one of the configure functions
		/// @returns true if a DDOP was provided, otherwise false
		bool get_was_ddop_supplied() const;

		/// @brief Sets up triggers for a process data variable based on the default process data settings
		/// @param[in] processDataObject The process data variable, used to validate trigger compatibility with the DDOP
		/// @param[in] elementNumber The element number of the process data variable
		/// @param[in] DDI The DDI of the process data variable
		/// @param[in] settings The default process data settings to use for setting up the triggers
		void populate_any_triggers_from_settings(std::shared_ptr<task_controller_object::DeviceProcessDataObject> processDataObject,
		                                         std::uint16_t elementNumber,
		                                         std::uint16_t DDI,
		                                         const DefaultProcessDataSettings &settings);

		/// @brief Searches the DDOP for a device object and stores that object's structure and localization labels
		void process_labels_from_ddop();

		/// @brief Processes queued TC requests and commands. Calls the user's callbacks if needed.
		void process_queued_commands();

		/// @brief Processes measurement threshold/interval commands
		void process_queued_threshold_commands();

		/// @brief Processes a CAN message destined for any TC client
		/// @param[in] message The CAN message being received
		/// @param[in] parentPointer A context variable to find the relevant TC client class
		static void process_rx_message(const CANMessage &message, void *parentPointer);

		/// @brief The callback passed to the network manager's send function to know when a Tx is completed
		/// @param[in] parameterGroupNumber The parameter group number of the message that was sent
		/// @param[in] dataLength The number of bytes sent
		/// @param[in] sourceControlFunction The control function that sent the message
		/// @param[in] destinationControlFunction The control function that received the message
		/// @param[in] successful Whether the message was sent successfully
		/// @param[in] parentPointer A context variable that is passed back through the callback
		static void process_tx_callback(std::uint32_t parameterGroupNumber,
		                                std::uint32_t dataLength,
		                                std::shared_ptr<InternalControlFunction> sourceControlFunction,
		                                std::shared_ptr<ControlFunction> destinationControlFunction,
		                                bool successful,
		                                void *parentPointer);

		/// @brief Sends the delete object pool command to the TC
		/// @details This is a message to delete the device descriptor object pool for the client that sends this message. The
		/// Object pool Delete message enables a client to delete the entire device descriptor object pool before sending an
		/// updated or changed device descriptor object pool with the object pool transfer message.
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_delete_object_pool() const;

		/// @brief Sends a process data message with 1 mux byte and all 0xFFs as payload
		/// @details This just reduces code duplication by consolidating common message formats
		/// @param[in] multiplexer The multiplexer to use for the message
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_generic_process_data(std::uint8_t multiplexer) const;

		/// @brief Sends the activate object pool message
		/// @details This message is sent by a client to complete its connection procedure to a TC
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_object_pool_activate() const;

		/// @brief Sends the deactivate object pool message
		/// @details This message is sent by a client to disconnect from a TC
		/// @returns `true` if the message was sent otherwise `false`
		bool send_object_pool_deactivate() const;

		/// @brief Sends a Process Data ACK
		/// @param[in] elementNumber The element number being acked
		/// @param[in] ddi The DDI being acked
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_pdack(std::uint16_t elementNumber, std::uint16_t ddi) const;

		/// @brief Sends a request to the TC for its localization label
		/// @details The Request Localization Label message allows the client to determine the availability of the requested
		/// device descriptor localization at the TC or DL.If the requested localization label is present,
		/// a localization label message with the requested localization label shall be transmitted by the TC or DL
		/// to the sender of the Request Localization Label message. Otherwise, a localization label message with all
		/// localization label bytes set to value = 0xFF shall be transmitted by the TC or DL to the sender of the
		/// Request Localization Label message.
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_request_localization_label() const;

		/// @brief Sends a request to the TC indicating we wish to transfer an object pool
		/// @details The Request Object-pool Transfer message allows the client to determine whether it is allowed to
		/// transfer(part of) the device descriptor object pool to the TC or DL.
		/// @returns `true` if the message was sent, otherwise false
		bool send_request_object_pool_transfer() const;

		/// @brief Sends a request to the TC for its structure label
		/// @details The Request Structure Label message allows the client to determine the availability of the requested
		/// device descriptor structure at the TC. If the requested structure label is present, a structure label
		/// message with the requested structure label shall be transmitted by the TC or DL to the sender
		/// of the Request Structure Label message. Otherwise, a structure label message with 7 structure
		/// label bytes set to value = 0xFF shall be transmitted by the TC or DL to the sender of the Request Structure Label message
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_request_structure_label() const;

		/// @brief Sends the response to a request for version from the TC
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_request_version_response() const;

		/// @brief Sends the status message to the TC
		/// @returns `true` if the message was sent, otherwise false
		bool send_status() const;

		/// @brief Sends the value command message for a specific DDI/Element number combo
		/// @param[in] elementNumber The element number for the command
		/// @param[in] ddi The DDI for the command
		/// @param[in] value The value to send
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_value_command(std::uint16_t elementNumber, std::uint16_t ddi, std::int32_t value) const;

		/// @brief Sends the version request message to the TC
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_version_request() const;

		/// @brief Sends the working set master message
		/// @returns `true` if the message was sent, otherwise false
		bool send_working_set_master() const;

		/// @brief Sets the common items found in all versions of `configure`
		/// @param[in] maxNumberBoomsSupported Configures the max number of booms the client supports
		/// @param[in] maxNumberSectionsSupported Configures the max number of sections supported by the client for section control
		/// @param[in] maxNumberChannelsSupportedForPositionBasedControl Configures the max number of channels supported by the client for position based control
		/// @param[in] reportToTCSupportsDocumentation Denotes if your app supports documentation
		/// @param[in] reportToTCSupportsTCGEOWithoutPositionBasedControl Denotes if your app supports TC-GEO without position based control
		/// @param[in] reportToTCSupportsTCGEOWithPositionBasedControl Denotes if your app supports TC-GEO with position based control
		/// @param[in] reportToTCSupportsPeerControlAssignment Denotes if your app supports peer control assignment
		/// @param[in] reportToTCSupportsImplementSectionControl Denotes if your app supports implement section control
		void set_common_config_items(std::uint8_t maxNumberBoomsSupported,
		                             std::uint8_t maxNumberSectionsSupported,
		                             std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
		                             bool reportToTCSupportsDocumentation,
		                             bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
		                             bool reportToTCSupportsTCGEOWithPositionBasedControl,
		                             bool reportToTCSupportsPeerControlAssignment,
		                             bool reportToTCSupportsImplementSectionControl);

		/// @brief Changes the internal state machine state and updates the associated timestamp
		/// @param[in] newState The new state for the state machine
		void set_state(StateMachineState newState);

		/// @brief Changes the internal state machine state and updates the associated timestamp to the specified one
		/// @note This is intended for testing purposes only
		/// @param[in] newState The new state for the state machine
		/// @param[in] timestamp The new value for the state machine timestamp (in milliseconds)
		void set_state(StateMachineState newState, std::uint32_t timestamp);

		/// @brief Sets the behavior of the language command interface based on the TC's reported version information
		void select_language_command_partner();

		/// @brief The worker thread will execute this function when it runs, if applicable
		void worker_thread_function();

		static constexpr std::uint32_t SIX_SECOND_TIMEOUT_MS = 6000; ///< The startup delay time defined in the standard
		static constexpr std::uint16_t TWO_SECOND_TIMEOUT_MS = 2000; ///< Used for sending the status message to the TC

	private:
		/// @brief Stores a default process data request callback along with its parent pointer
		struct DefaultProcessDataRequestCallbackInfo
		{
			/// @brief Allows easy comparison of callback data
			/// @param obj the object to compare against
			/// @returns true if the callback and parent pointer match, otherwise false
			bool operator==(const DefaultProcessDataRequestCallbackInfo &obj) const;
			DefaultProcessDataRequestedCallback callback; ///< The callback itself
			void *parent; ///< The parent pointer, generic context value
		};

		/// @brief Stores a TC request value command callback along with its parent pointer
		struct RequestValueCommandCallbackInfo
		{
			/// @brief Allows easy comparison of callback data
			/// @param obj the object to compare against
			/// @returns true if the callback and parent pointer match, otherwise false
			bool operator==(const RequestValueCommandCallbackInfo &obj) const;
			RequestValueCommandCallback callback; ///< The callback itself
			void *parent; ///< The parent pointer, generic context value
		};

		/// @brief Stores a TC value command callback along with its parent pointer
		struct ValueCommandCallbackInfo
		{
			/// @brief Allows easy comparison of callback data
			/// @param obj the object to compare against
			/// @returns true if the callback and parent pointer match, otherwise false
			bool operator==(const ValueCommandCallbackInfo &obj) const;
			ValueCommandCallback callback; ///< The callback itself
			void *parent; ///< The parent pointer, generic context value
		};

		/// @brief Enumerates the modes that the client may use when dealing with a DDOP
		enum class DDOPUploadType
		{
			ProgramaticallyGenerated, ///< Using the AgIsoStack++ DeviceDescriptorObjectPool class
			UserProvidedBinaryPointer, ///< Using a raw pointer to a binary DDOP
			UserProvidedVector ///< Uses a vector of bytes that comprise a binary DDOP
		};

		std::shared_ptr<PartneredControlFunction> partnerControlFunction; ///< The partner control function this client will send to
		std::shared_ptr<InternalControlFunction> myControlFunction; ///< The internal control function the client uses to send from
		std::shared_ptr<PartneredControlFunction> primaryVirtualTerminal; ///< A pointer to the primary VT's control function. Used for TCs < version 4 and language command compatibility
		std::shared_ptr<DeviceDescriptorObjectPool> clientDDOP; ///< Stores the DDOP for upload to the TC (if needed)
		std::uint8_t const *userSuppliedBinaryDDOP = nullptr; ///< Stores a client-provided DDOP if one was provided
		std::shared_ptr<std::vector<std::uint8_t>> userSuppliedVectorDDOP; ///< Stores a client-provided DDOP if one was provided
		std::vector<std::uint8_t> generatedBinaryDDOP; ///< Stores the DDOP in binary form after it has been generated
		std::vector<DefaultProcessDataRequestCallbackInfo> defaultProcessDataRequestedCallbacks; ///< A list of callbacks that will be called when the TC requests a process data value
		std::vector<RequestValueCommandCallbackInfo> requestValueCallbacks; ///< A list of callbacks that will be called when the TC requests a process data value
		std::vector<ValueCommandCallbackInfo> valueCommandsCallbacks; ///< A list of callbacks that will be called when the TC sets a process data value
		std::list<ProcessDataCallbackInfo> queuedValueRequests; ///< A list of queued value requests that will be processed on the next update
		std::list<ProcessDataCallbackInfo> queuedValueCommands; ///< A list of queued value commands that will be processed on the next update
		std::list<ProcessDataCallbackInfo> measurementDistanceIntervalCommands; ///< A list of measurement commands that will be processed on a distance interval
		std::list<ProcessDataCallbackInfo> measurementTimeIntervalCommands; ///< A list of measurement commands that will be processed on a time interval
		std::list<ProcessDataCallbackInfo> measurementMinimumThresholdCommands; ///< A list of measurement commands that will be processed when the value drops below a threshold
		std::list<ProcessDataCallbackInfo> measurementMaximumThresholdCommands; ///< A list of measurement commands that will be processed when the value above a threshold
		std::list<ProcessDataCallbackInfo> measurementOnChangeThresholdCommands; ///< A list of measurement commands that will be processed when the value changes by the specified amount
		Mutex clientMutex; ///< A general mutex to protect data in the worker thread against data accessed by the app or the network manager
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		std::thread *workerThread = nullptr; ///< The worker thread that updates this interface
#endif
		std::string ddopStructureLabel; ///< Stores a pre-parsed structure label, helps to avoid processing the whole DDOP during a CAN message callback
		std::string previousStructureLabel; ///< Stores the last structure label we used, helps to warn the user if they aren't updating the label properly
		std::array<std::uint8_t, 7> ddopLocalizationLabel = { 0 }; ///< Stores a pre-parsed localization label, helps to avoid processing the whole DDOP during a CAN message callback
		DDOPUploadType ddopUploadMode = DDOPUploadType::ProgramaticallyGenerated; ///< Determines if DDOPs get generated or raw uploaded
		StateMachineState currentState = StateMachineState::Disconnected; ///< Tracks the internal state machine's current state
		std::uint32_t stateMachineTimestamp_ms = 0; ///< Timestamp that tracks when the state machine last changed states (in milliseconds)
		std::uint32_t statusMessageTimestamp_ms = 0; ///< Timestamp corresponding to the last time we sent a status message to the TC
		std::uint32_t serverStatusMessageTimestamp_ms = 0; ///< Timestamp corresponding to the last time we received a status message from the TC
		std::uint32_t userSuppliedBinaryDDOPSize_bytes = 0; ///< The number of bytes in the user provided binary DDOP (if one was provided)
		std::uint32_t languageCommandWaitingTimestamp_ms = 0; ///< Timestamp used to determine when to give up on waiting for a language command response
		std::uint32_t totalMachineDistance = 0; ///< The total distance the machine has traveled since the application started. Used for distance interval triggers.
		std::uint8_t numberOfWorkingSetMembers = 1; ///< The number of working set members that will be reported in the working set master message
		std::uint8_t tcStatusBitfield = 0; ///< The last received TC/DL status from the status message
		std::uint8_t sourceAddressOfCommandBeingExecuted = 0; ///< Source address of client for which the current command is being executed
		std::uint8_t commandBeingExecuted = 0; ///< The current command the TC is executing as reported in the status message
		std::uint8_t serverVersion = 0; ///< The detected version of the TC Server
		std::uint8_t maxServerBootTime_s = 0; ///< Maximum number of seconds from a power cycle to transmission of first �Task Controller Status message� or 0xFF
		std::uint8_t serverOptionsByte1 = 0; ///< The options specified in ISO 11783-10 that this TC, DL, or client meets (The definition of this byte is introduced in ISO11783-10 version 3)
		std::uint8_t serverOptionsByte2 = 0; ///< Reserved for ISO assignment, should be zero or 0xFF.
		std::uint8_t serverNumberOfBoomsForSectionControl = 0; ///< When reported by the TC, this is the maximum number of section control booms that are supported
		std::uint8_t serverNumberOfSectionsForSectionControl = 0; ///< When reported by the TC, this is the maximum number of sections that are supported (or 0xFF for version 2 and earlier).
		std::uint8_t serverNumberOfChannelsForPositionBasedControl = 0; ///< When reported by the TC, this is the maximum number of individual control channels that is supported
		std::uint8_t numberBoomsSupported = 0; ///< Stores the number of booms this client supports for section control
		std::uint8_t numberSectionsSupported = 0; ///< Stores the number of sections this client supports for section control
		std::uint8_t numberChannelsSupportedForPositionBasedControl = 0; ///< Stores the number of channels this client supports for position based control
		bool initialized = false; ///< Tracks the initialization state of the interface instance
		bool shouldTerminate = false; ///< This variable tells the worker thread to exit
		bool enableStatusMessage = false; ///< Enables sending the status message to the TC cyclically
		bool supportsDocumentation = false; ///< Determines if the client reports documentation support to the TC
		bool supportsTCGEOWithoutPositionBasedControl = false; ///< Determines if the client reports TC-GEO without position control capability to the TC
		bool supportsTCGEOWithPositionBasedControl = false; ///< Determines if the client reports TC-GEO with position control capability to the TC
		bool supportsPeerControlAssignment = false; ///< Determines if the client reports peer control assignment capability to the TC
		bool supportsImplementSectionControl = false; ///< Determines if the client reports implement section control capability to the TC
		bool shouldReuploadAfterDDOPDeletion = false; ///< Used to determine how the state machine should progress when updating a DDOP
		bool shouldProcessAllDefaultProcessDataRequests = false; ///< Determines if the client should process all default process data requests. Used to offload from the CAN stack's thread if possible.
	};
} // namespace isobus

#endif // ISOBUS_TASK_CONTROLLER_CLIENT_HPP
