//================================================================================================
/// @file can_control_function.hpp
///
/// @brief Defines a base class to represent a generic ISOBUS control function.
/// @author Adrian Del Grosso
/// @author Daan Steenbergen
///
/// @copyright 2024 The Open-Agriculture Developers
//================================================================================================

#ifndef CAN_CONTROL_FUNCTION_HPP
#define CAN_CONTROL_FUNCTION_HPP

#include "isobus/isobus/can_NAME.hpp"
#include "isobus/utility/thread_synchronization.hpp"

#include <memory>
#include <string>

namespace isobus
{
	/// @brief A class that describes an ISO11783 control function, which includes a NAME and address.
	class ControlFunction
	{
	public:
		/// @brief The type of the control function
		enum class Type
		{
			Internal, ///< The control function is part of our stack and can address claim
			External, ///< The control function is some other device on the bus
			Partnered ///< An external control function that you explicitly want to talk to
		};

		/// @brief The constructor of a control function. In most cases use `CANNetworkManager::create_internal_control_function()` or
		/// `CANNetworkManager::create_partnered_control_function()` instead, only use this constructor if you have advanced needs.
		/// @param[in] NAMEValue The NAME of the control function
		/// @param[in] addressValue The current address of the control function
		/// @param[in] CANPort The CAN channel index that the control function communicates on
		/// @param[in] type The 'Type' of control function to create
		ControlFunction(NAME NAMEValue, std::uint8_t addressValue, std::uint8_t CANPort, Type type = Type::External);

		virtual ~ControlFunction() = default;

		/// @brief Returns the current address of the control function
		/// @returns The current address of the control function
		std::uint8_t get_address() const;

		/// @brief Describes if the control function has a valid address (not NULL or global)
		/// @returns true if the address is < 0xFE
		bool get_address_valid() const;

		/// @brief Returns the CAN channel index the control function communicates on
		/// @returns The control function's CAN channel index
		std::uint8_t get_can_port() const;

		/// @brief Returns the NAME of the control function as described by its address claim message
		/// @returns The control function's NAME
		NAME get_NAME() const;

		/// @brief Returns the `Type` of the control function
		/// @returns The control function type
		Type get_type() const;

		///@brief Returns the 'Type' of the control function as a string
		///@returns The control function type as a string
		std::string get_type_string() const;

	protected:
		friend class CANNetworkManager; ///< The network manager needs access to the control function's internals
		static Mutex controlFunctionProcessingMutex; ///< Protects the control function tables
		const Type controlFunctionType; ///< The Type of the control function
		NAME controlFunctionNAME; ///< The NAME of the control function
		bool claimedAddressSinceLastAddressClaimRequest = false; ///< Used to mark CFs as stale if they don't claim within a certain time
		std::atomic_uint8_t address; ///< The address of the control function
		const std::uint8_t canPortIndex; ///< The CAN channel index of the control function
	};

} // namespace isobus

#endif // CAN_CONTROL_FUNCTION_HPP
