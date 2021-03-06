#pragma once

#include <functional>
#include "DD_Container.h"
#include "DD_LuaHooks.h"
#include "DD_String.h"

#define QUEUE_BOUND 1000

struct DummyAgent {
  cbuff<64> name;
  float position[3] = {0.f, 0.f, 0.f};
  bool alive;
};

struct DD_SimpleQueue {
  DD_LEvent q_handle[QUEUE_BOUND];
  unsigned q_size = 0;
  unsigned q_head = 0;
  unsigned q_tail = 0;
  DD_FuncBuff fbuffer;

  bool push(DD_LEvent &levent);
  bool pop(DD_LEvent &levent);
};

struct DD_ResourceBin {
  DD_ResourceBin() : agents(100) {}

  dd_array<DummyAgent> agents;
  DD_FuncBuff arg_buffer;
  unsigned num_agents = 0;

  int add_new_agent(lua_State *L);
};

/// \brief Prints all agent infromation
void print_all_agents(DD_ResourceBin &resbin);