//================================================================================================
/// @file isobus_device_descriptor_object_pool_helpers.cpp
///
/// @brief Implements helpers for the DeviceDescriptorObjectPool class.
/// @author Adrian Del Grosso
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================
#include "isobus/isobus/isobus_device_descriptor_object_pool_helpers.hpp"

#include "isobus/isobus/can_stack_logger.hpp"

namespace isobus
{
	DeviceDescriptorObjectPoolHelper::ObjectPoolValue::operator bool() const
	{
		return exists();
	}

	bool DeviceDescriptorObjectPoolHelper::ObjectPoolValue::exists() const
	{
		return isValuePresent;
	}

	bool DeviceDescriptorObjectPoolHelper::ObjectPoolValue::editable() const
	{
		return isSettable;
	}

	std::int32_t DeviceDescriptorObjectPoolHelper::ObjectPoolValue::get() const
	{
		return value;
	}

	bool DeviceDescriptorObjectPoolHelper::ProductControlInformation::is_valid() const
	{
		return ((rateActual.dataDictionaryIdentifier != static_cast<std::uint16_t>(DataDescriptionIndex::Reserved)) ||
		        (rateDefault.dataDictionaryIdentifier != static_cast<std::uint16_t>(DataDescriptionIndex::Reserved)) ||
		        (rateMaximum.dataDictionaryIdentifier != static_cast<std::uint16_t>(DataDescriptionIndex::Reserved)) ||
		        (rateMinimum.dataDictionaryIdentifier != static_cast<std::uint16_t>(DataDescriptionIndex::Reserved)) ||
		        (rateSetpoint.dataDictionaryIdentifier != static_cast<std::uint16_t>(DataDescriptionIndex::Reserved)));
	}

	DeviceDescriptorObjectPoolHelper::Implement DeviceDescriptorObjectPoolHelper::get_implement_geometry(DeviceDescriptorObjectPool &ddop)
	{
		Implement retVal;

		if (0 == ddop.size())
		{
			LOG_ERROR("[DDOP Helper]: No objects in the pool.");
			return retVal; // Return empty object
		}

		// First, find the device object
		for (std::uint16_t i = 0; i < ddop.size(); i++)
		{
			auto deviceObject = ddop.get_object_by_index(i);

			if ((nullptr != deviceObject) &&
			    (task_controller_object::ObjectTypes::Device == deviceObject->get_object_type()))
			{
				// Track if we ever find a function. If not, the device will be the boom's root.
				bool foundFunction = false;
				std::shared_ptr<task_controller_object::DeviceElementObject> deviceElementObject;

				// Next, iterate through all elements whose parent is the device object
				for (std::uint16_t j = 0; j < ddop.size(); j++)
				{
					auto currentElementObject = ddop.get_object_by_index(j);

					if ((nullptr != currentElementObject) &&
					    (task_controller_object::ObjectTypes::DeviceElement == currentElementObject->get_object_type()) &&
					    (std::static_pointer_cast<task_controller_object::DeviceElementObject>(currentElementObject)->get_parent_object() == deviceObject->get_object_id()))
					{
						deviceElementObject = std::static_pointer_cast<task_controller_object::DeviceElementObject>(currentElementObject);

						// All the things we care about are likely in here, under the device element.
						for (std::uint16_t k = 0; k < ddop.size(); k++)
						{
							auto potentialFunction = ddop.get_object_by_index(k);

							if ((nullptr != potentialFunction) &&
							    (task_controller_object::ObjectTypes::DeviceElement == potentialFunction->get_object_type()) &&
							    (std::static_pointer_cast<task_controller_object::DeviceElementObject>(potentialFunction)->get_parent_object() == currentElementObject->get_object_id()) &&
							    (task_controller_object::DeviceElementObject::Type::Function == std::static_pointer_cast<task_controller_object::DeviceElementObject>(potentialFunction)->get_type()))
							{
								parse_element(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(potentialFunction), retVal);
								foundFunction = true;
							}
						}
						break;
					}
				}

				if (!foundFunction && (nullptr != deviceElementObject))
				{
					// If we didn't find a function, the device element object is the root of the boom.
					// So we'll reparse the device element object to get the sections and properties we care about.
					parse_element(ddop, deviceElementObject, retVal);

					// Search all elements whose parent is the device element object
					// To look for bins as well, since we didn't find any functions.
					for (std::uint16_t k = 0; k < ddop.size(); k++)
					{
						auto potentialBin = ddop.get_object_by_index(k);

						if ((nullptr != potentialBin) &&
						    (task_controller_object::ObjectTypes::DeviceElement == potentialBin->get_object_type()) &&
						    (std::static_pointer_cast<task_controller_object::DeviceElementObject>(potentialBin)->get_parent_object() == deviceElementObject->get_object_id()) &&
						    (task_controller_object::DeviceElementObject::Type::Bin == std::static_pointer_cast<task_controller_object::DeviceElementObject>(potentialBin)->get_type()))
						{
							auto binInfo = parse_bin(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(potentialBin));

							if (binInfo.is_valid() && !retVal.booms.empty())
							{
								retVal.booms[0].rates.push_back(binInfo);
							}
						}
					}
				}
				return retVal;
			}
		}
		LOG_ERROR("[DDOP Helper]: No device object in the pool.");
		return retVal; // If we got here, we didn't find a device object? Return empty object
	}

