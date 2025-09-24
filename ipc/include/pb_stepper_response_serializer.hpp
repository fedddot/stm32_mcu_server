#ifndef PB_STEPPER_RESPONSE_SERIALIZER_HPP
#define PB_STEPPER_RESPONSE_SERIALIZER_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>

#include "pb_encode.h"
#include "stepper_response.hpp"
#include "stepper_service.pb.h"
#include "stepper_types.hpp"

namespace ipc {
    class PbStepperResponseSerializer {
    public:
        PbStepperResponseSerializer() = default;
        PbStepperResponseSerializer(const PbStepperResponseSerializer &) = default;
        PbStepperResponseSerializer &operator=(const PbStepperResponseSerializer &) = default;
        virtual ~PbStepperResponseSerializer() noexcept = default;
        std::vector<uint8_t> operator()(const service::StepperResponse& response) const {
            using ResponseResult = service::StepperResponse::Result;
            const auto result_mapping = std::map<ResponseResult, stepper_service_StepperResultCode> {
                { ResponseResult::SUCCESS, stepper_service_StepperResultCode_SUCCESS },
                { ResponseResult::FAILURE, stepper_service_StepperResultCode_FAILURE },
            };
            const auto state_mapping = std::map<service::State, stepper_service_StepperStatus> {
                { service::State::DISABLED, stepper_service_StepperStatus_DISABLED },
                { service::State::ENABLED, stepper_service_StepperStatus_ENABLED },
            };
            const auto pb_msg = response.error_message().has_value() ? response.error_message().value() : "";
            const auto pb_response = stepper_service_StepperResponse {
                .result = result_mapping.at(response.result()),
                .stepper_status = state_mapping.at(response.state()),
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
            if (!pb_encode(&ostream, stepper_service_StepperResponse_fields, &pb_response)) {
                throw std::runtime_error("failed to encode StepperResponse into protocol buffer: " + std::string(PB_GET_ERROR(&ostream)));
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
#endif // PB_STEPPER_RESPONSE_SERIALIZER_HPP
                    