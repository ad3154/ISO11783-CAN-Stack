//================================================================================================
/// @file can_network_manager.cpp
///
/// @brief The main class that manages the ISOBUS stack including: callbacks, Name to Address
/// management, making control functions, and driving the various protocols.
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
///
/// @copyright 2022 The Open-Agriculture Developers
//================================================================================================

#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/can_constants.hpp"
#include "isobus/isobus/can_general_parameter_group_numbers.hpp"
#include "isobus/isobus/can_hardware_abstraction.hpp"
#include "isobus/isobus/can_message.hpp"
#include "isobus/isobus/can_parameter_group_number_request_protocol.hpp"
#include "isobus/isobus/can_partnered_control_function.hpp"
#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/utility/system_timing.hpp"
#include "isobus/utility/to_string.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <numeric>

namespace isobus
{
	CANNetworkManager CANNetworkManager::CANNetwork;

	void CANNetworkManager::initialize()
	{
		// Clear queues
		while (!receivedMessageQueue.empty())
		{
			get_next_can_message_from_rx_queue();
		}
		while (!transmittedMessageQueue.empty())
		{
			get_next_can_message_from_tx_queue();
		}
		initialized = true;
	}

	std::shared_ptr<InternalControlFunction> CANNetworkManager::create_internal_control_function(NAME desiredName, std::uint8_t CANPort, std::uint8_t preferredAddress)
	{
		LOCK_GUARD(Mutex, internalControlFunctionsMutex);
		auto controlFunction = std::make_shared<InternalControlFunction>(desiredName, preferredAddress, CANPort);
		controlFunction->pgnRequestProtocol.reset(new ParameterGroupNumberRequestProtocol(controlFunction));
		internalControlFunctions.push_back(controlFunction);
		heartBeatInterfaces.at(CANPort)->on_new_internal_control_function(controlFunction);
		return controlFunction;
	}

	std::shared_ptr<PartneredControlFunction> CANNetworkManager::create_partnered_control_function(std::uint8_t CANPort, const std::vector<NAMEFilter> &NAMEFilters)
	{
		auto controlFunction = std::make_shared<PartneredControlFunction>(CANPort, NAMEFilters);
		partneredControlFunctions.push_back(controlFunction);
		return controlFunction;
	}

	void CANNetworkManager::deactivate_control_function(std::shared_ptr<InternalControlFunction> controlFunction)
	{
		// We need to unregister the control function from the interfaces managed by the network manager first.
		controlFunction->pgnRequestProtocol.reset();
		heartBeatInterfaces.at(controlFunction->get_can_port())->on_destroyed_internal_control_function(controlFunction);
		internalControlFunctions.erase(std::remove(internalControlFunctions.begin(), internalControlFunctions.end(), controlFunction), internalControlFunctions.end());
		deactivate_control_function(std::static_pointer_cast<ControlFunction>(controlFunction));
	}

	void CANNetworkManager::deactivate_control_function(std::shared_ptr<PartneredControlFunction> controlFunction)
	{
		partneredControlFunctions.erase(std::remove(partneredControlFunctions.begin(), partneredControlFunctions.end(), controlFunction), partneredControlFunctions.end());
		deactivate_control_function(std::static_pointer_cast<ControlFunction>(controlFunction));
	}

