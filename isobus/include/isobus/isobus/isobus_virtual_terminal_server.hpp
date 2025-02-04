//================================================================================================
/// @file isobus_virtual_terminal_server.hpp
///
/// @brief An abstract VT server.
/// @author Adrian Del Grosso
///
/// @copyright 2023 Adrian Del Grosso
//================================================================================================
#ifndef ISOBUS_VIRTUAL_TERMINAL_SERVER_HPP
#define ISOBUS_VIRTUAL_TERMINAL_SERVER_HPP

#include "isobus/isobus/can_callbacks.hpp"
#include "isobus/isobus/can_constants.hpp"
#include "isobus/isobus/can_internal_control_function.hpp"
#include "isobus/isobus/isobus_language_command_interface.hpp"
#include "isobus/isobus/isobus_virtual_terminal_base.hpp"
#include "isobus/isobus/isobus_virtual_terminal_server_managed_working_set.hpp"
#include "isobus/utility/event_dispatcher.hpp"

namespace isobus
{
	/// @brief This class is an abstract VT server interface.
	/// @details The VT is a control function that provides a way for operators to interact with other
	/// control functions via a GUI. A VT has a pixel-addressable (graphical) display.
	/// The information that is shown in display areas are defined by Data Masks, Alarm Masks and Soft Key Masks.
	/// The data for these masks is contained in object definitions that are loaded into a VT via the ISO 11783 CAN bus, or from non-volatile memory.
	/// See ISO 11783-6 for the complete definition of this interface, and the objects involved.
	class VirtualTerminalServer : public VirtualTerminalBase
	{
	public:
		/// @brief Constructor for a VirtualTerminalServer
		/// @param[in] controlFunctionToUse The internal control function to use when sending messages to VT clients
		VirtualTerminalServer(std::shared_ptr<InternalControlFunction> controlFunctionToUse);

		/// @brief Destructor for the VirtualTerminalServer
		~VirtualTerminalServer();

		/// @brief Initializes the interface, which registers it with the network manager.
		void initialize();

		/// @brief Returns if the interface has been initialized yet.
		/// @returns true if initialize has been called on this object, otherwise false
		bool get_initialized() const;

		/// @brief Returns the internal control function used by the VT server
		/// @returns The internal control function used by the VT server
		std::shared_ptr<InternalControlFunction> get_internal_control_function() const;

		/// @brief Returns a pointer to the currently active working set
		/// @returns Pointer to the currently active working set, or nullptr if none is active
		std::shared_ptr<VirtualTerminalServerManagedWorkingSet> get_active_working_set() const;

		/// @brief The Button Activation message allows the VT to transmit operator selection of a Button object to the Working
		/// Set Master
		/// @param[in] activationCode 0 for released, 1 for "pressed", 2 for "still held", or 3 for "aborted"
		/// @param[in] objectId Object ID of Button object
		/// @param[in] parentObjectId Object ID of parent Data Mask or in the case where the Button is in a visible Window Mask object, the Object ID of the Window Mask object
		/// @param[in] keyNumber Button key code (see ISO11783-6)
		/// @param[in] destination The VT client to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_button_activation_message(KeyActivationCode activationCode, std::uint16_t objectId, std::uint16_t parentObjectId, std::uint8_t keyNumber, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends the VT Change Numeric Value message
		/// @details The VT sends this message any time the operator enters a numeric value for an input object or variable,
		/// regardless of whether or not the value changed.This message is not sent if the input was aborted(in this case a VT ESC message would be sent instead).For input objects that have a numeric variable reference,
		/// the Object ID of the numeric variable object is used in this message.
		/// @param[in] objectId The object ID of the affected object
		/// @param[in] value The value that the referenced object was changed to
		/// @param[in] destination The control function to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_change_numeric_value_message(std::uint16_t objectId, std::uint32_t value, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends the VT Select Input Object message
		/// @details This message is sent by the VT any time an input field, Button, or Key object is selected (gets focus),
		/// deselected (loses focus), opened for edit or closed after edit by the operator or an ESC command.
		/// @param[in] objectId The object ID of the affected object
		/// @param[in] isObjectSelected True of the object has focus, false if the object has been deselected
		/// @param[in] isObjectOpenForInput True if the object is open for data input (only is object is selected!), otherwise false
		/// @param[in] destination The control function to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_select_input_object_message(std::uint16_t objectId, bool isObjectSelected, bool isObjectOpenForInput, std::shared_ptr<ControlFunction> destination) const;

