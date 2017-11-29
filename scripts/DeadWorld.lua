-- DeadWorld.lua
-- Level script for Hero and Monsters
do
    base_hero = require "scripts.Hero"
    base_monster = require "scripts.Monster"

    DeadWorld = 
    {
        main_character = base_hero:new({name = "Slayer"}),
        enemies = {}
    }

    function DeadWorld:Generate( map )
        -- spawn some monsters
        for i=1,5 do
            new_name = string.format("monster_%d",i)
            self.enemies[i] = base_monster:new({name = new_name})
        end
    end

    function DeadWorld:world_status()
        self.main_character:curr_status()
        print(string.format("# of Monsters = %d", #(self.enemies)))
        for i,v in ipairs(self.enemies) do
            v:curr_status()
        end
    end

    function DeadWorld:update( event, args, num_args )
        if event == "post" then
            for k,v in pairs(args) do
                if k == "test_float" then print(v) end
            end
            e = {}
            e["event_id"] = "DeadWorld post called"
            return e
        end
        return { 0 }
    end

end