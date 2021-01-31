require("lua.log")

result = 0

function initialization_event()
    return duration.new(0)
end

function unplanned_event(elapsed_dt)
    log.info("PARTY")
    if ports:port_received("adder") then
        log.debug("Recieved")
        result = result + ports:get_double_port("adder")
    end
    return duration.new(0)
end

function planned_event(elapsed_dt)
    log.info("Adder Result:")
    log.info(result)
    return duration.new(1)
end

function finalization_event(elapsed_dt)
    log.debug(elapsed_dt:to_int())
    --    Keine Rückgabe!
end
