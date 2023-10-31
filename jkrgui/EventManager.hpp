#pragma once
#include "Window.hpp"

namespace Jkr
{
	class EventManager
	{
	public:
		void ProcessEvents() {
			while (SDL_PollEvent(&mEvent))
			{
				switch (mEvent.type)
				{
				case SDL_QUIT:
					should_quit = true;
				default:
					break;
				}
			}
			mCurrentPushedMouseButton = SDL_GetMouseState(&mMousePos.x, &mMousePos.y);
		}
		GETTER ShouldQuit() const { return should_quit; }
		GETTER GetEventHandle() const { return mEvent; }
		GETTER GetMousePos() const { return mMousePos; }
	private:
		SDL_Event mEvent;
		glm::ivec2 mMousePos;
		int mCurrentPushedMouseButton;
		bool should_quit = false;
	};
}