		/// @brief The Button Activation message allows the VT to transmit operator selection of a key object to the Working
		/// Set Master
		/// @param[in] activationCode 0 for released, 1 for "pressed", 2 for "still held", or 3 for "aborted"
		/// @param[in] objectId Object ID of Button object
		/// @param[in] parentObjectId Object ID of parent Data Mask or in the case where the Button is in a visible Window Mask object, the Object ID of the Window Mask object
		/// @param[in] keyNumber Button key code (see ISO11783-6)
		/// @param[in] destination The VT client to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_soft_key_activation_message(KeyActivationCode activationCode, std::uint16_t objectId, std::uint16_t parentObjectId, std::uint8_t keyNumber, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends the VT Change String Value Message.
		/// The VT uses this message to transfer a string entered into an Input String object or referenced String
		/// Variable object.
		/// @param[in] objectId The object ID that was altered, either for a string variable or an input string
		/// @param[in] value The entered string
		/// @param[in] destination The VT client to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_string_value_message(std::uint16_t objectId, const std::string &value, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a load version command
		/// The reason this is exposed is because you will need to send this message after
		/// the object pool processing thread completes at some point to tell the client to proceed if their
		/// object pool was loaded via a load version command.
		/// @param[in] errorCodes A set of error bits to report to the client. These will be reported from the managed working set's parsing results.
		/// @param[in] destination The VT client to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_load_version_response(std::uint8_t errorCodes, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Conditionally executes a macro. If the object passed in is of the specified type, and
		/// a macro is defined for that object, the macro will be executed if the macro event matches the
		/// event ID of the macro.
		/// @param[in] object The object to check for a macro (or macros) to execute
		/// @param[in] macroEvent The event ID of the macro(s) to execute
		/// @param[in] targetObjectType The type of object that the macro is defined for. Used to validate the object
		/// @param[in] workingset The working set to execute the macro on
		void process_macro(std::shared_ptr<isobus::VTObject> object, isobus::EventID macroEvent, isobus::VirtualTerminalObjectType targetObjectType, std::shared_ptr<isobus::VirtualTerminalServerManagedWorkingSet> workingset);

		// ----------- Mandatory Functions you must implement -----------------------

		/// @brief This function is called when the client wants to know if the server has enough memory to store the object pool.
		/// You should return true if the server has enough memory to store the object pool, otherwise false.
		/// @param[in] requestedMemory The amount of memory requested by the client
		/// @returns True if the server has enough memory to store the object pool, otherwise false
		virtual bool get_is_enough_memory(std::uint32_t requestedMemory) const = 0;

		/// @brief This function is called when the client wants to know the version of the VT.
		/// @returns The version of the VT
		virtual VTVersion get_version() const = 0;

		/// @brief This function is called when the interface wants to know the number of navigation soft keys.
		/// @returns The number of navigation soft keys
		virtual std::uint8_t get_number_of_navigation_soft_keys() const = 0;

		/// @brief This function is called when the interface needs to know the number of x pixels (width) of your soft keys
		/// @returns The number of x pixels (width) of your soft keys
		virtual std::uint8_t get_soft_key_descriptor_x_pixel_width() const = 0;

		/// @brief This function is called when the interface needs to know the number of y pixels (height) of your soft keys
		/// @returns The number of y pixels (height) of your soft keys
		virtual std::uint8_t get_soft_key_descriptor_y_pixel_height() const = 0;

		/// @brief This function is called when the interface needs to know the number of possible virtual soft keys in your soft key mask render area
		/// @returns The number of possible virtual soft keys in your soft key mask render area
		virtual std::uint8_t get_number_of_possible_virtual_soft_keys_in_soft_key_mask() const = 0;

		/// @brief This function is called when the interface needs to know the number of physical soft keys
		/// @returns The number of physical soft keys
		virtual std::uint8_t get_number_of_physical_soft_keys() const = 0;

		/// @brief This function is called when the interface needs to know the number of x pixels (width) of your data key mask render area
		/// @returns The number of x pixels (width) of your soft key mask render area
		virtual std::uint16_t get_data_mask_area_size_x_pixels() const = 0;

