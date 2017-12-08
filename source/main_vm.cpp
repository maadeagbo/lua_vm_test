#include <stdio.h>
#include <typeinfo>
#include "DD_LuaHooks.h"
#include "DummyClasses.h"

void check_sizes();
void run_startup(lua_State *L, const char *dir, DD_SimpleQueue *_q);

cbuff<64> lvl_found;
cbuff<256> file_;

int main(const int argv, const char **argc) {
  lua_State *L = init_lua_state();  // initialize lua

  // start up queue
  DD_SimpleQueue simple_q;
  DD_ResourceBin res_bin;
  // register instance and callback to lua
  register_instance_lua_xspace<DD_ResourceBin>(L, res_bin);
  register_callback_lua(
      L, "res_new_agent",
      &dispatch_<DD_ResourceBin, &DD_ResourceBin::add_new_agent>);

  if (argv == 1) {
    printf("Provide lua file to test: <filename>.lua\n");
    return 0;
  }

  for (int i = 1; i < argv; i++) {
    if (*argc[i] == '-') {
      const char *nxt = argc[1];
      nxt++;
      if (*nxt && *nxt == 's') {
        check_sizes();
        return 0;
      }
    } else {
      // attempt to read lua script then execute
      parse_luafile(L, argc[i]);
      cbuff<256> dir;
      dir.format("%s%s", ROOT_DIR, "scripts/");
      run_startup(L, dir.str(), &simple_q);

      print_all_agents(res_bin);
    }
  }
  lua_close(L);
  return 0;
}

void check_sizes() {
  printf("Varying<32> data: %u B\n", (unsigned)sizeof(Varying<32>));
  printf("Varying<128> data: %u B\n", (unsigned)sizeof(Varying<128>));
  printf("DD_LEvent data: %u B\n", (unsigned)sizeof(DD_LEvent));
  printf("DD_Queue data: %u B\n", (unsigned)sizeof(DD_SimpleQueue));
  printf("DD_CallBackBuff data: %u B\n", (unsigned)sizeof(DD_CallBackBuff));
}

void run_startup(lua_State *L, const char *dir, DD_SimpleQueue *_q) {
  // send event with directory
  DD_LEvent levent;
  levent.handle = "io";
  add_arg_LEvent<const char *>(&levent, "file", "bam_bam");

  callback_lua(L, levent, _q->cb_events, "generate_levels");
  print_callbackbuff(_q->cb_events);
  const char *lvl_1 = get_callback_val<const char>("lvl_1", _q->cb_events);

  if (!lvl_1) {
    return;
  }
  lvl_found = lvl_1;
  clear_callbackbuff(_q->cb_events);
  // reuse event to test update
  levent.handle = "post";
  levent.active = 0;

  file_.format("%s%s%s.lua", ROOT_DIR, "scripts/", lvl_found.str());
  bool file_found = parse_luafile(L, file_.str());
  if (!file_found) {
    return;
  }

  // invoke init function
  callback_lua(L, levent, _q->cb_events, "init", lvl_found.str());
  clear_callbackbuff(_q->cb_events);

  int class_ref = get_lua_ref(L, "", lvl_found.str());
  int func_ref = get_lua_ref(L, lvl_found.str(), "update");
  printf("Global ref <%d>, Func ref : %d\n", class_ref, func_ref);

  // change event
  add_arg_LEvent<float>(&levent, "test_float", 151515.f);
  levent.active++;
  callback_lua(L, levent, _q->cb_events, func_ref, class_ref);
  print_callbackbuff(_q->cb_events);
  clear_callbackbuff(_q->cb_events);
}