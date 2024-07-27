//================================================================================================
/// @file vt_application.cpp
///
/// @brief This is the implementation of the VT portion of the seeder example
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#include "vt_application.hpp"

#include "isobus/isobus/can_network_manager.hpp"
#include "isobus/isobus/isobus_standard_data_description_indices.hpp"
#include "isobus/utility/iop_file_interface.hpp"
#include "isobus/utility/system_timing.hpp"
#include "object_pool.hpp"

#include <cassert>
#include <iostream>

SeederVtApplication::SeederVtApplication(std::shared_ptr<isobus::PartneredControlFunction> VTPartner, std::shared_ptr<isobus::PartneredControlFunction> TCPartner, std::shared_ptr<isobus::InternalControlFunction> source) :
  TCClientInterface(TCPartner, source, nullptr),
  VTClientInterface(std::make_shared<isobus::VirtualTerminalClient>(VTPartner, source)),
  VTClientUpdateHelper(VTClientInterface),
  sectionControl(NUMBER_ONSCREEN_SECTIONS),
  speedMessages(source, false, false, false, false)
{
	alarms[AlarmType::NoMachineSpeed] = Alarm(10000); // 10 seconds
	alarms[AlarmType::NoTaskController] = Alarm(30000); // 30 seconds, TC can take a while to connect
}

