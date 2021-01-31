#ifndef SYDEVS_REALTIME_GENERIC_CLOSED_SYSTEM_H
#define SYDEVS_REALTIME_GENERIC_CLOSED_SYSTEM_H

#include <systems/generic_ports.h>
#include <sydevs/systems/composite_node.h>
#include <systems/generic_atomic_node.h>

#include <generics/lua.h>
#include <generics/simulation_config.h>

#include <utility>

namespace sydevs::systems {

    using namespace generics;
    using namespace std;

    class generic_closed_system : public composite_node, public generic_ports {
    public:
        generic_closed_system(const std::string& node_name, const node_context& external_context);
        generic_closed_system(const string &node_name, const node_context &external_context,
                              node_config node_config);
        ~generic_closed_system() override = default;

        // Components
        std::map<std::string, std::shared_ptr<system_node>> components;
    private:
        void create_from_config(const std::string &node_name, const node_context &external_context);

        simulation_config sim_config;
        node_config config;
    };

    inline generic_closed_system::generic_closed_system(const string &node_name, const node_context &external_context,
                                                        node_config node_config) :
                                                        composite_node(node_name, external_context),
                                                        generic_ports(),
                                                        sim_config(),
                                                        config(std::move(node_config)){
        create_from_config(node_name, external_context);
    }

    inline generic_closed_system::generic_closed_system(const std::string &node_name, const node_context &external_context)
            : composite_node(node_name, external_context),
              generic_ports(),
              sim_config(){

            auto own_node_name = node_name;
            if(node_name == "prototype") own_node_name = "main";

            config = node_config(sim_config.node(own_node_name));

            create_from_config(node_name, external_context);
    }

    inline void generic_closed_system::create_from_config(const std::string &node_name, const node_context &external_context) {

        auto own_node_name = node_name;
        if(node_name == "prototype") own_node_name = "main";

        config = node_config(sim_config.node(own_node_name));

        // Create Ports from Config
        for(auto& [port_name, port_config] : config.ports()) {
            add_port(port_name, external_interface(), port_config.mode(), port_config.direction());
        }

        // Create Nodes from Config
        for(auto& [component_name, node_config] : config.inner_nodes()) {
            switch (node_config.node_type()) {
                case config_base::NodeTypes::AtomicNode:
                    components[component_name] = std::shared_ptr<system_node>(new generic_atomic_node(component_name, internal_context(), node_config));
                    break;
                case config_base::NodeTypes::CompositeNode:
                    components[component_name] = std::shared_ptr<system_node>(new generic_closed_system(component_name, internal_context(), node_config));
                    break;
                case config_base::NodeTypes::FunctionNode:
                case config_base::NodeTypes::ParameterNode:
                case config_base::NodeTypes::CollectionNode:
                case config_base::NodeTypes::Simulation:
                case config_base::NodeTypes::RealTimeSimulation:
                case config_base::NodeTypes::OwnNode:
                case config_base::NodeTypes::None:
                    break;
            }
        }

        // Link Ports

        auto inner_nodes = config.inner_nodes();

        for(auto& [from_node_pair, to_node_pair] : config.connections()) {
            auto&[from_node_name, from_port_name] = from_node_pair;
            auto&[to_node_name, to_port_name] = to_node_pair;
            bool from_own_node = own_node_name == from_node_name;
            bool to_own_node = own_node_name == to_node_name;

            if (from_own_node and to_own_node) {
                // This shouldn't make sence for messages. flow it could work, but need it to?
                // There is no function. So no.
                //TODO: throw error!
                break;
            } else if (from_own_node) {
                auto to_node = inner_nodes.at(to_node_name);

                if (to_node.port(to_port_name).direction() == data_goal::output) break; //TODO: throw error!
                switch (to_node.port(to_port_name).mode()) {
                    case data_mode::flow:
                        inward_link(flow_input_port(from_port_name),
                                    dynamic_cast<sydevs::systems::generic_ports &>(*components[to_node_name]).flow_input_port(
                                            to_port_name));
                        break;
                    case data_mode::message:
                        inward_link(message_input_port(from_port_name),
                                    dynamic_cast<sydevs::systems::generic_ports &>(*components[to_node_name]).message_input_port(
                                            to_port_name));
                        break;
                }
            } else if (to_own_node) {
                auto from_node = inner_nodes.at(from_node_name);

                if (from_node.port(from_node_name).direction() == data_goal::input) break; //TODO: throw error!
                switch (from_node.port(from_node_name).mode()) {
                    case data_mode::flow:
                        outward_link(
                                dynamic_cast<sydevs::systems::generic_ports &>(*components[from_node_name]).flow_output_port(
                                        from_port_name), flow_output_port(to_port_name));
                        break;
                    case data_mode::message:
                        outward_link(
                                dynamic_cast<sydevs::systems::generic_ports &>(*components[from_node_name]).message_output_port(
                                        from_port_name), message_output_port(to_port_name));
                        break;
                }
            } else {
                auto from_node = inner_nodes.at(from_node_name);
                auto to_node = inner_nodes.at(to_node_name);

                if (from_node.port(from_node_name).direction() == data_goal::input or
                    to_node.port(to_port_name).direction() == data_goal::output)
                    break; //TODO: throw error!
                switch (to_node.port(to_port_name).mode()) {
                    case data_mode::flow:
                        inner_link(
                                dynamic_cast<sydevs::systems::generic_ports &>(*components[from_node_name]).flow_output_port(
                                        from_port_name),
                                dynamic_cast<sydevs::systems::generic_ports &>(*components[to_node_name]).flow_input_port(
                                        to_port_name));
                        break;
                    case data_mode::message:
                        inner_link(
                                dynamic_cast<sydevs::systems::generic_ports &>(*components[from_node_name]).message_output_port(
                                        from_port_name),
                                dynamic_cast<sydevs::systems::generic_ports &>(*components[to_node_name]).message_input_port(
                                        to_port_name));
                        break;
                }
            }
        }
    }
}

#endif //SYDEVS_REALTIME_GENERIC_CLOSED_SYSTEM_H
