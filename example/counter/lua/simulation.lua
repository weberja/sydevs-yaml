counter = 0
micro_counter = 0

function macro_initialization_event()
    ports:set_port("init",any.new(2.0))
    return duration.new(1, scales["unit"])
end

function micro_planned_event(agentID, elapsed_dt)
    micro_counter = micro_counter + elapsed_dt:to_int()
    print(agentID .. " was updated at" .. micro_counter)
end

function macro_planned_event(elapsed_dt)
    print("PING")
    counter = counter + elapsed_dt:to_number()
    print(counter)
    return duration.new(1)
end

function macro_finalization_event(elapsed_dt)
    print("DONE")
end