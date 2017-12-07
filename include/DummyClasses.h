#pragma once

#include "DD_Container.h"
#include "DD_LuaHooks.h"
#include "DD_String.h"
#include <functional>

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
  DD_CallBackBuff cb_events;

  bool push(DD_LEvent &levent);
  bool pop(DD_LEvent &levent);
};

struct DD_ResourceBin {
  DD_ResourceBin() : agents(100) {}

  dd_array<DummyAgent> agents;
  DD_CallBackBuff cb_events;
  unsigned num_agents = 0;

  int add_new_agent(lua_State *L);
};

/// \brief Prints all agent infromation
void print_all_agents(DD_ResourceBin &resbin);

// This template wraps a member function into a C-style "free" function
// compatible with lua.
template <typename T, int (T::*f)(lua_State *L)> int dispatch_(lua_State *L) {
  T *ptr = *static_cast<T **>(lua_getextraspace(L));
  return ((*ptr).*f)(L);
}
