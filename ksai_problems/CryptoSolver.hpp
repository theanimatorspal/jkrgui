#pragma once
#include "include.hpp"


class CryptoSolver {
	using ValueMapType = umap<char, int>;
public:
	void SetMap(ValueMapType inValue);
	void SolveByBruteForce();
	void RegisterInput(sv inString);
	void RegisterOutput(sv inOutput);

	bool HasSolved();
	void operator*();
private:
	void SolveByBruteForceRecursive(auto inItr);
	p<int, int> SumOfDigitsOfInputs(int inIndexFromRight = 0);
	ValueMapType mValueMap;
	v<sv> mInputs;
	sv mOutput;
};

inline void CryptoSolver::RegisterInput(sv inString)
{
	mInputs.push_back(inString);
	for (char c : inString)
	{
		if (not mValueMap[c])
		{
			mValueMap[c] = 0;
		}
	}
}

inline void CryptoSolver::RegisterOutput(sv inOutput)
{
	mOutput = inOutput;
	for (char c : inOutput)
	{
		if (not mValueMap[c])
		{
			mValueMap[c] = 0;
		}
	}
}

inline bool CryptoSolver::HasSolved()
{
	bool hasSolved = false;
	static v<int> Carrys;
	int OutputSize = (int)mOutput.size();
	Carrys.resize(mOutput.size() - 1, 0);
	for (int i = 0, j = -1; i < OutputSize; i++, j++)
	{
		auto digitsSumCarry = SumOfDigitsOfInputs(i);
		int sum = digitsSumCarry.second;
		int carry = digitsSumCarry.first;
		int total = sum;
		if (j >= 0) total += Carrys[j];
		carry += total / 10;
		if (j < (int)Carrys.size() - 1) {
			Carrys[j + 1] = carry;
		}
		char outputChar = mOutput[mOutput.size() - i - 1];
		int outputValue = mValueMap[outputChar];
		if (total == outputValue) {
			hasSolved = true;
		}
		else {
			hasSolved = false;
			break;
		}
	}
	bool isunique = not std::any_of(mValueMap.begin(), mValueMap.end(), [&](auto& x) {
		return std::any_of(mValueMap.begin(), mValueMap.end(), [=](auto& y) {
			return (x != y) and x.second == y.second;
			});
		});
	return hasSolved and mValueMap[mOutput.front()] != 0 and isunique;
}

inline void CryptoSolver::SetMap(ValueMapType inValue)
{
	mValueMap = inValue;
}

inline void CryptoSolver::SolveByBruteForceRecursive(auto inItr)
{
	for (auto y = inItr; y != mValueMap.end(); y++) {
		for (int dd = 0; dd < 10; dd++)
		{
			y->second = dd;
			SolveByBruteForceRecursive(std::next(y));
			if (HasSolved()) {
				break;
			}
		}
	}
}

inline void CryptoSolver::SolveByBruteForce()
{
	SolveByBruteForceRecursive(mValueMap.begin());
}

inline void CryptoSolver::operator*()
{
	for (auto& p : mValueMap)
	{
		std::cout << "'" << p.first << "' :" << p.second << '\n';
	}

	for (auto& input : mInputs )
	{
		for (int i = 0; i < mOutput.size() - input.size(); i++)
		{
			std::cout << "\t";
		}
		for (char c : input)
		{
			std::cout << c << "=" << mValueMap[c] << "\t";
		}
		std::cout << '\n';
	}

	std::cout << "----------------------------------------------------------------------------------------------\n";
	for (char c : mOutput)
	{
			std::cout << c << "=" << mValueMap[c] << "\t";
	}
	std::cout << "\n";
}


inline p<int, int> CryptoSolver::SumOfDigitsOfInputs(int inIndexFromRight)
{
	int sum = 0;
	for (sv v : mInputs)
	{
		int index = v.size() - inIndexFromRight - 1;
		if (0 <= index)
		{
			char character = v[index];
			int Value = mValueMap[character];
			sum += Value;
		}
	}
	return p<int, int>(sum / 10, sum % 10);
}


