#include "DD_LuaHooks.h"

namespace {
cbuff<64> _key, _val;
}

template <>
bool add_arg_LEvent<bool>(DD_LEvent *levent, const char *key, bool arg) {
  if (!levent || levent->active >= MAX_EVENT_ARGS) {
    return false;
  }
  levent->args[levent->active].key = key;
  levent->args[levent->active].val.type = VType::BOOL;
  levent->args[levent->active].val.v_bool = arg;
  levent->active += 1;
  return true;
}

template <>
bool add_arg_LEvent<int>(DD_LEvent *levent, const char *key, int arg) {
  if (!levent || levent->active >= MAX_EVENT_ARGS) {
    return false;
  }
  levent->args[levent->active].key = key;
  levent->args[levent->active].val.type = VType::INT;
  levent->args[levent->active].val.v_int = arg;
  levent->active += 1;
  return true;
}

template <>
bool add_arg_LEvent<float>(DD_LEvent *levent, const char *key, float arg) {
  if (!levent || levent->active >= MAX_EVENT_ARGS) {
    return false;
  }
  levent->args[levent->active].key = key;
  levent->args[levent->active].val.type = VType::FLOAT;
  levent->args[levent->active].val.v_float = arg;
  levent->active += 1;
  return true;
}

template <>
bool add_arg_LEvent<const char *>(DD_LEvent *levent, const char *key,
                                  const char *arg) {
  if (!levent || levent->active >= MAX_EVENT_ARGS) {
    return false;
  }
  levent->args[levent->active].key = key;
  levent->args[levent->active].val.type = VType::STRING;
  levent->args[levent->active].val.v_strptr = arg;
  levent->active += 1;
  return true;
}

template <> int *get_callback_val<int>(const char *ckey, DD_CallBackBuff &cb) {
  Varying *v = nullptr;
  cbuff<32> *k = nullptr;
  for (unsigned i = 0; i < cb.num_events; i++) {
    for (unsigned j = 0; j < cb.buffer[i].active; j++) {
      k = &cb.buffer[i].args[j].key;
      v = &cb.buffer[i].args[j].val;

      if (v->type == VType::INT) {
        if (k->compare(ckey) == 0) {
          return &v->v_int;
        }
      }
    }
  }
  return nullptr;
}

template <>
float *get_callback_val<float>(const char *ckey, DD_CallBackBuff &cb) {
  Varying *v = nullptr;
  cbuff<32> *k = nullptr;
  for (unsigned i = 0; i < cb.num_events; i++) {
    for (unsigned j = 0; j < cb.buffer[i].active; j++) {
      k = &cb.buffer[i].args[j].key;
      v = &cb.buffer[i].args[j].val;

      if (v->type == VType::FLOAT) {
        if (k->compare(ckey) == 0) {
          return &v->v_float;
        }
      }
    }
  }
  return nullptr;
}

template <>
bool *get_callback_val<bool>(const char *ckey, DD_CallBackBuff &cb) {
  Varying *v = nullptr;
  cbuff<32> *k = nullptr;
  for (unsigned i = 0; i < cb.num_events; i++) {
    for (unsigned j = 0; j < cb.buffer[i].active; j++) {
      k = &cb.buffer[i].args[j].key;
      v = &cb.buffer[i].args[j].val;

      if (v->type == VType::BOOL) {
        if (k->compare(ckey) == 0) {
          return &v->v_bool;
        }
      }
    }
  }
  return nullptr;
}

template <>
const char *get_callback_val<const char>(const char *ckey,
                                         DD_CallBackBuff &cb) {
  Varying *v = nullptr;
  cbuff<32> *k = nullptr;
  for (unsigned i = 0; i < cb.num_events; i++) {
    for (unsigned j = 0; j < cb.buffer[i].active; j++) {
      k = &cb.buffer[i].args[j].key;
      v = &cb.buffer[i].args[j].val;

      if (v->type == VType::STRING) {
        if (k->compare(ckey) == 0) {
          return v->v_strptr.str();
        }
      }
    }
  }
  return nullptr;
}

void clear_callbackbuff(DD_CallBackBuff &cb) {
  for (unsigned i = 0; i < cb.num_events; i++) {
    cb.buffer[i].active = 0;
    cb.buffer[i].handle = "";
  }
  cb.num_events = 0;
}

bool check_stack_nil(lua_State *L, int idx) {
  if (lua_type(L, idx) == LUA_TNIL) {
    return true;
  }
  return false;
}

