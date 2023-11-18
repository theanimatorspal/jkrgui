#pragma once
#include "Component_base.hpp"

namespace Jkr::Component
{
	template<size_t NoOfComponents>
	class VerticalLayout : public Component_base
	{
		using cb = Component_base;
	public:
		VerticalLayout(_2d& inR, EventManager& inE) : Component_base(inR, inE) { }

		void AddComponent(const Sp<cb>& inComponent)
		{
			mComponents[mCurrentIndex++] = inComponent;
		}

		void ResetPositionsAndDimensions(const std::array<float, NoOfComponents> inComponentProportion)
		{
			float ComponentCount = mComponents.size();
			float CurrentVSizePointer = this->GetPosition().y;

			for (int i = 0; i < mComponents.size(); ++i)
			{
				float VSizeOfEachElementWithPadding = this->GetDimension().y * inComponentProportion[i];
				float VSizeOfEachElementWithoutPadding = (VSizeOfEachElementWithPadding - VPadding * 2);
				float HSizeOfEachElementWithPadding = this->GetDimension().x;
				float HSizeOfEachElementWithoutPadding = HSizeOfEachElementWithPadding - HPadding * 2;

				CurrentVSizePointer += VPadding;
				mComponents[i]->SetDimension(glm::vec2(HSizeOfEachElementWithoutPadding, VSizeOfEachElementWithoutPadding));
				mComponents[i]->SetPosition(glm::vec2(HPadding + this->GetPosition().x, CurrentVSizePointer));
				CurrentVSizePointer += mComponents[i]->GetDimension().y;
				CurrentVSizePointer += VPadding;
			}
		}
		void ResetPositionsAndDimensions()
		{
			float ComponentCount = mComponents.size();
			float CurrentVSizePointer = this->GetPosition().y;

			for (int i = 0; i < mComponents.size(); ++i)
			{
				float VSizeOfEachElementWithPadding = this->GetDimension().y * (1.0f / static_cast<float>(NoOfComponents));
				float VSizeOfEachElementWithoutPadding = (VSizeOfEachElementWithPadding - VPadding * 2);
				float HSizeOfEachElementWithPadding = this->GetDimension().x;
				float HSizeOfEachElementWithoutPadding = HSizeOfEachElementWithPadding - HPadding * 2;

				CurrentVSizePointer += VPadding;
				mComponents[i]->SetDimension(glm::vec2(HSizeOfEachElementWithoutPadding, VSizeOfEachElementWithoutPadding));
				mComponents[i]->SetPosition(glm::vec2(HPadding + this->GetPosition().x, CurrentVSizePointer));
				CurrentVSizePointer += mComponents[i]->GetDimension().y;
				CurrentVSizePointer += VPadding;
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