	void CANNetworkManager::add_global_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent)
	{
		globalParameterGroupNumberCallbacks.emplace_back(parameterGroupNumber, callback, parent, nullptr);
	}

	void CANNetworkManager::remove_global_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent)
	{
		ParameterGroupNumberCallbackData tempObject(parameterGroupNumber, callback, parent, nullptr);
		auto callbackLocation = std::find(globalParameterGroupNumberCallbacks.begin(), globalParameterGroupNumberCallbacks.end(), tempObject);
		if (globalParameterGroupNumberCallbacks.end() != callbackLocation)
		{
			globalParameterGroupNumberCallbacks.erase(callbackLocation);
		}
	}

	std::size_t CANNetworkManager::get_number_global_parameter_group_number_callbacks() const
	{
		return globalParameterGroupNumberCallbacks.size();
	}

	void CANNetworkManager::add_any_control_function_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent)
	{
		LOCK_GUARD(Mutex, anyControlFunctionCallbacksMutex);
		anyControlFunctionParameterGroupNumberCallbacks.emplace_back(parameterGroupNumber, callback, parent, nullptr);
	}

	void CANNetworkManager::remove_any_control_function_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parent)
	{
		ParameterGroupNumberCallbackData tempObject(parameterGroupNumber, callback, parent, nullptr);
		LOCK_GUARD(Mutex, anyControlFunctionCallbacksMutex);
		auto callbackLocation = std::find(anyControlFunctionParameterGroupNumberCallbacks.begin(), anyControlFunctionParameterGroupNumberCallbacks.end(), tempObject);
		if (anyControlFunctionParameterGroupNumberCallbacks.end() != callbackLocation)
		{
			anyControlFunctionParameterGroupNumberCallbacks.erase(callbackLocation);
		}
	}

	EventDispatcher<CANMessage> &CANNetworkManager::get_transmitted_message_event_dispatcher()
	{
		return messageTransmittedEventDispatcher;
	}

	std::shared_ptr<InternalControlFunction> CANNetworkManager::get_internal_control_function(std::shared_ptr<ControlFunction> controlFunction) const
	{
		std::shared_ptr<InternalControlFunction> retVal = nullptr;

		if ((nullptr != controlFunction) &&
		    (ControlFunction::Type::Internal == controlFunction->get_type()))
		{
			retVal = std::static_pointer_cast<InternalControlFunction>(controlFunction);
		}
		return retVal;
	}

	float CANNetworkManager::get_estimated_busload(std::uint8_t canChannel)
	{
		LOCK_GUARD(Mutex, busloadUpdateMutex);
		constexpr float ISOBUS_BAUD_RATE_BPS = 250000.0f;
		float retVal = 0.0f;

		if (canChannel < CAN_PORT_MAXIMUM)
		{
			float totalTimeInAccumulatorWindow = (busloadMessageBitsHistory.at(canChannel).size() * BUSLOAD_UPDATE_FREQUENCY_MS) / 1000.0f;
			std::uint32_t totalBitCount = std::accumulate(busloadMessageBitsHistory.at(canChannel).begin(), busloadMessageBitsHistory.at(canChannel).end(), 0);
			retVal = (0 != totalTimeInAccumulatorWindow) ? ((totalBitCount / (totalTimeInAccumulatorWindow * ISOBUS_BAUD_RATE_BPS)) * 100.0f) : 0.0f;
		}
		return retVal;
	}

	bool CANNetworkManager::send_can_message(std::uint32_t parameterGroupNumber,
	                                         const std::uint8_t *dataBuffer,
	                                         std::uint32_t dataLength,
	                                         std::shared_ptr<InternalControlFunction> sourceControlFunction,
	                                         std::shared_ptr<ControlFunction> destinationControlFunction,
	                                         CANIdentifier::CANPriority priority,
	                                         TransmitCompleteCallback transmitCompleteCallback,
	                                         void *parentPointer,
	                                         DataChunkCallback frameChunkCallback)
	{
		bool retVal = false;

		if (((nullptr != dataBuffer) ||
		     (nullptr != frameChunkCallback)) &&
		    (dataLength > 0) &&
		    (dataLength <= CANMessage::ABSOLUTE_MAX_MESSAGE_LENGTH) &&
		    (nullptr != sourceControlFunction) &&
		    ((parameterGroupNumber == static_cast<std::uint32_t>(CANLibParameterGroupNumber::AddressClaim)) ||
		     (sourceControlFunction->get_address_valid())))
		{
			std::unique_ptr<CANMessageData> messageData;
			if (nullptr != frameChunkCallback)
			{
				messageData.reset(new CANMessageDataCallback(dataLength, frameChunkCallback, parentPointer));
			}
			else
			{
				messageData.reset(new CANMessageDataView(dataBuffer, dataLength));
			}
			if (transportProtocols[sourceControlFunction->get_can_port()]->protocol_transmit_message(parameterGroupNumber,
			                                                                                         messageData,
			                                                                                         sourceControlFunction,
			                                                                                         destinationControlFunction,
			                                                                                         transmitCompleteCallback,
			                                                                                         parentPointer))
			{
				// Successfully sent via the transport protocol
				retVal = true;
			}
			else if (extendedTransportProtocols[sourceControlFunction->get_can_port()]->protocol_transmit_message(parameterGroupNumber,
			                                                                                                      messageData,
			                                                                                                      sourceControlFunction,
			                                                                                                      destinationControlFunction,
			                                                                                                      transmitCompleteCallback,
			                                                                                                      parentPointer))
			{
				// Successfully sent via the extended transport protocol
				retVal = true;
			}

			//! @todo Allow sending 8 byte message with the frameChunkCallback
			if ((!retVal) &&
			    (nullptr != dataBuffer))
			{
				if (nullptr == destinationControlFunction)
				{
					// Todo move binding of dest address to hardware layer
					retVal = send_can_message_raw(sourceControlFunction->get_can_port(), sourceControlFunction->get_address(), 0xFF, parameterGroupNumber, static_cast<std::uint8_t>(priority), dataBuffer, dataLength);
				}
				else if (destinationControlFunction->get_address_valid())
				{
					retVal = send_can_message_raw(sourceControlFunction->get_can_port(), sourceControlFunction->get_address(), destinationControlFunction->get_address(), parameterGroupNumber, static_cast<std::uint8_t>(priority), dataBuffer, dataLength);
				}

				if (retVal &&
				    (nullptr != transmitCompleteCallback))
				{
					// Message was not sent via a protocol, so handle the tx callback now
					transmitCompleteCallback(parameterGroupNumber, dataLength, sourceControlFunction, destinationControlFunction, retVal, parentPointer);
				}
			}
		}
		return retVal;
	}

	void CANNetworkManager::update()
	{
		auto &processingMutex = ControlFunction::controlFunctionProcessingMutex;
		LOCK_GUARD(Mutex, processingMutex);

		if (!initialized)
		{
			initialize();
		}

		update_new_partners();

		process_rx_messages();

		// Update ISOBUS heartbeats (should be done before process_tx_messages
		// to minimize latency in safety critical paths)
		for (std::uint32_t i = 0; i < CAN_PORT_MAXIMUM; i++)
		{
			heartBeatInterfaces.at(i)->update();
		}

		process_tx_messages();

		update_internal_cfs();

		prune_inactive_control_functions();

		// Update transport protocols
		for (std::uint32_t i = 0; i < CAN_PORT_MAXIMUM; i++)
		{
			transportProtocols[i]->update();
			extendedTransportProtocols[i]->update();
			fastPacketProtocol[i]->update();
		}
		update_busload_history();
		updateTimestamp_ms = SystemTiming::get_timestamp_ms();
	}

	bool CANNetworkManager::send_can_message_raw(std::uint32_t portIndex,
	                                             std::uint8_t sourceAddress,
	                                             std::uint8_t destAddress,
	                                             std::uint32_t parameterGroupNumber,
	                                             std::uint8_t priority,
	                                             const void *data,
	                                             std::uint32_t size,
	                                             CANLibBadge<InternalControlFunction>) const
	{
		return send_can_message_raw(portIndex, sourceAddress, destAddress, parameterGroupNumber, priority, data, size);
	}

	ParameterGroupNumberCallbackData CANNetworkManager::get_global_parameter_group_number_callback(std::size_t index) const
	{
		ParameterGroupNumberCallbackData retVal(0, nullptr, nullptr, nullptr);

		if (index < get_number_global_parameter_group_number_callbacks())
		{
			retVal = globalParameterGroupNumberCallbacks[index];
		}
		return retVal;
	}

	void receive_can_message_frame_from_hardware(const CANMessageFrame &rxFrame)
	{
		CANNetworkManager::CANNetwork.process_receive_can_message_frame(rxFrame);
	}

	void on_transmit_can_message_frame_from_hardware(const CANMessageFrame &txFrame)
	{
		CANNetworkManager::CANNetwork.process_transmitted_can_message_frame(txFrame);
	}

	void periodic_update_from_hardware()
	{
		CANNetworkManager::CANNetwork.update();
	}

	void CANNetworkManager::process_receive_can_message_frame(const CANMessageFrame &rxFrame)
	{
		update_control_functions(rxFrame);

		CANIdentifier identifier(rxFrame.identifier);
		CANMessage message(CANMessage::Type::Receive,
		                   identifier,
		                   rxFrame.data,
		                   rxFrame.dataLength,
		                   get_control_function(rxFrame.channel, identifier.get_source_address()),
		                   get_control_function(rxFrame.channel, identifier.get_destination_address()),
		                   rxFrame.channel);

		update_busload(rxFrame.channel, rxFrame.get_number_bits_in_message());

		if (initialized)
		{
			LOCK_GUARD(Mutex, receivedMessageQueueMutex);
			receivedMessageQueue.push(std::move(message));
		}
	}

	void CANNetworkManager::process_transmitted_can_message_frame(const CANMessageFrame &txFrame)
	{
		update_busload(txFrame.channel, txFrame.get_number_bits_in_message());

		CANIdentifier identifier(txFrame.identifier);
		CANMessage message(CANMessage::Type::Transmit,
		                   identifier,
		                   txFrame.data,
		                   txFrame.dataLength,
		                   get_control_function(txFrame.channel, identifier.get_source_address()),
		                   get_control_function(txFrame.channel, identifier.get_destination_address()),
		                   txFrame.channel);

		if (initialized)
		{
			LOCK_GUARD(Mutex, transmittedMessageQueueMutex);
			transmittedMessageQueue.push(std::move(message));
		}
	}

	void CANNetworkManager::deactivate_control_function(std::shared_ptr<ControlFunction> controlFunction)
	{
		auto result = std::find(inactiveControlFunctions.begin(), inactiveControlFunctions.end(), controlFunction);
		if (result != inactiveControlFunctions.end())
		{
			inactiveControlFunctions.erase(result);
		}

		for (std::uint8_t i = 0; i < NULL_CAN_ADDRESS; i++)
		{
			if (controlFunctionTable[controlFunction->get_can_port()][i] == controlFunction)
			{
				if (i != controlFunction->get_address())
				{
					LOG_WARNING("[NM]: %s control function with address '%d' was at incorrect address '%d' in the lookup table prior to deactivation.",
					            controlFunction->get_type_string().c_str(),
					            controlFunction->get_address(),
					            i);
				}

				controlFunctionTable[controlFunction->get_can_port()][i] = nullptr;

				if (controlFunction->get_address_valid() && (ControlFunction::Type::Partnered == controlFunction->get_type()))
				{
					// The control function was an active partner when deleted, so we replace it with an new external control function instead
					create_external_control_function(controlFunction->get_NAME(), controlFunction->get_address(), controlFunction->get_can_port());
				}
			}
		}
		LOG_DEBUG("[NM]: %s control function at address '%d' is deactivated.",
		          controlFunction->get_type_string().c_str(),
		          controlFunction->get_address());
	}

	void CANNetworkManager::add_control_function_status_change_callback(ControlFunctionStateCallback callback)
	{
		if (nullptr != callback)
		{
			LOCK_GUARD(Mutex, controlFunctionStatusCallbacksMutex);
			controlFunctionStateCallbacks.emplace_back(callback);
		}
	}

	void CANNetworkManager::remove_control_function_status_change_callback(ControlFunctionStateCallback callback)
	{
		if (nullptr != callback)
		{
			LOCK_GUARD(Mutex, controlFunctionStatusCallbacksMutex);
			ControlFunctionStateCallback targetCallback(callback);
			auto callbackLocation = std::find(controlFunctionStateCallbacks.begin(), controlFunctionStateCallbacks.end(), targetCallback);

			if (controlFunctionStateCallbacks.end() != callbackLocation)
			{
				controlFunctionStateCallbacks.erase(callbackLocation);
			}
		}
	}

	const std::list<std::shared_ptr<InternalControlFunction>> &CANNetworkManager::get_internal_control_functions() const
	{
		return internalControlFunctions;
	}

	const std::list<std::shared_ptr<PartneredControlFunction>> &CANNetworkManager::get_partnered_control_functions() const
	{
		return partneredControlFunctions;
	}

	std::list<std::shared_ptr<ControlFunction>> isobus::CANNetworkManager::get_control_functions(bool includingOffline) const
	{
		std::list<std::shared_ptr<ControlFunction>> retVal;

		for (std::uint8_t channelIndex = 0; channelIndex < CAN_PORT_MAXIMUM; channelIndex++)
		{
			for (std::uint8_t address = 0; address < NULL_CAN_ADDRESS; address++)
			{
				if (nullptr != controlFunctionTable[channelIndex][address])
				{
					retVal.push_back(controlFunctionTable[channelIndex][address]);
				}
			}
		}

		if (includingOffline)
		{
			retVal.insert(retVal.end(), inactiveControlFunctions.begin(), inactiveControlFunctions.end());
		}

		return retVal;
	}

	std::list<std::shared_ptr<TransportProtocolSessionBase>> isobus::CANNetworkManager::get_active_transport_protocol_sessions(std::uint8_t canPortIndex) const
	{
		std::list<std::shared_ptr<TransportProtocolSessionBase>> retVal;
		retVal.insert(retVal.end(), transportProtocols[canPortIndex]->get_sessions().begin(), transportProtocols[canPortIndex]->get_sessions().end());
		retVal.insert(retVal.end(), extendedTransportProtocols[canPortIndex]->get_sessions().begin(), extendedTransportProtocols[canPortIndex]->get_sessions().end());
		return retVal;
	}

	std::unique_ptr<FastPacketProtocol> &CANNetworkManager::get_fast_packet_protocol(std::uint8_t canPortIndex)
	{
		return fastPacketProtocol[canPortIndex];
	}

	HeartbeatInterface &CANNetworkManager::get_heartbeat_interface(std::uint8_t canPortIndex)
	{
		assert(canPortIndex < CAN_PORT_MAXIMUM); // You passed in an out of range index!
		return *heartBeatInterfaces.at(canPortIndex);
	}

	CANNetworkConfiguration &CANNetworkManager::get_configuration()
	{
		return configuration;
	}

	EventDispatcher<std::shared_ptr<InternalControlFunction>> &CANNetworkManager::get_address_violation_event_dispatcher()
	{
		return addressViolationEventDispatcher;
	}

	bool CANNetworkManager::add_protocol_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parentPointer)
	{
		bool retVal = false;
		ParameterGroupNumberCallbackData callbackInfo(parameterGroupNumber, callback, parentPointer, nullptr);
		LOCK_GUARD(Mutex, protocolPGNCallbacksMutex);
		if ((nullptr != callback) && (protocolPGNCallbacks.end() == find(protocolPGNCallbacks.begin(), protocolPGNCallbacks.end(), callbackInfo)))
		{
			protocolPGNCallbacks.push_back(callbackInfo);
			retVal = true;
		}
		return retVal;
	}

	bool CANNetworkManager::remove_protocol_parameter_group_number_callback(std::uint32_t parameterGroupNumber, CANLibCallback callback, void *parentPointer)
	{
		bool retVal = false;
		ParameterGroupNumberCallbackData callbackInfo(parameterGroupNumber, callback, parentPointer, nullptr);
		LOCK_GUARD(Mutex, protocolPGNCallbacksMutex);
		if (nullptr != callback)
		{
			std::list<ParameterGroupNumberCallbackData>::iterator callbackLocation;
			callbackLocation = find(protocolPGNCallbacks.begin(), protocolPGNCallbacks.end(), callbackInfo);

			if (protocolPGNCallbacks.end() != callbackLocation)
			{
				protocolPGNCallbacks.erase(callbackLocation);
				retVal = true;
			}
		}
		return retVal;
	}

	CANNetworkManager::CANNetworkManager()
	{
		currentBusloadBitAccumulator.fill(0);
		lastAddressClaimRequestTimestamp_ms.fill(0);
		controlFunctionTable.fill({ nullptr });

		auto send_frame_callback = [this](std::uint32_t parameterGroupNumber,
		                                  CANDataSpan data,
		                                  std::shared_ptr<InternalControlFunction> sourceControlFunction,
		                                  std::shared_ptr<ControlFunction> destinationControlFunction,
		                                  CANIdentifier::CANPriority priority) { return this->send_can_message(parameterGroupNumber, data.begin(), data.size(), sourceControlFunction, destinationControlFunction, priority); };

		for (std::uint8_t i = 0; i < CAN_PORT_MAXIMUM; i++)
		{
			auto receive_message_callback = [this, i](const CANMessage &message) {
				// TODO: hack port_index for now, once network manager isn't a singleton, this can be removed
				CANMessage tempMessage(CANMessage::Type::Receive,
				                       message.get_identifier(),
				                       message.get_data(),
				                       message.get_source_control_function(),
				                       message.get_destination_control_function(),
				                       i);
				this->protocol_message_callback(message);
			};
			transportProtocols.at(i).reset(new TransportProtocolManager(send_frame_callback, receive_message_callback, &configuration));
			extendedTransportProtocols.at(i).reset(new ExtendedTransportProtocolManager(send_frame_callback, receive_message_callback, &configuration));
			fastPacketProtocol.at(i).reset(new FastPacketProtocol(send_frame_callback));
			heartBeatInterfaces.at(i).reset(new HeartbeatInterface(send_frame_callback));
		}
	}

	std::shared_ptr<ControlFunction> CANNetworkManager::create_external_control_function(NAME desiredName, std::uint8_t address, std::uint8_t CANPort)
	{
		auto controlFunction = std::make_shared<ControlFunction>(desiredName, address, CANPort, ControlFunction::Type::External);
		if ((CANPort < CAN_PORT_MAXIMUM) && (address < NULL_CAN_ADDRESS))
		{
			controlFunctionTable[CANPort][address] = controlFunction;
		}
		return controlFunction;
	}

	void CANNetworkManager::update_address_table(const CANMessage &message)
	{
		std::uint8_t channelIndex = message.get_can_port_index();

		if ((static_cast<std::uint32_t>(CANLibParameterGroupNumber::AddressClaim) == message.get_identifier().get_parameter_group_number()) &&
		    (channelIndex < CAN_PORT_MAXIMUM))
		{
			std::uint8_t claimedAddress = message.get_identifier().get_source_address();
			auto targetControlFunction = controlFunctionTable[channelIndex][claimedAddress];
			if ((nullptr != targetControlFunction) &&
			    (CANIdentifier::NULL_ADDRESS == targetControlFunction->get_address()))
			{
				// Someone is at that spot in the table, but their address was stolen
				// Need to evict them from the table and move them to the inactive list
				targetControlFunction->address = NULL_CAN_ADDRESS;
				inactiveControlFunctions.push_back(targetControlFunction);
				LOG_INFO("[NM]: %s CF '%016llx' is evicted from address '%d' on channel '%d', as their address is probably stolen.",
				         targetControlFunction->get_type_string().c_str(),
				         targetControlFunction->get_NAME().get_full_name(),
				         claimedAddress,
				         channelIndex);
				targetControlFunction = nullptr;
			}

			if (targetControlFunction != nullptr)
			{
				targetControlFunction->claimedAddressSinceLastAddressClaimRequest = true;
			}
			else
			{
				// Look through all inactive CFs, maybe one of them has freshly claimed the address
				for (auto currentControlFunction : inactiveControlFunctions)
				{
					if ((currentControlFunction->get_address() == claimedAddress) &&
					    (currentControlFunction->get_can_port() == channelIndex))
					{
						controlFunctionTable[channelIndex][claimedAddress] = currentControlFunction;
						LOG_DEBUG("[NM]: %s CF '%016llx' is now active at address '%d' on channel '%d'.",
						          currentControlFunction->get_type_string().c_str(),
						          currentControlFunction->get_NAME().get_full_name(),
						          claimedAddress,
						          channelIndex);
						process_control_function_state_change_callback(currentControlFunction, ControlFunctionState::Online);
						break;
					}
				}
			}
		}
		else if ((static_cast<std::uint32_t>(CANLibParameterGroupNumber::ParameterGroupNumberRequest) == message.get_identifier().get_parameter_group_number()) &&
		         (channelIndex < CAN_PORT_MAXIMUM))
		{
			auto requestedPGN = message.get_uint24_at(0);

			if (static_cast<std::uint32_t>(CANLibParameterGroupNumber::AddressClaim) == requestedPGN)
			{
				lastAddressClaimRequestTimestamp_ms.at(channelIndex) = SystemTiming::get_timestamp_ms();

				// Reset the claimedAddressSinceLastAddressClaimRequest flag for all control functions on the port
				auto result = std::find_if(inactiveControlFunctions.begin(), inactiveControlFunctions.end(), [channelIndex](std::shared_ptr<ControlFunction> controlFunction) {
					return (channelIndex == controlFunction->get_can_port());
				});
				if (result != inactiveControlFunctions.end())
				{
					(*result)->claimedAddressSinceLastAddressClaimRequest = true;
				}
				std::for_each(controlFunctionTable[channelIndex].begin(), controlFunctionTable[channelIndex].end(), [](std::shared_ptr<ControlFunction> controlFunction) {
					if (nullptr != controlFunction)
					{
						controlFunction->claimedAddressSinceLastAddressClaimRequest = false;
					}
				});
			}
		}
	}

	void CANNetworkManager::update_internal_cfs()
	{
		LOCK_GUARD(Mutex, internalControlFunctionsMutex);
		for (const auto &currentInternalControlFunction : internalControlFunctions)
		{
			if (currentInternalControlFunction->update_address_claiming())
			{
				std::uint8_t channelIndex = currentInternalControlFunction->get_can_port();
				std::uint8_t claimedAddress = currentInternalControlFunction->get_address();

				// Check if the internal control function switched addresses, and therefore needs to be moved in the table
				for (std::uint8_t address = 0; address < NULL_CAN_ADDRESS; address++)
				{
					if (controlFunctionTable[channelIndex][address] == currentInternalControlFunction)
					{
						controlFunctionTable[channelIndex][address] = nullptr;
						break;
					}
				}

				if (nullptr != controlFunctionTable[channelIndex][claimedAddress])
				{
					// Someone is at that spot in the table, but their address was stolen by an internal control function
					// Need to evict them from the table
					controlFunctionTable[channelIndex][claimedAddress]->address = NULL_CAN_ADDRESS;
					controlFunctionTable[channelIndex][claimedAddress] = nullptr;
				}

				// ECU has claimed since the last update, add it to the table
				controlFunctionTable[channelIndex][claimedAddress] = currentInternalControlFunction;
			}
		}
	}

	void CANNetworkManager::process_rx_message_for_address_claiming(const CANMessage &message) const
	{
		for (const auto &internalCF : internalControlFunctions)
		{
			internalCF->process_rx_message_for_address_claiming(message);
		}
	}

	void CANNetworkManager::update_busload(std::uint8_t channelIndex, std::uint32_t numberOfBitsProcessed)
	{
		LOCK_GUARD(Mutex, busloadUpdateMutex);
		currentBusloadBitAccumulator.at(channelIndex) += numberOfBitsProcessed;
	}

	void CANNetworkManager::update_busload_history()
	{
		LOCK_GUARD(Mutex, busloadUpdateMutex);
		if (SystemTiming::time_expired_ms(busloadUpdateTimestamp_ms, BUSLOAD_UPDATE_FREQUENCY_MS))
		{
			for (std::size_t i = 0; i < busloadMessageBitsHistory.size(); i++)
			{
				busloadMessageBitsHistory.at(i).push_back(currentBusloadBitAccumulator.at(i));

				while (busloadMessageBitsHistory.at(i).size() > (BUSLOAD_SAMPLE_WINDOW_MS / BUSLOAD_UPDATE_FREQUENCY_MS))
				{
					busloadMessageBitsHistory.at(i).pop_front();
				}
				currentBusloadBitAccumulator.at(i) = 0;
			}
			busloadUpdateTimestamp_ms = SystemTiming::get_timestamp_ms();
		}
	}

	void CANNetworkManager::update_control_functions(const CANMessageFrame &rxFrame)
	{
		if ((static_cast<std::uint32_t>(CANLibParameterGroupNumber::AddressClaim) == CANIdentifier(rxFrame.identifier).get_parameter_group_number()) &&
		    (CAN_DATA_LENGTH == rxFrame.dataLength) &&
		    (rxFrame.channel < CAN_PORT_MAXIMUM))
		{
			std::uint64_t claimedNAME;
			std::shared_ptr<ControlFunction> foundControlFunction = nullptr;
			uint8_t claimedAddress = CANIdentifier(rxFrame.identifier).get_source_address();

			claimedNAME = rxFrame.data[0];
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[1]) << 8);
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[2]) << 16);
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[3]) << 24);
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[4]) << 32);
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[5]) << 40);
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[6]) << 48);
			claimedNAME |= (static_cast<std::uint64_t>(rxFrame.data[7]) << 56);

			// Check if the claimed NAME is someone we already know about
			auto activeResult = std::find_if(controlFunctionTable[rxFrame.channel].begin(),
			                                 controlFunctionTable[rxFrame.channel].end(),
			                                 [claimedNAME](const std::shared_ptr<ControlFunction> &cf) {
				                                 return (nullptr != cf) && (cf->get_NAME().get_full_name() == claimedNAME);
			                                 });
			if (activeResult != controlFunctionTable[rxFrame.channel].end())
			{
				foundControlFunction = *activeResult;
			}
			else
			{
				auto inActiveResult = std::find_if(inactiveControlFunctions.begin(),
				                                   inactiveControlFunctions.end(),
				                                   [claimedNAME, &rxFrame](const std::shared_ptr<ControlFunction> &cf) {
					                                   return (cf->get_NAME().get_full_name() == claimedNAME) && (cf->get_can_port() == rxFrame.channel);
				                                   });
				if (inActiveResult != inactiveControlFunctions.end())
				{
					foundControlFunction = *inActiveResult;
				}
			}

			if (nullptr == foundControlFunction)
			{
				// If we still haven't found it, it might be a partner. Check the list of partners.
				for (const auto &partner : partneredControlFunctions)
				{
					if ((partner->get_can_port() == rxFrame.channel) &&
					    (partner->check_matches_name(NAME(claimedNAME))) &&
					    (0 == partner->get_NAME().get_full_name()))
					{
						partner->controlFunctionNAME = NAME(claimedNAME);
						foundControlFunction = partner;
						controlFunctionTable[rxFrame.channel][claimedAddress] = foundControlFunction;
						break;
					}
				}
			}

			// Remove any CF that has the same address as the one claiming
			std::for_each(controlFunctionTable[rxFrame.channel].begin(),
			              controlFunctionTable[rxFrame.channel].end(),
			              [&foundControlFunction, &claimedAddress](const std::shared_ptr<ControlFunction> &cf) {
				              if ((nullptr != cf) && (foundControlFunction != cf) && (cf->get_address() == claimedAddress))
					              cf->address = CANIdentifier::NULL_ADDRESS;
			              });

			std::for_each(inactiveControlFunctions.begin(),
			              inactiveControlFunctions.end(),
			              [&rxFrame, &foundControlFunction, &claimedAddress](const std::shared_ptr<ControlFunction> &cf) {
				              if ((foundControlFunction != cf) && (cf->get_address() == claimedAddress) && (cf->get_can_port() == rxFrame.channel))
					              cf->address = CANIdentifier::NULL_ADDRESS;
			              });

			if (nullptr == foundControlFunction)
			{
				// New device, need to start keeping track of it
				foundControlFunction = create_external_control_function(NAME(claimedNAME), claimedAddress, rxFrame.channel);
				LOG_DEBUG("[NM]: A control function claimed address %u on channel %u", foundControlFunction->get_address(), foundControlFunction->get_can_port());
			}
			else if (foundControlFunction->get_address() != claimedAddress)
			{
				if (foundControlFunction->get_address_valid())
				{
					controlFunctionTable[rxFrame.channel][claimedAddress] = foundControlFunction;
					controlFunctionTable[rxFrame.channel][foundControlFunction->get_address()] = nullptr;
					LOG_INFO("[NM]: The %s control function at address %d changed it's address to %d on channel %u.",
					         foundControlFunction->get_type_string().c_str(),
					         foundControlFunction->get_address(),
					         claimedAddress,
					         foundControlFunction->get_can_port());
				}
				else
				{
					LOG_INFO("[NM]: %s control function with name %016llx has claimed address %u on channel %u.",
					         foundControlFunction->get_type_string().c_str(),
					         foundControlFunction->get_NAME().get_full_name(),
					         claimedAddress,
					         foundControlFunction->get_can_port());
					process_control_function_state_change_callback(foundControlFunction, ControlFunctionState::Online);
				}
				foundControlFunction->address = claimedAddress;
			}
		}
	}

	void CANNetworkManager::update_new_partners()
	{
		for (const auto &partner : partneredControlFunctions)
		{
			if (!partner->initialized)
			{
				// Remove any inactive CF that matches the partner's name
				for (auto currentInactiveControlFunction = inactiveControlFunctions.begin(); currentInactiveControlFunction != inactiveControlFunctions.end(); currentInactiveControlFunction++)
				{
					if ((partner->check_matches_name((*currentInactiveControlFunction)->get_NAME())) &&
					    (partner->get_can_port() == (*currentInactiveControlFunction)->get_can_port()) &&
					    (ControlFunction::Type::External == (*currentInactiveControlFunction)->get_type()))
					{
						inactiveControlFunctions.erase(currentInactiveControlFunction);
						break;
					}
				}

				for (const auto &currentActiveControlFunction : controlFunctionTable[partner->get_can_port()])
				{
					if ((nullptr != currentActiveControlFunction) &&
					    (partner->check_matches_name(currentActiveControlFunction->get_NAME())) &&
					    (ControlFunction::Type::External == currentActiveControlFunction->get_type()))
					{
						// This CF matches the filter and is not an internal or already partnered CF
						// Populate the partner's data
						partner->address = currentActiveControlFunction->get_address();
						partner->controlFunctionNAME = currentActiveControlFunction->get_NAME();
						partner->initialized = true;
						controlFunctionTable[partner->get_can_port()][partner->address] = std::shared_ptr<ControlFunction>(partner);
						process_control_function_state_change_callback(partner, ControlFunctionState::Online);

						LOG_INFO("[NM]: A partner with name %016llx has claimed address %u on channel %u.",
						         partner->get_NAME().get_full_name(),
						         partner->get_address(),
						         partner->get_can_port());
						break;
					}
				}
				partner->initialized = true;
			}
		}
	}

	CANMessageFrame CANNetworkManager::construct_frame(std::uint32_t portIndex,
	                                                   std::uint8_t sourceAddress,
	                                                   std::uint8_t destAddress,
	                                                   std::uint32_t parameterGroupNumber,
	                                                   std::uint8_t priority,
	                                                   const void *data,
	                                                   std::uint32_t size) const
	{
		CANMessageFrame txFrame;
		txFrame.identifier = DEFAULT_IDENTIFIER;

		if ((NULL_CAN_ADDRESS != destAddress) && (priority <= static_cast<std::uint8_t>(CANIdentifier::CANPriority::PriorityLowest7)) && (size <= CAN_DATA_LENGTH) && (nullptr != data))
		{
			std::uint32_t identifier = 0;

			// Manually encode an identifier
			identifier |= ((priority & 0x07) << 26);
			identifier |= (sourceAddress & 0xFF);

			if (BROADCAST_CAN_ADDRESS == destAddress)
			{
				if ((parameterGroupNumber & 0xF000) >= 0xF000)
				{
					identifier |= ((parameterGroupNumber & 0x3FFFF) << 8);
				}
				else
				{
					identifier |= (destAddress << 8);
					identifier |= ((parameterGroupNumber & 0x3FF00) << 8);
				}
			}
			else
			{
				if ((parameterGroupNumber & 0xF000) < 0xF000)
				{
					identifier |= (destAddress << 8);
					identifier |= ((parameterGroupNumber & 0x3FF00) << 8);
				}
				else
				{
					LOG_WARNING("[NM]: Cannot send a message with PGN " +
					            isobus::to_string(static_cast<int>(parameterGroupNumber)) +
					            " as a destination specific message. " +
					            "Try resending it using nullptr as your destination control function.");
					identifier = DEFAULT_IDENTIFIER;
				}
			}

			if (DEFAULT_IDENTIFIER != identifier)
			{
				txFrame.channel = portIndex;
				memcpy(reinterpret_cast<void *>(txFrame.data), data, size);
				txFrame.dataLength = size;
				txFrame.isExtendedFrame = true;
				txFrame.identifier = identifier & 0x1FFFFFFF;
			}
		}
		return txFrame;
	}

	std::shared_ptr<ControlFunction> CANNetworkManager::get_control_function(std::uint8_t channelIndex, std::uint8_t address) const
	{
		std::shared_ptr<ControlFunction> retVal = nullptr;

		if ((address < NULL_CAN_ADDRESS) && (channelIndex < CAN_PORT_MAXIMUM))
		{
			retVal = controlFunctionTable[channelIndex][address];
		}
		return retVal;
	}

	CANMessage CANNetworkManager::get_next_can_message_from_rx_queue()
	{
		LOCK_GUARD(Mutex, receivedMessageQueueMutex);
		if (!receivedMessageQueue.empty())
		{
			CANMessage retVal = std::move(receivedMessageQueue.front());
			receivedMessageQueue.pop();
			return retVal;
		}
		return CANMessage::create_invalid_message();
	}

	CANMessage CANNetworkManager::get_next_can_message_from_tx_queue()
	{
		LOCK_GUARD(Mutex, transmittedMessageQueueMutex);
		if (!transmittedMessageQueue.empty())
		{
			CANMessage retVal = std::move(transmittedMessageQueue.front());
			transmittedMessageQueue.pop();
			return retVal;
		}
		return CANMessage::create_invalid_message();
	}

	void CANNetworkManager::process_any_control_function_pgn_callbacks(const CANMessage &currentMessage)
	{
		LOCK_GUARD(Mutex, anyControlFunctionCallbacksMutex);
		for (const auto &currentCallback : anyControlFunctionParameterGroupNumberCallbacks)
		{
			if ((currentCallback.get_parameter_group_number() == currentMessage.get_identifier().get_parameter_group_number()) &&
			    ((nullptr == currentMessage.get_destination_control_function()) ||
			     (ControlFunction::Type::Internal == currentMessage.get_destination_control_function()->get_type())))
			{
				currentCallback.get_callback()(currentMessage, currentCallback.get_parent());
			}
		}
	}

	void CANNetworkManager::process_can_message_for_address_violations(const CANMessage &currentMessage)
	{
		for (const auto &internalCF : internalControlFunctions)
		{
			if ((nullptr != internalCF) &&
			    internalCF->process_rx_message_for_address_violation(currentMessage))
			{
				addressViolationEventDispatcher.call(internalCF);
			}
		}
	}

	void CANNetworkManager::process_control_function_state_change_callback(std::shared_ptr<ControlFunction> controlFunction, ControlFunctionState state)
	{
		LOCK_GUARD(Mutex, controlFunctionStatusCallbacksMutex);
		for (const auto &callback : controlFunctionStateCallbacks)
		{
			callback(controlFunction, state);
		}
	}

	void CANNetworkManager::process_protocol_pgn_callbacks(const CANMessage &currentMessage)
	{
		LOCK_GUARD(Mutex, protocolPGNCallbacksMutex);
		for (const auto &currentCallback : protocolPGNCallbacks)
		{
			if (currentCallback.get_parameter_group_number() == currentMessage.get_identifier().get_parameter_group_number())
			{
				currentCallback.get_callback()(currentMessage, currentCallback.get_parent());
			}
		}
	}

	void CANNetworkManager::process_can_message_for_global_and_partner_callbacks(const CANMessage &message) const
	{
		std::shared_ptr<ControlFunction> messageDestination = message.get_destination_control_function();
		if ((nullptr == messageDestination) &&
		    ((nullptr != message.get_source_control_function()) ||
		     ((static_cast<std::uint32_t>(CANLibParameterGroupNumber::ParameterGroupNumberRequest) == message.get_identifier().get_parameter_group_number()) &&
		      (NULL_CAN_ADDRESS == message.get_identifier().get_source_address()))))
		{
			// Message destined to global
			for (std::size_t i = 0; i < get_number_global_parameter_group_number_callbacks(); i++)
			{
				if ((message.get_identifier().get_parameter_group_number() == get_global_parameter_group_number_callback(i).get_parameter_group_number()) &&
				    (nullptr != get_global_parameter_group_number_callback(i).get_callback()))
				{
					// We have a callback that matches this PGN
					get_global_parameter_group_number_callback(i).get_callback()(message, get_global_parameter_group_number_callback(i).get_parent());
				}
			}
		}
		else if ((messageDestination != nullptr) && (messageDestination->get_type() == ControlFunction::Type::Internal))
		{
			// Message is destined to us
			for (const auto &partner : partneredControlFunctions)
			{
				if ((nullptr != partner) &&
				    (partner->get_can_port() == message.get_can_port_index()))
				{
					// Message matches CAN port for a partnered control function
					for (std::size_t k = 0; k < partner->get_number_parameter_group_number_callbacks(); k++)
					{
						if ((message.get_identifier().get_parameter_group_number() == partner->get_parameter_group_number_callback(k).get_parameter_group_number()) &&
						    (nullptr != partner->get_parameter_group_number_callback(k).get_callback()) &&
						    ((nullptr == partner->get_parameter_group_number_callback(k).get_internal_control_function()) ||
						     (partner->get_parameter_group_number_callback(k).get_internal_control_function()->get_address() == message.get_identifier().get_destination_address())))
						{
							// We have a callback matching this message
							partner->get_parameter_group_number_callback(k).get_callback()(message, partner->get_parameter_group_number_callback(k).get_parent());
						}
					}
				}
			}
		}
	}

	void CANNetworkManager::process_rx_messages()
	{
		// We may miss a message without locking the mutex when checking if empty, but that's okay. It will be picked up on the next iteration
		while (!receivedMessageQueue.empty())
		{
			CANMessage currentMessage = get_next_can_message_from_rx_queue();

			update_address_table(currentMessage);
			process_can_message_for_address_violations(currentMessage);
			process_rx_message_for_address_claiming(currentMessage);

			// Update Special Callbacks, like protocols and non-cf specific ones
			transportProtocols.at(currentMessage.get_can_port_index())->process_message(currentMessage);
			extendedTransportProtocols.at(currentMessage.get_can_port_index())->process_message(currentMessage);
			fastPacketProtocol.at(currentMessage.get_can_port_index())->process_message(currentMessage);
			heartBeatInterfaces.at(currentMessage.get_can_port_index())->process_rx_message(currentMessage);
			process_protocol_pgn_callbacks(currentMessage);
			process_any_control_function_pgn_callbacks(currentMessage);

			// Update Others
			process_can_message_for_global_and_partner_callbacks(currentMessage);
		}
	}

	void CANNetworkManager::process_tx_messages()
	{
		// We may miss a message without locking the mutex when checking if empty, but that's okay. It will be picked up on the next iteration
		while (!transmittedMessageQueue.empty())
		{
			CANMessage currentMessage = get_next_can_message_from_tx_queue();

			// Update listen-only callbacks
			messageTransmittedEventDispatcher.call(currentMessage);
		}
	}

	void CANNetworkManager::prune_inactive_control_functions()
	{
		for (std::uint_fast8_t channelIndex = 0; channelIndex < CAN_PORT_MAXIMUM; channelIndex++)
		{
			constexpr std::uint32_t MAX_ADDRESS_CLAIM_RESOLUTION_TIME = 755; // This is 250ms + RTxD + 250ms
			if ((0 != lastAddressClaimRequestTimestamp_ms.at(channelIndex)) &&
			    (SystemTiming::time_expired_ms(lastAddressClaimRequestTimestamp_ms.at(channelIndex), MAX_ADDRESS_CLAIM_RESOLUTION_TIME)))
			{
				for (std::uint_fast8_t i = 0; i < NULL_CAN_ADDRESS; i++)
				{
					auto controlFunction = controlFunctionTable[channelIndex][i];
					if ((nullptr != controlFunction) &&
					    (!controlFunction->claimedAddressSinceLastAddressClaimRequest) &&
					    (ControlFunction::Type::Internal != controlFunction->get_type()))
					{
						inactiveControlFunctions.push_back(controlFunction);
						LOG_INFO("[NM]: Control function with address %u and NAME %016llx is now offline on channel %u.", controlFunction->get_address(), controlFunction->get_NAME(), channelIndex);
						controlFunctionTable[channelIndex][i] = nullptr;
						controlFunction->address = NULL_CAN_ADDRESS;
						process_control_function_state_change_callback(controlFunction, ControlFunctionState::Offline);
					}
					else if ((nullptr != controlFunction) &&
					         (!controlFunction->claimedAddressSinceLastAddressClaimRequest))
					{
						process_control_function_state_change_callback(controlFunction, ControlFunctionState::Offline);
					}
				}
				lastAddressClaimRequestTimestamp_ms.at(channelIndex) = 0;
			}
		}
	}

	bool CANNetworkManager::send_can_message_raw(std::uint32_t portIndex, std::uint8_t sourceAddress, std::uint8_t destAddress, std::uint32_t parameterGroupNumber, std::uint8_t priority, const void *data, std::uint32_t size) const
	{
		CANMessageFrame tempFrame = construct_frame(portIndex, sourceAddress, destAddress, parameterGroupNumber, priority, data, size);
		bool retVal = false;

		if ((DEFAULT_IDENTIFIER != tempFrame.identifier) &&
		    (portIndex < CAN_PORT_MAXIMUM))
		{
			retVal = send_can_message_frame_to_hardware(tempFrame);
		}
		return retVal;
	}

	void CANNetworkManager::protocol_message_callback(const CANMessage &message)
	{
		process_can_message_for_global_and_partner_callbacks(message);
		process_any_control_function_pgn_callbacks(message);
		process_rx_message_for_address_claiming(message);
	}

} // namespace isobus
