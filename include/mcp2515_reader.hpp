#ifndef MCP2515_READER_HPP
#define MCP2515_READER_HPP

#include "data_reader.hpp"

namespace ipc {
	template <typename Data>
	class Mcp2515Reader : public DataReader<Data> {
	public:
		explicit Mcp2515Reader() noexcept = default;
		~Mcp2515Reader() noexcept override = default;

		Mcp2515Reader(const Mcp2515Reader&) = delete;
		Mcp2515Reader& operator=(const Mcp2515Reader&) = delete;
		Mcp2515Reader(Mcp2515Reader&&) noexcept = delete;
		Mcp2515Reader& operator=(Mcp2515Reader&&) noexcept = delete;

		Data read() override {
			// TODO: Implement MCP2515 CAN message reading
			return Data{};
		}

	private:
		// TODO: Add MCP2515 device handle and configuration
	};
}

#endif // MCP2515_READER_HPP
