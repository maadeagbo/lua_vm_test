#include "DummyClasses.h"

bool DD_SimpleQueue::pop(DD_LEvent &levent) {
  if (q_size == 0) {
    return false;
  }

  levent = std::move(q_handle[q_head]);
  q_head = (q_head + 1) % QUEUE_BOUND;
  q_size -= 1;  // adjust queue size
  return true;
}

bool DD_SimpleQueue::push(DD_LEvent &levent) {
  if (q_size == QUEUE_BOUND) {
    return false;
  }

  q_handle[q_tail] = std::move(levent);
  q_tail = (q_tail + 1) % QUEUE_BOUND;
  q_size += 1;  // adjust queue size
  return true;
}

int DD_ResourceBin::add_new_agent(lua_State *L) {
  parse_lua_events(L, arg_buffer);
  // check if agent name exists then do add procedure
  const char *s_val = arg_buffer.get_func_val<const char>("agent.name");
  if (s_val) {
    int a_idx = num_agents;
    num_agents += 1;
    agents[a_idx].name = s_val;
    // printf("%s\n", s_val);

    float *f_val = arg_buffer.get_func_val<float>("agent.pos.x");
    if (f_val) {
      agents[a_idx].position[0] = *f_val;
    }

    f_val = arg_buffer.get_func_val<float>("agent.pos.y");
    if (f_val) {
      agents[a_idx].position[1] = *f_val;
    }

    f_val = arg_buffer.get_func_val<float>("agent.pos.z");
    if (f_val) {
      agents[a_idx].position[2] = *f_val;
    }

    bool *b_val = arg_buffer.get_func_val<bool>("agent.alive");
    if (b_val) {
      agents[a_idx].alive = *b_val;
    }
  } else {
    printf("Nope\n");
  }

  return 0;
}

void print_all_agents(DD_ResourceBin &resbin) {
  for (unsigned i = 0; i < resbin.num_agents; i++) {
    printf("Agent name: %s\n", resbin.agents[i].name.str());
    printf("\tpos: %.3f, %.3f, %.3f\n", resbin.agents[i].position[0],
           resbin.agents[i].position[1], resbin.agents[i].position[2]);
    printf("\talive: %s\n", resbin.agents[i].alive ? "true" : "false");
  }
}
