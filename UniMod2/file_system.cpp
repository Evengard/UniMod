#include <cmath>
#include <cstring>
#include <fstream>

#include "lua.hpp"
#include "map.h"
#include "console.h"
#include "file_system.h"

namespace {
	int oct_to_int(const char* str)
	{
		int ret = 0;
		while (*str == ' ') ++str;
		while (*str >= '0' && *str <= '7')
		{
			ret <<= 3;
			ret += *str - '0';
			++str;
		}
		return ret;
	}
	
	struct LoadF {
		int size;
		int readed;
		std::ifstream *ifile;
		char buffer[Fsystem::tar_block+1];
		LoadF(int t_size, std::ifstream *t_pfile)
			:size(t_size), ifile(t_pfile), readed(0) {}
	};
	 const char* getF (lua_State *L, void *ud, size_t *size) 
	 {
		 LoadF* lf = (LoadF*)ud;
		 if (lf->readed >= lf->size)
			 return NULL;
		 
		 if (lf->ifile->eof()) return NULL;

		 lf->ifile->read(lf->buffer, Fsystem::tar_block);

		 lf->buffer[Fsystem::tar_block] = 0;
		 lf->readed += Fsystem::tar_block;

		 *size = std::strlen(lf->buffer);
		 return (*size > 0) ? lf->buffer : NULL;
	 }

} // anonymous namespace

namespace Fsystem {

	int open_file(lua_State *L,const std::string& file_to_open) // вычитывает файл, возвращает функцию
	{
		std::string path(Map::current_map_name());
		path = "maps\\" + path + "\\"  + path + ".tar";

		std::ifstream ifile(path, std::ifstream::in | std::ifstream::binary);
		if (!ifile.is_open())
			throw Fsystem::Invalid("Can't open: " + path, Fsystem::Invalid::error_open);

		char t_name[100];
		char s_size[12];
		int size;
		while (true) // ищем файл
		{
			ifile.read(t_name, sizeof(t_name));
			if (*t_name == NULL)
			{
				ifile.close();
				throw Fsystem::Invalid("Can't find: " + file_to_open, Fsystem::Invalid::not_exist);
			}

			ifile.seekg(24, std::ifstream::cur);
			ifile.read(s_size, sizeof(s_size));
			size = oct_to_int(s_size);

			int temp_pos = std::ceil(ifile.tellg() / double(tar_block)) * tar_block; //Начало файла
			ifile.seekg(temp_pos, std::ifstream::beg);

			if (t_name != file_to_open)
			{
				int tt = std::ceil((int(ifile.tellg()) + size) / double(tar_block)) * tar_block;;
				ifile.seekg( tt, std::ifstream::beg);
				continue;
			}

			LoadF lf(size, &ifile);
			int status = lua_load(L, getF, &lf, NULL);
			ifile.close();
						
			return status;
		} 
	}
} // namespace Fsystem

