#pragma once

#include <vector>

namespace isobus
{
	// Note that this class currently only works for packets eight bytes or fewer, because that is
	// all the use-cases I needed.  The spec has a whole section on how data can span byte
	// boundaries, and how to align the bits when a data-type that is not a multiple of eight bits
	// crosses said boundaries.  Then every single parameter group uses padding to avoid those
	// cases!  The five-bit tractor command types have three bits of padding to make them exactly
	// one byte.  Why even specify the scheme if it isn't used?
	//
	// I wrote a huge amount of this file once, then somehow it vanished.  I don't know how!
	class ParameterGroupBuilder
	{
	private:
		std::size_t writeOffset = 0;
		std::size_t readOffset = 0;
		std::vector<std::uint8_t> buffer;

		std::size_t get_write_byte_offset() const
		{
			// Which byte to write to.
			return writeOffset / 8;
		}

		std::size_t get_write_bit_offset() const
		{
			// Which bit to write to in the current byte.
			return writeOffset % 8;
		}

		std::size_t get_read_byte_offset() const
		{
			// Which byte to read from.
			return readOffset / 8;
		}

		std::size_t get_read_bit_offset() const
		{
			// Which bit to read from in the current byte.
			return readOffset % 8;
		}

		bool write_bits(std::uint8_t const * data, std::size_t bits)
		{
			if (bits == 0)
			{
				// Err, OK, I guess...
				return true;
			}
			// First make a backup of the current write position, so that if there is an error
			// writing some data we can roll back the entire change.  This is replicated in the
			// string writing code because it has to roll back the entire string, not just the last
			// character.
			std::size_t byte = get_write_byte_offset();
			std::size_t offset = get_write_bit_offset();
			// Adjust the size, ensure the buffer is large enough, and initialise new values.
			writeOffset += bits;
			buffer.resize((writeOffset + 7) / 8, 255);

			// -------------------------------
			//  Stage naught - trivial cases.
			// -------------------------------

			// How much space is there left in this byte?
			std::size_t remaining = 8 - offset;
			if (remaining >= bits)
			{
				// Everything will fit in the current byte, which must mean there's at most one byte
				// of data to write.  Hence we put this version first because it covers more single
				// byte cases.
				std::uint8_t mask = (1 << offset) - 1;
				buffer[byte] = (buffer[byte] & mask) | (*data << offset);
				if (writeOffset % 8 != 0)
				{
					// Mask the top bits to hide excess data.
					buffer[byte] |= 255 << (writeOffset % 8);
				}
				return true;
			}

			// Whole bytes (one byte is often handled above.)
			if (offset == 0 && bits % 8 == 0)
			{
				// Whole bytes, which are aligned.
				do
				{
					buffer[byte] = *data++;
					byte += 1;
					bits -= 8;
				}
				while (bits);
				// No additional masking required in this case.
				return true;
			}

			// ----------------------------------------
			//  Stage one - fill out the current byte.
			// ----------------------------------------

			// All this data can come from the first byte of the input.
			// Everything will fit in the current byte, which must mean there's at most one byte
			// of data to write.  Hence we put this version first because it covers more single
			// byte cases.
			std::uint8_t mask = (1 << offset) - 1;
			buffer[byte] = (buffer[byte] & mask) | (*data << offset);
			bits -= remaining;
			++byte;

			// -------------------------------
			//  Stage two - copy whole bytes.
			// -------------------------------

			while (bits > 8)
			{
				buffer[byte] = (*data >> remaining);
				++data;
				buffer[byte] = (buffer[byte] & mask) | (*data << offset);
				bits -= 8;
				++byte;
			}

			// -------------------------------
			//  Stage three - copy whole bytes.
			// -------------------------------

			// `bits` is number of bits left.  It may or may not span a byte boundary in the input.
			if (bits == 0)
			{
				// Nothing left to copy.
				if (writeOffset % 8 != 0)
				{
					// Mask the top bits to hide excess data.
					buffer[byte] |= 255 << (writeOffset % 8);
				}
				return true;
			}

			// I'm sure this code can be compressed with the code above.
			buffer[byte] = (*data >> remaining);
			if (offset < bits)
			{
				// The final output spans two bytes of input.
				++data;
				buffer[byte] = (buffer[byte] & mask) | (*data << offset);
			}
			if (writeOffset % 8 != 0)
			{
				// Mask the top bits to hide excess data.
				buffer[byte] |= 255 << (writeOffset % 8);
			}

			return true;
		}
		
