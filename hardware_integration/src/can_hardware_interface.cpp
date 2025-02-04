//================================================================================================
/// @file can_hardware_interface.cpp
///
/// @brief The hardware abstraction layer that separates the stack from the underlying CAN driver
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#include "isobus/hardware_integration/can_hardware_interface.hpp"
#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/utility/system_timing.hpp"
#include "isobus/utility/to_string.hpp"

#include <algorithm>
#include <limits>

namespace isobus
{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
	std::unique_ptr<std::thread> CANHardwareInterface::updateThread;
	std::condition_variable CANHardwareInterface::updateThreadWakeupCondition;
#endif
	std::uint32_t CANHardwareInterface::periodicUpdateInterval = PERIODIC_UPDATE_INTERVAL;
	std::uint32_t CANHardwareInterface::lastUpdateTimestamp;

	EventDispatcher<const CANMessageFrame &> CANHardwareInterface::frameReceivedEventDispatcher;
	EventDispatcher<const CANMessageFrame &> CANHardwareInterface::frameTransmittedEventDispatcher;
	EventDispatcher<> CANHardwareInterface::periodicUpdateEventDispatcher;

	std::vector<std::unique_ptr<CANHardwareInterface::CANHardware>> CANHardwareInterface::hardwareChannels;
	Mutex CANHardwareInterface::hardwareChannelsMutex;
	Mutex CANHardwareInterface::updateMutex;
	std::atomic_bool CANHardwareInterface::started = { false };

	CANHardwareInterface CANHardwareInterface::SINGLETON;

	CANHardwareInterface::CANHardware::CANHardware(std::size_t queueCapacity) :
	  messagesToBeTransmittedQueue(queueCapacity), receivedMessagesQueue(queueCapacity)
	{
	}

	CANHardwareInterface::CANHardware::~CANHardware()
	{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		stop_threads();
#endif
	}

	bool CANHardwareInterface::CANHardware::start()
	{
		bool retVal = false;
		if (nullptr != frameHandler)
		{
			frameHandler->open();
			if (frameHandler->get_is_valid())
			{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
				start_threads();
#endif
				retVal = true;
			}
		}
		return retVal;
	}

	bool CANHardwareInterface::CANHardware::stop()
	{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		stop_threads();
#endif
		if (nullptr != frameHandler)
		{
			frameHandler = nullptr;
		}
		messagesToBeTransmittedQueue.clear();
		receivedMessagesQueue.clear();
		return false;
	}

	bool CANHardwareInterface::CANHardware::transmit_can_frame(const CANMessageFrame &frame) const
	{
		if ((nullptr != frameHandler) && frameHandler->get_is_valid() && frameHandler->write_frame(frame))
		{
			return true;
		}
		return false;
	}

	bool CANHardwareInterface::CANHardware::receive_can_frame()
	{
		if ((nullptr != frameHandler) && frameHandler->get_is_valid() && (!receivedMessagesQueue.is_full()))
		{
			CANMessageFrame frame;
			if (frameHandler->read_frame(frame))
			{
				receivedMessagesQueue.push(frame);
				return true; // Indicate that a frame was read
			}
		}
		return false;
	}

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
	CANHardwareInterface::~CANHardwareInterface()
	{
		stop_threads();
	}

	void CANHardwareInterface::CANHardware::start_threads()
	{
		receiveThreadRunning = true;
		if (nullptr == receiveMessageThread)
		{
			receiveMessageThread.reset(new std::thread([this]() { receive_thread_function(); }));
		}
	}

	void CANHardwareInterface::CANHardware::stop_threads()
	{
		receiveThreadRunning = false;
		if (nullptr != receiveMessageThread)
		{
			if (receiveMessageThread->joinable())
			{
				receiveMessageThread->join();
			}
			receiveMessageThread = nullptr;
		}
	}