bool parse_luafile(lua_State *L, const char *filename) {
  int err_num = 0;
  /// \brief quick print out error func
  auto handle_error = [&]() {
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
  };

  cbuff<256> file = filename;
  if (!file.contains(".lua")) {
    printf("Invalid file type <%s>\n", file.str());
  }

  // read file then execute
  if ((err_num = luaL_loadfile(L, filename)) != 0) {
    handle_error();
    return false;
  }
  if ((err_num = lua_pcall(L, 0, 0, 0)) != 0) {
    handle_error();
    return false;
  }
  return true;
}

void push_args(lua_State *L, const DD_LEvent &levent, const int idx) {
  // assumes new table is on top of stack before call
  const int _idx = idx - 1;

  luaL_checkstack(L, 2, "too many arguments");
  switch (levent.args[_idx].val.type) {
  case VType::BOOL:
    lua_pushstring(L, levent.args[_idx].key.str());
    lua_pushinteger(L, levent.args[_idx].val.v_bool ? 1 : 0);
    lua_settable(L, -3);
    break;
  case VType::STRING:
    lua_pushstring(L, levent.args[_idx].key.str());
    lua_pushstring(L, levent.args[_idx].val.v_strptr.str());
    lua_settable(L, -3);
    break;
  case VType::FLOAT:
    lua_pushstring(L, levent.args[_idx].key.str());
    lua_pushnumber(L, levent.args[_idx].val.v_float);
    lua_settable(L, -3);
    break;
  case VType::INT:
    lua_pushstring(L, levent.args[_idx].key.str());
    lua_pushinteger(L, levent.args[_idx].val.v_int);
    lua_settable(L, -3);
    break;
  default:
    break; // set nothing
  }
}

DD_LEvent *DD_CallBackBuff::getNewEvent() {
  if (num_events == MAX_CALLBACK_EVENTS) {
    return nullptr;
  }

  unsigned idx = num_events++;
  return &buffer[idx];
}

void print_callbackbuff(DD_CallBackBuff &cb) {
  for (unsigned i = 0; i < cb.num_events; i++) {
    printf("Event: %s\n", cb.buffer[i].handle.str());
    for (unsigned j = 0; j < cb.buffer[i].active; j++) {
      cbuff<32> &k = cb.buffer[i].args[j].key;
      Varying &v = cb.buffer[i].args[j].val;
      switch (v.type) {
      case VType::BOOL:
        printf("\t%s : %s\n", k.str(), v.v_bool ? "true" : "false");
        break;
      case VType::FLOAT:
        printf("\t%s : %.3f\n", k.str(), v.v_float);
        break;
      case VType::INT:
        printf("\t%s : %d\n", k.str(), v.v_int);
        break;
      case VType::STRING:
        printf("\t%s : %s\n", k.str(), v.v_strptr.str());
        break;
      default:
        break;
      }
    }
  }
}

void register_callback_lua(lua_State *L, const char *func_sig,
                           lua_CFunction _func) {
  lua_register(L, func_sig, _func);
}

