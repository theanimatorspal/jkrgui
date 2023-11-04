#pragma once
#include <glm/glm.hpp>
#include <Window.hpp>

namespace Jkr::Component
{
	class Component_base
	{
	public:
		constexpr void SetBoundedRect(glm::uvec2 mXy, glm::uvec2 mWh)
		{
			mXy = mXy;
			mWh = mWh;
		}
		constexpr bool IsPointWithin(glm::uvec2 inXy) {
			return ((mXy.x < inXy.x) && (mXy.x > inXy.x + mWh.y))
				&& ((mXy.y < inXy.y) && (mXy.y > inXy.y + mWh.y));
		}
	private:
		glm::uvec2 mXy;
		glm::uvec2 mWh;
	};

}