bool SeederVtApplication::initialize()
{
	objectPool = isobus::IOPFileInterface::read_iop_file("BasePool.iop");

	if (objectPool.empty())
	{
		std::cout << "Failed to load object pool from BasePool.iop" << std::endl;
		return false;
	}
	std::cout << "Loaded object pool from BasePool.iop" << std::endl;

	// Generate a unique version string for this object pool (this is optional, and is entirely application specific behavior)
	std::string objectPoolHash = isobus::IOPFileInterface::hash_object_pool_to_version(objectPool);

	VTClientInterface->set_object_pool(0, objectPool.data(), static_cast<std::uint32_t>(objectPool.size()), objectPoolHash);
	VTClientInterface->get_vt_soft_key_event_dispatcher().add_listener([this](const isobus::VirtualTerminalClient::VTKeyEvent &event) { this->handle_vt_key_events(event); });
	VTClientInterface->get_vt_button_event_dispatcher().add_listener([this](const isobus::VirtualTerminalClient::VTKeyEvent &event) { this->handle_vt_key_events(event); });
	VTClientInterface->get_vt_change_numeric_value_event_dispatcher().add_listener([this](const isobus::VirtualTerminalClient::VTChangeNumericValueEvent &event) { this->handle_numeric_value_events(event); });
	VTClientInterface->initialize(true);

	// Track the numeric values we want to update
	VTClientUpdateHelper.add_tracked_numeric_value(enableAlarms_VarNum, true);
	VTClientUpdateHelper.add_tracked_numeric_value(autoManual_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(statisticsSelection_VarNum, 1);
	VTClientUpdateHelper.add_tracked_numeric_value(selectedStatisticsContainer_ObjPtr, canStatistics_Container);
	VTClientUpdateHelper.add_tracked_numeric_value(canAddress_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(utAddress_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(busload_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(speedUnits_ObjPtr, unitKph_OutStr);
	VTClientUpdateHelper.add_tracked_numeric_value(tcAddress_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(tcNumberBoomsSupported_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(tcControlChannels_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(tcSupportedSections_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(tcVersion_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(section1EnableState_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(section2EnableState_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(section3EnableState_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(section4EnableState_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(section5EnableState_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(section6EnableState_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(currentSpeedMeter_VarNum, 16);
	VTClientUpdateHelper.add_tracked_numeric_value(currentSpeedReadout_VarNum, 16);
	VTClientUpdateHelper.add_tracked_numeric_value(utVersion_VarNum);
	VTClientUpdateHelper.add_tracked_numeric_value(currentAlarms1_ObjPtr);
	VTClientUpdateHelper.add_tracked_numeric_value(currentAlarms2_ObjPtr);

	// Track the attribute values we want to update
	VTClientUpdateHelper.add_tracked_attribute(speed_OutNum, 8, 0.0036f);
	VTClientUpdateHelper.add_tracked_attribute(section1Status_OutRect, 5, solidGreen_FillAttr);
	VTClientUpdateHelper.add_tracked_attribute(section2Status_OutRect, 5, solidYellow_FillAttr);
	VTClientUpdateHelper.add_tracked_attribute(section3Status_OutRect, 5, solidRed_FillAttr);
	VTClientUpdateHelper.add_tracked_attribute(section4Status_OutRect, 5, solidRed_FillAttr);
	VTClientUpdateHelper.add_tracked_attribute(section5Status_OutRect, 5, solidYellow_FillAttr);
	VTClientUpdateHelper.add_tracked_attribute(section6Status_OutRect, 5, solidGreen_FillAttr);

	VTClientUpdateHelper.initialize();

	// Update the objects to their initial state, we should try to minimize this
	VTClientUpdateHelper.set_numeric_value(currentSpeedMeter_VarNum, 0);
	VTClientUpdateHelper.set_numeric_value(currentSpeedReadout_VarNum, 0);
	VTClientUpdateHelper.set_numeric_value(autoManual_ObjPtr, sectionControl.get_is_mode_auto() ? autoMode_Container : manualMode_Container);
	for (std::uint8_t i = 0; i < NUMBER_ONSCREEN_SECTIONS; ++i)
	{
		update_section_objects(i);
	}

	speedMessages.initialize();
	speedMessages.get_machine_selected_speed_data_event_publisher().add_listener([this](const std::shared_ptr<isobus::SpeedMessagesInterface::MachineSelectedSpeedData> mssData, bool changed) { this->handle_machine_selected_speed(mssData, changed); });
	speedMessages.get_ground_based_machine_speed_data_event_publisher().add_listener([this](const std::shared_ptr<isobus::SpeedMessagesInterface::GroundBasedSpeedData> gbsData, bool changed) { this->handle_ground_based_speed(gbsData, changed); });
	speedMessages.get_wheel_based_machine_speed_data_event_publisher().add_listener([this](const std::shared_ptr<isobus::SpeedMessagesInterface::WheelBasedMachineSpeedData> wbsData, bool changed) { this->handle_wheel_based_speed(wbsData, changed); });

	ddop = std::make_shared<isobus::DeviceDescriptorObjectPool>(3);
	if (sectionControl.create_ddop(ddop, TCClientInterface.get_internal_control_function()->get_NAME()))
	{
		TCClientInterface.configure(ddop, 1, 255, 255, true, true, true, false, true);
		TCClientInterface.add_request_value_callback(SectionControlImplementSimulator::request_value_command_callback, &sectionControl);
		TCClientInterface.add_value_command_callback(SectionControlImplementSimulator::value_command_callback, &sectionControl);
		TCClientInterface.add_default_process_data_requested_callback(SectionControlImplementSimulator::default_process_data_request_callback, &sectionControl);
		TCClientInterface.initialize(true);
	}
	else
	{
		std::cout << "Failed generating DDOP. TC functionality will not work until the DDOP structure is fixed." << std::endl;
		return false;
	}
	return true;
}

void SeederVtApplication::handle_vt_key_events(const isobus::VirtualTerminalClient::VTKeyEvent &event)
{
	if (event.keyNumber == 0)
	{
		// We have the alarm ACK code, so let's check if an alarm is active
		for (auto &alarm : alarms)
		{
			if (alarm.second.is_active())
			{
				alarm.second.acknowledge();
				update_alarms();
				break;
			}
		}
	}

	if (isobus::VirtualTerminalClient::KeyActivationCode::ButtonUnlatchedOrReleased == event.keyEvent)
	{
		switch (event.objectID)
		{
			case home_Key:
			{
				VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, mainRunscreen_DataMask);
			}
			break;

			case settings_Key:
			{
				VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, settingsRunscreen_DataMask);
			}
			break;

			case statistics_Key:
			{
				VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, statisticsRunscreen_DataMask);
			}
			break;

			case alarms_Key:
			{
				VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, alarmsRunscreen_DataMask);
			}
			break;

			case acknowledgeAlarm_SoftKey:
			{
				// Acknowledge the first active alarm
				for (auto &alarm : alarms)
				{
					if (alarm.second.is_active())
					{
						alarm.second.acknowledge();
						update_alarms();
						break;
					}
				}
				break;
			}
			break;

			case autoManualToggle_Button:
			{
				sectionControl.set_is_mode_auto(!sectionControl.get_is_mode_auto());
				VTClientUpdateHelper.set_numeric_value(autoManual_ObjPtr, sectionControl.get_is_mode_auto() ? autoMode_Container : manualMode_Container);
				for (std::uint8_t i = 0; i < NUMBER_ONSCREEN_SECTIONS; ++i)
				{
					update_section_objects(i);
				}
				TCClientInterface.on_value_changed_trigger(static_cast<std::uint16_t>(SectionControlImplementSimulator::ImplementDDOPElementNumbers::BoomElement),
				                                           static_cast<std::uint16_t>(isobus::DataDescriptionIndex::RequestDefaultProcessData));
			}
			break;

			case section1Toggle_Button:
			{
				toggle_section(0);
			}
			break;

			case section2Toggle_Button:
			{
				toggle_section(1);
			}
			break;

			case section3Toggle_Button:
			{
				toggle_section(2);
			}
			break;

			case section4Toggle_Button:
			{
				toggle_section(3);
			}
			break;

			case section5Toggle_Button:
			{
				toggle_section(4);
			}
			break;

			case section6Toggle_Button:
			{
				toggle_section(5);
			}
			break;

			default:
				break;
		}
	}
}

void SeederVtApplication::handle_numeric_value_events(const isobus::VirtualTerminalClient::VTChangeNumericValueEvent &event)
{
	switch (event.objectID)
	{
		case statisticsSelection_VarNum:
		{
			// Update the frame to show the newly selected statistic
			std::uint16_t targetContainer;
			switch (event.value)
			{
				case 1:
				{
					targetContainer = canStatistics_Container;
				}
				break;

				case 2:
				{
					targetContainer = utStatistics_Container;
				}
				break;

				case 3:
				{
					targetContainer = tcStatistics_Container;
				}
				break;

				case 4:
				{
					targetContainer = credits_Container;
				}
				break;

				default:
				{
					targetContainer = UNDEFINED;
				}
				break;
			}
			VTClientUpdateHelper.set_numeric_value(selectedStatisticsContainer_ObjPtr, targetContainer);
		}
		break;

		default:
			break;
	}
}

void SeederVtApplication::handle_machine_selected_speed(const std::shared_ptr<isobus::SpeedMessagesInterface::MachineSelectedSpeedData> mssData, bool)
{
	process_new_speed(SpeedSources::MachineSelected, mssData->get_machine_speed());
}

void SeederVtApplication::handle_ground_based_speed(const std::shared_ptr<isobus::SpeedMessagesInterface::GroundBasedSpeedData> gbsData, bool)
{
	process_new_speed(SpeedSources::GroundBased, gbsData->get_machine_speed());
}

void SeederVtApplication::handle_wheel_based_speed(const std::shared_ptr<isobus::SpeedMessagesInterface::WheelBasedMachineSpeedData> wbsData, bool)
{
	process_new_speed(SpeedSources::WheelBased, wbsData->get_machine_speed());
}

void SeederVtApplication::process_new_speed(SpeedSources source, std::uint32_t speed)
{
	bool shouldConsumeThisSpeed = false;

	if (source == SpeedSources::MachineSelected)
	{
		shouldConsumeThisSpeed = true; // Best speed source.
	}
	else if ((SpeedSources::GroundBased == source) && (0 == speedMessages.get_number_received_machine_selected_speed_sources()))
	{
		shouldConsumeThisSpeed = true; // Second best speed source.
	}
	else if ((SpeedSources::WheelBased == source) &&
	         (0 == speedMessages.get_number_received_machine_selected_speed_sources()) &&
	         (0 == speedMessages.get_number_received_ground_based_speed_sources()))
	{
		shouldConsumeThisSpeed = true; // Third best speed source.
	}

	if (shouldConsumeThisSpeed)
	{
		update_speedometer_objects(speed);
	}
}

void SeederVtApplication::update()
{
	// Update some polled data or other things that don't need as frequent updates
	if (isobus::SystemTiming::time_expired_ms(slowUpdateTimestamp_ms, 1000))
	{
		auto VTClientControlFunction = VTClientInterface->get_internal_control_function();
		auto VTControlFunction = VTClientInterface->get_partner_control_function();
		auto TCControlFunction = TCClientInterface.get_partner_control_function();

		if (nullptr != VTClientControlFunction)
		{
			// These are used for displaying to the user. Address is not really needed to be known.
			VTClientUpdateHelper.set_numeric_value(canAddress_VarNum, VTClientControlFunction->get_address());
			VTClientUpdateHelper.set_numeric_value(utAddress_VarNum, VTControlFunction->get_address());
			if (!languageDataRequested)
			{
				languageDataRequested = VTClientInterface->languageCommandInterface.send_request_language_command();
			}
		}
		if (get_is_object_shown(busload_VarNum))
		{
			auto busload = isobus::CANNetworkManager::CANNetwork.get_estimated_busload(0);
			VTClientUpdateHelper.set_numeric_value(busload_VarNum, static_cast<std::uint32_t>(busload * 100.0f));
		}
		update_ut_version_objects(VTClientInterface->get_connected_vt_version());

		if (isobus::LanguageCommandInterface::DistanceUnits::ImperialUS == VTClientInterface->languageCommandInterface.get_commanded_distance_units())
		{
			VTClientUpdateHelper.set_attribute(speed_OutNum, 8, 0.0022369363f); // mm/s to mph
			VTClientUpdateHelper.set_numeric_value(speedUnits_ObjPtr, unitMph_OutStr);
		}
		else
		{
			VTClientUpdateHelper.set_attribute(speed_OutNum, 8, 0.0036f); // mm/s to kph
			VTClientUpdateHelper.set_numeric_value(speedUnits_ObjPtr, unitKph_OutStr);
		}
		VTClientUpdateHelper.set_numeric_value(tcAddress_VarNum, TCClientInterface.get_partner_control_function()->get_address());
		VTClientUpdateHelper.set_numeric_value(tcNumberBoomsSupported_VarNum, TCClientInterface.get_connected_tc_number_booms_supported());
		VTClientUpdateHelper.set_numeric_value(tcControlChannels_VarNum, TCClientInterface.get_connected_tc_number_channels_supported());
		VTClientUpdateHelper.set_numeric_value(tcSupportedSections_VarNum, TCClientInterface.get_connected_tc_number_sections_supported());
		VTClientUpdateHelper.set_numeric_value(tcVersion_VarNum, static_cast<std::uint32_t>(TCClientInterface.get_connected_tc_version()));

		if ((0 == speedMessages.get_number_received_machine_selected_speed_command_sources()) &&
		    (0 == speedMessages.get_number_received_ground_based_speed_sources()) &&
		    (0 == speedMessages.get_number_received_wheel_based_speed_sources()))
		{
			update_speedometer_objects(0);
		}

		update_alarms();
		slowUpdateTimestamp_ms = isobus::SystemTiming::get_timestamp_ms();
	}
	speedMessages.update();
	for (std::uint8_t i = 0; i < NUMBER_ONSCREEN_SECTIONS; ++i)
	{
		update_section_objects(i);
	}
	VTClientUpdateHelper.set_numeric_value(autoManual_ObjPtr, sectionControl.get_is_mode_auto() ? autoMode_Container : manualMode_Container);
}

void SeederVtApplication::toggle_section(std::uint8_t sectionIndex)
{
	sectionControl.set_section_switch_state(sectionIndex, !sectionControl.get_section_switch_state(sectionIndex));
	TCClientInterface.on_value_changed_trigger(2, static_cast<std::uint16_t>(isobus::DataDescriptionIndex::ActualCondensedWorkState1_16));
	update_section_objects(sectionIndex);
}

void SeederVtApplication::update_section_objects(std::uint8_t sectionIndex)
{
	std::uint16_t newObject = offButtonSliderSmall_OutPict;
	if (sectionControl.get_section_switch_state(sectionIndex))
	{
		newObject = onButtonSliderSmall_OutPict;
	}

	std::uint32_t fillAttribute = solidRed_FillAttr;
	if (sectionControl.get_section_actual_state(sectionIndex))
	{
		fillAttribute = solidGreen_FillAttr;
	}
	else if (sectionControl.get_section_setpoint_state(sectionIndex))
	{
		fillAttribute = solidYellow_FillAttr;
	}
	else
	{
		fillAttribute = solidRed_FillAttr;
	}

	std::uint16_t switchPointerId = UNDEFINED;
	std::uint16_t statusRectangleId = UNDEFINED;
	switch (sectionIndex)
	{
		case 0:
		{
			switchPointerId = section1EnableState_ObjPtr;
			statusRectangleId = section1Status_OutRect;
		}
		break;

		case 1:
		{
			switchPointerId = section2EnableState_ObjPtr;
			statusRectangleId = section2Status_OutRect;
		}
		break;

		case 2:
		{
			switchPointerId = section3EnableState_ObjPtr;
			statusRectangleId = section3Status_OutRect;
		}
		break;

		case 3:
		{
			switchPointerId = section4EnableState_ObjPtr;
			statusRectangleId = section4Status_OutRect;
		}
		break;

		case 4:
		{
			switchPointerId = section5EnableState_ObjPtr;
			statusRectangleId = section5Status_OutRect;
		}
		break;

		case 5:
		{
			switchPointerId = section6EnableState_ObjPtr;
			statusRectangleId = section6Status_OutRect;
		}
		break;

		default:
			break;
	}
	VTClientUpdateHelper.set_numeric_value(switchPointerId, newObject);
	VTClientUpdateHelper.set_attribute(statusRectangleId, 5, fillAttribute); // 5 Is the attribute ID of the fill attribute
}

void SeederVtApplication::update_speedometer_objects(std::uint32_t speed)
{
	if (get_is_object_shown(currentSpeedReadout_VarNum))
	{
		VTClientUpdateHelper.set_numeric_value(currentSpeedReadout_VarNum, speed);
	}

	// The meter uses a fixed max of "30", so we'll have to do some scaling ourselves
	switch (VTClientInterface->languageCommandInterface.get_commanded_distance_units())
	{
		case isobus::LanguageCommandInterface::DistanceUnits::Metric:
		{
			// Scale to KPH
			speed = static_cast<std::uint32_t>((speed * 0.001f) * 3.6f); // Converting mm/s to m/s, then mm/s to kph
		}
		break;

		case isobus::LanguageCommandInterface::DistanceUnits::ImperialUS:
		{
			// Scale to MPH
			speed = static_cast<std::uint32_t>((speed * 0.001f) * 2.23694f); // Converting mm/s to m/s, then mm/s to mph
		}
		break;

		default:
		{
			speed = 0; // Reserved or n/a?
		}
		break;
	}
	if (get_is_object_shown(currentSpeedMeter_VarNum))
	{
		VTClientUpdateHelper.set_numeric_value(currentSpeedMeter_VarNum, speed);
	}
}

bool SeederVtApplication::get_is_object_shown(std::uint16_t objectID) const
{
	//! TODO: add this functionality to the VTClientStateTracker

	if (!VTClientUpdateHelper.is_working_set_active())
	{
		return false;
	}
	bool retVal = false;

	switch (objectID)
	{
		case section1Status_OutRect:
		case section2Status_OutRect:
		case section3Status_OutRect:
		case section4Status_OutRect:
		case section5Status_OutRect:
		case section6Status_OutRect:
		case autoManual_Container:
		case autoManual_ObjPtr:
		case mainRunscreen_SoftKeyMask:
		case Title_OutStr:
		case planterRunscreenStatus_Container:
		case planter_OutPict:
		case sectionButtons_Container:
		case section1Switch_Container:
		case section2Switch_Container:
		case section3Switch_Container:
		case section4Switch_Container:
		case section5Switch_Container:
		case section6Switch_Container:
		case speed_OutNum:
		case speedReadout_Container:
		case speedUnits_ObjPtr:
		case currentSpeedReadout_VarNum:
		case currentSpeedMeter_VarNum:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() == mainRunscreen_DataMask);
		}
		break;

		case statisticsHeader_OutStr:
		case statisticsDropdown_Container:
		case statistics_InList:
		case selectedStatisticsContainer_ObjPtr:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() == statisticsRunscreen_DataMask);
		}
		break;

		case returnHome_SKeyMask:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() != mainRunscreen_DataMask);
		}
		break;

		case busload_VarNum:
		case canAddress_VarNum:
		{
			retVal = ((VTClientUpdateHelper.get_active_mask() == statisticsRunscreen_DataMask) &&
			          (VTClientUpdateHelper.get_numeric_value(selectedStatisticsContainer_ObjPtr) == canStatistics_Container));
		}
		break;

		case utAddress_VarNum:
		case utVersion_VarNum:
		{
			retVal = ((VTClientUpdateHelper.get_active_mask() == statisticsRunscreen_DataMask) &&
			          (VTClientUpdateHelper.get_numeric_value(selectedStatisticsContainer_ObjPtr) == utStatistics_Container));
		}
		break;

		case tcVersion_VarNum:
		case tcAddress_VarNum:
		case tcNumberBoomsSupported_VarNum:
		case tcSupportedSections_VarNum:
		case tcControlChannels_VarNum:
		{
			retVal = ((VTClientUpdateHelper.get_active_mask() == statisticsRunscreen_DataMask) &&
			          (VTClientUpdateHelper.get_numeric_value(selectedStatisticsContainer_ObjPtr) == tcStatistics_Container));
		}
		break;

		case machineSpeedNotDetectedSummary_OutStr:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() == noSpeed_AlarmMask);
		}
		break;

		case TCNotConnectedSummary_OutStr:
		case noTCTitle_OutStr:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() == noTaskController_AlarmMask);
		}
		break;

		case warning_OutPict:
		case alarm_SKeyMask:
		{
			retVal = ((VTClientUpdateHelper.get_active_mask() == noSpeed_AlarmMask) ||
			          (VTClientUpdateHelper.get_active_mask() == noTaskController_AlarmMask));
		}
		break;

		case currentAlarms1_ObjPtr:
		case currentAlarms2_ObjPtr:
		case currentAlarmsHeader_OutStr:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() == alarmsRunscreen_DataMask);
		}
		break;

		case enableAlarms_VarNum:
		case enableAlarms_Container:
		case enableAlarms_InBool:
		case enableAlarms_OutStr:
		{
			retVal = (VTClientUpdateHelper.get_active_mask() == settingsRunscreen_DataMask);
		}
		break;

		default:
		{
			retVal = true;
		}
		break;
	}
	return retVal;
}

