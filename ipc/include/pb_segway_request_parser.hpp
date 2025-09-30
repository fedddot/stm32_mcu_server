#ifndef PB_SEGWAY_REQUEST_PARSER_HPP
#define PB_SEGWAY_REQUEST_PARSER_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>

#include "pb_decode.h"
#include "segway_request.hpp"
#include "segway_service.pb.h"
#include "segway_types.hpp"

namespace ipc {
    class PbSegwayRequestParser {
    public:
        PbSegwayRequestParser() = default;
        PbSegwayRequestParser(const PbSegwayRequestParser &) = default;
        PbSegwayRequestParser &operator=(const PbSegwayRequestParser &) = default;
        virtual ~PbSegwayRequestParser() noexcept = default;
        service::SegwayRequest operator()(const std::vector<uint8_t> &data) const {
            segway_service_SegwayRequest request = segway_service_SegwayRequest_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(data.data(), data.size());
            if (!pb_decode(&stream, segway_service_SegwayRequest_fields, &request)) {
                throw std::runtime_error("failed to decode SegwayRequest: " + std::string(PB_GET_ERROR(&stream)));
            }
            switch (request.which_request) {
            case segway_service_SegwayRequest_enable_request_tag:
                return parse_enable_request(request);
            case segway_service_SegwayRequest_steps_request_tag:
                return parse_steps_request(request);
            default:
                throw std::runtime_error("unsupported SegwayRequest type");
            }
        }            
    private:
        static service::SegwayRequest parse_steps_request(const segway_service_SegwayRequest &pb_request) {
            const auto direction_mapping = std::map<segway_service_SegwayDirection, service::Direction> {
                {segway_service_SegwayDirection_CW, service::Direction::CLOCKWISE},
                {segway_service_SegwayDirection_CCW, service::Direction::COUNTERCLOCKWISE}
            };
            const auto duration = service::Duration(pb_request.request.steps_request.step_duration_us);
            const auto steps = static_cast<int>(pb_request.request.steps_request.steps_number);
            return service::SegwayRequest(
                service::SegwayRequest::Type::STEPS,
                direction_mapping.at(pb_request.request.steps_request.direction), steps,
                duration
            );
        }
        static service::SegwayRequest parse_enable_request(const segway_service_SegwayRequest &pb_request) {
            return service::SegwayRequest(service::SegwayRequest::Type::ENABLE);
        }
    };
} // namespace ipc
#endif // PB_SEGWAY_REQUEST_PARSER_HPP
                    