		bool read_bits(std::uint8_t * data, std::size_t bits)
		{
			std::size_t byte = get_read_byte_offset();
			std::size_t input = get_read_bit_offset();
			std::size_t remaining = 8 - input;
			std::size_t output = 0;
			std::size_t space = 8 - output;
			// Mark as read, even though we actually haven't yet.
			readOffset += bits;
			if (readOffset > writeOffset)
			{
				// Trying to read too much data.
				readOffset -= bits;
				return false;
			}
			// Initialise the current destination byte.
			*data = 0;
			while (bits)
			{
				// Move in the new data.
				*data = (*data & (255 >> space)) | ((buffer[byte] >> input) << output);
				if (space >= bits)
				{
					space -= bits;
					break;
				}
				if (space > remaining)
				{
					space -= remaining;
					output = 8 - space;
					remaining = 0;
					input = 0;
					++byte;
				}
				else if (remaining > space)
				{
					bits -= space;
					remaining -= space;
					input = 8 - remaining;
					space = 8;
					output = 0;
					++data;
					*data = 0;
				}
				else
				{
					bits -= space;
					// Perfectly matched.
					remaining = 8;
					input = 0;
					space = 8;
					output = 0;
					++data;
					*data = 0;
					++byte;
				}
			}
			if (space)
			{
				// Bits left to mask.
				*data = *data & (255 >> space);
			}
			return true;
		}

	public:
		ParameterGroupBuilder() :
		  buffer()
		{
		}

		ParameterGroupBuilder(std::vector<std::uint8_t> const &data) :
		  buffer()
		{
			buffer = data;
			writeOffset = data.size() * 8;
		}

		std::size_t get_written_bits() const
		{
			return writeOffset;
		}

		std::size_t get_written_bytes() const
		{
			return (writeOffset + 7) / 8;
		}

		std::size_t get_read_bits() const
		{
			return readOffset;
		}

		std::size_t get_read_bytes() const
		{
			return (readOffset + 7) / 8;
		}

		template <typename T>
		bool write(T const & data)
		{
			return write_bits((std::uint8_t const *)&data, sizeof (T) * 8);
		}

		template <typename T>
		bool write(T const & data, std::size_t bits)
		{
			return write_bits((std::uint8_t const *)&data, bits);
		}

		template <>
		bool write<bool>(bool const & data)
		{
			std::uint8_t bits = data ? 255 : 0;
			return write_bits(&bits, 1);
		}

		template <>
		bool write<char const *>(char const * const & data)
		{
			// What should the default for including NULL be?
			return write((std::uint8_t const *)data, false);
		}

		template <>
		bool write<char *>(char * const & data)
		{
			// What should the default for including NULL be?
			return write((std::uint8_t const *)data, false);
		}

		template <>
		bool write<std::uint8_t const *>(std::uint8_t const * const & data)
		{
			// What should the default for including NULL be?
			return write((std::uint8_t const *)data, false);
		}

		template <>
		bool write<std::uint8_t *>(std::uint8_t * const & data)
		{
			// What should the default for including NULL be?
			return write((std::uint8_t const *)data, false);
		}

		bool write(char const * data, bool includeNull)
		{
			return write((std::uint8_t const *)data, includeNull);
		}

		bool write(char * data, bool includeNull)
		{
			return write((std::uint8_t const *)data, includeNull);
		}

