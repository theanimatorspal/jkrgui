#include <ksai_config.hpp>

namespace Jkr::Misc {
using namespace ksai;

template <typename T> class RecycleBin {
      public:
          GETTER Exists(T inData) { return static_cast<bool>(mRecycleBin[inData]); }
          GETTER IsEmpty();
          void Remove(T inData);
          void Add(T inData);
          T Get();

      private:
          umap<T, bool> mRecycleBin;
};

template <typename T> inline void RecycleBin<T>::Remove(T inData) {
          if (Exists(inData)) mRecycleBin.erase(inData);
}
template <typename T> inline void RecycleBin<T>::Add(T inData) { mRecycleBin[inData] = true; }
template <typename T> GETTER RecycleBin<T>::IsEmpty() { return mRecycleBin.size() == 0; }
template <typename T> inline T RecycleBin<T>::Get() {
          auto X = mRecycleBin.begin()->first;
          Remove(X);
          return X;
}
} // namespace Jkr::Misc