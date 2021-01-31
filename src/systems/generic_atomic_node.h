#ifndef SYDEVS_GUI_GENERIC_ATOMIC_NODE_H
#define SYDEVS_GUI_GENERIC_ATOMIC_NODE_H

#include <sydevs/systems/atomic_node.h>

#include <systems/generic_ports.h>
#include <generics/lua.h>
#include <generics/simulation_config.h>

namespace sydevs::systems {

    using namespace sydevs::generics;

    class generic_atomic_node : public atomic_node, public generic_ports
    {
    public:
        // Constructor/Destructor:
        generic_atomic_node(const std::string& node_name, const node_context& external_context, node_config config);
        ~generic_atomic_node() override = default;

        // Attributes:
        scale time_precision() const override;

    protected:
        node_config config;
        lua interpreter; // Handler for Lua

        // Event Handlers:
        duration initialization_event() override;
        duration unplanned_event(duration elapsed_dt) override;
        duration planned_event(duration elapsed_dt) override;
        void finalization_event(duration elapsed_dt) override;
    };

    inline scale generic_atomic_node::time_precision() const {
        return nano;
    }

    inline generic_atomic_node::generic_atomic_node(const std::string& node_name, const node_context& external_context, node_config config)
            : atomic_node(node_name, external_context)
            , generic_ports()
            , config(config)
            , interpreter(config.path_to_lua(), this)
    {
        // Create Ports from Config
        for(auto& [port_name, port_config] : config.ports()) {
            add_port(port_name, external_interface(), port_config.mode(), port_config.direction());
        }
    }


    inline duration generic_atomic_node::initialization_event()
    {
        duration dt = interpreter.initialization_event();
        return dt;
    }


    inline duration generic_atomic_node::unplanned_event(duration elapsed_dt)
    {
        duration dt = interpreter.unplanned_event(elapsed_dt);
        return dt;
    }


    inline duration generic_atomic_node::planned_event(duration elapsed_dt)
    {
        duration dt = interpreter.planned_event(elapsed_dt);
        return dt;
    }


    inline void generic_atomic_node::finalization_event(duration elapsed_dt)
    {
        interpreter.planned_event(elapsed_dt);
    }

}  // namespace

#endif //SYDEVS_GUI_GENERIC_ATOMIC_NODE_H
