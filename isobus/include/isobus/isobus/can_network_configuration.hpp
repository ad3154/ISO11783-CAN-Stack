//================================================================================================
/// @file can_network_configuration.hpp
///
/// @brief This is a class for changing stack settings.
/// @author Adrian Del Grosso
///
/// @copyright 2022 The Open-Agriculture Developers
//================================================================================================

#ifndef CAN_NETWORK_CONFIGURATION_HPP
#define CAN_NETWORK_CONFIGURATION_HPP

#include <cstdint>

namespace isobus
{
	//================================================================================================
	/// @class CANNetworkConfiguration
	///
	/// @brief A class that defines stack-wide configuration data. You can set the values in there
	/// to suit your specific memory constraints.
	//================================================================================================
	class CANNetworkConfiguration
	{
	public:
		/// @brief The constructor for the configuration object
		CANNetworkConfiguration() = default;

		/// @brief The destructor for the configuration object
		~CANNetworkConfiguration() = default;

		/// @brief Configures the max number of concurrent TP sessions to provide a RAM limit for TP sessions
		/// @param[in] value The max allowable number of TP sessions
		void set_max_number_transport_protocol_sessions(std::uint32_t value);

		/// @brief Returns the max number of concurrent TP sessions
		/// @returns The max number of concurrent TP sessions
		std::uint32_t get_max_number_transport_protocol_sessions() const;

		/// @brief Sets the minimum time to wait between sending BAM frames
		/// (default is 50 ms for maximum J1939 compatibility)
		/// @details The acceptable range as defined by ISO-11783 is 10 to 200 ms.
		/// This is a minimum time, so if you set it to some value, like 10 ms, the
		/// stack will attempt to transmit it as close to that time as it can, but it is
		/// not possible to 100% ensure it.
		/// @param[in] value The minimum time to wait between sending BAM frames
		void set_minimum_time_between_transport_protocol_bam_frames(std::uint32_t value);

		/// @brief Returns the minimum time to wait between sending BAM frames
		/// @returns The minimum time to wait between sending BAM frames
		std::uint32_t get_minimum_time_between_transport_protocol_bam_frames() const;

		/// @brief Sets the max number of data frames the stack will use when
		/// in an ETP session, between EDPO phases. The default is 16.
		/// Note that the sending control function may choose to use a lower number of frames.
		/// @param[in] numberFrames The max number of data frames to use
		void set_number_of_packets_per_dpo_message(std::uint8_t numberFrames);

		/// @brief Returns the max number of data frames the stack will use when
		/// in an ETP session, between EDPO phases. The default is 16.
		/// Note that the sending control function may choose to use a lower number of frames.
		/// @returns The number of data frames the stack will use when sending ETP messages between EDPOs
		std::uint8_t get_number_of_packets_per_dpo_message() const;

		/// @brief Sets the max number of data frames the stack will send from each
		/// transport layer protocol, per update. The default is 255,
		/// but decreasing it may reduce bus load at the expense of transfer time.
		/// @param[in] numberFrames The max number of frames to use
		void set_max_number_of_network_manager_protocol_frames_per_update(std::uint8_t numberFrames);

		/// @brief Returns the max number of data frames the stack will send from each
		/// transport layer protocol, per update. The default is 255,
		/// but decreasing it may reduce bus load at the expense of transfer time.
		/// @returns The max number of frames to use in transport protocols in each network manager update
		std::uint8_t get_max_number_of_network_manager_protocol_frames_per_update() const;

		/// @brief Set the number of packets per CTS message for TP sessions. The default
		/// is 16. Note that the receiving control function may not support this limitation, or choose
		/// to ignore it and use a different number of packets per CTS packet.
		/// @param[in] numberPackets The number of packets per CTS packet for TP sessions.
		void set_number_of_packets_per_cts_message(std::uint8_t numberPackets);

		/// @brief Get the number of packets per CTS packet for TP sessions.
		/// @returns The number of packets per CTS packet for TP sessions.
		std::uint8_t get_number_of_packets_per_cts_message() const;

	private:
		static constexpr std::uint8_t DEFAULT_BAM_PACKET_DELAY_TIME_MS = 50; ///< The default time between BAM frames, as defined by J1939

		std::uint32_t maxNumberTransportProtocolSessions = 4; ///< The max number of TP sessions allowed
		std::uint32_t minimumTimeBetweenTransportProtocolBAMFrames = DEFAULT_BAM_PACKET_DELAY_TIME_MS; ///< The configurable time between BAM frames
		std::uint8_t networkManagerMaxFramesToSendPerUpdate = 0xFF; ///< Used to control the max number of transport layer frames added to the driver queue per network manager update
		std::uint8_t numberOfPacketsPerDPOMessage = 16; ///< The number of packets per DPO message for ETP sessions
		std::uint8_t numberOfPacketsPerCTSMessage = 16; ///< The number of packets per CTS message for TP sessions
	};
} // namespace isobus

#endif // CAN_NETWORK_CONFIGURATION_HPP
