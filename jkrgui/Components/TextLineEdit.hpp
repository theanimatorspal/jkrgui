#include "Area_base.hpp"
#include "BTextButtonRect.hpp"

namespace Jkr::Component
{
	class TextLineEdit : protected Area_base
	{
		using ab = Area_base;
	public:
		TextLineEdit(_2d& inR, EventManager& inE) : Area_base(inR, inE)
		{ }
		void Load()
		{
		}
		void Event()
		{

		}
	private:
		uint32_t mMaxLength;
		std::string mCurrentString;
		std::string_view mStringViewOfString;
	};
}