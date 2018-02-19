#include "uWS.h"

#include <iostream>
#include <string>

void testWsConnection(const std::string& url) {
  uWS::Hub h;

  h.onError([](void *user) {
    std::cout << "FAILURE: " << (long)user << " should not emit error!" << std::endl;
  });

  h.onMessage([](uWS::WebSocket<uWS::CLIENT>* ws, char* message, size_t length, uWS::OpCode opCode) {
    std::cout << "Client got a message: <" << ((length > 1)? std::string(message, length-1):"") << ">" << std::endl;
    ws->close(1234);
  });


  h.onConnection([](uWS::WebSocket<uWS::CLIENT>* ws, uWS::HttpRequest req) {
    std::cout << "Client established a remote connection over SSL" << std::endl;
    const char message[] = "Hello WebSockets!";
    ws->send(message, sizeof(message), uWS::OpCode::TEXT);
  });

  h.onDisconnection([](uWS::WebSocket<uWS::CLIENT>* ws, int code, char* message, size_t length) {
    std::cout << "Client got disconnected with data: "<< (int)ws->getUserData() 
              << ", code: " << code << ", message: <" << std::string(message, length) << ">" << std::endl;
  });

  std::cout << "Connecting to \"" << url << "\"\n";
  h.connect(url, (void*)9);

  h.run();
}

int main(int argc, char* argv[])
{
  const std::string url = (argc > 1)? argv[1] : "wss://echo.websocket.org";
  testWsConnection(url);

  return 0;
}

