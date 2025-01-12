//================================================================================================
/// @file isobus_task_controller_server.hpp
///
/// @brief An abstract task controller server class. You can consume this file and implement
/// the pure virtual functions to create your own task controller or data logger server.
/// @author Adrian Del Grosso
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#ifndef ISOBUS_TASK_CONTROLLER_SERVER_HPP
#define ISOBUS_TASK_CONTROLLER_SERVER_HPP

#include "isobus/isobus/can_constants.hpp"
#include "isobus/isobus/isobus_language_command_interface.hpp"
#include "isobus/isobus/isobus_task_controller_server_options.hpp"

#include <deque>

#include <condition_variable>

namespace isobus
{
	/// @brief An ISO11783-10 task controller (or data logger) server.
	/// A task controller collects task data from connected implements, and optionally controls them.
	/// This interface supports the CAN layer of TC-SC, TC-GEO, and TC-BAS.
	class TaskControllerServer
	{
	public:
		/// @brief Enumerates the different error codes that can be returned when activating
		/// a device descriptor object pool.
		enum class ObjectPoolActivationError : std::uint8_t
		{
			NoErrors = 0x00,
			ThereAreErrorsInTheDDOP = 0x01,
			TaskControllerRanOutOfMemoryDuringActivation = 0x02,
			AnyOtherError = 0x04,
			DifferentDDOPExistsWithSameStructureLabel = 0x08
		};

		/// @brief Enumerates the different error codes that can be returned when deleting
		/// a device descriptor object pool.
		enum class ObjectPoolDeletionErrors : std::uint8_t
		{
			ObjectPoolIsReferencedByTaskData = 0,
			ServerCannotCheckForObjectPoolReferences = 1,
			ErrorDetailsNotAvailable = 0xFF
		};

		/// @brief Enumerates the different error codes that can be returned when processing a DDOP
		enum class ObjectPoolErrorCodes : std::uint8_t
		{
			NoErrors = 0x00,
			MethodOrAttributeNotSupported = 0x01,
			UnknownObjectReference = 0x02,
			AnyOtherError = 0x04,
			DDOPWasDeletedFromVolatileMemory = 0x08
		};

		/// @brief Enumerates the different process data commands that can be sent to the client.
		enum class ProcessDataCommands : std::uint8_t
		{
			TechnicalCapabilities = 0x00, ///< Used for determining the technical capabilities of a TC, DL, or client.
			DeviceDescriptor = 0x01, ///< Transfer and management of device descriptors
			RequestValue = 0x02, ///< Used when the value of the data entity specified by the data dictionary identifier is requested.
			Value = 0x03, ///< This command is used both to answer a request value command and to set the value of a process data entity.
			MeasurementTimeInterval = 0x04, ///< The process data value is the time interval for sending the data element specified by the data dictionary identifier.
			MeasurementDistanceInterval = 0x05, ///< The process data value is the distance interval for sending the data element specified by the data dictionary identifier.
			MeasurementMinimumWithinThreshold = 0x06, ///< The client has to send the value	of this data element to the TC or DL when the value is higher than the threshold value
			MeasurementMaximumWithinThreshold = 0x07, ///< The client has to send the value of this data element to the TC or DL when the value is lower than the threshold value.
			MeasurementChangeThreshold = 0x08, ///< The client has to send the value of this data element to the TC or DL when the value change is higher than or equal to the change threshold since last transmission.
			PeerControlAssignment = 0x09, ///< This message is used to establish a connection between a setpoint value source and a setpoint value user
			SetValueAndAcknowledge = 0x0A, ///< This command is used to set the value of a process data entity and request a reception acknowledgement from the recipient
			Reserved = 0x0B, ///< Reserved.
			Reserved2 = 0x0C, ///< Reserved.
			Acknowledge = 0x0D, ///< Message is a Process Data Acknowledge (PDACK).
			Status = 0x0E, ///< Message is a Task Controller Status message
			ClientTask = 0x0F ///< Sent by the client as a status message every 2s
		};