		bool write(std::uint8_t const * data, bool includeNull)
		{
			// Base case.  Write each byte separately so they don't get put in little-endian, which
			// makes no sense for strings.
			std::size_t revert = writeOffset;
			while (*data)
			{
				if (!write_bits(data, 8))
				{
					writeOffset = revert;
					return false;
				}
				++data;
			}
			if (includeNull)
			{
				std::uint8_t naught = 0;
				if (!write_bits(&naught, 8))
				{
					writeOffset = revert;
					return false;
				}
			}
			return true;
		}

		bool write(std::uint8_t * data, bool includeNull)
		{
			return write((std::uint8_t const *)data, includeNull);
		}

		bool pad(std::size_t bits, bool value = true)
		{
			std::uint8_t data = value ? 255 : 0;
			std::size_t byte = get_write_byte_offset();
			std::size_t offset = get_write_bit_offset();
			std::size_t remaining = 8 - offset;
			std::uint8_t mask = 255 >> remaining;
			writeOffset += bits;
			buffer.resize((writeOffset + 7) / 8, 255);
			for (;;)
			{
				buffer[byte] = (buffer[byte] & mask) | (data << offset);
				if (remaining > bits)
				{
					// Weirdly we need a second masking here, to keep untouched bits as `1`.
					buffer[byte] |= 255 << (bits + offset);
					break;
				}
				else if (remaining == bits)
				{
					// Done.
					break;
				}
				else
				{
					bits -= remaining;
					offset = 0;
					remaining = 8;
					mask = 0;
					++byte;
				}
			}
			return true;
		}

		template <typename T>
		bool read(T & data)
		{
			return read_bits((std::uint8_t *)&data, sizeof (T) * 8);
		}

		template <typename T>
		bool read(T & data, std::size_t bits)
		{
			// Clear the memory, since we may not be reading the full width.
			memset(&data, 0, sizeof(T));
			return read_bits((std::uint8_t *)&data, bits);
		}

		template <>
		bool read<bool>(bool & data)
		{
			std::uint8_t bits = 0;
			if (read_bits(&bits, 1))
			{
				data = !!bits;
				return true;
			}
			data = false;
			return false;
		}

		template <>
		bool read<char *>(char * & data)
		{
			// Read until NULL.
			return read<std::uint8_t*>((std::uint8_t * &)data);
		}

		template <>
		bool read<std::uint8_t *>(std::uint8_t * & data)
		{
			// Read until NULL.
			std::size_t revert = readOffset;
			// Don't modify `data`!
			std::uint8_t * ptr = data;
			for ( ; ; )
			{
				if (!read_bits(ptr, 8))
				{
					readOffset = revert;
					*data = '\0';
					return false;
				}
				if (*ptr == 0)
				{
					// Found a NULL byte.
					break;
				}
				++ptr;
			}
			return true;
		}

		template <>
		bool read<char *>(char * & data, std::size_t bits)
		{
			// It is a bit awkward to specify how much of a string to read.
			return read((std::uint8_t * &)data, bits);
		}

		template <>
		bool read<std::uint8_t *>(std::uint8_t * & data, std::size_t bits)
		{
			if (bits % 8 != 0)
			{
				// This requires normal string sizes.
				return false;
			}
			// Don't modify `data`!
			std::uint8_t * ptr = data;
			// Don't write NULL, just assume the caller handles that.
			std::size_t revert = readOffset;
			while (bits)
			{
				if (!read_bits(ptr, 8))
				{
					readOffset = revert;
					*data = '\0';
					return false;
				}
				bits -= 8;
				++ptr;
			}
			return true;
		}

		bool skip(std::size_t bits)
		{
			readOffset += bits;
			if (readOffset > writeOffset)
			{
				readOffset -= bits;
				return false;
			}
			return true;
		}

		std::size_t get_data(std::vector<std::uint8_t> &output)
		{
			size_t size = get_written_bytes();
			output = buffer;
			output.resize(size);
			return size;
		}

		void reset_read()
		{
			readOffset = 0;
		}

		void reset_write()
		{
			readOffset = 0;
			writeOffset = 0;
			buffer.clear();
		}
	};
}

