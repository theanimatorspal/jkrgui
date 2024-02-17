#define LUA_LIB
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <span>
#include <Eigen/Eigen>
#include <sol/sol.hpp>

namespace Neural {
	// Yo chae Geeks for Geeks bata herdai lekhya
	using real = float;
	using matX = Eigen::MatrixXf;
	using rowV = Eigen::RowVectorXf;
	using colV = Eigen::VectorXf;

	template <typename T>
	using up = std::unique_ptr<T>;
	template <typename T>
	using v = std::vector<T>;

	class Network;

#define mu std::make_unique
	using ActivationFunctionType = std::function<real(real)>;
}


struct Neural::Network {
	Network(std::vector<int> inTopology, real inLearningRate = real(0.005));
	void PropagateForward(rowV& input);
	void PropagateBackward(rowV& inOutput);
	void CalculateErrors(rowV& output);
	void UpdateWeights();
	void Train(std::span<up<rowV>> inData, std::span<up<rowV>> outputData);

	ActivationFunctionType mActivationFunction = [](real x) {return tanhf(x); };
	ActivationFunctionType mActivationFunctionDerivative = [](real x) { return 1 - tanhf(x) * tanhf(x); };
	std::vector<int> mTopology;
	v<up<rowV>> mNeuronLayers; // out network ko layers lai store garxa
	v<up<rowV>> mCacheLayers; // activation function apply navako lai store garxa
	v<up<rowV>> mDeltas; // pratyek neuron ko error contribution lai herxa
	v<up<matX>> mWeights; // connection ko weights
	real mLearningRate;
};


inline Neural::Network::Network(std::vector<int> inTopology, real inLearningRate)
{
	mTopology = std::vector<int>(inTopology);
	mLearningRate = inLearningRate;
	for (uint32_t i = 0; i < mTopology.size(); i++)
	{
		bool isLastEntry = i == mTopology.size() - 1;
		if (isLastEntry)
		{
			mNeuronLayers.emplace_back(mu<rowV>(mTopology[i]));
			mCacheLayers.emplace_back(mu<rowV>(mTopology[i]));
		}
		else {
			mNeuronLayers.emplace_back(mu<rowV>(mTopology[i] + 1)); // bias neuron except for output layer
			mCacheLayers.emplace_back(mu<rowV>(mTopology[i] + 1));
		}

		mDeltas.emplace_back(mu<rowV>(mNeuronLayers.size()));

		if (not isLastEntry)
		{
			mNeuronLayers.back()->coeffRef(mTopology[i]) = 1.0; // bias neuron ko weight 1
			mCacheLayers.back()->coeffRef(mTopology[i]) = 1.0;
		}

		if (i > 0)
		{
			if (i != mTopology.size() - 1)
			{
				mWeights.emplace_back(mu<matX>(mTopology[i - 1] + 1, mTopology[i] + 1)); // bias for both sides
				mWeights.back()->setRandom();
				mWeights.back()->col(mTopology[i]).setZero();
				mWeights.back()->coeffRef(mTopology[i - 1], mTopology[i]) = 1.0;
			}
			else {
				mWeights.emplace_back(mu<matX>(mTopology[i - 1] + 1, mTopology[i])); // bias for left and not for right(last layer)
				mWeights.back()->setRandom();
			}
		}

	}
}

inline void Neural::Network::PropagateForward(rowV& input)
{
	// aba input lai chae input layer banaune
	mNeuronLayers.front()->block(0, 0, 1, mNeuronLayers.front()->size() - 1) = input; // exclude the bias

	// aba data lai forward propagate garne ani apply garne activation function lai
	for (uint32_t i = 1; i < mTopology.size(); i++)
	{
		(*mNeuronLayers[i]) = (*mNeuronLayers[i - 1]) * (*mWeights[i - 1]); // Layer2_out = Matrix * Layer1
		*mCacheLayers[i] = *mNeuronLayers[i];
		mNeuronLayers[i]->block(0, 0, 1, mTopology[i]) = mNeuronLayers[i]->block(0, 0, 1, mTopology[i]).unaryExpr(mActivationFunction);
	}
}

inline void Neural::Network::CalculateErrors(rowV& inOutput)
{
	// Calculate the errors made by neurons of the last layer
	(*mDeltas.back()) = inOutput - (*mNeuronLayers.back());
	for (uint32_t i = mTopology.size() - 2; i > 0; i--) // traverse back in hidden layers
	{
		(*mDeltas[i]) = (*mDeltas[i + 1]) * (mWeights[i]->transpose()); // Layer1 = TransposeWeights1 * Layer2
	}
}

inline void Neural::Network::UpdateWeights()
{
	for (uint32_t i = 0; i < mTopology.size() - 1; i++)
	{
		if (i != mTopology.size() - 2) // if is not output layer then exclude the bias neuron
		{
			for (uint32_t c = 0; c < mWeights[i]->cols() - 1; c++)
			{
				for (uint32_t r = 0; r < mWeights[i]->rows(); r++)
				{
					mWeights[i]->coeffRef(r, c) += mLearningRate *
						mDeltas[i + 1]->coeffRef(c) *
						mActivationFunctionDerivative(mCacheLayers[i + 1]->coeffRef(c)) *
						mNeuronLayers[i]->coeffRef(r);
				}
			}
		}
		else {
			for (uint32_t c = 0; c < mWeights[i]->cols(); c++)
			{
				for (uint32_t r = 0; r < mWeights[i]->rows(); r++)
				{
					mWeights[i]->coeffRef(r, c) += mLearningRate *
						mDeltas[i + 1]->coeffRef(c) *
						mActivationFunctionDerivative(mCacheLayers[i + 1]->coeffRef(c)) *
						mNeuronLayers[i]->coeffRef(r);
				}
			}
		}
	}
}

