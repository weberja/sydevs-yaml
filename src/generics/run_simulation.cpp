#include <generics/run_simulation.h>

namespace sydevs::generics {

using namespace sydevs;
using namespace sydevs::systems;

    realtime::realtime(const std::string& path_to_config)
        : sim_ptr_()
    {
        this->path_to_config = path_to_config;
    }

    void realtime::mainloop(duration total_dt, int64 seed, std::ostream& out_stream, bool synchronize_time)
    {
        try {
            simulation_config::set_path(path_to_config);

            sim_ptr_ = std::make_unique<real_time_simulation<generic_interactiv_system>>(total_dt, seed, out_stream);
            sim_ptr_->update_time_advancement_rate(1);

            if(synchronize_time) sim_ptr_->update_time_synchronization_rate(pow(10, 9 - 5));

            while (true) {
                int64 event_count = sim_ptr_->process_frame_if_time_reached();
                if (event_count > 0) {
                    if (sim_ptr_->frame_index() == 0) {
                        sim_ptr_->update_synchronization_time(sim_ptr_->frame_time(), sim_ptr_->frame_clock_time());
                    }
                    sim_ptr_->frame_time().gap(time_point()).rescaled(sydevs::milli);
                }
            }
        }
        catch (const system_node::error& e) {
            std::cout << "SYSTEM NODE ERROR: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "OTHER ERROR: " << e.what() << std::endl;
            std::cout << "Type:    " << typeid(e).name() << std::endl;
        }
    }
}  // namespace