		/// @brief This function is called when the interface needs to know the number of y pixels (height) of your data key mask render area
		/// @returns The number of y pixels (height) of your data key mask render area
		virtual std::uint16_t get_data_mask_area_size_y_pixels() const = 0;

		/// @brief The interface calls this function when it wants you to discontinue/suspend a working set
		/// @param[in] workingSetWithError The working set to suspend
		virtual void suspend_working_set(std::shared_ptr<VirtualTerminalServerManagedWorkingSet> workingSetWithError) = 0;

		/// @brief This function is called when the interface needs to know the wide chars you support
		/// @param[in] codePlane The code plane to inquire about
		/// @param[in] firstWideCharInInquiryRange The first wide char in the inquiry range
		/// @param[in] lastWideCharInInquiryRange The last wide char in the inquiry range
		/// @param[out] numberOfRanges The number of wide char ranges supported
		/// @param[out] wideCharRangeArray The wide char range array
		/// @returns The error code for the supported wide chars inquiry
		virtual SupportedWideCharsErrorCode get_supported_wide_chars(std::uint8_t codePlane,
		                                                             std::uint16_t firstWideCharInInquiryRange,
		                                                             std::uint16_t lastWideCharInInquiryRange,
		                                                             std::uint8_t &numberOfRanges,
		                                                             std::vector<std::uint8_t> &wideCharRangeArray) = 0;

		/// @brief This function is called when the interface needs to know what versions of object pools are available for a client.
		/// @param[in] clientNAME The client requesting the object pool versions
		/// @returns A vector of object pool versions available for the client
		virtual std::vector<std::array<std::uint8_t, 7>> get_versions(NAME clientNAME) = 0;

		/// @brief This function is called when the interface needs to know what objects are supported by the server.
		/// @returns A vector of supported objects
		virtual std::vector<std::uint8_t> get_supported_objects() const = 0;

		/// @brief This function is called when the client wants the server to load a previously stored object pool.
		/// If there exists in the VT's non-volatile memory an object pool matching the provided version label,
		/// return it. If one does not exist, return an empty vector.
		/// @param[in] versionLabel The object pool version to load for the given client NAME
		/// @param[in] clientNAME The client requesting the object pool
		/// @returns The requested object pool associated with the version label.
		virtual std::vector<std::uint8_t> load_version(const std::vector<std::uint8_t> &versionLabel, NAME clientNAME) = 0;

		/// @brief This function is called when the client wants the server to save an object pool
		/// to the VT's non-volatile memory.
		/// If the object pool is saved successfully, return true, otherwise return false.
		/// @note This may be called multiple times with the same version, but different data. When this
		/// happens, the expectation is that you will append each objectPool together into one large file.
		/// @param[in] objectPool The object pool data to save
		/// @param[in] versionLabel The object pool version to save for the given client NAME
		/// @param[in] clientNAME The client requesting the object pool
		/// @returns The requested object pool associated with the version label.
		virtual bool save_version(const std::vector<std::uint8_t> &objectPool, const std::vector<std::uint8_t> &versionLabel, NAME clientNAME) = 0;

		/// @brief This function is called when the client wants the server to delete a stored object pool.
		/// All object pool files matching the specified version label should then be deleted from the VT's
		/// non-volatile storage.
		/// @param[in] versionLabel The version label for the object pool(s) to delete
		/// @param[in] clientNAME The NAME of the client that is requesting deletion
		/// @returns True if the version was deleted from VT non-volatile storage, otherwise false.
		virtual bool delete_version(const std::vector<std::uint8_t> &versionLabel, NAME clientNAME) = 0;

		/// @brief This function is called when the client wants the server to delete ALL stored object pools associated to it's NAME.
		/// All object pool files matching the specified client NAME should then be deleted from the VT's
		/// non-volatile storage.
		/// @param[in] clientNAME The NAME of the client that is requesting deletion
		/// @returns True if all relevant object pools were deleted from VT non-volatile storage, otherwise false.
		virtual bool delete_all_versions(NAME clientNAME) = 0;

