#pragma once
#include "Node.hpp"
#include "GUI2DRenderer.hpp"
#include "GUIWindow.hpp"
#include "FastTextFrontEnd.hpp"
#include "../Colors.hpp"
#include "../Button.hpp"
#include "../TextButton.hpp"

namespace App {
	class NodeView : public Node
	{
	public:
		NodeView() = default;
		NodeView(NodeView&& inNodeView) {
			mNodeRectColor = inNodeView.mNodeRectColor;
			mName = inNodeView.mName;
			mRectTextButton = std::move(inNodeView.mRectTextButton);
			mOutputButton = std::move(inNodeView.mOutputButton);
			mInputButtons = std::move(inNodeView.mInputButtons);
			mInConnections = std::move(inNodeView.mInConnections);
			inNodeView.mRectTextButton.reset();
		}
		NodeView& operator=(NodeView&& inNodeView) {
			if (this != &inNodeView)
			{
				mNodeRectColor = inNodeView.mNodeRectColor;
				mName = inNodeView.mName;
				mRectTextButton = std::move(inNodeView.mRectTextButton);
				mOutputButton = std::move(inNodeView.mOutputButton);
				mInputButtons = std::move(inNodeView.mInputButtons);
				mInConnections = std::move(inNodeView.mInConnections);
				inNodeView.mRectTextButton.reset();
			}
			return *this;
		}
	public:
		NodeView& Register(const Button& inRectButton, const Button& inCircleButton, Jkr::GUI2DRenderer& inR, Jkr::Font::FastTextRenderer& inFr);
		void Update(Jkr::GUI2DRenderer& inR, glm::vec2 inXy, uint32_t inDepthValue);
		NodeView& SetNext(NodeView* inNode) { mNext = inNode; return *this; }
		NodeView& SetColor(glm::vec3 inColor) { mNodeRectColor = inColor; return *this; }
		NodeView& SetNodeName(const std::string_view inString) { Node::SetNodeName(inString); return *this; }
		void Draw(Jkr::GUI2DRenderer& inR, uint32_t inWindowW, uint32_t inWindowH, const Jkr::GUIWindow& inWindow);
		void EventCall(Jkr::GUIWindow& inWindow, std::any inEventInfo);
		void PushConnection(Jkr::GUI2DRenderer& inR, Jkr::Font::FastTextRenderer& inFr, const Button& inCircleButton, DataType inDataType, Data inData);
		constexpr glm::vec2 GetInputIndexPosition(glm::vec2 inXy, uint32_t inIndex) { return glm::vec2(inXy.x + (inIndex) * 20.0f + 10, inXy.y - 10); }
		constexpr glm::vec2 GetOutputIndexPosition(glm::vec2 inXy) { return glm::vec2(inXy.x + mFeatures.mWidth / 2.0f + 5, inXy.y + 25 + 10); }
		GETTER& GetButton() const { return mRectTextButton; }
		GETTER& GetCurrentSelectedButton() { return mCurrentSelectedButton; }
		void ResetSelectedButton() { mCurrentSelectedButton = invalid; }
	private:
		Up<TextButton> mRectTextButton = nullptr;
		Up<Button> mOutputButton = nullptr;
		std::vector<Button> mInputButtons;
		glm::vec3 mInputColor = glm::vec3(0.2f, 0.8f, 0.1f);
		glm::vec3 mNodeRectColor = glm::vec3(0.8f, 0.2f, 0.1f);
		glm::vec3 mOutputColor = glm::vec3(0.3f, 0.1f, 1.0f);
		ksai::Font::FastText::TextFeatures mFeatures;
		NodeView* mNext = nullptr;
		const int invalid = -100;
		int mCurrentSelectedButton = invalid;
	};

}
