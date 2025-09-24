#ifndef PB_STEPPER_REQUEST_PARSER_HPP
#define PB_STEPPER_REQUEST_PARSER_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>

#include "pb_decode.h"
#include "stepper_request.hpp"
#include "stepper_service.pb.h"
#include "stepper_types.hpp"

namespace ipc {
    class PbStepperRequestParser {
    public:
        PbStepperRequestParser() = default;
        PbStepperRequestParser(const PbStepperRequestParser &) = default;
        PbStepperRequestParser &operator=(const PbStepperRequestParser &) = default;
        virtual ~PbStepperRequestParser() noexcept = default;
        service::StepperRequest operator()(const std::vector<uint8_t> &data) const {
            stepper_service_StepperRequest request = stepper_service_StepperRequest_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(data.data(), data.size());
            if (!pb_decode(&stream, stepper_service_StepperRequest_fields, &request)) {
                throw std::runtime_error("failed to decode StepperRequest: " + std::string(PB_GET_ERROR(&stream)));
            }
            switch (request.which_request) {
            case stepper_service_StepperRequest_enable_request_tag:
                return parse_enable_request(request);
            case stepper_service_StepperRequest_steps_request_tag:
                return parse_steps_request(request);
            default:
                throw std::runtime_error("unsupported StepperRequest type");
            }
        }            
    private:
        static service::StepperRequest parse_steps_request(const stepper_service_StepperRequest &pb_request) {
            const auto direction_mapping = std::map<stepper_service_StepperDirection, service::Direction> {
                {stepper_service_StepperDirection_CW, service::Direction::CLOCKWISE},
                {stepper_service_StepperDirection_CCW, service::Direction::COUNTERCLOCKWISE}
            };
            const auto duration = service::Duration(pb_request.request.steps_request.step_duration_us);
            const auto steps = static_cast<int>(pb_request.request.steps_request.steps_number);
            return service::StepperRequest(
                service::StepperRequest::Type::STEPS,
                direction_mapping.at(pb_request.request.steps_request.direction), steps,
                duration
            );
        }
        static service::StepperRequest parse_enable_request(const stepper_service_StepperRequest &pb_request) {
            return service::StepperRequest(service::StepperRequest::Type::ENABLE);
        }
    };
} // namespace ipc
#endif // PB_STEPPER_REQUEST_PARSER_HPP
                    