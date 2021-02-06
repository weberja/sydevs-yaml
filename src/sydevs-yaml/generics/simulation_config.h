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

#include <any>

#include <iostream>

namespace sydevs::generics {
    using namespace sydevs;
    using namespace sydevs::systems;
    using namespace std;

    using data_type = std::type_index;

    using connection_type = std::pair<std::pair<std::string, std::string>, std::pair<std::string, std::string>>; ///< Nested pair of a pair. [[from_node_name, from_port_name],[to_node_name, to_port_name]]

    /**
     * Base class for all configs. This class also holds static variables to map the strings used in the .yaml with classes and objects used by SyDEVS and SyDEVS-yaml.
     */
    class config_base {
    public:
        /**
         * Construtor
         * @param config yaml node. Can be anything.
         * @param config_directory Path to the directory the config file
         */
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
         * Holds all Types of nodes which can be created with SyDEVS-yaml
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

        /**
         * Maps the key, used in the config, to the enum ::NodeTypes
         */
        inline static const std::unordered_map<std::string, NodeTypes> node_types_to_yaml = {
                {"atomic_nodes",         NodeTypes::AtomicNode},
                {"function_nodes",       NodeTypes::FunctionNode},
                {"parameter_nodes",      NodeTypes::ParameterNode},
                {"composite_nodes",      NodeTypes::CompositeNode},
                {"collection_nodes",     NodeTypes::CollectionNode},
                {"simulation",           NodeTypes::Simulation},
                {"real_time_simulation", NodeTypes::RealTimeSimulation},
                {"own_nodes",            NodeTypes::OwnNode}
        };

        /**
         * Vector of all types supported in lua and yaml. The string is the same a named in yaml.
         * See Data types in the README.md for more infos.
         */
        inline static const std::vector<std::string> qualified_types = {
                "int",
                "double",
                "float",
                "string",
                "bool",
                "duration"
        };

        static bool qualified_type(const std::string& type) {
            if(std::find(std::begin(qualified_types), std::end(qualified_types), type) != std::end(qualified_types)) {
                return true;
            }
            return false;
        }

        /**
         * Maps the key, used in the config, to the enum sydevs::systems::data_mode
         */
        inline static const std::unordered_map<std::string, data_mode> string_to_data_mode = {
                {"flow",    data_mode::flow},
                {"message", data_mode::message}
        };

        /**
         * Maps the key, used in the config, to the enum sydevs::systems::data_goal
         */
        inline static const std::unordered_map<std::string, data_goal> string_to_data_goal = {
                {"input",  data_goal::input},
                {"output", data_goal::output}
        };

        /**
         * Return the Name of the Node/Port/Simulation
         */
        virtual std::string name();

        /**
         * Return the full/absolute path to the lua file of the Node/Port/Simulation
         */
        virtual std::string path_to_lua();

        /**
         * Return the set precision of the Node/Simulation
         */
        virtual scale time_precision() const;

    protected:
        YAML::Node config; ///< Holds the YAML::Node
        std::filesystem::path config_directory; ///< Absolute path to folder which the holdes the config
    };

    /**
     * Parse and holds the config of a port
     */
    class port_config : config_base {
    public:
        explicit port_config(const YAML::Node& port_config); ///< Constructor
        port_config() = default; ///< Create an empty config. It can't be change after the creation

        data_mode mode(); ///< Return the mode (flow/message) of the port
        data_goal direction(); ///< Return the direction (input/output) of the port

        std::string path_to_lua() override;
        scale time_precision() const override;
    };

    class simulation_config;

    /**
     * Parse and holds the config of a node
     */
    class node_config : public config_base {
    public:
        node_config(const YAML::Node& node_config, NodeTypes node_type, std::filesystem::path config_directory, simulation_config* simulation); ///< Constructor
        node_config(); ///< Create an empty config. It can't be change after the creation

        NodeTypes node_type() const; ///< Returns the type of the node. The type is computed from the map config_base::node_types_to_yaml
        std::vector<connection_type> connections(); ///< Returns the parsed connection of an node. This can be empty if the node is not a composite_node!
        std::unordered_map<std::string, node_config> inner_nodes(); ///< Returns the nodes with are in this node. This can be empty if the node is not a composite_node!
        std::unordered_map<std::string, port_config> ports(); ///< Returns the port with it's name and config
        port_config port(const std::string& port_name); ///< Return the config of a port
        std::string user_class(); ///< Return the name with which the user-defined class is registered. This can be empty if the node is not a own_node!

        template<typename ReturnType>
        ReturnType get_config(const std::string& key); ///< Search in the config of the node if the key exist and return the value. The value is casted to the ReturnType
    private:
        NodeTypes type;
        simulation_config* sim{nullptr};
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

        simulation_config(); ///< Constructor

        node_config node(const std::string& node_name); ///< Return the config of the given node - if it exists

        /**
         * Returns the value for each port of the node "main"
         * @return for every port mention in the .yaml under "init_ports" it returns the value to be set
         */
        std::unordered_map<std::string, std::any> init_ports();

        int64 count_agent();

    private:
        inline static std::string path = "./simulation.yaml"; //TODO: Standard - Empty or this?

        void load_config(); ///< load and parse the config with the ::path
        std::unordered_map<std::string, node_config> nodes; ///< store the config of the nodes in this simulation
    };

}

#endif //SYDEVS_GUI_SIMULATION_CONFIG_H
