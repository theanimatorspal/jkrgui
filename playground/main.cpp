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

#include <Network/Server.hpp>
using namespace Jkr;
int main() {
    Network::ServerInterface Server(60000);
    Network::ServerInterface::OnClientConnectionFunctionType OnClientConnectionFunction =
         [](sp<Network::Connection> client) {
             Network::Message Msg;
             Msg.mHeader.mId = 2;
             client->Send(Msg);
             return true;
         };

    Network::ServerInterface::OnClientValidationFunctionType OnClientValidationFunction =
         [](sp<Network::Connection> client) {

         };
    Server.Start(OnClientValidationFunction, OnClientConnectionFunction);
    Network::ServerInterface::OnMessageFunctionType OnMessage =
         [](sp<Network::Connection> inConnection, Network::Message& msg) {
             switch (msg.mHeader.mId) {
                 case 1:
                     ksai_print(s("[") + std::to_string(inConnection->GetId()) + "]: Server Ping");
                     inConnection->Send(msg);
                     break;
             }
         };
    while (true) {
        Server.Update(OnMessage);
    }
}