void SeederVtApplication::revert_to_previous_data_mask()
{
	for (std::uint16_t maskId : VTClientUpdateHelper.get_mask_history())
	{
		// Check if mask is a data mask and if it is not the current mask
		if ((maskId != noSpeed_AlarmMask) && (maskId != noTaskController_AlarmMask) && (maskId != VTClientUpdateHelper.get_active_mask()))
		{
			VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, maskId);
			return;
		}
	}
	// No previous data mask found, revert to main runscreen
	VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, mainRunscreen_DataMask);
}

void SeederVtApplication::update_ut_version_objects(isobus::VirtualTerminalClient::VTVersion version)
{
	std::uint8_t integerVersion = 0xFF;

	switch (version)
	{
		case isobus::VirtualTerminalClient::VTVersion::Version2OrOlder:
		{
			integerVersion = 2;
		}
		break;

		case isobus::VirtualTerminalClient::VTVersion::Version3:
		{
			integerVersion = 3;
		}
		break;

		case isobus::VirtualTerminalClient::VTVersion::Version4:
		{
			integerVersion = 4;
		}
		break;

		case isobus::VirtualTerminalClient::VTVersion::Version5:
		{
			integerVersion = 5;
		}
		break;

		case isobus::VirtualTerminalClient::VTVersion::Version6:
		{
			integerVersion = 6;
		}
		break;

		default:
			break;
	}
	VTClientUpdateHelper.set_numeric_value(utVersion_VarNum, integerVersion);
}

