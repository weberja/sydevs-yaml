#include "simulation_config.h"

namespace sydevs::generics {
    config_base::config_base(const YAML::Node& config, std::filesystem::path config_directory) :
                            config(config),
                            config_directory(std::move(config_directory)) {
    }

    std::string config_base::name() {
        if(not config["name"]) return "";

        return config["name"].as<std::string>();
    }

    std::string config_base::path_to_lua() {
        if( not config["lua"]) return "";

        auto path = config["lua"].as<std::string>();
        return config_directory / path;
    }

    int64 simulation_config::count_agent() {
        return config["count_agent"].as<int64>();
    }

    scale config_base::time_precision() const {
        if(not config["time_precision"]) return scale();

        return scales.at(config["time_precision"].as<std::string>());
    }

    simulation_config::simulation_config() : config_base(YAML::LoadFile(path), std::filesystem::path(path).parent_path()) {
        load_config();
    }

    void simulation_config::load_config() {
        for (const auto& [yaml_name, enum_type] : node_types_to_yaml) {
            if(auto temp_config = config[yaml_name]) {
                for (YAML::const_iterator it = temp_config.begin(); it != temp_config.end(); ++it) {
                    auto current_yaml_node = it->as<YAML::Node>();
                    nodes.try_emplace(current_yaml_node["name"].as<std::string>(), current_yaml_node, enum_type, config_directory,this);
                }
            }
        }
    }

    node_config simulation_config::node(const std::string &node_name) {
        return nodes.at(node_name);
    }

    std::unordered_map<std::string, std::any> simulation_config::init_ports(){
        std::unordered_map<std::string, std::any> result{};
        if(auto temp_config = config["init_ports"]) {
            for(const auto& it : temp_config) {
                auto port = it.as<YAML::Node>();

                auto port_name = port["name"].as<std::string>();
                auto port_type = port["type"].as<std::string>();

                if(not qualified_type(port_type)) {
                    throw std::logic_error("Cant initialize Port " + port_name + " because the type " + port_type + " is not supported by SyDEVS-yaml");
                }
                if(port_type == "int") {
                    result.try_emplace(port_name, std::any(port["value"].as<int64>()));
                } else if(port_type == "double" || port_type == "float") {
                    result.try_emplace(port_name, std::any(port["value"].as<float64>()));
                } else if(port_type == "bool") {
                    result.try_emplace(port_name, std::any(port["value"].as<bool>()));
                } else if(port_type == "string") {
                    result.try_emplace(port_name, std::any(port["value"].as<std::string>()));
                } else if(port_type == "duration") {
                    auto value = port["multiplier"].as<int64>();
                    auto unit = scales.at(port["time_precision"].as<std::string>());
                    duration init_value{value, unit};
                    result.try_emplace(port_name, std::any(init_value));
                }
            }
        }
        return result;
    }

    node_config::node_config(const YAML::Node& node_config, config_base::NodeTypes node_type, std::filesystem::path config_directory, simulation_config* simulation) :
                                config_base(node_config,
                                std::move(config_directory)),
                                sim(simulation) {
        this->type = node_type;
    }

    config_base::NodeTypes node_config::node_type() const{
        return type;
    }

    std::vector<connection_type> node_config::connections() {
        std::vector<connection_type> connections{};
        if(auto temp_config = config["connections"]) {
            for(const auto& it : temp_config)
            {
                auto connection = it.as<std::string>();
                auto delimiter = ".>";

                std::vector<std::string> result;

                std::size_t current, previous = 0;
                current = connection.find_first_of(delimiter);
                while (current != std::string::npos) {
                    result.push_back(connection.substr(previous, current - previous));
                    previous = current + 1;
                    current = connection.find_first_of(delimiter, previous);
                }
                result.push_back(connection.substr(previous, current - previous));

                if(result.size() != 4) throw runtime_error("Can't determent the Connection for the Node " + name() + " and the Connection " + connection);

                for(auto& string : result) {
                    string.erase(std::remove_if(string.begin(), string.end(), [](unsigned char x){return std::isspace(x);}), string.end());
                }

                auto from_node = make_pair(result[0], result[1]);
                auto to_node = make_pair(result[2], result[3]);

                connections.emplace_back(from_node, to_node);
            }
        }
        return connections;
    }

    std::unordered_map<std::string, node_config> node_config::inner_nodes() {
        std::unordered_map<std::string, node_config> inner_nodes{};

        if (auto temp_config = config["nodes"]) {
            for(auto node : temp_config) {
                auto node_name = node.as<std::string>();
                inner_nodes.insert_or_assign(node_name,sim->node(node_name));
            }
        }

        return inner_nodes;
    }

    std::unordered_map<std::string, port_config> node_config::ports() {
        std::unordered_map<std::string, port_config> ports{};

        if(auto temp_config = config["ports"]) {
            for (auto port : temp_config) {
                auto current_port = port.as<YAML::Node>();
                ports.try_emplace(current_port["name"].as<std::string>(), current_port);
            }
        }

        return ports;
    }

    node_config::node_config() {
        type = NodeTypes::None;
    }

    port_config node_config::port(const string &port_name) {
        return ports().at(port_name);
    }

    std::string node_config::user_class() {
        return config["class"].as<std::string>();
    }

    template<typename ReturnType>
    ReturnType node_config::get_config(const string &key) {
        return config[key].as<ReturnType>();
    }

    port_config::port_config(const YAML::Node& port_config) : config_base(port_config, "") {
    }

    std::string port_config::path_to_lua() {
        return "";
    }

    scale port_config::time_precision() const {
        return scale();
    }

    data_mode port_config::mode() {
        auto type = config["port_type"].as<std::string>();
        return string_to_data_mode.at(type);
    }

    data_goal port_config::direction() {
        auto goal = config["direction"].as<std::string>();
        return string_to_data_goal.at(goal);
    }

//    data_type port_config::type() {
//        auto goal = config["data_type"].as<std::string>();
//        return data_types.at(goal);
//    }
}


