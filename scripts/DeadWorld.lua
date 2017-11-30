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

	function create_agent( enitity )
		e =	{}
		e["event_id"] = "agent_creation"
		e["agent.name"] = enitity.name
		e["agent.pos.x"] = enitity.position[1]
		e["agent.pos.y"] = enitity.position[2]
		e["agent.pos.z"] = enitity.position[3]
		e["agent.alive"] = enitity.alive
		res_new_agent(e)
	end

    function DeadWorld:init( map )
		-- spawn hero
		print("Deadworld init called\n")
		self.main_character.name = "Bob"
		self.main_character.position = { 0.0, 1.0, -2.0 }
		self.main_character.alive = true
		create_agent(self.main_character)
        -- spawn some monsters
        for i=1,5 do
            new_name = string.format("monster_%d",i)
            self.enemies[i] = base_monster:new({name = new_name, alive = true})
			create_agent(self.enemies[i])
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