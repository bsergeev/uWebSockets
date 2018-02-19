#include "uWS.h"

#include <iostream>
#include <string>

void serveEventSource(int port) {
  uWS::Hub hub;


  // stop and delete the timer on http disconnection
  hub.onHttpDisconnection([](uWS::HttpSocket<uWS::SERVER> *s) {
    if (auto timer = (uS::Timer*) s->getUserData()) {
      timer->stop();
      timer->close();
    }
  });

  // terminate any upgrade attempt, this is http only
  hub.onHttpUpgrade([](uWS::HttpSocket<uWS::SERVER>* s, uWS::HttpRequest) {
    s->terminate();
  });

  hub.onHttpRequest([&hub](uWS::HttpResponse* res, uWS::HttpRequest req, char*, size_t, size_t) {
    static const std::string EVENT_ROUT = "eventSource";
    const std::string url = req.getUrl().toString();
    if (url == "/") {
      // The script sets the page to receive Server Sent Events:
      //  https://developer.mozilla.org/en-US/docs/Web/API/EventSource
      static const std::string doc =
        "<!doctype html><html lang=en>"
        "<head>"
          "<meta charset=utf-8><title>uWS Server Sent Events</title></head>"
        "<body>"
          "<h2>The server sends an event once a second:</h2>"
          "<div id='log'></div>"
          "<script>"
            "var e=new EventSource('"+ EVENT_ROUT +"');"
            "e.onmessage=function(message){"
              "var log=document.getElementById('log');"
              "log.innerHTML+='<p><b>Received:</b> '+ message.data +'</p>';"
            "};"
          "</script>";
        "</body></html>";

      res->end((char*)doc.data(), doc.length());
      return;
    } else if (url == "/"+ EVENT_ROUT) {
      if (!res->getUserData()) {
        // Establish a 'text/event-stream' connection where we can send messages server -> client at any time
        static const std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\n\r\n";
        res->write((char*)header.data(), header.length());

        // Create and attach a timer to the socket and let it send messages to the client once a second
        uS::Timer *timer = new uS::Timer(hub.getLoop());
        timer->setData(res);
        timer->start([](uS::Timer* timer) {
          static int n = 0;
          std::string message = "data: server sent event "+ std::to_string(++n) +"\n\n";
          if (auto response = (uWS::HttpResponse*)timer->getData()) {
            response->write((char*)message.data(), message.length());
          }
        }, 0, 1000);
        res->setUserData(timer);
      } else { 
        std::cerr << "Why would the client send a new request at this point?\n";
        res->getHttpSocket()->terminate();
      }
    } else if (url != "/favicon.ico") {
      std::cerr << "Wrong endpoint:\"" << url <<"\", quitting...\n";
      res->getHttpSocket()->terminate();
    }
  });

  hub.listen(port);
  std::cout << "Open \"localhost:" << port << "\" in your browser\nWhen you close the browser, the server will quit.\n";
  hub.run();
}

int main(int argc, char* argv[])
{
  const int port = (argc > 1)? std::atoi(argv[1]) : 3000;
  serveEventSource(port);

  return 0;
}

