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
	using real = float;
	using matX = Eigen::MatrixXf;
	using rowV = Eigen::RowVectorXf;
	using colV = Eigen::VectorXf;
	using ActivationFunctionType = std::function<real(real)>;
}

struct Jkr::Neural::Network {
	Network(std::vector<int> inTopology, real inLearningRate = real(0.005));
	Network(const Network& inNetwork) = default;
	Network& operator=(const Network& inNetwork) = default;

public: /* Interface */
	void PropagateForward(const rowV& input);
	void PropagateBackward(rowV& inOutput);
	void AddSAData(rowV& inInput, rowV& inOutput);
	void ApplySA(real inTemperature = 100000, int inMaxIterations = 100);
	void SaveToFile(sv inFileName); /* TODO */
	void LoadFromFile(sv inFileName); /* TODO */

public:
	real EnergySA();
	real GetMeanSquaredError(const rowV& inInput, const rowV& inOutput)
	{
		PropagateForward(inInput);
		CalculateErrors(inOutput);
		auto& db = mDeltas.back();
		return std::sqrt(db.dot(db) / db.size());
	}
	void CalculateErrors(const rowV& output);
	void UpdateWeightsBackPropagation();

public:
	ActivationFunctionType mActivationFunction = [](real x) {return tanhf(x); };
	ActivationFunctionType mActivationFunctionDerivative = [](real x) { return 1 - tanhf(x) * tanhf(x); };

public:
	v<int> mTopology;
	v<rowV> mNeuronLayers;
	v<rowV> mCacheLayers;
	v<rowV> mDeltas;
	v<matX> mWeights;
	real mLearningRate;
};


inline void Jkr::Neural::Network::CalculateErrors(const rowV& inOutput)
{
	ZoneScoped;
	mDeltas.back() = inOutput - mNeuronLayers.back();
	for (uint32_t i = mTopology.size() - 2; i > 0; i--)
	{
		mDeltas[i] = mDeltas[i + 1] * mWeights[i].transpose();
	}
}

inline void Jkr::Neural::Network::PropagateForward(const rowV& input)
{
	ZoneScoped;
	mNeuronLayers.front().block(0, 0, 1, mNeuronLayers.front().size() - 1) = input; // exclude the bias
	for (uint32_t i = 1; i < mTopology.size(); i++)
	{
		mNeuronLayers[i] = mNeuronLayers[i - 1] * mWeights[i - 1]; // Layer2_out = Matrix * Layer1
		mCacheLayers[i] = mNeuronLayers[i];
		mNeuronLayers[i].block(0, 0, 1, mTopology[i]) = mNeuronLayers[i].block(0, 0, 1, mTopology[i]).unaryExpr(mActivationFunction);
	}
}
