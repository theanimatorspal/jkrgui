#pragma once
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <variant>
#include <vector>

using DataType = std::variant<std::string_view, float, int, uint32_t, glm::vec2, glm::uvec2, glm::uvec3, glm::vec3, glm::vec4, std::vector<uint32_t>, std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<int>>;
using CookFunctionType = std::function<DataType(std::vector<DataType> inInput)>;
struct Input {
    std::string_view mInputName;
    DataType mV;
};

namespace App {

class Node {
private:
    DataType mOutputData;
    bool mCooked = false;

public:
    explicit Node(CookFunctionType&& inFunction, uint32_t mNoOfInputs)
        : mCookFunction(std::forward<CookFunctionType>(inFunction))
    {
        mPrevious.resize(mNoOfInputs, nullptr);
    }

    explicit Node(DataType inData)
    {
        mOutputData = inData;
        mCooked = true;
    }

    explicit Node (Node &inNode)
        : mCookFunction(inNode.mCookFunction)
    {
        mPrevious.resize(inNode.mPrevious.size(), nullptr);
    }

    auto Cook() -> DataType
    {
        if (not mCooked) {
            std::vector<DataType> InputData;

            bool HasAllTheNodeCooked = true;
            for (int i = 0; i < mPrevious.size(); i++) {
                auto& NodeIsOkay = mPrevious[i];
                if (not NodeIsOkay) {
                    continue;
                } else if (not NodeIsOkay->mCooked) {
                    HasAllTheNodeCooked = false;
                    break;
                }
                InputData.push_back(NodeIsOkay->mOutputData);
            }

            if (HasAllTheNodeCooked) {
                this->mOutputData = this->mCookFunction(InputData);
                for (int i = 0; i < mNext.size(); i++) {
                    auto& NodeIsOkay = mNext[i];
                    if (NodeIsOkay) {
                        NodeIsOkay->Cook();
                    }
                }
                mCooked = true;
            }
        }
        return this->mOutputData;
    }

public:
    static void ConnectNode(Node* inInputNode, Node* inOutputNode, uint32_t inSlot)
    {
        inInputNode->mNext.push_back(inOutputNode);
        inOutputNode->mPrevious[inSlot] = inInputNode;
    }

    static void DisconnectNode(Node* inInputNode, Node* inOutputNode)
    {
        auto I = std::find(inInputNode->mPrevious.begin(), inInputNode->mPrevious.end(), inOutputNode);
        *I = nullptr;
        auto Remove = std::remove(inOutputNode->mNext.begin(), inOutputNode->mNext.end(), inInputNode);
        inOutputNode->mNext.erase(Remove);
    }

    static auto GetOutput(Node* inNode)
    {
        return inNode->mOutputData;
    }

private:
    std::vector<Node*> mPrevious;
    std::vector<Node*> mNext;

    CookFunctionType mCookFunction = [](std::vector<DataType> inType) {
        return DataType(0);
    };
};

}