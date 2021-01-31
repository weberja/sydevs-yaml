#pragma once
#ifndef SYDEVS_GUI_SIMULATION_CONFIG_H
#define SYDEVS_GUI_SIMULATION_CONFIG_H

#include <yaml-cpp/yaml.h>

#include <sydevs/core/number_types.h>
#include <sydevs/core/scale.h>
#include <sydevs/systems/port.h>

#include <string>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

#include <iostream>

namespace sydevs::generics {
    using namespace sydevs;
    using namespace sydevs::systems;
    using namespace std;

    using data_type = std::type_index;

    using connection_type = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>; ///< Nested pair of a pair. [[from_node_name, from_port_name],[to_node_name, to_port_name]]

    class config_base {
    public:

        config_base(const YAML::Node &config, std::filesystem::path config_directory);

        config_base() = default;

        inline static const std::unordered_map<std::string, scale> scales = {
                {"no_scale", no_scale},
                {"yocto",    yocto},
                {"zepto",    zepto},
                {"atto",     atto},
                {"femto",    femto},
                {"pico",     pico},
                {"nano",     nano},
                {"micro",    micro},
                {"milli",    milli},
                {"unit",     unit},
                {"kilo",     kilo},
                {"mega",     mega},
                {"giga",     giga},
                {"tera",     tera},
                {"peta",     peta},
                {"exa",      exa},
                {"zetta",    zetta},
                {"yotta",    yotta}
        };

        /**
         *
         */
        enum class NodeTypes {
            AtomicNode,
            FunctionNode,
            ParameterNode,
            CompositeNode,
            CollectionNode,
            Simulation,
            RealTimeSimulation,
            OwnNode,
            None
        };

        inline static const std::unordered_map<std::string, NodeTypes> node_types_to_yaml = {
                {"atomic_nodes",         NodeTypes::AtomicNode},
                {"function_nodes",       NodeTypes::FunctionNode},
                {"parameter_nodes",      NodeTypes::ParameterNode},
                {"composite_nodes",      NodeTypes::CompositeNode},
                {"collection_nodes",     NodeTypes::CollectionNode},
                {"simulation",           NodeTypes::Simulation},
                {"real_time_simulation", NodeTypes::RealTimeSimulation},
        };

        inline static const std::unordered_map<std::string, data_mode> string_to_data_mode = {
                {"flow",    data_mode::flow},
                {"message", data_mode::message}
        };

        inline static const std::unordered_map<std::string, data_goal> string_to_data_goal = {
                {"input",  data_goal::input},
                {"output", data_goal::output}
        };

        virtual std::string name();

        virtual std::string path_to_lua();

        virtual scale time_precision() const;

    protected:
        YAML::Node config;
        std::filesystem::path config_directory;
    };

    class port_config : config_base {
    public:
        explicit port_config(const YAML::Node& port_config);
        port_config() = default;

        data_mode mode();
        data_goal direction();
//        data_type type();

        std::string path_to_lua() override;
        scale time_precision() const override;
    };

    class simulation_config;

    class node_config : public config_base {
    public:
        node_config(const YAML::Node& node_config, NodeTypes node_type, std::filesystem::path config_directory, simulation_config* simulation);
        node_config();

        NodeTypes node_type() const;
        std::vector<connection_type> connections();
        std::unordered_map<std::string, node_config> inner_nodes();
        std::unordered_map<std::string, port_config> ports();
        port_config port(const std::string& port_name);
    private:
        NodeTypes type;
        simulation_config* sim;
    };

    /**
     * Load the .yaml file and process it.
     */
    class simulation_config : public config_base {
    public:
        /**
         * Set the path once. After it was set once, it can't be changed!
         * @param path path to the .yaml file
         */
        static void set_path(const std::string& path_to_config) {
             path = path_to_config; // TODO: Check if file exists. Faile *not* silentlty?
        }

        //explicit simulation_config(const std::string& path_to_config);
        simulation_config();

        node_config node(const std::string& node_name);

        int64 count_agent();

    private:
        inline static std::string path = "./simulation.yaml"; //TODO: Standard - Empty or this?

        void load_config();
        std::unordered_map<std::string, node_config> nodes;
    };

}

#endif //SYDEVS_GUI_SIMULATION_CONFIG_H
