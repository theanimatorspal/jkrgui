#include <Neural/ConvolutionalNN.hpp>

using namespace Jkr;

inline Neural::kernelX KernelXByVector(std::vector<int>& inImage)
{
    const int img_size = std::sqrt(inImage.size());
    Neural::kernelX Image(img_size, img_size);
    int d = 0;
    for (int y = 0; y < img_size; y++)
    {
        for (int x = 0; x < img_size; x++)
        {
            Image.coeffRef(x, y) = inImage[d];
            d++;
        }
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

inline Neural::kernelX Conv(Neural::kernelX& inImage, Neural::kernelX& inKernel)
{
    int img_size = inImage.cols();
    int ker_size = inKernel.cols();
    int size = img_size - ker_size + 1;
	Neural::kernelX Output(size, size);
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
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

int main ( ) {
    std::vector<int> img = { 1, 0, -2, 0, -1, 0, 1, 1, 0, 2, 2, 1, 1, 0, 1, -2 };
    std::vector<int> kernel = { 0, 1, 1, 0 };
    auto Image = KernelXByVector(img);
    auto Kernel = KernelXByVector(kernel);
    auto out = Conv(Image, Kernel);
    return 0;
}