		/// @brief Enumerates the different options that can be reported by the server.
		/// Each option is a bit in a bitfield, with 1 meaning the option is supported and 0 meaning it is not.
		/// For example, if the server supports documentation and peer control assignment, but not the other options,
		/// the bitfield would be 0b00001001.
		enum class ServerOptions : std::uint8_t
		{
			SupportsDocumentation = 0x01,
			SupportsTCGEOWithoutPositionBasedControl = 0x02,
			SupportsTCGEOWithPositionBasedControl = 0x04,
			SupportsPeerControlAssignment = 0x08,
			SupportsImplementSectionControl = 0x10,
			ReservedOption1 = 0x20,
			ReservedOption2 = 0x40,
			ReservedOption3 = 0x80
		};

		/// @brief Enumerates all PDACK error codes that can be sent to or from the client.
		enum class ProcessDataAcknowledgeErrorCodes : std::uint8_t
		{
			ProcessDataCommandNotSupported = 0x01,
			InvalidElementNumber = 0x02,
			DDINotSupportedByElement = 0x04,
			TriggerMethodNotSupported = 0x08,
			ProcessDataNotSettable = 0x10,
			InvalidOrUnsupportedIntervalOrThreshold = 0x20,
			ProcessDataValueDoesNotConformToDDIDefinition = 0x40,
			ProcessDataValueIsOutOfOperationalRangeOfThisDevice = 0x80
		};

		/// @brief Enumerates the different versions of the task controller standard.
		enum class TaskControllerVersion : std::uint8_t
		{
			DraftInternationalStandard = 0, ///< The version of the DIS (draft International Standard).
			FinalDraftInternationalStandardFirstEdition = 1, ///< The version of the FDIS.1 (final draft International Standard, first edition).
			FirstPublishedEdition = 2, ///< The version of the FDIS.2 and the first edition published ss an International Standard.
			SecondEditionDraft = 3, ///< The version of the second edition published as a draft International Standard(E2.DIS).
			SecondPublishedEdition = 4, ///< The version of the second edition published as the final draft International Standard(E2.FDIS) and as the International Standard(E2.IS)
			Unknown = 0xFF
		};

		/// @brief Constructor for a TC server.
		/// @param[in] internalControlFunction The control function to use to communicate with the clients.
		/// @param[in] numberBoomsSupported The number of booms to report as supported by the TC.
		/// @param[in] numberSectionsSupported The number of sections to report as supported by the TC.
		/// @param[in] numberChannelsSupportedForPositionBasedControl The number of channels to report as supported by the TC.
		/// @param[in] options The options to report as supported by the TC. See the TaskControllerOptions object for more info.
		/// @param[in] versionToReport The version of the task controller standard to report as supported by the TC. Generally you should leave this as 4 (SecondPublishedEdition).
		TaskControllerServer(std::shared_ptr<InternalControlFunction> internalControlFunction,
		                     std::uint8_t numberBoomsSupported,
		                     std::uint8_t numberSectionsSupported,
		                     std::uint8_t numberChannelsSupportedForPositionBasedControl,
		                     const TaskControllerOptions &options,
		                     TaskControllerVersion versionToReport = TaskControllerVersion::SecondPublishedEdition);

		/// @brief Destructor for a TC server.
		virtual ~TaskControllerServer();

		/// @brief Deleted copy constructor
		TaskControllerServer(TaskControllerServer &) = delete;

		/// @brief Deleted assignment operator
		TaskControllerServer &operator=(const TaskControllerServer &) = delete;

		// **** Functions to be implemented by the consumer of the library ****

		/// @brief This function will be called by the server when the client wants to activate its DDOP.
		/// You should implement this function to activate the DDOP and return whether or not it was successful.
		/// Generally this means that you will want to parse the pool, and make sure its schema is valid at this time.
		/// You can use our DeviceDescriptorObjectPool class to help you with this.
		/// @param[in] clientControlFunction The control function which is requesting the activation.
		/// @param[out] activationError The error code to return if the activation fails.
		/// @param[out] objectPoolError This error code tells the client if there was an error in the DDOP.
		/// @param[out] parentObjectIDOfFaultyObject If there was an error in the DDOP, this is the parent object ID of the object that caused the error. Otherwise you should return 0xFFFF
		/// @param[out] faultyObjectID If there was an error in the DDOP, this is the object ID of the object that caused the error. Otherwise you should return 0xFFFF
		/// @returns Whether or not the activation was successful.
		virtual bool activate_object_pool(std::shared_ptr<ControlFunction> clientControlFunction, ObjectPoolActivationError &activationError, ObjectPoolErrorCodes &objectPoolError, std::uint16_t &parentObjectIDOfFaultyObject, std::uint16_t &faultyObjectID) = 0;

