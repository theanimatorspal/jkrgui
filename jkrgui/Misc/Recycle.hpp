#include "Config.hpp"

namespace Jkr::Misc {
using namespace ksai;

template <typename T> class Recycle {
public:
  GETTER Exists(T inData) { return static_cast<bool>(mRecycleBin[inData]); }
  void Remove(T inData) {
    if (Exists(inData)) {
      std::remove(mRecycleBin.begin(), mRecycleBin.end(), inData);
    }
  }
  void Add(T inData) { mRecycleBin[inData] = true; }

private:
  umap<T, bool> mRecycleBin;
};
} // namespace Jkr::Misc