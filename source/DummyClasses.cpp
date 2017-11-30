#include "DummyClasses.h"

bool DD_SimpleQueue::pop(DD_LEvent &levent)
{
	if( q_size == 0 ) { return false; }

	levent = std::move(q_handle[q_head]);
	q_head = (q_head + 1) % QUEUE_BOUND;
	q_size -= 1;    // adjust queue size
	return true;
}

bool DD_SimpleQueue::push(DD_LEvent &levent)
{
    if( q_size == QUEUE_BOUND ) { return false; }
	
    q_handle[q_tail] = std::move(levent);
	q_tail = (q_tail + 1) % QUEUE_BOUND;
	q_size += 1;    // adjust queue size
	return true;
}

int DD_ResourceBin::add_new_agent(lua_State* L)
{
	parse_lua_events(L, cb_events);
	print_callbackbuff(cb_events);
	int a_idx = num_agents;
	// check if agent exists then do add procedure
	if (cb_events.num_events == 1 && 
		cb_events.buffer[0].handle.compare("agent_creation") == 0) {
		Varying *v = nullptr;
		cbuff<32> *k = nullptr;
		for (unsigned i = 0; i < cb_events.buffer[0].active; i++) {
			k = &cb_events.buffer[0].args[i].key;
			v = &cb_events.buffer[0].args[i].val;
			switch (v->type) {
				case VType::STRING: 
					if (k->compare("agent.name") == 0) { 
						agents[a_idx].name = v->v_strptr.str();
					}
				case VType::FLOAT:
					if (k->compare("agent.pos.x") == 0) {
						agents[a_idx].position[0] = v->v_float;
					}
					else if (k->compare("agent.pos.y") == 0) {
						agents[a_idx].position[1] = v->v_float;
					}
					else if (k->compare("agent.pos.z") == 0) {
						agents[a_idx].position[2] = v->v_float;
					}
					break;
				case VType::BOOL:
					if (k->compare("agent.alive") == 0) {
						agents[a_idx].alive = v->v_bool;
					}
					break;
				default:
					break;
			}
		}
	}
	return 0;
}