		/// @brief This function is called when the client wants the server to deactivate its object pool.
		/// You should treat this as a disconnection by the client, as it may be moving to another VT.
		/// @attention This does not mean to delete the pool from non-volatile memory!!! This only deactivates the active pool.
		/// @details This command is used to delete the entire object pool of this Working Set from volatile storage.
		/// This command can be used by an implement when it wants to move its object pool to another VT,
		/// or when it is shutting down or during the development of object pools.
		/// @param[in] clientNAME The NAME of the client that is requesting deletion
		/// @returns True if the client's active object pool was deactivated and removed from volatile storage, otherwise false.
		virtual bool delete_object_pool(NAME clientNAME) = 0;

		//------------ Optional functions you can override --------------------

		/// @brief If you want to override the graphics mode from its default 256 color mode, you can override this function.
		/// Though, that would be unusual.
		/// @returns The graphic mode of the VT to report to clients
		virtual VirtualTerminalBase::GraphicMode get_graphic_mode() const;

		/// @brief If you want to override the amount of time the VT reports it takes to power up, you can override this function.
		/// @returns The amount of time the VT reports it takes to power up, or 255 if it is not known
		virtual std::uint8_t get_powerup_time() const;

		/// @brief By default, the VT server will report that it supports all small and large fonts.
		/// If you want to override this, you can override this function.
		/// @returns The bitfield of supported small fonts
		virtual std::uint8_t get_supported_small_fonts_bitfield() const;

		/// @brief By default, the VT server will report that it supports all small and large fonts.
		/// If you want to override this, you can override this function.
		/// @returns The bitfield of supported large fonts
		virtual std::uint8_t get_supported_large_fonts_bitfield() const;

		//-------------- Callbacks/Event driven interface ---------------------

		/// @brief Returns the event dispatcher for repaint events
		/// @returns The event dispatcher for repaint events
		EventDispatcher<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>> &get_on_repaint_event_dispatcher();

		/// @brief Returns the event dispatcher for change active mask events
		/// @returns The event dispatcher for change active mask events
		EventDispatcher<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>, std::uint16_t, std::uint16_t> &get_on_change_active_mask_event_dispatcher();

		/// @brief Returns the event dispatcher for when an object is focused
		/// @returns The event dispatcher for when an object is focused
		EventDispatcher<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>, std::uint16_t, bool> &get_on_focus_object_event_dispatcher();

		//----------------- Other Server Settings -----------------------------

		/// @brief Returns the language command interface for the server, which
		/// can be used to inform clients of the current unit systems, language, and country code
		/// @returns The language command interface for the server
		LanguageCommandInterface &get_language_command_interface();

	protected:
		/// @brief Enumerates the bit indices of the error fields that can be set in a change active mask response
		enum class ChangeActiveMaskErrorBit : std::uint8_t
		{
			InvalidWorkingSetObjectID = 0,
			InvalidMaskObjectID = 1,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change background colour response
		enum class ChangeBackgroundColourErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidColourCode = 1,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change child location/position value response
		enum class ChangeChildLocationorPositionErrorBit : std::uint8_t
		{
			ParentObjectDoesntExistOrIsNotAParentOfSpecifiedObject = 0,
			TargetObjectDoesNotExistOrIsNotApplicable = 1,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change fill attributes response
		enum class ChangeFillAttributesErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidType = 1,
			InvalidColour = 2,
			InvalidPatternObjectID = 3,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change font attributes response
		enum class ChangeFontAttributesErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidColour = 1,
			InvalidSize = 2,
			InvalidType = 3,
			InvalidStyle = 4,
			AnyOtherError = 5
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change list item response
		enum class ChangeListItemErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidListIndex = 1,
			InvalidNewListItemObjectID = 2,
			Reserved = 3, ///< Set to zero
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change numeric value response
		enum class ChangeNumericValueErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidValue = 1,
			ValueInUse = 2, // such as: open for input
			Undefined = 3,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change priority response
		enum class ChangePriorityErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidPriority = 1,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change size response
		enum class ChangeSizeErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change soft key mask response
		enum class ChangeSoftKeyMaskErrorBit : std::uint8_t
		{
			InvalidDataOrAlarmMaskObjectID = 0,
			InvalidSoftKeyMaskObjectID = 1,
			MissingObjects = 2,
			MaskOrChildObjectHasErrors = 3,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change string value response
		enum class ChangeStringValueErrorBit : std::uint8_t
		{
			Undefined = 0, ///< This bit should always be set to zero
			InvalidObjectID = 1,
			StringTooLong = 2,
			AnyOtherError = 3,
			Reserved = 4 ///< In VT version 4 and 5 this bit was "value in use" but that is now deprecated
		};

