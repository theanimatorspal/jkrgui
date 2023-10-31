#pragma once
#include <string>
#include <iostream>
#include <Vendor/lua/lua.hpp>
#include <sol/sol.hpp>

namespace App
{
	class LuaConfig
	{
	public:
		LuaConfig() = default;
		~LuaConfig() = default;
		void ExecuteFile(const char* inString) {
			mLua.script_file(inString);
		}

		sol::state& GetHandle() { return mLua; }
		template <typename T>
		T GetGlobal(const char* inString);
	private:
		sol::state mLua;
	};
}
