#include "LuaConfig.hpp"
#include <glm/glm.hpp>
#include <vector>

template <>
glm::vec3 App::LuaConfig::GetGlobal(const char* inString)
{
	sol::optional<sol::table> table = mLua[inString];
	if (table)
	{
		float r = table.value()[1];
		float g = table.value()[2];
		float b = table.value()[3];
		return glm::vec3(r, g, b);
	}
	return glm::vec3(0.0f);
}

template <>
std::vector<std::vector<std::string>> App::LuaConfig::GetGlobal(const char* inString) {
	sol::optional<sol::table> menuTable = mLua[inString];
	std::vector<std::vector<std::string>> menuItem;

	if (menuTable) {
		for (int i = 1; i <= menuTable.value().size(); i++) {
			sol::optional<sol::table> subMenuTable = menuTable.value()[i];
			std::vector<std::string> subMenuItem;

			if (subMenuTable) {
				for (int j = 1; j <= subMenuTable.value().size(); j++) {
					sol::optional<std::string> item = subMenuTable.value()[j];
					if (item) {
						subMenuItem.push_back(item.value());
					}
				}
				menuItem.push_back(subMenuItem);
			}
		}
	}

	return menuItem;
}

