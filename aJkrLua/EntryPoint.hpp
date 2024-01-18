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
#define SETTER inline void

// This is our Legacy C++ Component System, This class is used inside so cannot be deleted now,
// TODO: Needs cleanup
namespace Jkr::Component {
using namespace Jkr::Renderer;
class Component_base {
public:
    Component_base(const Component_base&) = default;
    Component_base(Component_base&&) = default;
    Component_base& operator=(const Component_base&) = default;
    Component_base& operator=(Component_base&&) = default;

public:
    GETTER& GetFastTexts() { return mFastTexts; }
    GETTER& GetLines() { return mLines; }
    GETTER& GetShapes() { return mShapes; }

private:
    std::vector<glm::uvec2> mFastTexts;
    std::vector<glm::uvec2> mLines;
    std::vector<glm::uvec2> mShapes;

public:
    Component_base(_2d& inR, EventManager& inE)
        : r(inR)
        , e(inE)
    {
        mPosition = { INT_MAX, INT_MAX };
        mDimension = { INT_MAX, INT_MAX };
        mDepthValue = INT_MAX;
        mWindow = nullptr;
        mWh = INT_MAX;
        mWw = INT_MAX;
    }
    void CheckExpectations()
    {
        assert(mPosition.x != static_cast<float>(INT_MAX));
        assert(mPosition.y != static_cast<float>(INT_MAX));
        assert(mDimension.x != static_cast<float>(INT_MAX));
        assert(mDimension.y != static_cast<float>(INT_MAX));
        assert(mDepthValue != static_cast<float>(INT_MAX));
        assert(mWh != INT_MAX);
        assert(mWw != INT_MAX);
        assert(mWindow != nullptr);
    }
    void SetDefaultBoundedRectangle()
    {
        mBoundedRectangleId = e.SetBoundedRect(mPosition, mDimension, mDepthValue);
    }
    void UpdateDefaultBoundedRectangle()
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
    GETTER IsFocused() const { return isFocused; }
    GETTER IsMouseOnTop() const { return e.IsMouseWithinAtTopOfStack(mBoundedRectangleId, mDepthValue); }
    GETTER IsMouseWithin() const { return e.IsMouseWithin(mBoundedRectangleId, mDepthValue); }
    SETTER ToggleFocus() { isFocused = !isFocused; }
    SETTER SetFocus(bool inFocus) { isFocused = inFocus; }
    GETTER GetTranslationMatrix() const
    {
        return mTransformMatrix;
    }
    glm::vec2 FromComponent(glm::vec2 inComp)
    {
        return glm::vec2(inComp) + mPosition;
    }
    glm::vec2 FromWorld(glm::vec2 inWorld)
    {
        return glm::vec2(inWorld) - mPosition;
    }
    glm::vec2 ToCentralizedCoords(glm::uvec2 mBoxDimension, glm::uvec2 mBox)
    {
    }

protected:
    _2d& r;
    EventManager& e;
    Window* mWindow = nullptr;
    uint32_t mWh = 0, mWw = 0;

public:
    SETTER SetWindow(Window* inWindow, uint32_t inW, uint32_t inH)
    {
        mWindow = inWindow;
        mWw = inW;
        mWh = inH;
    }
    SETTER SetWindowDimensions(float inW, float inH)
    {
        mWw = inW;
        mWh = inH;
    }
    GETTER GetWindowWidth() const
    {
        return mWw;
    }
    GETTER GetWindowHeight() const
    {
        return mWh;
    }
    GETTER& GetWindow()
    {
        return mWindow;
    }

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

static int my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_execption, sol::string_view description)
{
    std::cout << "An Exception has occured in the function here is what it says: ";
    if (maybe_execption) {
        std::cout << "(Straight from the exception) ";
        const std::exception& ex = *maybe_execption;
        std::cout << ex.what() << std::endl;
    } else {
        std::cout << "(From the Descriptor Parameter) ";
        std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
        std::cout << std::endl;
    }
    return sol::stack::push(L, description);
}

void CreateSDLEventBindings(sol::state& lua);
void CreateGLMBindings(sol::state& lua);
void BindMathFunctions(sol::state& lua);
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
    { "-bgr", "1.0" },
    { "-bgg", "1.0" },
    { "-bgb", "1.0" },
    { "-bga", "1.0" },
    { "-des_size", "1000" },
    { "-des_pool_size", "10000" },
    { "-var_des_size", "5000" },
    { "-cache_folder", "cache/" },
    { "-main_file", "main.lua" } };

static auto FillConfig(const vector<string_view>& inArguments)
{
    for (auto i = inArguments.begin(); i < inArguments.end(); i += 2) {
        auto arg_index = *i;
        auto argument = *(i + 1);
        gConfiguration[arg_index] = argument;
    }
}

void Create2DBindings(Instance& i, Window& w, sol::state& l, EventManager& em, Jkr::Renderer::_2d& td, Jkr::Renderer::BestText_Alt& inALT);

void Create3DBindings(Instance& i, Window& w, sol::state& l);