	void CANHardwareInterface::CANHardware::receive_thread_function()
	{
		while (receiveThreadRunning)
		{
			if ((nullptr != frameHandler) && frameHandler->get_is_valid())
			{
				if (!receive_can_frame())
				{
					// There was no frame to receive, so if any other thread wants to do something, let it.
					std::this_thread::yield();
				}
				else
				{
					CANHardwareInterface::updateThreadWakeupCondition.notify_all();
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Arbitrary, but don't want to infinite loop on the validity check.
			}
		}
	}
#endif

	bool send_can_message_frame_to_hardware(const CANMessageFrame &frame)
	{
		return CANHardwareInterface::transmit_can_frame(frame);
	}

	bool CANHardwareInterface::set_number_of_can_channels(std::uint8_t value, std::size_t queueCapacity)
	{
		LOCK_GUARD(Mutex, hardwareChannelsMutex);

		if (started)
		{
			LOG_ERROR("[HardwareInterface] Cannot set number of channels after interface is started.");
			return false;
		}

		while (value > static_cast<std::uint8_t>(hardwareChannels.size()))
		{
			hardwareChannels.emplace_back(new CANHardware(queueCapacity));
		}
		while (value < static_cast<std::uint8_t>(hardwareChannels.size()))
		{
			hardwareChannels.pop_back();
		}
		return true;
	}

	bool CANHardwareInterface::assign_can_channel_frame_handler(std::uint8_t channelIndex, std::shared_ptr<CANHardwarePlugin> driver)
	{
		LOCK_GUARD(Mutex, hardwareChannelsMutex);

		if (started)
		{
			LOG_ERROR("[HardwareInterface] Cannot assign frame handlers after interface is started.");
			return false;
		}

		if (channelIndex >= static_cast<std::uint8_t>(hardwareChannels.size()))
		{
			LOG_ERROR("[HardwareInterface] Unable to set frame handler at channel " + to_string(channelIndex) +
			          ", because there are only " + to_string(hardwareChannels.size()) + " channels set. " +
			          "Use set_number_of_can_channels() to increase the number of channels before assigning frame handlers.");
			return false;
		}

		if (nullptr != hardwareChannels[channelIndex]->frameHandler)
		{
			LOG_ERROR("[HardwareInterface] Unable to set frame handler at channel " + to_string(channelIndex) + ", because it is already assigned.");
			return false;
		}

		hardwareChannels[channelIndex]->frameHandler = driver;
		return true;
	}

	std::uint8_t CANHardwareInterface::get_number_of_can_channels()
	{
		return static_cast<std::uint8_t>(hardwareChannels.size() & std::numeric_limits<std::uint8_t>::max());
	}

	bool CANHardwareInterface::unassign_can_channel_frame_handler(std::uint8_t channelIndex)
	{
		LOCK_GUARD(Mutex, hardwareChannelsMutex);

		if (started)
		{
			LOG_ERROR("[HardwareInterface] Cannot remove frame handlers after interface is started.");
			return false;
		}

		if (channelIndex >= static_cast<std::uint8_t>(hardwareChannels.size()))
		{
			LOG_ERROR("[HardwareInterface] Unable to remove frame handler at channel " + to_string(channelIndex) +
			          ", because there are only " + to_string(hardwareChannels.size()) + " channels set.");
			return false;
		}

		if (nullptr == hardwareChannels[channelIndex]->frameHandler)
		{
			LOG_ERROR("[HardwareInterface] Unable to remove frame handler at channel " + to_string(channelIndex) + ", because it is not assigned.");
			return false;
		}

		hardwareChannels[channelIndex]->frameHandler = nullptr;
		return true;
	}

	std::shared_ptr<CANHardwarePlugin> CANHardwareInterface::get_assigned_can_channel_frame_handler(std::uint8_t channelIndex)
	{
		std::shared_ptr<CANHardwarePlugin> retVal;

		if (channelIndex < static_cast<std::uint8_t>(hardwareChannels.size()))
		{
			retVal = hardwareChannels.at(channelIndex)->frameHandler;
		}
		return retVal;
	}

	bool CANHardwareInterface::start()
	{
		LOCK_GUARD(Mutex, hardwareChannelsMutex);

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		start_threads();
#endif
		std::for_each(hardwareChannels.begin(), hardwareChannels.end(), [](const std::unique_ptr<CANHardware> &channel) {
			channel->start();
		});

		started = true;
		return true;
	}

	bool CANHardwareInterface::stop()
	{
		if (!started)
		{
			LOG_ERROR("[HardwareInterface] Cannot stop interface before it is started.");
			return false;
		}
		frameReceivedEventDispatcher.clear_listeners();
		frameTransmittedEventDispatcher.clear_listeners();
		periodicUpdateEventDispatcher.clear_listeners();

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		stop_threads();
#endif

		LOCK_GUARD(Mutex, hardwareChannelsMutex);
		std::for_each(hardwareChannels.begin(), hardwareChannels.end(), [](const std::unique_ptr<CANHardware> &channel) {
			channel->stop();
		});
		return true;
	}

	bool CANHardwareInterface::is_running()
	{
		return started;
	}

	bool CANHardwareInterface::transmit_can_frame(const CANMessageFrame &frame)
	{
		if (!started)
		{
			LOG_ERROR("[HardwareInterface] Cannot transmit message before interface is started.");
			return false;
		}

		if (frame.channel >= static_cast<std::uint8_t>(hardwareChannels.size()))
		{
			LOG_ERROR("[HardwareInterface] Cannot transmit message on channel " + to_string(frame.channel) +
			          ", because there are only " + to_string(hardwareChannels.size()) + " channels set.");
			return false;
		}

		const std::unique_ptr<CANHardware> &channel = hardwareChannels[frame.channel];
		if (nullptr == channel->frameHandler)
		{
			LOG_ERROR("[HardwareInterface] Cannot transmit message on channel " + to_string(frame.channel) + ", because it is not assigned.");
			return false;
		}

		if ((channel->frameHandler->get_is_valid()) &&
		    (channel->messagesToBeTransmittedQueue.push(frame)))
		{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
			updateThreadWakeupCondition.notify_all();
#endif
			return true;
		}
		return false;
	}

	EventDispatcher<const CANMessageFrame &> &CANHardwareInterface::get_can_frame_received_event_dispatcher()
	{
		return frameReceivedEventDispatcher;
	}

	EventDispatcher<const CANMessageFrame &> &CANHardwareInterface::get_can_frame_transmitted_event_dispatcher()
	{
		return frameTransmittedEventDispatcher;
	}

	EventDispatcher<> &CANHardwareInterface::get_periodic_update_event_dispatcher()
	{
		return periodicUpdateEventDispatcher;
	}

	void CANHardwareInterface::set_periodic_update_interval(std::uint32_t value)
	{
		periodicUpdateInterval = value;
	}

	std::uint32_t CANHardwareInterface::get_periodic_update_interval()
	{
		return periodicUpdateInterval;
	}

	void CANHardwareInterface::update()
	{
		if (started)
		{
			{
				// Stage 1 - Receiving messages from hardware
				LOCK_GUARD(Mutex, hardwareChannelsMutex);
				for (std::uint8_t i = 0; i < hardwareChannels.size(); i++)
				{
#if defined CAN_STACK_DISABLE_THREADS || defined ARDUINO
					// If we don't have threads, we need to poll the hardware for messages here
					hardwareChannels[i]->receive_can_frame();
#endif

					isobus::CANMessageFrame frame;
					while (hardwareChannels[i]->receivedMessagesQueue.peek(frame))
					{
						frame.channel = i;
						frameReceivedEventDispatcher.invoke(frame);
						receive_can_message_frame_from_hardware(frame);
						hardwareChannels[i]->receivedMessagesQueue.pop();
					}
				}
			}

			// Stage 2 - Update stack. That will fill up the transmit queues if needed
			if (SystemTiming::time_expired_ms(lastUpdateTimestamp, periodicUpdateInterval))
			{
				periodicUpdateEventDispatcher.invoke();
				periodic_update_from_hardware();
				lastUpdateTimestamp = SystemTiming::get_timestamp_ms();
			}

			// Stage 3 - Transmitting messages to hardware
			{
				LOCK_GUARD(Mutex, hardwareChannelsMutex);
				std::for_each(hardwareChannels.begin(), hardwareChannels.end(), [](const std::unique_ptr<CANHardware> &channel) {
					isobus::CANMessageFrame frame;
					while (channel->messagesToBeTransmittedQueue.peek(frame))
					{
						if (channel->transmit_can_frame(frame))
						{
							frameTransmittedEventDispatcher.invoke(frame);
							on_transmit_can_message_frame_from_hardware(frame);
							channel->messagesToBeTransmittedQueue.pop();
						}
						else
						{
							break;
						}
					}
				});
			}
		}
	}

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
	void CANHardwareInterface::update_thread_function()
	{
		std::unique_lock<std::mutex> hardwareLock(hardwareChannelsMutex);
		// Wait until everything is running
		hardwareLock.unlock();

		while (started)
		{
			std::unique_lock<std::mutex> threadLock(updateMutex);
			updateThreadWakeupCondition.wait_for(threadLock, std::chrono::milliseconds(periodicUpdateInterval)); // Update with at least the periodic interval
			update();
		}
	}

	void CANHardwareInterface::start_threads()
	{
		started = true;
		if (nullptr == updateThread)
		{
			updateThread.reset(new std::thread(update_thread_function));
		}
	}

	void CANHardwareInterface::stop_threads()
	{
		started = false;
		if (nullptr != updateThread)
		{
			if (updateThread->joinable())
			{
				updateThreadWakeupCondition.notify_all();
				updateThread->join();
			}
			updateThread = nullptr;
		}
	}
#endif
}