	void DeviceDescriptorObjectPoolHelper::parse_element(DeviceDescriptorObjectPool &ddop,
	                                                     std::shared_ptr<task_controller_object::DeviceElementObject> elementObject,
	                                                     Implement &implementToPopulate)
	{
		Boom boomToPopulate;
		boomToPopulate.elementNumber = elementObject->get_element_number();

		if (task_controller_object::DeviceElementObject::Type::Function == elementObject->get_type())
		{
			// Accumulate the number of functions under this function.
			// Need to search the whole pool because elements have parent links, not child links, which is not very efficient.
			for (std::uint16_t i = 0; i < ddop.size(); i++)
			{
				auto element = ddop.get_object_by_index(i);

				if ((nullptr != element) &&
				    (task_controller_object::ObjectTypes::DeviceElement == element->get_object_type()) &&
				    (std::static_pointer_cast<task_controller_object::DeviceElementObject>(element)->get_parent_object() == elementObject->get_object_id()))
				{
					if (task_controller_object::DeviceElementObject::Type::Function == std::static_pointer_cast<task_controller_object::DeviceElementObject>(element)->get_type())
					{
						boomToPopulate.subBooms.push_back(parse_sub_boom(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(element)));
					}
					else if (task_controller_object::DeviceElementObject::Type::Bin == std::static_pointer_cast<task_controller_object::DeviceElementObject>(element)->get_type())
					{
						auto binInfo = parse_bin(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(element));

						if (binInfo.is_valid())
						{
							boomToPopulate.rates.push_back(binInfo);
						}
					}
				}
			}
		}

		if (boomToPopulate.subBooms.empty())
		{
			// Find all sections in this boom
			for (std::uint16_t i = 0; i < ddop.size(); i++)
			{
				auto section = ddop.get_object_by_index(i);

				if ((nullptr != section) &&
				    (task_controller_object::ObjectTypes::DeviceElement == section->get_object_type()) &&
				    (task_controller_object::DeviceElementObject::Type::Section == std::static_pointer_cast<task_controller_object::DeviceElementObject>(section)->get_type()) &&
				    (std::static_pointer_cast<task_controller_object::DeviceElementObject>(section)->get_parent_object() == elementObject->get_object_id()))
				{
					boomToPopulate.sections.push_back(parse_section(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(section)));
				}
			}

			// Find child DDIs that we care about
			for (std::uint16_t childObjectId : elementObject->get_child_object_ids())
			{
				auto child = ddop.get_object_by_id(childObjectId);

				if (nullptr != child)
				{
					set_value_and_editable_from_object(boomToPopulate.xOffset_mm, child, DataDescriptionIndex::DeviceElementOffsetX);
					set_value_and_editable_from_object(boomToPopulate.yOffset_mm, child, DataDescriptionIndex::DeviceElementOffsetY);
					set_value_and_editable_from_object(boomToPopulate.zOffset_mm, child, DataDescriptionIndex::DeviceElementOffsetZ);

					if ((task_controller_object::ObjectTypes::DeviceElement == child->get_object_type()) &&
					    (task_controller_object::DeviceElementObject::Type::Bin == std::static_pointer_cast<task_controller_object::DeviceElementObject>(child)->get_type()))
					{
						auto binInfo = parse_bin(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(child));

						if (binInfo.is_valid())
						{
							boomToPopulate.rates.push_back(binInfo);
						}
					}
				}
			}
		}
		else
		{
			// Sections are part of the sub booms
		}
		implementToPopulate.booms.push_back(boomToPopulate);
	}

