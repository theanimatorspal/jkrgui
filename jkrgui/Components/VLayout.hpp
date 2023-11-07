#pragma once
#include "Component_base.hpp"

namespace Jkr::Component
{
	class HorizontalLayout : public Component_base
	{
		using cb = Component_base;
	public:
		HorizontalLayout(_2d& inR, EventManager& inE) : Component_base(inR, inE) { }

		void AddComponent(const Sp<cb>& inComponent)
		{
			mComponents.emplace_back(inComponent);
		}
		void ResetPositionsAndDimensions()
		{
			float ComponentCount = mComponents.size();
			float HSizeOfEachElementWithPadding = static_cast<float>(this->GetDimension().x) / ComponentCount;
			float HSizeOfEachElementWithoutPadding = HSizeOfEachElementWithPadding - HPadding * 2;
			float VSizeOfEachElementWithPadding = static_cast<float>(this->GetDimension().y);
			float VSizeOfEachElementWithoutPadding = VSizeOfEachElementWithPadding - VPadding * 2;

			float CurrentHSizePointer = 0;
			for (int i = 0; i < mComponents.size(); ++i)
			{
				CurrentHSizePointer += HPadding;
				mComponents[i]->SetDimension(glm::vec2(HSizeOfEachElementWithoutPadding, VSizeOfEachElementWithoutPadding));
				mComponents[i]->SetPosition(glm::vec2(CurrentHSizePointer, VPadding));
				CurrentHSizePointer += mComponents[i]->GetDimension().x;
				CurrentHSizePointer += HPadding;
			}
		}
	private:
		std::vector<Sp<cb>> mComponents;
	private:
		class Bug
		{

		};
	private:
		uint32_t HPadding = 10;
		uint32_t VPadding = 10;
	};
}