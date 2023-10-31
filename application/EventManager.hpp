#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <variant>
#include <functional>

namespace App {
	class EventManager
	{
	public:
		enum class Type
		{
			MenuBarClick
		};
		using Data = std::variant <glm::ivec2,
			glm::ivec3,
			glm::vec2,
			glm::vec3>;

		using Listener = std::function<void(Data inData)>;
		void FireEvent(Type inType, Data inData);
		void RegisterListener(Type inType, Listener inFunction);
		void ProcessEvents();
	private:
		std::unordered_map<Type, Data> mEventData;
		std::vector<std::pair<Type, Listener>> mListeners;
	};
}
