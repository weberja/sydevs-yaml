#ifndef SYDEVS_GENERICS_GENERIC_PORT_H
#define SYDEVS_GENERICS_GENERIC_PORT_H

#include <map>
#include <any>
#include <string>
#include <sydevs/systems/port.h>
#include <sydevs/core/quantity.h>
#include <sydevs/systems/collection_node.h>

namespace sydevs::generics {
    using namespace sydevs;
    using namespace sydevs::systems;

    class generic_ports {
    public:

        explicit generic_ports(bool no_ports=false);

        [[nodiscard]] bool portless() const;

        std::pair<data_mode, data_goal> get_node_type(const std::string& port_name) const;

        void add_port(const std::string& port_name, const node_interface& interface, data_mode mode, data_goal goal);

        virtual void set_port(const std::string& port_name, const std::any& value);

        [[nodiscard]] port<flow, input, std::shared_ptr<std::any>>& flow_input_port(const std::string& port_name) const;
        [[nodiscard]] port<flow, output, std::shared_ptr<std::any>>& flow_output_port(const std::string& port_name) const;
        [[nodiscard]] port<message, input, std::shared_ptr<std::any>>& message_input_port(const std::string& port_name) const;
        [[nodiscard]] port<message, output, std::shared_ptr<std::any>>& message_output_port(const std::string& port_name) const;

        bool port_received(const std::string& port_name);

        template<typename ReturnType>
        ReturnType get_port(const std::string& port_name);

        virtual std::string get_string_port(const std::string& port_name);

        virtual double get_double_port(const std::string& port_name);
        virtual duration get_duration_port(const std::string& port_name);
        virtual bool get_bool_port(const std::string& port_name);

    private:
        bool no_ports;

        std::map<std::string, systems::port<flow, input, std::shared_ptr<std::any>>> flow_input_ports;
        std::map<std::string, systems::port<flow, output, std::shared_ptr<std::any>>> flow_output_ports;
        std::map<std::string, systems::port<message, input, std::shared_ptr<std::any>>> message_input_ports;
        std::map<std::string, systems::port<message, output, std::shared_ptr<std::any>>> message_output_ports;

        std::map<std::string, std::pair<data_mode, data_goal>> port_types;
    };
}
#endif //SYDEVS_GENERICS_GENERIC_PORT_H
