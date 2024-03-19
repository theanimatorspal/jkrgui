#include <Global/Standards.hpp>
#include <glm/glm.hpp>
#include <ksai_config.hpp>

namespace Jkr {
using namespace ksai;
enum class Shapes { EllipseWire, EllipseFill, RectangleFill, RectangleWire, Bezier2_8Wire };
struct Generator {
          using Arguments = std::variant<glm::uvec2, std::vector<glm::uvec2>, std::vector<glm::vec2>, uint32_t, glm::vec4>;
          Generator() = default;
          Generator(Shapes inShape, Arguments inArgs);
          void operator()(int inX,
                          int inY,
                          uint32_t inZ,
                          uint32_t inStartVertexIndex,
                          uint32_t inStartIndexIndex,
                          ksai::v<kstd::Vertex>& modVertices,
                          ksai::v<ksai::ui>& modIndices);

          void operator()(float inX,
                          float inY,
                          float inZ,
                          uint32_t inStartVertexIndex,
                          uint32_t inStartIndexIndex,
                          ksai::v<kstd::Vertex3D>& modVertices,
                          ksai::v<ksai::ui>& modIndices);
          GETTER GetShape() const { return mShape; }
          GETTER GetVertexCount() const { return mVertexCount; }
          GETTER GetIndexCount() const { return mIndexCount; }

      private:
          Arguments mArgs;
          Shapes mShape;
          uint32_t mVertexCount = 0;
          uint32_t mIndexCount = 0;
};
} // namespace Jkr