		/// @brief This function will be called by the server when the client wants to change the designator of an object.
		/// This could be called because the client wants to change the name of an implement, or the name of a section, or change the active language being used in the DDOP's designators.
		/// You should implement this function to change the designator of the object and return whether or not it was successful.
		/// @param[in] clientControlFunction The control function which is requesting the designator change.
		/// @param[in] objectIDToAlter The object ID of the object to change the designator of.
		/// @param[in] designator The new designator to set for the object.
		/// @returns Whether or not the designator change was successful.
		virtual bool change_designator(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t objectIDToAlter, const std::vector<std::uint8_t> &designator) = 0;

		/// @brief This function will be called by the server when the client wants to deactivate its DDOP.
		/// You should implement this function to deactivate the DDOP and return whether or not it was successful.
		/// @param[in] clientControlFunction The control function which is requesting the deactivation.
		/// @returns Whether or not the deactivation was successful.
		virtual bool deactivate_object_pool(std::shared_ptr<ControlFunction> clientControlFunction) = 0;

		/// @brief This function will be called by the server when the client wants to delete its DDOP.
		/// Each client is allowed to have one DDOP, so deletion is not required, but a client might be kind and delete its DDOP when it is no longer needed.
		/// You should implement this function to delete the DDOP and return whether or not it was successful.
		/// @param[in] clientControlFunction The control function which is requesting the deletion.
		/// @param[out] returnedErrorCode The error code to return if the deletion fails.
		/// @returns Whether or not the deletion was successful.
		virtual bool delete_device_descriptor_object_pool(std::shared_ptr<ControlFunction> clientControlFunction, ObjectPoolDeletionErrors &returnedErrorCode) = 0;

		/// @brief This function will be called by the server when the server needs to know if it has previously saved to non volatile memory (NVM)
		/// a DDOP which is identified by the provided structure label, and optionally also the provided extended structure label.
		/// You should implement this function to return whether or not the DDOP is stored in NVM.
		/// @param[in] clientControlFunction The control function which is requesting the information.
		/// @param[in] structureLabel The structure label of the DDOP to check for. (always 7 bytes)
		/// @param[in] extendedStructureLabel The extended structure label of the DDOP to check for. (up to 32 bytes)
		/// @returns Whether or not the DDOP is stored in NVM.
		virtual bool get_is_stored_device_descriptor_object_pool_by_structure_label(std::shared_ptr<ControlFunction> clientControlFunction, const std::vector<std::uint8_t> &structureLabel, const std::vector<std::uint8_t> &extendedStructureLabel) = 0;

		/// @brief This function will be called by the server when the server needs to know if it has previously saved to non volatile memory (NVM)
		/// a DDOP which is identified by the provided localization label.
		/// You should implement this function to return whether or not the DDOP is stored in NVM.
		/// @param[in] clientControlFunction The control function which is requesting the information.
		/// @param[in] localizationLabel The localization label of the DDOP to check for.
		/// @returns Whether or not the DDOP is stored in NVM.
		virtual bool get_is_stored_device_descriptor_object_pool_by_localization_label(std::shared_ptr<ControlFunction> clientControlFunction, const std::array<std::uint8_t, 7> &localizationLabel) = 0;

		/// @brief This function will be called by the server when the client wants to transfer its DDOP to the server and needs to know
		/// if the server has enough memory available to store the DDOP.
		/// You should implement this function to return whether or not the server has enough memory available to store the DDOP.
		/// @param[in] numberBytesRequired The number of bytes required to store the DDOP.
		/// @returns Whether or not the server has enough memory available to store the DDOP. A value of true indicates: "There may be enough memory available. However,
		/// because there is overhead associated with object storage,it is impossible to predict whether there is enough memory available." and false indicates:
		/// "There is not enough memory available. Do not transmit device descriptor object pool."
		virtual bool get_is_enough_memory_available(std::uint32_t numberBytesRequired) = 0;

