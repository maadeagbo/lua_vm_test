-- Config file for loading levels and other settings

local inputs =
{
    "DeadWorld"
}

function generate_levels( event, args, num_args )

    -- create a list of valid levels
    levels = {}
    levels["event_id"] = "lvls_found"

    num_lvls = 0; dir_str = ""
    if (event == "io" and num_args == 1) then
        for k,v in pairs(args) do 
			dir_str = v -- grab file location
		end 
    end
    for i,v in pairs(inputs) do
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