#include "ConvolutionalNN.hpp"

Jkr::Neural::CNetwork::CNetwork(std::vector<int> inTopology, real inLearningRate)
	: Neural::Network(inTopology, inLearningRate)
{
	constexpr real alpha = 0.001;
	mActivationFunction = [](real x) {
		return std::max(alpha * x, x);
		};

	mActivationFunctionDerivative = [](real x) {
		return  x >= 0 ? 1 : alpha;
		};
}

using namespace Jkr;
inline Neural::kernelX KernelXByVector(std::vector<int>& inImage, int inPaddingLayers = 0)
{
	const int img_size = std::sqrt(inImage.size());
	Neural::kernelX Image(img_size + inPaddingLayers, img_size + inPaddingLayers);
	Image.setZero();
	int d = 0;
	for (int y = inPaddingLayers; y < img_size; y++)
	{
		for (int x = inPaddingLayers; x < img_size; x++)
		{
			Image.coeffRef(x, y) = inImage[d];
			std::cout << Image.coeff(x, y) << "\t";
			d++;
		}
		std::cout << "\n";
	}
	return Image;
}

inline auto SumProd(auto& inA, auto& inB)
{
	int sum = 0;
	for (int y = 0; y < inA.cols(); y++)
	{
		for (int x = 0; x < inA.cols(); x++)
		{
			sum += inA.coeff(x, y) * inB.coeff(x, y);
		}
	}
	return sum;
}

// IG it is correlate
inline Neural::kernelX Correlate2d(Neural::kernelX& inImage, Neural::kernelX& inKernel, int inStride = 1, int inPaddingLayers = 0)
{
	int img_size = inImage.cols();
	int ker_size = inKernel.cols();
	int size = 1 + (img_size + 2 * inPaddingLayers - ker_size) / inStride;
	Neural::kernelX Output(size, size);
	for (int y = 0; y < size; y += inStride)
	{
		for (int x = 0; x < size; x += inStride)
		{
			int i = 0;
			auto img_area = inImage.block(x, y, ker_size, ker_size);
			Output.coeffRef(x, y) = SumProd(img_area, inKernel);
			std::cout << Output.coeff(x, y) << "\t";
		}
		std::cout << "\n";
	}
	return Output;
}
