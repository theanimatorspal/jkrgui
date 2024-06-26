#include "SimpleNN.hpp"
#include <fstream>
#include <iostream>

using namespace Jkr;
using namespace Neural;

Neural::Network::Network(std::vector<int> inTopology, real inLearningRate) {
    mTopology     = std::vector<int>(inTopology);
    mLearningRate = inLearningRate;
    for (uint32_t i = 0; i < mTopology.size(); i++) {
        bool isLastEntry = i == mTopology.size() - 1;
        if (isLastEntry) {
            mNeuronLayers.emplace_back(rowV(mTopology[i]));
            mCacheLayers.emplace_back(rowV(mTopology[i]));
        } else {
            mNeuronLayers.emplace_back(
                 rowV(mTopology[i] + 1)); // bias neuron except for output layer
            mCacheLayers.emplace_back(rowV(mTopology[i] + 1));
        }

        mDeltas.emplace_back(mNeuronLayers.size());

        if (not isLastEntry) {
            mNeuronLayers.back().coeffRef(mTopology[i]) = 1.0; // bias neuron ko weight 1
            mCacheLayers.back().coeffRef(mTopology[i])  = 1.0;
        }

        if (i > 0) {
            if (i != mTopology.size() - 1) {
                mWeights.emplace_back(
                     matX(mTopology[i - 1] + 1, mTopology[i] + 1)); // bias for both sides
                mWeights.back().setRandom();
                mWeights.back().col(mTopology[i]).setZero();
                mWeights.back().coeffRef(mTopology[i - 1], mTopology[i]) = 1.0;
            } else {
                mWeights.emplace_back(
                     matX(mTopology[i - 1] + 1,
                          mTopology[i])); // bias for left and not for right(last layer)
                mWeights.back().setRandom();
            }
        }
    }
}

void Neural::Network::UpdateWeightsBackPropagation() {
    for (uint32_t i = 0; i < mTopology.size() - 1; i++) {
        if (i != mTopology.size() - 2) {
            for (uint32_t c = 0; c < mWeights[i].cols() - 1; c++) {
                for (uint32_t r = 0; r < mWeights[i].rows(); r++) {
                    mWeights[i].coeffRef(r, c) +=
                         mLearningRate * mDeltas[i + 1].coeffRef(c) *
                         mActivationFunctionDerivative(mCacheLayers[i + 1].coeffRef(c)) *
                         mNeuronLayers[i].coeffRef(r);
                }
            }
        } else {
            for (uint32_t c = 0; c < mWeights[i].cols(); c++) {
                for (uint32_t r = 0; r < mWeights[i].rows(); r++) {
                    mWeights[i].coeffRef(r, c) +=
                         mLearningRate * mDeltas[i + 1].coeffRef(c) *
                         mActivationFunctionDerivative(mCacheLayers[i + 1].coeffRef(c)) *
                         mNeuronLayers[i].coeffRef(r);
                }
            }
        }
    }
}

