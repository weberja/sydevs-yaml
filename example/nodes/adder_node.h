#ifndef SYDEVS_YAML_ADDER_NODE_H
#define SYDEVS_YAML_ADDER_NODE_H
#include <registerer/registerer.h>

#include <systems/generic_own_node.h>

#include <utility>

using namespace sydevs::systems;
using namespace sydevs::generics;

class adder_node : public generic_own_node {
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

    inline scale adder_node::time_precision() const {
        return sydevs::nano;
    }

    inline adder_node::adder_node(const std::string& node_name, const node_context& external_context, node_config config)
            : generic_own_node(node_name, external_context, std::move(config))
    {
        // Ports are declared in the .yaml config!
        // Ports added here will be ignored!
    }


    inline duration adder_node::initialization_event()
    {
        print("Hello! Init");
        return 0_s;
    }


    inline duration adder_node::unplanned_event(duration elapsed_dt)
    {
        print("Hello! UnPlanned_event");
        if(port_received("adder")) print(get_port_as<double>("adder"));
        return 0_s;
    }


    inline duration adder_node::planned_event(duration elapsed_dt)
    {
        print("Hello! Planned_event");
        return 1_s;
    }


    inline void adder_node::finalization_event(duration elapsed_dt)
    {
        print("Hello! finalize");
    }


#endif //SYDEVS_YAML_ADDER_NODE_H
