//================================================================================================
/// @file isobus_diagnostic_protocol.cpp
///
/// @brief A protocol that handles the ISO-11783 Active DTC Protocol.
/// @brief A protocol that handles the ISO 11783-12 Diagnostic Protocol.
/// @details This protocol manages many of the messages defined in ISO 11783-12
/// and a subset of the messages defined in SAE J1939-73.
/// The ISO-11783 definition of some of these is based on the J1939 definition with some tweaks.
/// You can select if you want the protocol to behave like J1939 by calling set_j1939_mode.
/// One of the messages this protocol supports is the DM1 message.
/// The DM1 is sent via BAM, which has some implications to your application,
/// as only 1 BAM can be active at a time. This message
/// is sent at 1 Hz. In ISOBUS mode, unlike in J1939, the message is discontinued when no DTCs are active to
/// minimize bus load. Also, ISO-11783 does not utilize or support lamp status.
/// Other messages this protocol supports include: DM2, DM3, DM11, DM22, software ID, and Product ID.
/// @author Adrian Del Grosso
///
/// @copyright 2022 Adrian Del Grosso
//================================================================================================

#include "isobus_diagnostic_protocol.hpp"

#include "can_general_parameter_group_numbers.hpp"
#include "can_network_manager.hpp"
#include "system_timing.hpp"

#include <algorithm>

namespace isobus
{
	std::list<DiagnosticProtocol *> DiagnosticProtocol::diagnosticProtocolList;

	DiagnosticProtocol::DiagnosticTroubleCode::DiagnosticTroubleCode() :
	  suspectParameterNumber(0xFFFFFFFF),
	  failureModeIdentifier(static_cast<std::uint8_t>(FailureModeIdentifier::ConditionExists)),
	  lampState(LampStatus::None)
	{
	}

	DiagnosticProtocol::DiagnosticTroubleCode::DiagnosticTroubleCode(std::uint32_t spn, FailureModeIdentifier fmi, LampStatus lamp) :
	  suspectParameterNumber(spn),
	  failureModeIdentifier(static_cast<std::uint8_t>(fmi)),
	  lampState(lamp)
	{
	}

	bool DiagnosticProtocol::DiagnosticTroubleCode::operator==(const DiagnosticTroubleCode &obj)
	{
		return ((suspectParameterNumber == obj.suspectParameterNumber) &&
		        (failureModeIdentifier == obj.failureModeIdentifier) &&
		        (lampState == obj.lampState));
	}

	std::uint8_t DiagnosticProtocol::DiagnosticTroubleCode::get_occurrance_count() const
	{
		return occuranceCount;
	}

	DiagnosticProtocol::DiagnosticProtocol(std::shared_ptr<InternalControlFunction> internalControlFunction) :
	  myControlFunction(internalControlFunction),
	  txFlags(static_cast<std::uint32_t>(TransmitFlags::NumberOfFlags), process_flags, this),
	  lastDM1SentTimestamp(0),
	  j1939Mode(false)
	{
		diagnosticProtocolList.push_back(this);
		ecuIdentificationFields.resize(static_cast<std::size_t>(ECUIdentificationFields::NumberOfFields));

		for (auto ecuIDField : ecuIdentificationFields)
		{
			ecuIDField = "*";
		}
	}

