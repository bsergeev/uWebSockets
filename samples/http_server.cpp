#include "uWS.h"

#include <iostream>
#include <string>

void serveHttp(int port) {
  uWS::Hub h;
  h.onHttpRequest([](uWS::HttpResponse* res, uWS::HttpRequest req, 
                     char* data, size_t length, size_t remainingBytes) {
    static const std::string document = 
      "<!doctype html><html lang=en>"
      "<head><meta charset=utf-8><title>Test uWS HTTP server</title></head>"
      "<body><h2>Hello!</h2><p>This is uWS HTTP server.</p></body></html>";
    res->end(document.data(), document.length());
  });
  h.listen(port);
  std::cout << "Listening on port " << port << "...\nHit Ctrl-C to quit.\n";
  h.run();
}

int main(int argc, char* argv[])
{
  const int port = (argc > 1)? std::atoi(argv[1]) : 3000;
  serveHttp(port);

  return 0;
}