void callback_lua(lua_State *L, const DD_LEvent &levent, DD_CallBackBuff &cb,
                  const char *func, const char *lclass) {
  /// \brief quick print out error func
  int err_num = 0;
  auto handle_error = [&]() {
    fprintf(stderr, "callback_lua::%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
  };

  DD_LEvent out;
  bool lclass_flag = lclass && *lclass;

  // find lua class, func, and set class as argument (self)
  // return if function or class doesn't exist
  if (lclass_flag) {
    lua_getglobal(L, lclass);
    if (check_stack_nil(L, -1)) {
      printf("<%s> class doesn't exist.\n", lclass);
      return;
    }
    lua_getfield(L, -1, func);
    if (check_stack_nil(L, -1)) {
      printf("<%s> : <%s> function doesn't exist.\n", lclass, func);
      return;
    }
    lua_rotate(L, 1, -1);
  } else { // find global function
    lua_getglobal(L, func);
    if (check_stack_nil(L, -1)) {
      printf("<%s> global function doesn't exist.\n", func);
      return;
    }
  }

  // push event arguments (in the form of a table)
  lua_pushstring(L, levent.handle.str()); // push event handle
  lua_newtable(L); // create new table and put on top of stack
  if (levent.active > 0) {
    for (unsigned i = 0; i < levent.active; i++) {
      push_args(L, levent, i + 1); // push arguments
    }
  }
  lua_pushinteger(L, (int)levent.active); // push # of arguments

  // call function
  int num_args = lclass_flag ? 4 : 3;
  if ((err_num = lua_pcall(L, num_args, LUA_MULTRET, 0)) != 0) {
    handle_error();
  }

  // get returned events and fill buffer
  parse_callbacks(L, cb);
}

void callback_lua(lua_State *L, const DD_LEvent &levent, DD_CallBackBuff &cb,
                  int func_ref, int global_ref) {
  /// \brief quick print out error func
  int err_num = 0;
  auto handle_error = [&]() {
    fprintf(stderr, "callback_lua::%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
  };

  DD_LEvent out;

  // retrieve function
  if (global_ref) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
    if (check_stack_nil(L, -1)) {
      printf("<%d> class function doesn't exist.\n", func_ref);
      return;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, global_ref);
    if (check_stack_nil(L, -1)) {
      printf("<%d> global doesn't exist.\n", global_ref);
      return;
    }
  } else { // find global function
    lua_rawgeti(L, LUA_REGISTRYINDEX, func_ref);
    if (check_stack_nil(L, -1)) {
      printf("<%d> global function doesn't exist.\n", func_ref);
      return;
    }
  }

  // push event arguments (in the form of a table)
  lua_pushstring(L, levent.handle.str()); // push event handle
  lua_newtable(L); // create new table and put on top of stack
  if (levent.active > 0) {
    for (unsigned i = 0; i < levent.active; i++) {
      push_args(L, levent, i + 1); // push arguments
    }
  }
  lua_pushinteger(L, (int)levent.active); // push # of arguments

  // call function
  int num_args = global_ref ? 4 : 3;
  if ((err_num = lua_pcall(L, num_args, LUA_MULTRET, 0)) != 0) {
    handle_error();
  }

  // get returned events and fill buffer
  parse_callbacks(L, cb);
}

void stack_dump(lua_State *L) {
  printf("Stack trace:\t");
  int top = lua_gettop(L);         /* depth of the stack */
  for (int i = 1; i <= top; i++) { /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TSTRING: /* strings */
      printf("'%s'", lua_tostring(L, i));
      break;
    case LUA_TBOOLEAN: /* Booleans */
      printf(lua_toboolean(L, i) ? "true" : "false");
      break;
    case LUA_TNUMBER:          /* numbers */
      if (lua_isinteger(L, i)) /* integer? */
        printf("%lld", lua_tointeger(L, i));
      else /* float */
        printf("%g", lua_tonumber(L, i));
      break;
    default: /* other values */
      printf("%s", lua_typename(L, t));
      break;
    }
    printf("  "); /* put a separator */
  }
  printf("\n"); /* end the listing */
}

void parse_callbacks(lua_State *L, DD_CallBackBuff &cb) {
  parse_lua_events(L, cb);
}

void parse_lua_events(lua_State *L, DD_CallBackBuff &cb) {
  // events must be in the form of a table
  int top = lua_gettop(L); /* number of events */
  // printf("# of returns: %d\n", top);
  for (int i = 1; i <= top; i++) {
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TSTRING: {
      // skip
      break;
    }
    case LUA_TBOOLEAN: {
      // skip
      break;
    }
    case LUA_TNUMBER: {
      // skip
      break;
    }
    case LUA_TTABLE: {
      DD_LEvent *levent = cb.getNewEvent();
      if (levent) {
        parse_table(L, levent);
        // printf("\tEvent #%u\n", cb.num_events);
      }
      lua_pop(L, 1); // Pop table
      break;
    }
    default:
      break;
    }
  }
  // printf("# of events parsed: %u\n", cb.num_events);
}

