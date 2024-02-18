#include "SimpleNN.hpp"
#include <algorithm>

using namespace Jkr;
using namespace Neural;

Neural::Network::Network(std::vector<int> inTopology, real inLearningRate)
{
	mTopology = std::vector<int>(inTopology);
	mLearningRate = inLearningRate;
	for (uint32_t i = 0; i < mTopology.size(); i++)
	{
		bool isLastEntry = i == mTopology.size() - 1;
		if (isLastEntry)
		{
			mNeuronLayers.emplace_back(rowV(mTopology[i]));
			mCacheLayers.emplace_back(rowV(mTopology[i]));
		}
		else {
			mNeuronLayers.emplace_back(rowV(mTopology[i] + 1)); // bias neuron except for output layer
			mCacheLayers.emplace_back(rowV(mTopology[i] + 1));
		}

		mDeltas.emplace_back(mNeuronLayers.size());

		if (not isLastEntry)
		{
			mNeuronLayers.back().coeffRef(mTopology[i]) = 1.0; // bias neuron ko weight 1
			mCacheLayers.back().coeffRef(mTopology[i]) = 1.0;
		}

		if (i > 0)
		{
			if (i != mTopology.size() - 1)
			{
				mWeights.emplace_back(matX(mTopology[i - 1] + 1, mTopology[i] + 1)); // bias for both sides
				mWeights.back().setRandom();
				mWeights.back().col(mTopology[i]).setZero();
				mWeights.back().coeffRef(mTopology[i - 1], mTopology[i]) = 1.0;
			}
			else {
				mWeights.emplace_back(matX(mTopology[i - 1] + 1, mTopology[i])); // bias for left and not for right(last layer)
				mWeights.back().setRandom();
			}
		}

	}
}

void Neural::Network::PropagateForward(rowV& input)
{
	mNeuronLayers.front().block(0, 0, 1, mNeuronLayers.front().size() - 1) = input; // exclude the bias
	for (uint32_t i = 1; i < mTopology.size(); i++)
	{
		mNeuronLayers[i] = mNeuronLayers[i - 1] * mWeights[i - 1]; // Layer2_out = Matrix * Layer1
		mCacheLayers[i] = mNeuronLayers[i];
		mNeuronLayers[i].block(0, 0, 1, mTopology[i]) = mNeuronLayers[i].block(0, 0, 1, mTopology[i]).unaryExpr(mActivationFunction);
	}
}


void Neural::Network::CalculateErrors(rowV& inOutput)
{
	mDeltas.back() = inOutput - mNeuronLayers.back();
	for (uint32_t i = mTopology.size() - 2; i > 0; i--)
	{
		mDeltas[i] = mDeltas[i + 1] * mWeights[i].transpose();
	}
}

void Neural::Network::UpdateWeightsBackPropagation()
{
	for (uint32_t i = 0; i < mTopology.size() - 1; i++)
	{
		if (i != mTopology.size() - 2)
		{
			for (uint32_t c = 0; c < mWeights[i].cols() - 1; c++)
			{
				for (uint32_t r = 0; r < mWeights[i].rows(); r++)
				{
					mWeights[i].coeffRef(r, c) += mLearningRate *
						mDeltas[i + 1].coeffRef(c) *
						mActivationFunctionDerivative(mCacheLayers[i + 1].coeffRef(c)) *
						mNeuronLayers[i].coeffRef(r);
				}
			}
		}
		else {
			for (uint32_t c = 0; c < mWeights[i].cols(); c++)
			{
				for (uint32_t r = 0; r < mWeights[i].rows(); r++)
				{
					mWeights[i].coeffRef(r, c) += mLearningRate *
						mDeltas[i + 1].coeffRef(c) *
						mActivationFunctionDerivative(mCacheLayers[i + 1].coeffRef(c)) *
						mNeuronLayers[i].coeffRef(r);
				}
			}
		}
	}
}

void Neural::Network::PropagateBackward(rowV& output)
{
	CalculateErrors(output);
	UpdateWeightsBackPropagation();
}


/* SIMULATED ANNEALING */
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/
/*============================================================================================*/


inline static v<matX> NeighbourWeight(Jkr::Neural::Network& inNetwork)
{
	v<matX> neighbourWeights;
	for (auto& i : inNetwork.mWeights)
	{
		matX newMatrix = matX(i);
		newMatrix.unaryExpr([](real inX) -> real {
			float rand1 = rand() / RAND_MAX;
			int rand3 = roundf(rand() / RAND_MAX);
			inX += std::powf(-1, rand3) * rand1;
			return inX;
			});
		neighbourWeights.push_back(newMatrix);
	}
	return neighbourWeights;
}

inline static Jkr::Neural::Network NeighbourNetwork(Jkr::Neural::Network inNetwork)
{
	inNetwork.mWeights = NeighbourWeight(inNetwork);
	return inNetwork;
}

inline static real ProbabilityDistrFunction(real inE, real inE_new, real inT)
{
	if (inE_new < inE)
		return 1;
	else
		return std::exp(-(inE_new - inE) / inT);
}

void Jkr::Neural::Network::ApplySimulatedAnnealing(rowV& inOutput, real inTemperature, int inMaxIterations)
{
	CalculateErrors(inOutput);
	Jkr::Neural::Network State = Jkr::Neural::Network(mTopology, mLearningRate);

	int k_max = inMaxIterations;
	real CurrentTemperature = inTemperature;
	for (int k = 0; k < inMaxIterations; ++k)
	{
		float CurrentTemperature = CurrentTemperature * (1 - (k + 1) / k_max);
		auto State_new = NeighbourNetwork(State);
		auto P = ProbabilityDistrFunction(State.GetMeanSquaredError(), State_new.GetMeanSquaredError(), CurrentTemperature);
		if (P >= rand() / RAND_MAX) {
			State = State_new;
		}
	}
	*this = State;
}

void Jkr::Neural::Network::SaveToFile(sv inFileName)
{
}

void Jkr::Neural::Network::LoadFromFile(sv inFileName)
{
}
