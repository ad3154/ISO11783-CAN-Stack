//================================================================================================
/// @file isobus_task_controller_client.cpp
///
/// @brief A class to manage a client connection to a ISOBUS field computer's task controller
/// @author Adrian Del Grosso
///
/// @copyright 2023 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_task_controller_client.hpp"

#include "isobus/isobus/can_general_parameter_group_numbers.hpp"
#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/isobus/isobus_standard_data_description_indices.hpp"
#include "isobus/utility/system_timing.hpp"
#include "isobus/utility/to_string.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
#include <thread>
#endif

namespace isobus
{
	TaskControllerClient::TaskControllerClient(std::shared_ptr<PartneredControlFunction> partner, std::shared_ptr<InternalControlFunction> clientSource, std::shared_ptr<PartneredControlFunction> primaryVT) :
	  languageCommandInterface(clientSource, partner),
	  partnerControlFunction(partner),
	  myControlFunction(clientSource),
	  primaryVirtualTerminal(primaryVT)
	{
	}

	TaskControllerClient::~TaskControllerClient()
	{
		terminate();
	}

	void TaskControllerClient::initialize(bool spawnThread)
	{
		// You cannot use this interface without having valid control functions.
		assert(nullptr != myControlFunction);
		assert(nullptr != partnerControlFunction);

		partnerControlFunction->add_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData), process_rx_message, this);
		partnerControlFunction->add_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::Acknowledge), process_rx_message, this);
		CANNetworkManager::CANNetwork.add_global_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData), process_rx_message, this);

		if (!languageCommandInterface.get_initialized())
		{
			languageCommandInterface.initialize();
		}

		if (shouldTerminate)
		{
			shouldTerminate = false;
			initialized = false;
		}

		if (!initialized)
		{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
			if (spawnThread)
			{
				workerThread = new std::thread([this]() { worker_thread_function(); });
			}
#endif
			initialized = true;
		}
	}

	void TaskControllerClient::add_default_process_data_requested_callback(DefaultProcessDataRequestedCallback callback, void *parentPointer)
	{
		LOCK_GUARD(Mutex, clientMutex);

		DefaultProcessDataRequestCallbackInfo callbackData = { callback, parentPointer };
		defaultProcessDataRequestedCallbacks.push_back(callbackData);
	}

	void TaskControllerClient::add_request_value_callback(RequestValueCommandCallback callback, void *parentPointer)
	{
		LOCK_GUARD(Mutex, clientMutex);

		RequestValueCommandCallbackInfo callbackData = { callback, parentPointer };
		requestValueCallbacks.push_back(callbackData);
	}

	void TaskControllerClient::add_value_command_callback(ValueCommandCallback callback, void *parentPointer)
	{
		LOCK_GUARD(Mutex, clientMutex);

		ValueCommandCallbackInfo callbackData = { callback, parentPointer };
		valueCommandsCallbacks.push_back(callbackData);
	}

	void TaskControllerClient::remove_default_process_data_requested_callback(DefaultProcessDataRequestedCallback callback, void *parentPointer)
	{
		LOCK_GUARD(Mutex, clientMutex);

		DefaultProcessDataRequestCallbackInfo callbackData = { callback, parentPointer };
		auto callbackLocation = std::find(defaultProcessDataRequestedCallbacks.begin(), defaultProcessDataRequestedCallbacks.end(), callbackData);

		if (defaultProcessDataRequestedCallbacks.end() != callbackLocation)
		{
			defaultProcessDataRequestedCallbacks.erase(callbackLocation);
		}
	}

	void TaskControllerClient::remove_request_value_callback(RequestValueCommandCallback callback, void *parentPointer)
	{
		LOCK_GUARD(Mutex, clientMutex);

		RequestValueCommandCallbackInfo callbackData = { callback, parentPointer };
		auto callbackLocation = std::find(requestValueCallbacks.begin(), requestValueCallbacks.end(), callbackData);

		if (requestValueCallbacks.end() != callbackLocation)
		{
			requestValueCallbacks.erase(callbackLocation);
		}
	}

	void TaskControllerClient::remove_value_command_callback(ValueCommandCallback callback, void *parentPointer)
	{
		LOCK_GUARD(Mutex, clientMutex);

		ValueCommandCallbackInfo callbackData = { callback, parentPointer };
		auto callbackLocation = std::find(valueCommandsCallbacks.begin(), valueCommandsCallbacks.end(), callbackData);

		if (valueCommandsCallbacks.end() != callbackLocation)
		{
			valueCommandsCallbacks.erase(callbackLocation);
		}
	}

	void TaskControllerClient::configure(std::shared_ptr<DeviceDescriptorObjectPool> DDOP,
	                                     std::uint8_t maxNumberBoomsSupported,
	                                     std::uint8_t maxNumberSectionsSupported,
	                                     std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
	                                     bool reportToTCSupportsDocumentation,
	                                     bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
	                                     bool reportToTCSupportsTCGEOWithPositionBasedControl,
	                                     bool reportToTCSupportsPeerControlAssignment,
	                                     bool reportToTCSupportsImplementSectionControl)
	{
		if (StateMachineState::Disconnected == get_state())
		{
			assert(nullptr != DDOP); // Client will not work without a DDOP.
			generatedBinaryDDOP.clear();
			ddopStructureLabel.clear();
			userSuppliedVectorDDOP = nullptr;
			ddopLocalizationLabel.fill(0x00);
			ddopUploadMode = DDOPUploadType::ProgramaticallyGenerated;
			clientDDOP = DDOP;
			userSuppliedBinaryDDOP = nullptr;
			userSuppliedBinaryDDOPSize_bytes = 0;
			set_common_config_items(maxNumberBoomsSupported,
			                        maxNumberSectionsSupported,
			                        maxNumberChannelsSupportedForPositionBasedControl,
			                        reportToTCSupportsDocumentation,
			                        reportToTCSupportsTCGEOWithoutPositionBasedControl,
			                        reportToTCSupportsTCGEOWithPositionBasedControl,
			                        reportToTCSupportsPeerControlAssignment,
			                        reportToTCSupportsImplementSectionControl);
		}
		else
		{
			// We don't want someone to erase our object pool or something while it is being used.
			LOG_ERROR("[TC]: Cannot reconfigure TC client while it is running!");
		}
	}

	void TaskControllerClient::configure(const std::uint8_t *binaryDDOP,
	                                     std::uint32_t DDOPSize,
	                                     std::uint8_t maxNumberBoomsSupported,
	                                     std::uint8_t maxNumberSectionsSupported,
	                                     std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
	                                     bool reportToTCSupportsDocumentation,
	                                     bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
	                                     bool reportToTCSupportsTCGEOWithPositionBasedControl,
	                                     bool reportToTCSupportsPeerControlAssignment,
	                                     bool reportToTCSupportsImplementSectionControl)
	{
		if (StateMachineState::Disconnected == get_state())
		{
			assert(nullptr != binaryDDOP); // Client will not work without a DDOP.
			assert(0 != DDOPSize);
			generatedBinaryDDOP.clear();
			ddopStructureLabel.clear();
			userSuppliedVectorDDOP = nullptr;
			ddopLocalizationLabel.fill(0x00);
			ddopUploadMode = DDOPUploadType::UserProvidedBinaryPointer;
			userSuppliedBinaryDDOP = binaryDDOP;
			userSuppliedBinaryDDOPSize_bytes = DDOPSize;
			set_common_config_items(maxNumberBoomsSupported,
			                        maxNumberSectionsSupported,
			                        maxNumberChannelsSupportedForPositionBasedControl,
			                        reportToTCSupportsDocumentation,
			                        reportToTCSupportsTCGEOWithoutPositionBasedControl,
			                        reportToTCSupportsTCGEOWithPositionBasedControl,
			                        reportToTCSupportsPeerControlAssignment,
			                        reportToTCSupportsImplementSectionControl);
		}
		else
		{
			// We don't want someone to erase our object pool or something while it is being used.
			LOG_ERROR("[TC]: Cannot reconfigure TC client while it is running!");
		}
	}

	void TaskControllerClient::configure(std::shared_ptr<std::vector<std::uint8_t>> binaryDDOP,
	                                     std::uint8_t maxNumberBoomsSupported,
	                                     std::uint8_t maxNumberSectionsSupported,
	                                     std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
	                                     bool reportToTCSupportsDocumentation,
	                                     bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
	                                     bool reportToTCSupportsTCGEOWithPositionBasedControl,
	                                     bool reportToTCSupportsPeerControlAssignment,
	                                     bool reportToTCSupportsImplementSectionControl)
	{
		if (StateMachineState::Disconnected == get_state())
		{
			assert(nullptr != binaryDDOP); // Client will not work without a DDOP.
			ddopStructureLabel.clear();
			generatedBinaryDDOP.clear();
			ddopLocalizationLabel.fill(0x00);
			userSuppliedVectorDDOP = binaryDDOP;
			ddopUploadMode = DDOPUploadType::UserProvidedVector;
			userSuppliedBinaryDDOP = nullptr;
			userSuppliedBinaryDDOPSize_bytes = 0;
			set_common_config_items(maxNumberBoomsSupported,
			                        maxNumberSectionsSupported,
			                        maxNumberChannelsSupportedForPositionBasedControl,
			                        reportToTCSupportsDocumentation,
			                        reportToTCSupportsTCGEOWithoutPositionBasedControl,
			                        reportToTCSupportsTCGEOWithPositionBasedControl,
			                        reportToTCSupportsPeerControlAssignment,
			                        reportToTCSupportsImplementSectionControl);
		}
		else
		{
			// We don't want someone to erase our object pool or something while it is being used.
			LOG_ERROR("[TC]: Cannot reconfigure TC client while it is running!");
		}
	}

	void TaskControllerClient::restart()
	{
		if (initialized)
		{
			LOCK_GUARD(Mutex, clientMutex);
			set_state(StateMachineState::Disconnected);
		}
	}

	void TaskControllerClient::terminate()
	{
		if (initialized)
		{
			if (nullptr != partnerControlFunction)
			{
				partnerControlFunction->remove_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData), process_rx_message, this);
				partnerControlFunction->remove_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::Acknowledge), process_rx_message, this);
				CANNetworkManager::CANNetwork.remove_global_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData), process_rx_message, this);
			}

			shouldTerminate = true;

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
			if ((nullptr != workerThread) && (workerThread->get_id() != std::this_thread::get_id()))
			{
				workerThread->join();
				delete workerThread;
				workerThread = nullptr;
			}
