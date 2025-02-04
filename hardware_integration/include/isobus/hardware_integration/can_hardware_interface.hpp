//================================================================================================
/// @file can_hardware_interface.hpp
///
/// @brief The hardware abstraction layer that separates the stack from the underlying CAN driver
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#ifndef CAN_HARDWARE_INTERFACE_HPP
#define CAN_HARDWARE_INTERFACE_HPP

#include "isobus/hardware_integration/can_hardware_plugin.hpp"
#include "isobus/isobus/can_hardware_abstraction.hpp"
#include "isobus/isobus/can_message_frame.hpp"
#include "isobus/utility/event_dispatcher.hpp"

#include <atomic>
#include <cstdint>
#include <cstring>
#include <deque>
#include <vector>

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
#include <condition_variable>
#include <thread>
#endif

namespace isobus
{
	/// @brief Provides a common queuing and thread layer for running the CAN stack and all CAN drivers
	/// @details The `CANHardwareInterface` class was created to provide a common queuing and thread
	/// layer for running the CAN stack and all CAN drivers to simplify integration and crucially to
	/// provide a consistent, safe order of operations for all the function calls needed to properly
	/// drive the stack.
	class CANHardwareInterface
	{
	public:
		/// @brief Returns the number of configured CAN channels that the class is managing
		/// @returns The number of configured CAN channels that the class is managing
		static std::uint8_t get_number_of_can_channels();

		/// @brief Sets the number of CAN channels to manage
		/// @details Allocates the proper number of `CanHardware` objects to track
		/// each CAN channel's Tx and Rx message queues. If you pass in a smaller number than what was
		/// already configured, it will delete the unneeded `CanHardware` objects.
		/// @note The function will fail if the channel is already assigned to a driver or the interface is already started
		/// @param value The number of CAN channels to manage
		/// @param queueCapacity The capacity of the transmit and receive queues
		/// @returns `true` if the channel count was set, otherwise `false`.
		static bool set_number_of_can_channels(std::uint8_t value, std::size_t queueCapacity = 40);

		/// @brief Assigns a CAN driver to a channel
		/// @param[in] channelIndex The channel to assign to
		/// @param[in] canDriver The driver to assign to the channel
		/// @note The function will fail if the channel is already assigned to a driver or the interface is already started
		/// @returns `true` if the driver was assigned to the channel, otherwise `false`
		static bool assign_can_channel_frame_handler(std::uint8_t channelIndex, std::shared_ptr<CANHardwarePlugin> canDriver);

		/// @brief Removes a CAN driver from a channel
		/// @param[in] channelIndex The channel to remove the driver from
		/// @note The function will fail if the channel is already assigned to a driver or the interface is already started
		/// @returns `true` if the driver was removed from the channel, otherwise `false`
		static bool unassign_can_channel_frame_handler(std::uint8_t channelIndex);

		/// @brief Gets the CAN driver assigned to a channel
		/// @param[in] channelIndex The channel to get the driver from
		/// @returns The driver assigned to the channel, or `nullptr` if the channel is not assigned
		static std::shared_ptr<CANHardwarePlugin> get_assigned_can_channel_frame_handler(std::uint8_t channelIndex);

		/// @brief Starts the threads for managing the CAN stack and CAN drivers
		/// @returns `true` if the threads were started, otherwise false (perhaps they are already running)
		static bool start();

		/// @brief Stops all CAN management threads and discards all remaining messages in the Tx and Rx queues.
		/// @returns `true` if the threads were stopped, otherwise `false`
		static bool stop();

		/// @brief Checks if the CAN stack and CAN drivers are running
		/// @returns `true` if the threads are running, otherwise `false`
		static bool is_running();

		/// @brief Called externally, adds a message to a CAN channel's Tx queue
		/// @param[in] frame The frame to add to the Tx queue
		/// @returns `true` if the frame was accepted, otherwise `false` (maybe wrong channel assigned)
		static bool transmit_can_frame(const CANMessageFrame &frame);

		/// @brief Get the event dispatcher for when a CAN message frame is received from hardware event
		/// @returns The event dispatcher which can be used to register callbacks/listeners to
		static EventDispatcher<const CANMessageFrame &> &get_can_frame_received_event_dispatcher();

		/// @brief Get the event dispatcher for when a CAN message frame will be send to hardware event
		/// @returns The event dispatcher which can be used to register callbacks/listeners to
		static EventDispatcher<const CANMessageFrame &> &get_can_frame_transmitted_event_dispatcher();

