#include "include.hpp"

template<typename T>
class Tree {
public:
	struct Node {
		T mData;
		u32 mId;
		v<u32> mChildren;
	};
	u32 AddNode(T inData, opt<u32> inParentId = std::nullopt);
	void DeleteNode(u32 inId);
	Node& GetRef(u32 inId);
private:
	u32 AddNode(T inData);
	u32 mCurrentId = 0;
	v<opt<Node>> mNodePool;
};

template<typename T>
inline void Tree<T>::DeleteNode(u32 inId) {
	mNodePool[inId] = {};
}

template<typename T>
inline u32 Tree<T>::AddNode(T inData)
{
	mNodePool.push_back(Node{ inData, mCurrentId });
	return mCurrentId++;
}

template<typename T>
inline u32 Tree<T>::AddNode(T inData, opt<u32> inParentId)
{
	if (inParentId)
	{
		Node& Node = GetRef(inParentId);
		u32 NewNodeIndex = AddNode(inData);
		Node.mChildren.push_back(NewNodeIndex);
		return NewNodeIndex;
	}
	else {
		return AddNode(inData);
	}
}

template<typename T>
inline Tree<T>::Node& Tree<T>::GetRef(u32 inId)
{
	return mNodePool[inId].value();
}