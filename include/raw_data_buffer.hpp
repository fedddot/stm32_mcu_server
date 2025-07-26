#ifndef	RAW_DATA_BUFFER_HPP
#define	RAW_DATA_BUFFER_HPP

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace service {
	template <std::size_t N>
	class RawDataBuffer {
	public:
		RawDataBuffer() {}
		RawDataBuffer(const RawDataBuffer&) = delete;
		RawDataBuffer& operator=(const RawDataBuffer&) = delete;
		virtual ~RawDataBuffer() noexcept = default;
		
		void push_back(const char ch) {
			
		}
		char pop_first() {

		}
		std::size_t size() const {
		}
		void clear() {
		}
	private:
		std::array<char, N> m_buffer;
	};
}

#endif // RAW_DATA_BUFFER_HPP