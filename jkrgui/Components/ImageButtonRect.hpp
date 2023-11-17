#include "Button_base.hpp"

namespace Jkr::Component {

class ImageButtonRect : public Button_base {
public:
    using bb = Button_base;

public:
    ImageButtonRect(_2d& inR, EventManager& inE)
        : bb(inR, inE)
    {
    }

    /*
    @brief This function expects you to have set the Dimension and DepthValue beforehand.
    */
    void Load()
    {
        auto ImgContainer = Jkr::Generator(Jkr::Shapes::Rectangle, this->GetDimension());
        r.sh.AddImage(mFilePath, mImageId);
        r.sh.Add(ImgContainer, 0, 0, this->GetDepthValue(), mShapeId);
        bb::Load();
    }

    constexpr void DrawImages(Window& inWindow, uint32_t inW, uint32_t inH)
    {
        r.sh.BindImage(*this->GetWindow(), mImageId);
        r.sh.Draw(inWindow, glm::vec4(), inW, inH, mShapeId, mShapeId, this->GetTranslationMatrix());
    }

    void Update()
    {
        bb::Update();
    }

    SETTER SetFilePath(std::string_view inFilePath) { mFilePath = inFilePath; }

private:
    std::string_view mFilePath;
    uint32_t mImageId;
    uint32_t mShapeId;
};

}