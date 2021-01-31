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
    /**
     * This class holds all port object of the node. This class is in combination with a SyDEVS node and provide an base class for all generic_nodes
     *
     * It also provide an interface for other classes to access and modife the ports.
     */
    class generic_ports {
    public:
        /**
         * Create an empty generic_port object. No ports are initialized.
         * @param no_ports
         */
        generic_ports() = default;
        virtual ~generic_ports() = default;

        /**
         * Lookup the data_mode and data_goal of a port
         * @param port_name Name of the port
         * @return A pair of the data_mode(flow or message) and the data_goal(input or output)
         */
        std::pair<data_mode, data_goal> get_node_type(const std::string& port_name) const;

        /**
         * Create and initialize a port with the given properties (data_mode and data_goal)
         * @param port_name Name of the port
         * @param interface Interface of the node to which the port is added (from SyDEVS)
         * @param mode data_mode::flow or data_mode::message (from SyDEVS)
         * @param goal data_goal::input or data_goal::output (from SyDEVS)
         */
        void add_port(const std::string& port_name, const node_interface& interface, data_mode mode, data_goal goal);

        /**
         * Set the value of the port. To set it the value need to be converted to a std::any.
         * @param port_name Name of the port
         * @param value Value to be set. The type can be anything as long it is casted to std::any
         */
        virtual void set_port(const std::string& port_name, const std::any& value);

        ///@{
        /**
         * Return the port object of the given port
         * @param port_name Name of the port
         */
        [[nodiscard]] port<flow, input, std::shared_ptr<std::any>>& flow_input_port(const std::string& port_name) const;
        [[nodiscard]] port<flow, output, std::shared_ptr<std::any>>& flow_output_port(const std::string& port_name) const;
        [[nodiscard]] port<message, input, std::shared_ptr<std::any>>& message_input_port(const std::string& port_name) const;
        [[nodiscard]] port<message, output, std::shared_ptr<std::any>>& message_output_port(const std::string& port_name) const;
        ///@}

        /**
         * Check if new data is available at the port
         * @param port_name Name of the Port
         * @return True if the there is new data at the port
         */
        bool port_received(const std::string& port_name);

        /**
         * Return the value of the port. Throws an exception if the type of the value of the port can't be converted to the ReturnType.
         * @tparam ReturnType Type of the value of the port
         * @param port_name Name of the port
         * @return Value of the Port
         */
        template<typename ReturnType>
        ReturnType get_port(const std::string& port_name);

        ///@{
        /**
         * Specialised version of the get_port function. Currently it is only used by the lua interpreter
         * @param port_name Name of the port
         * @return Value of the port
         */
        virtual std::string get_string_port(const std::string& port_name);
        virtual double get_double_port(const std::string& port_name);
        virtual duration get_duration_port(const std::string& port_name);
        virtual bool get_bool_port(const std::string& port_name);
        ///@}

    private:
        std::map<std::string, systems::port<flow, input, std::shared_ptr<std::any>>> flow_input_ports;
        std::map<std::string, systems::port<flow, output, std::shared_ptr<std::any>>> flow_output_ports;
        std::map<std::string, systems::port<message, input, std::shared_ptr<std::any>>> message_input_ports;
        std::map<std::string, systems::port<message, output, std::shared_ptr<std::any>>> message_output_ports;

        std::map<std::string, std::pair<data_mode, data_goal>> port_types;
    };
}
#endif //SYDEVS_GENERICS_GENERIC_PORT_H