void parse_table(lua_State *L, DD_LEvent *levent, const int tabs) {
  lua_pushnil(L);                // push key on stack for table access
  while (lua_next(L, -2) != 0) { // adds value to the top of the stack
    int t = lua_type(L, -1);     // get value type
    switch (t) {
    case LUA_TSTRING: {
      _val.format("%s", lua_tostring(L, -1));
      lua_pushvalue(L, -2); // copy the key
      _key.format("%s", lua_tostring(L, -1));
      lua_pop(L, 1); // remove copy key

      if (_key.compare("event_id") == 0) {
        levent->handle = _val.str();
      } else {
        bool arg_set = add_arg_LEvent(levent, _key.str(), _val.str());
        if (!arg_set) {
          printf("No more arg slots available\n");
        }
      }
      break;
    }
    case LUA_TBOOLEAN: {
      bool lbool = lua_toboolean(L, -1);
      lua_pushvalue(L, -2); // copy the key
      _key.format("%s", lua_tostring(L, -1));
      lua_pop(L, 1); // remove copy key

      bool arg_set = add_arg_LEvent(levent, _key.str(), lbool);
      if (!arg_set) {
        printf("No more args available\n");
      }

      break;
    }
    case LUA_TNUMBER: {
      if (lua_isinteger(L, -1)) {
        int val = (int)lua_tointeger(L, -1);
        lua_pushvalue(L, -2); // copy the key
        _key.format("%s", lua_tostring(L, -1));
        lua_pop(L, 1); // remove copy key

        bool arg_set = add_arg_LEvent(levent, _key.str(), val);
        if (!arg_set) {
          printf("No more args available\n");
        }
      } else {
        float val = (float)lua_tonumber(L, -1);
        lua_pushvalue(L, -2); // copy the key
        _key.format("%s", lua_tostring(L, -1));
        lua_pop(L, 1); // remove copy key

        bool arg_set = add_arg_LEvent(levent, _key.str(), val);
        if (!arg_set) {
          printf("No more args available\n");
        }
      }
      break;
    }
    case LUA_TTABLE: {
      parse_table(L, levent, tabs + 1);
      break;
    }
    default:
      break;
    }
    lua_pop(L, 1); // remove value
  }
}

void print_table(lua_State *L, const int tabs) {
  // assume table is already on stack
  lua_pushnil(L);                // push key on stack for table access
  while (lua_next(L, -2) != 0) { // adds value to the top of the stack
    // copy the key so that lua_tostring does not modify the original
    lua_pushvalue(L, -2);
    /* uses 'key' (at index -1) and 'value' (at index -2) */
    if (lua_isboolean(L, -2)) {                         // boolean
      bool lbool = lua_toboolean(L, -2);
      printf("key(bool) : %s\n", lua_tostring(L, -1));
      printf("\t%s \n", lbool ? "t" : "f");
      lua_pop(L, 1); // remove copy key
    } else if (lua_isnumber(L, -2)) {
      if (lua_isinteger(L, -2)) {                       // integer
        int32_t val = (int32_t)lua_tointeger(L, -2);
        printf("key(int) : %s\n", lua_tostring(L, -1));
        printf("\t%d \n", val);
        lua_pop(L, 1); // remove copy key
      } else {                                          // float
        float val = (float)lua_tonumber(L, -2);
        printf("key(float) : %s\n", lua_tostring(L, -1));
        printf("\t%.4f \n", val);
        lua_pop(L, 1); // remove copy key
      }
    } else if (lua_isstring(L, -2)) {                   // string
      _key.format("%s", lua_tostring(L, -2));
      printf("key(str) : %s\n", lua_tostring(L, -1));
      printf("\t%s \n", _key.str());
      lua_pop(L, 1); // remove copy key
    } else if (lua_istable(L, -2)) {
      printf("--key (array - %d) : %s\n", tabs, lua_tostring(L, -1));
      lua_pop(L, 1); // remove copy key
      // printf("-->%d\t", tabs); stack_dump(L);	// check entrance
      print_table(L, tabs + 1);
      printf("--array - %d : done\n", tabs);
    }
    lua_pop(L, 1); // remove value
  }
  // printf("<--%d\t", tabs); stack_dump(L);			// check exit
}

int get_lua_ref(lua_State *L, const char *lclass, const char *func) {
  bool lclass_flag = lclass && *lclass;
  bool not_found = true;

  if (lclass_flag) { // find class function
    lua_getglobal(L, lclass);
    if (check_stack_nil(L, -1)) {
      printf("<%s> global doesn't exist.\n", lclass);
    }

    lua_getfield(L, -1, func);
    if (check_stack_nil(L, -1)) {
      printf("<%s> : <%s> function doesn't exist.\n", lclass, func);
    } else {
      // remove global table from stack once class function found
      lua_rotate(L, 1, -1);
      lua_pop(L, 1);
    }
  } else { // find global
    lua_getglobal(L, func);
    if (check_stack_nil(L, -1)) {
      printf("<%s> global function/class doesn't exist.\n", func);
    }
  }

  int t = lua_type(L, -1);
  if (t == LUA_TFUNCTION || t == LUA_TTABLE) {
    return luaL_ref(L, LUA_REGISTRYINDEX); // store reference
  } else {
    lua_pop(L, 1); // remove nil from stack
  }
  return LUA_REFNIL;
}

void clear_lua_ref(lua_State *L, int func_ref)
{
	luaL_unref(L, LUA_REGISTRYINDEX, func_ref);
}