//================================================================================================
/// @file can_network_manager.hpp
///
/// @brief The main class that manages the ISOBUS stack including: callbacks, Name to Address
/// management, making control functions, and driving the various protocols.
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
///
/// @copyright 2022 The Open-Agriculture Developers
//================================================================================================

#ifndef CAN_NETWORK_MANAGER_HPP
#define CAN_NETWORK_MANAGER_HPP

#include "isobus/isobus/can_badge.hpp"
#include "isobus/isobus/can_callbacks.hpp"
#include "isobus/isobus/can_constants.hpp"
#include "isobus/isobus/can_control_function.hpp"
#include "isobus/isobus/can_extended_transport_protocol.hpp"
#include "isobus/isobus/can_identifier.hpp"
#include "isobus/isobus/can_internal_control_function.hpp"
#include "isobus/isobus/can_message.hpp"
#include "isobus/isobus/can_message_frame.hpp"
#include "isobus/isobus/can_network_configuration.hpp"
#include "isobus/isobus/can_partnered_control_function.hpp"
#include "isobus/isobus/can_transport_protocol.hpp"
#include "isobus/isobus/isobus_heartbeat.hpp"
#include "isobus/isobus/nmea2000_fast_packet_protocol.hpp"
#include "isobus/utility/event_dispatcher.hpp"
#include "isobus/utility/thread_synchronization.hpp"

#include <array>
#include <deque>
#include <list>
#include <memory>
#include <queue>

/// @brief This namespace encompasses all of the ISO11783 stack's functionality to reduce global namespace pollution
namespace isobus
{

	//================================================================================================
	/// @class CANNetworkManager
	///
	/// @brief The main CAN network manager object, handles protocol management and updating other
	/// stack components. Provides an interface for sending CAN messages.
	//================================================================================================
	class CANNetworkManager
	{
	public:
		static CANNetworkManager CANNetwork; ///< Static singleton of the one network manager. Use this to access stack functionality.

		/// @brief Initializer function for the network manager
		void initialize();

		/// @brief The factory function to construct an internal control function, also automatically initializes it to be functional
		/// @param[in] desiredName The NAME for this control function to claim as
		/// @param[in] CANPort The CAN channel index for this control function to use
		/// @param[in] preferredAddress Optionally, the preferred address for this control function to claim as, if not provided or NULL address,
		/// it will fallback onto a random preferred address somewhere in the arbitrary address range, which means your NAME must have the arbitrary address bit set.
		/// @returns A shared pointer to an InternalControlFunction object created with the parameters passed in
		std::shared_ptr<InternalControlFunction> create_internal_control_function(NAME desiredName, std::uint8_t CANPort, std::uint8_t preferredAddress = NULL_CAN_ADDRESS);

		/// @brief The factory function to construct a partnered control function, also automatically initializes it to be functional
		/// @param[in] CANPort The CAN channel associated with this control function definition
		/// @param[in] NAMEFilters A list of filters that describe the identity of the CF based on NAME components
		/// @returns A shared pointer to a PartneredControlFunction object created with the parameters passed in
		std::shared_ptr<PartneredControlFunction> create_partnered_control_function(std::uint8_t CANPort, const std::vector<NAMEFilter> &NAMEFilters);

		/// @brief Removes an internal control function from the network manager, making it inactive
		/// @param[in] controlFunction The control function to deactivate
		void deactivate_control_function(std::shared_ptr<InternalControlFunction> controlFunction);

		/// @brief Removes a partnered control function from the network manager, making it inactive
		/// @param[in] controlFunction The control function to deactivate
		void deactivate_control_function(std::shared_ptr<PartneredControlFunction> controlFunction);

		/// @brief Getter for a control function based on certain port and address, normally only used internaly.
		/// You should try to refrain from using addresses directly, instead try keeping a reference to the control function.
		/// @param[in] channelIndex CAN Channel index of the control function
		/// @param[in] address Address of the control function
		/// @returns A control function that matches the parameters, or nullptr if no match was found
		std::shared_ptr<ControlFunction> get_control_function(std::uint8_t channelIndex, std::uint8_t address) const;

