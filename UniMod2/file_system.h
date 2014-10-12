#pragma once
#include <string>
#include <vector>
#include <fstream>


namespace Fsystem {
	const int max_path = 0x104; // from win header
	const int tar_block = 512;

	int load_map_file(lua_State *L, const std::string& file_to_open); // ищет нужный луа файл, взависмости от карты и текущего режима

	class Invalid { // for errors
	public:
		enum Type {
			error_open, // тар файла нет
			not_exist // не может найти внутри ар файла файл
		};
		std::string what;
		Type type;
		Invalid(const std::string& s, Type t_type)
			:what(s), type(t_type) {}
	};
}
