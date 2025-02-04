//================================================================================================
/// @file isobus_virtual_terminal_working_set_base.cpp
///
/// @brief Implements a base class for a VT working set that isolates common working set functionality
/// so that things useful to VT designer application and a VT server application can be shared.
/// @author Adrian Del Grosso
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_virtual_terminal_working_set_base.hpp"

#include "isobus/isobus/can_stack_logger.hpp"
#include "isobus/utility/to_string.hpp"

#include <cstring>

namespace isobus
{
	std::uint16_t VirtualTerminalWorkingSetBase::get_object_pool_faulting_object_id()
	{
		std::lock_guard<std::mutex> lock(managedWorkingSetMutex);
		return faultingObjectID;
	}

	void VirtualTerminalWorkingSetBase::add_iop_raw_data(const std::vector<std::uint8_t> &dataToAdd)
	{
		transferredIopSize += dataToAdd.size();
		iopFilesRawData.push_back(dataToAdd);
	}

	std::size_t VirtualTerminalWorkingSetBase::get_number_iop_files() const
	{
		return iopFilesRawData.size();
	}

	std::vector<std::uint8_t> &VirtualTerminalWorkingSetBase::get_iop_raw_data(std::size_t index)
	{
		return iopFilesRawData.at(index);
	}

	VTColourVector VirtualTerminalWorkingSetBase::get_colour(std::uint8_t colourIndex) const
	{
		return workingSetColourTable.get_colour(colourIndex);
	}

	const std::map<std::uint16_t, std::shared_ptr<VTObject>> &VirtualTerminalWorkingSetBase::get_object_tree() const
	{
		return vtObjectTree;
	}

	bool VirtualTerminalWorkingSetBase::add_or_replace_object(std::shared_ptr<VTObject> objectToAdd)
	{
		bool retVal = false;

		if (nullptr != objectToAdd)
		{
			vtObjectTree[objectToAdd->get_id()] = objectToAdd;
			retVal = true;
		}
		return retVal;
	}

