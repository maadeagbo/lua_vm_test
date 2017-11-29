#include <stdio.h>
#include <typeinfo>
#include "DummyClasses.h"
#include "DD_LuaHooks.h"

void check_sizes();
void run_startup(lua_State* L, const char* dir, DD_SimpleQueue *_q);

cbuff<64> lvl_found;
cbuff<256> file_;

int main(const int argv, const char** argc)
{
	lua_State *L = luaL_newstate();         // opens lua
	luaL_openlibs(L);                       // opens standard libraries
	
	// start up queue
	DD_SimpleQueue simple_q;
	DD_ResourceBin res_bin;
	// register instance and callback to lua
	register_instance_lua_xspace<DD_ResourceBin>(L, res_bin);
	register_callback_lua(L, "res_new_agent", 
		&dispatch_<DD_ResourceBin, &DD_ResourceBin::add_new_agent>);

	if (argv == 1) {
		printf("Provide lua file to test: <filename>.lua\n");
	}
	
	for (int i = 1; i < argv; i++)
	{
		if (*argc[i] == '-') {
			const char* nxt = argc[1];
			nxt++;
			if (*nxt && *nxt == 's') { 
				check_sizes();
				return 0;
			}
		}
		else {
			// attempt to read lua script then execute
			parse_luafile(L, argc[i]);
			cbuff<256> dir;
			dir.format("%s%s", ROOT_DIR, "scripts");
			run_startup(L, dir.str(), &simple_q);
		}
	}
	lua_close(L);
	return 0;
}

void check_sizes()
{
	printf("Varying data: %u B\n", (unsigned)sizeof(Varying));
	printf("DD_LEvent data: %u B\n", (unsigned)sizeof(DD_LEvent));
	printf("DD_Queue data: %u B\n", (unsigned)sizeof(DD_SimpleQueue));
	printf("DD_CallBackBuff data: %u B\n", (unsigned)sizeof(DD_CallBackBuff));
}

void run_startup(lua_State* L, const char* dir, DD_SimpleQueue *_q)
{
	// send event with directory
	DD_LEvent levent;
	levent.handle = "io";
	levent.args[0].key = "file";
	levent.args[0].val.type = VType::STRING;
	levent.args[0].val.v_strptr = dir;
	levent.active++;
	stack_dump(L);

	callback_lua(L, "", "generate_levels", levent, _q->cb_events);
	print_callbackbuff(_q->cb_events);
	if (_q->cb_events.num_events == 1 && 
		_q->cb_events.buffer[0].handle.compare("lvls_found") == 0) {
		Varying *v = nullptr;
		cbuff<32> *k = nullptr;
		for (unsigned i = 0; i < _q->cb_events.buffer[0].active; i++) {
			k = &_q->cb_events.buffer[0].args[i].key;
			v = &_q->cb_events.buffer[0].args[i].val;
			switch (v->type) {
				case VType::STRING: 
					if (k->contains("lvl_")) { lvl_found = v->v_strptr.str(); }
				default:
					break;
			}
		}
	}
	stack_dump(L);
	
	if (!(*lvl_found.str()) || lvl_found.compare(" ") == 0) { return; }
	clear_callbackbuff(_q->cb_events);
	// reuse event to test update
	levent.handle = "post";
	levent.args[0].key = "check";
	levent.args[0].val.type = VType::STRING;
	levent.args[0].val.v_strptr = "bang";

	file_.format("/home/maadeagbo/Documents/lua_vm_test/scripts/%s.lua", 
				 lvl_found.str());
	parse_luafile(L, file_.str());

	callback_lua(L, lvl_found.str(), "update", levent, _q->cb_events);
	print_callbackbuff(_q->cb_events);
}