require("lua.log")

counter = 0

function initialization_event()
    counter = ports:get_double_port("starter")
    log.debug(counter)
    return duration.new(0)
end

function unplanned_event(elapsed_dt)
    return duration.new(1)
end

function planned_event(elapsed_dt)
    counter = counter + 1
    log.debug(counter)
    ports:set_port("counter",any.new(counter))
    return duration.new(1)
end

function finalization_event(elapsed_dt)
    log.debug(elapsed_dt:to_int())
    --    Keine Rückgabe!
end
