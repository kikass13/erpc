/*
 * Copyright 2020 (c) Sierra Wireless
 * Copyright 2021 ACRIOS Systems s.r.o.
 * Copyright 2021 DroidDrive GmbH
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_manually_constructed.h"
#include "erpc_fifo_transport.h"
#include "erpc_transport_setup.h"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static ManuallyConstructed<FifoTransport> transportA;
static ManuallyConstructed<FifoTransport> transportB;
static bool i = true;
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_transport_t erpc_transport_fifo_init(std::queue<uint8_t>* receiveBuffer, std::queue<uint8_t>* sendBuffer)
{
    erpc_transport_t transport;
    if(i){
        transportA.construct(receiveBuffer, sendBuffer);
        transport = reinterpret_cast<erpc_transport_t>(transportA.get());
    }
    else{
        transportB.construct(receiveBuffer, sendBuffer);
        transport = reinterpret_cast<erpc_transport_t>(transportB.get());

    }
    i = !i;
    return transport;
}

