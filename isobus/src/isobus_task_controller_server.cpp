//================================================================================================
/// @file isobus_task_controller_server.cpp
///
/// @brief Implements portions of an abstract task controller server class.
/// You can consume this file and implement the pure virtual functions to create your own
/// task controller or data logger server.
/// @author Adrian Del Grosso
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_task_controller_server.hpp"

#include "isobus/isobus/can_general_parameter_group_numbers.hpp"
#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/isobus/isobus_data_dictionary.hpp"
#include "isobus/utility/system_timing.hpp"

#include <cassert>

namespace isobus
{
	TaskControllerServer::TaskControllerServer(std::shared_ptr<InternalControlFunction> internalControlFunction,
	                                           std::uint8_t numberBoomsSupported,
	                                           std::uint8_t numberSectionsSupported,
	                                           std::uint8_t numberChannelsSupportedForPositionBasedControl,
	                                           const TaskControllerOptions &options,
	                                           TaskControllerVersion versionToReport) :
	  languageCommandInterface(internalControlFunction, true),
	  serverControlFunction(internalControlFunction),
	  reportedVersion(versionToReport),
	  numberBoomsSupportedToReport(numberBoomsSupported),
	  numberSectionsSupportedToReport(numberSectionsSupported),
	  numberChannelsSupportedForPositionBasedControlToReport(numberChannelsSupportedForPositionBasedControl),
	  optionsBitfieldToReport(options.get_bitfield())
	{
	}

	TaskControllerServer::~TaskControllerServer()
	{
		terminate();
	}

	bool TaskControllerServer::send_request_value(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber) const
	{
		std::array<std::uint8_t, CAN_DATA_LENGTH> payload = { 0 };

		payload[0] = (static_cast<std::uint8_t>(ProcessDataCommands::RequestValue) & 0x0F);
		payload[0] |= (elementNumber & 0x0F) << 4;
		payload[1] = static_cast<std::uint8_t>(elementNumber >> 4);
		payload[2] = static_cast<std::uint8_t>(dataDescriptionIndex);
		payload[3] = static_cast<std::uint8_t>(dataDescriptionIndex >> 8);
		payload[4] = 0xFF;
		payload[5] = 0xFF;
		payload[6] = 0xFF;
		payload[7] = 0xFF;
		return send_process_data_to_client(clientControlFunction,
		                                   payload.data(),
		                                   payload.size());
	}

	bool TaskControllerServer::send_time_interval_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t timeInterval) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::MeasurementTimeInterval), dataDescriptionIndex, elementNumber, timeInterval);
	}

	bool TaskControllerServer::send_distance_interval_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t distanceInterval) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::MeasurementDistanceInterval), dataDescriptionIndex, elementNumber, distanceInterval);
	}

	bool TaskControllerServer::send_minimum_threshold_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t minimum) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::MeasurementMinimumWithinThreshold), dataDescriptionIndex, elementNumber, minimum);
	}

	bool TaskControllerServer::send_maximum_threshold_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t maximum) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::MeasurementMaximumWithinThreshold), dataDescriptionIndex, elementNumber, maximum);
	}

	bool TaskControllerServer::send_change_threshold_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t threshold) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::MeasurementChangeThreshold), dataDescriptionIndex, elementNumber, threshold);
	}

	bool TaskControllerServer::send_set_value_and_acknowledge(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t processDataValue) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::SetValueAndAcknowledge), dataDescriptionIndex, elementNumber, processDataValue);
	}

	bool TaskControllerServer::send_set_value(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t processDataValue) const
	{
		return send_measurement_command(clientControlFunction, static_cast<std::uint8_t>(ProcessDataCommands::Value), dataDescriptionIndex, elementNumber, processDataValue);
	}

	void TaskControllerServer::set_task_totals_active(bool isTaskActive)
	{
		if (isTaskActive != get_task_totals_active())
		{
			currentStatusByte &= ~static_cast<std::uint8_t>(ServerStatusBit::TaskTotalsActive);
			currentStatusByte |= (static_cast<std::uint8_t>(isTaskActive) & static_cast<std::uint8_t>(ServerStatusBit::TaskTotalsActive));
			lastStatusMessageTimestamp_ms = 0; // Force a status message to be sent on the next update.
		}
	}

	bool TaskControllerServer::get_task_totals_active() const
	{
		return (0 != (currentStatusByte & static_cast<std::uint8_t>(ServerStatusBit::TaskTotalsActive)));
	}

	void TaskControllerServer::initialize()
	{
		if (!initialized)
		{
			assert(nullptr != serverControlFunction); // You can't have a server without a control function to send messages from! That wouldn't make any sense.
			languageCommandInterface.initialize();
			CANNetworkManager::CANNetwork.add_any_control_function_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData), store_rx_message, this);
			CANNetworkManager::CANNetwork.add_any_control_function_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::WorkingSetMaster), store_rx_message, this);
			initialized = true;
		}
	}

	bool TaskControllerServer::get_initialized() const
	{
		return initialized;
	}

	void TaskControllerServer::terminate()
	{
		if (initialized)
		{
			initialized = false;
			CANNetworkManager::CANNetwork.remove_any_control_function_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData), store_rx_message, this);
			CANNetworkManager::CANNetwork.remove_any_control_function_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::WorkingSetMaster), store_rx_message, this);
		}
	}

	LanguageCommandInterface &TaskControllerServer::get_language_command_interface()
	{
		return languageCommandInterface;
	}

