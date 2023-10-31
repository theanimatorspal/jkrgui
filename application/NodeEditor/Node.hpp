#pragma once
#include "../Application/LuaConfig.hpp"
#include "GUI2DRenderer.hpp"
#include <variant>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

namespace App {
	class Node
	{
	public:
		/*
			The First Bool Decides Whether the Variant Contains Data Or NOT
		*/
		using Data = std::variant<bool, bool, glm::vec2, glm::vec3, glm::ivec2, glm::ivec3, std::string>;
		enum class DataType : uint32_t
		{
			Bool = 1,
			Vec2 = 2,
			Vec3 = 3,
			Ivec2 = 4,
			Ivec3 = 5,
			String = 6
		};
		GETTER& GetCookFunction() const { return mCookFunction; }
		GETTER& GetCookedData() const { return mOutData; }
		void Cook() { mOutData = mCookFunction(this); }
		void SetCookFunction(std::function<DataType(Node*)> inCookFunction) { mCookFunction = inCookFunction; }
		void PushConnection(DataType inDataType, Data inData) { mInConnections.push_back(std::pair(inDataType, inData)); }
		Node& SetNext(Node* inNode) { mNext = inNode; return *this; }
		Node& SetNodeName(const std::string_view inString) { mName = inString; return *this; }
	protected:
		std::string mName;
		std::function<DataType(Node*)> mCookFunction;
		std::vector <std::pair<DataType, Data>> mInConnections;
		DataType mOutData;
		Node* mNext = nullptr;
	};
}