		/// @brief Enumerates the different error bit indices that can be set in a delete version response
		enum class DeleteVersionErrorBit : std::uint8_t
		{
			Reserved = 0,
			VersionLabelNotCorrectOrUnknown = 1,
			AnyOtherError = 3
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a enable/disable object response
		enum class EnableDisableObjectErrorBit : std::uint8_t
		{
			Undefined = 0,
			InvalidObjectID = 1,
			InvalidEnableDisableCommandValue = 2,
			CouldNotCompleteTheInputObjectIsCurrentlyBeingModified = 3,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in an execute macro response
		enum class ExecuteMacroResponseErrorBit : std::uint8_t
		{
			ObjectDoesntExist = 0,
			ObjectIsNotAMacro = 1,
			AnyOtherError = 2
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a hide/show object response
		enum class HideShowObjectErrorBit : std::uint8_t
		{
			ReferencesToMissingChildObjects = 0,
			InvalidObjectID = 1,
			CommandError = 2,
			Undefined = 3,
			AnyOtherError = 4
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a select input object response
		enum class SelectInputObjectErrorBit : std::uint8_t
		{
			ObjectIsDisabled = 0,
			InvalidObjectID = 1,
			ObjectIsNotOnTheActiveMaskOrIsInAHiddenContainer = 2,
			CouldNotCompleteAnotherFieldIsBeingModified = 3,
			AnyOtherError = 4,
			InvalidOptionValue = 5
		};

		/// @brief Enumerates the different responses to a select input object message
		enum class SelectInputObjectResponse : std::uint8_t
		{
			ObjectIsNotSelectedOrIsNullOrError = 0,
			ObjectIsSelected = 1,
			ObjectIsOpenedForEdit = 2 // VT version 4 and later
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a change polygon point response
		enum class ChangePolygonPointErrorBit : std::uint8_t
		{
			InvalidObjectID = 0,
			InvalidPointIndex = 1,
			AnyOtherError = 2
		};

		/// @brief Enumerates the bit indices of the error fields that can be set in a delete object pool response
		enum class DeleteObjectPoolErrorBit : std::uint8_t
		{
			DeletionError = 0,
			AnyOtherError = 8
		};

		/// @brief Checks to see if the message should be listened to based on
		/// what the message is, and if the client has sent the proper working set master message
		/// @param[in] message The CAN message to check
		/// @returns true if the source of the message is in a valid, managed state by our server, otherwise false
		bool check_if_source_is_managed(const CANMessage &message);

		/// @brief Processes a macro's execution synchronously as if it were a CAN message.
		/// Basically, if you want the server to execute a macro as if it were a CAN message, you can call this function
		/// though it will require you to create a CAN message to pass in. If you don't want to use this and
		/// instead want to manually affect the required changes in the object pool, that's fine too.
		/// @param[in] message The macro to execute
		void execute_macro_as_rx_message(const CANMessage &message);

		/// @brief Executes a macro synchronously by object ID.
		/// @param[in] objectIDOfMacro The object ID of the macro to execute
		/// @param[in] workingSet The working set to execute the macro on
		/// @returns true if the macro was executed, otherwise false
		bool execute_macro(std::uint16_t objectIDOfMacro, std::shared_ptr<VirtualTerminalServerManagedWorkingSet> workingSet);

		/// @brief Returns the priority to use, depending on the VT version
		/// @returns The priority to use, depending on the VT version
		CANIdentifier::CANPriority get_priority() const;

		/// @brief Maps a VTVersion to its corresponding byte representation
		/// @param[in] version The version to get the corresponding byte for
		/// @returns The VT version byte associated to the specified version
		static std::uint8_t get_vt_version_byte(VTVersion version);

		/// @brief Processes a CAN message from any VT client
		/// @param[in] message The CAN message being received
		/// @param[in] parent A context variable to find the relevant VT server class
		static void process_rx_message(const CANMessage &message, void *parent);