	DeviceDescriptorObjectPoolHelper::Section DeviceDescriptorObjectPoolHelper::parse_section(DeviceDescriptorObjectPool &ddop,
	                                                                                          std::shared_ptr<task_controller_object::DeviceElementObject> elementObject)
	{
		Section retVal;

		for (std::uint16_t childObjectId : elementObject->get_child_object_ids())
		{
			auto sectionChildObject = ddop.get_object_by_id(childObjectId);

			if (nullptr != sectionChildObject)
			{
				set_value_and_editable_from_object(retVal.xOffset_mm, sectionChildObject, DataDescriptionIndex::DeviceElementOffsetX);
				set_value_and_editable_from_object(retVal.yOffset_mm, sectionChildObject, DataDescriptionIndex::DeviceElementOffsetY);
				set_value_and_editable_from_object(retVal.zOffset_mm, sectionChildObject, DataDescriptionIndex::DeviceElementOffsetZ);
				set_value_and_editable_from_object(retVal.width_mm, sectionChildObject, DataDescriptionIndex::ActualWorkingWidth);
				if (!retVal.width_mm.exists())
				{
					set_value_and_editable_from_object(retVal.width_mm, sectionChildObject, DataDescriptionIndex::MaximumWorkingWidth);
				}
				if (!retVal.width_mm.exists())
				{
					set_value_and_editable_from_object(retVal.width_mm, sectionChildObject, DataDescriptionIndex::DefaultWorkingWidth);
				}

				if ((task_controller_object::ObjectTypes::DeviceElement == sectionChildObject->get_object_type()) &&
				    (task_controller_object::DeviceElementObject::Type::Bin == std::static_pointer_cast<task_controller_object::DeviceElementObject>(sectionChildObject)->get_type()))
				{
					auto binInfo = parse_bin(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(sectionChildObject));

					if (binInfo.is_valid())
					{
						retVal.rates.push_back(binInfo);
					}
				}
			}
		}
		retVal.elementNumber = elementObject->get_element_number();
		return retVal;
	}

	DeviceDescriptorObjectPoolHelper::SubBoom DeviceDescriptorObjectPoolHelper::parse_sub_boom(DeviceDescriptorObjectPool &ddop,
	                                                                                           std::shared_ptr<task_controller_object::DeviceElementObject> elementObject)
	{
		SubBoom retVal;
		retVal.elementNumber = elementObject->get_element_number();

		// Find all sections in this sub boom
		// We again have to search the whole pool because elements have parent links, not child links
		for (std::uint16_t i = 0; i < ddop.size(); i++)
		{
			auto section = ddop.get_object_by_index(i);

			if ((nullptr != section) &&
			    (task_controller_object::ObjectTypes::DeviceElement == section->get_object_type()) &&
			    (task_controller_object::DeviceElementObject::Type::Section == std::static_pointer_cast<task_controller_object::DeviceElementObject>(section)->get_type()) &&
			    (std::static_pointer_cast<task_controller_object::DeviceElementObject>(section)->get_parent_object() == elementObject->get_object_id()))
			{
				retVal.sections.push_back(parse_section(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(section)));
			}
		}

		// Process child DDIs of this sub boom to locate offset and width
		for (std::uint16_t childObjectId : elementObject->get_child_object_ids())
		{
			auto childObject = ddop.get_object_by_id(childObjectId);

			if (nullptr != childObject)
			{
				set_value_and_editable_from_object(retVal.xOffset_mm, childObject, DataDescriptionIndex::DeviceElementOffsetX);
				set_value_and_editable_from_object(retVal.yOffset_mm, childObject, DataDescriptionIndex::DeviceElementOffsetY);
				set_value_and_editable_from_object(retVal.zOffset_mm, childObject, DataDescriptionIndex::DeviceElementOffsetZ);
				set_value_and_editable_from_object(retVal.width_mm, childObject, DataDescriptionIndex::ActualWorkingWidth);
				if (!retVal.width_mm.exists())
				{
					set_value_and_editable_from_object(retVal.width_mm, childObject, DataDescriptionIndex::MaximumWorkingWidth);
				}
				if (!retVal.width_mm.exists())
				{
					set_value_and_editable_from_object(retVal.width_mm, childObject, DataDescriptionIndex::DefaultWorkingWidth);
				}

				if ((task_controller_object::ObjectTypes::DeviceElement == childObject->get_object_type()) &&
				    (task_controller_object::DeviceElementObject::Type::Bin == std::static_pointer_cast<task_controller_object::DeviceElementObject>(childObject)->get_type()))
				{
					auto binInfo = parse_bin(ddop, std::static_pointer_cast<task_controller_object::DeviceElementObject>(childObject));

					if (binInfo.is_valid())
					{
						retVal.rates.push_back(binInfo);
					}
				}
			}
		}
		return retVal;
	}