		/// @brief This function will be called if someone requests that the TC identify itself.
		/// If this gets called, you should display the TC number for 3 seconds if your TC has a visual interface.
		/// @param[in] taskControllerNumber The task controller number to display.
		virtual void identify_task_controller(std::uint8_t taskControllerNumber) = 0;

		/// @brief This function will be called by the server when a connected client times out.
		/// You should implement this function to do whatever you want to do when a client times out.
		/// Generally this means you will want to also deactivate the DDOP for that client.
		/// @param[in] clientControlFunction The control function which timed out.
		virtual void on_client_timeout(std::shared_ptr<ControlFunction> clientControlFunction) = 0;

		/// @brief This function will be called by the server when a client sends an acknowledgement for a
		/// process data command that was sent to it.
		/// This can be useful to know if the client received the command or not when using the set_value_and_acknowledge command.
		/// @param[in] clientControlFunction The control function which sent the acknowledgement.
		/// @param[in] dataDescriptionIndex The data description index of the data element that was acknowledged.
		/// @param[in] elementNumber The element number of the data element that was acknowledged.
		/// @param[in] errorCodesFromClient The error codes that the client sent in the acknowledgement. This will be a bitfield defined by the ProcessDataAcknowledgeErrorCodes enum.
		/// @param[in] processDataCommand The process data command that was acknowledged.
		virtual void on_process_data_acknowledge(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint8_t errorCodesFromClient, ProcessDataCommands processDataCommand) = 0;

		/// @brief This function will be called by the server when a client sends a value command to the TC.
		/// You should implement this function to do whatever you want to do when a client sends a value command.
		/// This could be anything from setting a value in your program, to sending a command to a connected implement.
		/// The client could be telling you that a section's state changed, or that a boom's position changed, etc. Therefore
		/// this is probably the most important function to implement to get your TC "working".
		/// Use the ISOBUS data dictionary to determine what the dataDescriptionIndex and elementNumber mean.
		/// @param[in] clientControlFunction The control function which sent the value command.
		/// @param[in] dataDescriptionIndex The data description index of the data element that was sent.
		/// @param[in] elementNumber The element number of the data element that was sent.
		/// @param[in] processDataValue The process data value that was sent.
		/// @param[out] errorCodes You should return any errors that occurred while processing the value command in this variable as defined by the ProcessDataAcknowledgeErrorCodes enum.
		/// This will be sent back to the client if an acknowledgement is requested.
		/// @returns Whether or not the value command was processed successfully.
		virtual bool on_value_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::int32_t processDataValue, std::uint8_t &errorCodes) = 0;

		/// @brief This function is called when the server wants you to save a DDOP to non volatile memory (NVM).
		/// You should implement this function to save the DDOP to NVM.
		/// If appendToPool is true, you should append the DDOP to the existing DDOP in NVM.
		/// @param[in] clientControlFunction The control function which is requesting the save.
		/// @param[in] objectPoolData The DDOP itself as a binary blob.
		/// @param[in] appendToPool Whether or not to append the DDOP to the existing DDOP in NVM, or overwrite it.
		/// @returns Whether or not the save was successful.
		virtual bool store_device_descriptor_object_pool(std::shared_ptr<ControlFunction> clientControlFunction, const std::vector<std::uint8_t> &objectPoolData, bool appendToPool) = 0;

		// **** Functions used to communicate with the client ****

		/// @brief Sends a request to a client for an element's value of a particular DDI.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The Data Description Index being requested
		/// @param[in] elementNumber The element number being requested
		/// @returns true if the message was sent, otherwise false
		bool send_request_value(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber) const;

