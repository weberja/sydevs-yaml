#include "lua.h"

namespace sydevs::generics {

    lua::lua(const std::string &file_path, generic_ports* node_ports) {
        init_lua();
        // Register generic_ports
        l.new_usertype<generic_ports>("generic_ports",
                                      "set_port", &generic_ports::set_port,
                                      "port_received", &generic_ports::port_received,
                                      "get_string_port", &generic_ports::get_port_as<std::string>,
                                      "get_double_port", &generic_ports::get_port_as<float64>,
                                      "get_int_port", &generic_ports::get_port_as<int64>,
                                      "get_duration_port", &generic_ports::get_port_as<duration>,
                                      "get_bool_port", &generic_ports::get_port_as<bool>);

        // Make Ports accessible
        l["ports"] = node_ports;

        run_file(file_path); // TODO: Check if lua file exsist. Otherwise just do nothing! Fail silently?
    }


    void lua::init_lua() {
        // Open Liberays
        l.open_libraries();

//        l.set_exception_handler(&lua::exception_handler);

        l.new_usertype<scale>("scale", sol::constructors<scale(int64)>());

        sol::usertype<duration> duration_type = l.new_usertype<duration>("duration", sol::constructors<duration(), duration(int64), duration(int64, scale)>());
        duration_type["fixed_at"] = &duration::fixed_at;
        duration_type["multiplier"] = &duration::multiplier;
        duration_type["precision"] = &duration::precision;
        duration_type["to_number"] = [](duration d) {
            return d.multiplier() * d.precision().approx();
        };

        l.new_usertype<std::any>("any", sol::constructors<std::any(std::string), std::any(int), std::any(double)>());

        run_file("lua/log.lua");
        run_file("lua/scale.lua");
    }

    void lua::run_file(const std::string& file_paths) {

        sol::protected_function_result res =  l.safe_script_file(file_paths);

        if(!res.valid()) {
            sol::error err = res;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    duration lua::planned_event(duration elapsed_dt) {
        sol::protected_function f = l["planned_event"];

        sol::protected_function_result result = f(elapsed_dt);
        if (result.valid()) {
            duration s = result;
            return s;
        } else {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    void lua::finalization_event(duration elapsed_dt) {
        sol::protected_function f = l["finalization_event"];

        sol::protected_function_result result = f(elapsed_dt);
        if (result.valid()) {
            return;
        } else {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    duration lua::unplanned_event(duration elapsed_dt) {
        sol::protected_function f = l["unplanned_event"];

        sol::protected_function_result result = f(elapsed_dt);
        if (result.valid()) {
            duration s = result;
            return s;
        } else {
            sol::error err = result;
            std::string what = err.what();
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    duration lua::initialization_event() {
        sol::protected_function f = l["initialization_event"];

        sol::protected_function_result result = f();
        if (result.valid()) {
            duration s = result;
            return s;
        } else {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    duration lua::macro_initialization_event() {
        sol::protected_function f = l["macro_initialization_event"];

        sol::protected_function_result result = f();
        if (result.valid()) {
            duration s = result;
            return s;
        } else {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    void lua::micro_planned_event(const std::string &agent_id, duration elapsed_dt) {
        sol::protected_function f = l["micro_planned_event"];

        sol::protected_function_result result = f(agent_id, elapsed_dt);
        if (!result.valid()) {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    duration lua::macro_planned_event(duration elapsed_dt) {
        sol::protected_function f = l["macro_planned_event"];

        sol::protected_function_result result = f(elapsed_dt);
        if (result.valid()) {
            duration s = result;
            return s;
        } else {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    void lua::macro_finalization_event(duration elapsed_dt) {
        sol::protected_function f = l["macro_finalization_event"];

        sol::protected_function_result result = f(elapsed_dt);
        if (!result.valid()) {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

    void lua::flow_event() {
        sol::protected_function f = l["flow_event"];

        sol::protected_function_result result = f();
        if (!result.valid()) {
            sol::error err = result;
            throw std::runtime_error("[LUA Script Error] " + std::string(err.what()));
        }
    }

}
