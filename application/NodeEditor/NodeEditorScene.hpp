#pragma once
#include "GUIWindow.hpp"
#include "GUI2DRenderer.hpp"
#include "GeneralRenderers/Line.hpp"
#include "NodeView.hpp"


namespace App {
	class NodeEditorScene
	{
	public:
		NodeEditorScene() = default;
		void LoadConfigAndRegister(Jkr::GUI2DRenderer& inR, Jkr::Font::FastTextRenderer& inT, Jkr::Renderer::Line& inLineR, App::LuaConfig& inLua, const std::string_view inConfigFilePath = "");
		void Update(Jkr::GUI2DRenderer& inR, uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH);
		void Dispatch(const Jkr::GUIWindow& mWindow);
		void EventCall(App::LuaConfig& inConfig, Jkr::GUIWindow& inWindow, Jkr::Renderer::Line& inLineR, std::any inEvent);
		void Draw(Jkr::GUI2DRenderer& inR, uint32_t inW, uint32_t inH, const Jkr::GUIWindow& inWindow);
		void DrawConnections(Jkr::Renderer::Line& inLineR, Jkr::GUIWindow& inWindow, uint32_t inW, uint32_t inH);
		void UpdateConnections(Jkr::Renderer::Line& inLineR, Jkr::GUIWindow& inWindow, uint32_t inW, uint32_t inH);
		void UpdateMenuBarSelection(glm::ivec2 inVec) { MenuBarSelection = inVec; }
	private:
		void AddConnection(Jkr::Renderer::Line& inLineR, uint32_t inNodeIdFrom, uint32_t inNodeIdTo, uint32_t inInputIndex);
		void GetSelectedInputsAndOutput(Jkr::GUIWindow& inWindow, std::any& inEvent, const int& invalid, glm::uvec2& Selection, int& InputIndex);
		void CheckSelectionAndToggleConnection(Jkr::Renderer::Line& inLineR);
	private:
		struct NodeConnection
		{
			uint32_t mDstNodeInputIndex;
			uint32_t mSrcNodeId;
			uint32_t mDstNodeId;
			uint32_t mConnectionLineId;
			uint32_t invalid = false;
		};
		uint32_t mGridId;
		uint32_t mGridCenterId;
		Up<Button> mBaseButtonRect;
		Up<Button> mBaseButtonCircle;
		struct NodeViewInfo {
			glm::ivec2 mPosition;
			NodeView mNodeView;
		};
		std::vector<NodeViewInfo> mNodeViews;
		std::vector<NodeConnection> mConnections;
	private:
		const int invalid = -100;
		int InputIndex = -1;
		glm::uvec2 Selection = { invalid, invalid };
	private:
		glm::ivec2 MenuBarSelection = { invalid, invalid };
	};

}
