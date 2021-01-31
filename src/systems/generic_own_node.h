#ifndef SYDEVS_YAML_GENERIC_OWN_NODE_H
#define SYDEVS_YAML_GENERIC_OWN_NODE_H

#include <sydevs/systems/atomic_node.h>
#include <systems/generic_ports.h>
#include <generics/simulation_config.h>

#include <utility>


namespace sydevs::generics {
    /**
     * Interface for user written classes
     *
     * Here is an example class. A generic_own_node is a specialized version of an atomic_node from SyDEVS and can be used as one.
     * Please note that there is no need generate ports or mangel with the config
     * \code{.cpp}
     *  class adder_node : public generic_own_node {
        REGISTER("adder_node", generic_own_node, const std::string&, const node_context&, node_config);

        public:
            // Constructor/Destructor:
            adder_node(const std::string& node_name, const node_context& external_context, node_config config);
            ~adder_node() override = default;

            // Attributes:
            scale time_precision() const override;

        protected:

            // Event Handlers:
            duration initialization_event() override;
            duration unplanned_event(duration elapsed_dt) override;
            duration planned_event(duration elapsed_dt) override;
            void finalization_event(duration elapsed_dt) override;
        };
     * \endcode
     */
    class generic_own_node : public atomic_node, public generic_ports {
    protected:
        /**
         * This class can only be initialized from a derived class. All parameter are passed through to the class
         * atomic_node from SyDEVS, except for the node_config
         */
        generic_own_node(const std::string &node_name, const node_context &external_context, node_config config);


        /**
         * Configuration of the current user_node. To acess user-defined keys in the .yaml the function node_config::get_config()
         */
        node_config config;
    private:

    };

    inline generic_own_node::generic_own_node(const std::string &node_name, const node_context &external_context, node_config config)
                                                : atomic_node(node_name, external_context)
                                                , generic_ports()
                                                , config(std::move(config)) {
        // Create Ports from Config
        for(auto& [port_name, port_config] : config.ports()) {
            add_port(port_name, external_interface(), port_config.mode(), port_config.direction());
        }
    }

}
#endif //SYDEVS_YAML_GENERIC_OWN_NODE_H
