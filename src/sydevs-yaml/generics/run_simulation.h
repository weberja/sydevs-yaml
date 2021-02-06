#ifndef SYDEVS_RUN_SIMULATION_H_
#define SYDEVS_RUN_SIMULATION_H_

#include <sydevs/core/scale.h>
#include <sydevs/systems/real_time_simulation.h>
#include <sydevs/systems/simulation.h>
#include <sydevs/core/quantity.h>

#include <sydevs-yaml/systems/generic_interactiv_system.h>
#include <sydevs-yaml/generics/simulation_config.h>

#include <iostream>

namespace sydevs::generics {

using namespace sydevs;
using namespace sydevs::systems;

    class realtime
    {
    public:
        explicit realtime(const std::string& path_to_config);

        void mainloop(duration total_dt, int64 seed, std::ostream& stream, bool synchronize_time);
    private:
        std::string path_to_config;
        std::unique_ptr<real_time_simulation<generic_interactiv_system>> sim_ptr_;
    };

    class simulation
    {
    public:
        explicit simulation(const std::string& path_to_config);

        virtual void mainloop() = 0;
    private:
        std::unique_ptr<sydevs::systems::simulation<system_node>> sim_ptr_;
    };


}  // namespace

#endif
