#pragma once
#include "lua.hpp"

namespace Config {
	enum Flags {
		fl_debug_mode, 
		fl_more_noxs // больше одного нокса
	}; 

	void init();

	int check_flag(Config::Flags flag);

	extern int environment;
}