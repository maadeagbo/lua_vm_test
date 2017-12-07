#pragma once
#include "DD_String.h"
#include <cstdint>
#include <stdlib.h>
#include <typeinfo>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#define MAX_CALLBACK_EVENTS 10
#define MAX_EVENT_ARGS 8

/// \brief class for parsing Varying data struct
enum class VType : unsigned { INT, BOOL, FLOAT, STRING, COUNT };

/// \brief Data type for event values
struct Varying {
  Varying() : v_strptr() {}
  VType type;

  union {
    cbuff<64> v_strptr;
    int32_t v_int;
    float v_float;
    bool v_bool;
  };
};

/// \brief Data type for events passed thru callbacks
struct DD_LEvent {
  // DD_LEvent() : args(0) {}
  struct KeyVal {
    cbuff<32> key;
    Varying val;
  };

  cbuff<32> handle;
  KeyVal args[MAX_EVENT_ARGS];
  unsigned active = 0;
  unsigned priority_timer[2] = {0, 0};
};

/// \brief Add argument to DD_LEvent
template <typename T>
bool add_arg_LEvent(DD_LEvent *levent, const char *key, T arg) {
  return false;
}

/// \brief Buffer to hold evens from callback function
struct DD_CallBackBuff {
  /// \brief Get new event from buffer (return nullptr if buffer is full)
  DD_LEvent *getNewEvent();

  DD_LEvent buffer[MAX_CALLBACK_EVENTS];
  unsigned num_events = 0;
};

/// \brief resets DD_CallBackBuff
void clear_callbackbuff(DD_CallBackBuff &cb);

/// \brief Parse the arguments from an error (lua) and end lua VM
template <typename... Args>
void parse_error(lua_State *L, const char *fmt, const Args &... args) {
  fprintf(stderr, fmt, args...);
  lua_close(L);
  exit(EXIT_FAILURE);
}

/// \brief Register instance into lua_extraspace
template <class T>
void register_instance_lua_xspace(lua_State *L, T &instance) {
  *static_cast<T **>(lua_getextraspace(L)) = &instance;
}

/// \brief Check lua stack value for nil (returns true if nil)
bool check_stack_nil(lua_State *L, int idx);

/// \brief Register class function w/ dispatch template
void register_callback_lua(lua_State *L, const char *func_sig,
                           lua_CFunction _func);

/// \brief Read and execute lua script file
bool parse_luafile(lua_State *L, const char *filename);

/// \brief Envoke lua callback function and return event
void callback_lua(lua_State *L, const DD_LEvent &event, DD_CallBackBuff &cb,
                  const char *func, const char *lclass = "");

/// \brief Envoke lua callback function with lua_ref pointer
void callback_lua(lua_State *L, const DD_LEvent &event, DD_CallBackBuff &cb,
                  int func_ref, int global_ref = LUA_REFNIL);

/// \brief Push event arguments onto the stack
void push_args(lua_State *L, const DD_LEvent &levent, const int idx);

/// \brief Dump stack
void stack_dump(lua_State *L);

/// \brief Get returned events from callbacks
void parse_callbacks(lua_State *L, DD_CallBackBuff &cb);

/// \brief Parse all events from callbacks
void parse_lua_events(lua_State *L, DD_CallBackBuff &cb);

/// ]brief Parse table into DD_LEvent
void parse_table(lua_State *L, DD_LEvent *levent, const int tabs = 0);

/// \brief Print tables for returned events (debug)
void print_table(lua_State *L, const int tabs = 0);

/// \brief Print out contents of DD_CallbackBuff
void print_callbackbuff(DD_CallBackBuff &cb);

/// \brief Store and return handle to lua function
int get_lua_ref(lua_State *L, const char *lclass, const char *func);

/// \brief Clear function reference
void clear_lua_ref(lua_State *L, int func_ref);

/// \brief Parse DD_CallBackBuff arguments to into values
template <typename T>
T* get_callback_val(const char* ckey, DD_CallBackBuff &cb) { return nullptr; }

template<>
const char* get_callback_val<const char>(const char* ckey, DD_CallBackBuff &cb);

template<>
bool* get_callback_val<bool>(const char* ckey, DD_CallBackBuff &cb);

template<>
float* get_callback_val<float>(const char* ckey, DD_CallBackBuff &cb);
