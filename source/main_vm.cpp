#include <stdio.h>
#include <typeinfo>
#include "DD_LuaHooks.h"
#include "DummyClasses.h"

void check_sizes();
void run_startup(lua_State *L, const char *dir, DD_SimpleQueue *_q);
int retrieve_func_ptr(lua_State *L);

cbuff<64> lvl_found;
cbuff<256> file_;
int dummy_global_ref = -1;
int dummy_func_ref = -1;

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
	// add retrieve_func_ptr to scripts
	add_func_to_scripts(L, retrieve_func_ptr, "register_update_func");

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

      //print_all_agents(res_bin);
    }
  }
  lua_close(L);
  return 0;
}

void check_sizes() {
  printf("Varying<32> data: %u B\n", (unsigned)sizeof(Varying<32>));
  printf("Varying<256> data: %u B\n", (unsigned)sizeof(Varying<256>));
  printf("DD_LEvent data: %u B\n", (unsigned)sizeof(DD_LEvent));
  printf("DD_Queue data: %u B\n", (unsigned)sizeof(DD_SimpleQueue));
  //printf("DD_CallBackBuff data: %u B\n", (unsigned)sizeof(DD_CallBackBuff));
  printf("DD_FuncBuff data: %u B\n", (unsigned)sizeof(DD_FuncBuff));
}

void run_startup(lua_State *L, const char *dir, DD_SimpleQueue *_q) {
  // send event with directory
  DD_LEvent levent;
  levent.handle = "io";
  add_arg_LEvent<const char *>(&levent, "file", "bam_bam");

	int gen_func = get_lua_ref(L, nullptr, "generate_levels");
  callback_lua(L, levent, _q->fbuffer, gen_func, -1);
  print_buffer(_q->fbuffer);
  const char *lvl_1 = _q->fbuffer.get_func_val<const char>("1");

  if (!lvl_1) {
    return;
  }
  lvl_found = lvl_1;
  // reuse event to test update
  levent.handle = "post";
  levent.active = 0;

  file_.format("%s%s%s.lua", ROOT_DIR, "scripts/", lvl_found.str());
  bool file_found = parse_luafile(L, file_.str());
  if (!file_found) {
    return;
  }

  // invoke init function
	int class_ref = get_lua_ref(L, "", lvl_found.str());
	int func_ref = get_lua_ref(L, lvl_found.str(), "init");
	callback_lua(L, levent, _q->fbuffer, func_ref, class_ref);
  //callback_lua(L, levent, _q->cb_events, "init", lvl_found.str());

  func_ref = get_lua_ref(L, lvl_found.str(), "update");
  //printf("Global ref <%d>, Func ref : %d\n", class_ref, func_ref);

  // change event
  add_arg_LEvent<float>(&levent, "test_float", 151515.f);
  levent.active++;
  callback_lua(L, levent, _q->fbuffer, func_ref, class_ref);
  //print_callbackbuff(_q->cb_events);
}

int retrieve_func_ptr(lua_State * L) {
	dummy_global_ref = get_lua_object(L);
	dummy_func_ref = get_lua_ref(L, dummy_global_ref, "update");
	printf("retrieve_func_ptr:: %d, %d\n", dummy_global_ref, dummy_func_ref);
	return 0;
}