		/// @brief Sends a time interval measurement command.
		/// The process data value for this command is the time interval for sending the data element
		/// specified by the data dictionary identifier.The client has to send the value of this data
		/// element to the TC or DL cyclic with this time interval in milliseconds.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] timeInterval The time interval in milliseconds for sending the data element specified by the data dictionary identifier.
		/// @returns true if the message was sent, otherwise false
		bool send_time_interval_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t timeInterval) const;

		/// @brief Sends a distance interval measurement command.
		/// The process data value for this command is the distance interval for sending the data element
		/// specified by the data dictionary identifier.The client has to send the value of this data
		/// element to the TC or DL cyclic with this distance interval.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] distanceInterval The distance interval for sending the data element specified by the data dictionary identifier.
		/// @returns true if the message was sent, otherwise false
		bool send_distance_interval_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t distanceInterval) const;

		/// @brief Sends a minimum threshold measurement command.
		/// The process data value for this command is the minimum threshold for sending the data element
		/// specified by the data dictionary identifier.The client has to send the value of this data
		/// element to the TC or DL when the value is higher than the threshold value.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] minimum The minimum threshold for sending the data element specified by the data dictionary identifier.
		/// @returns true if the message was sent, otherwise false
		bool send_minimum_threshold_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t minimum) const;

		/// @brief Sends a maximum threshold measurement command.
		/// The process data value for this command is the maximum threshold for sending the data element
		/// specified by the data dictionary identifier.The client has to send the value of this data
		/// element to the TC or DL when the value is lower than the threshold value.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] maximum The maximum threshold for sending the data element specified by the data dictionary identifier.
		/// @returns true if the message was sent, otherwise false
		bool send_maximum_threshold_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t maximum) const;

		/// @brief Sends a change threshold measurement command.
		/// The process data value for this command is the change threshold for sending the data element
		/// specified by the data dictionary identifier.The client has to send the value of this data
		/// element to the TC or DL when the value change is higher than or equal to the change threshold since last transmission.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] threshold The change threshold for sending the data element specified by the data dictionary identifier.
		/// @returns true if the message was sent, otherwise false
		bool send_change_threshold_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t threshold) const;

		/// @brief Sends a set value and acknowledge command.
		/// This command is used to set the value of a process data entity and request a reception acknowledgement from the recipient.
		/// The set value command process data value is the value of the data entity specified by the data dictionary identifier.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] processDataValue The process data value to send
		/// @returns true if the message was sent, otherwise false
		bool send_set_value_and_acknowledge(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t processDataValue) const;

		/// @brief Sends a set value command without requesting an acknowledgement.
		/// This command is used to set the value of a process data entity.
		/// The set value command process data value is the value of the data entity specified by the data dictionary identifier.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] processDataValue The process data value to send
		/// @returns true if the message was sent, otherwise false
		bool send_set_value(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t processDataValue) const;

		/// @brief Use this to set the reported task state in the status message.
		/// Basically, this should be set to true when the user starts a job, and false when the user stops a job.
		/// @note Don't be like some terminals which set this to true all the time, that's very annoying for the client.
		/// @param[in] isTaskActive Whether a task is currently active or not.
		void set_task_totals_active(bool isTaskActive);

		/// @brief Returns whether a task is currently active or not.
		/// @returns Whether a task is currently active or not.
		bool get_task_totals_active() const;

		/// @brief Returns the language command interface used to communicate with the client which language/units are in use.
		/// The language command is very important for the TC to function correctly, so it is recommended that you call this
		/// function and configure the language command interface before calling initialize().
		/// @returns The language command interface used to communicate with the client which language/units are in use.
		LanguageCommandInterface &get_language_command_interface();

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		/// @brief Returns a condition variable which you can optionally use to wake up your server's thread
		/// when messages are received from the client.
		/// @returns A condition variable which you can optionally use to wake up your server's thread
		std::condition_variable &get_condition_variable();
