#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <span>
#include <Eigen/Eigen>
#include <sol/sol.hpp>
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
	void PropagateForward(rowV& input);
	void PropagateBackward(rowV& inOutput);
	void ApplySimulatedAnnealing(rowV& inOutput, real inTemperature = 100000, int inMaxIterations = 100);
	void SaveToFile(sv inFileName); /* TODO */
	void LoadFromFile(sv inFileName); /* TODO */

public:
	real GetMeanSquaredError()
	{
		auto& db = mDeltas.back();
		return std::sqrt(db.dot(db) / db.size());
	}
	void CalculateErrors(rowV& output);
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
