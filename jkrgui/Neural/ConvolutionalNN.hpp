#include "SimpleNN.hpp"

namespace Jkr::Neural { class CNetwork; }

class Jkr::Neural::CNetwork : public Jkr::Neural::Network {
	CNetwork(std::vector<int> inTopology, real inLearningRate);
	~CNetwork() = default;
	CNetwork(const CNetwork& inNetwork) = default;
	CNetwork& operator=(const CNetwork& inNetwork) = default;
};
