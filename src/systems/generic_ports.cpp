//TODO: Translate error messages!
#include "generic_ports.h"

namespace sydevs::generics {

    std::pair<data_mode, data_goal> generic_ports::get_node_type(const std::string &port_name) const {
        return port_types.at(port_name);
    }

    void generic_ports::add_port(const std::string &port_name, const node_interface &interface, data_mode mode,
                                 data_goal goal) {
        switch (mode) {
            case data_mode::flow:
                switch (goal) {
                    case data_goal::input:
                        port_types[port_name] = std::make_pair(flow, input);
                        flow_input_ports.try_emplace(port_name, port_name, interface);
                        break;
                    case data_goal::output:
                        port_types[port_name] = std::make_pair(flow, output);
                        flow_output_ports.try_emplace(port_name, port_name, interface);
                        break;
                }
                break;
            case data_mode::message:
                switch (goal) {
                    case data_goal::input:
                        port_types[port_name] = std::make_pair(message, input);
                        message_input_ports.try_emplace(port_name, port_name, interface);
                        break;
                    case data_goal::output:
                        port_types[port_name] = std::make_pair(message, output);
                        message_output_ports.try_emplace(port_name, port_name, interface);
                        break;
                }
                break;
        }
    }

    void generic_ports::set_port(const std::string &port_name, const std::any& value) {

        data_mode mode;
        data_goal goal;
        std::tie(mode, goal) = get_node_type(port_name);

        if (goal != data_goal::output) throw std::runtime_error("Port "+ port_name +" ist kein Ausgang!");

        switch (mode) {
            case data_mode::flow:
                flow_output_ports.at(port_name).assign(std::make_shared<std::any>(value));
                break;
            case data_mode::message:
                message_output_ports.at(port_name).send(std::make_shared<std::any>(value));
                break;
        }
    }

    port<flow, input, std::shared_ptr<std::any>>& generic_ports::flow_input_port(const std::string& port_name) const
    {
        return const_cast<port<flow, input, std::shared_ptr<std::any>>&>(flow_input_ports.at(port_name));
    }

    port<flow, output, std::shared_ptr<std::any>>& generic_ports::flow_output_port(const std::string& port_name) const
    {
        return const_cast<port<flow, output, std::shared_ptr<std::any>>&>(flow_output_ports.at(port_name));
    }

    port<message, input, std::shared_ptr<std::any>>& generic_ports::message_input_port(const std::string& port_name) const
    {
        return const_cast<port<message, input, std::shared_ptr<std::any>>&>(message_input_ports.at(port_name));
    }

    port<message, output, std::shared_ptr<std::any>>& generic_ports::message_output_port(const std::string& port_name) const
    {
        return const_cast<port<message, output, std::shared_ptr<std::any>>&>(message_output_ports.at(port_name));
    }

    bool generic_ports::port_received(const std::string &port_name) {

        data_mode mode;
        data_goal goal;
        std::tie(mode, goal) = get_node_type(port_name);

        if (goal != data_goal::input) throw std::runtime_error("Port "+ port_name +" ist kein Eingang!");
        if (mode != data_mode::message) throw std::runtime_error("Port "+ port_name +" ist kein Message Port!");

        return message_input_ports.at(port_name).received();
    }
}

