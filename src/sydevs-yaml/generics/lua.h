#ifndef SYDEVS_GUI_LUA_INTERPRETER_H
#define SYDEVS_GUI_LUA_INTERPRETER_H

#define SOL_ALL_SAFETIES_ON 1
#define SOL_USING_CXX_LUA 1
#include <sol/sol.hpp>

#include <sydevs/core/scale.h>
#include <sydevs/systems/data_goal.h>
#include <sydevs/systems/data_mode.h>

#include <sydevs-yaml/systems/generic_ports.h>

#include <iostream>
#include <any>
#include <memory>
#include <string>

namespace sydevs::generics {
    using namespace sydevs;

    class lua {
    public:
        lua(const std::string &file_path, generic_ports* node_ports);

        // Atomic Nodes
        duration initialization_event();
        duration unplanned_event(duration elapsed_dt);
        duration planned_event(duration elapsed_dt);
        void finalization_event(duration elapsed_dt);

        // Function Nodes
        void flow_event();

        // Collection Nodes
        duration macro_initialization_event();
        void micro_planned_event(const std::string& agent_id, duration elapsed_dt);
        duration macro_planned_event(duration elapsed_dt);
        void macro_finalization_event(duration elapsed_dt);

    private:
        sol::state l;

        // Helper Funktionen
        void run_file(const std::string& file_paths);

        void init_lua();
    };


}
#endif //SYDEVS_GUI_LUA_INTERPRETER_H
