#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <span>
#include <Eigen/Eigen>
#include <sol/sol.hpp>

namespace Jkr::Neural {
	template <typename T>
	using up = std::unique_ptr<T>;
	template <typename T>
	using v = std::vector<T>;
	class Network;
#define mu std::make_unique
	using real = float;
	using matX = Eigen::MatrixXf;
	using rowV = Eigen::RowVectorXf;
	using colV = Eigen::VectorXf;
	using ActivationFunctionType = std::function<real(real)>;
}

struct Jkr::Neural::Network {
	Network(std::vector<int> inTopology, real inLearningRate = real(0.005));
	Network(Network&& inNetwork) = default;
	Network(Network& inNetwork) = delete;
	void PropagateForward(rowV& input);
	void PropagateBackward(rowV& inOutput);
	void ApplySimulatedAnnealing(rowV& inOutput, real inTemperature = 100000, int inMaxIterations = 100);


	void CalculateErrors(rowV& output);
	void UpdateWeightsBackPropagation();
	ActivationFunctionType mActivationFunction = [](real x) {return tanhf(x); };
	ActivationFunctionType mActivationFunctionDerivative = [](real x) { return 1 - tanhf(x) * tanhf(x); };
	std::vector<int> mTopology;
	v<rowV> mNeuronLayers;
	v<rowV> mCacheLayers; 
	v<rowV> mDeltas; 
	v<matX> mWeights; 
	real mLearningRate;
};
