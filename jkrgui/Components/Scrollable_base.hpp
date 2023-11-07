#pragma once
#include "Area_base.hpp"

namespace Jkr::Component
{
	class Scrollable_base : public Area_base
	{
		using ab = Area_base;
	public:
		Scrollable_base(_2d& inR, EventManager& inE) : Area_base(inR, inE) { }
		void Load()
		{
			ab::Load();
		}
		GETTER GetNormalizedValue() { return mValueNormalized; }
		GETTER GetVerticalScrollBarHeigthRatio() { return mVerticalScrollBarHeightRatio; }
		SETTER SetNormalizedValue(float inValue) { mValueNormalized = inValue; }
		SETTER SetVerticalScrollBarHeigthRatio(float inValue) { mVerticalScrollBarHeightRatio = inValue; }
	private:
		float mValueNormalized = 0.0f;
		float mVerticalScrollBarHeightRatio = 0.3f;
	};
}