		/// @brief This is how you register a callback for any PGN destined for the global address (0xFF)
		/// @param[in] parameterGroupNumber The PGN you want to register for
		/// @param[in] callback The callback that will be called when parameterGroupNumber is received from the global address (0xFF)
		/// @param[in] parent A generic context variable that helps identify what object the callback is destined for. Can be nullptr if you don't want to use it.
		void add_global_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent);

		/// @brief This is how you remove a callback for any PGN destined for the global address (0xFF)
		/// @param[in] parameterGroupNumber The PGN of the callback to remove
		/// @param[in] callback The callback that will be removed
		/// @param[in] parent A generic context variable that helps identify what object the callback was destined for
		void remove_global_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent);

		/// @brief Returns the number of global PGN callbacks that have been registered with the network manager
		/// @returns The number of global PGN callbacks that have been registered with the network manager
		std::size_t get_number_global_parameter_group_number_callbacks() const;

		/// @brief Registers a callback for ANY control function sending the associated PGN
		/// @param[in] parameterGroupNumber The PGN you want to register for
		/// @param[in] callback The callback that will be called when parameterGroupNumber is received from any control function
		/// @param[in] parent A generic context variable that helps identify what object the callback is destined for. Can be nullptr if you don't want to use it.
		void add_any_control_function_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent);

		/// @brief This is how you remove a callback added with add_any_control_function_parameter_group_number_callback
		/// @param[in] parameterGroupNumber The PGN of the callback to remove
		/// @param[in] callback The callback that will be removed
		/// @param[in] parent A generic context variable that helps identify what object the callback was destined for
		void remove_any_control_function_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent);

		/// @brief Returns the network manager's event dispatcher for notifying consumers whenever a
		/// message is transmitted by our application
		/// @returns An event dispatcher which can be used to get notified about transmitted messages
		EventDispatcher<CANMessage> &get_transmitted_message_event_dispatcher();

		/// @brief Returns an internal control function if the passed-in control function is an internal type
		/// @param[in] controlFunction The control function to get the internal control function from
		/// @returns An internal control function casted from the passed in control function
		std::shared_ptr<InternalControlFunction> get_internal_control_function(std::shared_ptr<ControlFunction> controlFunction) const;

		/// @brief Returns an estimated busload between 0.0f and 100.0f
		/// @details This calculates busload over a 1 second window.
		/// @note This function averages between best and worst case bit-stuffing.
		/// This may be more or less aggressive than the actual amount of bit stuffing. Knowing
		/// the actual amount of bit stuffing is impossible, so this should only be used as an estimate.
		/// @param[in] canChannel The channel to estimate the bus load for
		/// @returns Estimated busload over the last 1 second
		float get_estimated_busload(std::uint8_t canChannel);

		/// @brief This is the main way to send a CAN message of any length.
		/// @details This function will automatically choose an appropriate transport protocol if needed.
		/// If you don't specify a destination (or use nullptr) you message will be sent as a broadcast
		/// if it is valid to do so.
		/// You can also get a callback on success or failure of the transmit.
		/// @param[in] parameterGroupNumber The PGN to use when sending the message
		/// @param[in] dataBuffer A pointer to the data buffer to send from
		/// @param[in] dataLength The size of the message to send
		/// @param[in] sourceControlFunction The control function that is sending the message
		/// @param[in] destinationControlFunction The control function that the message is destined for or nullptr if broadcast
		/// @param[in] priority The CAN priority of the message being sent
		/// @param[in] txCompleteCallback A callback to be called when the message is sent or fails to send
		/// @param[in] parentPointer A generic context variable that helps identify what object the callback is destined for
		/// @param[in] frameChunkCallback A callback which can be supplied to have the tack call you back to get chunks of the message as they are sent
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_can_message(std::uint32_t parameterGroupNumber,
		                      const std::uint8_t *dataBuffer,
		                      std::uint32_t dataLength,
		                      std::shared_ptr<InternalControlFunction> sourceControlFunction,
		                      std::shared_ptr<ControlFunction> destinationControlFunction = nullptr,
		                      CANIdentifier::CANPriority priority = CANIdentifier::CANPriority::PriorityDefault6,
		                      TransmitCompleteCallback txCompleteCallback = nullptr,
		                      void *parentPointer = nullptr,
		                      DataChunkCallback frameChunkCallback = nullptr);

		/// @brief The main update function for the network manager. Updates all protocols.
		void update();

		/// @brief Used to tell the network manager when frames are received on the bus.
		/// @param[in] rxFrame Frame to process
		void process_receive_can_message_frame(const CANMessageFrame &rxFrame);

		/// @brief Used to tell the network manager when frames are emitted on the bus.
		/// @param[in] txFrame The frame that was just emitted onto the bus
		void process_transmitted_can_message_frame(const CANMessageFrame &txFrame);

		/// @brief Use this to get a callback when a control function goes online or offline.
		/// This could be useful if you want event driven notifications for when your partners are disconnected from the bus.
		/// @param[in] callback The callback you want to be called when the any control function changes state
		void add_control_function_status_change_callback(ControlFunctionStateCallback callback);

		/// @brief Used to remove callbacks added with add_control_function_status_change_callback
		/// @param[in] callback The callback you want to remove
		void remove_control_function_status_change_callback(ControlFunctionStateCallback callback);

		/// @brief Gets all the internal control functions that are currently registered in the network manager
		/// @returns A list of all the internal control functions
		const std::list<std::shared_ptr<InternalControlFunction>> &get_internal_control_functions() const;

		/// @brief Gets all the partnered control functions that are currently registered in the network manager
		/// @returns A list of all the partnered control functions
		const std::list<std::shared_ptr<PartneredControlFunction>> &get_partnered_control_functions() const;

		/// @brief Gets all the control functions that are known to the network manager
		/// @param[in] includingOffline If true, all control functions are returned, otherwise only online control functions are returned
		/// @returns A list of all the control functions
		std::list<std::shared_ptr<ControlFunction>> get_control_functions(bool includingOffline) const;

		/// @brief Gets all the active transport protocol sessions that are currently active
		/// @note The list returns pointers to the transport protocol sessions, but they can disappear at any time
		/// @param[in] canPortIndex The CAN channel index to get the transport protocol sessions for
		/// @returns A list of all the active transport protocol sessions
		std::list<std::shared_ptr<TransportProtocolSessionBase>> get_active_transport_protocol_sessions(std::uint8_t canPortIndex) const;

		/// @brief Returns the class instance of the NMEA2k fast packet protocol.
		/// Use this to register for FP multipacket messages
		/// @param[in] canPortIndex The CAN channel index to get the fast packet protocol for
		/// @returns The class instance of the NMEA2k fast packet protocol.
		std::unique_ptr<FastPacketProtocol> &get_fast_packet_protocol(std::uint8_t canPortIndex);

		/// @brief Returns an interface which can be used to manage ISO11783-7 heartbeat messages.
		/// @param[in] canPortIndex The index of the CAN channel associated to the interface you're requesting
		/// @returns ISO11783-7 heartbeat interface
		HeartbeatInterface &get_heartbeat_interface(std::uint8_t canPortIndex);

		/// @brief Returns the configuration of this network manager
		/// @returns The configuration class for this network manager
		CANNetworkConfiguration &get_configuration();

		/// @brief Returns the network manager's event dispatcher for notifying consumers whenever an
		/// address violation occurs involving an internal control function.
		/// @returns An event dispatcher which can be used to get notified about address violations
		EventDispatcher<std::shared_ptr<InternalControlFunction>> &get_address_violation_event_dispatcher();

	protected:
		// Using protected region to allow protocols use of special functions from the network manager
		friend class InternalControlFunction; ///< Allows the network manager to work closely with the address claiming process
		friend class ExtendedTransportProtocolManager; ///< Allows the network manager to access the ETP manager
		friend class TransportProtocolManager; ///< Allows the network manager to work closely with the transport protocol manager object
		friend class DiagnosticProtocol; ///< Allows the diagnostic protocol to access the protected functions on the network manager
		friend class ParameterGroupNumberRequestProtocol; ///< Allows the PGN request protocol to access the network manager protected functions
		friend class FastPacketProtocol; ///< Allows the FP protocol to access the network manager protected functions

		/// @brief Adds a PGN callback for a protocol class
		/// @param[in] parameterGroupNumber The PGN to register for
		/// @param[in] callback The callback to call when the PGN is received
		/// @param[in] parentPointer A generic context variable that helps identify what object the callback was destined for
		/// @returns `true` if the callback was added, otherwise `false`
		bool add_protocol_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parentPointer);

		/// @brief Removes a PGN callback for a protocol class
		/// @param[in] parameterGroupNumber The PGN to register for
		/// @param[in] callback The callback to call when the PGN is received
		/// @param[in] parentPointer A generic context variable that helps identify what object the callback was destined for
		/// @returns `true` if the callback was removed, otherwise `false`
		bool remove_protocol_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parentPointer);

		/// @brief Sends a CAN message using raw addresses. Used only by the stack.
		/// @param[in] portIndex The CAN channel index to send the message from
		/// @param[in] sourceAddress The source address to send the CAN message from
		/// @param[in] destAddress The destination address to send the message to
		/// @param[in] parameterGroupNumber The PGN to use when sending the message
		/// @param[in] priority The CAN priority of the message being sent
		/// @param[in] data A pointer to the data buffer to send from
		/// @param[in] size The size of the message to send
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_can_message_raw(std::uint32_t portIndex,
		                          std::uint8_t sourceAddress,
		                          std::uint8_t destAddress,
		                          std::uint32_t parameterGroupNumber,
		                          std::uint8_t priority,
		                          const void *data,
		                          std::uint32_t size,
		                          CANLibBadge<InternalControlFunction>) const;

		/// @brief Processes completed protocol messages. Causes PGN callbacks to trigger.
		/// @param[in] message The completed protocol message
		void protocol_message_callback(const CANMessage &message);

	private:
		/// @brief Constructor for the network manager. Sets default values for members
		CANNetworkManager();

		/// @brief Factory function to create an external control function, also automatically assigns it to the lookup table.
		/// @param[in] desiredName The NAME of the control function
		/// @param[in] address The address of the control function
		/// @param[in] CANPort The CAN channel index of the control function
		/// @returns A shared pointer to the control function created
		std::shared_ptr<ControlFunction> create_external_control_function(NAME desiredName, std::uint8_t address, std::uint8_t CANPort);

		/// @brief Removes a control function from the network manager, making it inactive
		/// @param[in] controlFunction The control function to remove
		void deactivate_control_function(std::shared_ptr<ControlFunction> controlFunction);

		/// @brief Updates the internal address table based on a received CAN message
		/// @param[in] message A message being received by the stack
		void update_address_table(const CANMessage &message);

		/// @brief Updates the internal address table based on updates to internal cfs addresses
		void update_internal_cfs();

		/// @brief Processes a message for each internal control function for address claiming
		/// @param[in] message The message to process
		void process_rx_message_for_address_claiming(const CANMessage &message) const;

		/// @brief Processes a CAN message's contribution to the current busload
		/// @param[in] channelIndex The CAN channel index associated to the message being processed
		/// @param[in] numberOfBitsProcessed The number of bits to add to the busload calculation
		void update_busload(std::uint8_t channelIndex, std::uint32_t numberOfBitsProcessed);

		/// @brief Updates the stored bit accumulators for calculating the bus load over a multiple sample windows
		void update_busload_history();

		/// @brief Creates new control function classes based on the frames coming in from the bus
		/// @param[in] rxFrame Raw frames coming in from the bus
		void update_control_functions(const CANMessageFrame &rxFrame);

		/// @brief Checks if new partners have been created and matches them to existing control functions
		void update_new_partners();

		/// @brief Builds a CAN frame from a frame's discrete components
		/// @param[in] portIndex The CAN channel index of the CAN message being processed
		/// @param[in] sourceAddress The source address to send the CAN message from
		/// @param[in] destAddress The destination address to send the message to
		/// @param[in] parameterGroupNumber The PGN to use when sending the message
		/// @param[in] priority The CAN priority of the message being sent
		/// @param[in] data A pointer to the data buffer to send from
		/// @param[in] size The size of the message to send
		/// @returns The constructed frame based on the inputs
		CANMessageFrame construct_frame(std::uint32_t portIndex,
		                                std::uint8_t sourceAddress,
		                                std::uint8_t destAddress,
		                                std::uint32_t parameterGroupNumber,
		                                std::uint8_t priority,
		                                const void *data,
		                                std::uint32_t size) const;

		/// @brief Get the next CAN message from the received message queue, and remove it from the queue.
		/// @note This will only ever get an 8 byte message because they are directly translated from CAN frames.
		/// @returns The message that was at the front of the queue, or an invalid message if the queue is empty
		CANMessage get_next_can_message_from_rx_queue();

		/// @brief Get the next CAN message from the received message queue, and remove it from the queue
		/// @note This will only ever get an 8 byte message because they are directly translated from CAN frames.
		/// @returns The message that was at the front of the queue, or an invalid message if the queue is empty
		CANMessage get_next_can_message_from_tx_queue();

		/// @brief Processes a can message for callbacks added with add_any_control_function_parameter_group_number_callback
		/// @param[in] currentMessage The message to process
		void process_any_control_function_pgn_callbacks(const CANMessage &currentMessage);

		/// @brief Validates that a CAN message has not caused an address violation.
		/// If a violation is found, the network manager will notify the affected address claim state machine
		/// to re-claim as is required by ISO 11783-5, and will attempt to activate a DTC that is defined in ISO 11783-5.
		/// @note Address violation occurs when two CFs are using the same source address.
		/// @param[in] currentMessage The message to process
		void process_can_message_for_address_violations(const CANMessage &currentMessage);

		/// @brief Checks the control function state callback list to see if we need to call
		/// a control function state callback.
		/// @param[in] controlFunction The controlFunction whose state has changed
		/// @param[in] state The new state of the control function
		void process_control_function_state_change_callback(std::shared_ptr<ControlFunction> controlFunction, ControlFunctionState state);

		/// @brief Processes a can message for callbacks added with add_protocol_parameter_group_number_callback
		/// @param[in] currentMessage The message to process
		void process_protocol_pgn_callbacks(const CANMessage &currentMessage);

		/// @brief Matches a CAN message to any matching PGN callback, and calls that callback
		/// @param[in] message A pointer to a CAN message to be processed
		void process_can_message_for_global_and_partner_callbacks(const CANMessage &message) const;

		/// @brief Processes the internal received message queue
		void process_rx_messages();

		/// @brief Processes the internal transmitted message queue
		void process_tx_messages();

		/// @brief Checks to see if any control function didn't claim during a round of
		/// address claiming and removes it if needed.
		void prune_inactive_control_functions();

		/// @brief Sends a CAN message using raw addresses. Used only by the stack.
		/// @param[in] portIndex The CAN channel index to send the message from
		/// @param[in] sourceAddress The source address to send the CAN message from
		/// @param[in] destAddress The destination address to send the message to
		/// @param[in] parameterGroupNumber The PGN to use when sending the message
		/// @param[in] priority The CAN priority of the message being sent
		/// @param[in] data A pointer to the data buffer to send from
		/// @param[in] size The size of the message to send
		/// @returns `true` if the message was sent, otherwise `false`
		bool send_can_message_raw(std::uint32_t portIndex,
		                          std::uint8_t sourceAddress,
		                          std::uint8_t destAddress,
		                          std::uint32_t parameterGroupNumber,
		                          std::uint8_t priority,
		                          const void *data,
		                          std::uint32_t size) const;

		/// @brief Gets a PGN callback for the global address by index
		/// @param[in] index The index of the callback to get
		/// @returns A structure containing the global PGN callback data
		ParameterGroupNumberCallbackData get_global_parameter_group_number_callback(std::size_t index) const;

		static constexpr std::uint32_t BUSLOAD_SAMPLE_WINDOW_MS = 1000; ///< Using a 1s window to average the bus load, otherwise it's very erratic
		static constexpr std::uint32_t BUSLOAD_UPDATE_FREQUENCY_MS = 100; ///< Bus load bit accumulation happens over a 100ms window

		CANNetworkConfiguration configuration; ///< The configuration for this network manager
		std::array<std::unique_ptr<TransportProtocolManager>, CAN_PORT_MAXIMUM> transportProtocols; ///< One instance of the transport protocol manager for each channel
		std::array<std::unique_ptr<ExtendedTransportProtocolManager>, CAN_PORT_MAXIMUM> extendedTransportProtocols; ///< One instance of the extended transport protocol manager for each channel
		std::array<std::unique_ptr<FastPacketProtocol>, CAN_PORT_MAXIMUM> fastPacketProtocol; ///< One instance of the fast packet protocol for each channel
		std::array<std::unique_ptr<HeartbeatInterface>, CAN_PORT_MAXIMUM> heartBeatInterfaces; ///< Manages ISOBUS heartbeat requests, one per channel

		std::array<std::deque<std::uint32_t>, CAN_PORT_MAXIMUM> busloadMessageBitsHistory; ///< Stores the approximate number of bits processed on each channel over multiple previous time windows
		std::array<std::uint32_t, CAN_PORT_MAXIMUM> currentBusloadBitAccumulator; ///< Accumulates the approximate number of bits processed on each channel during the current time window
		std::array<std::uint32_t, CAN_PORT_MAXIMUM> lastAddressClaimRequestTimestamp_ms; ///< Stores timestamps for when the last request for the address claim PGN was received. Used to prune stale CFs.

		std::array<std::array<std::shared_ptr<ControlFunction>, NULL_CAN_ADDRESS>, CAN_PORT_MAXIMUM> controlFunctionTable; ///< Table to maintain address to NAME mappings
		std::list<std::shared_ptr<ControlFunction>> inactiveControlFunctions; ///< A list of the control function that currently don't have a valid address
		std::list<std::shared_ptr<InternalControlFunction>> internalControlFunctions; ///< A list of the internal control functions
		Mutex internalControlFunctionsMutex; ///< A mutex for internal control functions thread safety
		std::list<std::shared_ptr<PartneredControlFunction>> partneredControlFunctions; ///< A list of the partnered control functions

		std::list<ParameterGroupNumberCallbackData> protocolPGNCallbacks; ///< A list of PGN callback registered by CAN protocols
		std::queue<CANMessage> receivedMessageQueue; ///< A queue of received messages to process
		std::queue<CANMessage> transmittedMessageQueue; ///< A queue of transmitted messages to process (already sent, so changes to the message won't affect the bus)
		std::list<ControlFunctionStateCallback> controlFunctionStateCallbacks; ///< List of all control function state callbacks
		std::vector<ParameterGroupNumberCallbackData> globalParameterGroupNumberCallbacks; ///< A list of all global PGN callbacks
		std::vector<ParameterGroupNumberCallbackData> anyControlFunctionParameterGroupNumberCallbacks; ///< A list of all global PGN callbacks
		EventDispatcher<CANMessage> messageTransmittedEventDispatcher; ///< An event dispatcher for notifying consumers about transmitted messages by our application
		EventDispatcher<std::shared_ptr<InternalControlFunction>> addressViolationEventDispatcher; ///< An event dispatcher for notifying consumers about address violations
		Mutex receivedMessageQueueMutex; ///< A mutex for receive messages thread safety
		Mutex protocolPGNCallbacksMutex; ///< A mutex for PGN callback thread safety
		Mutex anyControlFunctionCallbacksMutex; ///< Mutex to protect the "any CF" callbacks
		Mutex busloadUpdateMutex; ///< A mutex that protects the busload metrics since we calculate it on our own thread
		Mutex controlFunctionStatusCallbacksMutex; ///< A Mutex that protects access to the control function status callback list
		Mutex transmittedMessageQueueMutex; ///< A mutex for protecting the transmitted message queue
		std::uint32_t busloadUpdateTimestamp_ms = 0; ///< Tracks a time window for determining approximate busload
		std::uint32_t updateTimestamp_ms = 0; ///< Keeps track of the last time the CAN stack was update in milliseconds
		bool initialized = false; ///< True if the network manager has been initialized by the update function
	};

} // namespace isobus

#endif // CAN_NETWORK_MANAGER_HPP