		/// @brief Sends a message using the acknowledgement PGN
		/// @param[in] type The type of acknowledgement to send (Ack, vs Nack, etc)
		/// @param[in] parameterGroupNumber The PGN to acknowledge
		/// @param[in] source The source control function to send from
		/// @param[in] destination The destination control function to send the acknowledgement to
		/// @returns true if the message was sent, false otherwise
		bool send_acknowledgement(AcknowledgementType type, std::uint32_t parameterGroupNumber, std::shared_ptr<InternalControlFunction> source, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change active mask command
		/// @param[in] newMaskObjectID The object ID for the new active mask
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_active_mask_response(std::uint16_t newMaskObjectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change attribute command
		/// @param[in] objectID The object ID for the target object
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] attributeID The attribute ID that was changed
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_attribute_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::uint8_t attributeID, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change background colour command
		/// @param[in] objectID The object ID for the object to change
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] colour The colour the background was set to
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_background_colour_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::uint8_t colour, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change child location command
		/// @param[in] parentObjectID The object ID for the parent of the object to move
		/// @param[in] objectID The object ID for the object to move
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_child_location_response(std::uint16_t parentObjectID, std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change child position command
		/// @param[in] parentObjectID The object ID for the parent of the object to move
		/// @param[in] objectID The object ID for the object to move
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_child_position_response(std::uint16_t parentObjectID, std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change fill attributes command
		/// @param[in] objectID The object ID for the object to change
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_fill_attributes_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change font attributes command
		/// @param[in] objectID The object ID for the object to change
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_font_attributes_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change list item command
		/// @param[in] objectID The object ID for the object to change
		/// @param[in] newObjectID The object ID for the object to place at the specified list index, or NULL_OBJECT_ID (0xFFFF)
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] listIndex The list index to change, numbered 0 to n
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_list_item_response(std::uint16_t objectID, std::uint16_t newObjectID, std::uint8_t errorBitfield, std::uint8_t listIndex, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change numeric value command
		/// @param[in] objectID The object ID for the object whose numeric value was meant to be changed
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] value The value that was set by the client
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_numeric_value_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::uint32_t value, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change polygon point command
		/// @param[in] objectID The object ID of the modified polygon
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_polygon_point_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change size command
		/// @param[in] objectID The object ID for the object whose size was meant to be changed
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_size_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change soft key mask command
		/// @param[in] objectID The object ID of a data mask or alarm mask
		/// @param[in] newObjectID The object ID of the soft key mask to apply to the mask indicated by objectID
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_soft_key_mask_response(std::uint16_t objectID, std::uint16_t newObjectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a change string value command
		/// @param[in] objectID The object ID for the object whose value was meant to be changed
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_change_string_value_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a delete version command
		/// @param[in] errorBitfield An error bitfield to report back to the client
		/// @param[in] destination The control function to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_delete_version_response(std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to a delete object pool command
		/// @param[in] errorBitfield An error bitfield to report back to the client
		/// @param[in] destination The control function to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_delete_object_pool_response(std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination) const;

		/// @brief Sends a response to the enable/disable object command
		/// @param[in] objectID The object ID for the object
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] value The enable/disable state that was set by the client
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_enable_disable_object_response(std::uint16_t objectID, std::uint8_t errorBitfield, bool value, std::shared_ptr<ControlFunction> destination);

		/// @brief This message is sent by the VT to a Working Set Master to acknowledge the End of Object Pool message.
		/// @details When the VT replies with an error of any type
		/// the VT should delete the object pool from volatile memory storage and inform the operator
		/// by an alarm type method of the suspension of the Working Set and indicate the reason for
		/// the deletion. On reception of this message, the responsible ECU(s) should enter a failsafe
		/// operation mode providing a safe shutdown procedure of the whole device.
		/// @param[in] success Indicates if the pool was error free
		/// @param[in] parentIDOfFaultingObject The parent object ID for the faulty object, or NULL_OBJECT_ID
		/// @param[in] faultingObjectID The faulty object's ID or the NULL_OBJECT_ID
		/// @param[in] errorCodes A bitfield of error codes that describe the issues with the pool
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_end_of_object_pool_response(bool success,
		                                      std::uint16_t parentIDOfFaultingObject,
		                                      std::uint16_t faultingObjectID,
		                                      std::uint8_t errorCodes,
		                                      std::shared_ptr<ControlFunction> destination);

		/// @brief Sends a response to the execute macro or extended macro command
		/// @param[in] objectID The object ID for the macro
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] destination The control function to send the message to
		/// @param[in] extendedMacro True if the macro is an extended macro, otherwise false
		/// @returns true if the message was sent, otherwise false
		bool send_execute_macro_or_extended_macro_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::shared_ptr<ControlFunction> destination, bool extendedMacro);

