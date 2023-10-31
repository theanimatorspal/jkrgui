#include "EventManager.hpp"

void App::EventManager::FireEvent(Type inType, Data inData)
{
	mEventData[inType] = inData;
}

void App::EventManager::RegisterListener(Type inType, Listener inFunction)
{
	mListeners.push_back(std::make_pair(inType, inFunction));
}

void App::EventManager::ProcessEvents()
{
	for (auto& u : mListeners)
	{
		if (mEventData.contains(u.first))
		{
			u.second(mEventData[u.first]);
		}
	}
	mEventData.clear();
}
