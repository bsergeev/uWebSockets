cmake_minimum_required(VERSION 3.0)
set(REQUIRED_CPP_VERSION 11)

set(PROJECT_NAME uWS)
project(${PROJECT_NAME} CXX)

# gather the files
set(Backend_headers
  "${PROJECT_SOURCE_DIR}/src/Backend.h"
  "${PROJECT_SOURCE_DIR}/src/Asio.h"
  "${PROJECT_SOURCE_DIR}/src/Epoll.h"
  "${PROJECT_SOURCE_DIR}/src/Libuv.h"
)
set(uWS_headers
  "${PROJECT_SOURCE_DIR}/src/Extensions.h"
  "${PROJECT_SOURCE_DIR}/src/Group.h"
  "${PROJECT_SOURCE_DIR}/src/HTTPSocket.h"
  "${PROJECT_SOURCE_DIR}/src/Hub.h"
  "${PROJECT_SOURCE_DIR}/src/Networking.h"
  "${PROJECT_SOURCE_DIR}/src/Node.h"
  "${PROJECT_SOURCE_DIR}/src/Socket.h"
  "${PROJECT_SOURCE_DIR}/src/uWS.h"
  "${PROJECT_SOURCE_DIR}/src/WebSocket.h"
  "${PROJECT_SOURCE_DIR}/src/WebSocketProtocol.h"
)
set(Header_Files
  ${Backend_headers}
  ${uWS_headers}
)
FILE(GLOB Source_Files "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${Source_Files} ${Header_Files})

include_directories("src")
# target_include_directories(${PROJECT_NAME} PRIVATE "${ASIO_INCLUDE_DIR}")

# Unfortunately, I haven't figured out how to wrap ASIO Standalone to correctly
# define uv_os_sock_t (and probably Poll class too). Therefore, removing 
# ASIO_STANDALONE and using Boost ASIO
# target_compile_definitions(${PROJECT_NAME} PRIVATE USE_ASIO ASIO_STANDALONE=1) 

# find_package(OpenSSL REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_executable(HttpServer samples/http_server.cpp)
target_link_libraries(HttpServer LINK_PUBLIC ${PROJECT_NAME})

add_executable(HttpServerSentEvents samples/http_server_sent_events.cpp)
target_link_libraries(HttpServerSentEvents LINK_PUBLIC ${PROJECT_NAME})

add_executable(WsClient samples/ws_client.cpp)
target_link_libraries(WsClient LINK_PUBLIC ${PROJECT_NAME})

if (MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_target_properties(HttpServer           PROPERTIES FOLDER Samples)
  set_target_properties(HttpServerSentEvents PROPERTIES FOLDER Samples)
  set_target_properties(WsClient             PROPERTIES FOLDER Samples)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT WsClient)

  # Project filters (to group files in IDE project)
  source_group("Header Files\\Backends" FILES ${Backend_headers})
  source_group("Source Files\\Backends" FILES "${PROJECT_SOURCE_DIR}/src/Epoll.cpp")
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD ${REQUIRED_CPP_VERSION})
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
elseif(UNIX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${REQUIRED_CPP_VERSION} -stdlib=libc++")
    target_link_libraries(${PROJECT_NAME} c++ c++abi pthread)
endif()
