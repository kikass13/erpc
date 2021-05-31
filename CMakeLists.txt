### adopted
### original from https://github.com/ierturk/erpc

cmake_minimum_required(VERSION 3.0.0)
project(erpc)

set(ERPC_INCLUDE_INSTALL_DIR include/erpc)
set(ERPC_LIB_INSTALL_DIR lib)
set(BUILD_TOOLS TRUE)

if(WIN32)
  set(OS "win32")
elseif(UNIX AND NOT APPLE)
  set(OS "linux")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(OS_LIBS pthread rt)
elseif(APPLE)
  # This must come *before* linux or MacOSX will identify as Unix.
  set(OS "macosx")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread pcap)
elseif(FREERTOS)
  set(OS "FreeRTOS")
elseif(AZURERTOS)
  set(OS "AzureRTOS")
endif()

message("We will build eRPC for the OS ${OS}")

set(ERPC_SOURCES
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_arbitrated_client_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_basic_codec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_client_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_crc16.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_framed_transport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_message_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_message_loggers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_simple_server.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_transport_arbitrator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/erpc_pre_post_action.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/port/erpc_port_stdlib.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/port/erpc_threading_pthreads.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/port/erpc_serial.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_arbitrated_client_setup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_client_setup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_setup_mbf_dynamic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_setup_mbf_static.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_server_setup.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_setup_serial.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/erpc_inter_thread_buffer_transport.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/erpc_serial_transport.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/erpc_tcp_transport.cpp
)

if("${OS}" STREQUAL "linux")
  set(ERPC_SOURCES
    ${ERPC_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/port/erpc_threading_pthreads.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/port/erpc_serial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_setup_serial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/erpc_serial_transport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/erpc_tcp_transport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_setup_tcp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_client_setup.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/erpc_setup_fifo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/erpc_fifo_transport.cpp
  )
endif()

file(GLOB ERPC_CONFIG_HEADERS       ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/config/*.h)
file(GLOB ERPC_INFRA_HEADERS        ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/infra/*.h)
file(GLOB ERPC_PORT_HEADERS         ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/port/*.h)
file(GLOB ERPC_SETUP_HEADERS        ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/setup/*.h)
file(GLOB ERPC_TRANSPORTS_HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/erpc_c/transports/*.h)

add_library(erpc STATIC
  ${ERPC_SOURCES})
target_link_libraries(erpc ${OS_LIBS})

target_include_directories(erpc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/erpc_c/config>
  $<INSTALL_INTERFACE:include/erpc>)

target_include_directories(erpc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/erpc_c/infra>
  $<INSTALL_INTERFACE:include/erpc>)

target_include_directories(erpc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/erpc_c/port>
  $<INSTALL_INTERFACE:include/erpc>)

target_include_directories(erpc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/erpc_c/setup>
  $<INSTALL_INTERFACE:include/erpc>)

target_include_directories(erpc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/erpc_c/transports>
  $<INSTALL_INTERFACE:include/erpc>)

message("LIB_DIR: ${ERPC_LIB_INSTALL_DIR}")

install(TARGETS erpc EXPORT erpcConfig DESTINATION ${ERPC_LIB_INSTALL_DIR})
#install(EXPORT erpcConfig DESTINATION ${CMAKE_INSTALL_DATA_DIR}/erpc/cmake)
install(EXPORT erpcConfig DESTINATION ${ERPC_LIB_INSTALL_DIR}/erpc/cmake)

install(FILES
  ${ERPC_CONFIG_HEADERS}
  ${ERPC_INFRA_HEADERS}
  ${ERPC_PORT_HEADERS}
  ${ERPC_SETUP_HEADERS}
  ${ERPC_TRANSPORTS_HEADERS}
  DESTINATION ${ERPC_INCLUDE_INSTALL_DIR})

if(BUILD_TOOLS)
    message("We will build eRPC tools..")
    add_subdirectory(erpcgen)
    add_subdirectory(erpcsniffer)
  else()
    message("We will not build eRPC tools..")
endif()