	bool VirtualTerminalWorkingSetBase::parse_next_object(std::uint8_t *&iopData, std::uint32_t &iopLength)
	{
		bool retVal = false;

		if (iopLength > 3)
		{
			// We at least have object ID and type
			auto decodedID = static_cast<uint16_t>(static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
			VirtualTerminalObjectType decodedType = static_cast<VirtualTerminalObjectType>(iopData[2]);

			switch (decodedType)
			{
				case VirtualTerminalObjectType::WorkingSet:
				{
					if ((NULL_OBJECT_ID == workingSetID) ||
					    ((nullptr != get_object_by_id(workingSetID)) &&
					     (get_object_by_id(workingSetID)->get_id() == decodedID)))
					{
						workingSetID = decodedID;
						auto tempObject = std::make_shared<WorkingSet>();

						if (iopLength >= tempObject->get_minumum_object_length())
						{
							tempObject->set_id(decodedID);
							tempObject->set_background_color(iopData[3]);
							tempObject->set_selectable(iopData[4]);
							tempObject->set_active_mask(static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8));

							// Now add child objects
							const std::uint8_t childrenToFollow = iopData[7];
							const std::uint16_t sizeOfChildren = (childrenToFollow * 6); // ID, X, Y 2 bytes each
							const std::uint8_t numberOfMacrosToFollow = iopData[8];
							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
							const std::uint8_t numberOfLanguagesToFollow = iopData[9];
							iopLength -= 10; // Subtract the bytes we've processed so far.
							iopData += 10; // Move the pointer

							if (iopLength >= sizeOfChildren)
							{
								for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
								{
									std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
									auto childX = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
									auto childY = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
									tempObject->add_child(childID, childX, childY);
									iopLength -= 6;
									iopData += 6;
								}

								// Next, parse macro list
								if (iopLength >= sizeOfMacros)
								{
									for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
									{
										// If the first byte is 255, then more bytes are used! 4.6.22.3
										if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
										{
											std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

											if (EventID::Reserved != get_event_from_byte(iopData[2]))
											{
												tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
												retVal = true;
											}
											else
											{
												CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
												retVal = false;
												break;
											}
										}
										else
										{
											if (EventID::Reserved != get_event_from_byte(iopData[0]))
											{
												tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
												retVal = true;
											}
											else
											{
												CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
												retVal = false;
												break;
											}
										}

										iopLength -= 2;
										iopData += 2;
									}

									// Next, parse language list
									if (iopLength >= static_cast<uint16_t>(numberOfLanguagesToFollow * 2))
									{
										for (std::uint_fast8_t i = 0; i < numberOfLanguagesToFollow; i++)
										{
											std::string langCode;
											langCode.push_back(static_cast<char>(iopData[0]));
											langCode.push_back(static_cast<char>(iopData[1]));
											iopLength -= 2;
											iopData += 2;
											CANStackLogger::debug("[WS]: IOP Language parsed: " + langCode);
										}
									}
									else
									{
										CANStackLogger::error("[WS]: Not enough IOP data to parse working set language codes for object " + isobus::to_string(static_cast<int>(decodedID)));
									}
									retVal = true;
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to parse working set macros for object " + isobus::to_string(static_cast<int>(decodedID)));
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse working set children for object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse working set object " + isobus::to_string(static_cast<int>(decodedID)));
						}

						if (retVal)
						{
							retVal = add_or_replace_object(tempObject);
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Multiple working set objects are not allowed in the object pool. Faulting object " + isobus::to_string(static_cast<int>(decodedID)));
					}
				}
				break;

				case VirtualTerminalObjectType::DataMask:
				{
					auto tempObject = std::make_shared<DataMask>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);
						tempObject->set_soft_key_mask(static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8));
						// Now add child objects
						const std::uint8_t childrenToFollow = iopData[6];
						const std::uint16_t sizeOfChildren = (childrenToFollow * 6); // ID, X, Y 2 bytes each
						const std::uint8_t numberOfMacrosToFollow = iopData[7];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 8; // Subtract the bytes we've processed so far.
						iopData += 8; // Move the pointer

						if (iopLength >= sizeOfChildren)
						{
							for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								std::int16_t childX = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
								std::int16_t childY = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
								tempObject->add_child(childID, childX, childY);
								iopLength -= 6;
								iopData += 6;
							}

							// Next, parse macro list
							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse data mask macros for object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse data mask children for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse data mask object for object " + isobus::to_string(static_cast<int>(decodedID)));
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::AlarmMask:
				{
					auto tempObject = std::make_shared<AlarmMask>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);
						tempObject->set_soft_key_mask(static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8));

						if (iopData[6] <= static_cast<std::uint8_t>(AlarmMask::Priority::Low))
						{
							tempObject->set_mask_priority(static_cast<AlarmMask::Priority>(iopData[6]));

							if (iopData[7] <= static_cast<std::uint8_t>(AlarmMask::AcousticSignal::None))
							{
								// Now add child objects
								const std::uint8_t childrenToFollow = iopData[8];
								const std::uint16_t sizeOfChildren = (childrenToFollow * 6); // ID, X, Y 2 bytes each
								const std::uint8_t numberOfMacrosToFollow = iopData[9];
								const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
								iopLength -= 10; // Subtract the bytes we've processed so far.
								iopData += 10; // Move the pointer

								if (iopLength >= sizeOfChildren)
								{
									for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
									{
										std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
										std::int16_t childX = (static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
										std::int16_t childY = (static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
										tempObject->add_child(childID, childX, childY);
										iopLength -= 6;
										iopData += 6;
									}

									// Next, parse macro list
									if (iopLength >= sizeOfMacros)
									{
										for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
										{
											// If the first byte is 255, then more bytes are used! 4.6.22.3
											if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
											{
												std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

												if (EventID::Reserved != get_event_from_byte(iopData[2]))
												{
													tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
													retVal = true;
												}
												else
												{
													CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
													retVal = false;
													break;
												}
											}
											else
											{
												if (EventID::Reserved != get_event_from_byte(iopData[0]))
												{
													tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
													retVal = true;
												}
												else
												{
													CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
													retVal = false;
													break;
												}
											}

											iopLength -= 2;
											iopData += 2;
										}

										if (0 == sizeOfMacros)
										{
											retVal = true;
										}
									}
									else
									{
										CANStackLogger::error("[WS]: Not enough IOP data to parse alarm mask macros for object " + isobus::to_string(static_cast<int>(decodedID)));
									}
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to parse alarm mask children for object " + isobus::to_string(static_cast<int>(decodedID)));
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Invalid acoustic signal priority " +
								                      isobus::to_string(static_cast<int>(iopData[7])) +
								                      " specified for alarm mask object " +
								                      isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Invalid alarm mask priority " +
							                      isobus::to_string(static_cast<int>(iopData[6])) +
							                      " specified for alarm mask object" +
							                      isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse alarm mask object for object " + isobus::to_string(static_cast<int>(decodedID)));
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::Container:
				{
					auto tempObject = std::make_shared<Container>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_hidden(0 != iopData[7]);

						if (iopData[7] > 1)
						{
							CANStackLogger::warn("[WS]: Container " +
							                     isobus::to_string(static_cast<int>(decodedID)) +
							                     " hidden attribute is not a supported value. Assuming that it is hidden.");
						}

						// Now add child objects
						const std::uint8_t childrenToFollow = iopData[8];
						const std::uint16_t sizeOfChildren = (childrenToFollow * 6); // ID, X, Y 2 bytes each
						const std::uint8_t numberOfMacrosToFollow = iopData[9];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 10; // Subtract the bytes we've processed so far.
						iopData += 10; // Move the pointer

						if (iopLength >= sizeOfChildren)
						{
							for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								auto childX = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
								auto childY = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
								tempObject->add_child(childID, childX, childY);
								iopLength -= 6;
								iopData += 6;
							}

							// Next, parse macro list

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse container macros for object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse container children for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse container object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::WindowMask:
				{
					auto tempObject = std::make_shared<WindowMask>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						retVal = true;
						tempObject->set_id(decodedID);

						if ((iopData[3] != 1) && (iopData[3] != 2))
						{
							CANStackLogger::warn("[WS]: Unknown window mask width for object %u. Allowed range is 1-2.", decodedID);
						}
						tempObject->set_width(iopData[3]);

						if ((iopData[4] < 1) || (iopData[4] > 6))
						{
							CANStackLogger::warn("[WS]: Unknown window mask height for object %u. Allowed range is 1-6.", decodedID);
						}
						tempObject->set_height(iopData[4]);

						if (iopData[5] > 18)
						{
							CANStackLogger::error("[WS]: Unknown window mask type for object %u. Allowed range is 1-18.", decodedID);
							retVal = false;
						}
						else
						{
							tempObject->set_window_type(static_cast<WindowMask::WindowType>(iopData[5]));
						}

						if (retVal)
						{
							tempObject->set_background_color(iopData[6]);
							tempObject->set_options(iopData[7]);

							const std::uint16_t name = (static_cast<std::uint16_t>(iopData[8]) | (static_cast<std::uint16_t>(iopData[9]) << 8));
							const std::uint16_t title = (static_cast<std::uint16_t>(iopData[10]) | (static_cast<std::uint16_t>(iopData[11]) << 8));
							const std::uint16_t icon = (static_cast<std::uint16_t>(iopData[12]) | (static_cast<std::uint16_t>(iopData[13]) << 8));

							tempObject->set_name_object_id(name);
							tempObject->set_title_object_id(title);
							tempObject->set_icon_object_id(icon);

							const std::uint8_t numberOfObjectReferences = iopData[14];
							const std::uint8_t numberOfChildObjects = iopData[15];
							const std::uint8_t numberOfMacros = iopData[16];
							const std::uint16_t sizeOfMacros = (numberOfMacros * 2);
							const std::uint16_t sizeOfChildren = (numberOfChildObjects * 6); // ID, X, Y 2 bytes each

							switch (tempObject->get_window_type())
							{
								case WindowMask::WindowType::StringOutputValue1x1:
								case WindowMask::WindowType::NumericOutputValueNoUnits1x1:
								case WindowMask::WindowType::SingleButton1x1:
								case WindowMask::WindowType::StringInputValue1x1:
								case WindowMask::WindowType::SingleButton2x1:
								case WindowMask::WindowType::HorizontalLinearBarGraphNoUnits2x1:
								case WindowMask::WindowType::NumericOutputValueNoUnits2x1:
								case WindowMask::WindowType::NumericInputValueNoUnits1x1:
								case WindowMask::WindowType::HorizontalLinearBarGraphNoUnits1x1:
								case WindowMask::WindowType::StringOutputValue2x1:
								case WindowMask::WindowType::StringInputValue2x1:
								case WindowMask::WindowType::NumericInputValueNoUnits2x1:
								{
									if (1 != numberOfObjectReferences)
									{
										retVal = false;
										CANStackLogger::error("[WS]: Window mask %u has an invalid number of object references. Value must be exactly 1.", decodedID);
									}
								}
								break;

								case WindowMask::WindowType::NumericOutputValueWithUnits1x1:
								case WindowMask::WindowType::DoubleButton2x1:
								case WindowMask::WindowType::NumericInputValueWithUnits1x1:
								case WindowMask::WindowType::NumericOutputValueWithUnits2x1:
								case WindowMask::WindowType::NumericInputValueWithUnits2x1:
								case WindowMask::WindowType::DoubleButton1x1:
								{
									if (2 != numberOfObjectReferences)
									{
										retVal = false;
										CANStackLogger::error("[WS]: Window mask %u has an invalid number of object references. Value must be exactly 2.", decodedID);
									}
								}
								break;

								case WindowMask::WindowType::Freeform:
								{
									if (0 != numberOfObjectReferences)
									{
										retVal = false;
										CANStackLogger::error("[WS]: Window mask %u has an invalid number of object references. Value must be exactly 0.", decodedID);
									}
								}
								break;
							}

							iopLength -= tempObject->get_minumum_object_length(); // Subtract the bytes we've processed so far.
							iopData += tempObject->get_minumum_object_length(); // Move the pointer

							if (iopLength >= static_cast<std::uint32_t>(2 * numberOfObjectReferences))
							{
								for (std::uint_fast8_t i = 0; i < numberOfObjectReferences; i++)
								{
									std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
									tempObject->add_child(childID, 0, 0);
									iopLength -= 2;
									iopData += 2;
								}

								if (iopLength >= sizeOfChildren)
								{
									for (std::uint_fast8_t i = 0; i < numberOfChildObjects; i++)
									{
										std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
										std::int16_t childX = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
										std::int16_t childY = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
										tempObject->add_child(childID, childX, childY);
										iopLength -= 6;
										iopData += 6;
									}

									// Next, parse macro list

									if (iopLength >= sizeOfMacros)
									{
										for (std::uint_fast8_t i = 0; i < numberOfMacros; i++)
										{
											// If the first byte is 255, then more bytes are used! 4.6.22.3
											if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
											{
												std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

												if (EventID::Reserved != get_event_from_byte(iopData[2]))
												{
													tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
													retVal = true;
												}
												else
												{
													CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
													retVal = false;
													break;
												}
											}
											else
											{
												if (EventID::Reserved != get_event_from_byte(iopData[0]))
												{
													tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
													retVal = true;
												}
												else
												{
													CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
													retVal = false;
													break;
												}
											}

											iopLength -= 2;
											iopData += 2;
										}

										if (0 == sizeOfMacros)
										{
											retVal = true;
										}
									}
									else
									{
										CANStackLogger::error("[WS]: Not enough IOP data to parse macros for object " + isobus::to_string(static_cast<int>(decodedID)));
										retVal = false;
									}
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to parse children for object " + isobus::to_string(static_cast<int>(decodedID)));
									retVal = false;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse object references for object " + isobus::to_string(static_cast<int>(decodedID)));
								retVal = false;
							}

							if (retVal)
							{
								retVal = add_or_replace_object(tempObject);
							}
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse window mask object.");
					}
				}
				break;

				case VirtualTerminalObjectType::SoftKeyMask:
				{
					auto tempObject = std::make_shared<SoftKeyMask>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);

						// Now add child objects
						const std::uint8_t childrenToFollow = iopData[4];
						const std::uint16_t sizeOfChildren = (childrenToFollow * 2); // ID 2 bytes
						const std::uint8_t numberOfMacrosToFollow = iopData[5];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 6; // Subtract the bytes we've processed so far.
						iopData += 6; // Move the pointer

						if (iopLength >= sizeOfChildren)
						{
							// For soft key masks, no x,y positions are included
							for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								tempObject->add_child(childID, 0, 0);
								iopLength -= 2;
								iopData += 2;
							}

							// Next, parse macro list

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse soft key mask macros for object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse soft key mask children for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse soft key mask object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::Key:
				{
					auto tempObject = std::make_shared<Key>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);
						tempObject->set_key_code(iopData[4]);

						// Now add child objects
						const std::uint8_t childrenToFollow = iopData[5];
						const std::uint16_t sizeOfChildren = (childrenToFollow * 6); // ID, X, Y 2 bytes each
						const std::uint8_t numberOfMacrosToFollow = iopData[6];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 7; // Subtract the bytes we've processed so far.
						iopData += 7; // Move the pointer

						if (iopLength >= sizeOfChildren)
						{
							for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								auto childX = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
								auto childY = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
								tempObject->add_child(childID, childX, childY);
								iopLength -= 6;
								iopData += 6;
							}

							// Next, parse macro list

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for key object" + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse key children for object" + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to key object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::Button:
				{
					auto tempObject = std::make_shared<Button>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_background_color(iopData[7]);
						tempObject->set_border_colour(iopData[8]);
						tempObject->set_key_code(iopData[9]);
						tempObject->set_options(iopData[10]);

						// Now add child objects
						const std::uint8_t childrenToFollow = iopData[11];
						const std::uint16_t sizeOfChildren = (childrenToFollow * 6); // ID, X, Y 2 bytes each
						const std::uint8_t numberOfMacrosToFollow = iopData[12];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 13; // Subtract the bytes we've processed so far.
						iopData += 13; // Move the pointer

						if (iopLength >= sizeOfChildren)
						{
							for (std::uint_fast8_t i = 0; i < childrenToFollow; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								std::int16_t childX = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[2]) | (static_cast<std::int16_t>(iopData[3]) << 8));
								std::int16_t childY = static_cast<std::int16_t>(static_cast<std::int16_t>(iopData[4]) | (static_cast<std::int16_t>(iopData[5]) << 8));
								tempObject->add_child(childID, childX, childY);
								iopLength -= 6;
								iopData += 6;
							}

							// Next, parse macro list
							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for button object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse button children for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse button object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::KeyGroup:
				{
					auto tempObject = std::make_shared<KeyGroup>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_options(iopData[3]);
						tempObject->set_name_object_id(static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8)); // Output string for the object's name/label
						tempObject->set_key_group_icon(static_cast<std::uint16_t>(iopData[6]) | (static_cast<std::uint16_t>(iopData[7]) << 8));

						// Parse children
						const std::uint8_t numberChildrenToFollow = iopData[8];
						iopLength -= 9;
						iopData += 9;

						const int64_t iopLengthRemaining = (iopLength - (numberChildrenToFollow * 2));

						if (iopLength >= iopLengthRemaining)
						{
							if (numberChildrenToFollow <= KeyGroup::MAX_CHILD_KEYS)
							{
								for (std::uint_fast8_t i = 0; i < numberChildrenToFollow; i++)
								{
									tempObject->add_child(static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8), 0, 0);
									iopLength -= 2;
									iopData += 2;
								}

								// Now parse macros
								const std::uint8_t numberOfMacrosToFollow = iopData[0];
								iopData++;
								iopLength--;

								const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
								if (iopLength >= sizeOfMacros)
								{
									for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
									{
										// If the first byte is 255, then more bytes are used! 4.6.22.3
										/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
										iopLength -= 2;
										iopData += 2;
										CANStackLogger::warn("[WS]: Skipped parsing macro reference in key group object (todo)");
									}
									retVal = true;
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to parse macros for key group object " + isobus::to_string(static_cast<int>(decodedID)));
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Key group " + isobus::to_string(static_cast<int>(decodedID)) + " has too many child key objects! Only 4 are permitted.");
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse key group object children");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse key group object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::InputBoolean:
				{
					auto tempObject = std::make_shared<InputBoolean>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8)));
						tempObject->set_foreground_colour_object_id((static_cast<std::uint16_t>(iopData[6]) | (static_cast<std::uint16_t>(iopData[7]) << 8))); // Child Font Attribute
						tempObject->set_variable_reference(static_cast<std::uint16_t>(iopData[8]) | (static_cast<std::uint16_t>(iopData[9]) << 8)); // Add variable reference
						tempObject->set_value(iopData[10]);
						tempObject->set_enabled(iopData[11]);

						// No children list to parse

						// Next, parse macro list
						const std::uint8_t numberOfMacrosToFollow = iopData[12];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopData += 13;
						iopLength -= 13;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
								{
									std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

									if (EventID::Reserved != get_event_from_byte(iopData[2]))
									{
										tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
										retVal = true;
									}
									else
									{
										CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
										retVal = false;
										break;
									}
								}
								else
								{
									if (EventID::Reserved != get_event_from_byte(iopData[0]))
									{
										tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
										retVal = true;
									}
									else
									{
										CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
										retVal = false;
										break;
									}
								}

								iopLength -= 2;
								iopData += 2;
							}

							if (0 == sizeOfMacros)
							{
								retVal = true;
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for input boolean object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse input boolean object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::InputString:
				{
					auto tempObject = std::make_shared<InputString>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_background_color(iopData[7]);
						tempObject->set_font_attributes((static_cast<std::uint16_t>(iopData[8]) | (static_cast<std::uint16_t>(iopData[9]) << 8)));
						tempObject->set_input_attributes((static_cast<std::uint16_t>(iopData[10]) | (static_cast<std::uint16_t>(iopData[11]) << 8)));
						tempObject->set_options(iopData[12]);
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[13]) | (static_cast<std::uint16_t>(iopData[14]) << 8))); // Number variable
						tempObject->set_justification_bitfield(iopData[15]);

						const std::size_t lengthOfStringObject = iopData[16];
						const int64_t iopLengthRemaining = (iopLength - 17); // Use larger signed int to detect negative rollover

						if (iopLengthRemaining > static_cast<std::uint16_t>((lengthOfStringObject + 2))) // +2 is for enabled byte and number of macros to follow
						{
							std::string tempString;
							tempString.reserve(lengthOfStringObject);

							for (std::uint_fast8_t i = 0; i < lengthOfStringObject; i++)
							{
								tempString.push_back(static_cast<char>(iopData[17 + i]));
							}

							tempObject->set_enabled(iopData[17 + lengthOfStringObject]);
							iopData += (18 + lengthOfStringObject);
							iopLength -= (18 + static_cast<std::uint32_t>(lengthOfStringObject));

							// Next, parse macro list
							const std::uint8_t numberOfMacrosToFollow = iopData[0];

							iopData++;
							iopLength--;

							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for input boolean object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse input string object value");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse input string object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::InputNumber:
				{
					auto tempObject = std::make_shared<InputNumber>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_background_color(iopData[7]);
						tempObject->set_font_attributes((static_cast<std::uint16_t>(iopData[8]) | (static_cast<std::uint16_t>(iopData[9]) << 8)));
						tempObject->set_options(iopData[10]);
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[11]) | (static_cast<std::uint16_t>(iopData[12]) << 8))); // Number variable
						tempObject->set_value(static_cast<std::uint32_t>(iopData[13]) |
						                      (static_cast<std::uint32_t>(iopData[14]) << 8) |
						                      (static_cast<std::uint32_t>(iopData[15]) << 16) |
						                      (static_cast<std::uint32_t>(iopData[16]) << 24));
						tempObject->set_minimum_value(static_cast<std::uint32_t>(iopData[17]) |
						                              (static_cast<std::uint32_t>(iopData[18]) << 8) |
						                              (static_cast<std::uint32_t>(iopData[19]) << 16) |
						                              (static_cast<std::uint32_t>(iopData[20]) << 24));
						tempObject->set_maximum_value(static_cast<std::uint32_t>(iopData[21]) |
						                              (static_cast<std::uint32_t>(iopData[22]) << 8) |
						                              (static_cast<std::uint32_t>(iopData[23]) << 16) |
						                              (static_cast<std::uint32_t>(iopData[24]) << 24));
						tempObject->set_offset(static_cast<std::uint32_t>(iopData[25]) |
						                       (static_cast<std::uint32_t>(iopData[26]) << 8) |
						                       (static_cast<std::uint32_t>(iopData[27]) << 16) |
						                       (static_cast<std::uint32_t>(iopData[28]) << 24));
						float tempFloat = 0;
						std::uint8_t floatBuffer[4] = {
							iopData[29],
							iopData[30],
							iopData[31],
							iopData[32]
						};
						std::memcpy(&tempFloat, &floatBuffer, 4); // TODO Feels kinda bad...

						tempObject->set_scale(tempFloat);
						tempObject->set_number_of_decimals(iopData[33]);
						tempObject->set_format(0 != iopData[34]);

						if (iopData[34] > 1)
						{
							CANStackLogger::warn("[WS]: Input number " + isobus::to_string(static_cast<int>(decodedID)) + " format byte has undefined value. Setting to exponential format.");
						}

						tempObject->set_justification_bitfield(iopData[35]);
						tempObject->set_options2(iopData[36]);

						// Parse macros
						const std::uint8_t numberOfMacrosToFollow = iopData[37];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 38;
						iopData += 38;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in input number (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for input number object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse input number object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::InputList:
				{
					auto tempObject = std::make_shared<InputList>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8))); // Number variable
						tempObject->set_value(iopData[9]);
						tempObject->set_options(iopData[11]);

						// Parse children
						const std::uint8_t numberOfListItems = iopData[10];
						iopData += 12;
						iopLength -= 12;

						const std::uint8_t numberOfMacrosToFollow = iopData[0];
						iopData++;
						iopLength--;

						if (iopLength >= static_cast<std::uint16_t>((2 * numberOfListItems)))
						{
							for (std::uint_fast8_t i = 0; i < numberOfListItems; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								tempObject->add_child(childID, 0, 0);
								iopLength -= 2;
								iopData += 2;
							}

							// Next, parse macro list
							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									if (iopData[0] == static_cast<std::uint8_t>(EventID::UseExtendedMacroReference))
									{
										std::uint16_t macroID = (static_cast<std::uint16_t>(iopData[1]) | (static_cast<std::uint16_t>(iopData[3]) << 8));

										if (EventID::Reserved != get_event_from_byte(iopData[2]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[2]), macroID });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", macroID, decodedID);
											retVal = false;
											break;
										}
									}
									else
									{
										if (EventID::Reserved != get_event_from_byte(iopData[0]))
										{
											tempObject->add_macro({ get_event_from_byte(iopData[0]), iopData[1] });
											retVal = true;
										}
										else
										{
											CANStackLogger::error("[WS]: Macro with ID %u which is listed as part of object %u has an invalid or unsupported event ID.", iopData[1], decodedID);
											retVal = false;
											break;
										}
									}

									iopLength -= 2;
									iopData += 2;
								}

								if (0 == sizeOfMacros)
								{
									retVal = true;
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for input list object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse children of input list object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse input list object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputString:
				{
					auto tempObject = std::make_shared<OutputString>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_background_color(iopData[7]);
						tempObject->set_font_attributes((static_cast<std::uint16_t>(iopData[8]) | (static_cast<std::uint16_t>(iopData[9]) << 8)));
						tempObject->set_options(iopData[10]);
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[11]) | (static_cast<std::uint16_t>(iopData[12]) << 8))); // String Variable
						tempObject->set_justification_bitfield(iopData[13]);

						const std::uint16_t stringLengthToFollow = (static_cast<std::uint16_t>(iopData[14]) | (static_cast<std::uint16_t>(iopData[15]) << 8));
						std::string tempString;
						tempString.reserve(stringLengthToFollow);
						iopData += 16;
						iopLength -= 16;

						if (iopLength >= stringLengthToFollow)
						{
							for (uint_fast8_t i = 0; i < stringLengthToFollow; i++)
							{
								tempString.push_back(static_cast<char>(iopData[0]));
								iopData++;
								iopLength--;
							}
							tempObject->set_value(tempString);

							// Parse macros
							const std::uint8_t numberOfMacrosToFollow = iopData[0];
							iopData++;
							iopLength--;

							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in output string object (todo)");
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output string object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse output string object value");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output string object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputNumber:
				{
					auto tempObject = std::make_shared<OutputNumber>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_background_color(iopData[7]);
						tempObject->set_font_attributes((static_cast<std::uint16_t>(iopData[8]) | (static_cast<std::uint16_t>(iopData[9]) << 8)));
						tempObject->set_options(iopData[10]);
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[11]) | (static_cast<std::uint16_t>(iopData[12]) << 8))); // Number Variable
						tempObject->set_value(static_cast<std::uint32_t>(iopData[13]) |
						                      (static_cast<std::uint32_t>(iopData[14]) << 8) |
						                      (static_cast<std::uint32_t>(iopData[15]) << 16) |
						                      (static_cast<std::uint32_t>(iopData[16]) << 24));
						tempObject->set_offset(static_cast<std::uint32_t>(iopData[17]) |
						                       (static_cast<std::uint32_t>(iopData[18]) << 8) |
						                       (static_cast<std::uint32_t>(iopData[19]) << 16) |
						                       (static_cast<std::uint32_t>(iopData[20]) << 24));
						float tempFloat = 0;
						std::uint8_t floatBuffer[4] = {
							iopData[21],
							iopData[22],
							iopData[23],
							iopData[24]
						};
						std::memcpy(&tempFloat, &floatBuffer, 4); // TODO Feels kinda bad...

						tempObject->set_scale(tempFloat);
						tempObject->set_number_of_decimals(iopData[25]);
						tempObject->set_format(0 != iopData[26]);

						if (iopData[26] > 1)
						{
							CANStackLogger::warn("[WS]: Output number " +
							                     isobus::to_string(static_cast<int>(decodedID)) +
							                     " format byte has undefined value. Setting to exponential format.");
						}
						tempObject->set_justification_bitfield(iopData[27]);

						// Parse Macros
						const std::uint8_t numberOfMacrosToFollow = iopData[28];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopLength -= 29;
						iopData += 29;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in output number (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output number object {}" + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output number object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputList:
				{
					auto tempObject = std::make_shared<OutputList>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8)));
						tempObject->set_value(iopData[9]);

						// Parse children
						const std::uint8_t numberOfListItems = iopData[10];
						const std::uint8_t numberOfMacrosToFollow = iopData[11];
						iopData += 12;
						iopLength -= 12;

						if (iopLength >= static_cast<std::uint16_t>((2 * numberOfListItems)))
						{
							for (std::uint_fast8_t i = 0; i < numberOfListItems; i++)
							{
								std::uint16_t childID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
								tempObject->add_child(childID, 0, 0);
								iopLength -= 2;
								iopData += 2;
							}

							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in output list object (todo)");
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output list object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse children for output list object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output list object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputLine:
				{
					auto tempObject = std::make_shared<OutputLine>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_line_attributes((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_width((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8)));

						if (iopData[9] <= 1)
						{
							tempObject->set_line_direction(static_cast<OutputLine::LineDirection>(iopData[9]));
						}
						else
						{
							CANStackLogger::error("[WS]: Unknown output line direction in object %u", decodedID);
						}

						iopData += 10;
						iopLength -= 10;

						// Parse macros
						const std::uint8_t numberOfMacrosToFollow = iopData[0];
						iopData++;
						iopLength--;

						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in output line object (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output line object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output line object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputRectangle:
				{
					auto tempObject = std::make_shared<OutputRectangle>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_line_attributes((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_width((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8)));
						tempObject->set_line_suppression_bitfield(iopData[9]);
						tempObject->set_fill_attributes((static_cast<std::uint16_t>(iopData[10]) | (static_cast<std::uint16_t>(iopData[11]) << 8)));
						iopData += 12;
						iopLength -= 12;

						// Parse macros
						const std::uint8_t numberOfMacrosToFollow = iopData[0];
						iopData++;
						iopLength--;

						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in output rectangle object (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output rectangle object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output rectangle object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputEllipse:
				{
					auto tempObject = std::make_shared<OutputEllipse>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_line_attributes((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_width((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8)));

						if (iopData[9] <= static_cast<std::uint8_t>(OutputEllipse::EllipseType::ClosedEllipseSection))
						{
							tempObject->set_ellipse_type(static_cast<OutputEllipse::EllipseType>(iopData[9]));
							tempObject->set_start_angle(iopData[10]);
							tempObject->set_end_angle(iopData[11]);
							tempObject->set_fill_attributes((static_cast<std::uint16_t>(iopData[12]) | (static_cast<std::uint16_t>(iopData[13]) << 8)));
							iopData += 14;
							iopLength -= 14;

							// Parse macros
							const std::uint8_t numberOfMacrosToFollow = iopData[0];
							iopData++;
							iopLength--;

							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in output ellipse object (todo)");
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output ellipse object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Output Ellipse type is undefined for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output ellipse object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputPolygon:
				{
					auto tempObject = std::make_shared<OutputPolygon>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_line_attributes((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8)));
						tempObject->set_fill_attributes((static_cast<std::uint16_t>(iopData[9]) | (static_cast<std::uint16_t>(iopData[10]) << 8)));

						if (iopData[11] <= 3)
						{
							tempObject->set_type(static_cast<OutputPolygon::PolygonType>(iopData[11]));

							const std::uint8_t numberOfPoints = iopData[12];
							const std::uint8_t numberOfMacrosToFollow = iopData[13];
							iopLength -= 14;
							iopData += 14;

							if (numberOfPoints < 3)
							{
								CANStackLogger::warn("[WS]: Output Polygon must have at least 3 points. Polygon %u will not be drawable.", decodedID);
							}

							if (iopLength >= static_cast<std::uint16_t>((numberOfPoints * 4)))
							{
								for (std::uint_fast8_t i = 0; i < numberOfPoints; i++)
								{
									tempObject->add_point((static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8)), (static_cast<std::uint16_t>(iopData[2]) | (static_cast<std::uint16_t>(iopData[3]) << 8)));
									iopLength -= 4;
									iopData += 4;
								}

								const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);

								if (iopLength >= sizeOfMacros)
								{
									for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
									{
										// If the first byte is 255, then more bytes are used! 4.6.22.3
										/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
										iopLength -= 2;
										iopData += 2;
										CANStackLogger::warn("[WS]: Skipped parsing macro reference in output polygon object (todo)");
									}
									retVal = true;
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output polygon object " + isobus::to_string(static_cast<int>(decodedID)));
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse output polygon child points for object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Polygon type is undefined for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output polygon object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputMeter:
				{
					auto tempObject = std::make_shared<OutputMeter>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height(tempObject->get_width());
						tempObject->set_needle_colour(iopData[5]);
						tempObject->set_border_colour(iopData[6]);
						tempObject->set_arc_and_tick_colour(iopData[7]);
						tempObject->set_options(iopData[8]);
						tempObject->set_number_of_ticks(iopData[9]);
						tempObject->set_start_angle(iopData[10]);
						tempObject->set_end_angle(iopData[11]);
						tempObject->set_min_value((static_cast<std::uint16_t>(iopData[12]) | (static_cast<std::uint16_t>(iopData[13]) << 8)));
						tempObject->set_max_value((static_cast<std::uint16_t>(iopData[14]) | (static_cast<std::uint16_t>(iopData[15]) << 8)));
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[16]) | (static_cast<std::uint16_t>(iopData[17]) << 8))); // Number Variable
						tempObject->set_value((static_cast<std::uint16_t>(iopData[18]) | (static_cast<std::uint16_t>(iopData[19]) << 8)));
						const std::uint8_t numberOfMacrosToFollow = iopData[20];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopData += 21;
						iopLength -= 21;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in output meter object (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output meter object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output meter object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputLinearBarGraph:
				{
					auto tempObject = std::make_shared<OutputLinearBarGraph>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_colour(iopData[7]);
						tempObject->set_target_line_colour(iopData[8]);
						tempObject->set_options(iopData[9]);
						tempObject->set_number_of_ticks(iopData[10]);
						tempObject->set_min_value((static_cast<std::uint16_t>(iopData[11]) | (static_cast<std::uint16_t>(iopData[12]) << 8)));
						tempObject->set_max_value((static_cast<std::uint16_t>(iopData[13]) | (static_cast<std::uint16_t>(iopData[14]) << 8)));
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[15]) | (static_cast<std::uint16_t>(iopData[16]) << 8))); // Number Variable
						tempObject->set_value((static_cast<std::uint16_t>(iopData[17]) | (static_cast<std::uint16_t>(iopData[18]) << 8)));
						tempObject->set_target_value_reference((static_cast<std::uint16_t>(iopData[19]) | (static_cast<std::uint16_t>(iopData[20]) << 8)));
						tempObject->set_target_value((static_cast<std::uint16_t>(iopData[21]) | (static_cast<std::uint16_t>(iopData[22]) << 8)));
						const std::uint8_t numberOfMacrosToFollow = iopData[23];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopData += 24;
						iopLength -= 24;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in output linear bar graph object (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output linear bar graph object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output linear bar graph object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::OutputArchedBarGraph:
				{
					auto tempObject = std::make_shared<OutputArchedBarGraph>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_height((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_colour(iopData[7]);
						tempObject->set_target_line_colour(iopData[8]);
						tempObject->set_options(iopData[9]);
						tempObject->set_start_angle(iopData[10]);
						tempObject->set_end_angle(iopData[11]);
						tempObject->set_bar_graph_width((static_cast<std::uint16_t>(iopData[12]) | (static_cast<std::uint16_t>(iopData[13]) << 8)));
						tempObject->set_min_value((static_cast<std::uint16_t>(iopData[14]) | (static_cast<std::uint16_t>(iopData[15]) << 8)));
						tempObject->set_max_value((static_cast<std::uint16_t>(iopData[16]) | (static_cast<std::uint16_t>(iopData[17]) << 8)));
						tempObject->set_variable_reference((static_cast<std::uint16_t>(iopData[18]) | (static_cast<std::uint16_t>(iopData[19]) << 8))); // Number Variable
						tempObject->set_value((static_cast<std::uint16_t>(iopData[20]) | (static_cast<std::uint16_t>(iopData[21]) << 8)));
						tempObject->set_target_value_reference((static_cast<std::uint16_t>(iopData[22]) | (static_cast<std::uint16_t>(iopData[23]) << 8)));
						tempObject->set_target_value((static_cast<std::uint16_t>(iopData[24]) | (static_cast<std::uint16_t>(iopData[25]) << 8)));
						const std::uint8_t numberOfMacrosToFollow = iopData[26];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopData += 27;
						iopLength -= 27;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in output arched bar graph object (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for output arched bar graph object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse output arched bar graph object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::GraphicsContext:
				{
					CANStackLogger::error("[WS]: Graphics context not supported yet (todo)");
				}
				break;

				case VirtualTerminalObjectType::Animation:
				{
					CANStackLogger::error("[WS]: Animation not supported yet (todo)");
				}
				break;

				case VirtualTerminalObjectType::PictureGraphic:
				{
					auto tempObject = std::make_shared<PictureGraphic>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_width((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						tempObject->set_actual_width((static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)));
						tempObject->set_actual_height((static_cast<std::uint16_t>(iopData[7]) | (static_cast<std::uint16_t>(iopData[8]) << 8)));
						tempObject->set_height(static_cast<std::uint16_t>(tempObject->get_actual_height() * (static_cast<float>(tempObject->get_width()) / static_cast<float>(tempObject->get_actual_width()))));

						if (iopData[9] <= static_cast<std::uint8_t>(PictureGraphic::Format::EightBitColour))
						{
							tempObject->set_format(static_cast<PictureGraphic::Format>(iopData[9]));
							tempObject->set_options(iopData[10]);
							tempObject->set_transparency_colour(iopData[11]);
							tempObject->set_number_of_bytes_in_raw_data(static_cast<std::uint32_t>(iopData[12]) |
							                                            (static_cast<std::uint32_t>(iopData[13]) << 8) |
							                                            (static_cast<std::uint32_t>(iopData[14]) << 16) |
							                                            (static_cast<std::uint32_t>(iopData[15]) << 24));
							const std::uint8_t numberOfMacrosToFollow = iopData[16];
							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
							iopData += 17;
							iopLength -= 17;

							if (tempObject->get_option(PictureGraphic::Options::RunLengthEncoded))
							{
								if (0 != (tempObject->get_number_of_bytes_in_raw_data() % 2))
								{
									CANStackLogger::error("[WS]: Picture graphic has RLE but an odd number of data bytes. Object: " + isobus::to_string(static_cast<int>(decodedID)));
								}
								else
								{
									// Decode the RLE
									std::size_t lineAmountLeft = tempObject->get_actual_width();
									for (std::uint_fast32_t i = 0; i < (tempObject->get_number_of_bytes_in_raw_data() / 2); i++)
									{
										for (std::size_t j = 0; j < iopData[0]; j++)
										{
											switch (tempObject->get_format())
											{
												case PictureGraphic::Format::EightBitColour:
												{
													tempObject->add_raw_data(iopData[1]);
												}
												break;

												case PictureGraphic::Format::FourBitColour:
												{
													tempObject->add_raw_data(iopData[1] >> 4);
													lineAmountLeft--;

													if (lineAmountLeft > 0)
													{
														//Unused bits at the end of a line are ignored.
														tempObject->add_raw_data(iopData[1] & 0x0F);
														lineAmountLeft--;

														if (0 == lineAmountLeft)
														{
															lineAmountLeft = tempObject->get_actual_width();
														}
													}
													else
													{
														lineAmountLeft = tempObject->get_actual_width();
													}
												}
												break;

												case PictureGraphic::Format::Monochrome:
												{
													for (std::uint_fast8_t k = 0; k < 8U; k++)
													{
														tempObject->add_raw_data(static_cast<std::uint8_t>(0 != ((iopData[1]) & (1 << (7 - k)))));
														lineAmountLeft--;

														if (0 == lineAmountLeft)
														{
															break;
														}
													}

													if (0 == lineAmountLeft)
													{
														lineAmountLeft = tempObject->get_actual_width();
													}
												}
												break;

												default:
													break;
											}
										}
										iopData += 2;
										iopLength -= 2;
									}
								}
							}
							else
							{
								if (iopLength >= tempObject->get_number_of_bytes_in_raw_data())
								{
									switch (tempObject->get_format())
									{
										case PictureGraphic::Format::EightBitColour:
										{
											tempObject->set_raw_data(iopData, tempObject->get_number_of_bytes_in_raw_data());
											iopData += tempObject->get_number_of_bytes_in_raw_data();
											iopLength -= tempObject->get_number_of_bytes_in_raw_data();
										}
										break;

										case PictureGraphic::Format::FourBitColour:
										{
											std::size_t lineAmountLeft = tempObject->get_actual_width();

											for (std::uint_fast32_t i = 0; i < tempObject->get_number_of_bytes_in_raw_data(); i++)
											{
												tempObject->add_raw_data(iopData[0] >> 4);
												lineAmountLeft--;

												if (lineAmountLeft > 0)
												{
													tempObject->add_raw_data(iopData[0] & 0x0F);
													lineAmountLeft--;

													if (0 == lineAmountLeft)
													{
														lineAmountLeft = tempObject->get_actual_width();
													}
												}
												else
												{
													lineAmountLeft = tempObject->get_actual_width();
												}
												iopData++;
												iopLength--;
											}
										}
										break;

										case PictureGraphic::Format::Monochrome:
										{
											std::size_t lineAmountLeft = tempObject->get_actual_width();

											for (std::uint_fast32_t i = 0; i < tempObject->get_number_of_bytes_in_raw_data(); i++)
											{
												for (std::uint_fast8_t j = 0; j < 8U; j++)
												{
													tempObject->add_raw_data(static_cast<std::uint8_t>(0 != ((iopData[0]) & (1 << (7 - j)))));
													lineAmountLeft--;

													if (0 == lineAmountLeft)
													{
														break;
													}
												}

												if (0 == lineAmountLeft)
												{
													lineAmountLeft = tempObject->get_actual_width();
												}
												iopData++;
												iopLength--;
											}
										}
										break;
									}
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to deserialize picture graphic's pixel data. Object: " + isobus::to_string(static_cast<int>(decodedID)));
								}
							}

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in picture graphic object (todo)");
								}

								if (tempObject->get_raw_data().size() == (tempObject->get_actual_width() * tempObject->get_actual_height()))
								{
									retVal = true;
								}
								else
								{
									CANStackLogger::error("[WS]: Picture graphic object has invalid dimensions compared to its data. Object: " + isobus::to_string(static_cast<int>(decodedID)));
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for picture graphic object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Picture graphic format is undefined for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse picture graphic object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::NumberVariable:
				{
					auto tempObject = std::make_shared<NumberVariable>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_value(static_cast<std::uint32_t>(iopData[3]) |
						                      (static_cast<std::uint32_t>(iopData[4]) << 8) |
						                      (static_cast<std::uint32_t>(iopData[5]) << 16) |
						                      (static_cast<std::uint32_t>(iopData[6]) << 24));
						iopLength -= 7;
						iopData += 7;
						retVal = true;
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse number variable object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::StringVariable:
				{
					auto tempObject = std::make_shared<StringVariable>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);

						const std::uint16_t length = (static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8));
						iopLength -= 5;
						iopData += 5;

						if (iopLength >= length)
						{
							std::string tempStringValue;
							tempStringValue.reserve(length);

							for (std::uint32_t i = 0; i < length; i++)
							{
								tempStringValue.push_back(static_cast<char>(iopData[0]));
								iopData++;
								iopLength--;
							}
							tempObject->set_value(tempStringValue);
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse string variable object raw data");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse string variable object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::FontAttributes:
				{
					auto tempObject = std::make_shared<FontAttributes>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_colour(iopData[3]);
						tempObject->set_size(static_cast<FontAttributes::FontSize>(iopData[4]));

						if ((iopData[5] <= static_cast<std::uint8_t>(FontAttributes::FontType::ISO8859_7)) &&
						    (iopData[5] != static_cast<std::uint8_t>(FontAttributes::FontType::Reserved_1)) &&
						    (iopData[5] != static_cast<std::uint8_t>(FontAttributes::FontType::Reserved_2)))
						{
							tempObject->set_type(static_cast<FontAttributes::FontType>(iopData[5]));
							tempObject->set_style(iopData[6]);

							const std::uint8_t numberOfMacrosToFollow = iopData[7];
							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
							iopData += 8;
							iopLength -= 8;

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in font attributes object (todo)");
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for font attributes object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Proprietary and reserved fonts are not supported, and will likely never be supported.");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse font attributes object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::LineAttributes:
				{
					auto tempObject = std::make_shared<LineAttributes>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);
						tempObject->set_width(iopData[4]);
						tempObject->set_line_art_bit_pattern(static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8));

						const std::uint8_t numberOfMacrosToFollow = iopData[7];
						const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
						iopData += 8;
						iopLength -= 8;

						if (iopLength >= sizeOfMacros)
						{
							for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
							{
								// If the first byte is 255, then more bytes are used! 4.6.22.3
								/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
								iopLength -= 2;
								iopData += 2;
								CANStackLogger::warn("[WS]: Skipped parsing macro reference in line attributes object (todo)");
							}
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse macros for line attributes object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse line attributes object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::FillAttributes:
				{
					auto tempObject = std::make_shared<FillAttributes>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);

						if (iopData[3] <= static_cast<std::uint8_t>(FillAttributes::FillType::FillWithPatternGivenByFillPatternAttribute))
						{
							tempObject->set_type(static_cast<FillAttributes::FillType>(iopData[3]));
							tempObject->set_background_color(iopData[4]);
							tempObject->set_fill_pattern(static_cast<std::uint16_t>(iopData[5]) | (static_cast<std::uint16_t>(iopData[6]) << 8)); // Object ID for a picture graphic

							const std::uint8_t numberOfMacrosToFollow = iopData[7];
							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
							iopData += 8;
							iopLength -= 8;

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in fill attributes object (todo)");
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for fill attributes object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Fill attribute type is undefined for object " + isobus::to_string(static_cast<int>(decodedID)));
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse fill attributes object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::InputAttributes:
				{
					auto tempObject = std::make_shared<InputAttributes>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);

						if (iopData[3] <= static_cast<std::uint8_t>(InputAttributes::ValidationType::InvalidCharactersAreListed))
						{
							tempObject->set_validation_type(static_cast<InputAttributes::ValidationType>(iopData[3] & 0x01));
						}
						else
						{
							tempObject->set_validation_type(static_cast<InputAttributes::ValidationType>(iopData[3] & 0x01));
							CANStackLogger::warn("[WS]: Invalid input attributes validation type. Validation type must be < 2");
						}

						const std::uint8_t validationStringLength = iopData[4];
						iopData += 5;
						iopLength -= 5;

						if (iopLength >= validationStringLength)
						{
							std::string tempValidationString;
							tempValidationString.reserve(validationStringLength);

							for (std::uint_fast16_t i = 0; i < validationStringLength; i++)
							{
								tempValidationString.push_back(static_cast<char>(iopData[i]));
							}
							iopData += validationStringLength;
							iopLength -= validationStringLength;

							tempObject->set_validation_string(tempValidationString);

							const std::uint8_t numberOfMacrosToFollow = iopData[0];
							const std::uint16_t sizeOfMacros = (numberOfMacrosToFollow * 2);
							iopData++;
							iopLength--;

							if (iopLength >= sizeOfMacros)
							{
								for (std::uint_fast8_t i = 0; i < numberOfMacrosToFollow; i++)
								{
									// If the first byte is 255, then more bytes are used! 4.6.22.3
									/// @todo Parse macro data, check VT version 5 for 16 bit macro IDs
									iopLength -= 2;
									iopData += 2;
									CANStackLogger::warn("[WS]: Skipped parsing macro reference in input attributes object (todo)");
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse macros for input attributes object " + isobus::to_string(static_cast<int>(decodedID)));
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Not enough IOP data to parse input attributes validation string");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse input attributes object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::ExtendedInputAttributes:
				{
					auto tempObject = std::make_shared<ExtendedInputAttributes>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);

						if (iopData[3] <= static_cast<std::uint8_t>(ExtendedInputAttributes::ValidationType::InvalidCharactersAreListed))
						{
							tempObject->set_validation_type(static_cast<ExtendedInputAttributes::ValidationType>(iopData[3] & 0x01));
						}
						else
						{
							tempObject->set_validation_type(static_cast<ExtendedInputAttributes::ValidationType>(iopData[3] & 0x01));
							CANStackLogger::warn("[WS]: Invalid extended input attributes validation type. Validation type must be < 2");
						}

						const std::uint8_t numberOfCodePlanesToFollow = iopData[4];
						tempObject->set_number_of_code_planes(numberOfCodePlanesToFollow);
						CANStackLogger::error("[WS]: Extended input attributes not supported yet (todo)");
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse extended input attributes object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::ColourMap:
				{
					auto tempObject = std::make_shared<ColourMap>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse colour map object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::ObjectLabelRefrenceList:
				{
					CANStackLogger::error("[WS]: Object label reference not supported yet (todo)");
				}
				break;

				case VirtualTerminalObjectType::ObjectPointer:
				{
					auto tempObject = std::make_shared<ObjectPointer>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_value((static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8)));
						iopLength -= 5;
						iopData += 5;
						retVal = true;
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse object pointer object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::ExternalObjectDefinition:
				{
					CANStackLogger::error("[WS]: External object definition not supported yet (todo)");
				}
				break;

				case VirtualTerminalObjectType::ExternalReferenceNAME:
				{
					CANStackLogger::error("[WS]: External reference name not supported yet (todo)");
				}
				break;

				case VirtualTerminalObjectType::ExternalObjectPointer:
				{
					CANStackLogger::error("[WS]: External object pointer not supported yet (todo)");
				}
				break;

				case VirtualTerminalObjectType::Macro:
				{
					auto tempObject = std::make_shared<Macro>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);

						auto numberBytesToFollow = static_cast<std::uint16_t>(static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8));
						std::uint16_t numberBytesProcessed = 0;
						iopLength -= 5;
						iopData += 5;

						if (iopLength >= numberBytesToFollow)
						{
							retVal = true;

							while (numberBytesProcessed < numberBytesToFollow)
							{
								auto commandLength = 8;
								switch (static_cast<Macro::Command>(iopData[0]))
								{
									case Macro::Command::ChangeChildPosition:
										// special case: 9 bytes
										retVal = tempObject->add_command_packet({
										  iopData[0],
										  iopData[1],
										  iopData[2],
										  iopData[3],
										  iopData[4],
										  iopData[5],
										  iopData[6],
										  iopData[7],
										  iopData[8],
										});
										commandLength = 9;
										break;
									case Macro::Command::GraphicsContextCommand:
										// FIXME
										break;
									case Macro::Command::ChangeStringValue:
									{
										// Change string value has variable length
										std::vector<std::uint8_t> command;
										auto stringLength = static_cast<std::uint16_t>(static_cast<std::uint16_t>(iopData[3]) | (static_cast<std::uint16_t>(iopData[4]) << 8));
										for (int i = 0; i < (stringLength + 5); i++)
										{
											command.push_back(iopData[i]);
										}
										retVal = tempObject->add_command_packet(command);
										commandLength = 5 + stringLength;
										break;
									}
									default:
										// all other macro commands are 8 byte long
										retVal = tempObject->add_command_packet({
										  iopData[0],
										  iopData[1],
										  iopData[2],
										  iopData[3],
										  iopData[4],
										  iopData[5],
										  iopData[6],
										  iopData[7],
										});
										commandLength = 8;
										break;
								}
								iopLength -= commandLength;
								iopData += commandLength;
								numberBytesProcessed += commandLength;

								if (!retVal)
								{
									CANStackLogger::error("[WS]: Macro object %u cannot be parsed because a command packet could not be added.", decodedID);
									break;
								}
							}

							if (retVal)
							{
								retVal = tempObject->get_are_command_packets_valid();

								if (!retVal)
								{
									CANStackLogger::error("[WS]: Macro object %u contains malformed commands", decodedID);
								}
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Macro object %u cannot be parsed because there is not enough IOP data left", decodedID);
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse macro object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::AuxiliaryFunctionType1:
				{
					auto tempObject = std::make_shared<AuxiliaryFunctionType1>();

					CANStackLogger::warn("[WS]: Deserializing an Aux function type 1 object. This object is parsed and validated but NOT utilized by version 3 or later VTs in making Auxiliary Control Assignments.");

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);

						if (iopData[4] <= 2)
						{
							tempObject->set_function_type(static_cast<AuxiliaryFunctionType1::FunctionType>(iopData[4]));

							const std::uint8_t numberOfObjectsToFollow = iopData[5];
							const std::uint8_t numberOfBytesToFollow = numberOfObjectsToFollow * 6;
							iopData += 6;
							iopLength -= 6;

							if (iopLength >= numberOfBytesToFollow)
							{
								for (std::uint_fast8_t i = 0; i < numberOfObjectsToFollow; i++)
								{
									const std::uint16_t objectID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
									const std::uint16_t xPosition = (static_cast<std::uint16_t>(iopData[2]) | (static_cast<std::uint16_t>(iopData[3]) << 8));
									const std::uint16_t yPosition = (static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8));

									tempObject->add_child(objectID, xPosition, yPosition);
									iopData += 6;
									iopLength -= 6;
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary function type 1 object's children.");
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Auxiliary function type 1 object with ID %u has an invalid function type. The function type must be 2 or less.", decodedID);
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary function type 1 object.");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::AuxiliaryInputType1:
				{
					auto tempObject = std::make_shared<AuxiliaryInputType1>();

					CANStackLogger::warn("[WS]: Deserializing an Aux input type 1 object. This object is parsed and validated but NOT utilized by version 3 or later VTs in making Auxiliary Control Assignments.");

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);

						if (iopData[4] <= 2)
						{
							tempObject->set_function_type(static_cast<AuxiliaryInputType1::FunctionType>(iopData[4]));

							if (iopData[5] <= 250)
							{
								tempObject->set_input_id(iopData[5]);

								const std::uint8_t numberOfObjectsToFollow = iopData[6];
								const std::uint8_t numberOfBytesToFollow = numberOfObjectsToFollow * 6;
								iopData += 7;
								iopLength -= 7;

								if (iopLength >= numberOfBytesToFollow)
								{
									for (std::uint_fast8_t i = 0; i < numberOfObjectsToFollow; i++)
									{
										const std::uint16_t objectID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
										const std::uint16_t xPosition = (static_cast<std::uint16_t>(iopData[2]) | (static_cast<std::uint16_t>(iopData[3]) << 8));
										const std::uint16_t yPosition = (static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8));

										tempObject->add_child(objectID, xPosition, yPosition);
										iopData += 6;
										iopLength -= 6;
									}
									retVal = true;
								}
								else
								{
									CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary function type 2 object's children.");
								}
							}
							else
							{
								CANStackLogger::error("[WS]: Auxiliary input type 1 object %u has an invalid input ID. Input ID must be 250 or less, but was decoded as %u", decodedID, iopData[5]);
							}
						}
						else
						{
							CANStackLogger::error("[WS]: Auxiliary input type 1 object %u has an invalid function type. Function type must be 2 or less.");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary input type 1 object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::AuxiliaryFunctionType2:
				{
					auto tempObject = std::make_shared<AuxiliaryFunctionType2>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);

						if ((iopData[4] & 0x1F) >= static_cast<std::uint8_t>(AuxiliaryFunctionType2::FunctionType::ReservedRangeStart))
						{
							CANStackLogger::error("[WS]: Auxiliary function type 2 with object ID %u has a reserved function type.", decodedID);
						}
						else if ((iopData[4] & 0x1F) == static_cast<std::uint8_t>(AuxiliaryFunctionType2::FunctionType::ReservedRangeEnd))
						{
							CANStackLogger::error("[WS]: Auxiliary function type 2 with object ID %u is using the remove assignment command function type, which is not allowed.", decodedID);
						}
						else
						{
							tempObject->set_function_type(static_cast<AuxiliaryFunctionType2::FunctionType>(iopData[4] & 0x1F));
							tempObject->set_function_attribute(AuxiliaryFunctionType2::CriticalControl, 0 != (iopData[4] & 0x20));
							tempObject->set_function_attribute(AuxiliaryFunctionType2::AssignmentRestriction, 0 != (iopData[4] & 0x40));
							tempObject->set_function_attribute(AuxiliaryFunctionType2::SingleAssignment, 0 != (iopData[4] & 0x80));

							const std::uint8_t numberOfObjectsToFollow = iopData[5];
							const std::uint8_t numberOfBytesToFollow = numberOfObjectsToFollow * 6;
							iopData += 6;
							iopLength -= 6;

							if (iopLength >= numberOfBytesToFollow)
							{
								for (std::uint_fast8_t i = 0; i < numberOfObjectsToFollow; i++)
								{
									const std::uint16_t objectID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
									const std::uint16_t xPosition = (static_cast<std::uint16_t>(iopData[2]) | (static_cast<std::uint16_t>(iopData[3]) << 8));
									const std::uint16_t yPosition = (static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8));

									tempObject->add_child(objectID, xPosition, yPosition);
									iopData += 6;
									iopLength -= 6;
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary function type 2 object's children.");
							}
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary function type 2 object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::AuxiliaryInputType2:
				{
					auto tempObject = std::make_shared<AuxiliaryInputType2>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);
						tempObject->set_background_color(iopData[3]);

						if ((iopData[4] & 0x1F) >= static_cast<std::uint8_t>(AuxiliaryFunctionType2::FunctionType::ReservedRangeStart))
						{
							CANStackLogger::error("[WS]: Auxiliary input type 2 with object ID %u has a reserved function type.", decodedID);
						}
						else if ((iopData[4] & 0x1F) == static_cast<std::uint8_t>(AuxiliaryFunctionType2::FunctionType::ReservedRangeEnd))
						{
							CANStackLogger::error("[WS]: Auxiliary input type 2 with object ID %u is using the remove assignment command function type, which is not allowed.", decodedID);
						}
						else
						{
							tempObject->set_function_type(static_cast<AuxiliaryFunctionType2::FunctionType>(iopData[4] & 0x1F));
							tempObject->set_function_attribute(AuxiliaryInputType2::CriticalControl, 0 != (iopData[4] & 0x20));
							tempObject->set_function_attribute(AuxiliaryInputType2::SingleAssignment, 0 != (iopData[4] & 0x80));

							if (0 != (iopData[4] & 0x40))
							{
								CANStackLogger::warn("[WS]: Auxiliary input type 2 with object ID %u is using the assignment restriction attribute, which is reserved and should be zero.", decodedID);
							}

							const std::uint8_t numberOfObjectsToFollow = iopData[5];
							const std::uint8_t numberOfBytesToFollow = numberOfObjectsToFollow * 6;
							iopData += 6;
							iopLength -= 6;

							if (iopLength >= numberOfBytesToFollow)
							{
								for (std::uint_fast8_t i = 0; i < numberOfObjectsToFollow; i++)
								{
									const std::uint16_t objectID = (static_cast<std::uint16_t>(iopData[0]) | (static_cast<std::uint16_t>(iopData[1]) << 8));
									const std::uint16_t xPosition = (static_cast<std::uint16_t>(iopData[2]) | (static_cast<std::uint16_t>(iopData[3]) << 8));
									const std::uint16_t yPosition = (static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8));
									tempObject->add_child(objectID, xPosition, yPosition);
									iopData += 6;
									iopLength -= 6;
								}
								retVal = true;
							}
							else
							{
								CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary input type 2 object's children.");
							}
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary input type 2 object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				case VirtualTerminalObjectType::AuxiliaryControlDesignatorType2:
				{
					auto tempObject = std::make_shared<AuxiliaryControlDesignatorType2>();

					if (iopLength >= tempObject->get_minumum_object_length())
					{
						tempObject->set_id(decodedID);

						if (iopData[3] <= 3)
						{
							tempObject->set_pointer_type(iopData[3]);
							tempObject->set_auxiliary_object_id((static_cast<std::uint16_t>(iopData[4]) | (static_cast<std::uint16_t>(iopData[5]) << 8)));
							iopData += 6;
							iopLength -= 6;
							retVal = true;
						}
						else
						{
							CANStackLogger::error("[WS]: Auxiliary control designator type 2 object %u  has an invalid pointer type. Pointer type must be 3 or less.");
						}
					}
					else
					{
						CANStackLogger::error("[WS]: Not enough IOP data to parse auxiliary control designator type 2 object");
					}

					if (retVal)
					{
						retVal = add_or_replace_object(tempObject);
					}
				}
				break;

				default:
				{
					CANStackLogger::error("[WS]: Unsupported Object");
				}
				break;
			}

			if (!retVal)
			{
				set_object_pool_faulting_object_id(decodedID);
			}
		}
		return retVal;
	}

	std::shared_ptr<VTObject> VirtualTerminalWorkingSetBase::get_object_by_id(std::uint16_t objectID)
	{
		return vtObjectTree[objectID];
	}

	std::shared_ptr<VTObject> VirtualTerminalWorkingSetBase::get_working_set_object()
	{
		return get_object_by_id(workingSetID);
	}

	bool VirtualTerminalWorkingSetBase::get_object_id_exists(std::uint16_t objectID)
	{
		bool retVal;

		if (vtObjectTree.find(objectID) == vtObjectTree.end())
		{
			retVal = false;
		}
		else
		{
			retVal = (nullptr != vtObjectTree[objectID]);
		}
		return retVal;
	}

	EventID VirtualTerminalWorkingSetBase::get_event_from_byte(std::uint8_t eventByte)
	{
		switch (eventByte)
		{
			case static_cast<std::uint8_t>(EventID::OnActivate):
			case static_cast<std::uint8_t>(EventID::OnDeactivate):
			case static_cast<std::uint8_t>(EventID::OnShow):
			case static_cast<std::uint8_t>(EventID::OnHide):
			case static_cast<std::uint8_t>(EventID::OnEnable):
			case static_cast<std::uint8_t>(EventID::OnDisable):
			case static_cast<std::uint8_t>(EventID::OnChangeActiveMask):
			case static_cast<std::uint8_t>(EventID::OnChangeSoftKeyMask):
			case static_cast<std::uint8_t>(EventID::OnChangeAttribute):
			case static_cast<std::uint8_t>(EventID::OnChangeBackgroundColour):
			case static_cast<std::uint8_t>(EventID::ChangeFontAttributes):
			case static_cast<std::uint8_t>(EventID::ChangeLineAttributes):
			case static_cast<std::uint8_t>(EventID::ChangeFillAttributes):
			case static_cast<std::uint8_t>(EventID::ChangeChildLocation):
			case static_cast<std::uint8_t>(EventID::OnChangeSize):
			case static_cast<std::uint8_t>(EventID::OnChangeValue):
			case static_cast<std::uint8_t>(EventID::OnChangePriority):
			case static_cast<std::uint8_t>(EventID::OnChangeEndpoint):
			case static_cast<std::uint8_t>(EventID::OnInputFieldSelection):
			case static_cast<std::uint8_t>(EventID::OnInputFieldDeselection):
			case static_cast<std::uint8_t>(EventID::OnESC):
			case static_cast<std::uint8_t>(EventID::OnEntryOfAValue):
			case static_cast<std::uint8_t>(EventID::OnEntryOfANewValue):
			case static_cast<std::uint8_t>(EventID::OnKeyPress):
			case static_cast<std::uint8_t>(EventID::OnKeyRelease):
			case static_cast<std::uint8_t>(EventID::OnChangeChildPosition):
			case static_cast<std::uint8_t>(EventID::OnPointingEventPress):
			case static_cast<std::uint8_t>(EventID::OnPointingEventRelease):
			{
				return static_cast<EventID>(eventByte);
			}
			break;

			default:
			{
				return EventID::Reserved;
			}
			break;
		}
	}

	bool VirtualTerminalWorkingSetBase::parse_iop_into_objects(std::uint8_t *iopData, std::uint32_t iopLength)
	{
		uint32_t remainingLength = iopLength;
		std::uint8_t *currentIopPointer = iopData;
		bool retVal = true;

		if (iopLength > 0)
		{
			while (remainingLength > 0)
			{
				if (!parse_next_object(currentIopPointer, remainingLength))
				{
					CANStackLogger::error("[WS]: Parsing object pool failed.");
					retVal = false;
					break;
				}
			}
		}
		else
		{
			retVal = false;
		}
		return retVal;
	}

	void VirtualTerminalWorkingSetBase::set_object_pool_faulting_object_id(std::uint16_t value)
	{
		const std::lock_guard<std::mutex> lock(managedWorkingSetMutex);
		faultingObjectID = value;
	}
} // namespace isobus
