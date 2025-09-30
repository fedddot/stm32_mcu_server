#ifndef PB_SEGWAY_RESPONSE_SERIALIZER_HPP
#define PB_SEGWAY_RESPONSE_SERIALIZER_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>

#include "pb_encode.h"
#include "segway_response.hpp"
#include "segway_service.pb.h"
#include "segway_types.hpp"

namespace ipc {
    class PbSegwayResponseSerializer {
    public:
        PbSegwayResponseSerializer() = default;
        PbSegwayResponseSerializer(const PbSegwayResponseSerializer &) = default;
        PbSegwayResponseSerializer &operator=(const PbSegwayResponseSerializer &) = default;
        virtual ~PbSegwayResponseSerializer() noexcept = default;
        std::vector<uint8_t> operator()(const service::SegwayResponse& response) const {
            using ResponseResult = service::SegwayResponse::Result;
            const auto result_mapping = std::map<ResponseResult, segway_service_SegwayResultCode> {
                { ResponseResult::SUCCESS, segway_service_SegwayResultCode_SUCCESS },
                { ResponseResult::FAILURE, segway_service_SegwayResultCode_FAILURE },
            };
            const auto state_mapping = std::map<service::State, segway_service_SegwayStatus> {
                { service::State::DISABLED, segway_service_SegwayStatus_DISABLED },
                { service::State::ENABLED, segway_service_SegwayStatus_ENABLED },
            };
            const auto pb_msg = response.error_message().has_value() ? response.error_message().value() : "";
            const auto pb_response = segway_service_SegwayResponse {
                .result = result_mapping.at(response.result()),
                .segway_status = state_mapping.at(response.state()),
                .error_message = pb_callback_t {
                    .funcs = {
                        .encode = &encode_string,
                    },
                    .arg = const_cast<std::string *>(&pb_msg),
                }
            };

            enum: int { BUFF_SIZE = 256UL };
            pb_byte_t buffer[BUFF_SIZE];
            pb_ostream_t ostream = pb_ostream_from_buffer(
                buffer,
                BUFF_SIZE
            );
            if (!pb_encode(&ostream, segway_service_SegwayResponse_fields, &pb_response)) {
                throw std::runtime_error("failed to encode SegwayResponse into protocol buffer: " + std::string(PB_GET_ERROR(&ostream)));
            }
            return std::vector<std::uint8_t>((const char *)buffer, (const char *)buffer + ostream.bytes_written);
        }            
    private:
        static bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
            if (!arg || !*arg) {
                throw std::runtime_error("encode_string called with null arg");
            }
            const auto str = *static_cast<const std::string *>(*arg);
            if (!pb_encode_tag_for_field(stream, field)) {
                return false;
            }
            return pb_encode_string(stream, (const pb_byte_t *)(str.c_str()), str.size());
        }
    };
} // namespace ipc
#endif // PB_SEGWAY_RESPONSE_SERIALIZER_HPP
                    