#endif
		}
	}

	std::shared_ptr<InternalControlFunction> TaskControllerClient::get_internal_control_function() const
	{
		return myControlFunction;
	}

	std::shared_ptr<PartneredControlFunction> TaskControllerClient::get_partner_control_function() const
	{
		return partnerControlFunction;
	}

	std::uint8_t TaskControllerClient::get_number_booms_supported() const
	{
		return numberBoomsSupported;
	}

	std::uint8_t TaskControllerClient::get_number_sections_supported() const
	{
		return numberSectionsSupported;
	}

	std::uint8_t TaskControllerClient::get_number_channels_supported_for_position_based_control() const
	{
		return numberChannelsSupportedForPositionBasedControl;
	}

	bool TaskControllerClient::get_supports_documentation() const
	{
		return supportsDocumentation;
	}

	bool TaskControllerClient::get_supports_tcgeo_without_position_based_control() const
	{
		return supportsTCGEOWithoutPositionBasedControl;
	}

	bool TaskControllerClient::get_supports_tcgeo_with_position_based_control() const
	{
		return supportsTCGEOWithPositionBasedControl;
	}

	bool TaskControllerClient::get_supports_peer_control_assignment() const
	{
		return supportsPeerControlAssignment;
	}

	bool TaskControllerClient::get_supports_implement_section_control() const
	{
		return supportsImplementSectionControl;
	}

	bool TaskControllerClient::get_is_initialized() const
	{
		return initialized;
	}

	bool TaskControllerClient::get_is_connected() const
	{
		return (StateMachineState::Connected == currentState);
	}

	bool TaskControllerClient::get_is_task_active() const
	{
		return (get_is_connected() && (0 != (0x01 & tcStatusBitfield)));
	}

	bool TaskControllerClient::reupload_device_descriptor_object_pool(std::shared_ptr<std::vector<std::uint8_t>> binaryDDOP)
	{
		bool retVal = false;
		LOCK_GUARD(Mutex, clientMutex);

		if (StateMachineState::Connected == get_state())
		{
			assert(nullptr != binaryDDOP); // Client will not work without a DDOP.
			assert(!binaryDDOP->empty()); // Client will not work without a DDOP.
			ddopStructureLabel.clear();
			generatedBinaryDDOP.clear();
			ddopLocalizationLabel.fill(0x00);
			userSuppliedVectorDDOP = binaryDDOP;
			ddopUploadMode = DDOPUploadType::UserProvidedVector;
			userSuppliedBinaryDDOP = nullptr;
			userSuppliedBinaryDDOPSize_bytes = 0;
			shouldReuploadAfterDDOPDeletion = true;
			set_state(StateMachineState::DeactivateObjectPool);
			clear_queues();
			retVal = true;
			LOG_INFO("[TC]: Requested to change the DDOP. Object pool will be deactivated for a little while.");
		}
		return retVal;
	}

	bool TaskControllerClient::reupload_device_descriptor_object_pool(std::uint8_t const *binaryDDOP, std::uint32_t DDOPSize)
	{
		bool retVal = false;
		LOCK_GUARD(Mutex, clientMutex);

		if (StateMachineState::Connected == get_state())
		{
			assert(nullptr != binaryDDOP); // Client will not work without a DDOP.
			assert(0 != DDOPSize);
			generatedBinaryDDOP.clear();
			ddopStructureLabel.clear();
			userSuppliedVectorDDOP = nullptr;
			ddopLocalizationLabel.fill(0x00);
			ddopUploadMode = DDOPUploadType::UserProvidedBinaryPointer;
			userSuppliedBinaryDDOP = binaryDDOP;
			userSuppliedBinaryDDOPSize_bytes = DDOPSize;
			shouldReuploadAfterDDOPDeletion = true;
			set_state(StateMachineState::DeactivateObjectPool);
			clear_queues();
			retVal = true;
			LOG_INFO("[TC]: Requested to change the DDOP. Object pool will be deactivated for a little while.");
		}
		return retVal;
	}

	bool TaskControllerClient::reupload_device_descriptor_object_pool(std::shared_ptr<DeviceDescriptorObjectPool> DDOP)
	{
		bool retVal = false;
		LOCK_GUARD(Mutex, clientMutex);

		if (StateMachineState::Connected == get_state())
		{
			assert(nullptr != DDOP); // Client will not work without a DDOP.
			generatedBinaryDDOP.clear();
			ddopStructureLabel.clear();
			userSuppliedVectorDDOP = nullptr;
			ddopLocalizationLabel.fill(0x00);
			ddopUploadMode = DDOPUploadType::ProgramaticallyGenerated;
			clientDDOP = DDOP;
			userSuppliedBinaryDDOP = nullptr;
			userSuppliedBinaryDDOPSize_bytes = 0;
			shouldReuploadAfterDDOPDeletion = true;
			set_state(StateMachineState::DeactivateObjectPool);
			clear_queues();
			retVal = true;
			LOG_INFO("[TC]: Requested to change the DDOP. Object pool will be deactivated for a little while.");
		}
		return retVal;
	}

	void TaskControllerClient::set_distance(std::uint32_t distance)
	{
		if (distance != totalMachineDistance)
		{
			LOCK_GUARD(Mutex, clientMutex);

			totalMachineDistance = distance;

			// Check all defined distance triggers to see if we need to send a value to the TC
			for (auto &distanceTrigger : measurementDistanceIntervalCommands)
			{
				if (totalMachineDistance >= (distanceTrigger.lastValue + distanceTrigger.processDataValue))
				{
					ProcessDataCallbackInfo requestData = { 0, 0, 0, 0, false, false };

					requestData.elementNumber = distanceTrigger.elementNumber;
					requestData.ddi = distanceTrigger.ddi;
					queuedValueRequests.push_back(requestData);
					distanceTrigger.lastValue = totalMachineDistance;
				}
			}
		}
	}

	void TaskControllerClient::update()
	{
		DeviceDescriptorObjectPool *pool = nullptr;

		// Check if we need to check all default process data requests before proceeding to queue/state processing.
		if (shouldProcessAllDefaultProcessDataRequests)
		{
			shouldProcessAllDefaultProcessDataRequests = false;

			if (nullptr != clientDDOP)
			{
				// Already have a DDOP, so we can process the requests.
				pool = clientDDOP.get();
			}
			else
			{
				// Need to parse the DDOP to know what the default process data is. This could fail if the DDOP is invalid
				// so if it fails, you need to check your DDOP for issues and the log for [DDOP] events!
				DeviceDescriptorObjectPool tempPool;
				bool poolValid = false;

				if (nullptr != userSuppliedVectorDDOP)
				{
					poolValid = tempPool.deserialize_binary_object_pool(userSuppliedVectorDDOP->data(), userSuppliedVectorDDOP->size(), myControlFunction->get_NAME());
				}
				else if (nullptr != userSuppliedBinaryDDOP)
				{
					poolValid = tempPool.deserialize_binary_object_pool(userSuppliedBinaryDDOP, userSuppliedBinaryDDOPSize_bytes, myControlFunction->get_NAME());
				}

				if (poolValid)
				{
					pool = &tempPool;
				}
				else
				{
					LOG_ERROR("[TC]: Cannot proceed with default process data request without a valid DDOP. Check log for [DDOP] events and fix any logged issues in your DDOP.");
				}
			}

			if (nullptr != pool)
			{
				for (std::size_t i = 0; i < pool->size(); i++)
				{
					auto object = pool->get_object_by_index(i);

					// Find every element, then find every DPD under it. For every DPD, check if it is a member of the default set
					// If it is, then we need to query for the trigger settings and send the initial value to the TC.

					if (task_controller_object::ObjectTypes::DeviceElement == object->get_object_type())
					{
						auto deviceElement = std::static_pointer_cast<task_controller_object::DeviceElementObject>(object);

						for (std::size_t j = 0; j < deviceElement->get_number_child_objects(); j++)
						{
							std::uint16_t childID = deviceElement->get_child_object_id(j);
							auto childObject = pool->get_object_by_id(childID);

							if ((nullptr != childObject) &&
							    (task_controller_object::ObjectTypes::DeviceProcessData == childObject->get_object_type()))
							{
								auto processData = std::static_pointer_cast<task_controller_object::DeviceProcessDataObject>(childObject);
								DefaultProcessDataSettings settings;

								for (const auto &callback : defaultProcessDataRequestedCallbacks)
								{
									if (callback.callback(deviceElement->get_element_number(), processData->get_ddi(), settings, callback.parent))
									{
										populate_any_triggers_from_settings(processData, deviceElement->get_element_number(), processData->get_ddi(), settings);
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		switch (currentState)
		{
			case StateMachineState::Disconnected:
			{
				enableStatusMessage = false;
				shouldReuploadAfterDDOPDeletion = false;

				if (get_was_ddop_supplied())
				{
					set_state(StateMachineState::WaitForStartUpDelay);
				}
			}
			break;

			case StateMachineState::WaitForStartUpDelay:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, SIX_SECOND_TIMEOUT_MS))
				{
					LOG_DEBUG("[TC]: Startup delay complete, waiting for TC server status message.");
					set_state(StateMachineState::WaitForServerStatusMessage);
				}
			}
			break;

			case StateMachineState::SendWorkingSetMaster:
			{
				if (send_working_set_master())
				{
					set_state(StateMachineState::SendStatusMessage);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout sending working set master message. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::SendStatusMessage:
			{
				// Start sending the status message
				if (send_status())
				{
					enableStatusMessage = true;
					statusMessageTimestamp_ms = SystemTiming::get_timestamp_ms();
					set_state(StateMachineState::RequestVersion);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout sending first status message. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::RequestVersion:
			{
				if (send_version_request())
				{
					set_state(StateMachineState::WaitForRequestVersionResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout sending version request message. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForRequestVersionResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for version request response. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForRequestVersionFromServer:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, SIX_SECOND_TIMEOUT_MS))
				{
					LOG_WARNING("[TC]: Timeout waiting for version request from TC. This is not required, so proceeding anways.");
					select_language_command_partner();
					set_state(StateMachineState::RequestLanguage);
				}
			}
			break;

			case StateMachineState::SendRequestVersionResponse:
			{
				if (send_request_version_response())
				{
					select_language_command_partner();
					set_state(StateMachineState::RequestLanguage);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout sending version request response. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::RequestLanguage:
			{
				if (languageCommandInterface.send_request_language_command())
				{
					set_state(StateMachineState::WaitForLanguageResponse);
					languageCommandWaitingTimestamp_ms = SystemTiming::get_timestamp_ms();
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, SIX_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to send request for language command message. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForLanguageResponse:
			{
				if ((SystemTiming::get_time_elapsed_ms(languageCommandInterface.get_language_command_timestamp()) < TWO_SECOND_TIMEOUT_MS) &&
				    ("" != languageCommandInterface.get_language_code()))
				{
					set_state(StateMachineState::ProcessDDOP);
				}
				else if (SystemTiming::time_expired_ms(languageCommandWaitingTimestamp_ms, SIX_SECOND_TIMEOUT_MS))
				{
					LOG_WARNING("[TC]: Timeout waiting for language response. Moving on to processing the DDOP anyways.");
					set_state(StateMachineState::ProcessDDOP);
				}
				else if ((SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS)) &&
				         (nullptr != languageCommandInterface.get_partner()))
				{
					LOG_WARNING("[TC]: No response to our request for the language command data, which is unusual.");

					if (nullptr != primaryVirtualTerminal)
					{
						LOG_WARNING("[TC]: Falling back to VT for language data.");
						languageCommandInterface.set_partner(primaryVirtualTerminal);
						languageCommandInterface.send_request_language_command();
						stateMachineTimestamp_ms = SystemTiming::get_timestamp_ms();
					}
					else
					{
						LOG_WARNING("[TC]: Since no VT was specified, falling back to a global request for language data.");
						languageCommandInterface.set_partner(nullptr);
						languageCommandInterface.send_request_language_command();
						stateMachineTimestamp_ms = SystemTiming::get_timestamp_ms();
					}
				}
			}
			break;

			case StateMachineState::ProcessDDOP:
			{
				if (DDOPUploadType::ProgramaticallyGenerated == ddopUploadMode)
				{
					assert(0 != clientDDOP->size()); // Need to have a valid object pool!

					if (serverVersion < clientDDOP->get_task_controller_compatibility_level())
					{
						clientDDOP->set_task_controller_compatibility_level(serverVersion); // Manipulate the DDOP slightly if needed to upload a version compatible DDOP
						LOG_INFO("[TC]: DDOP will be generated using the server's version instead of the specified version. New version: " +
						         isobus::to_string(static_cast<int>(serverVersion)));
					}

					if (generatedBinaryDDOP.empty())
					{
						// Binary DDOP has not been generated before.
						if (clientDDOP->generate_binary_object_pool(generatedBinaryDDOP))
						{
							process_labels_from_ddop();
							LOG_DEBUG("[TC]: DDOP Generated, size: " + isobus::to_string(static_cast<int>(generatedBinaryDDOP.size())));

							if ((!previousStructureLabel.empty()) && (ddopStructureLabel == previousStructureLabel))
							{
								LOG_ERROR("[TC]: You didn't properly update your new DDOP's structure label. ISO11783-10 states that an update to an object pool must include an updated structure label.");
							}
							previousStructureLabel = ddopStructureLabel;

							set_state(StateMachineState::RequestStructureLabel);
						}
						else
						{
							LOG_ERROR("[TC]: Cannot proceed with connection to TC due to invalid DDOP. Check log for [DDOP] events. TC client will now terminate.");
							terminate();
						}
					}
					else
					{
						LOG_DEBUG("[TC]: Using previously generated DDOP binary");
						set_state(StateMachineState::RequestStructureLabel);
					}
				}
				else
				{
					if ((ddopLocalizationLabel.empty()) ||
					    (ddopStructureLabel.empty()))
					{
						LOG_DEBUG("[TC]: Beginning a search of pre-serialized DDOP for device structure and localization labels.");
						process_labels_from_ddop();

						if ((ddopLocalizationLabel.empty()) ||
						    (ddopStructureLabel.empty()))
						{
							LOG_ERROR("[TC]: Failed to parse the DDOP. Ensure you provided a valid device object. TC client will now terminate.");
							terminate();
						}
					}
					else
					{
						LOG_DEBUG("[TC]: Reusing previously located device labels.");
					}
					set_state(StateMachineState::RequestStructureLabel);
				}
			}
			break;

			case StateMachineState::RequestStructureLabel:
			{
				if (send_request_structure_label())
				{
					set_state(StateMachineState::WaitForStructureLabelResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to send request for TC structure label. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForStructureLabelResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for TC structure label. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::RequestLocalizationLabel:
			{
				if (send_request_localization_label())
				{
					set_state(StateMachineState::WaitForLocalizationLabelResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to send request for TC localization label. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForLocalizationLabelResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for TC localization label. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::SendDeleteObjectPool:
			{
				if (send_delete_object_pool())
				{
					set_state(StateMachineState::WaitForDeleteObjectPoolResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to send delete object pool message. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForDeleteObjectPoolResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for delete object pool response. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::SendRequestTransferObjectPool:
			{
				if (send_request_object_pool_transfer())
				{
					set_state(StateMachineState::WaitForRequestTransferObjectPoolResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to send request to transfer object pool. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForRequestTransferObjectPoolResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for request transfer object pool response. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::BeginTransferDDOP:
			{
				bool transmitSuccessful = false;
				std::uint32_t dataLength = 0;

				switch (ddopUploadMode)
				{
					case DDOPUploadType::ProgramaticallyGenerated:
					{
						dataLength = static_cast<std::uint32_t>(generatedBinaryDDOP.size() + 1); // Account for Mux byte
					}
					break;

					case DDOPUploadType::UserProvidedBinaryPointer:
					{
						dataLength = static_cast<std::uint32_t>(userSuppliedBinaryDDOPSize_bytes + 1);
					}
					break;

					case DDOPUploadType::UserProvidedVector:
					{
						dataLength = static_cast<std::uint32_t>(userSuppliedVectorDDOP->size() + 1);
					}
					break;

					default:
						break;
				}

				assert(0 != dataLength);
				transmitSuccessful = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
				                                                                    nullptr,
				                                                                    dataLength,
				                                                                    myControlFunction,
				                                                                    partnerControlFunction,
				                                                                    CANIdentifier::CANPriority::PriorityLowest7,
				                                                                    process_tx_callback,
				                                                                    this,
				                                                                    process_internal_object_pool_upload_callback);
				if (transmitSuccessful)
				{
					set_state(StateMachineState::WaitForDDOPTransfer);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to begin the object pool upload. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForDDOPTransfer:
			case StateMachineState::WaitForServerStatusMessage:
			{
				// Waiting...
			}
			break;

			case StateMachineState::WaitForObjectPoolTransferResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for object pool transfer response. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::SendObjectPoolActivate:
			{
				if (send_object_pool_activate())
				{
					set_state(StateMachineState::WaitForObjectPoolActivateResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout trying to activate object pool. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::WaitForObjectPoolActivateResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout waiting for activate object pool response. Resetting client connection.");
					set_state(StateMachineState::Disconnected);
				}
			}
			break;

			case StateMachineState::Connected:
			{
				if (SystemTiming::time_expired_ms(serverStatusMessageTimestamp_ms, SIX_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Server Status Message Timeout. The TC may be offline.");
					set_state(StateMachineState::Disconnected);
				}
				else
				{
					process_queued_commands();
					process_queued_threshold_commands();
				}
			}
			break;

			case StateMachineState::DeactivateObjectPool:
			{
				if (send_object_pool_deactivate())
				{
					set_state(StateMachineState::WaitForObjectPoolDeactivateResponse);
				}
				else if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					LOG_ERROR("[TC]: Timeout sending object pool deactivate. Client terminated.");
					set_state(StateMachineState::Disconnected);
					terminate();
				}
			}
			break;

			case StateMachineState::WaitForObjectPoolDeactivateResponse:
			{
				if (SystemTiming::time_expired_ms(stateMachineTimestamp_ms, TWO_SECOND_TIMEOUT_MS))
				{
					if (shouldReuploadAfterDDOPDeletion)
					{
						LOG_WARNING("[TC]: Timeout waiting for deactivate object pool response. This is unusual, but we're just going to reconnect anyways.");
						shouldReuploadAfterDDOPDeletion = false;
						set_state(StateMachineState::ProcessDDOP);
					}
					else
					{
						LOG_ERROR("[TC]: Timeout waiting for deactivate object pool response. Client terminated.");
						set_state(StateMachineState::Disconnected);
						terminate();
					}
				}
			}
			break;

			default:
			{
				assert(false); // Unknown state? File a bug on GitHub if you see this happen.
			}
			break;
		}

		if ((enableStatusMessage) &&
		    (SystemTiming::time_expired_ms(statusMessageTimestamp_ms, TWO_SECOND_TIMEOUT_MS)) &&
		    (send_status()))
		{
			statusMessageTimestamp_ms = SystemTiming::get_timestamp_ms();
		}
	}

	bool TaskControllerClient::DefaultProcessDataRequestCallbackInfo::operator==(const DefaultProcessDataRequestCallbackInfo &obj) const
	{
		return ((obj.callback == this->callback) && (obj.parent == this->parent));
	}

	bool TaskControllerClient::ProcessDataCallbackInfo::operator==(const ProcessDataCallbackInfo &obj) const
	{
		return ((obj.ddi == this->ddi) && (obj.elementNumber == this->elementNumber));
	}

	bool TaskControllerClient::RequestValueCommandCallbackInfo::operator==(const RequestValueCommandCallbackInfo &obj) const
	{
		return (obj.callback == this->callback) && (obj.parent == this->parent);
	}

	bool TaskControllerClient::ValueCommandCallbackInfo::operator==(const ValueCommandCallbackInfo &obj) const
	{
		return (obj.callback == this->callback) && (obj.parent == this->parent);
	}

	void TaskControllerClient::add_measurement_change_threshold(ProcessDataCallbackInfo &info)
	{
		auto previousCommand = std::find(measurementOnChangeThresholdCommands.begin(), measurementOnChangeThresholdCommands.end(), info);
		if (measurementOnChangeThresholdCommands.end() == previousCommand)
		{
			measurementOnChangeThresholdCommands.push_back(info);
			LOG_DEBUG("[TC]: New change threshold trigger. Element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " on change by at least: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)));
		}
		else
		{
			// Just update the existing one with the new value
			previousCommand->processDataValue = info.processDataValue;
			previousCommand->thresholdPassed = false;
			LOG_DEBUG("[TC]: Altered change threshold trigger. Element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " new threshold: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)));
		}
	}

	void TaskControllerClient::add_measurement_distance_interval(ProcessDataCallbackInfo &info)
	{
		auto previousCommand = std::find(measurementDistanceIntervalCommands.begin(), measurementDistanceIntervalCommands.end(), info);
		if (measurementDistanceIntervalCommands.end() == previousCommand)
		{
			measurementDistanceIntervalCommands.push_back(info);
			LOG_DEBUG("[TC]: New distance interval trigger. Element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " every: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)) +
			          " mm.");
		}
		else
		{
			// Use the existing one and update the value
			previousCommand->processDataValue = info.processDataValue;
			LOG_DEBUG("[TC]: Altered distance interval trigger for element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " every: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)) +
			          " mm.");
		}
	}

	void TaskControllerClient::add_measurement_time_interval(ProcessDataCallbackInfo &info)
	{
		auto previousCommand = std::find(measurementTimeIntervalCommands.begin(), measurementTimeIntervalCommands.end(), info);
		if (measurementTimeIntervalCommands.end() == previousCommand)
		{
			measurementTimeIntervalCommands.push_back(info);
			LOG_DEBUG("[TC]: New time interval trigger. Element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " every: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)) +
			          " milliseconds.");
		}
		else
		{
			// Use the existing one and update the value
			previousCommand->processDataValue = info.processDataValue;
			LOG_DEBUG("[TC]: Altered time interval trigger for element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " every: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)) +
			          " milliseconds.");
		}
	}

	void TaskControllerClient::add_measurement_maximum_threshold(ProcessDataCallbackInfo &info)
	{
		auto previousCommand = std::find(measurementMaximumThresholdCommands.begin(), measurementMaximumThresholdCommands.end(), info);
		if (measurementMaximumThresholdCommands.end() == previousCommand)
		{
			measurementMaximumThresholdCommands.push_back(info);
			LOG_DEBUG("[TC]: New maximum measurement threshold trigger. Element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " when it is above the raw value: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)));
		}
		else
		{
			// Just update the existing one with the new value
			previousCommand->processDataValue = info.processDataValue;
			previousCommand->thresholdPassed = false;

			LOG_DEBUG("[TC]: Altered maximum threshold trigger for element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " threshold: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)));
		}
	}

	void TaskControllerClient::add_measurement_minimum_threshold(ProcessDataCallbackInfo &info)
	{
		auto previousCommand = std::find(measurementMinimumThresholdCommands.begin(), measurementMinimumThresholdCommands.end(), info);
		if (measurementMinimumThresholdCommands.end() == previousCommand)
		{
			measurementMinimumThresholdCommands.push_back(info);
			LOG_DEBUG("[TC]: New minimum measurement threshold trigger. Element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " when it is below the raw value: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)));
		}
		else
		{
			// Just update the existing one with the new value
			previousCommand->processDataValue = info.processDataValue;
			previousCommand->thresholdPassed = false;

			LOG_DEBUG("[TC]: Altered minimum threshold trigger for element: " +
			          isobus::to_string(static_cast<int>(info.elementNumber)) +
			          " DDI: " +
			          isobus::to_string(static_cast<int>(info.ddi)) +
			          " threshold: " +
			          isobus::to_string(static_cast<int>(info.processDataValue)));
		}
	}

	void TaskControllerClient::clear_queues()
	{
		queuedValueRequests.clear();
		queuedValueCommands.clear();
		measurementTimeIntervalCommands.clear();
		measurementMinimumThresholdCommands.clear();
		measurementMaximumThresholdCommands.clear();
		measurementOnChangeThresholdCommands.clear();
		measurementDistanceIntervalCommands.clear();
	}

	bool TaskControllerClient::get_was_ddop_supplied() const
	{
		bool retVal = false;

		switch (ddopUploadMode)
		{
			case DDOPUploadType::ProgramaticallyGenerated:
			{
				retVal = (nullptr != clientDDOP);
			}
			break;

			case DDOPUploadType::UserProvidedBinaryPointer:
			{
				retVal = (nullptr != userSuppliedBinaryDDOP) &&
				  (0 != userSuppliedBinaryDDOPSize_bytes);
			}
			break;

			case DDOPUploadType::UserProvidedVector:
			{
				retVal = (nullptr != userSuppliedVectorDDOP) &&
				  (!userSuppliedVectorDDOP->empty());
			}
			break;

			default:
				break;
		}
		return retVal;
	}

	void TaskControllerClient::populate_any_triggers_from_settings(std::shared_ptr<task_controller_object::DeviceProcessDataObject> processDataObject, std::uint16_t elementNumber, std::uint16_t DDI, const DefaultProcessDataSettings &settings)
	{
		if (nullptr != processDataObject)
		{
			if (settings.enableChangeThresholdTrigger)
			{
				if (0 != (processDataObject->get_trigger_methods_bitfield() & static_cast<std::uint8_t>(task_controller_object::DeviceProcessDataObject::AvailableTriggerMethods::ThresholdLimits)))
				{
					ProcessDataCallbackInfo triggerData = { 0, 0, 0, 0, false, false };

					triggerData.elementNumber = elementNumber;
					triggerData.ddi = DDI;
					triggerData.processDataValue = settings.changeThreshold;
					add_measurement_change_threshold(triggerData);
				}
				else
				{
					LOG_ERROR("[TC]: You have enabled a change threshold trigger, but your DDOP does not define it for this object! Element: %d, DDI: %d. You need to edit your DDOP, or remove this trigger.", elementNumber, DDI);
				}
			}
			if (settings.enableDistanceTrigger)
			{
				if (0 != (processDataObject->get_trigger_methods_bitfield() & static_cast<std::uint8_t>(task_controller_object::DeviceProcessDataObject::AvailableTriggerMethods::DistanceInterval)))
				{
					ProcessDataCallbackInfo triggerData = { 0, 0, 0, 0, false, false };

					// Distance triggers are weird because distance needs to be handled by the consuming application, not this interface.
					// We track it anyways so that the value can be sent when the trigger is set at least.
					triggerData.elementNumber = elementNumber;
					triggerData.ddi = DDI;
					triggerData.processDataValue = settings.distanceTriggerInterval_mm;
					add_measurement_distance_interval(triggerData);
				}
				else
				{
					LOG_ERROR("[TC]: You have enabled a distance interval trigger, but your DDOP does not define it for this object! Element: %d, DDI: %d. You need to edit your DDOP, or remove this trigger.", elementNumber, DDI);
				}
			}
			if (settings.enableMaximumWithinThresholdTrigger)
			{
				if (0 != (processDataObject->get_trigger_methods_bitfield() & static_cast<std::uint8_t>(task_controller_object::DeviceProcessDataObject::AvailableTriggerMethods::ThresholdLimits)))
				{
					ProcessDataCallbackInfo triggerData = { 0, 0, 0, 0, false, false };

					triggerData.elementNumber = elementNumber;
					triggerData.ddi = DDI;
					triggerData.processDataValue = settings.maximumWithinThreshold;
					add_measurement_maximum_threshold(triggerData);
				}
				else
				{
					LOG_ERROR("[TC]: You have enabled a maximum threshold limit trigger, but your DDOP does not define it for this object! Element: %d, DDI: %d. You need to edit your DDOP, or remove this trigger.", elementNumber, DDI);
				}
			}

			if (settings.enableMinimumWithinThresholdTrigger)
			{
				if (0 != (processDataObject->get_trigger_methods_bitfield() & static_cast<std::uint8_t>(task_controller_object::DeviceProcessDataObject::AvailableTriggerMethods::ThresholdLimits)))
				{
					ProcessDataCallbackInfo triggerData = { 0, 0, 0, 0, false, false };

					triggerData.elementNumber = elementNumber;
					triggerData.ddi = DDI;
					triggerData.processDataValue = settings.enableMinimumWithinThresholdTrigger;
					add_measurement_minimum_threshold(triggerData);
				}
				else
				{
					LOG_ERROR("[TC]: You have enabled a minimum threshold limit trigger, but your DDOP does not define it for this object! Element: %d, DDI: %d. You need to edit your DDOP, or remove this trigger.", elementNumber, DDI);
				}
			}

			if (settings.enableTimeTrigger)
			{
				if (0 != (processDataObject->get_trigger_methods_bitfield() & static_cast<std::uint8_t>(task_controller_object::DeviceProcessDataObject::AvailableTriggerMethods::TimeInterval)))
				{
					ProcessDataCallbackInfo triggerData = { 0, 0, 0, 0, false, false };

					// We'll automatically send time interval triggers to the TC if we have the timestamp set to zero here, so no need to manually mess with sending the trigger.
					triggerData.elementNumber = elementNumber;
					triggerData.ddi = DDI;
					triggerData.processDataValue = settings.timeTriggerInterval_ms;
					add_measurement_time_interval(triggerData);
				}
				else
				{
					LOG_ERROR("[TC]: You have enabled a time interval trigger, but your DDOP does not define it for this object! Element: %d, DDI: %d. You need to edit your DDOP, or remove this trigger.", elementNumber, DDI);
				}
			}
		}
	}

	void TaskControllerClient::process_labels_from_ddop()
	{
		std::uint32_t currentByteIndex = 0;
		const std::string DEVICE_TABLE_ID = "DVC";
		constexpr std::uint8_t DESIGNATOR_BYTE_OFFSET = 5;
		constexpr std::uint8_t CLIENT_NAME_LENGTH = 8;

		switch (ddopUploadMode)
		{
			case DDOPUploadType::ProgramaticallyGenerated:
			{
				assert(nullptr != clientDDOP); // You need a DDOP
				// Does your DDOP have a device object? Device object 0 is required by ISO11783-10
				auto deviceObject = clientDDOP->get_object_by_id(0);
				assert(nullptr != deviceObject);
				assert(task_controller_object::ObjectTypes::Device == deviceObject->get_object_type());

				ddopStructureLabel = std::static_pointer_cast<task_controller_object::DeviceObject>(deviceObject)->get_structure_label();

				while (ddopStructureLabel.size() < task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH)
				{
					ddopStructureLabel.push_back(' ');
				}

				ddopLocalizationLabel = std::static_pointer_cast<task_controller_object::DeviceObject>(deviceObject)->get_localization_label();
			}
			break;

			case DDOPUploadType::UserProvidedBinaryPointer:
			case DDOPUploadType::UserProvidedVector:
			{
				auto getDDOPSize = [this]() {
					if (ddopUploadMode == DDOPUploadType::UserProvidedBinaryPointer)
					{
						return userSuppliedBinaryDDOPSize_bytes;
					}
					else
					{
						return static_cast<std::uint32_t>(userSuppliedVectorDDOP->size());
					}
				};

				auto getDDOPByteAt = [this](std::size_t index) {
					if (ddopUploadMode == DDOPUploadType::UserProvidedBinaryPointer)
					{
						assert(nullptr != userSuppliedBinaryDDOP); // Can't be null!

						// just in case assert is disabled
						if (nullptr != userSuppliedBinaryDDOP)
						{
							return userSuppliedBinaryDDOP[index];
						}
						else
						{
							return std::uint8_t(0);
						}
					}
					else
					{
						return userSuppliedVectorDDOP->at(index);
					}
				};
				// Searching for "DVC"
				while (currentByteIndex < (getDDOPSize() - DEVICE_TABLE_ID.size()))
				{
					if ((DEVICE_TABLE_ID[0] == getDDOPByteAt(currentByteIndex)) &&
					    (DEVICE_TABLE_ID[1] == getDDOPByteAt(currentByteIndex + 1)) &&
					    (DEVICE_TABLE_ID[2] == getDDOPByteAt(currentByteIndex + 2)))
					{
						// We have to do a lot of error checking on the DDOP length
						// This is because we don't control the content of this DDOP, and have no
						// assurances that the schema is even valid

						assert((currentByteIndex + DESIGNATOR_BYTE_OFFSET) < getDDOPSize()); // Not enough bytes to read the designator length
						currentByteIndex += DESIGNATOR_BYTE_OFFSET; // Skip to the next variable length part of the object

						const std::uint32_t DESIGNATOR_LENGTH = getDDOPByteAt(currentByteIndex); // "N", See Table A.1
						assert(currentByteIndex + DESIGNATOR_LENGTH < getDDOPSize()); // Not enough bytes in your DDOP!
						currentByteIndex += DESIGNATOR_LENGTH + 1;

						const std::uint32_t SOFTWARE_VERSION_LENGTH = getDDOPByteAt(currentByteIndex); // "M", See Table A.1
						assert(currentByteIndex + SOFTWARE_VERSION_LENGTH + CLIENT_NAME_LENGTH < getDDOPSize()); // Not enough bytes in your DDOP!
						currentByteIndex += SOFTWARE_VERSION_LENGTH + CLIENT_NAME_LENGTH + 1;

						const std::uint32_t SERIAL_NUMBER_LENGTH = getDDOPByteAt(currentByteIndex); // "O", See Table A.1
						assert(currentByteIndex + SERIAL_NUMBER_LENGTH < getDDOPSize()); // Not enough bytes in your DDOP!
						currentByteIndex += SERIAL_NUMBER_LENGTH + 1;

						assert(currentByteIndex + task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH < getDDOPSize()); // // Not enough bytes in your DDOP!
						for (std::uint_fast8_t i = 0; i < task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH; i++)
						{
							ddopStructureLabel.push_back(getDDOPByteAt(currentByteIndex + i)); // Read the descriptor
						}

						currentByteIndex += task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH;
						assert(currentByteIndex + task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH < getDDOPSize()); // // Not enough bytes in your DDOP!
						for (std::uint_fast8_t i = 0; i < task_controller_object::DeviceObject::MAX_STRUCTURE_AND_LOCALIZATION_LABEL_LENGTH; i++)
						{
							ddopLocalizationLabel[i] = (getDDOPByteAt(currentByteIndex + i)); // Read the localization label
						}
						break;
					}
				}
			}
			break;

			default:
				break;
		}
	}

	void TaskControllerClient::process_queued_commands()
	{
		LOCK_GUARD(Mutex, clientMutex);
		bool transmitSuccessful = true;

		while (!queuedValueRequests.empty() && transmitSuccessful)
		{
			const auto &currentRequest = queuedValueRequests.front();

			for (auto &currentCallback : requestValueCallbacks)
			{
				std::int32_t newValue = 0;
				if (currentCallback.callback(currentRequest.elementNumber, currentRequest.ddi, newValue, currentCallback.parent))
				{
					transmitSuccessful = send_value_command(currentRequest.elementNumber, currentRequest.ddi, newValue);
					break;
				}
			}
			queuedValueRequests.pop_front();
		}
		while (!queuedValueCommands.empty() && transmitSuccessful)
		{
			const auto &currentRequest = queuedValueCommands.front();

			for (auto &currentCallback : valueCommandsCallbacks)
			{
				if (currentCallback.callback(currentRequest.elementNumber, currentRequest.ddi, currentRequest.processDataValue, currentCallback.parent))
				{
					break;
				}
			}
			queuedValueCommands.pop_front();

			//! @todo process PDACKs better
			if (currentRequest.ackRequested)
			{
				transmitSuccessful = send_pdack(currentRequest.elementNumber, currentRequest.ddi);
			}
		}
	}

	void TaskControllerClient::process_queued_threshold_commands()
	{
		bool transmitSuccessful = false;

		for (auto &measurementTimeCommand : measurementTimeIntervalCommands)
		{
			if (SystemTiming::time_expired_ms(static_cast<std::uint32_t>(measurementTimeCommand.lastValue), static_cast<std::uint32_t>(measurementTimeCommand.processDataValue)))
			{
				// Time to update this time interval variable
				transmitSuccessful = false;
				for (auto &currentCallback : requestValueCallbacks)
				{
					std::int32_t newValue = 0;
					if (currentCallback.callback(measurementTimeCommand.elementNumber, measurementTimeCommand.ddi, newValue, currentCallback.parent))
					{
						transmitSuccessful = send_value_command(measurementTimeCommand.elementNumber, measurementTimeCommand.ddi, newValue);
						break;
					}
				}

				if (transmitSuccessful)
				{
					measurementTimeCommand.lastValue = static_cast<std::int32_t>(SystemTiming::get_timestamp_ms());
				}
			}
		}
		for (auto &measurementMaxCommand : measurementMaximumThresholdCommands)
		{
			// Get the current process data value
			std::int32_t newValue = 0;
			for (auto &currentCallback : requestValueCallbacks)
			{
				if (currentCallback.callback(measurementMaxCommand.elementNumber, measurementMaxCommand.ddi, newValue, currentCallback.parent))
				{
					break;
				}
			}

			if (!measurementMaxCommand.thresholdPassed)
			{
				if ((newValue > measurementMaxCommand.processDataValue) &&
				    (send_value_command(measurementMaxCommand.elementNumber, measurementMaxCommand.ddi, newValue)))
				{
					measurementMaxCommand.thresholdPassed = true;
				}
			}
			else
			{
				if (newValue < measurementMaxCommand.processDataValue)
				{
					measurementMaxCommand.thresholdPassed = false;
				}
			}
		}
		for (auto &measurementMinCommand : measurementMinimumThresholdCommands)
		{
			// Get the current process data value
			std::int32_t newValue = 0;
			for (auto &currentCallback : requestValueCallbacks)
			{
				if (currentCallback.callback(measurementMinCommand.elementNumber, measurementMinCommand.ddi, newValue, currentCallback.parent))
				{
					break;
				}
			}

			if (!measurementMinCommand.thresholdPassed)
			{
				if ((newValue < measurementMinCommand.processDataValue) &&
				    (send_value_command(measurementMinCommand.elementNumber, measurementMinCommand.ddi, newValue)))
				{
					measurementMinCommand.thresholdPassed = true;
				}
			}
			else
			{
				if (newValue > measurementMinCommand.processDataValue)
				{
					measurementMinCommand.thresholdPassed = false;
				}
			}
		}
		for (auto &measurementChangeCommand : measurementOnChangeThresholdCommands)
		{
			// Get the current process data value
			std::int32_t newValue = 0;
			for (auto &currentCallback : requestValueCallbacks)
			{
				if (currentCallback.callback(measurementChangeCommand.elementNumber, measurementChangeCommand.ddi, newValue, currentCallback.parent))
				{
					break;
				}
			}

			std::int64_t lowerLimit = (static_cast<int64_t>(measurementChangeCommand.lastValue) - measurementChangeCommand.processDataValue);
			if (lowerLimit < 0)
			{
				lowerLimit = 0;
			}

			if ((newValue != measurementChangeCommand.lastValue) &&
			    ((newValue >= (measurementChangeCommand.lastValue + measurementChangeCommand.processDataValue)) ||
			     (newValue <= lowerLimit)) &&
			    (send_value_command(measurementChangeCommand.elementNumber, measurementChangeCommand.ddi, newValue)))
			{
				measurementChangeCommand.lastValue = newValue;
			}
		}
	}

	void TaskControllerClient::process_rx_message(const CANMessage &message, void *parentPointer)
	{
		if ((nullptr != parentPointer) &&
		    (CAN_DATA_LENGTH <= message.get_data_length()) &&
		    (nullptr != message.get_source_control_function()))
		{
			auto parentTC = static_cast<TaskControllerClient *>(parentPointer);
			auto &clientMutex = parentTC->clientMutex;
			const auto &messageData = message.get_data();

			switch (message.get_identifier().get_parameter_group_number())
			{
				case static_cast<std::uint32_t>(CANLibParameterGroupNumber::Acknowledge):
				{
					if (AcknowledgementType::Negative == static_cast<AcknowledgementType>(message.get_uint8_at(0)))
					{
						std::uint32_t targetParameterGroupNumber = message.get_uint24_at(5);
						if (static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData) == targetParameterGroupNumber)
						{
							LOG_ERROR("[TC]: The TC Server is NACK-ing our messages. Disconnecting.");
							parentTC->set_state(StateMachineState::Disconnected);
						}
					}
				}
				break;

				case static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData):
				{
					switch (static_cast<ProcessDataCommands>(messageData[0] & 0x0F))
					{
						case ProcessDataCommands::TechnicalCapabilities:
						{
							switch (static_cast<TechnicalDataMessageCommands>(messageData[0] >> 4))
							{
								case TechnicalDataMessageCommands::ParameterRequestVersion:
								{
									if (StateMachineState::WaitForRequestVersionFromServer == parentTC->get_state())
									{
										parentTC->set_state(StateMachineState::SendRequestVersionResponse);
									}
									else
									{
										LOG_WARNING("[TC]: Server requested version information at a strange time.");
									}
								}
								break;

								case TechnicalDataMessageCommands::ParameterVersion:
								{
									parentTC->serverVersion = messageData[1];
									parentTC->maxServerBootTime_s = messageData[2];
									parentTC->serverOptionsByte1 = messageData[3];
									parentTC->serverOptionsByte2 = messageData[4];
									parentTC->serverNumberOfBoomsForSectionControl = messageData[5];
									parentTC->serverNumberOfSectionsForSectionControl = messageData[6];
									parentTC->serverNumberOfChannelsForPositionBasedControl = messageData[7];

									if (messageData[1] > static_cast<std::uint8_t>(Version::SecondPublishedEdition))
									{
										LOG_WARNING("[TC]: Server version is newer than client's maximum supported version.");
									}
									LOG_DEBUG("[TC]: TC Server supports version %u with %u booms, %u sections, and %u position based control channels.",
									          messageData[1],
									          messageData[5],
									          messageData[6],
									          messageData[7]);

									if (StateMachineState::WaitForRequestVersionResponse == parentTC->get_state())
									{
										parentTC->set_state(StateMachineState::WaitForRequestVersionFromServer);
									}
								}
								break;

								default:
								{
									LOG_WARNING("[TC]: Unsupported process data technical data message received. Message will be dropped.");
								}
								break;
							}
						}
						break;

						case ProcessDataCommands::DeviceDescriptor:
						{
							switch (static_cast<DeviceDescriptorCommands>(messageData[0] >> 4))
							{
								case DeviceDescriptorCommands::StructureLabel:
								{
									if (StateMachineState::WaitForStructureLabelResponse == parentTC->get_state())
									{
										if ((0xFF == messageData[1]) &&
										    (0xFF == messageData[2]) &&
										    (0xFF == messageData[3]) &&
										    (0xFF == messageData[4]) &&
										    (0xFF == messageData[5]) &&
										    (0xFF == messageData[6]) &&
										    (0xFF == messageData[7]) &&
										    (CAN_DATA_LENGTH == messageData.size()))
										{
											// TC has no structure label for us. Need to upload the DDOP.
											parentTC->set_state(StateMachineState::SendRequestTransferObjectPool);
										}
										else
										{
											std::string tcStructure;

											for (std::size_t i = 1; i < messageData.size(); i++)
											{
												tcStructure.push_back(messageData[i]);
											}

											if (tcStructure.size() > 40)
											{
												LOG_WARNING("[TC]: Structure Label from TC exceeds the max length allowed by ISO11783-10");
											}

											if (parentTC->ddopStructureLabel == tcStructure)
											{
												// Structure label matched. No upload needed yet.
												LOG_DEBUG("[TC]: Task controller structure labels match");
												parentTC->set_state(StateMachineState::RequestLocalizationLabel);
											}
											else
											{
												// Structure label did not match. Need to delete current DDOP and re-upload.
												LOG_INFO("[TC]: Task controller structure labels do not match. DDOP will be deleted and reuploaded.");
												parentTC->set_state(StateMachineState::SendDeleteObjectPool);
											}
										}
									}
									else
									{
										LOG_WARNING("[TC]: Structure label message received, but ignored due to current state machine state.");
									}
								}
								break;

								case DeviceDescriptorCommands::LocalizationLabel:
								{
									// Right now, we'll just reload the pool if the localization doesn't match, but
									// in the future we should permit modifications to the localization and DVP objects
									//! @todo Localization label partial pool handling
									if (StateMachineState::WaitForLocalizationLabelResponse == parentTC->get_state())
									{
										if ((0xFF == messageData[1]) &&
										    (0xFF == messageData[2]) &&
										    (0xFF == messageData[3]) &&
										    (0xFF == messageData[4]) &&
										    (0xFF == messageData[5]) &&
										    (0xFF == messageData[6]) &&
										    (0xFF == messageData[7]) &&
										    (CAN_DATA_LENGTH == messageData.size()))
										{
											// TC has no localization label for us. Need to upload the DDOP.
											parentTC->set_state(StateMachineState::SendRequestTransferObjectPool);
										}
										else
										{
											assert(7 == parentTC->ddopLocalizationLabel.size()); // Make sure the DDOP is valid before we access the label. It must be 7 bytes
											bool labelsMatch = true;

											for (std::uint_fast8_t i = 0; i < (CAN_DATA_LENGTH - 1); i++)
											{
												if (messageData[i + 1] != parentTC->ddopLocalizationLabel[i])
												{
													labelsMatch = false;
													break;
												}
											}

											if (labelsMatch)
											{
												// DDOP labels all matched
												LOG_DEBUG("[TC]: Task controller localization labels match");
												parentTC->set_state(StateMachineState::SendObjectPoolActivate);
											}
											else
											{
												// Labels didn't match. Reupload
												LOG_INFO("[TC]: Task controller localization labels do not match. DDOP will be deleted and reuploaded.");
												parentTC->set_state(StateMachineState::SendDeleteObjectPool);
											}
										}
									}
									else
									{
										LOG_WARNING("[TC]: Localization label message received, but ignored due to current state machine state.");
									}
								}
								break;

								case DeviceDescriptorCommands::RequestObjectPoolTransferResponse:
								{
									if (StateMachineState::WaitForRequestTransferObjectPoolResponse == parentTC->get_state())
									{
										if (0 == messageData[1])
										{
											// Because there is overhead associated with object storage, it is impossible to predict whether there is enough memory available, technically.
											LOG_DEBUG("[TC]: Server indicates there may be enough memory available.");
											parentTC->set_state(StateMachineState::BeginTransferDDOP);
										}
										else
										{
											LOG_ERROR("[TC]: Server states that there is not enough memory available for our DDOP. Client will terminate.");
											parentTC->terminate();
										}
									}
									else
									{
										LOG_WARNING("[TC]: Request Object-pool Transfer Response message received, but ignored due to current state machine state.");
									}
								}
								break;

								case DeviceDescriptorCommands::ObjectPoolActivateDeactivateResponse:
								{
									if (StateMachineState::WaitForObjectPoolActivateResponse == parentTC->get_state())
									{
										if (0 == messageData[1])
										{
											LOG_INFO("[TC]: DDOP Activated without error.");
											parentTC->set_state(StateMachineState::Connected);
										}
										else
										{
											LOG_ERROR("[TC]: DDOP was not activated.");
											if (0x01 & messageData[1])
											{
												LOG_ERROR("[TC]: There are errors in the DDOP. Faulting parent ID: " +
												          isobus::to_string(static_cast<int>(static_cast<std::uint16_t>(messageData[2]) |
												                                             static_cast<std::uint16_t>(messageData[3] << 8))) +
												          " Faulting object: " +
												          isobus::to_string(static_cast<int>(static_cast<std::uint16_t>(messageData[4]) |
												                                             static_cast<std::uint16_t>(messageData[5] << 8))));
												if (0x01 & messageData[6])
												{
													LOG_ERROR("[TC]: Method or attribute not supported by the TC");
												}
												if (0x02 & messageData[6])
												{
													LOG_ERROR("[TC]: Unknown object reference (missing object)");
												}
												if (0x04 & messageData[6])
												{
													LOG_ERROR("[TC]: Unknown error (Any other error)");
												}
												if (0x08 & messageData[6])
												{
													LOG_ERROR("[TC]: Device descriptor object pool was deleted from volatile memory");
												}
												if (0xF0 & messageData[6])
												{
													LOG_WARNING("[TC]: The TC sent illegal errors in the reserved bits of the response.");
												}
											}
											if (0x02 & messageData[1])
											{
												LOG_ERROR("[TC]: Task Controller ran out of memory during activation.");
											}
											if (0x04 & messageData[1])
											{
												LOG_ERROR("[TC]: Task Controller indicates an unknown error occurred.");
											}
											if (0x08 & messageData[1])
											{
												LOG_ERROR("[TC]: A different DDOP with the same structure label already exists in the TC.");
											}
											if (0xF0 & messageData[1])
											{
												LOG_WARNING("[TC]: The TC sent illegal errors in the reserved bits of the response.");
											}
											parentTC->set_state(StateMachineState::Disconnected);
											LOG_ERROR("[TC]: Client terminated.");
											parentTC->terminate();
										}
									}
									else if (StateMachineState::WaitForObjectPoolDeactivateResponse == parentTC->get_state())
									{
										if (0 == messageData[1])
										{
											LOG_INFO("[TC]: Object pool deactivated OK.");

											if (parentTC->shouldReuploadAfterDDOPDeletion)
											{
												parentTC->set_state(StateMachineState::SendDeleteObjectPool);
											}
										}
										else
										{
											LOG_ERROR("[TC]: Object pool deactivation error.");
										}
									}
									else
									{
										LOG_WARNING("[TC]: Object pool activate/deactivate response received at a strange time. Message dropped.");
									}
								}
								break;

								case DeviceDescriptorCommands::ObjectPoolDeleteResponse:
								{
									// Message content of this is unreliable, the standard is ambiguous on what to even check.
									// Plus, if the delete failed, the recourse is the same, always proceed.
									if (StateMachineState::WaitForDeleteObjectPoolResponse == parentTC->get_state())
									{
										parentTC->set_state(StateMachineState::SendRequestTransferObjectPool);
									}
								}
								break;

								case DeviceDescriptorCommands::ObjectPoolTransferResponse:
								{
									if (StateMachineState::WaitForObjectPoolTransferResponse == parentTC->get_state())
									{
										if (0 == messageData[1])
										{
											LOG_DEBUG("[TC]: DDOP upload completed with no errors.");
											parentTC->set_state(StateMachineState::SendObjectPoolActivate);
										}
										else
										{
											if (0x01 == messageData[1])
											{
												LOG_ERROR("[TC]: DDOP upload completed but TC ran out of memory during transfer.");
											}
											else
											{
												LOG_ERROR("[TC]: DDOP upload completed but TC had some unknown error.");
											}
											LOG_ERROR("[TC]: Client terminated.");
											parentTC->terminate();
										}
									}
									else
									{
										LOG_WARNING("[TC]: Recieved unexpected object pool transfer response");
									}
								}
								break;

								default:
								{
									LOG_WARNING("[TC]: Unsupported device descriptor command message received. Message will be dropped.");
								}
								break;
							}
						}
						break;

						case ProcessDataCommands::StatusMessage:
						{
							if (parentTC->partnerControlFunction->get_NAME() == message.get_source_control_function()->get_NAME())
							{
								// Many values in the status message were undefined in version 2 and before, so the
								// standard explicitly tells us to ignore those attributes. The only things that really
								// matter are that we got the mesesage, and bytes 5, 6 and 7.
								parentTC->tcStatusBitfield = messageData[4];
								parentTC->sourceAddressOfCommandBeingExecuted = messageData[5];
								parentTC->commandBeingExecuted = messageData[6];
								parentTC->serverStatusMessageTimestamp_ms = SystemTiming::get_timestamp_ms();
								if (StateMachineState::WaitForServerStatusMessage == parentTC->currentState)
								{
									parentTC->set_state(StateMachineState::SendWorkingSetMaster);
								}
							}
						}
						break;

						case ProcessDataCommands::ClientTask:
						{
							LOG_WARNING("[TC]: Server sent the client task message, which is not meant to be sent by servers.");
						}
						break;

						case ProcessDataCommands::RequestValue:
						{
							ProcessDataCallbackInfo requestData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							requestData.ackRequested = false;
							requestData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							requestData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							requestData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->queuedValueRequests.push_back(requestData);

							// Handle requests for default process data specially. Ask the app for all the defaults and send a response.
							if (static_cast<std::uint16_t>(DataDescriptionIndex::RequestDefaultProcessData) == requestData.ddi)
							{
								// The RequestDefaultProcessData DDI may only be requested from	device element 0 of a device
								if (0 == requestData.elementNumber)
								{
									LOG_DEBUG("[TC]: Server requested default process data.");
									parentTC->shouldProcessAllDefaultProcessDataRequests = true; // Defer to the TC thread or app thread
								}
								else
								{
									LOG_WARNING("[TC]: Server requested default process data from an illegal element.");
								}
							}
						}
						break;

						case ProcessDataCommands::Value:
						{
							ProcessDataCallbackInfo requestData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							requestData.ackRequested = false;
							requestData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							requestData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							requestData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->queuedValueCommands.push_back(requestData);
						}
						break;

						case ProcessDataCommands::SetValueAndAcknowledge:
						{
							ProcessDataCallbackInfo requestData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							requestData.ackRequested = true;
							requestData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							requestData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							requestData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->queuedValueCommands.push_back(requestData);
						}
						break;

						case ProcessDataCommands::MeasurementTimeInterval:
						{
							ProcessDataCallbackInfo commandData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							commandData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							commandData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							commandData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							commandData.lastValue = static_cast<std::int32_t>(SystemTiming::get_timestamp_ms());
							parentTC->add_measurement_time_interval(commandData);
						}
						break;

						case ProcessDataCommands::MeasurementMaximumWithinThreshold:
						{
							ProcessDataCallbackInfo commandData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							commandData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							commandData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							commandData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->add_measurement_maximum_threshold(commandData);
						}
						break;

						case ProcessDataCommands::MeasurementMinimumWithinThreshold:
						{
							ProcessDataCallbackInfo commandData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							commandData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							commandData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							commandData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->add_measurement_minimum_threshold(commandData);
						}
						break;

						case ProcessDataCommands::MeasurementChangeThreshold:
						{
							ProcessDataCallbackInfo commandData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							commandData.elementNumber = (static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							commandData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							commandData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->add_measurement_change_threshold(commandData);
						}
						break;

						case ProcessDataCommands::MeasurementDistanceInterval:
						{
							ProcessDataCallbackInfo commandData = { 0, 0, 0, 0, false, false };
							LOCK_GUARD(Mutex, clientMutex);

							commandData.elementNumber = static_cast<std::uint16_t>(static_cast<std::uint16_t>(messageData[0] >> 4) | (static_cast<std::uint16_t>(messageData[1]) << 4));
							commandData.ddi = static_cast<std::uint16_t>(messageData[2]) |
							  (static_cast<std::uint16_t>(messageData[3]) << 8);
							commandData.processDataValue = (static_cast<std::int32_t>(messageData[4]) |
							                                (static_cast<std::int32_t>(messageData[5]) << 8) |
							                                (static_cast<std::int32_t>(messageData[6]) << 16) |
							                                (static_cast<std::int32_t>(messageData[7]) << 24));
							parentTC->add_measurement_distance_interval(commandData);
						}
						break;

						case ProcessDataCommands::ProcessDataAcknowledge:
						{
							if (0 != messageData[4])
							{
								LOG_WARNING("[TC]: TC sent us a PDNACK");
							}
						}
						break;

						default:
						{
							LOG_WARNING("[TC]: Unhandled process data message!");
						}
						break;
					}
				}
				break;

				default:
				{
				}
				break;
			}
		}
	}

	bool TaskControllerClient::process_internal_object_pool_upload_callback(std::uint32_t,
	                                                                        std::uint32_t bytesOffset,
	                                                                        std::uint32_t numberOfBytesNeeded,
	                                                                        std::uint8_t *chunkBuffer,
	                                                                        void *parentPointer)
	{
		auto parentTCClient = static_cast<TaskControllerClient *>(parentPointer);
		bool retVal = false;

		// These assertions should never fail, but if they do, please consider reporting it on our GitHub page
		// along with a CAN trace and accompanying CANStackLogger output of the issue.
		assert(nullptr != parentTCClient);
		assert(nullptr != chunkBuffer);
		assert(0 != numberOfBytesNeeded);

		if (((bytesOffset + numberOfBytesNeeded) <= parentTCClient->generatedBinaryDDOP.size() + 1) ||
		    ((bytesOffset + numberOfBytesNeeded) <= parentTCClient->userSuppliedBinaryDDOPSize_bytes + 1))
		{
			retVal = true;
			if (0 == bytesOffset)
			{
				chunkBuffer[0] = static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
				  (static_cast<std::uint8_t>(DeviceDescriptorCommands::ObjectPoolTransfer) << 4);

				if (DDOPUploadType::UserProvidedBinaryPointer == parentTCClient->ddopUploadMode)
				{
					memcpy(&chunkBuffer[1], &parentTCClient->userSuppliedBinaryDDOP[bytesOffset], numberOfBytesNeeded - 1);
				}
				else
				{
					memcpy(&chunkBuffer[1], &parentTCClient->generatedBinaryDDOP[bytesOffset], numberOfBytesNeeded - 1);
				}
			}
			else
			{
				if (DDOPUploadType::UserProvidedBinaryPointer == parentTCClient->ddopUploadMode)
				{
					// Subtract off 1 to account for the mux in the first byte of the message
					memcpy(chunkBuffer, &parentTCClient->userSuppliedBinaryDDOP[bytesOffset - 1], numberOfBytesNeeded);
				}
				else
				{
					// Subtract off 1 to account for the mux in the first byte of the message
					memcpy(chunkBuffer, &parentTCClient->generatedBinaryDDOP[bytesOffset - 1], numberOfBytesNeeded);
				}
			}
		}
		else
		{
			LOG_ERROR("[TC]: DDOP internal data callback received out of range request.");
		}
		return retVal;
	}

	void TaskControllerClient::process_tx_callback(std::uint32_t parameterGroupNumber,
	                                               std::uint32_t,
	                                               std::shared_ptr<InternalControlFunction>,
	                                               std::shared_ptr<ControlFunction> destinationControlFunction,
	                                               bool successful,
	                                               void *parentPointer)
	{
		if ((nullptr != parentPointer) &&
		    (static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData) == parameterGroupNumber) &&
		    (nullptr != destinationControlFunction))
		{
			auto parent = reinterpret_cast<TaskControllerClient *>(parentPointer);

			if (StateMachineState::WaitForDDOPTransfer == parent->get_state())
			{
				if (successful)
				{
					parent->set_state(StateMachineState::WaitForObjectPoolTransferResponse);
				}
				else
				{
					LOG_ERROR("[TC]: DDOP upload did not complete. Resetting.");
					parent->set_state(StateMachineState::Disconnected);
				}
			}
		}
	}

	bool TaskControllerClient::send_delete_object_pool() const
	{
		return send_generic_process_data(static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
		                                 (static_cast<std::uint8_t>(DeviceDescriptorCommands::ObjectPoolDelete) << 4));
	}

	bool TaskControllerClient::send_generic_process_data(std::uint8_t multiplexer) const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { multiplexer,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF };

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_object_pool_activate() const
	{
		return send_generic_process_data(static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
		                                 (static_cast<std::uint8_t>(DeviceDescriptorCommands::ObjectPoolActivateDeactivate) << 4));
	}

	bool TaskControllerClient::send_object_pool_deactivate() const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
			                                                           (static_cast<std::uint8_t>(DeviceDescriptorCommands::ObjectPoolActivateDeactivate) << 4),
			                                                         0x00,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF };

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_pdack(std::uint16_t elementNumber, std::uint16_t ddi) const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { static_cast<std::uint8_t>(static_cast<std::uint8_t>(ProcessDataCommands::ProcessDataAcknowledge) |
			                                                                                   static_cast<std::uint8_t>(elementNumber & 0x0F) << 4),
			                                                         static_cast<std::uint8_t>(elementNumber >> 4),
			                                                         static_cast<std::uint8_t>(ddi & 0xFF),
			                                                         static_cast<std::uint8_t>(ddi >> 8),
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF };
		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_request_localization_label() const
	{
		return send_generic_process_data(static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
		                                 (static_cast<std::uint8_t>(DeviceDescriptorCommands::RequestLocalizationLabel) << 4));
	}

	bool TaskControllerClient::send_request_object_pool_transfer() const
	{
		std::size_t binaryPoolSize = generatedBinaryDDOP.size();

		if (DDOPUploadType::UserProvidedBinaryPointer == ddopUploadMode)
		{
			binaryPoolSize = userSuppliedBinaryDDOPSize_bytes;
		}

		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
			                                                           (static_cast<std::uint8_t>(DeviceDescriptorCommands::RequestObjectPoolTransfer) << 4),
			                                                         static_cast<std::uint8_t>(binaryPoolSize & 0xFF),
			                                                         static_cast<std::uint8_t>((binaryPoolSize >> 8) & 0xFF),
			                                                         static_cast<std::uint8_t>((binaryPoolSize >> 16) & 0xFF),
			                                                         static_cast<std::uint8_t>((binaryPoolSize >> 24) & 0xFF),
			                                                         0xFF,
			                                                         0xFF,
			                                                         0xFF };

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_request_structure_label() const
	{
		// When all bytes are 0xFF, the TC will tell us about the latest structure label
		return send_generic_process_data(static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) |
		                                 (static_cast<std::uint8_t>(DeviceDescriptorCommands::RequestStructureLabel) << 4));
	}

	bool TaskControllerClient::send_request_version_response() const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { (static_cast<std::uint8_t>(TechnicalDataMessageCommands::ParameterVersion) << 4),
			                                                         static_cast<std::uint8_t>(Version::SecondPublishedEdition),
			                                                         0xFF, // Must be 0xFF when a client sends it (boot time)
			                                                         static_cast<std::uint8_t>(static_cast<std::uint8_t>(supportsDocumentation) |
			                                                                                   (static_cast<std::uint8_t>(supportsTCGEOWithoutPositionBasedControl) << 1) |
			                                                                                   (static_cast<std::uint8_t>(supportsTCGEOWithPositionBasedControl) << 2) |
			                                                                                   (static_cast<std::uint8_t>(supportsPeerControlAssignment) << 3) |
			                                                                                   (static_cast<std::uint8_t>(supportsImplementSectionControl) << 4)),
			                                                         0x00,
			                                                         numberBoomsSupported,
			                                                         numberSectionsSupported,
			                                                         numberChannelsSupportedForPositionBasedControl };

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_status() const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { static_cast<std::uint8_t>(ProcessDataCommands::ClientTask) | 0xF0,
			                                                         0xFF, // Element number N/A
			                                                         0xFF, // DDI N/A
			                                                         0xFF, // DDI N/A
			                                                         static_cast<std::uint8_t>(tcStatusBitfield & 0x01), // Actual TC or DL status
			                                                         0x00, // Reserved (0)
			                                                         0x00, // Reserved (0)
			                                                         0x00 }; // Reserved (0)

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_value_command(std::uint16_t elementNumber, std::uint16_t ddi, std::int32_t value) const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { static_cast<std::uint8_t>(static_cast<std::uint8_t>(ProcessDataCommands::Value) |
			                                                                                   (static_cast<std::uint8_t>(elementNumber & 0x0F) << 4)),
			                                                         static_cast<std::uint8_t>(elementNumber >> 4),
			                                                         static_cast<std::uint8_t>(ddi & 0xFF),
			                                                         static_cast<std::uint8_t>(ddi >> 8),
			                                                         static_cast<std::uint8_t>(value),
			                                                         static_cast<std::uint8_t>(value >> 8),
			                                                         static_cast<std::uint8_t>(value >> 16),
			                                                         static_cast<std::uint8_t>(value >> 24) };
		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      partnerControlFunction);
	}

	bool TaskControllerClient::send_version_request() const
	{
		return send_generic_process_data(static_cast<std::uint8_t>(TechnicalDataMessageCommands::ParameterRequestVersion));
	}

	bool TaskControllerClient::send_working_set_master() const
	{
		const std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { numberOfWorkingSetMembers, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::WorkingSetMaster),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      nullptr);
	}

	void TaskControllerClient::set_common_config_items(std::uint8_t maxNumberBoomsSupported,
	                                                   std::uint8_t maxNumberSectionsSupported,
	                                                   std::uint8_t maxNumberChannelsSupportedForPositionBasedControl,
	                                                   bool reportToTCSupportsDocumentation,
	                                                   bool reportToTCSupportsTCGEOWithoutPositionBasedControl,
	                                                   bool reportToTCSupportsTCGEOWithPositionBasedControl,
	                                                   bool reportToTCSupportsPeerControlAssignment,
	                                                   bool reportToTCSupportsImplementSectionControl)
	{
		numberBoomsSupported = maxNumberBoomsSupported;
		numberSectionsSupported = maxNumberSectionsSupported;
		numberChannelsSupportedForPositionBasedControl = maxNumberChannelsSupportedForPositionBasedControl;
		supportsDocumentation = reportToTCSupportsDocumentation;
		supportsTCGEOWithoutPositionBasedControl = reportToTCSupportsTCGEOWithoutPositionBasedControl;
		supportsTCGEOWithPositionBasedControl = reportToTCSupportsTCGEOWithPositionBasedControl;
		supportsPeerControlAssignment = reportToTCSupportsPeerControlAssignment;
		supportsImplementSectionControl = reportToTCSupportsImplementSectionControl;
	}

	void TaskControllerClient::set_state(StateMachineState newState)
	{
		if (newState != currentState)
		{
			stateMachineTimestamp_ms = SystemTiming::get_timestamp_ms();
			currentState = newState;

			if (StateMachineState::Disconnected == newState)
			{
				clear_queues();
			}
		}
	}

	void TaskControllerClient::set_state(StateMachineState newState, std::uint32_t timestamp)
	{
		stateMachineTimestamp_ms = timestamp;
		currentState = newState;
	}

	void TaskControllerClient::select_language_command_partner()
	{
		if (serverVersion < static_cast<std::uint8_t>(Version::SecondPublishedEdition))
		{
			if (nullptr == primaryVirtualTerminal)
			{
				languageCommandInterface.set_partner(nullptr); // TC might not reply and no VT specified, so just see if anyone knows.
				LOG_WARNING("[TC]: The TC is < version 4 but no VT was provided. Language data will be requested globally, which might not be ideal.");
			}
			else
			{
				languageCommandInterface.set_partner(primaryVirtualTerminal);
				LOG_DEBUG("[TC]: Using VT as the partner for language data, because the TC's version is less than 4.");
			}
		}
	}

	void TaskControllerClient::worker_thread_function()
	{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		for (;;)
		{
			if (shouldTerminate)
			{
				break;
			}
			update();
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
#endif
	}

	TaskControllerClient::StateMachineState TaskControllerClient::get_state() const
	{
		return currentState;
	}

	std::uint8_t TaskControllerClient::get_connected_tc_number_booms_supported() const
	{
		return serverNumberOfBoomsForSectionControl;
	}

	std::uint8_t TaskControllerClient::get_connected_tc_number_sections_supported() const
	{
		return serverNumberOfSectionsForSectionControl;
	}

	std::uint8_t TaskControllerClient::get_connected_tc_number_channels_supported() const
	{
		return serverNumberOfChannelsForPositionBasedControl;
	}

	std::uint8_t TaskControllerClient::get_connected_tc_max_boot_time() const
	{
		return maxServerBootTime_s;
	}

	bool TaskControllerClient::get_connected_tc_option_supported(ServerOptions option) const
	{
		return (0 != (static_cast<std::uint8_t>(option) & serverOptionsByte1));
	}

	TaskControllerClient::Version TaskControllerClient::get_connected_tc_version() const
	{
		Version retVal = Version::Unknown;

		if (serverVersion <= static_cast<std::uint8_t>(Version::SecondPublishedEdition))
		{
			retVal = static_cast<Version>(serverVersion);
		}
		return retVal;
	}

	void TaskControllerClient::on_value_changed_trigger(std::uint16_t elementNumber, std::uint16_t DDI)
	{
		ProcessDataCallbackInfo requestData = { 0, 0, 0, 0, false, false };
		LOCK_GUARD(Mutex, clientMutex);

		requestData.ackRequested = false;
		requestData.elementNumber = elementNumber;
		requestData.ddi = DDI;
		requestData.processDataValue = 0;
		queuedValueRequests.push_back(requestData);
	}

	bool TaskControllerClient::request_task_controller_identification() const
	{
		constexpr std::array<std::uint8_t, CAN_DATA_LENGTH> buffer = { static_cast<std::uint8_t>(ProcessDataCommands::TechnicalCapabilities) |
			                                                               (static_cast<std::uint8_t>(TechnicalDataMessageCommands::IdentifyTaskController) << 4),
			                                                             0xFF,
			                                                             0xFF,
			                                                             0xFF,
			                                                             0xFF,
			                                                             0xFF,
			                                                             0xFF,
			                                                             0xFF };
		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
		                                                      buffer.data(),
		                                                      CAN_DATA_LENGTH,
		                                                      myControlFunction,
		                                                      nullptr);
	}

} // namespace isobus
