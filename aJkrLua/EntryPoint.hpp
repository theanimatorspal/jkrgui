#pragma once
#include "BestText.hpp"
#include "BestText_base.hpp"
#include "SDL2/SDL_clipboard.h"
#include "SDL2/SDL_events.h"
#include "Shape_base.hpp"
#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/2d.hpp>
#include <Renderers/BestText_Alt.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/ThreeD/glTF.hpp>
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Misc/MousePicker.hpp>
#define SETTER inline void

namespace Jkr::Component {
using namespace Jkr::Renderer;
class Component_base {
public:
    Component_base() = default;
    Component_base(const Component_base&) = default;
    Component_base(Component_base&&) = default;
    Component_base& operator=(const Component_base&) = default;
    Component_base& operator=(Component_base&&) = default;

    void SetDefaultBoundedRectangle(EventManager& e)
    {
        mBoundedRectangleId = e.SetBoundedRect(mPosition, mDimension, mDepthValue);
    }
    void UpdateDefaultBoundedRectangle(EventManager& e)
    {
        Jkr::BoundRect2D Rect { .mXy = mPosition, .mWh = mDimension };
        e.UpdateBoundRect(mDepthValue, mBoundedRectangleId, Rect);
    }
    GETTER GetDimension() const { return mDimension; }
    GETTER GetPosition() const { return mPosition; }
    SETTER SetDimension(glm::vec2 inVec) { mDimension = inVec; }
    SETTER SetPosition(glm::vec2 inVec)
    {
        mPosition = inVec;
        mTransformMatrix = glm::identity<glm::mat4>();
        mTransformMatrix = glm::translate(mTransformMatrix, glm::vec3(mPosition.x, mPosition.y, 0));
    }
    SETTER SetDepthValue(uint32_t inDepthValue) { mDepthValue = inDepthValue; }
    GETTER GetDepthValue() { return mDepthValue; }
    GETTER IsMouseOnTop(EventManager& e) const { return e.IsMouseWithinAtTopOfStack(mBoundedRectangleId, mDepthValue); }
    GETTER IsMouseWithin(EventManager& e) const { return e.IsMouseWithin(mBoundedRectangleId, mDepthValue); }

private:
    glm::vec2 mDimension;
    glm::vec2 mPosition;
    glm::mat4 mTransformMatrix = glm::identity<glm::mat4>();
    uint32_t mDepthValue;
    uint32_t mBoundedRectangleId;
    bool isFocused = false;
};
}

#ifdef WIN32
#include <Windows.h>
#endif

#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1

#include <Vendor/sol/sol.hpp>
#include <Window.hpp>

enum CallbackType : int {
    Load = 0,
    Event,
    Update,
    Dispatch,
    Draw
};


using namespace std;
using namespace glm;
using namespace Jkr;
using namespace Jkr::Renderer;
using namespace Jkr::Component;

constexpr bool toBool(string_view inString)
{
    if (inString == "true")
        return true;
    else
        return false;
}

inline float toFloat(string_view inString)
{
    return std::stof(string(inString));
}

const string gHeight = to_string(1080 / 2);
const string gWidth = to_string(1920 / 2);
static unordered_map<string_view, string_view> gConfiguration { { "-title", "JkrGUI" },
    { "-height", gHeight },
    { "-width", gWidth },
    { "-store", "false" },
    { "-debug", "true" },
    { "-bgr", "1.0" },
    { "-bgg", "1.0" },
    { "-bgb", "1.0" },
    { "-bga", "1.0" },
    { "-des_size", "1000" },
    { "-des_pool_size", "10000" },
    { "-var_des_size", "5000" },
    { "-cache_folder", "cache/" },
    { "-main_file", "main.lua" } };

static auto FillConfigFrom(const vector<string_view>& inArguments)
{
    for (auto i = inArguments.begin(); i < inArguments.end(); i += 2) {
        auto arg_index = *i;
        auto argument = *(i + 1);
        gConfiguration[arg_index] = argument;
    }
}

using GlobalAccessType = unordered_map<int, int>;

void CreateSDLEventBindings(sol::state& lua);
void CreateGLMBindings(sol::state& lua);
void BindMathFunctions(sol::state& lua);
void Create2DBindings(Instance& i, Window& w, sol::state& l, EventManager& em, Jkr::Renderer::_2d& td, Jkr::Renderer::BestText_Alt& inALT, Jkr::Misc::FrameCapturer& inCapturer);
void Create3DBindings(Instance& i, Window& w, sol::state& l);
void NeuralBindings(sol::state& s);

class GlobalAccessBindingsGenerator {
	using GlobalDataType = std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, std::string>;

public:
    GlobalAccessBindingsGenerator(Instance& i, Window& w, sol::state& l)
    {
        CreateGlobalAccessBindings(i, w, l);
    }
    void CreateGlobalAccessBindings(Instance& i, Window& w, sol::state& l);

private:
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    bool mNotIsBeingAccessed = true;
    std::unordered_map<std::string, GlobalDataType> mGlobalData;
};