#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
	std::condition_variable &TaskControllerServer::get_condition_variable()
	{
		return updateWakeupCondition;
	}
#endif

	void TaskControllerServer::update()
	{
		process_rx_messages();
		if ((true == SystemTiming::time_expired_ms(lastStatusMessageTimestamp_ms, STATUS_MESSAGE_RATE_MS)) &&
		    (true == send_status_message()))
		{
			lastStatusMessageTimestamp_ms = SystemTiming::get_timestamp_ms();
		}

		// Remove any clients that have timed out.
		activeClients.erase(std::remove_if(activeClients.begin(),
		                                   activeClients.end(),
		                                   [](std::shared_ptr<ActiveClient> clientInfo) {
			                                   constexpr std::uint32_t CLIENT_TASK_TIMEOUT_MS = 6000;
			                                   if (SystemTiming::time_expired_ms(clientInfo->lastStatusMessageTimestamp_ms, CLIENT_TASK_TIMEOUT_MS))
			                                   {
				                                   LOG_WARNING("[TC Server]: Client %hhu has timed out. Removing from active client list.", clientInfo->clientControlFunction->get_address());
				                                   return true;
			                                   }
			                                   return false;
		                                   }),
		                    activeClients.end());
	}

	TaskControllerServer::ActiveClient::ActiveClient(std::shared_ptr<ControlFunction> clientControlFunction) :
	  clientControlFunction(clientControlFunction),
	  lastStatusMessageTimestamp_ms(SystemTiming::get_timestamp_ms())
	{
	}

	void TaskControllerServer::store_rx_message(const CANMessage &message, void *parentPointer)
	{
		if (nullptr != parentPointer)
		{
			auto server = static_cast<TaskControllerServer *>(parentPointer);
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
			const std::lock_guard<std::mutex> lock(server->messagesMutex);
			server->rxMessageQueue.push_back(message);
			server->updateWakeupCondition.notify_all();
#else
			server->rxMessageQueue.push_back(message);
#endif
		}
	}

	void TaskControllerServer::process_rx_messages()
	{
#if !defined CAN_STACK_DISABLE_THREADS && !defined ARDUINO
		const std::lock_guard<std::mutex> lock(messagesMutex);
#endif
		while (!rxMessageQueue.empty())
		{
			const auto &rxMessage = rxMessageQueue.front();
			auto &rxData = rxMessage.get_data();

			switch (rxMessage.get_identifier().get_parameter_group_number())
			{
				case static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData):
				{
					switch (static_cast<ProcessDataCommands>(rxData[0] & 0x0F))
					{
						case ProcessDataCommands::TechnicalCapabilities:
						{
							if ((rxData[0] >> 4) <= static_cast<std::uint8_t>(TechnicalDataCommandParameters::IdentifyTaskController))
							{
								switch (static_cast<TechnicalDataCommandParameters>(rxData[0] >> 4))
								{
									case TechnicalDataCommandParameters::RequestVersion:
									{
										if (serverControlFunction == rxMessage.get_destination_control_function())
										{
											send_version(rxMessage.get_source_control_function());
											send_generic_process_data_default_payload(static_cast<std::uint8_t>(TechnicalDataCommandParameters::RequestVersion), rxMessage.get_source_control_function());
										}
									}
									break;

									case TechnicalDataCommandParameters::ParameterVersion:
									{
										if (CAN_DATA_LENGTH == rxMessage.get_data_length())
										{
											uint8_t version = rxData[1];

											// We can store the reported version to use the proper DDOP parsing approach later on.
											LOG_DEBUG("[TC Server]: Client reports that its version is %u", version);
											get_active_client(rxMessage.get_source_control_function())->reportedVersion = version;
										}
									}
									break;

									case TechnicalDataCommandParameters::IdentifyTaskController:
									{
										LOG_INFO("[TC Server]: Received identify task controller command from 0x%02X. We are TC number %u", rxMessage.get_source_control_function()->get_address(), serverControlFunction->get_NAME().get_function_instance());
										if (serverControlFunction == rxMessage.get_destination_control_function())
										{
											send_generic_process_data_default_payload(rxData[0], rxMessage.get_source_control_function());
											identify_task_controller(serverControlFunction->get_NAME().get_function_instance() + 1);
										}
										else
										{
											// No response needed for a global request.
											identify_task_controller(serverControlFunction->get_NAME().get_function_instance() + 1);
										}
									}
									break;
								}
							}
							else
							{
								LOG_WARNING("[TC Server]: Unknown technical capabilities command received: 0x%02X", rxData[0]);
							}
						}
						break;

						case ProcessDataCommands::DeviceDescriptor:
						{
							if ((rxData[0] >> 4) <= static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::ChangeDesignatorResponse))
							{
								if ((rxMessage.get_data_length() >= CAN_DATA_LENGTH) &&
								    (nullptr != rxMessage.get_source_control_function()) &&
								    (nullptr != rxMessage.get_destination_control_function()))
								{
									switch (static_cast<DeviceDescriptorCommandParameters>(rxData[0] >> 4))
									{
										case DeviceDescriptorCommandParameters::RequestStructureLabel:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												std::vector<std::uint8_t> structureLabel;
												std::vector<std::uint8_t> extendedStructureLabel;

												for (std::uint8_t i = 0; i < CAN_DATA_LENGTH - 1; i++)
												{
													structureLabel.push_back(rxData[i + 1]);
												}

												if (rxMessage.get_data_length() > CAN_DATA_LENGTH)
												{
													// If the length is greater than 8, then an extended label is being requested.
													for (std::size_t i = 0; i < (rxMessage.get_data_length() - CAN_DATA_LENGTH); i++)
													{
														extendedStructureLabel.push_back(rxData[CAN_DATA_LENGTH + i]);
													}
												}

												if (get_is_stored_device_descriptor_object_pool_by_structure_label(rxMessage.get_source_control_function(), structureLabel, extendedStructureLabel))
												{
													LOG_INFO("[TC Server]:Client %hhu structure label(s) matched.", rxMessage.get_source_control_function()->get_address());
													send_structure_label(rxMessage.get_source_control_function(), structureLabel, extendedStructureLabel);
												}
												else
												{
													// No object pool found. Send FFs as the structure label.
													LOG_INFO("[TC Server]:Client %hhu structure label(s) did not match. Sending 0xFFs as the structure label.", rxMessage.get_source_control_function()->get_address());
													send_generic_process_data_default_payload((static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | (static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::StructureLabel) << 4)), rxMessage.get_source_control_function());
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::RequestLocalizationLabel:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												const std::array<std::uint8_t, 7> localizationLabel = { rxData.at(1), rxData.at(2), rxData.at(3), rxData.at(4), rxData.at(5), rxData.at(6), rxData.at(7) };
												if (get_is_stored_device_descriptor_object_pool_by_localization_label(rxMessage.get_source_control_function(), localizationLabel))
												{
													LOG_INFO("[TC Server]:Client %hhu localization label matched.", rxMessage.get_source_control_function()->get_address());
													send_localization_label(rxMessage.get_source_control_function(), localizationLabel);
												}
												else
												{
													// No object pool found. Send FFs as the localization label.
													LOG_INFO("[TC Server]: No object pool found for client %hhu localization label. Sending FFs as the localization label.", rxMessage.get_source_control_function()->get_address());
													send_generic_process_data_default_payload(static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::LocalizationLabel) << 4, rxMessage.get_source_control_function());
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::RequestObjectPoolTransfer:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												std::uint32_t requestedSize = rxMessage.get_uint32_at(1);

												if ((requestedSize <= CANMessage::ABSOLUTE_MAX_MESSAGE_LENGTH) &&
												    (get_is_enough_memory_available(requestedSize)))
												{
													LOG_INFO("[TC Server]: Client %hhu requests object pool transfer of %u bytes", rxMessage.get_source_control_function()->get_address(), requestedSize);

													get_active_client(rxMessage.get_source_control_function())->clientDDOPsize_bytes = requestedSize;
													send_request_object_pool_transfer_response(rxMessage.get_source_control_function(), true);
												}
												else
												{
													LOG_ERROR("[TC Server]: Client %hhu requests object pool transfer of %u bytes but there is not enough memory available.", rxMessage.get_source_control_function()->get_address(), requestedSize);
													send_request_object_pool_transfer_response(rxMessage.get_source_control_function(), false);
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::ObjectPoolTransfer:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												std::vector<std::uint8_t> objectPool = rxData;
												objectPool.erase(objectPool.begin()); // Strip the command byte from the front of the object pool

												if (0 == get_active_client(rxMessage.get_source_control_function())->clientDDOPsize_bytes)
												{
													LOG_WARNING("[TC Server]: Client %hhu sent object pool transfer without first requesting a transfer!", rxMessage.get_source_control_function()->get_address());
												}

												if (store_device_descriptor_object_pool(rxMessage.get_source_control_function(), objectPool, 0 != get_active_client(rxMessage.get_source_control_function())->numberOfObjectPoolSegments))
												{
													LOG_INFO("[TC Server]: Stored DDOP segment for client %hhu", rxMessage.get_source_control_function()->get_address());
													send_object_pool_transfer_response(rxMessage.get_source_control_function(), 0, static_cast<std::uint32_t>(objectPool.size())); // No error, transfer OK
												}
												else
												{
													LOG_ERROR("[TC Server]: Failed to store DDOP segment for client %hhu. Reporting to the client as \"Any other error\"", rxMessage.get_source_control_function()->get_address());
													send_object_pool_transfer_response(rxMessage.get_source_control_function(), 2, static_cast<std::uint32_t>(objectPool.size()));
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::ObjectPoolActivateDeactivate:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												constexpr std::uint8_t ACTIVATE = 0xFF;
												constexpr std::uint8_t DEACTIVATE = 0x00;
												ObjectPoolActivationError activationError = ObjectPoolActivationError::NoErrors;
												ObjectPoolErrorCodes errorCode = ObjectPoolErrorCodes::NoErrors;
												std::uint16_t faultingParentObject = 0;
												std::uint16_t faultingObject = 0;

												if (ACTIVATE == rxData[1])
												{
													LOG_INFO("[TC Server]: Client %hhu requests activation of object pool", rxMessage.get_source_control_function()->get_address());
													auto client = get_active_client(rxMessage.get_source_control_function());

													if (activate_object_pool(rxMessage.get_source_control_function(), activationError, errorCode, faultingParentObject, faultingObject))
													{
														LOG_INFO("[TC Server]: Object pool activated for client %hhu", rxMessage.get_source_control_function()->get_address());
														client->isDDOPActive = true;
														send_object_pool_activate_deactivate_response(rxMessage.get_source_control_function(), 0, 0, 0xFFFF, 0xFFFF);
													}
													else
													{
														LOG_ERROR("[TC Server]: Failed to activate object pool for client %hhu. Error code: %u, Faulty object: %u, Parent of faulty object: %u", rxMessage.get_source_control_function()->get_address(), static_cast<std::uint8_t>(activationError), faultingObject, faultingParentObject);
														send_object_pool_activate_deactivate_response(rxMessage.get_source_control_function(), static_cast<std::uint8_t>(activationError), static_cast<std::uint8_t>(errorCode), faultingParentObject, faultingObject);
													}
												}
												else if (DEACTIVATE == rxData[1])
												{
													LOG_INFO("[TC Server]: Client %hhu requests deactivation of object pool", rxMessage.get_source_control_function()->get_address());

													if (deactivate_object_pool(rxMessage.get_source_control_function()))
													{
														LOG_INFO("[TC Server]: Object pool deactivated for client %hhu", rxMessage.get_source_control_function()->get_address());
														get_active_client(rxMessage.get_source_control_function())->isDDOPActive = false;
														send_object_pool_activate_deactivate_response(rxMessage.get_source_control_function(), 0, 0, 0xFFFF, 0xFFFF);
													}
													else
													{
														LOG_ERROR("[TC Server]: Failed to deactivate object pool for client %hhu", rxMessage.get_source_control_function()->get_address());
														send_object_pool_activate_deactivate_response(rxMessage.get_source_control_function(), static_cast<std::uint8_t>(ObjectPoolActivationError::AnyOtherError), 0, 0xFFFF, 0xFFFF);
													}
												}
												else
												{
													LOG_ERROR("[TC Server]: Client %hhu requests activation/deactivation of object pool with invalid value: 0x%02X", rxMessage.get_source_control_function()->get_address(), rxData[1]);
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::DeleteObjectPool:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												ObjectPoolDeletionErrors errorCode = ObjectPoolDeletionErrors::ErrorDetailsNotAvailable;

												if (delete_device_descriptor_object_pool(rxMessage.get_source_control_function(), errorCode))
												{
													LOG_INFO("[TC Server]: Deleted object pool for client %hhu", rxMessage.get_source_control_function()->get_address());
													send_delete_object_pool_response(rxMessage.get_source_control_function(), true, static_cast<std::uint8_t>(ObjectPoolDeletionErrors::ErrorDetailsNotAvailable));
												}
												else
												{
													LOG_ERROR("[TC Server]: Failed to delete object pool for client %hhu. Error code: %u", rxMessage.get_source_control_function()->get_address(), static_cast<std::uint8_t>(errorCode));
													send_delete_object_pool_response(rxMessage.get_source_control_function(), false, static_cast<std::uint8_t>(errorCode));
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::ChangeDesignator:
										{
											if (nullptr != get_active_client(rxMessage.get_source_control_function()))
											{
												if (get_active_client(rxMessage.get_source_control_function())->isDDOPActive)
												{
													std::uint16_t objectID = rxMessage.get_uint16_at(1);
													std::vector<std::uint8_t> newDesignatorUTF8Bytes;

													for (std::size_t i = 0; i < rxData.size() - 3; i++)
													{
														newDesignatorUTF8Bytes.push_back(rxData[3 + i]);
													}

													if (change_designator(rxMessage.get_source_control_function(), objectID, newDesignatorUTF8Bytes))
													{
														LOG_INFO("[TC Server]: Changed designator for client %hhu. Object ID: %u", rxMessage.get_source_control_function()->get_address(), objectID);
														send_change_designator_response(rxMessage.get_source_control_function(), objectID, 0);
													}
													else
													{
														LOG_ERROR("[TC Server]: Failed to change designator for client %hhu. Object ID: %u", rxMessage.get_source_control_function()->get_address(), objectID);
														send_change_designator_response(rxMessage.get_source_control_function(), objectID, 1);
													}
												}
												else
												{
													LOG_ERROR("[TC Server]: Client %hhu requests change to change a designator but the object pool is not active.", rxMessage.get_source_control_function()->get_address());
												}
											}
											else
											{
												nack_process_data_command(rxMessage.get_source_control_function());
											}
										}
										break;

										case DeviceDescriptorCommandParameters::StructureLabel:
										case DeviceDescriptorCommandParameters::LocalizationLabel:
										case DeviceDescriptorCommandParameters::RequestObjectPoolTransferResponse:
										case DeviceDescriptorCommandParameters::ObjectPoolTransferResponse:
										case DeviceDescriptorCommandParameters::ObjectPoolActivateDeactivateResponse:
										case DeviceDescriptorCommandParameters::DeleteObjectPoolResponse:
										case DeviceDescriptorCommandParameters::ChangeDesignatorResponse:
										{
											// Nack server side messages
											nack_process_data_command(rxMessage.get_source_control_function());
										}
										break;
									}
								}
								else
								{
									LOG_WARNING("[TC Server]: Device descriptor message received with invalid DLC. DLC must be at least 8.");
								}
							}
							else
							{
								LOG_WARNING("[TC Server]: Unknown device descriptor command received: 0x%02X", rxData[0]);
							}
						}
						break;

						case ProcessDataCommands::Value:
						case ProcessDataCommands::SetValueAndAcknowledge:
						{
							if (nullptr != get_active_client(rxMessage.get_source_control_function()))
							{
								if (get_active_client(rxMessage.get_source_control_function())->isDDOPActive)
								{
									std::uint16_t DDI = rxMessage.get_uint16_at(2);
									std::uint16_t elementNumber = static_cast<std::uint16_t>(rxData[0] >> 4) | (static_cast<std::uint16_t>(rxData[1]) << 4);
									std::int32_t processVariableValue = rxMessage.get_int32_at(4);
									std::uint8_t errorCodes = 0;

									if (on_value_command(rxMessage.get_source_control_function(), DDI, elementNumber, processVariableValue, errorCodes))
									{
										LOG_DEBUG("[TC Server]: Client %hhu value command for element %u DDI %s with value %s OK.", rxMessage.get_source_control_function()->get_address(), elementNumber, DataDictionary::ddi_to_string(DDI).c_str(), DataDictionary::format_value_with_ddi(DDI, processVariableValue).c_str());

										if (ProcessDataCommands::SetValueAndAcknowledge == static_cast<ProcessDataCommands>(rxData[0] & 0x0F))
										{
											send_process_data_acknowledge(rxMessage.get_source_control_function(), DDI, elementNumber, 0, static_cast<ProcessDataCommands>(rxData[0] & 0x0F));
										}
									}
									else
									{
										LOG_ERROR("[TC Server]: Client %hhu value command for element %u DDI %s with value %s failed.", rxMessage.get_source_control_function()->get_address(), elementNumber, DataDictionary::ddi_to_string(DDI).c_str(), DataDictionary::format_value_with_ddi(DDI, processVariableValue).c_str());

										if (0 == errorCodes)
										{
											LOG_ERROR("[TC Server]: Your derived TC server class must set errorCodes to a non-zero value if a value command fails.");
											errorCodes = static_cast<std::uint8_t>(ProcessDataAcknowledgeErrorCodes::DDINotSupportedByElement); // Like this!
											assert(false); // See above error message.
										}
										send_process_data_acknowledge(rxMessage.get_source_control_function(), DDI, elementNumber, errorCodes, static_cast<ProcessDataCommands>(rxData[0] & 0x0F));
									}
								}
								else
								{
									LOG_ERROR("[TC Server]: Client %hhu sent a value command but the object pool is not active.", rxMessage.get_source_control_function()->get_address());
								}
							}
							else
							{
								nack_process_data_command(rxMessage.get_source_control_function());
							}
						}
						break;

						case ProcessDataCommands::Acknowledge:
						{
							if (nullptr != get_active_client(rxMessage.get_source_control_function()))
							{
								std::uint16_t DDI = rxMessage.get_uint16_at(2);
								std::uint16_t elementNumber = static_cast<std::uint16_t>(rxData[0] >> 4) | (static_cast<std::uint16_t>(rxData[1]) << 4);

								if (get_active_client(rxMessage.get_source_control_function())->isDDOPActive)
								{
									on_process_data_acknowledge(rxMessage.get_source_control_function(), DDI, elementNumber, rxData[4], static_cast<ProcessDataCommands>(rxData[0] & 0x0F));
								}
								else
								{
									LOG_ERROR("[TC Server]: Client %hhu sent an acknowledge command but the object pool is not active.", rxMessage.get_source_control_function()->get_address());
									send_process_data_acknowledge(rxMessage.get_source_control_function(), DDI, elementNumber, static_cast<std::uint8_t>(ProcessDataAcknowledgeErrorCodes::ProcessDataNotSettable), static_cast<ProcessDataCommands>(rxData[0] & 0x0F));
								}
							}
							else
							{
								nack_process_data_command(rxMessage.get_source_control_function());
							}
						}
						break;

						case ProcessDataCommands::MeasurementTimeInterval:
						case ProcessDataCommands::MeasurementDistanceInterval:
						case ProcessDataCommands::MeasurementMinimumWithinThreshold:
						case ProcessDataCommands::MeasurementMaximumWithinThreshold:
						case ProcessDataCommands::MeasurementChangeThreshold:
						{
							if (CAN_DATA_LENGTH == rxMessage.get_data_length())
							{
								std::uint16_t DDI = static_cast<std::uint16_t>(rxData[2]) | (static_cast<std::uint16_t>(rxData[3]) << 8);
								std::uint16_t elementNumber = static_cast<std::uint16_t>(rxData[0] >> 4) | (static_cast<std::uint16_t>(rxData[1]) << 4);
								LOG_ERROR("[TC Server]: Client %hhu is sending measurement commands?", rxMessage.get_source_control_function()->get_address());
								send_process_data_acknowledge(rxMessage.get_source_control_function(), DDI, elementNumber, static_cast<std::uint8_t>(ProcessDataAcknowledgeErrorCodes::ProcessDataCommandNotSupported), static_cast<ProcessDataCommands>(rxData[0] & 0x0F));
							}
							else
							{
								LOG_ERROR("[TC Server]: Client %hhu is sending measurement commands with invalid lengths, which is very unusual.", rxMessage.get_source_control_function()->get_address());
							}
						}
						break;

						case ProcessDataCommands::Status:
						case ProcessDataCommands::RequestValue:
						{
							// Ignore server side messages
						}
						break;

						case ProcessDataCommands::ClientTask:
						{
							if (CAN_DATA_LENGTH == rxMessage.get_data_length())
							{
								for (const auto &activeClient : activeClients)
								{
									if ((nullptr != activeClient) &&
									    (activeClient->clientControlFunction == rxMessage.get_source_control_function()))
									{
										std::uint32_t status = rxData[4];
										status |= static_cast<std::uint32_t>(rxData[5]) << 8;
										status |= static_cast<std::uint32_t>(rxData[6]) << 16;
										status |= static_cast<std::uint32_t>(rxData[7]) << 24;
										activeClient->lastStatusMessageTimestamp_ms = SystemTiming::get_timestamp_ms();
										activeClient->statusBitfield = status;
									}
								}
							}
							else
							{
								LOG_WARNING("[TC Server]: client task message received with invalid DLC. DLC must be 8.");
							}
						}
						break;

						case ProcessDataCommands::PeerControlAssignment:
						{
							LOG_WARNING("[TC Server]: Peer Control is currently not supported");
						}
						break;

						case ProcessDataCommands::Reserved:
						case ProcessDataCommands::Reserved2:
						{
							LOG_WARNING("[TC Server]: Reserved command received: 0x%02X", rxData[0]);
						}
						break;

						default:
						{
							LOG_WARNING("[TC Server]: Unknown ProcessData command received: 0x%02X", rxData[0]);
						}
						break;
					}
				}
				break;

				case static_cast<std::uint32_t>(CANLibParameterGroupNumber::WorkingSetMaster):
				{
					if (CAN_DATA_LENGTH == rxMessage.get_data_length())
					{
						std::uint8_t numberOfWorkingSetMembers = rxData[0];

						if (1 == numberOfWorkingSetMembers)
						{
							if (nullptr == get_active_client(rxMessage.get_source_control_function()))
							{
								activeClients.push_back(std::make_shared<ActiveClient>(rxMessage.get_source_control_function()));
							}
						}
						else
						{
							LOG_ERROR("[TC Server]: Working set master message received with unsupported number of working set members: %u", numberOfWorkingSetMembers);
						}
					}
					else
					{
						LOG_ERROR("[TC Server]: Working set master message received with invalid DLC. DLC should be 8.");
					}
				}
				break;

				default:
				{
				}
				break;
			}
			rxMessageQueue.pop_front();
		}
	}

	bool TaskControllerServer::send_generic_process_data_default_payload(std::uint8_t multiplexer, std::shared_ptr<ControlFunction> destination) const
	{
		std::array<std::uint8_t, CAN_DATA_LENGTH> payload = { multiplexer, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		CANIdentifier::CANPriority priority = CANIdentifier::CANPriority::Priority5;

		switch (multiplexer & 0x0F)
		{
			case static_cast<std::uint8_t>(ProcessDataCommands::Value):
			case static_cast<std::uint8_t>(ProcessDataCommands::SetValueAndAcknowledge):
			case static_cast<std::uint8_t>(ProcessDataCommands::Status):
			case static_cast<std::uint8_t>(ProcessDataCommands::ClientTask):
			{
				priority = CANIdentifier::CANPriority::Priority3;
			}
			break;

			case static_cast<std::uint8_t>(ProcessDataCommands::Acknowledge):
			{
				priority = CANIdentifier::CANPriority::Priority4;
			}
			break;

			default:
				break;
		}
		return send_process_data_to_client(destination,
		                                   payload.data(),
		                                   payload.size(),
		                                   priority);
	}

	bool TaskControllerServer::send_measurement_command(std::shared_ptr<ControlFunction> clientControlFunction, std::uint8_t commandValue, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint32_t processDataValue) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			std::array<std::uint8_t, CAN_DATA_LENGTH> payload = { 0 };
			payload[0] = (commandValue & 0x0F);
			payload[0] |= (elementNumber & 0x0F) << 4;
			payload[1] = static_cast<std::uint8_t>(elementNumber >> 4);
			payload[2] = static_cast<std::uint8_t>(dataDescriptionIndex);
			payload[3] = static_cast<std::uint8_t>(dataDescriptionIndex >> 8);
			payload[4] = static_cast<std::uint8_t>(processDataValue);
			payload[5] = static_cast<std::uint8_t>(processDataValue >> 8);
			payload[6] = static_cast<std::uint8_t>(processDataValue >> 16);
			payload[7] = static_cast<std::uint8_t>(processDataValue >> 24);

			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size(),
			                                     static_cast<std::uint8_t>(ProcessDataCommands::SetValueAndAcknowledge) == commandValue ? CANIdentifier::CANPriority::Priority3 : CANIdentifier::CANPriority::Priority5);
		}
		return retVal;
	}

	bool TaskControllerServer::send_status_message() const
	{
		std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
			static_cast<std::uint8_t>(ProcessDataCommands::Status) | 0xF0U,
			0xFF,
			0xFF,
			0xFF,
			currentStatusByte,
			currentCommandSourceAddress,
			currentCommandByte,
			0xFF
		};

		return send_process_data_to_client(nullptr,
		                                   payload.data(),
		                                   payload.size(),
		                                   CANIdentifier::CANPriority::Priority3);
	}

	bool TaskControllerServer::send_version(std::shared_ptr<ControlFunction> clientControlFunction) const
	{
		std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
			static_cast<std::uint8_t>(TechnicalDataCommandParameters::ParameterVersion) << 4,
			static_cast<std::uint8_t>(reportedVersion),
			0xFF,
			optionsBitfieldToReport,
			0x00,
			numberBoomsSupportedToReport,
			numberSectionsSupportedToReport,
			numberChannelsSupportedForPositionBasedControlToReport
		};
		return send_process_data_to_client(clientControlFunction,
		                                   payload.data(),
		                                   payload.size());
	}

	std::shared_ptr<TaskControllerServer::ActiveClient> TaskControllerServer::get_active_client(std::shared_ptr<ControlFunction> clientControlFunction) const
	{
		for (const auto &activeClient : activeClients)
		{
			if ((nullptr != activeClient) &&
			    (nullptr != activeClient->clientControlFunction) &&
			    (nullptr != clientControlFunction) &&
			    (activeClient->clientControlFunction->get_NAME() == clientControlFunction->get_NAME()) &&
			    (activeClient->clientControlFunction->get_can_port() == clientControlFunction->get_can_port()))
			{
				return activeClient;
			}
		}
		return nullptr;
	}

	bool TaskControllerServer::nack_process_data_command(std::shared_ptr<ControlFunction> clientControlFunction) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(AcknowledgementType::Negative),
				0xFF,
				0xFF,
				0xFF,
				clientControlFunction->get_address(),
				static_cast<std::uint8_t>(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData) & 0xFF),
				static_cast<std::uint8_t>((static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData) >> 8) & 0xFF),
				static_cast<std::uint8_t>((static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData) >> 16) & 0xFF)
			};

			LOG_WARNING("[TC Server]: NACKing process data command from 0x%02X because they are not known to us. Clients must send the working set master message first.", clientControlFunction->get_address());
			retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::Acknowledge),
			                                                        payload.data(),
			                                                        payload.size(),
			                                                        serverControlFunction,
			                                                        clientControlFunction);
		}
		return retVal;
	}

	bool TaskControllerServer::send_structure_label(std::shared_ptr<ControlFunction> clientControlFunction, std::vector<std::uint8_t> &structureLabel, const std::vector<std::uint8_t> &extendedStructureLabel) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			while (structureLabel.size() < CAN_DATA_LENGTH - 1)
			{
				structureLabel.push_back(0xFF);
			}

			std::vector<std::uint8_t> payload;
			payload.push_back(static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::StructureLabel) << 4);

			for (const auto &labelByte : structureLabel)
			{
				payload.push_back(labelByte);
			}
			for (const auto &extendedLabelByte : extendedStructureLabel)
			{
				payload.push_back(extendedLabelByte);
			}
			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_localization_label(std::shared_ptr<ControlFunction> clientControlFunction, const std::array<std::uint8_t, 7> &localizationLabel) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::LocalizationLabel) << 4,
				localizationLabel[0],
				localizationLabel[1],
				localizationLabel[2],
				localizationLabel[3],
				localizationLabel[4],
				localizationLabel[5],
				localizationLabel[6]
			};

			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_request_object_pool_transfer_response(std::shared_ptr<ControlFunction> clientControlFunction, bool isEnoughMemory) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::RequestObjectPoolTransferResponse) << 4,
				static_cast<std::uint8_t>(false == isEnoughMemory),
				0xFF,
				0xFF,
				0xFF,
				0xFF,
				0xFF,
				0xFF
			};

			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_object_pool_transfer_response(std::shared_ptr<ControlFunction> clientControlFunction, std::uint8_t errorBitfield, std::uint32_t sizeBytes) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::ObjectPoolTransferResponse) << 4,
				errorBitfield,
				static_cast<std::uint8_t>(sizeBytes),
				static_cast<std::uint8_t>(sizeBytes >> 8),
				static_cast<std::uint8_t>(sizeBytes >> 16),
				static_cast<std::uint8_t>(sizeBytes >> 24),
				0xFF,
				0xFF
			};

			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_object_pool_activate_deactivate_response(std::shared_ptr<ControlFunction> clientControlFunction,
	                                                                         std::uint8_t activationErrorBitfield,
	                                                                         std::uint8_t objectPoolErrorBitfield,
	                                                                         std::uint16_t parentOfFaultingObject,
	                                                                         std::uint16_t faultingObject) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::ObjectPoolActivateDeactivateResponse) << 4,
				activationErrorBitfield,
				static_cast<std::uint8_t>(parentOfFaultingObject),
				static_cast<std::uint8_t>(parentOfFaultingObject >> 8),
				static_cast<std::uint8_t>(faultingObject),
				static_cast<std::uint8_t>(faultingObject >> 8),
				objectPoolErrorBitfield,
				0xFF
			};
			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_delete_object_pool_response(std::shared_ptr<ControlFunction> clientControlFunction, bool deletionResult, std::uint8_t errorCode) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::DeleteObjectPoolResponse) << 4,
				static_cast<std::uint8_t>(!deletionResult), // 0 = No errors, 1 = Error
				errorCode,
				0xFF,
				0xFF,
				0xFF,
				0xFF,
				0xFF
			};
			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_change_designator_response(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t objectID, std::uint8_t errorCode) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(ProcessDataCommands::DeviceDescriptor) | static_cast<std::uint8_t>(DeviceDescriptorCommandParameters::ChangeDesignatorResponse) << 4,
				static_cast<std::uint8_t>(objectID),
				static_cast<std::uint8_t>(objectID >> 8),
				errorCode,
				0xFF,
				0xFF,
				0xFF,
				0xFF
			};
			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size());
		}
		return retVal;
	}

	bool TaskControllerServer::send_process_data_acknowledge(std::shared_ptr<ControlFunction> clientControlFunction, std::uint16_t dataDescriptionIndex, std::uint16_t elementNumber, std::uint8_t errorBitfield, ProcessDataCommands processDataCommand) const
	{
		bool retVal = false;

		if (nullptr != clientControlFunction)
		{
			const std::array<std::uint8_t, CAN_DATA_LENGTH> payload = {
				static_cast<std::uint8_t>(static_cast<std::uint8_t>(ProcessDataCommands::Acknowledge) | (static_cast<std::uint8_t>(elementNumber & 0x0F) << 4)),
				static_cast<std::uint8_t>(elementNumber >> 4),
				static_cast<std::uint8_t>(dataDescriptionIndex),
				static_cast<std::uint8_t>(dataDescriptionIndex >> 8),
				errorBitfield,
				static_cast<std::uint8_t>(0xF0 | static_cast<std::uint8_t>(processDataCommand)),
				0xFF,
				0xFF
			};
			retVal = send_process_data_to_client(clientControlFunction,
			                                     payload.data(),
			                                     payload.size(),
			                                     CANIdentifier::CANPriority::Priority4);
		}
		return retVal;
	}

	bool TaskControllerServer::send_process_data_to_client(std::shared_ptr<ControlFunction> clientControlFunction,
	                                                       const std::uint8_t *dataBuffer,
	                                                       std::uint32_t dataLength,
	                                                       CANIdentifier::CANPriority priority) const
	{
		bool retVal = false;

		if ((nullptr != dataBuffer) && (dataLength > 0))
		{
			retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProcessData),
			                                                        dataBuffer,
			                                                        dataLength,
			                                                        serverControlFunction,
			                                                        clientControlFunction,
			                                                        priority);
		}
		return retVal;
	}
}