		/// @brief Get the event dispatcher for when a periodic update is called
		/// @returns The event dispatcher which can be used to register callbacks/listeners to
		static EventDispatcher<> &get_periodic_update_event_dispatcher();

		/// @brief Call this periodically if you have threads disabled.
		/// @note Try to call this very often, say at least every millisecond to ensure CAN messages are retrieved from the hardware
		static void update();

		/// @brief Set the interval between periodic updates to the network manager
		/// @param[in] value The interval between update calls in milliseconds
		static void set_periodic_update_interval(std::uint32_t value);

		/// @brief Get the interval between periodic updates to the network manager
		/// @returns The interval between update calls in milliseconds
		static std::uint32_t get_periodic_update_interval();

	private:
		/// @brief Stores the data for a single CAN channel
		class CANHardware
		{
		public:
			/// @brief Constructor for the CANHardware
			/// @param[in] queueCapacity The capacity of the transmit and receive queues
			explicit CANHardware(std::size_t queueCapacity);

			/// @brief Destructor for the CANHardware
			virtual ~CANHardware();

			/// @brief Starts this hardware channel
			/// @returns `true` if the channel was started, otherwise `false`
			bool start();

			/// @brief Stops this hardware channel
			/// @returns `true` if the channel was stopped, otherwise `false`
			bool stop();

			/// @brief Try to transmit the frame to the hardware
			/// @param[in] frame The frame to transmit
			/// @returns `true` if the frame was transmitted, otherwise `false`
			bool transmit_can_frame(const CANMessageFrame &frame) const;

			/// @brief Receives a frame from the hardware and adds it to the receive queue
			/// @returns `true` if a frame was received, otherwise `false`
			bool receive_can_frame();

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
			/// @brief Starts the receiving thread for this CAN channel
			void start_threads();

			/// @brief Stops the receiving thread for this CAN channel
			void stop_threads();

			/// @brief The receiving thread loop for this CAN channel
			void receive_thread_function();

			std::unique_ptr<std::thread> receiveMessageThread; ///< Thread to manage getting messages from a CAN channel
			std::atomic_bool receiveThreadRunning = { false }; ///< Flag to indicate if the receive thread is running
#endif

			std::shared_ptr<CANHardwarePlugin> frameHandler; ///< The CAN driver to use for a CAN channel

			LockFreeQueue<CANMessageFrame> messagesToBeTransmittedQueue; ///< Transmit message queue for a CAN channel
			LockFreeQueue<CANMessageFrame> receivedMessagesQueue; ///< Receive message queue for a CAN channel
		};

		/// @brief Singleton instance of the CANHardwareInterface class
		/// @details This is a little hack that allows to have the destructor called
		static CANHardwareInterface SINGLETON;

		/// @brief The default update interval for the CAN stack. Mostly arbitrary
		static constexpr std::uint32_t PERIODIC_UPDATE_INTERVAL = 4;

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		/// @brief Deconstructor for the CANHardwareInterface class for stopping threads
		virtual ~CANHardwareInterface();

		/// @brief The main thread loop for updating the stack
		static void update_thread_function();

		/// @brief Starts all threads related to the hardware interface
		static void start_threads();

		/// @brief Stops all threads related to the hardware interface
		static void stop_threads();

		static std::unique_ptr<std::thread> updateThread; ///< The main thread
		static std::condition_variable updateThreadWakeupCondition; ///< A condition variable to allow for signaling the `updateThread` to wakeup
#endif
		static std::uint32_t lastUpdateTimestamp; ///< The last time the network manager was updated
		static std::uint32_t periodicUpdateInterval; ///< The period between calls to the network manager update function in milliseconds
		static EventDispatcher<const CANMessageFrame &> frameReceivedEventDispatcher; ///< The event dispatcher for when a CAN message frame is received from hardware event
		static EventDispatcher<const CANMessageFrame &> frameTransmittedEventDispatcher; ///< The event dispatcher for when a CAN message has been transmitted via hardware
		static EventDispatcher<> periodicUpdateEventDispatcher; ///< The event dispatcher for when a periodic update is called

		static std::vector<std::unique_ptr<CANHardware>> hardwareChannels; ///< A list of all CAN channel's metadata
		static Mutex hardwareChannelsMutex; ///< Mutex to protect `hardwareChannels`
		static Mutex updateMutex; ///< A mutex for the main thread
		static std::atomic_bool started; ///< Stores if the threads have been started
	};
}
#endif // CAN_HARDWARE_INTERFACE_HPP
