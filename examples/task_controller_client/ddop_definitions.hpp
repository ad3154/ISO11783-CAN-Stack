//================================================================================================
/// @file ddop_definitions.hpp
///
/// @brief Enumerates object IDs for our example DDOP.
/// @author Adrian Del Grosso
///
/// @copyright 2023 Adrian Del Grosso
//================================================================================================
#include <cstdint>

#ifndef DDOP_DEFINITIONS_HPP
#define DDOP_DEFINITIONS_HPP

/// @brief Max 16 sections in this example
static constexpr std::size_t NUMBER_SECTIONS_TO_CREATE = 16;

/// @brief Enumerates unique IDs in the example DDOP
enum class SprayerDDOPObjectIDs : std::uint16_t
{
	Device = 0,

	MainDeviceElement,

	DeviceActualWorkState,
	RequestDefaultProcessData,
	DeviceTotalTime,

	Connector,
	ConnectorXOffset,
	ConnectorYOffset,
	ConnectorType,

	SprayBoom,
	ActualWorkState,
	ActualWorkingWidth,
	AreaTotal,
	SetpointWorkState,
	SectionCondensedWorkState1_16,
	BoomXOffset,
	BoomYOffset,
	BoomZOffset,

	Section1,
	SectionMax = Section1 + (NUMBER_SECTIONS_TO_CREATE - 1),
	Section1XOffset,
	SectionXOffsetMax = Section1XOffset + (NUMBER_SECTIONS_TO_CREATE - 1),
	Section1YOffset,
	SectionYOffsetMax = Section1YOffset + (NUMBER_SECTIONS_TO_CREATE - 1),
	Section1Width,
	SectionWidthMax = Section1Width + (NUMBER_SECTIONS_TO_CREATE - 1),
	ActualCondensedWorkingState,
	SetpointCondensedWorkingState,

	LiquidProduct,
	TankCapacity,
	TankVolume,
	LifetimeApplicationVolumeTotal,
	PrescriptionControlState,
	ActualCulturalPractice,
	TargetRate,
	ActualRate,

	AreaPresentation,
	TimePresentation,
	ShortWidthPresentation,
	LongWidthPresentation,
	VolumePresentation,
	VolumePerAreaPresentation
};

#endif // DDOP_DEFINITIONS_HPP