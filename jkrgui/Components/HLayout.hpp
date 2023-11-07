#pragma once
#include "Component_base.hpp"

namespace Jkr::Component
{
	template<size_t NoOfComponents>
	class HorizontalLayout : public Component_base
	{
		using cb = Component_base;
	public:
		HorizontalLayout(_2d& inR, EventManager& inE) : Component_base(inR, inE) { }

		void AddComponent(const Sp<cb>& inComponent)
		{
			mComponents[mCurrentIndex++] = inComponent;
		}

		void ResetPositionsAndDimensions(const std::array<float, NoOfComponents> inComponentProportion)
		{
			float ComponentCount = mComponents.size();
			float CurrentHSizePointer = 0;

			for (int i = 0; i < mComponents.size(); ++i)
			{
				float HSizeOfEachElementWithPadding = static_cast<float>(this->GetDimension().x) * inComponentProportion[i];
				float HSizeOfEachElementWithoutPadding = (HSizeOfEachElementWithPadding - HPadding * 2);
				float VSizeOfEachElementWithPadding = static_cast<float>(this->GetDimension().y);
				float VSizeOfEachElementWithoutPadding = VSizeOfEachElementWithPadding - VPadding * 2;

				CurrentHSizePointer += HPadding;
				mComponents[i]->SetDimension(glm::vec2(HSizeOfEachElementWithoutPadding, VSizeOfEachElementWithoutPadding));
				mComponents[i]->SetPosition(glm::vec2(CurrentHSizePointer, VPadding + this->GetPosition().y));
				CurrentHSizePointer += mComponents[i]->GetDimension().x;
				CurrentHSizePointer += HPadding;
			}
		}

		SETTER SetHPadding(uint32_t inPadding) { HPadding = inPadding; }
		SETTER SetVPadding(uint32_t inPadding) { VPadding = inPadding; }
	private:
		std::array<Sp<cb>, NoOfComponents> mComponents;
		uint32_t mCurrentIndex = 0;
	private:
		uint32_t HPadding = 10;
		uint32_t VPadding = 10;
	};
}
