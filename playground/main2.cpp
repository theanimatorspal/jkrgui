#include <Network/Client.hpp>

using namespace Jkr;
int main() {
    Network::ClientInterface Client;
    Network::Connection::OnClientValidationFunctionType OnClientValidationFunction =
         [](sp<Network::Connection>) {};
    Client.Connect(OnClientValidationFunction, "192.168.56.1", 60000);
    auto PingServer = [&]() {
        Network::Message Msg;
        Msg.mHeader.mId = 1;
        auto timeNow    = std::chrono::system_clock::now();
        Msg << timeNow;
        Client.Send(Msg);
    };

    bool Quit = false;
    int i     = 1;
    while (not Quit) {
        if (i % 10000000 == 0) {
            PingServer();
        }

        if (Client.IsConnected()) {
            if (not Client.GetIncomingQMessages().empty()) {
                auto Msg = Client.GetIncomingQMessages().pop_front();
                switch (Msg.mHeader.mId) {
                    case 1:
                        auto timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        Msg >> timeThen;
                        ksai_print(s("Time Duration:") +
                                   std::to_string(
                                        std::chrono::duration<double>(timeNow - timeThen).count()));
                        break;
                }
            }

        } else {
            ksai_print("Server Down");
            Quit = true;
        }

        i++;
    }
    return 0;
}