-- Config file for loading levels and other settings
hero_script = require "scripts.Hero"

local inputs =
{
    "DeadWorld"
}

function generate_levels( event, args, num_args )
    -- register new agent on cpp side
    main_hero = hero_script:new()
    main_hero.name = "Bob"
    main_hero.position = { 0.0, 1.0, -2.0 }
    e = {}
    e["event_id"] = "agent_creation"
    e["agent.name"] = main_hero.name
    e["agent.pos.x"] = main_hero.position[1]
    e["agent.pos.y"] = main_hero.position[2]
    e["agent.pos.z"] = main_hero.position[3]
    res_new_agent(e)

    -- create a list of valid levels
    levels = {}
    levels["event_id"] = "lvls_found"

    num_lvls = 0; dir_str = ""
    if (event == "io" and num_args == 1) then
		print(k, v)
        for k,v in pairs(args) do dir_str = v end -- grab file location
    end
    for i,v in pairs(inputs) do
		print(i, v)
        filename = string.format( "%s%s.lua", tostring(dir_str), v)
        _file = io.open(filename)
        -- add to output if found
        if _file then 
            num_lvls = num_lvls + 1
            idx = string.format( "lvl_%d", num_lvls)
            levels[idx] = v -- add to output
        end
        if _file == nil then 
            print(string.format("Not found: %s", v))
        end
    end
    --[[
        print("Level scripts found:")
        for i,v in pairs(levels) do
            print(i, v)
        end

        e = {}
        e["event_id"] = "dummy_xtra0"
        e["dummy_f"] = 5.0
        e["dummy_i"] = 5
        e["dummy_b"] = true
        e1 = {}
        e1["event_id"] = "dummy_xtra1"
        e1["dummy_f"] = 3.0
    --]]
    return levels
end