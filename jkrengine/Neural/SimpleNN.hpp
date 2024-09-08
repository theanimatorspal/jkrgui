// #define EIGEN_USE_BLAS
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <span>
#include <Eigen/Eigen>
#include <sol/sol.hpp>
#include <Tracy/tracy/Tracy.hpp>
#include <Global/Config.hpp>

namespace Jkr::Neural {
using namespace ksai;
class Network;
using real                   = float;
using matX                   = Eigen::MatrixXf;
using kernelX                = Eigen::MatrixXi;
using rowV                   = Eigen::RowVectorXf;
using colV                   = Eigen::VectorXf;
using ActivationFunctionType = std::function<real(real)>;
} // namespace Jkr::Neural

struct Jkr::Neural::Network {
    Network(std::vector<int> inTopology, real inLearningRate = real(0.005));
    Network(const Network& inNetwork)            = default;
    Network& operator=(const Network& inNetwork) = default;

    public: /* Interface */
    void PropagateForward(const rowV& input);
    void PropagateBackward(rowV& inOutput);
    void AddSAData(rowV& inInput, rowV& inOutput);
    void ApplySA(real inTemperature = 100000, int inMaxIterations = 100);
    void SaveToFile(sv inFileName);   /* TODO */
    void LoadFromFile(sv inFileName); /* TODO */

    public:
    real EnergySA();
    real GetMeanSquaredError(const rowV& inInput, const rowV& inOutput) {
        PropagateForward(inInput);
        CalculateErrors(inOutput);
        auto& db = mDeltas.back();
        return std::sqrt(db.dot(db) / db.size());
    }
    real GetMeanSquaredErrorCurrent() {
        auto& db = mDeltas.back();
        return std::sqrt(db.dot(db) / db.size());
    }
    void CalculateErrors(const rowV& output);
    void UpdateWeightsBackPropagation();

    public:
    ActivationFunctionType mActivationFunction           = [](real x) { return tanhf(x); };
    ActivationFunctionType mActivationFunctionDerivative = [](real x) {
        return 1 - tanhf(x) * tanhf(x);
    };

    public:
    v<int> mTopology;
    v<rowV> mNeuronLayers;
    v<rowV> mCacheLayers;
    v<rowV> mDeltas;
    v<matX> mWeights;
    real mLearningRate;
};