#endif

		// **** Functions used to initialize and run the server ****

		/// @brief Initializes the task controller server.
		void initialize();

		/// @brief Returns whether or not the task controller server has been initialized.
		/// @returns Whether or not the task controller server has been initialized.
		bool get_initialized() const;

		/// @brief Shuts down the TC server, unregisters PGN callbacks
		void terminate();

		/// @brief This must be called periodically for the interface to operate correctly.
		/// @details This function must be called periodically. You have some choices on how to do this:
		/// First, you could poll it at a high rate in your main thread, at least 2-3x as fast as your fastest triggered message.
		/// Second, you could call it at a slower rate (something like 250-500 ms), and update it when the condition variable is notified.
		/// You can get the condition variable by calling get_condition_variable() if threading is enabled in the CAN stack.
		/// Third, you could run this in a separate thread, but again, you should call it at least 2-3x as fast as your fastest triggered message.
		/// Calling this often ensures timed out clients are pruned, and messages are processed in a timely fashion, which
		/// is important for the TC to function correctly and for agronomic/implement performance.
		void update();

	protected:
		/// @brief Enumerates the different status bits that can be sent in the status message.
		enum class ServerStatusBit : std::uint8_t
		{
			TaskTotalsActive = 0x01,
			BusySavingDataToNVM = 0x02,
			BusyReadingDataFromNVM = 0x04,
			BusyExecutingACommand = 0x08,
			OutOfMemory = 0x80,
		};

		/// @brief Enumerates the subcommands for determining the technical capabilities of a TC, DL, or client
		enum class TechnicalDataCommandParameters : std::uint8_t
		{
			RequestVersion = 0x00, /// The Request Version message allows the TC, DL, and the client to determine the ISO 11783-10 version of the implementation
			ParameterVersion = 0x01, /// The Version message is sent in response to the request version message and contains the ISO 11783-10 version information of the TC, DL, or client implementation
			IdentifyTaskController = 0x02 /// Upon receipt of this message, the TC shall display, for a period of 3 s, the TC Number.
		};

		/// @brief Enumerates subcommands for the transfer and management of device descriptors. These device descriptor
		/// messages are defined in ISO11783-10 B.6.
		enum class DeviceDescriptorCommandParameters : std::uint8_t
		{
			RequestStructureLabel = 0x00, /// Allows the client to determine the availability of the requested device descriptor structure
			StructureLabel = 0x01, /// The Structure Label message is sent by the TC or DL to inform the client about the availability of the requested version of the device descriptor structure
			RequestLocalizationLabel = 0x02, /// Allows the client to determine the availability of the requested device descriptor localization
			LocalizationLabel = 0x03, /// /// Sent by the TC or DL to inform the client about the availability of the requested localization version of the device descriptor
			RequestObjectPoolTransfer = 0x04, /// /// The Request Object-pool Transfer message allows the client to determine whether it is allowed to transfer(part of) the device descriptor object pool to the TC
			RequestObjectPoolTransferResponse = 0x05, /// Sent in response to Request Object-pool Transfer message
			ObjectPoolTransfer = 0x06, /// Enables the client to transfer (part of) the device descriptor object pool to the TC
			ObjectPoolTransferResponse = 0x07, /// Response to an object pool transfer message
			ObjectPoolActivateDeactivate = 0x08, /// Sent by a client to complete its connection procedure to a TC or DL or to disconnect from a TC or DL.
			ObjectPoolActivateDeactivateResponse = 0x09, /// Sent by a client to complete its connection procedure to a TC or DL or to disconnect from a TC or DL.
			DeleteObjectPool = 0x0A, /// This is a message to delete the device descriptor object pool for the client that sends this message.
			DeleteObjectPoolResponse = 0x0B, /// TC response to a Object-pool Delete message
			ChangeDesignator = 0x0C, /// This message is used to update the designator of an object.
			ChangeDesignatorResponse = 0x0D /// Sent in response to Change Designator message
		};

		/// @brief Stores information about a client that is currently being communicated with.
		class ActiveClient
		{
		public:
			/// @brief Constructor for an active client object which stores information about a client that is currently being communicated with.
			/// @param clientControlFunction The control function used to communicate with the client.
			explicit ActiveClient(std::shared_ptr<ControlFunction> clientControlFunction);
			std::shared_ptr<ControlFunction> clientControlFunction; ///< The control function used to communicate with the client.
			std::uint32_t lastStatusMessageTimestamp_ms = 0; ///< The timestamp of the last status message sent to the client.
			std::uint32_t clientDDOPsize_bytes = 0; ///< The size of the client's DDOP in bytes.
			std::uint32_t statusBitfield = 0; ///< The status bitfield that the client is reporting to us.
			std::uint16_t numberOfObjectPoolSegments = 0; ///< The number of object pool segments that have been sent to the client.
			std::uint8_t reportedVersion = 0; ///< The value representing a version reported by the client.
			bool isDDOPActive = false; ///< Whether or not the client's DDOP is active.
		};

		/// @brief Stores messages received from task controller clients for processing later.
		/// @details This is used to avoid processing messages on the CAN stack's thread.
		/// Messages are actually processed in process_rx_messages which is called by update().
		/// Because update is called by your application, this means that messages are processed on your application's thread,
		/// which avoids a bunch of mutexing in your app.
		/// @param[in] message The message received from the client.
		/// @param[in] parentPointer A context variable that can be used to find this class's instance.
		static void store_rx_message(const CANMessage &message, void *parentPointer);

		/// @brief Processes messages received from task controller clients.
		/// @details This is called by update() and processes messages that were received from clients.
		/// Because update is called by your application, this means that messages are processed on your application's thread,
		/// rather than on the CAN stack's thread, which avoids a bunch of mutexing in your app.
		/// You can get a condition variable from get_condition_variable() which you can use to wake up your application's thread
		/// to process messages if you want to avoid polling the interface at a high rate.
		void process_rx_messages();

		/// @brief This sends a process data message with all FFs in the payload except for the command byte.
		/// Useful for avoiding a lot of boilerplate code when sending process data messages.
		/// @param[in] multiplexer The multiplexer value to send in the message.
		/// @param[in] destination The control function to send the message to.
		/// @returns true if the message was sent, otherwise false
		bool send_generic_process_data_default_payload(std::uint8_t multiplexer, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a measurement command to the client.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] commandValue The command value to send
		/// @param[in] dataDescriptionIndex The data description index of the data element to send the command for
		/// @param[in] elementNumber The element number of the data element to send the command for
		/// @param[in] processDataValue The process data value to send
		/// @returns true if the message was sent, otherwise false
		bool send_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint8_t commandValue, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t processDataValue) const;

		/// @brief Sends a status message broadcast
		/// @returns true if the message was sent, otherwise false
		bool send_status_message() const;

		/// @brief Sends the version message to a client
		/// @param[in] clientControlFunction The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_version(std::shared_ptr<ControlFunction> clientControlFunction) const;

		/// @brief Checks to see if we are communicating with a control function that is already in our list of active clients.
		/// If we are, it returns a pointer to our active client object for that control function.
		/// @param[in] clientControlFunction The control function to check for.
		/// @returns A pointer to our active client object for that control function, or nullptr if we are not communicating with that control function.
		std::shared_ptr<ActiveClient> get_active_client(std::shared_ptr<ControlFunction> clientControlFunction) const;

		/// @brief Sends a negative acknowledge for a the process data PGN which indicates to clients
		/// that we aren't listening to them because they aren't following the protocol.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool nack_process_data_command(std::shared_ptr<ControlFunction> clientControlFunction) const;

		/// @brief Sends a response to a request structure label command.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] structureLabel The structure label to send
		/// @param[in] extendedStructureLabel The extended structure label to send
		/// @returns true if the message was sent, otherwise false
		bool send_structure_label(std::shared_ptr<ControlFunction> clientControlFunction, std::vector<std::uint8_t> &structureLabel, const std::vector<std::uint8_t> &extendedStructureLabel) const;

		/// @brief Sends a response to a request localization label command.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] localizationLabel The localization label to send
		/// @returns true if the message was sent, otherwise false
		bool send_localization_label(std::shared_ptr<ControlFunction> clientControlFunction, const std::array<std::uint8_t, 7> &localizationLabel) const;

		/// @brief Sends a response to a request object pool transfer command.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] isEnoughMemory Whether or not there is enough memory available to transfer the object pool.
		/// @returns true if the message was sent, otherwise false
		bool send_request_object_pool_transfer_response(std::shared_ptr<ControlFunction> clientControlFunction, bool isEnoughMemory) const;

		/// @brief Sends a response to an object pool transfer
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] errorBitfield The error bitfield to send
		/// @param[in] sizeBytes The size of the object pool in bytes
		/// @returns true if the message was sent, otherwise false
		bool send_object_pool_transfer_response(std::shared_ptr<ControlFunction> clientControlFunction, std::uint8_t errorBitfield, std::uint32_t sizeBytes) const;

		/// @brief Sends a response to an object pool activate/deactivate command
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] activationErrorBitfield The error bitfield to send for the activation
		/// @param[in] objectPoolErrorBitfield The error bitfield to send for the object pool
		/// @param[in] parentOfFaultingObject The parent of the object that caused the error (or 0xFFFF if there is no error or it's unknown)
		/// @param[in] faultingObject The object that caused the error (or 0xFFFF if there is no error or it's unknown)
		/// @returns true if the message was sent, otherwise false
		bool send_object_pool_activate_deactivate_response(std::shared_ptr<ControlFunction> clientControlFunction,
		                                                   std::uint8_t activationErrorBitfield,
		                                                   std::uint8_t objectPoolErrorBitfield,
		                                                   std::uint16_t parentOfFaultingObject,
		                                                   std::uint16_t faultingObject) const;

		/// @brief Sends a response to a delete object pool command
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] deletionResult Whether or not the object pool was deleted
		/// @param[in] errorCode The error code to send
		/// @returns true if the message was sent, otherwise false
		bool send_delete_object_pool_response(std::shared_ptr<ControlFunction> clientControlFunction, bool deletionResult, std::uint8_t errorCode) const;

		/// @brief Sends a response to a change designator command
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] objectID The object ID that was changed
		/// @param[in] errorCode The error code to send
		/// @returns true if the message was sent, otherwise false
		bool send_change_designator_response(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t objectID, std::uint8_t errorCode) const;

		/// @brief Sends a process data acknowledge message to the client
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataDescriptionIndex The data description index of the data element that was acknowledged.
		/// @param[in] elementNumber The element number of the data element that was acknowledged.
		/// @param[in] errorBitfield The error bitfield to send (see ProcessDataAcknowledgeErrorCodes enum)
		/// @param[in] processDataCommand The process data command that was acknowledged. (or 0x0F if N/A)
		/// @returns true if the message was sent, otherwise false
		bool send_process_data_acknowledge(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint8_t errorBitfield, ProcessDataCommands processDataCommand) const;

		/// @brief Sends a process data message to a client with a slightly shorter signature than calling send_can_message.
		/// @param[in] clientControlFunction The control function to send the message to
		/// @param[in] dataBuffer The data to send
		/// @param[in] dataLength The data buffer length
		/// @param[in] priority The priority of the message, normally 5
		/// @returns true if the message was sent, otherwise false
		bool send_process_data_to_client(std::shared_ptr<ControlFunction> clientControlFunction,
		                                 const std::uint8_t *dataBuffer,
		                                 std::uint32_t dataLength,
		                                 CANIdentifier::CANPriority priority = CANIdentifier::CANPriority::Priority5) const;

		static constexpr std::uint32_t STATUS_MESSAGE_RATE_MS = 2000; ///< The rate at which status messages are sent to the clients in milliseconds.

		LanguageCommandInterface languageCommandInterface; ///< The language command interface used to communicate with the client which language/units are in use.
		std::shared_ptr<InternalControlFunction> serverControlFunction; ///< The control function used to communicate with the clients.
		std::deque<CANMessage> rxMessageQueue; ///< A queue of messages received from the clients which will be processed when update is called.
		std::deque<std::shared_ptr<ActiveClient>> activeClients; ///< A list of clients that are currently being communicated with.
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		std::condition_variable updateWakeupCondition; ///< A condition variable you can optionally use to update the interface when messages are received
		std::mutex messagesMutex; ///< A mutex used to protect the rxMessageQueue.
#endif
		std::uint32_t lastStatusMessageTimestamp_ms = 0; ///< The timestamp of the last status message sent on the bus
		const TaskControllerVersion reportedVersion; ///< The version of the TC that will be reported to the clients.
		const std::uint8_t numberBoomsSupportedToReport; ///< The number of booms that will be reported as supported by the TC.
		const std::uint8_t numberSectionsSupportedToReport; ///< The number of sections that will be reported as supported by the TC.
		const std::uint8_t numberChannelsSupportedForPositionBasedControlToReport; ///< The number of channels that will be reported as supported by the TC.
		const std::uint8_t optionsBitfieldToReport; ///< The options bitfield that will be reported as supported by the TC.
		std::uint8_t currentStatusByte = 0; ///< The current status byte to send in the status message.
		std::uint8_t currentCommandByte = 0; ///< The current command byte to send in the status message.
		std::uint8_t currentCommandSourceAddress = NULL_CAN_ADDRESS; ///< The current command source address to send in the status message.
		bool initialized = false; ///< Whether or not the task controller server has been initialized.
	};
} // namespace isobus

#endif // ISOBUS_TASK_CONTROLLER_SERVER_HPP
