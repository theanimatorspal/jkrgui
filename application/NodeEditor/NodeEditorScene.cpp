#include "NodeEditorScene.hpp"

void App::NodeEditorScene::LoadConfigAndRegister(Jkr::GUI2DRenderer& inR, Jkr::Font::FastTextRenderer& inT, Jkr::Renderer::Line& inLineR, App::LuaConfig& inLua, const std::string_view inFilePath)
{
	inR.RegisterDynamicQuad(
		mGridId,
		[=](Jkr::GUI2DRenderer& inR, ksai::kstd::ui32 inId)
		{
			inR.DrawToQuadTextureDispatch(inId, Jkr::GUI2DRenderer::Shape::GridFill,
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(1.0f / 8.0f, 1.0f / 8.0f, 0.0f, 1.0f),
			glm::vec4(0.0f)
			);
		},
		1000 / 2,
		1000 / 2
	);
	inR.RegisterDynamicQuad(
		mGridCenterId,
		[=](Jkr::GUI2DRenderer& inR, ksai::kstd::ui32 inId)
		{
			inR.DrawToQuadTextureDispatch(inId, Jkr::GUI2DRenderer::Shape::Circle,
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
			glm::vec4(1.0f)
			);
		},
		100,
		100
	);

	mBaseButtonRect = MakeUp<Button>(inR, inT);
	mBaseButtonRect->Register<Button::Type::Rectangle>(0, 0, 1, 1, glm::vec3(1.0f, 1.0f, 1.0f), 0.3f);
	mBaseButtonCircle = MakeUp<Button>(inR, inT);
	mBaseButtonCircle->Register<Button::Type::CircleInRectangle>(0, 0, 100, 100, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	if (inFilePath != "")
	{
		auto NodeEditor_CreateNode = [&](std::string inString, float r, float g, float b, float a, float x, float y) -> int
			{
				NodeViewInfo info;
				info.mPosition = glm::ivec2(x, y);
				info.mNodeView = std::move(
					NodeView().SetColor(glm::vec3(r, g, b))
					.SetNodeName(inString)
					.Register(*mBaseButtonRect, *mBaseButtonCircle, inR, inT)
				);
				mNodeViews.push_back(std::move(info));
				return mNodeViews.size() - 1;
			};

		auto NodeEditor_RegisterNumberOfInputs = [&](int inId, int InDataType, int Data)
			{
				mNodeViews[inId]
					.mNodeView
					.PushConnection(inR, inT, *mBaseButtonCircle, static_cast<App::Node::DataType>(inId), glm::vec3(0, 0, 0));
			};

		auto NodeEditor_AddConnection = [&](int From, int To, int Input) {
			AddConnection(inLineR, From, To, Input);
			};

		inLua.GetHandle().new_usertype<glm::ivec2>("uvec2",
			sol::constructors<glm::ivec2(int, int)>(),
			"x", &glm::ivec2::x,
			"y", &glm::ivec2::y
		);

		auto NodeEditor_UpdateNodePosition = [&](int id, const glm::ivec2& v1) {
			mNodeViews[id].mPosition = v1;
			};

		inLua.GetHandle().set_function("NodeEditor_CreateNode", NodeEditor_CreateNode);
		inLua.GetHandle().set_function("NodeEditor_RegisterNumberOfInputs", NodeEditor_RegisterNumberOfInputs);
		inLua.GetHandle().set_function("NodeEditor_AddConnection", NodeEditor_AddConnection);
		inLua.GetHandle().set_function("NodeEditor_UpdateNodePosition", NodeEditor_UpdateNodePosition);
		inLua.ExecuteFile(inFilePath.data());
	}
}

void App::NodeEditorScene::Update(Jkr::GUI2DRenderer& inR, uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH)
{
	inR.UpdateDynamicQuad(mGridId, inX, inY, inW, inH, 5);
	inR.UpdateDynamicQuad(mGridCenterId, (inW - inX) / 2.0f - 10.0f, (inH - inY) / 2.0f - 10.0f, 10.0f, 10.0f, 4);

	for (auto& u : mNodeViews)
	{
		u.mNodeView.Update(inR, glm::vec2(u.mPosition.x, u.mPosition.y), 3);
	}


	for (auto i = mNodeViews.begin(); i < mNodeViews.end(); i++)
	{
		for (auto j = mNodeViews.begin(); j < mNodeViews.end(); j++)
		{
			if (i->mPosition == j->mPosition)
			{
				i->mPosition -= glm::ivec2(10, 0);
				j->mPosition += glm::ivec2(10, 0);
			}
		}
	}

}

void App::NodeEditorScene::Dispatch(const Jkr::GUIWindow& mWindow)
{
	//int inMouseX, inMouseY;
	//SDL_GetMouseState(&inMouseX, &inMouseY);
	//auto x = mWindow.GetWindowSize();
	//glm::vec2 TargetPos = {
	//	std::lerp(-1.0f, 1.0f, static_cast<float>(inMouseX) / (x.first)),
	//	std::lerp(1.0f, -1.0f, static_cast<float>(inMouseY) / (x.second))
	//};
	//mRenderer.DrawToQuadTextureDispatch(mGridId, Jkr::GUI2DRenderer::Shape::Fill,
	//	glm::vec4(0.0f),
	//	glm::vec4(0.0f),
	//	glm::vec4(0.0f)
	//);
	//mRenderer.DrawToQuadTextureDispatch(mGridId, Jkr::GUI2DRenderer::Shape::GridFill,
	//	glm::vec4(TargetPos[0], TargetPos[1], 1.0f, 1.0f),
	//	glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
	//	glm::vec4(0.0f)
	//);
	//mRenderer.DrawToQuadTextureDispatch(mGridId, Jkr::GUI2DRenderer::Shape::Circle,
	//	glm::vec4(TargetPos[0], TargetPos[1], 1.0f, 1.0f),
	//	glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
	//	glm::vec4(0.5f)
	//);
}

void App::NodeEditorScene::EventCall(App::LuaConfig& inLua, Jkr::GUIWindow& inWindow, Jkr::Renderer::Line& inLineR, std::any inEvent)
{
	GetSelectedInputsAndOutput(inWindow, inEvent, invalid, Selection, InputIndex);
	CheckSelectionAndToggleConnection(inLineR);

	auto i = 0;
	static auto CurrentMovableNode = -1;

	for (auto& u : mNodeViews)
	{
		if (u.mNodeView.GetButton()->GetIsButtonPushed())
		{
			if (CurrentMovableNode == -1) CurrentMovableNode = i;
			else CurrentMovableNode = -1;
		}
		i++;
	}

	int x, y;
	auto CurrentState = SDL_GetMouseState(&x, &y);

	if (CurrentMovableNode != -1)
	{
		mNodeViews[CurrentMovableNode].mPosition.x = x - 10;
		mNodeViews[CurrentMovableNode].mPosition.y = y - 10;
	}
	sol::function LuaCallBack = inLua.GetHandle()["NodeEventCallBack"];

	auto CurrentMouseButton = -1;
	switch (CurrentState)
	{
	case SDL_BUTTON(1):
		CurrentMouseButton = 1;
		break;
	case SDL_BUTTON(2):
		CurrentMouseButton = 2;
		break;
	case SDL_BUTTON(3):
		CurrentMouseButton = 3;
		break;
	}

	int NumKeys;
	auto CurrentKeyBoardState = SDL_GetKeyboardState(&NumKeys);

	int HoveredButton = -1;
	for (int i = 0; i < mNodeViews.size(); i++)
	{
		if (mNodeViews[i].mNodeView.GetButton()->GetIsBeingHovered())
		{
			HoveredButton = i;
		}
	}
	LuaCallBack(HoveredButton, CurrentMouseButton, 0, MenuBarSelection);
	MenuBarSelection = { invalid, invalid };
}

void App::NodeEditorScene::GetSelectedInputsAndOutput(Jkr::GUIWindow& inWindow, std::any& inEvent, const int& invalid, glm::uvec2& Selection, int& InputIndex)
{
	int selections = 0;
	for (int i = 0; i < mNodeViews.size(); i++)
	{
		auto& u = mNodeViews[i];
		u.mNodeView.EventCall(inWindow, inEvent);
		if (u.mNodeView.GetCurrentSelectedButton() != invalid)
		{
			if (u.mNodeView.GetCurrentSelectedButton() == -1)
			{
				Selection[0] = i;
			}
			else {
				Selection[1] = i;
				InputIndex = u.mNodeView.GetCurrentSelectedButton();
			}
			selections++;
		}
	}
	if (selections > 1)
	{
		for (auto& u : mNodeViews) u.mNodeView.ResetSelectedButton();
	}
}

void App::NodeEditorScene::CheckSelectionAndToggleConnection(Jkr::Renderer::Line& inLineR)
{
	if (Selection[0] != Selection[1] && Selection[0] != invalid && Selection[0] != invalid && InputIndex != -1)
	{
		auto i = std::find_if(mConnections.begin(), mConnections.end(), [&](NodeConnection& in) {
			return  in.mSrcNodeId == Selection[0] && in.mDstNodeId == Selection[1] && in.mDstNodeInputIndex == InputIndex;
			});
		if (i == mConnections.end())
		{
			AddConnection(inLineR, Selection[0], Selection[1], InputIndex);
		}
		else if (i->invalid == true) {
			i->invalid = false;
		}
		else {
			i->invalid = true;
		}
		InputIndex = -1;
		Selection = glm::uvec2(invalid, invalid);

		for (auto& u : mNodeViews) u.mNodeView.ResetSelectedButton();
	}

}


void App::NodeEditorScene::Draw(Jkr::GUI2DRenderer& inR, uint32_t inW, uint32_t inH, const Jkr::GUIWindow& inWindow)
{
	for (auto& u : mNodeViews)
	{
		u.mNodeView.Draw(inR, inW, inH, inWindow);
	}
}

void App::NodeEditorScene::DrawConnections(Jkr::Renderer::Line& inLineR, Jkr::GUIWindow& inWindow, uint32_t inW, uint32_t inH)
{
	inLineR.DrawBatched(inWindow, glm::vec4(1.0f, 0.0f, 1.2f, 1.0f), inW, inH, 0, mConnections.size());
}

void App::NodeEditorScene::UpdateConnections(Jkr::Renderer::Line& inLineR, Jkr::GUIWindow& inWindow, uint32_t inW, uint32_t inH)
{
	for (auto const& u : mConnections)
	{
		if (u.invalid != true)
		{
			const auto src = mNodeViews[u.mSrcNodeId].mNodeView.GetOutputIndexPosition(mNodeViews[u.mSrcNodeId].mPosition);
			const auto dst = mNodeViews[u.mDstNodeId].mNodeView.GetInputIndexPosition(mNodeViews[u.mDstNodeId].mPosition, u.mDstNodeInputIndex);
			inLineR.UpdateLine(
				u.mConnectionLineId,
				src,
				dst,
				3
			);
		}
		else {
			const auto src = glm::vec2(0, 0);
			const auto dst = glm::vec2(0, 0);
			inLineR.UpdateLine(
				u.mConnectionLineId,
				src,
				dst,
				3
			);
		}
	}
}

void App::NodeEditorScene::AddConnection(Jkr::Renderer::Line& inLineR, uint32_t inNodeIdFrom, uint32_t inNodeIdTo, uint32_t inInputIndex)
{
	uint32_t id;
	auto src = mNodeViews[inNodeIdFrom].mNodeView.GetOutputIndexPosition(mNodeViews[inNodeIdFrom].mPosition);
	auto dst = mNodeViews[inNodeIdTo].mNodeView.GetInputIndexPosition(mNodeViews[inNodeIdTo].mPosition, inInputIndex);
	inLineR.AddLine(src, dst, 3, id);

	mConnections.push_back(
		NodeConnection{
			.mDstNodeInputIndex = inInputIndex,
			.mSrcNodeId = inNodeIdFrom,
			.mDstNodeId = inNodeIdTo,
			.mConnectionLineId = id,
			.invalid = false
		}
	);
}