	DeviceDescriptorObjectPoolHelper::ProductControlInformation DeviceDescriptorObjectPoolHelper::parse_bin(DeviceDescriptorObjectPool &ddop,
	                                                                                                        std::shared_ptr<task_controller_object::DeviceElementObject> elementObject)
	{
		// A bin is used to identify a product.
		// We'll use this to populate the product control information.
		ProductControlInformation retVal;

		if (task_controller_object::DeviceElementObject::Type::Bin == elementObject->get_type())
		{
			retVal.elementNumber = elementObject->get_element_number();
			for (std::uint16_t childObjectId : elementObject->get_child_object_ids())
			{
				auto object = ddop.get_object_by_id(childObjectId);

				if (nullptr != object)
				{
					if (task_controller_object::ObjectTypes::DeviceProcessData == object->get_object_type())
					{
						std::uint16_t ddi = std::static_pointer_cast<task_controller_object::DeviceProcessDataObject>(object)->get_ddi();
						set_product_control_information_max_rate(retVal, object, ddi);
						set_product_control_information_min_rate(retVal, object, ddi);
						set_product_control_information_default_rate(retVal, object, ddi);
						set_product_control_information_setpoint_rate(retVal, object, ddi);
						set_product_control_information_actual_rate(retVal, object, ddi);
					}
					else if (task_controller_object::ObjectTypes::DeviceProperty == object->get_object_type())
					{
						std::uint16_t ddi = std::static_pointer_cast<task_controller_object::DevicePropertyObject>(object)->get_ddi();
						set_product_control_information_max_rate(retVal, object, ddi);
						set_product_control_information_min_rate(retVal, object, ddi);
						set_product_control_information_default_rate(retVal, object, ddi);
						set_product_control_information_setpoint_rate(retVal, object, ddi);
						set_product_control_information_actual_rate(retVal, object, ddi);
					}
				}
			}
		}
		return retVal;
	}

	void DeviceDescriptorObjectPoolHelper::set_value_and_editable_from_object(ObjectPoolValue &objectPoolValue,
	                                                                          const std::shared_ptr<task_controller_object::Object> &object,
	                                                                          DataDescriptionIndex ddi)
	{
		if (task_controller_object::ObjectTypes::DeviceProcessData == object->get_object_type())
		{
			auto processData = std::static_pointer_cast<task_controller_object::DeviceProcessDataObject>(object);
			if (processData->get_ddi() == static_cast<std::uint16_t>(ddi))
			{
				objectPoolValue.isSettable = processData->has_property(task_controller_object::DeviceProcessDataObject::PropertiesBit::Settable);
			}
		}
		else if (task_controller_object::ObjectTypes::DeviceProperty == object->get_object_type())
		{
			auto property = std::static_pointer_cast<task_controller_object::DevicePropertyObject>(object);
			if (property->get_ddi() == static_cast<std::uint16_t>(ddi))
			{
				objectPoolValue.value = property->get_value();
				objectPoolValue.isValuePresent = true;
			}
		}
	}

	void DeviceDescriptorObjectPoolHelper::set_product_control_information_rate(RateMetadata &rate,
	                                                                            const std::shared_ptr<task_controller_object::Object> &object,
	                                                                            std::uint16_t ddi)
	{
		if (task_controller_object::ObjectTypes::DeviceProcessData == object->get_object_type())
		{
			auto processData = std::static_pointer_cast<task_controller_object::DeviceProcessDataObject>(object);

			if (ddi == processData->get_ddi())
			{
				rate.isSettable = (0 != (static_cast<std::uint8_t>(task_controller_object::DeviceProcessDataObject::PropertiesBit::Settable) & processData->get_properties_bitfield()));
				rate.objectID = processData->get_object_id();
				rate.dataDictionaryIdentifier = processData->get_ddi();
			}
		}
		else if (task_controller_object::ObjectTypes::DeviceProperty == object->get_object_type())
		{
			auto property = std::static_pointer_cast<task_controller_object::DevicePropertyObject>(object);

			if (ddi == property->get_ddi())
			{
				rate.objectID = property->get_object_id();
				rate.isValuePresent = true;
				rate.value = property->get_value();
				rate.dataDictionaryIdentifier = property->get_ddi();
			}
		}
	}

