/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _MSDIOFIFO_H
#define _MSDIOFIFO_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Idle state, do nothing
#define MSDIO_IDLE          0
/// Start, to start IO operation
#define MSDIO_START         1
/// Wait, waiting for IO operation done
#define MSDIO_WAIT          2
/// Next, to check if the next block can be performed
#define MSDIO_NEXT          3
/// Pause, to pause the process for buffer full or null
#define MSDIO_PAUSE         4
/// Abort, to abort the process
#define MSDIO_ABORT         5
/// Done, finish without error
#define MSDIO_DONE          6
/// Error, any error happens
#define MSDIO_ERROR         7

/// FIFO offset before USB transmit start
//#define MSDIO_FIFO_OFFSET   (4*512)

/// FIFO trunk size (in each transfer, large amount of data)
#if !defined(MSD_OP_BUFFER)
#define MSDIO_READ10_CHUNK_SIZE     (4*512)
#define MSDIO_WRITE10_CHUNK_SIZE    (4*512)
#endif

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

/// FIFO buffer for READ/WRITE (disk) operation of a mass storage device
typedef struct _MSDIOFifo {

    /// Pointer to the ring buffer allocated for read/write
    unsigned char * pBuffer;
    /// The size of the buffer allocated
    unsigned int    bufferSize;
#ifdef MSDIO_FIFO_OFFSET
    /// The offset to start USB transfer (READ10)
    unsigned int    bufferOffset;
#endif
    /// The index of input data (loaded to fifo buffer)
    unsigned int    inputNdx;
    /// The total size of the loaded data
    unsigned int    inputTotal;
    /// The index of output data (sent from the fifo buffer)
    unsigned int    outputNdx;
    /// The total size of the output data
    unsigned int    outputTotal;

    /// The total size of the data
    unsigned int    dataTotal;
    /// The size of the block in bytes
    unsigned short  blockSize;
#if  defined(MSDIO_READ10_CHUNK_SIZE) || defined(MSDIO_WRITE10_CHUNK_SIZE)
    /// The size of one chunk
    /// (1 block, or several blocks for large amount data R/W)
    unsigned int    chunkSize;
#endif
    /// State of input & output
    unsigned char   inputState;
    unsigned char   outputState;

    //- Statistics
    /// Times when fifo has no data to send
    unsigned short  nullCnt;
    /// Times when fifo can not load more input data
    unsigned short  fullCnt;
} MSDIOFifo, *PMSDIOFifo;

//------------------------------------------------------------------------------
//         MACROS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Increase the index, by defined block size, in the ring buffer
/// \param ndx          The index to be increased
/// \param sectSize     The defined block size
/// \param bufSize      The ring buffer size
//------------------------------------------------------------------------------
#define MSDIOFifo_IncNdx(ndx, sectSize, bufSize) \
    if ((ndx) >= (bufSize) - (sectSize)) (ndx) = 0; \
    else (ndx) += (sectSize)


//------------------------------------------------------------------------------
//         Exported Functions
//------------------------------------------------------------------------------


extern void MSDIOFifo_Init(MSDIOFifo *pFifo,
                           void * pBuffer, unsigned short bufferSize);

#endif // _MSDIOFIFO_H

