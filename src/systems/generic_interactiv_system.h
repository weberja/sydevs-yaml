//
// Created by minime on 26.12.20.
//

#ifndef SYDEVS_REALTIME_GENERIC_INTERACTIV_SYSTEM_H
#define SYDEVS_REALTIME_GENERIC_INTERACTIV_SYSTEM_H

#include <sydevs/systems/interactive_system.h>
#include <systems/generic_closed_system.h>

#include <generics/simulation_config.h>
#include <generics/lua.h>

#include <any>

namespace sydevs::systems {

    using namespace sydevs;
    using namespace sydevs::generics;

/**
 * @brief Specialisation of 'interactive_system'. With a reference to a .yaml file it generate and link the simulation described in the .yaml.
 *
 * @details
 * The generic is achieved by moving the simulation part in to a lua file. In lua the four following virtual functions of interactiv_system are forwarded to lua. In
 * - macro_initialization_update
 * - micro_planned_update
 * - macro_planned_update
 * - macro_finalization_update
 *
 * To access the ports in lua, there are special function are provided.
 * The lua file has the following injected function to interact with the ports of the prototype.
 * ~~~
 * ports:set_port(PORT_NAME, VALUE)
 *
 * ports:get_string_port(PORT_NAME)
 * ports:get_double_port(PORT_NAME)
 * ports:get_bool_port(PORT_NAME)
 * ports:get_duration_port(PORT_NAME)c++ add
 * ~~~
 *
 * In addition to the function there are two C++ types also provided in lua.
 * The first one is the class duration, with is used to describe the length of an _update function and the second one is std::any.
 * std::any is used to set the value of a port. If std::any is not used an exception will be thrown. An Example call for ports:set_port to set the port 'out_port' with the value 2 would be the following:
 * ~~~
 * ports:set_port("out_port", any.new(2))
 * ~~~
 *
 * The yaml has two bigger sections, on the root level the information for the simulation are stored.
 * Each generic_node has a lua file connected with it and the path with is given in the config is relativ to position of the yaml file
 * ~~~
 * name: 'bouncing_ball'
 * count_agent: 2
 * lua: 'simulation.lua'
 * time_precision: unit
 * simulation_main_node: 'main'
 * real_time: true
 * ~~~
 *
 * @see
*/
class generic_interactiv_system : public interactive_system<std::string, generic_closed_system, std::any, std::any>, generic_ports
    {
    public:
        /**
         * @brief Constructs an `generic_interactiv_system` node.
         *
         * @details
         * Constructs the generic interactive system node with a gerneric_cloased_system in it and associates it with the
         * surrounding context. It assume that the .yaml config of the simulation is in the same folder as the executable.
         * An exception is thrown if the node has any ports.
         *
         * @param node_name The name of the node within the encompassing context.
         * @param external_context The context in which the node is constructed.
         * @throws std::logic_error If the interactive_system has any ports.
         */
        generic_interactiv_system(const std::string& node_name, const node_context& external_context);

        ~generic_interactiv_system() override = default;

        // Attributes:
        scale time_precision() const override;
    private:
        simulation_config config;
        lua interpreter;

        // Event Handlers:
        duration macro_initialization_update(std::any& injection) override;
        void micro_planned_update(const std::string& agent_id, duration elapsed_dt) override;
        duration macro_planned_update(duration elapsed_dt, const std::any& injection, std::any& observation) override;
        void macro_finalization_update(duration elapsed_dt) override;

        /** @name C++ and Lua Functions
         * These documentations are valid for call from lua and c++. Please note that the previous named cave for the call from lua
         */
        ///@{
        /**
         * Set the given Port of the prototype or agent.
         * @param port_name Name of the port of the prototype
         * @param value Any object casted to std::any
         * @throw runtime_error If the given port is not of the type input it can't be set
         */
        void set_port(const std::string& port_name, const std::any& value) override;



        template<typename ReturnType>
        ReturnType get_port_value_as(const std::string& port_name);
    };

    inline generic_interactiv_system::generic_interactiv_system(const std::string &node_name,
                                                                const node_context &external_context)
            :interactive_system<std::string, generic_closed_system, std::any, std::any>(node_name, external_context),
             config(),
             interpreter(config.path_to_lua(), this) {
    }

    inline scale generic_interactiv_system::time_precision() const {
        return config.time_precision();
    }

    inline duration generic_interactiv_system::macro_initialization_update(std::any& injection)
    {
        duration dt = interpreter.macro_initialization_event();
        auto ports_to_init = config.init_ports();
        for(auto [port_name, value] : ports_to_init) {
            set_port(port_name, value);
        }
        try {
            invoke_agent("main");
        } catch(std::logic_error& e) {
            create_agent("main");
        }
        return dt;
    }


    inline void generic_interactiv_system::micro_planned_update(const std::string& agent_id, duration elapsed_dt)
    {
        interpreter.micro_planned_event(agent_id, elapsed_dt);
    }


    inline duration generic_interactiv_system::macro_planned_update(duration elapsed_dt, const std::any& injection, std::any& observation)
    {
        return interpreter.macro_planned_event(elapsed_dt);
    }

    inline void generic_interactiv_system::macro_finalization_update(duration elapsed_dt)
    {
        interpreter.macro_finalization_event(elapsed_dt);
    }


    //region Function for LUA to interact with the ports of the prototype
    inline void generic_interactiv_system::set_port(const std::string& port_name, const std::any& value)
    {
        data_mode mode;
        data_goal goal;

        std::tie(mode, goal) = prototype.get_node_type(port_name);

        if (goal != data_goal::input) throw std::runtime_error("Port "+ port_name +" ist kein Eingang!");

        switch (mode) {
            case data_mode::flow:
                access(prototype.flow_input_port(port_name)) = std::make_shared<std::any>(value);
                break;
            case data_mode::message:
                access(prototype.message_input_port(port_name)) = std::make_shared<std::any>(value);
                break;
        }
    }

    template<typename ReturnType>
    inline ReturnType generic_interactiv_system::get_port_value_as(const string &port_name) {
        data_mode mode;
        data_goal goal;
        bool return_value;
        std::tie(mode, goal) = prototype.get_node_type(port_name);

        if (goal != data_goal::output) throw std::runtime_error("Port ist kein Eingang!");

        try {
            switch (mode) {
                case data_mode::flow:
                    return_value = std::any_cast<ReturnType>(*access(prototype.flow_output_port(port_name)));
                    break;
                case data_mode::message:
                    return_value = std::any_cast<ReturnType>(*access(prototype.message_output_port(port_name)));
                    break;
            }
        }  catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Datentype des Ports "+ port_name +" ist falsch. Es wurde versucht ein " + typeid(ReturnType).name() + " zubekommen" );
        }

        return return_value;
    }
    //endregion
}

#endif //SYDEVS_REALTIME_GENERIC_INTERACTIV_SYSTEM_H
