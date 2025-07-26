#ifndef	RAW_DATA_BUFFER_HPP
#define	RAW_DATA_BUFFER_HPP

#include <array>
#include <cstddef>
#include <stdexcept>

namespace service {
	template <std::size_t N>
	class RawDataBuffer {
	public:
		RawDataBuffer() : m_head(0), m_tail(0), m_full(false) {}
		RawDataBuffer(const RawDataBuffer&) = delete;
		RawDataBuffer& operator=(const RawDataBuffer&) = delete;
		virtual ~RawDataBuffer() noexcept = default;

		void push_back(const char ch) {
			if (m_full) {
				throw std::overflow_error("RawDataBuffer is full");
			}
			m_buffer[m_tail] = ch;
			m_tail = (m_tail + 1) % N;
			if (m_tail == m_head) {
				m_full = true;
			}
		}

		char pop_first() {
			if (empty()) {
				throw std::underflow_error("RawDataBuffer is empty");
			}
			char value = m_buffer[m_head];
			m_head = (m_head + 1) % N;
			m_full = false;
			return value;
		}

		char get(const std::size_t index) const {
			if (index >= size()) {
				throw std::out_of_range("Index out of range in RawDataBuffer");
			}
			return m_buffer[(m_head + index) % N];
		}
		
		std::size_t size() const {
			if (m_full) {
				return N;
			}
			if (m_tail >= m_head) {
				return m_tail - m_head;
			} else {
				return N - m_head + m_tail;
			}
		}

		void clear() {
			m_head = 0;
			m_tail = 0;
			m_full = false;
		}

		bool empty() const {
			return (!m_full && (m_head == m_tail));
		}

		
	private:
		std::array<char, N> m_buffer;
		std::size_t m_head;
		std::size_t m_tail;
		bool m_full;
	};
}

#endif // RAW_DATA_BUFFER_HPP