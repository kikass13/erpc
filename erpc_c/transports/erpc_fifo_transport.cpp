/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * Copyright 2021 DroidDrive GmbH
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "erpc_fifo_transport.h"

// #include <iostream>
#include <string>
#include <chrono>
using namespace std::chrono_literals;


using namespace erpc;

// Set this to 1 to enable debug logging.
// TODO fix issue with the transport not working on Linux if debug logging is disabled.
//#define TCP_TRANSPORT_DEBUG_LOG (1)

#if TCP_TRANSPORT_DEBUG_LOG
#define TCP_DEBUG_PRINT(_fmt_, ...) printf(_fmt_, ##__VA_ARGS__)
#define TCP_DEBUG_ERR(_msg_) err(errno, _msg_)
#else
#define TCP_DEBUG_PRINT(_fmt_, ...)
#define TCP_DEBUG_ERR(_msg_)
#endif

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

FifoTransport::FifoTransport(std::queue<uint8_t>* receiveBuffer, std::queue<uint8_t>* sendBuffer)
: receiveBuffer_(receiveBuffer), sendBuffer_(sendBuffer)
{
}

FifoTransport::~FifoTransport(void) {}

erpc_status_t FifoTransport::connectClient(void)
{
    erpc_status_t status = kErpcStatus_Success;
    return status;
}

bool FifoTransport::hasMessage(void){
    return receiveBuffer_->size() > 0;
}


#define TIMEOUT_DURATION 100ms
erpc_status_t FifoTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_ReceiveFailed;
    bool timeout = false;

    /// kikass13:
    /// apparently, this has to block until we receive something, or the cient will just read nothing
    /// the server hasnt even received the request and the client will not wait for an answer inside the erpc framework.
    /// so the response has to be given directly via the streaming process
    // if(receiveBuffer_->size() >= size)
    auto now = std::chrono::high_resolution_clock::now();
    while(receiveBuffer_->size() == 0){
        /// ....
        auto t = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t - now);
        // std::cout << "elapsed ... " << elapsed.count() << std::endl;
        if(elapsed >= TIMEOUT_DURATION){
            timeout = true;
            break;
        }
    }
    
    
    if(timeout){
        status = kErpcStatus_Timeout;
    }
    else{
        // std::cout << "receiveBuffer to data ["<< size <<"]: " << (unsigned long) receiveBuffer_ << "\n";
        for(unsigned int i = 0; i < size; i++){
            uint8_t val = receiveBuffer_->front();
            data[i] = val;
            receiveBuffer_->pop();
            // std::cout << std::hex << (unsigned int) val << std::dec << "  ";
            // if(i > 0 && i % 20 == 0){
            //     std::cout << "\n";
            // }
        }
        // std::cout << "\n";
        status = kErpcStatus_Success;
    }
    return status;
}

erpc_status_t FifoTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    erpc_status_t status = kErpcStatus_SendFailed;
    // std::cout << "data to sendBuffer ["<< size <<"]: " << (unsigned long) sendBuffer_ << "\n";
    for(unsigned int i = 0; i < size; i++){
        uint8_t val = data[i];
        sendBuffer_->push(val);
        // std::cout << std::hex << (unsigned int) val << std::dec << "  ";
        // if(i > 0 && i % 20 == 0){}
        //     std::cout << "\n";
        // }
    }
    // std::cout << "\n";
    status = kErpcStatus_Success;
    return status;
}