void Neural::Network::PropagateBackward(rowV& output) {
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

inline static v<matX> NeighbourWeight(Jkr::Neural::Network& inNetwork) {
    ZoneScoped;
    v<matX> neighbourWeights;
    for (const auto& i : inNetwork.mWeights) {
        ZoneScoped;
        matX newMatrix = matX(i);
        newMatrix      = newMatrix.unaryExpr([](real inX) -> real {
            float rand1 = rand() / (float)RAND_MAX * 0.01;
            int rand3   = roundf((int)rand() % 2);
            inX += std::powf(-1, rand3) * rand1;
            return inX;
        });
        neighbourWeights.push_back(newMatrix);
    }
    return neighbourWeights;
}

inline static Jkr::Neural::Network NeighbourNetwork(const Jkr::Neural::Network& inNetwork) {
    ZoneScoped;
    Jkr::Neural::Network Net(inNetwork);
    Net.mWeights = NeighbourWeight(Net);
    return Net;
}

inline static real ProbabilityDistrFunction(real inE, real inE_new, real inT) {
    ZoneScoped;
    if (inE_new < inE)
        return 1;
    else
        return std::exp(-(inE_new - inE) / inT);
}

static v<rowV> Inputs;
static v<rowV> Outputs;

Jkr::Neural::real Jkr::Neural::Network::EnergySA() {
    ZoneScoped;
    int data_count = Inputs.size();
    float Energy   = 0.0;
    for (int i = 0; i < data_count; i++) {
        auto& Input  = Inputs[i];
        auto& Output = Outputs[i];
        Energy += GetMeanSquaredError(Input, Output);
    }
    return Energy / data_count;
}

void Jkr::Neural::Network::AddSAData(rowV& inInput, rowV& inOutput) {
    Inputs.push_back(inInput);
    Outputs.push_back(inOutput);
}

void Jkr::Neural::Network::ApplySA(real inTemperature, int inMaxIterations) {
    ZoneScoped;
    Jkr::Neural::Network State = *this;
    int k_max                  = inMaxIterations;
    real CurrentTemperature    = inTemperature;
    for (int k = 0; k < inMaxIterations; k++) {
        ZoneScoped;
        CurrentTemperature = CurrentTemperature * (1.0f - (k + 1.0f) / (float)k_max);
        auto State_new     = NeighbourNetwork(State);
        float Energy       = State.EnergySA();
        float Energy_new   = State_new.EnergySA();
        auto P             = ProbabilityDistrFunction(Energy, Energy_new, CurrentTemperature);
        if (P >= static_cast<float>(rand()) / (float)RAND_MAX) {
            State = State_new;
        }
    }
    float Energy = State.EnergySA();
    std::cout << Energy << "And" << State.mWeights.back().coeff(0) << '\t';
    std::cout << '\n';
    *this = State;
    Inputs.clear();
    Outputs.clear();
}

enum class Op { Read, Write };

template <typename T>
concept isNumeral = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <typename T>
concept IsVector = requires(T& t) {
    typename T::value_type;
    typename T::size_type;
    typename T::iterator;
    typename T::const_iterator;
    { std::declval<T>().size() } -> std::same_as<typename T::size_type>;
    { std::declval<T>().begin() } -> std::same_as<typename T::iterator>;
    { std::declval<T>().end() } -> std::same_as<typename T::iterator>;
    { std::declval<T>().cbegin() } -> std::same_as<typename T::const_iterator>;
    { std::declval<T>().cend() } -> std::same_as<typename T::const_iterator>;
};

template <typename T, Op inShouldWrite = Op::Write>
inline static T RW(std::fstream& inFile, std::optional<T> inType = std::nullopt) {
    if constexpr (isNumeral<T>) {
        if constexpr (inShouldWrite == Op::Write) {
            T integer = inType.value();
            inFile.write(reinterpret_cast<char*>(&integer), sizeof(T));
            return integer;
        } else {
            T integer = 0;
            inFile.read(reinterpret_cast<char*>(&integer), sizeof(T));
            return integer;
        }
    }

    if constexpr (std::is_same_v<T, rowV>) {
        if constexpr (inShouldWrite == Op::Write) {
            RW<int>(inFile, inType.value().size());
            for (auto& i : inType.value()) {
                RW<float>(inFile, i);
            }
            return rowV(0);
        } else {
            int size = RW<int, Op::Read>(inFile);
            rowV vec(size);
            for (int i = 0; i < size; i++) {
                vec.coeffRef(i) = RW<real, Op::Read>(inFile);
            }
            return vec;
        }
    }

    if constexpr (std::is_same_v<T, matX>) {
        if constexpr (inShouldWrite == Op::Write) {
            int rowscount = inType.value().rows();
            int colcount  = inType.value().cols();
            RW<int>(inFile, rowscount);
            RW<int>(inFile, colcount);
            for (int y = 0; y < rowscount; y++) {
                for (int x = 0; x < colcount; x++) {
                    RW<real>(inFile, inType.value().coeff(x, y));
                }
            }
            return inType.value();
        } else {
            int rowscount = RW<int, Op::Read>(inFile);
            int colcount  = RW<int, Op::Read>(inFile);
            matX mat(rowscount, colcount);
            for (int y = 0; y < rowscount; y++) {
                for (int x = 0; x < colcount; x++) {
                    mat.coeffRef(x, y) = RW<real, Op::Read>(inFile);
                }
            }
            return mat;
        }
    }
}

template <typename T, Op inShouldWrite = Op::Write>
inline static void RW(std::fstream& inFile, IsVector auto& outType) {
    if constexpr (inShouldWrite == Op::Write) {
        RW<int>(inFile, outType.size());
        for (auto& i : outType) {
            RW<T>(inFile, i);
        }
    } else {
        int size = RW<int, Op::Read>(inFile);
        outType.clear();
        outType.shrink_to_fit();
        outType.reserve(size);
        for (int i = 0; i < size; i++) {
            outType.push_back(RW<T, Op::Read>(inFile));
        }
    }
}

void Jkr::Neural::Network::SaveToFile(sv inFileName) {
    std::fstream file(s(inFileName),
                      std::ios_base::binary | std::fstream::out | std::fstream::trunc);
    RW<int>(file, mTopology);
    RW<rowV>(file, mNeuronLayers);
    RW<rowV>(file, mCacheLayers);
    RW<rowV>(file, mDeltas);
    RW<matX>(file, mWeights);
    RW<real>(file, mLearningRate);
}

void Jkr::Neural::Network::LoadFromFile(sv inFileName) {
    std::fstream file(s(inFileName), std::ios_base::binary | std::fstream::in);
    RW<int, Op::Read>(file, mTopology);
    RW<rowV, Op::Read>(file, mNeuronLayers);
    RW<rowV, Op::Read>(file, mCacheLayers);
    RW<rowV, Op::Read>(file, mDeltas);
    RW<matX, Op::Read>(file, mWeights);
    mLearningRate = RW<real, Op::Read>(file, mLearningRate);
}

void Jkr::Neural::Network::CalculateErrors(const rowV& inOutput) {
    ZoneScoped;
    mDeltas.back() = inOutput - mNeuronLayers.back();
    for (uint32_t i = mTopology.size() - 2; i > 0; i--) {
        mDeltas[i] = mDeltas[i + 1] * mWeights[i].transpose();
    }
}

void Jkr::Neural::Network::PropagateForward(const rowV& input) {
    ZoneScoped;
    mNeuronLayers.front().block(0, 0, 1, mNeuronLayers.front().size() - 1) =
         input; // exclude the bias
    for (uint32_t i = 1; i < mTopology.size(); i++) {
        mNeuronLayers[i] = mNeuronLayers[i - 1] * mWeights[i - 1]; // Layer2_out = Matrix * Layer1
        mCacheLayers[i]  = mNeuronLayers[i];
        mNeuronLayers[i].block(0, 0, 1, mTopology[i]) =
             mNeuronLayers[i].block(0, 0, 1, mTopology[i]).unaryExpr(mActivationFunction);
    }
}