	DiagnosticProtocol::~DiagnosticProtocol()
	{
		auto protocolLocation = find(diagnosticProtocolList.begin(), diagnosticProtocolList.end(), this);

		if (diagnosticProtocolList.end() != protocolLocation)
		{
			diagnosticProtocolList.erase(protocolLocation);
		}

		if (initialized)
		{
			initialized = false;
			CANNetworkManager::CANNetwork.remove_protocol_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ParameterGroupNumberRequest), process_message, this);
			CANNetworkManager::CANNetwork.remove_protocol_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage22), process_message, this);
		}
	}

	bool DiagnosticProtocol::assign_diagnostic_protocol_to_internal_control_function(std::shared_ptr<InternalControlFunction> internalControlFunction)
	{
		bool retVal = true;

		for (auto protocolLocation : diagnosticProtocolList)
		{
			if (protocolLocation->myControlFunction == internalControlFunction)
			{
				retVal = false;
				break;
			}
		}

		if (retVal)
		{
			DiagnosticProtocol *newProtocol = new DiagnosticProtocol(internalControlFunction);
			diagnosticProtocolList.push_back(newProtocol);
		}
		return retVal;
	}

	bool DiagnosticProtocol::deassign_diagnostic_protocol_to_internal_control_function(std::shared_ptr<InternalControlFunction> internalControlFunction)
	{
		bool retVal = false;

		for (auto protocolLocation = diagnosticProtocolList.begin(); protocolLocation != diagnosticProtocolList.end(); protocolLocation++)
		{
			if ((*protocolLocation)->myControlFunction == internalControlFunction)
			{
				retVal = true;
				delete *protocolLocation;
				diagnosticProtocolList.erase(protocolLocation);
				break;
			}
		}
		return retVal;
	}

	DiagnosticProtocol *DiagnosticProtocol::get_diagnostic_protocol_by_internal_control_function(std::shared_ptr<InternalControlFunction> internalControlFunction)
	{
		DiagnosticProtocol *retVal = nullptr;
		for (auto protocol : diagnosticProtocolList)
		{
			if (protocol->myControlFunction == internalControlFunction)
			{
				retVal = protocol;
				break;
			}
		}
		return retVal;
	}

	void DiagnosticProtocol::initialize(CANLibBadge<CANNetworkManager>)
	{
		if (!initialized)
		{
			initialized = true;
			CANNetworkManager::CANNetwork.add_protocol_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ParameterGroupNumberRequest), process_message, this);
			CANNetworkManager::CANNetwork.add_protocol_parameter_group_number_callback(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage22), process_message, this);
		}
	}

	void DiagnosticProtocol::set_j1939_mode(bool value)
	{
		j1939Mode = value;
	}

	bool DiagnosticProtocol::get_j1939_mode() const
	{
		return j1939Mode;
	}

	void DiagnosticProtocol::clear_active_diagnostic_trouble_codes()
	{
		inactiveDTCList.insert(std::end(inactiveDTCList), std::begin(activeDTCList), std::end(activeDTCList));
		activeDTCList.clear();
		txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM1));
	}

	void DiagnosticProtocol::clear_inactive_diagnostic_trouble_codes()
	{
		inactiveDTCList.clear();
	}

	void DiagnosticProtocol::clear_software_id_fields()
	{
		softwareIdentificationFields.clear();
	}

	void DiagnosticProtocol::set_ecu_id_field(ECUIdentificationFields field, std::string value)
	{
		if (field <= ECUIdentificationFields::NumberOfFields)
		{
			ecuIdentificationFields[static_cast<std::size_t>(field)] = value + "*";
		}
	}

	bool DiagnosticProtocol::set_diagnostic_trouble_code_active(const DiagnosticTroubleCode &dtc, bool active)
	{
		bool retVal = false;

		if (active)
		{
			// First check to see if it's already active
			auto activeLocation = std::find(activeDTCList.begin(), activeDTCList.end(), dtc);

			if (activeDTCList.end() == activeLocation)
			{
				// Not already active. This is valid
				auto inactiveLocation = std::find(inactiveDTCList.begin(), inactiveDTCList.end(), dtc);

				if (inactiveDTCList.end() != inactiveLocation)
				{
					inactiveLocation->occuranceCount++;
					activeDTCList.push_back(*inactiveLocation);
					inactiveDTCList.erase(inactiveLocation);
				}
				else
				{
					activeDTCList.push_back(dtc);
					activeDTCList[activeDTCList.size() - 1].occuranceCount = 1;

					if (SystemTiming::get_time_elapsed_ms(lastDM1SentTimestamp) > DM_MAX_FREQUENCY_MS)
					{
						txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM1));
						lastDM1SentTimestamp = SystemTiming::get_timestamp_ms();
					}
				}
			}
			else
			{
				// Already active!
				retVal = false;
			}
		}
		else
		{
			/// First check to see if it's already in the inactive list
			auto inactiveLocation = std::find(inactiveDTCList.begin(), inactiveDTCList.end(), dtc);

			if (inactiveDTCList.end() == inactiveLocation)
			{
				auto activeLocation = std::find(activeDTCList.begin(), activeDTCList.end(), dtc);

				if (activeDTCList.end() != activeLocation)
				{
					inactiveDTCList.push_back(*activeLocation);
					activeDTCList.erase(activeLocation);
				}
			}
			else
			{
				// Already inactive!
				retVal = false;
			}
		}
		return retVal;
	}

	bool DiagnosticProtocol::get_diagnostic_trouble_code_active(const DiagnosticTroubleCode &dtc)
	{
		auto activeLocation = std::find(activeDTCList.begin(), activeDTCList.end(), dtc);
		bool retVal = false;

		if (activeDTCList.end() != activeLocation)
		{
			retVal = true;
		}
		return retVal;
	}

	bool DiagnosticProtocol::set_product_identification_code(std::string value)
	{
		bool retVal = false;

		if (value.size() < PRODUCT_IDENTIFICATION_MAX_STRING_LENGTH)
		{
			productIdentificationCode = value;
			retVal = true;
		}
		return retVal;
	}

	bool DiagnosticProtocol::set_product_identification_brand(std::string value)
	{
		bool retVal = false;

		if (value.size() < PRODUCT_IDENTIFICATION_MAX_STRING_LENGTH)
		{
			productIdentificationBrand = value;
			retVal = true;
		}
		return retVal;
	}

	bool DiagnosticProtocol::set_product_identification_model(std::string value)
	{
		bool retVal = false;

		if (value.size() < PRODUCT_IDENTIFICATION_MAX_STRING_LENGTH)
		{
			productIdentificationModel = value;
			retVal = true;
		}
		return retVal;
	}

	void DiagnosticProtocol::set_software_id_field(std::uint32_t index, std::string value)
	{
		if (index >= softwareIdentificationFields.size())
		{
			softwareIdentificationFields.resize(index + 1);
		}
		else if ("" == value)
		{
			if (index == softwareIdentificationFields.size())
			{
				softwareIdentificationFields.pop_back();
			}
		}
		softwareIdentificationFields[index] = value;
	}

	void DiagnosticProtocol::update(CANLibBadge<CANNetworkManager>)
	{
		if (j1939Mode)
		{
			if (SystemTiming::time_expired_ms(lastDM1SentTimestamp, DM_MAX_FREQUENCY_MS))
			{
				txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM1));
				lastDM1SentTimestamp = SystemTiming::get_timestamp_ms();
			}
		}
		else
		{
			if ((0 != activeDTCList.size()) &&
			    (SystemTiming::time_expired_ms(lastDM1SentTimestamp, DM_MAX_FREQUENCY_MS)))
			{
				txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM1));
				lastDM1SentTimestamp = SystemTiming::get_timestamp_ms();
			}
		}
		txFlags.process_all_flags();
	}

	std::uint8_t DiagnosticProtocol::convert_flash_state_to_byte(FlashState flash)
	{
		std::uint8_t retVal = 0;

		switch (flash)
		{
			case FlashState::Slow:
			{
				retVal = 0x00;
			}
			break;

			case FlashState::Fast:
			{
				retVal = 0x01;
			}
			break;

			case FlashState::Solid:
			default:
			{
				retVal = 0x03;
			}
			break;
		}
		return retVal;
	}

	void DiagnosticProtocol::get_active_list_lamp_state_and_flash_state(Lamps targetLamp, FlashState &flash, bool &lampOn)
	{
		flash = FlashState::Solid;
		lampOn = false;

		for (auto dtc : activeDTCList)
		{
			switch (targetLamp)
			{
				case Lamps::AmberWarningLamp:
				{
					if (dtc.lampState == LampStatus::AmberWarningLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::AmberWarningLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::AmberWarningLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
					}
				}
				break;

				case Lamps::MalfunctionIndicatorLamp:
				{
					if (dtc.lampState == LampStatus::MalfunctionIndicatorLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::MalfuctionIndicatorLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::MalfunctionIndicatorLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
					}
				}
				break;

				case Lamps::ProtectLamp:
				{
					if (dtc.lampState == LampStatus::EngineProtectLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::EngineProtectLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::EngineProtectLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
					}
				}
				break;

				case Lamps::RedStopLamp:
				{
					if (dtc.lampState == LampStatus::RedStopLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::RedStopLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::RedStopLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
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

	void DiagnosticProtocol::get_inactive_list_lamp_state_and_flash_state(Lamps targetLamp, FlashState &flash, bool &lampOn)
	{
		flash = FlashState::Solid;
		lampOn = false;

		for (auto dtc : inactiveDTCList)
		{
			switch (targetLamp)
			{
				case Lamps::AmberWarningLamp:
				{
					if (dtc.lampState == LampStatus::AmberWarningLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::AmberWarningLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::AmberWarningLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
					}
				}
				break;

				case Lamps::MalfunctionIndicatorLamp:
				{
					if (dtc.lampState == LampStatus::MalfunctionIndicatorLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::MalfuctionIndicatorLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::MalfunctionIndicatorLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
					}
				}
				break;

				case Lamps::ProtectLamp:
				{
					if (dtc.lampState == LampStatus::EngineProtectLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::EngineProtectLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::EngineProtectLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
					}
				}
				break;

				case Lamps::RedStopLamp:
				{
					if (dtc.lampState == LampStatus::RedStopLampSolid)
					{
						lampOn = true;
					}
					else if (dtc.lampState == LampStatus::RedStopLampSlowFlash)
					{
						lampOn = true;
						if (flash != FlashState::Fast)
						{
							flash = FlashState::Slow;
						}
					}
					else if (dtc.lampState == LampStatus::RedStopLampFastFlash)
					{
						lampOn = true;
						flash = FlashState::Fast;
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

	bool DiagnosticProtocol::protocol_transmit_message(std::uint32_t,
	                                                   const std::uint8_t *,
	                                                   std::uint32_t,
	                                                   ControlFunction *,
	                                                   ControlFunction *,
	                                                   TransmitCompleteCallback,
	                                                   void *,
	                                                   DataChunkCallback)
	{
		return false;
	}

	bool DiagnosticProtocol::send_diagnostic_message_1()
	{
		bool retVal = false;

		if (nullptr != myControlFunction)
		{
			std::uint16_t payloadSize = (activeDTCList.size() * DM_PAYLOAD_BYTES_PER_DTC) + 2; // 2 Bytes (0 and 1) are reserved
			std::vector<std::uint8_t> buffer;

			if (payloadSize <= MAX_PAYLOAD_SIZE_BYTES)
			{
				buffer.resize(payloadSize);

				if (get_j1939_mode())
				{
					bool tempLampState = false;
					FlashState tempLampFlashState = FlashState::Solid;
					get_active_list_lamp_state_and_flash_state(Lamps::ProtectLamp, tempLampFlashState, tempLampState);

					/// Encode Protect state and flash
					buffer[0] = tempLampState;
					buffer[1] = convert_flash_state_to_byte(tempLampFlashState);

					get_active_list_lamp_state_and_flash_state(Lamps::AmberWarningLamp, tempLampFlashState, tempLampState);

					/// Encode amber warning lamp state and flash
					buffer[0] |= (tempLampState << 2);
					buffer[1] |= (convert_flash_state_to_byte(tempLampFlashState) << 2);

					get_active_list_lamp_state_and_flash_state(Lamps::RedStopLamp, tempLampFlashState, tempLampState);

					/// Encode red stop lamp state and flash
					buffer[0] |= (tempLampState << 4);
					buffer[1] |= (convert_flash_state_to_byte(tempLampFlashState) << 4);

					get_active_list_lamp_state_and_flash_state(Lamps::MalfunctionIndicatorLamp, tempLampFlashState, tempLampState);

					/// Encode malfunction indicator lamp state and flash
					buffer[0] |= (tempLampState << 6);
					buffer[1] |= (convert_flash_state_to_byte(tempLampFlashState) << 6);
				}
				else
				{
					// ISO 11783 does not use lamp state or lamp flash bytes
					buffer[0] = 0xFF;
					buffer[1] = 0xFF;
				}

				if (0 == activeDTCList.size())
				{
					buffer[2] = 0x00;
					buffer[3] = 0x00;
					buffer[4] = 0x00;
					buffer[5] = 0x00;
					buffer[6] = 0xFF;
					buffer[7] = 0xFF;
					retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage1),
					                                                        buffer.data(),
					                                                        CAN_DATA_LENGTH,
					                                                        myControlFunction.get());
				}
				else
				{
					for (std::size_t i = 0; i < activeDTCList.size(); i++)
					{
						buffer[2 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = static_cast<std::uint8_t>(activeDTCList[i].suspectParameterNumber & 0xFF);
						buffer[3 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = static_cast<std::uint8_t>((activeDTCList[i].suspectParameterNumber >> 8) & 0xFF);
						buffer[4 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = ((static_cast<std::uint8_t>((activeDTCList[i].suspectParameterNumber >> 16) & 0xFF) << 5) | static_cast<std::uint8_t>(activeDTCList[i].failureModeIdentifier & 0x1F));
						buffer[5 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = (activeDTCList[i].occuranceCount & 0x7F);
					}

					if (payloadSize < CAN_DATA_LENGTH)
					{
						buffer[6] = 0xFF;
						buffer[7] = 0xFF;
						payloadSize = CAN_DATA_LENGTH;
					}

					retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage1),
					                                                        buffer.data(),
					                                                        payloadSize,
					                                                        myControlFunction.get());
				}
			}
		}
		return retVal;
	}

	bool DiagnosticProtocol::send_diagnostic_message_2()
	{
		bool retVal = false;

		if (nullptr != myControlFunction)
		{
			std::uint16_t payloadSize = (inactiveDTCList.size() * DM_PAYLOAD_BYTES_PER_DTC) + 2; // 2 Bytes (0 and 1) are reserved or used for lamp + flash
			std::vector<std::uint8_t> buffer;

			if (payloadSize <= MAX_PAYLOAD_SIZE_BYTES)
			{
				buffer.resize(payloadSize);

				if (get_j1939_mode())
				{
					bool tempLampState = false;
					FlashState tempLampFlashState = FlashState::Solid;
					get_inactive_list_lamp_state_and_flash_state(Lamps::ProtectLamp, tempLampFlashState, tempLampState);

					/// Encode Protect state and flash
					buffer[0] = tempLampState;
					buffer[1] = convert_flash_state_to_byte(tempLampFlashState);

					get_inactive_list_lamp_state_and_flash_state(Lamps::AmberWarningLamp, tempLampFlashState, tempLampState);

					/// Encode amber warning lamp state and flash
					buffer[0] |= (tempLampState << 2);
					buffer[1] |= (convert_flash_state_to_byte(tempLampFlashState) << 2);

					get_inactive_list_lamp_state_and_flash_state(Lamps::RedStopLamp, tempLampFlashState, tempLampState);

					/// Encode red stop lamp state and flash
					buffer[0] |= (tempLampState << 4);
					buffer[1] |= (convert_flash_state_to_byte(tempLampFlashState) << 4);

					get_inactive_list_lamp_state_and_flash_state(Lamps::MalfunctionIndicatorLamp, tempLampFlashState, tempLampState);

					/// Encode malfunction indicator lamp state and flash
					buffer[0] |= (tempLampState << 6);
					buffer[1] |= (convert_flash_state_to_byte(tempLampFlashState) << 6);
				}
				else
				{
					// ISO 11783 does not use lamp state or lamp flash bytes
					buffer[0] = 0xFF;
					buffer[1] = 0xFF;
				}

				if (0 == inactiveDTCList.size())
				{
					buffer[2] = 0x00;
					buffer[3] = 0x00;
					buffer[4] = 0x00;
					buffer[5] = 0x00;
					buffer[6] = 0xFF;
					buffer[7] = 0xFF;
					retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage2),
					                                                        buffer.data(),
					                                                        CAN_DATA_LENGTH,
					                                                        myControlFunction.get());
				}
				else
				{
					for (std::size_t i = 0; i < inactiveDTCList.size(); i++)
					{
						buffer[2 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = static_cast<std::uint8_t>(inactiveDTCList[i].suspectParameterNumber & 0xFF);
						buffer[3 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = static_cast<std::uint8_t>((inactiveDTCList[i].suspectParameterNumber >> 8) & 0xFF);
						buffer[4 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = ((static_cast<std::uint8_t>((inactiveDTCList[i].suspectParameterNumber >> 16) & 0xFF) << 5) | static_cast<std::uint8_t>(inactiveDTCList[i].failureModeIdentifier & 0x1F));
						buffer[5 + (DM_PAYLOAD_BYTES_PER_DTC * i)] = (inactiveDTCList[i].occuranceCount & 0x7F);
					}

					if (payloadSize < CAN_DATA_LENGTH)
					{
						buffer[6] = 0xFF;
						buffer[7] = 0xFF;
						payloadSize = CAN_DATA_LENGTH;
					}

					retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage2),
					                                                        buffer.data(),
					                                                        payloadSize,
					                                                        myControlFunction.get());
				}
			}
		}
		return retVal;
	}

	bool DiagnosticProtocol::send_diagnostic_message_3_ack(ControlFunction *destination)
	{
		std::array<std::uint8_t, CAN_DATA_LENGTH> buffer;
		bool retVal = false;

		// "A positive or negative acknowledgement is not required in response to a global request."
		if (nullptr != destination)
		{
			constexpr std::uint32_t DM3PGN = static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage3);

			buffer[0] = 0x00; // Positive acknowledgement
			buffer[1] = 0xFF; // Group function value
			buffer[2] = 0xFF; // Reserved
			buffer[3] = 0xFF; // Reserved
			buffer[4] = destination->get_address(); // Address acknowledged
			buffer[5] = (DM3PGN & 0xFF); // PGN LSB
			buffer[6] = ((DM3PGN >> 8) & 0xFF); // PGN middle byte
			buffer[7] = ((DM3PGN >> 16) & 0xFF); // PGN MSB

			retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::Acknowledge),
			                                                        buffer.data(),
			                                                        CAN_DATA_LENGTH,
			                                                        myControlFunction.get(),
			                                                        nullptr);
		}
		return retVal;
	}

	bool DiagnosticProtocol::send_diagnostic_message_11_ack(ControlFunction *destination)
	{
		std::array<std::uint8_t, CAN_DATA_LENGTH> buffer;
		bool retVal = false;

		// "A positive or negative acknowledgement is not required in response to a global request."
		if (nullptr != destination)
		{
			constexpr std::uint32_t DM11PGN = static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage11);

			buffer[0] = 0x00; // Positive acknowledgement
			buffer[1] = 0xFF; // Group function value
			buffer[2] = 0xFF; // Reserved
			buffer[3] = 0xFF; // Reserved
			buffer[4] = destination->get_address(); // Address acknowledged
			buffer[5] = (DM11PGN & 0xFF); // PGN LSB
			buffer[6] = ((DM11PGN >> 8) & 0xFF); // PGN middle byte
			buffer[7] = ((DM11PGN >> 16) & 0xFF); // PGN MSB

			retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::Acknowledge),
			                                                        buffer.data(),
			                                                        CAN_DATA_LENGTH,
			                                                        myControlFunction.get(),
			                                                        nullptr);
		}
		return retVal;
	}

	bool DiagnosticProtocol::send_diagnostic_protocol_identification()
	{
		bool retVal = false;

		if (nullptr != myControlFunction)
		{
			/// Bit 1 = J1939-73, Bit 2 = ISO 14230, Bit 3 = ISO 15765-3, else Reserved
			constexpr std::uint8_t SUPPORTED_DIAGNOSTIC_PROTOCOLS_BITFIELD = 0x01;
			std::array<std::uint8_t, CAN_DATA_LENGTH> buffer;

			buffer.fill(0xFF); // Reserved bytes
			buffer[0] = SUPPORTED_DIAGNOSTIC_PROTOCOLS_BITFIELD;
			retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticProtocolIdentification),
			                                                        buffer.data(),
			                                                        CAN_DATA_LENGTH,
			                                                        myControlFunction.get());
		}
		return retVal;
	}

	bool DiagnosticProtocol::send_ecu_identification()
	{
		std::string ecuIdString = "";

		for (auto stringComponent : ecuIdentificationFields)
		{
			ecuIdString.append(stringComponent);
		}

		std::vector<std::uint8_t> buffer(ecuIdString.begin(), ecuIdString.end());
		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ECUIdentificationInformation),
		                                                      buffer.data(),
		                                                      buffer.size(),
		                                                      myControlFunction.get());
	}

	bool DiagnosticProtocol::send_product_identification()
	{
		std::string productIdString = productIdentificationCode + "*" + productIdentificationBrand + "*" + productIdentificationModel + "*";
		std::vector<std::uint8_t> buffer(productIdString.begin(), productIdString.end());

		return CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProductIdentification),
		                                                      buffer.data(),
		                                                      buffer.size(),
		                                                      myControlFunction.get());
	}

	bool DiagnosticProtocol::send_software_identification()
	{
		bool retVal = false;

		if (0 != softwareIdentificationFields.size())
		{
			std::string softIDString = "";

			for (auto softIdString = softwareIdentificationFields.begin(); softIdString != softwareIdentificationFields.end(); softIdString++)
			{
				softIDString.append(*softIdString);
				softIDString.append("*");
			}

			std::vector<std::uint8_t> buffer(softIDString.begin(), softIDString.end());
			retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::SoftwareIdentification),
			                                                        buffer.data(),
			                                                        buffer.size(),
			                                                        myControlFunction.get());
		}
		return retVal;
	}

	bool DiagnosticProtocol::process_all_dm22_responses()
	{
		bool retVal = false;

		if (0 != dm22ResponseQueue.size())
		{
			std::size_t numberOfMessage = dm22ResponseQueue.size();

			for (std::size_t i = 0; i < numberOfMessage; i++)
			{
				std::array<std::uint8_t, CAN_DATA_LENGTH> buffer;
				DM22Data currentMessageData = dm22ResponseQueue.back();

				if (currentMessageData.nack)
				{
					if (currentMessageData.clearActive)
					{
						buffer[0] = static_cast<std::uint8_t>(DM22ControlByte::NegativeAcknowledgeOfActiveDTCClear);
						buffer[1] = currentMessageData.nackIndicator;
					}
					else
					{
						buffer[0] = static_cast<std::uint8_t>(DM22ControlByte::NegativeAcknowledgeOfPreviouslyActiveDTCClear);
						buffer[1] = currentMessageData.nackIndicator;
					}
				}
				else
				{
					if (currentMessageData.clearActive)
					{
						buffer[0] = static_cast<std::uint8_t>(DM22ControlByte::PositiveAcknowledgeOfActiveDTCClear);
						buffer[1] = 0xFF;
					}
					else
					{
						buffer[0] = static_cast<std::uint8_t>(DM22ControlByte::PositiveAcknowledgeOfPreviouslyActiveDTCClear);
						buffer[1] = 0xFF;
					}
				}

				buffer[2] = 0xFF;
				buffer[3] = 0xFF;
				buffer[4] = 0xFF;
				buffer[5] = (currentMessageData.suspectParameterNumber & 0xFF);
				buffer[6] = ((currentMessageData.suspectParameterNumber >> 8) & 0xFF);
				buffer[7] = (((currentMessageData.suspectParameterNumber >> 16) << 5) & 0xFF);
				buffer[7] |= (currentMessageData.failureModeIdentifier & 0x07);

				retVal = CANNetworkManager::CANNetwork.send_can_message(static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage22),
				                                                        buffer.data(),
				                                                        buffer.size(),
				                                                        myControlFunction.get(),
				                                                        currentMessageData.destination);
				if (retVal)
				{
					dm22ResponseQueue.pop_back();
				}
			}
		}
		return retVal;
	}

	void DiagnosticProtocol::process_message(CANMessage *const message)
	{
		if (nullptr != message)
		{
			switch (message->get_identifier().get_parameter_group_number())
			{
				case static_cast<std::uint32_t>(CANLibParameterGroupNumber::ParameterGroupNumberRequest):
				{
					if (3 == message->get_data_length()) /// PGN requests DLC is 3
					{
						std::vector<std::uint8_t> messageData = message->get_data();
						std::uint32_t requestedPGN = static_cast<std::uint32_t>(messageData.at(0));
						requestedPGN |= (static_cast<std::uint32_t>(messageData.at(1)) << 8);
						requestedPGN |= (static_cast<std::uint32_t>(messageData.at(2)) << 16);

						switch (requestedPGN)
						{
							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage2):
							{
								txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM2));
							}
							break;

							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage3):
							{
								clear_inactive_diagnostic_trouble_codes();
								send_diagnostic_message_3_ack(message->get_source_control_function());
							}
							break;

							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage11):
							{
								clear_active_diagnostic_trouble_codes();
								send_diagnostic_message_11_ack(message->get_source_control_function());
							}
							break;

							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::ProductIdentification):
							{
								txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::ProductIdentification));
							}
							break;

							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticProtocolIdentification):
							{
								txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DiagnosticProtocolID));
							}
							break;

							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::SoftwareIdentification):
							{
								send_software_identification();
							}
							break;

							case static_cast<std::uint32_t>(CANLibParameterGroupNumber::ECUIdentificationInformation):
							{
								send_ecu_identification();
							}
							break; 

							default:
							{
								// This PGN request is not handled by the diagnostic protocol
							}
							break;
						}
					}
				}
				break;

				case static_cast<std::uint32_t>(CANLibParameterGroupNumber::DiagnosticMessage22):
				{
					if (CAN_DATA_LENGTH == message->get_data_length())
					{
						auto messageData = message->get_data();

						DM22Data tempDM22Data;
						bool wasDTCCleared = false;

						tempDM22Data.suspectParameterNumber = messageData.at(5);
						tempDM22Data.suspectParameterNumber |= (messageData.at(6) << 8);
						tempDM22Data.suspectParameterNumber |= (((messageData.at(7) & 0xE0) >> 5) << 16);
						tempDM22Data.failureModeIdentifier = (messageData.at(7) & 0x1F);
						tempDM22Data.destination = message->get_source_control_function();
						tempDM22Data.nackIndicator = 0;
						tempDM22Data.clearActive = false;

						switch (messageData.at(0))
						{
							case static_cast<std::uint8_t>(DM22ControlByte::RequestToClearActiveDTC):
							{
								tempDM22Data.clearActive = true;

								for (auto dtc = activeDTCList.begin(); dtc != activeDTCList.end(); dtc++)
								{
									if ((tempDM22Data.suspectParameterNumber == dtc->suspectParameterNumber) && 
										(tempDM22Data.failureModeIdentifier == dtc->failureModeIdentifier))
									{
										inactiveDTCList.push_back(*dtc);
										activeDTCList.erase(dtc);
										wasDTCCleared = true;
										tempDM22Data.nack = false;
										
										dm22ResponseQueue.push_back(tempDM22Data);
										txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM22));
										break;
									}
								}

								if (!wasDTCCleared)
								{
									tempDM22Data.nack = true;

									// Since we didn't find the DTC in the active list, we check the inactive to determine the proper NACK reason
									for (auto dtc : inactiveDTCList)
									{
										if ((tempDM22Data.suspectParameterNumber == dtc.suspectParameterNumber) &&
										    (tempDM22Data.failureModeIdentifier == dtc.failureModeIdentifier))
										{
											// The DTC was active, but is inactive now, so we NACK with the proper reason
											tempDM22Data.nackIndicator = static_cast<std::uint8_t>(DM22NegativeAcknowledgeIndicator::DTCNoLongerActive);
											break;
										}
									}


									if (0 != tempDM22Data.nackIndicator)
									{
										// DTC is in neither list. NACK with the reason that we don't know anything about it
										tempDM22Data.nackIndicator = static_cast<std::uint8_t>(DM22NegativeAcknowledgeIndicator::UnknownOrDoesNotExist);
									}
									dm22ResponseQueue.push_back(tempDM22Data);
									txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM22));
								}
							}
							break;

							case static_cast<std::uint8_t>(DM22ControlByte::RequestToClearPreviouslyActiveDTC):
							{
								for (auto dtc = inactiveDTCList.begin(); dtc != inactiveDTCList.end(); dtc++)
								{
									if ((tempDM22Data.suspectParameterNumber == dtc->suspectParameterNumber) &&
									    (tempDM22Data.failureModeIdentifier == dtc->failureModeIdentifier))
									{
										inactiveDTCList.erase(dtc);
										wasDTCCleared = true;
										tempDM22Data.nack = false;

										dm22ResponseQueue.push_back(tempDM22Data);
										txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM22));
										break;
									}
								}

								if (!wasDTCCleared)
								{
									tempDM22Data.nack = true;

									// Since we didn't find the DTC in the inactive list, we check the active to determine the proper NACK reason
									for (auto dtc : activeDTCList)
									{
										if ((tempDM22Data.suspectParameterNumber == dtc.suspectParameterNumber) &&
										    (tempDM22Data.failureModeIdentifier == dtc.failureModeIdentifier))
										{
											// The DTC was inactive, but is active now, so we NACK with the proper reason
											tempDM22Data.nackIndicator = static_cast<std::uint8_t>(DM22NegativeAcknowledgeIndicator::DTCUNoLongerPreviouslyActive);
											break;
										}
									}

									if (0 != tempDM22Data.nackIndicator)
									{
										// DTC is in neither list. NACK with the reason that we don't know anything about it
										tempDM22Data.nackIndicator = static_cast<std::uint8_t>(DM22NegativeAcknowledgeIndicator::UnknownOrDoesNotExist);
									}
									dm22ResponseQueue.push_back(tempDM22Data);
									txFlags.set_flag(static_cast<std::uint32_t>(TransmitFlags::DM22));
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
				break;

				default:
				{
				}
				break;
			}
		}
	}

	void DiagnosticProtocol::process_message(CANMessage *const message, void *parent)
	{
		if (nullptr != parent)
		{
			reinterpret_cast<DiagnosticProtocol *>(parent)->process_message(message);
		}
	}

	void DiagnosticProtocol::process_flags(std::uint32_t flag, void *parentPointer)
	{
		if (nullptr != parentPointer)
		{
			DiagnosticProtocol *parent = reinterpret_cast<DiagnosticProtocol *>(parentPointer);
			bool transmitSuccessful = false;

			switch (flag)
			{
				case static_cast<std::uint32_t>(TransmitFlags::DM1):
				{
					transmitSuccessful = parent->send_diagnostic_message_1();

					if (transmitSuccessful)
					{
						parent->lastDM1SentTimestamp = SystemTiming::get_timestamp_ms();
					}
				}
				break;

				case static_cast<std::uint32_t>(TransmitFlags::DM2):
				{
					transmitSuccessful = parent->send_diagnostic_message_2();
				}
				break;

				case static_cast<std::uint32_t>(TransmitFlags::DiagnosticProtocolID):
				{
					transmitSuccessful = parent->send_diagnostic_protocol_identification();
				}
				break;

				case static_cast<std::uint32_t>(TransmitFlags::ProductIdentification):
				{
					transmitSuccessful = parent->send_product_identification();
				}
				break;

				case static_cast<std::uint32_t>(TransmitFlags::DM22):
				{
					transmitSuccessful = parent->process_all_dm22_responses();
				}
				break;

				default:
				{
				}
				break;
			}

			if (false == transmitSuccessful)
			{
				parent->txFlags.set_flag(flag);
			}
		}
	}

} // namespace isobus