		/// @brief Sends a response to the hide/show object command
		/// @param[in] objectID The object ID for the object
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] value The hide/show state that was set by the client
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false
		bool send_hide_show_object_response(std::uint16_t objectID, std::uint8_t errorBitfield, bool value, std::shared_ptr<ControlFunction> destination);

		/// @brief Sends a response to the change priority command
		/// @param[in] objectID The object ID for the object
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] priority The priority that was set by the client
		/// @param[in] destination The control function to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_change_priority_response(std::uint16_t objectID, std::uint8_t errorBitfield, std::uint8_t priority, std::shared_ptr<ControlFunction> destination);

		/// @brief Sends a response to the select input object command
		/// @param[in] objectID The object ID for the object
		/// @param[in] errorBitfield An error bitfield
		/// @param[in] response The response to the select input object command
		/// @param[in] destination The control function to send the message to
		/// @returns True if the message was sent, otherwise false
		bool send_select_input_object_response(std::uint16_t objectID, std::uint8_t errorBitfield, SelectInputObjectResponse response, std::shared_ptr<ControlFunction> destination);

		/// @brief Sends the VT status message broadcast. The status message
		/// contains information such as which working set is the active one, and information about
		/// what the VT server is doing, such as busy flags. This message should be sent at 1 Hz.
		/// @returns true if the message was sent, otherwise false
		bool send_status_message();

		/// @brief Sends the list of objects that the server supports to a client, usually in
		/// response to a "get supported objects" message, which is used by a client.
		/// @param[in] destination The control function to send the message to
		/// @returns true if the message was sent, otherwise false.
		bool send_supported_objects(std::shared_ptr<ControlFunction> destination) const;

		/// @brief Cyclic update function
		void update();

		static constexpr std::uint8_t VERSION_LABEL_LENGTH = 7; ///< The length of a standard object pool version label

		EventDispatcher<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>> onRepaintEventDispatcher; ///< Event dispatcher for repaint events
		EventDispatcher<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>, std::uint16_t, std::uint16_t> onChangeActiveMaskEventDispatcher; ///< Event dispatcher for active mask change events
		EventDispatcher<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>, std::uint16_t, bool> onFocusObjectEventDispatcher; ///< Event dispatcher for focus object events
		LanguageCommandInterface languageCommandInterface; ///< The language command interface for the server
		std::shared_ptr<InternalControlFunction> serverInternalControlFunction; ///< The internal control function for the server
		std::vector<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>> managedWorkingSetList; ///< The list of managed working sets
		std::map<std::shared_ptr<VirtualTerminalServerManagedWorkingSet>, bool> managedWorkingSetIopLoadStateMap; ///< A map to hold the IOP load state per session
		std::shared_ptr<VirtualTerminalServerManagedWorkingSet> activeWorkingSet; ///< The active working set
		std::uint32_t statusMessageTimestamp_ms = 0; ///< The timestamp of the last status message sent
		std::uint16_t activeWorkingSetDataMaskObjectID = NULL_OBJECT_ID; ///< The object ID of the active working set's data mask
		std::uint16_t activeWorkingSetSoftkeyMaskObjectID = NULL_OBJECT_ID; ///< The object ID of the active working set's soft key mask
		std::uint8_t activeWorkingSetMasterAddress = NULL_CAN_ADDRESS; ///< The address of the active working set's master
		std::uint8_t busyCodesBitfield = 0; ///< The busy codes bitfield
		std::uint8_t currentCommandFunctionCode = 0; ///< The current command function code being processed
		bool initialized = false; ///< True if the server has been initialized, otherwise false
	};
} // namespace isobus
#endif //ISOBUS_VIRTUAL_TERMINAL_SERVER_HPP
