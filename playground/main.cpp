// #include <iostream>
// #include <chrono>
// #define ASIO_STANDALONE
// #include <asio.hpp>
// #include <asio/ts/buffer.hpp>   // movment of memory
// #include <asio/ts/internet.hpp> // network communication

// std::vector<char> vBuffer(20 * 1024);
// void GrabSomeData(asio::ip::tcp::socket& inSocket) {
//     inSocket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
//                              [&](std::error_code ec, std::size_t length) {
//                                  if (not ec) {
//                                      std::cout << "\n\nRead" << length << " bytes\n\n";
//                                      for (int i = 0; i < length; ++i) {
//                                          std::cout << vBuffer[i];
//                                      }
//                                      GrabSomeData(inSocket);
//                                  }
//                              });
// }

// int main() {
//     asio::error_code ec;
//     asio::io_context context; // platform specific interface
//     asio::io_context::work idleWork(context);
//     std::thread thrContext = std::thread([&]() { context.run(); });
//     asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

//     // Create a socket, the context will deliver the implementation
//     asio::ip::tcp::socket socket(context);

//     socket.connect(endpoint, ec);

//     if (not ec) {
//         std::cout << "Connected\n";
//     } else {
//         std::cout << "Failed to Connect\n";
//         std::cout << ec.message() << "\n";
//     }

//     if (socket.is_open()) {
//         GrabSomeData(socket);

//         std::string sRequest = R"""(
// GET /index.html HTTP/1.1
// HOST: david-barr.co.uk
// Connection: close

//             )""";

//         socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);
//         using namespace std::chrono_literals;
//         std::this_thread::sleep_for(20000ms);

//         context.stop();
//         if (thrContext.joinable()) thrContext.join();
//     }
//     return 0;
// }

// #include <Network/Server.hpp>
// using namespace Jkr;
// int main() {
//     using namespace Network;
//     Network::ServerInterface Server(60000);
//     OnClientConnectionFunctionType OnClientConnectionFunction = [](sp<Network::Connection>
//     client) {
//         Network::Message Msg;
//         Msg.mHeader.mId = 2;
//         client->Send(Msg);
//         return true;
//     };

//     OnClientValidationFunctionType OnClientValidationFunction = [](sp<Network::Connection>
//     client) {

//     };
//     Server.Start(OnClientValidationFunction, OnClientConnectionFunction);
//     OnMessageFunctionType OnMessage = [](sp<Network::Connection> inConnection,
//                                          Network::Message& msg) {
//         switch (msg.mHeader.mId) {
//             case 1:
//                 ksai_print(s("[") + std::to_string(inConnection->GetId()) + "]: Server Ping");
//                 inConnection->Send(msg);
//                 break;
//         }
//     };
//     while (true) {
//         Server.Update(OnMessage);
//     }
// }

// #include <iostream>
// #include <algorithm>
// #include <vector>
// #include <map>
// #include <unordered_map>
// #include <optional>

// using sv                                     = std::string_view;
// template <typename T> using v                = std::vector<T>;
// template <typename T, typename U> using map  = std::map<T, U>;
// template <typename T, typename U> using umap = std::unordered_map<T, U>;
// template <typename T, typename U> using p    = std::pair<T, U>;
// template <typename T> using u                = std::unique_ptr<T>;
// template <typename T> using opt              = std::optional<T>;
// using u32                                    = uint32_t;
// #define mu std::make_unique
// #define mv std::move
// #define mksh std::make_shared

// class CryptoSolver {
//     using ValueMapType = umap<char, int>;

//     public:
//     void SetMap(ValueMapType inValue);
//     void SolveByBruteForce();
//     void RegisterInput(sv inString);
//     void RegisterOutput(sv inOutput);

//     bool HasSolved();
//     void operator*();

//     private:
//     void SolveByBruteForceRecursive(auto inItr);
//     p<int, int> SumOfDigitsOfInputs(int inIndexFromRight = 0);
//     ValueMapType mValueMap;
//     v<sv> mInputs;
//     sv mOutput;
// };

// int main() {
//     CryptoSolver Solver;
//     Solver.RegisterInput("0handle");
//     Solver.RegisterInput("nothing");
//     Solver.RegisterOutput("exception");
//     Solver.SolveByBruteForce();
// }

// inline void CryptoSolver::RegisterInput(sv inString) {
//     mInputs.push_back(inString);
//     try {
//         for (char c : inString) {
//             if (not isalpha(c)) {
//                 throw std::runtime_error("Exception: Attempt to use non-alphabetical character");
//             }
//             if (not mValueMap[c]) {
//                 mValueMap[c] = 0;
//             }
//         }
//     } catch (const std::exception& e) {
//         std::cerr << e.what() << "\n";
//         exit(-1);
//     }
// }