	void DeviceDescriptorObjectPoolHelper::set_product_control_information_max_rate(ProductControlInformation &productControlInformation,
	                                                                                const std::shared_ptr<task_controller_object::Object> &object,
	                                                                                std::uint16_t ddi)
	{
		switch (ddi)
		{
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumApplicationRateOfAmmonium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumApplicationRateOfDryMatter):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumApplicationRateOfNitrogen):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumApplicationRateOfPhosphor):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumApplicationRateOfPotassium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumCountPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumCountPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumMassPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumMassPerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumMassPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumVolumePerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumVolumePerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumVolumePerVolumeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumVolumePerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumSpacingApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MaximumRevolutionsPerTime):
			{
				set_product_control_information_rate(productControlInformation.rateMaximum, object, ddi);
			}
			break;

			default:
				break;
		}
	}

	void DeviceDescriptorObjectPoolHelper::set_product_control_information_min_rate(ProductControlInformation &productControlInformation,
	                                                                                const std::shared_ptr<task_controller_object::Object> &object,
	                                                                                std::uint16_t ddi)
	{
		switch (ddi)
		{
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumApplicationRateOfAmmonium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumApplicationRateOfDryMatter):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumApplicationRateOfNitrogen):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumApplicationRateOfPhosphor):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumApplicationRateOfPotassium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumCountPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumCountPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumMassPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumMassPerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumMassPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumVolumePerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumVolumePerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumVolumePerVolumeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumVolumePerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumSpacingApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::MinimumRevolutionsPerTime):
			{
				set_product_control_information_rate(productControlInformation.rateMinimum, object, ddi);
			}
			break;

			default:
				break;
		}
	}

	void DeviceDescriptorObjectPoolHelper::set_product_control_information_default_rate(ProductControlInformation &productControlInformation,
	                                                                                    const std::shared_ptr<task_controller_object::Object> &object,
	                                                                                    std::uint16_t ddi)
	{
		switch (ddi)
		{
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultRevolutionsPerTime):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultCountPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultCountPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultMassPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultMassPerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultMassPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultVolumePerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultVolumePerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultVolumePerVolumeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultVolumePerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::DefaultSpacingApplicationRate):
			{
				set_product_control_information_rate(productControlInformation.rateDefault, object, ddi);
			}
			break;

			default:
				break;
		}
	}

	void DeviceDescriptorObjectPoolHelper::set_product_control_information_setpoint_rate(ProductControlInformation &productControlInformation,
	                                                                                     const std::shared_ptr<task_controller_object::Object> &object,
	                                                                                     std::uint16_t ddi)
	{
		switch (ddi)
		{
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointApplicationRateOfAmmonium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointApplicationRateOfDryMatter):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointApplicationRateOfNitrogen):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointApplicationRateOfPhosphor):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointApplicationRateOfPotassium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCountPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointCountPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointMassPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointMassPerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointMassPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointVolumePerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointVolumePerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointVolumePerVolumeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointVolumePerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointSpacingApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::SetpointRevolutionsSpecifiedAsCountPerTime):
			{
				set_product_control_information_rate(productControlInformation.rateSetpoint, object, ddi);
			}
			break;

			default:
				break;
		}
	}

	void DeviceDescriptorObjectPoolHelper::set_product_control_information_actual_rate(ProductControlInformation &productControlInformation,
	                                                                                   const std::shared_ptr<task_controller_object::Object> &object,
	                                                                                   std::uint16_t ddi)
	{
		switch (ddi)
		{
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualApplicationRateOfAmmonium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualApplicationRateOfDryMatter):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualApplicationRateOfNitrogen):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualApplicationRateOfPhosphor):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualApplicationRateOfPotassium):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCountPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualCountPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualMassPerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualMassPerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualMassPerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualVolumePerAreaApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualVolumePerMassApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualVolumePerVolumeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualVolumePerTimeApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualSpacingApplicationRate):
			case static_cast<std::uint16_t>(DataDescriptionIndex::ActualRevolutionsPerTime):
			{
				set_product_control_information_rate(productControlInformation.rateActual, object, ddi);
			}
			break;

			default:
				break;
		}
	}
} // namespace isobus