void SeederVtApplication::update_alarms()
{
	if (VTClientInterface->get_is_connected() && VTClientUpdateHelper.get_numeric_value(enableAlarms_VarNum))
	{
		// Check if we have a speed source
		if ((0 == speedMessages.get_number_received_machine_selected_speed_command_sources()) &&
		    (0 == speedMessages.get_number_received_ground_based_speed_sources()) &&
		    (0 == speedMessages.get_number_received_wheel_based_speed_sources()))
		{
			alarms.at(AlarmType::NoMachineSpeed).trigger();
		}
		else
		{
			alarms.at(AlarmType::NoMachineSpeed).reset();
		}

		// Check if we have a TC connected
		if (false == TCClientInterface.get_is_connected())
		{
			alarms.at(AlarmType::NoTaskController).trigger();
		}
		else
		{
			alarms.at(AlarmType::NoTaskController).reset();
		}

		// Show the first alarm that is active (i.e. highest priority)
		std::size_t activeAlarmsCount = 0;
		for (auto const &alarm : alarms)
		{
			if (alarm.second.is_active())
			{
				activeAlarmsCount++;
				switch (alarm.first)
				{
					case AlarmType::NoMachineSpeed:
					{
						if (1 == activeAlarmsCount)
						{
							VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, noSpeed_AlarmMask);
						}
						VTClientUpdateHelper.set_numeric_value(currentAlarms1_ObjPtr, NoMachineSpeed_OutStr);
					}
					break;

					case AlarmType::NoTaskController:
					{
						if (1 == activeAlarmsCount)
						{
							VTClientUpdateHelper.set_active_data_or_alarm_mask(example_WorkingSet, noTaskController_AlarmMask);
						}
						VTClientUpdateHelper.set_numeric_value(1 == activeAlarmsCount ? currentAlarms1_ObjPtr : currentAlarms2_ObjPtr, NoTaskController_OutStr);
					}
					break;

					default:
						break;
				}
			}
		}

		if ((0 == activeAlarmsCount) && ((VTClientUpdateHelper.get_active_mask() == noSpeed_AlarmMask) || (VTClientUpdateHelper.get_active_mask() == noTaskController_AlarmMask)))
		{
			// No alarms active, but we're showing the alarm screen. Clear it.
			revert_to_previous_data_mask();
		}

		for (std::size_t i = activeAlarmsCount; i < static_cast<std::size_t>(AlarmType::Count); ++i)
		{
			// Clear the remaining alarm slots
			VTClientUpdateHelper.set_numeric_value(i == 0 ? currentAlarms1_ObjPtr : currentAlarms2_ObjPtr, UNDEFINED);
		}
	}
}

SeederVtApplication::Alarm::Alarm(std::uint32_t activationDelay_ms) :
  activationDelay_ms(activationDelay_ms)
{
}

bool SeederVtApplication::Alarm::is_active() const
{
	return (!acknowledged) && (timestampTriggered_ms != 0) &&
	  isobus::SystemTiming::time_expired_ms(timestampTriggered_ms, activationDelay_ms);
}

void SeederVtApplication::Alarm::trigger()
{
	if (timestampTriggered_ms == 0)
	{
		timestampTriggered_ms = isobus::SystemTiming::get_timestamp_ms();
	}
}

void SeederVtApplication::Alarm::acknowledge()
{
	acknowledged = true;
}

void SeederVtApplication::Alarm::reset()
{
	timestampTriggered_ms = 0;
	acknowledged = false;
}
