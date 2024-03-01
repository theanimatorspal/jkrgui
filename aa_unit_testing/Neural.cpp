#include <Neural/ConvolutionalNN.hpp>
using namespace Jkr;
inline Neural::kernelX KernelXByVector(std::vector<int>& inImage, int inPaddingLayers = 0)
{
	const int img_size = std::sqrt(inImage.size());
	Neural::kernelX Image(img_size + inPaddingLayers * 2, img_size + inPaddingLayers * 2);
	Image.setZero();
	int d = 0;
	for (int y = inPaddingLayers; y < img_size; y++)
	{
		for (int x = inPaddingLayers; x < img_size; x++)
		{
			Image.coeffRef(x, y) = inImage[d];
			d++;
		}
	}

	std::cout << Image.cols() << Image.rows() << "\n";
	std::cout << Image << "\n";
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
inline Neural::kernelX Correlate2d(Neural::kernelX& inImage, Neural::kernelX& inKernel, int inStride = 1)
{
	int img_size = inImage.cols();
	int ker_size = inKernel.cols();
	//int size = 1 + (img_size + 2 * inPaddingLayers - ker_size) / inStride;
	// no need here as the img_size mai aa xa
	int size = 1 + (img_size - ker_size) / inStride;
	Neural::kernelX Output(size, size);
	for (int y = 0; y < size; y += inStride)
	{
		for (int x = 0; x < size; x += inStride)
		{
			auto img_area = inImage.block(x, y, ker_size, ker_size);
			Output.coeffRef(x, y) = SumProd(img_area, inKernel);
		}
	}
	std::cout << Output << "\n";
	return Output;
}

inline Neural::kernelX MaxPool(Neural::kernelX& inImage, int inFilterSize, int inStride = 1)
{
	int img_size = inImage.cols();
	int ker_size = inFilterSize;
	//int size = 1 + (img_size + 2 * inPaddingLayers - ker_size) / inStride;
	// no need here as the img_size mai aa xa
	int size = 1 + (img_size - ker_size) / inStride;
	Neural::kernelX Output(size, size);
	for (int y = 0; y < size; y += inStride)
	{
		for (int x = 0; x < size; x += inStride)
		{
			auto img_area = inImage.block(x, y, ker_size, ker_size);
			Output.coeffRef(x, y) = img_area.maxCoeff();
		}
	}
	std::cout << Output << "\n";
	return Output;
}

int main() {
	std::vector<int> img = { 1, 0, -2, 0, -1, 0, 1, 1, 0, 2, 2, 1, 1, 0, 1, -2 };
	std::vector<int> kernel = { 0, 1, 1, 0 };
	auto Image = KernelXByVector(img, 1);
	std::cout << "\n";
	auto Kernel = KernelXByVector(kernel);
	std::cout << "\n";
	auto out = Correlate2d(Image, Kernel, 1);
	auto maxPooled = MaxPool(out, 3);
	Neural::Network NewNeu(std::vector<int>{4, 3, 2, 1});
	return 0;
}
