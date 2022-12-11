//================================================================================================
/// @file nmea2000_fast_packet_protocol.hpp
///
/// @brief A protocol that handles the NMEA 2000 fast packet protocol.
///
/// @author Adrian Del Grosso
///
/// @copyright 2022 Adrian Del Grosso
//================================================================================================

#ifndef NMEA2000_FAST_PACKET_PROTOCOL_HPP
#define NMEA2000_FAST_PACKET_PROTOCOL_HPP

#include "isobus/isobus/can_internal_control_function.hpp"
#include "isobus/isobus/can_protocol.hpp"
#include "isobus/isobus/can_managed_message.hpp"

#include <mutex>

namespace isobus
{
	class FastPacketProtocol : public CANLibProtocol
	{
	public:
		/// @brief An object for tracking fast packet session state
		class FastPacketProtocolSession
		{
		public:
			/// @brief Enumerates the possible session directions, Rx or Tx
			enum class Direction
			{
				Transmit, ///< We are transmitting a message
				Receive ///< We are receving a message
			};

			/// @brief A useful way to compare sesson objects to each other for equality
			bool operator==(const FastPacketProtocolSession &obj);

		private:
			friend class FastPacketProtocol; ///< Allows the TP manager full access

			/// @brief The constructor for a TP session
			/// @param[in] sessionDirection Tx or Rx
			/// @param[in] canPortIndex The CAN channel index for the session
			FastPacketProtocolSession(Direction sessionDirection, std::uint8_t canPortIndex);

			/// @brief The destructor for a TP session
			~FastPacketProtocolSession();

			CANLibManagedMessage sessionMessage; ///< A CAN message is used in the session to represent and store data like PGN
			TransmitCompleteCallback sessionCompleteCallback; ///< A callback that is to be called when the session is completed
			DataChunkCallback frameChunkCallback; ///< A callback that might be used to get chunks of data to send
			void *parent; ///< A generic context variable that helps identify what object callbacks are destined for. Can be nullptr
			std::uint32_t timestamp_ms; ///< A timestamp used to track session timeouts
			std::uint16_t lastPacketNumber; ///< The last processed sequence number for this set of packets
			std::uint8_t packetCount; ///< The total number of packets to receive or send in this session
			std::uint8_t processedPacketsThisSession; ///< The total processed packet count for the whole session so far
			std::uint8_t sequenceNumber; ///< The sequence number for this PGN
			const Direction sessionDirection; ///< Represents Tx or Rx session
		};

		static FastPacketProtocol Protocol; ///< Static instance of the protocol

		/// @brief A generic way to initialize a protocol
		/// @details The network manager will call a protocol's initialize function
		/// when it is first updated, if it has yet to be initialized.
		void initialize(CANLibBadge<CANNetworkManager>) override;

		/// @brief A generic way for a protocol to process a received message
		/// @param[in] message A received CAN message
		void process_message(CANMessage *const message) override;

		/// @brief Used to send CAN messages using fast packet
		/// @details You have to use this function instead of the network manager
		/// because otherwise the CAN stack has no way of knowing to send your message
		/// with FP instead of TP.
		/// @param[in] parameterGroupNumber The PGN of the message
		/// @param[in] data The data to be sent
		/// @param[in] messageLength The length of the data to be sent
		/// @param[in] source The source control function
		/// @param[in] destination The destination control function
		/// @param[in] priority The priority to encode in the IDs of the component CAN messages
		/// @param[in] transmitCompleteCallback A callback for when the protocol completes its work
		/// @param[in] parentPointer A generic context object for the tx complete and chunk callbacks
		/// @param[in] frameChunkCallback A callback to get some data to send
		/// @returns true if the message was accepted by the protocol for processing
		bool send_multipacket_message(std::uint32_t parameterGroupNumber,
		                              const std::uint8_t *data,
		                              std::uint8_t messageLength,
		                              InternalControlFunction *source,
		                              ControlFunction *destination,
		                              CANIdentifier::CANPriority priority = CANIdentifier::CANPriority::PriorityDefault6,
		                              TransmitCompleteCallback txCompleteCallback = nullptr,
		                              void *parentPointer = nullptr,
		                              DataChunkCallback frameChunkCallback = nullptr);

		/// @brief This will be called by the network manager on every cyclic update of the stack
		void update(CANLibBadge<CANNetworkManager>) override;

	private:

		/// @brief Ends a session and cleans up the memory associated with its metadata
		/// @param[in] session The session to close
		void close_session(FastPacketProtocolSession *session);

		/// @brief Returns a session that matches the parameters, if one exists
		/// @param[in,out] session The returned session
		/// @param[in] parameterGroupNumber The PGN
		/// @param[in] source The session source control function
		/// @param[in] destination The sesssion destination control function
		/// @returns `true` if a session was found that matches, otherwise `false`
		bool get_session(FastPacketProtocolSession *&returnedSession, std::uint32_t parameterGroupNumber, ControlFunction *source, ControlFunction *destination);

		/// @brief The network manager calls this to see if the protocol can accept a non-raw CAN message for processing
		/// @param[in] parameterGroupNumber The PGN of the message
		/// @param[in] data The data to be sent
		/// @param[in] messageLength The length of the data to be sent
		/// @param[in] source The source control function
		/// @param[in] destination The destination control function
		/// @param[in] transmitCompleteCallback A callback for when the protocol completes its work
		/// @param[in] parentPointer A generic context object for the tx complete and chunk callbacks
		/// @param[in] frameChunkCallback A callback to get some data to send
		/// @returns true if the message was accepted by the protocol for processing
		bool protocol_transmit_message(std::uint32_t parameterGroupNumber,
		                               const std::uint8_t *data,
		                               std::uint32_t messageLength,
		                               ControlFunction *source,
		                               ControlFunction *destination,
		                               TransmitCompleteCallback transmitCompleteCallback,
		                               void *parentPointer,
		                               DataChunkCallback frameChunkCallback) override;

		/// @brief Updates in-progress sessions
		/// @param[in] session The session to process
		void update_state_machine(FastPacketProtocolSession *session);

		static constexpr std::uint32_t FP_MIN_PARAMETER_GROUP_NUMBER = 0x1F000; ///< Start of PGNs that can be received via Fast Packet
		static constexpr std::uint32_t FP_MAX_PARAMETER_GROUP_NUMBER = 0x1FFFF; ///< End of PGNs that can be received via Fast Packet
		static constexpr std::uint8_t MAX_PROTOCOL_MESSAGE_LENGTH = 223; ///< Max message length based on there being 5 bits of sequence data
		static constexpr std::uint8_t FRAME_COUNTER_BIT_MASK = 0x1F; ///< Bit mask for masking out the frame counter
		static constexpr std::uint8_t SEQUENCE_NUMBER_BIT_MASK = 0x07; ///< Bit mask for masking out the sequence number bits
		static constexpr std::uint8_t SEQUENCE_NUMBER_BIT_OFFSET = 0x05; ///< The bit offset into the first byte of data to get the seq number
		static constexpr std::uint8_t PROTOCOL_BYTES_PER_FRAME = 7; ///< The number of payload bytes per frame for all but the first message, which has 6

		std::list<FastPacketProtocolSession *> activeSessions; ///< A list of all active TP sessions
		std::mutex sessionMutex; ///< A mutex to lock the sessions list in case someone starts a Tx while the stack is processing sessions
	};

} // namespace isobus

#endif // NMEA2000_FAST_PACKET_PROTOCOL_HPP
