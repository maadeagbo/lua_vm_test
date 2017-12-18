-- Config file for loading levels and other settings

local inputs =
{
    "DeadWorld"
}

function generate_levels( event, args, num_args )
-- creates a list of found level scripts
    levels = {}
    for i,v in ipairs(inputs) do
				filename = string.format( "%sscripts/%s.lua", ROOT_DIR, v)
				_file = io.open(filename)					-- check file exists
				if _file then 
						levels[#levels + 1] = v -- add to output
				else
						if not _file then print("Not found: "..filename) end
				end
    end
    levels["num_levels"] = #levels
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