inline void Neural::Network::PropagateBackward(rowV& output)
{
	CalculateErrors(output);
	UpdateWeights();
}

inline void Neural::Network::Train(std::span<up<rowV>> inData, std::span<up<rowV>> outputData)
{
	//std::stringstream str;
	for (uint32_t i = 0; i < inData.size(); i++)
	{
		//str << "Input to Neural Network is " << *inData[i] << '\n';
		PropagateForward(*inData[i]);
		//str << "Expected Output is " << *outputData[i] << '\n';
		//str << "Output Produced is  " << *mNeuronLayers.back() << '\n';
		PropagateBackward(*outputData[i]);
		auto& outdelta = *mDeltas.back();
		//str << "MSE: " << std::sqrt(outdelta.dot(outdelta) / outdelta.size()) << "\n";
	}
	//std::cout << str.rdbuf();

}

using namespace Neural;

static void ReadCSV(std::string inFileName, std::vector<up<rowV>>& outData)
{
	outData.clear();
	std::ifstream file(inFileName);
	std::string line, word;

	std::getline(file, line, '\n');
	std::stringstream ss(line);
	std::vector<real> parsed_vector;
	while (std::getline(ss, word, ',')) {
		parsed_vector.push_back(real(std::stof(&word[0])));
	}
	uint32_t columns = parsed_vector.size();
	outData.emplace_back(mu<rowV>(columns));
	for (uint32_t i = 0; i < columns; i++)
	{
		outData.back()->coeffRef(0, i) = parsed_vector[i];
	}

	if (file.is_open())
	{
		while (std::getline(file, line, '\n'))
		{
			std::stringstream ss(line);
			outData.emplace_back(mu<rowV>(1, columns));
			uint32_t i = 0;
			while (std::getline(ss, word, ','))
			{
				outData.back()->coeffRef(i) = real(std::stof(&word[0]));
				i++;
			}
		}
	}
}

static void GenData(std::string inFileName, int dataCount)
{
	std::ofstream file1(inFileName + "-in");
	std::ofstream file2(inFileName + "-out");
	for (uint32_t r = 0; r < dataCount; r++)
	{
		real x = round(rand() / real(RAND_MAX));
		real y = round(rand() / real(RAND_MAX));
		file1 << x << ", " << y << '\n';
		if (x == y)
		{
			file2 << 1.0 << '\n';
		}
		else if (x != y)
		{
			file2 << 0.0 << '\n';
		}
	}
}

extern "C" __declspec(dllexport) int luaopen_neural(lua_State * l)
{
	sol::state_view s(l);
	s.set_function("neural", []() {
		std::cout << "Hello World" << '\n';
		});

	auto neural = s["neur"].get_or_create<sol::table>();
	//auto neural = s["n"].get_or_create<sol::table>();
	neural.new_usertype<Neural::Network>(
		"network",
		sol::call_constructor,
		sol::factories(
			[&](std::vector<int>& inTopology) {
				return Neural::Network(inTopology);
			},
			[&](std::vector<int>& inTopology, real inLearningRate) {
				return Neural::Network(inTopology, inLearningRate);
			}
		),
		"weight_of_connection",
		[](Neural::Network& inNetwork, int inLayer, int inNeuron1, int inNeuron2) -> real
		{
			return inNetwork.mWeights[inLayer]->coeff(inNeuron1, inNeuron2);
		},
		"value_of_neuron",
		[](Neural::Network& inNetwork, int inLayer, int inNeuron)
		{
			return inNetwork.mNeuronLayers[inLayer]->coeff(inNeuron);
		},
		"get_layer_vector_float",
		[](Neural::Network& inNetwork, int inLayer) -> std::vector<float>
		{
			std::vector<float> outvec;
			auto& rowv = *inNetwork.mNeuronLayers[inLayer];
			for (int i = 0; i < rowv.size(); i++)
			{
				outvec.push_back(rowv.coeff(i));
			}
			return outvec;
		},
		"propagate_forward",
		[](Neural::Network& inNetwork, std::vector<float> inFloat){
			rowV v(inFloat.size());
			for (int i = 0; i < inFloat.size(); i++)
			{
				v.coeffRef(i) = inFloat[i];
			}
			inNetwork.PropagateForward(v);
		},

		"propagate_backward_current",
		[](Neural::Network& inNetwork, std::vector<float> inFloat)
		{
			rowV v(inFloat.size());
			for (int i = 0; i < inFloat.size(); i++)
			{
				v.coeffRef(i) = inFloat[i];
			}
			inNetwork.PropagateBackward(v);
		},
		/* Dummy train function all TODO remove this*/
		"dummy_train",
		[&](Neural::Network& inNetwork, int inDataCount) {
			std::vector<up<rowV>> in_data, out_data;
			GenData("test", inDataCount);
			ReadCSV("test-in", in_data);
			ReadCSV("test-out", out_data);
			inNetwork.Train(in_data, out_data);
		}
	);

	return 0;
}

int main() {
	auto Topology = std::vector<int>({ 2, 3, 2, 1 });
	Neural::Network n(Topology);
	std::vector<up<rowV>> in_data, out_data;
	GenData("test", 5000);
	ReadCSV("test-in", in_data);
	ReadCSV("test-out", out_data);
	n.Train(in_data, out_data);
	getchar();
	exit(0);
}
