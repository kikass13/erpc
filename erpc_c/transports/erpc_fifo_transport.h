/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * Copyright 2021 DroidDrive GmbH
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _EMBEDDED_RPC__FIFO_TRANSPORT_H_
#define _EMBEDDED_RPC__FIFO_TRANSPORT_H_

#include "erpc_framed_transport.h"
// #include "erpc_threading.h"

#include <queue>
#include <cstddef>
#include <cassert>

/*!
 * @addtogroup fifo_transport
 * @{
 * @file
 */

////////////////////////////////////////////////////////////////////////////////
// Classes
////////////////////////////////////////////////////////////////////////////////

namespace erpc {



/// kikass13:
/// Is a CRTP pattern a better solution here?
class Buffer{
public:
    virtual size_t capacity() const = 0;
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;    
    virtual void clear() = 0;
    virtual void push(const uint8_t& item) = 0;
    virtual const uint8_t& front() = 0;
    virtual void pop() = 0;

};

template<class T, int N>
class RingBuffer : public Buffer{
public:
    RingBuffer() { clear(); }
    size_t capacity() const { return N; }
    bool empty() const { return head_ == tail_; }
    size_t size() const {
        return head_ >= tail_ ?
            head_ - tail_ :
            BUFSIZE - (tail_ - head_);
    }
  void push(const T& item) {
        buffer_[head_] = item;
        advance(head_);
        if (head_ == tail_) {
            advance(tail_); // Drop oldest entry, keep rest.
        }
    }
    void pop() {
        advance(tail_);
    }
    const T& front() {
        assert(!empty());
        //size_t old_tail = tail_;
        //advance(tail_);
        //return buffer_[old_tail];
        return buffer_[tail_];
    }
    void clear() { tail_ = head_ = 0U; }
 
private:
    /// well mutexes are slow ... lets try it without ;D
    //std::mutex dataMutex_;
    static const size_t BUFSIZE = N + 1U;
    void advance(size_t& value) { value = (value + 1) % BUFSIZE; }
 
    T buffer_[BUFSIZE];
    size_t head_;
    size_t tail_;
};
 

/*!
 * @brief fifo transport.
 *
 * @ingroup fifo_transport
 */
class FifoTransport : public FramedTransport
{
public:
    /*!
     * @brief Constructor.
     *
     * This function initializes object attributes.
     *
     * @param[in] buffer 
     */
    FifoTransport(Buffer* receiveBuffer, Buffer* sendBuffer);

    /*!
     * @brief FifoTransport destructor
     */
    virtual ~FifoTransport(void);

protected:
    Buffer* receiveBuffer_;
    Buffer* sendBuffer_;

    /*!
     * @brief This function connect client to the server.
     *
     * @retval kErpcStatus_Success When client connected successfully.
     * @retval kErpcStatus_Fail When client doesn't connected successfully.
     */
    virtual erpc_status_t connectClient(void);

    /*!
     * @brief This function read data.
     *
     * @param[inout] data Preallocated buffer for receiving data.
     * @param[in] size Size of data to read.
     *
     * @retval #kErpcStatus_Success When data was read successfully.
     * @retval #kErpcStatus_ReceiveFailed When reading data ends with error.
     * @retval #kErpcStatus_ConnectionClosed Peer closed the connection.
     */

    virtual bool hasMessage(void) override;

    virtual erpc_status_t underlyingReceive(uint8_t *data, uint32_t size);

    /*!
     * @brief This function writes data.
     *
     * @param[in] data Buffer to send.
     * @param[in] size Size of data to send.
     *
     * @retval #kErpcStatus_Success When data was written successfully.
     * @retval #kErpcStatus_SendFailed When writing data ends with error.
     * @retval #kErpcStatus_ConnectionClosed Peer closed the connection.
     */
    virtual erpc_status_t underlyingSend(const uint8_t *data, uint32_t size);
};

} // namespace erpc

/*! @} */

#endif // _EMBEDDED_RPC__FIFO_TRANSPORT_H_