// inline void CryptoSolver::RegisterOutput(sv inOutput) {
//     mOutput = inOutput;
//     for (char c : inOutput) {
//         if (not mValueMap[c]) {
//             mValueMap[c] = 0;
//         }
//     }
// }

// inline bool CryptoSolver::HasSolved() {
//     bool hasSolved = false;
//     static v<int> Carrys;
//     int OutputSize = (int)mOutput.size();
//     Carrys.resize(mOutput.size() - 1, 0);
//     for (int i = 0, j = -1; i < OutputSize; i++, j++) {
//         auto digitsSumCarry = SumOfDigitsOfInputs(i);
//         int sum             = digitsSumCarry.second;
//         int carry           = digitsSumCarry.first;
//         int total           = sum;
//         if (j >= 0) total += Carrys[j];
//         carry += total / 10;
//         if (j < (int)Carrys.size() - 1) {
//             Carrys[j + 1] = carry;
//         }
//         char outputChar = mOutput[mOutput.size() - i - 1];
//         int outputValue = mValueMap[outputChar];
//         if (total == outputValue) {
//             hasSolved = true;
//         } else {
//             hasSolved = false;
//             break;
//         }
//     }
//     bool isunique = not std::any_of(mValueMap.begin(), mValueMap.end(), [&](auto& x) {
//         return std::any_of(mValueMap.begin(), mValueMap.end(), [=](auto& y) {
//             return (x != y) and x.second == y.second;
//         });
//     });
//     return hasSolved and mValueMap[mOutput.front()] != 0 and isunique;
// }

// inline void CryptoSolver::SetMap(ValueMapType inValue) { mValueMap = inValue; }

// inline void CryptoSolver::SolveByBruteForceRecursive(auto inItr) {
//     for (auto y = inItr; y != mValueMap.end(); y++) {
//         for (int dd = 0; dd < 10; dd++) {
//             y->second = dd;
//             SolveByBruteForceRecursive(std::next(y));
//             if (HasSolved()) {
//                 break;
//             }
//         }
//     }
// }

// inline void CryptoSolver::SolveByBruteForce() { SolveByBruteForceRecursive(mValueMap.begin()); }

// inline void CryptoSolver::operator*() {
//     for (auto& p : mValueMap) {
//         std::cout << "'" << p.first << "' :" << p.second << '\n';
//     }

//     for (auto& input : mInputs) {
//         for (int i = 0; i < mOutput.size() - input.size(); i++) {
//             std::cout << "\t";
//         }
//         for (char c : input) {
//             std::cout << c << "=" << mValueMap[c] << "\t";
//         }
//         std::cout << '\n';
//     }

//     std::cout <<
//     "---------------------------------------------------------------------------------"
//                  "-------------\n";
//     for (char c : mOutput) {
//         std::cout << c << "=" << mValueMap[c] << "\t";
//     }
//     std::cout << "\n";
// }

// inline p<int, int> CryptoSolver::SumOfDigitsOfInputs(int inIndexFromRight) {
//     int sum = 0;
//     for (sv v : mInputs) {
//         int index = v.size() - inIndexFromRight - 1;
//         if (0 <= index) {
//             char character = v[index];
//             int Value      = mValueMap[character];
//             sum += Value;
//         }
//     }
//     return p<int, int>(sum / 10, sum % 10);
// }

#include <string>
#include <iostream>
using namespace std;

struct State {
    State* operator()(char inChar) { return mMap[inChar]; }
    std::unordered_map<char, State*> mMap;
};

struct Machine {
    Machine(State& inState, const string& inArray) {
        Q.resize(inArray.size());
        for (int i = 0; i < inArray.length() - 1; ++i) {
            Q[i].mMap[inArray[i + 1]] = &Q[i + 1];
            Q[i].mMap[inArray[i]]     = &Q[i];
        }
        mcQ                        = &inState;
        mStartQ                    = &inState;
        mcQ->mMap[inArray.front()] = &Q.front();
    }
    bool Eat(char inC) {
        if (mcQ->mMap[inC]) {
            mcQ = mcQ->mMap[inC];
        } else {
            mcQ = mStartQ;
        }
        return mcQ == &Q.back();
    }
    std::vector<State> Q;
    State* mcQ     = nullptr;
    State* mStartQ = nullptr;
};

class Solution {
    public:
    int strStr(string haystack, string needle) {
        State State;
        Machine FSM(State, needle);
        int match         = 0;
        int needle_length = needle.length();
        for (int i = 0; i < haystack.length(); i++) {
            if (FSM.Eat(haystack[i])) return i - needle_length + 1;
        }
        return -1;
    }
};

int main() {

    Solution soln;

    std::cout << soln.strStr("mississipi", "issip") << "\n";
}
