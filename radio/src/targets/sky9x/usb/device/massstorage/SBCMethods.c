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

//------------------------------------------------------------------------------
//      Headers
//------------------------------------------------------------------------------

#include "../../../usb/device/massstorage/SBCMethods.h"

#include "../../../usb/device/core/USBD.h"
#include "../../../usb/device/massstorage/MSDDStateMachine.h"
#include "../../../usb/device/massstorage/MSDIOFifo.h"

//------------------------------------------------------------------------------
//      Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Macros
//------------------------------------------------------------------------------

#ifdef MSDIO_READ10_CHUNK_SIZE
/// READ10 - Read data from specific LUN to FIFO
#define SBC_READ_CHUNK(pLun, lba, pFifo, pCb, pArg) \
    LUN_Read((pLun), (lba), \
            &(pFifo)->pBuffer[(pFifo)->inputNdx], \
             ((pFifo)->chunkSize/(pFifo)->blockSize), \
             (TransferCallback)(pCb), (void*)pArg)
/// READ10 - Transfer data from FIFO to USB
#define SBC_TX_CHUNK(pFifo, pCb, pArg) \
    MSDD_Write(&(pFifo)->pBuffer[(pFifo)->outputNdx], \
                (pFifo)->chunkSize, \
                (TransferCallback)(pCb), (void*)(pArg))
#endif

#ifdef MSDIO_WRITE10_CHUNK_SIZE
/// WRITE10 - Read data from USB to FIFO
#define SBC_RX_CHUNK(pFifo,pCb,pArg) \
    MSDD_Read(&(pFifo)->pBuffer[(pFifo)->inputNdx], \
               (pFifo)->chunkSize, \
               (TransferCallback)(pCb), (void*)(pArg))
/// WRITE10 - Write data from FIFO to LUN
#define SBC_WRITE_CHUNK(pLun, lba, pFifo, pCb, pArg) \
    LUN_Write((pLun), (lba), \
             &(pFifo)->pBuffer[(pFifo)->outputNdx], \
              ((pFifo)->chunkSize/(pFifo)->blockSize), \
              (TransferCallback)(pCb), (void*)(pArg))
#endif


//------------------------------------------------------------------------------
//! \brief  Header for the mode pages data
//! \see    SBCModeParameterHeader6
//------------------------------------------------------------------------------
static const SBCModeParameterHeader6 modeParameterHeader6 = {

    sizeof(SBCModeParameterHeader6) - 1,        //! Length is 0x03
    SBC_MEDIUM_TYPE_DIRECT_ACCESS_BLOCK_DEVICE, //! Direct-access block device
    0,                                          //! Reserved bits
    0,                                          //! DPO/FUA not supported
    0,                                          //! Reserved bits
    0,                                          //! not write-protected
    0                                           //! No block descriptor
};

//------------------------------------------------------------------------------
//      Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief  Check if the LUN is ready.
//! \param  lun          Pointer to the LUN affected by the command
//! \return 1 if the LUN is ready to be written
//! \see    MSDLun
//------------------------------------------------------------------------------
static unsigned char SBCLunIsReady(MSDLun *lun)
{
    unsigned char lunIsReady = 0;
    Media *media = lun->media;

    if (media == 0 || lun->status < LUN_CHANGED) {
        TRACE_INFO("SBCLunIsReady: Not Present!\n\r");
        SBC_UpdateSenseData(&(lun->requestSenseData),
                            SBC_SENSE_KEY_NOT_READY,
                            SBC_ASC_MEDIUM_NOT_PRESENT,
                            0);

    }
    else if (lun->status < LUN_READY) {
        TRACE_INFO("SBCLunIsReady: Changing!\n\r");
        SBC_UpdateSenseData(&(lun->requestSenseData),
                            SBC_SENSE_KEY_UNIT_ATTENTION,
                            SBC_ASC_NOT_READY_TO_READY_CHANGE,
                            0);
        lun->status = LUN_READY;
    }
    else {

        lunIsReady = 1;
    }

    return lunIsReady;
}
//------------------------------------------------------------------------------
//! \brief  Check if the LUN can write.
//! \param  lun          Pointer to the LUN affected by the command
//! \return 1 if the LUN is ready to be written
//! \see    MSDLun
//------------------------------------------------------------------------------
static unsigned char SBCLunCanBeWritten(MSDLun *lun)
{
    unsigned char canBeWritten = 0;

    if (!SBCLunIsReady(lun)) {
        
        TRACE_WARNING("SBCLunCanBeWritten: Not Ready!\n\r");
    }
    else if (lun->protected) {

        TRACE_WARNING("SBCLunCanBeWritten: Protected!\n\r");
        SBC_UpdateSenseData(&(lun->requestSenseData),
                            SBC_SENSE_KEY_DATA_PROTECT,
                            SBC_ASC_WRITE_PROTECTED,
                            0);
    }
    else {

        canBeWritten = 1;
    }

    return canBeWritten;
}

//------------------------------------------------------------------------------
//! \brief  Performs a WRITE (10) command on the specified LUN.
//!
//!         The data to write is first received from the USB host and then
//!         actually written on the media.
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_Write10(MSDLun          *lun,
                                 MSDCommandState *commandState)
{
    unsigned char status;
    unsigned char result = MSDD_STATUS_INCOMPLETE;
    SBCRead10 *command = (SBCRead10 *) commandState->cbw.pCommand;
    MSDTransfer *transfer = &(commandState->transfer);
    MSDTransfer *disktransfer = &(commandState->disktransfer);
    MSDIOFifo   *fifo = &lun->ioFifo;
    Media *media = lun->media;

    // Init command state
    if (commandState->state == 0) {

        commandState->state = SBC_STATE_WRITE;

        // The command should not be proceeded if READONLY
        if (!SBCLunCanBeWritten(lun)) {

            return MSDD_STATUS_RW;
        }
        else {


            // Initialize FIFO
            fifo->dataTotal = commandState->length;
            fifo->blockSize = lun->blockSize * media->blockSize;
          #ifdef MSDIO_WRITE10_CHUNK_SIZE
            if (   fifo->dataTotal >= 64 * 1024
                && fifo->blockSize < MSDIO_WRITE10_CHUNK_SIZE)
                fifo->chunkSize = MSDIO_WRITE10_CHUNK_SIZE;
            else
                fifo->chunkSize = fifo->blockSize;
          #endif
            fifo->fullCnt = 0;
            fifo->nullCnt = 0;

            // Initialize FIFO output (Disk)
            fifo->outputNdx = 0;
            fifo->outputTotal = 0;
            fifo->outputState = MSDIO_IDLE;
            transfer->semaphore = 0;

            // Initialize FIFO input (USB)
            fifo->inputNdx = 0;
            fifo->inputTotal = 0;
            fifo->inputState = MSDIO_START;
            disktransfer->semaphore = 0;
        }

    }

    if (commandState->length == 0) {

        // Perform the callback!
        if (lun->dataMonitor) {

            lun->dataMonitor(0, fifo->dataTotal, fifo->nullCnt, fifo->fullCnt);
        }
        return MSDD_STATUS_SUCCESS;
    }

    // USB receive task
    switch(fifo->inputState) {

    //------------------
    case MSDIO_IDLE:
    //------------------
        if (fifo->inputTotal < fifo->dataTotal &&
            fifo->inputTotal - fifo->outputTotal < fifo->bufferSize) {

            fifo->inputState = MSDIO_START;
        }
        break;

    //------------------
    case MSDIO_START:
    //------------------
        // Should not start if there is any disk error
        if (fifo->outputState == MSDIO_ERROR) {

            TRACE_INFO_WP("udErr ");
            fifo->inputState = MSDIO_ERROR;
            break;
        }

        // Read one block of data sent by the host
        if (media->mappedWR) {

            // Directly read to memory
            status = MSDD_Read((void*)
                                ((media->baseAddress
                                  + (lun->baseAddress
                                      + DWORDB(command->pLogicalBlockAddress)
                                        * lun->blockSize
                                    )
                                  ) * media->blockSize
                                ),
                                fifo->dataTotal,
                                (TransferCallback) MSDDriver_Callback,
                                (void *) transfer);
        }
        else {
          #ifdef MSDIO_WRITE10_CHUNK_SIZE
            status = SBC_RX_CHUNK(fifo, MSDDriver_Callback, transfer);
          #else
            // Read block to buffer
            status = MSDD_Read((void*)&fifo->pBuffer[fifo->inputNdx],
                               fifo->blockSize,
                               (TransferCallback) MSDDriver_Callback,
                               (void *) transfer);
          #endif
        }

        // Check operation result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Write10: Failed to start receiving\n\r");
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_HARDWARE_ERROR,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
        }
        else {

            TRACE_INFO_WP("uRx ");

            // Prepare next device state
            fifo->inputState = MSDIO_WAIT;
        }
        break; // MSDIO_START

    //------------------
    case MSDIO_WAIT:
    //------------------
        TRACE_INFO_WP("uWait ");

        // Check semaphore
        if (transfer->semaphore > 0) {

            transfer->semaphore--;
            fifo->inputState = MSDIO_NEXT;
        }
        break;

    //------------------
    case MSDIO_NEXT:
    //------------------
        // Check the result code of the read operation
        if (transfer->status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Write10: Failed to received\n\r");
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_HARDWARE_ERROR,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
        }
        else {

            TRACE_INFO_WP("uNxt ");

            // Mapped read, all data done
            if (media->mappedWR) {

                fifo->inputTotal = fifo->dataTotal;
                fifo->inputState = MSDIO_IDLE;
            }
            else {

                // Update input index
              #ifdef MSDIO_WRITE10_CHUNK_SIZE
                MSDIOFifo_IncNdx(fifo->inputNdx,
                                 fifo->chunkSize,
                                 fifo->bufferSize);
                fifo->inputTotal += fifo->chunkSize;
              #else
                MSDIOFifo_IncNdx(fifo->inputNdx,
                                 fifo->blockSize,
                                 fifo->bufferSize);
                fifo->inputTotal += fifo->blockSize;
              #endif

                // Start Next block
                // - All Data done?
                if (fifo->inputTotal >= fifo->dataTotal) {

                    fifo->inputState = MSDIO_IDLE;
                }
                // - Buffer full?
                else if (fifo->inputNdx == fifo->outputNdx) {
                    fifo->inputState = MSDIO_IDLE;
                    fifo->fullCnt ++;

                    TRACE_DEBUG_WP("ufFull%d ", fifo->inputNdx);
                }
                // - More data to transfer?
                else if (fifo->inputTotal < fifo->dataTotal) {
                    fifo->inputState = MSDIO_START;

                    TRACE_INFO_WP("uStart ");
                }
                // never executed !
                //else {
                //    fifo->inputState = MSDIO_IDLE;
                //    TRACE_INFO_WP("uDone ");
                //}
            }

        }
        break; // MSDIO_NEXT

    //------------------
    case MSDIO_ERROR:
    //------------------

        TRACE_WARNING_WP("uErr ");
        commandState->length -= fifo->inputTotal;
        return MSDD_STATUS_RW;

    }

    // Disk write task
    switch(fifo->outputState) {

    //------------------
    case MSDIO_IDLE:
    //------------------
        if (fifo->outputTotal < fifo->inputTotal) {

            fifo->outputState = MSDIO_START;
        }
        break;

    //------------------
    case MSDIO_START:
    //------------------

        // Write the block to the media
        if (media->mappedWR) {

            MSDDriver_Callback(disktransfer, MED_STATUS_SUCCESS, 0, 0);
            status = LUN_STATUS_SUCCESS;
        }
        else {
          #ifdef MSDIO_WRITE10_CHUNK_SIZE
            status = SBC_WRITE_CHUNK(lun, DWORDB(command->pLogicalBlockAddress),
                                     fifo, MSDDriver_Callback, disktransfer);
          #else
            status = LUN_Write(lun,
                               DWORDB(command->pLogicalBlockAddress),
                               &fifo->pBuffer[fifo->outputNdx],
                               1,
                               (TransferCallback) MSDDriver_Callback,
                               (void *) disktransfer);
          #endif
        }

        // Check operation result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Write10: Failed to start write - ");

            if (!SBCLunCanBeWritten(lun)) {

                TRACE_WARNING("?\n\r");
                SBC_UpdateSenseData(&(lun->requestSenseData),
                                    SBC_SENSE_KEY_NOT_READY,
                                    0,
                                    0);
            }
            
            fifo->outputState = MSDIO_ERROR;
        }
        else {

            // Prepare next state
            fifo->outputState = MSDIO_WAIT;
        }
        break; // MSDIO_START

    //------------------
    case MSDIO_WAIT:
    //------------------
        TRACE_INFO_WP("dWait ");

        // Check semaphore value
        if (disktransfer->semaphore > 0) {

            // Take semaphore and move to next state
            disktransfer->semaphore--;
            fifo->outputState = MSDIO_NEXT;
        }
        break;

    //------------------
    case MSDIO_NEXT:
    //------------------
        // Check operation result code
        if (transfer->status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Write10: Failed to write\n\r");
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_RECOVERED_ERROR,
                                SBC_ASC_TOO_MUCH_WRITE_DATA,
                                0);
            result = MSDD_STATUS_ERROR;
        }
        else {

            TRACE_INFO_WP("dNxt ");

            // Update transfer length and block address

            // Mapped memory, done
            if (media->mappedWR) {

                commandState->length = 0;
                fifo->outputState = MSDIO_IDLE;
            }
            else {

                // Update output index
              #ifdef MSDIO_WRITE10_CHUNK_SIZE
                STORE_DWORDB(DWORDB(command->pLogicalBlockAddress)
                                 + fifo->chunkSize/fifo->blockSize,
                             command->pLogicalBlockAddress);
                MSDIOFifo_IncNdx(fifo->outputNdx,
                                 fifo->chunkSize,
                                 fifo->bufferSize);
                fifo->outputTotal += fifo->chunkSize;
              #else
                STORE_DWORDB(DWORDB(command->pLogicalBlockAddress) + 1,
                             command->pLogicalBlockAddress);
                MSDIOFifo_IncNdx(fifo->outputNdx,
                                 fifo->blockSize,
                                 fifo->bufferSize);
                fifo->outputTotal += fifo->blockSize;
              #endif

                // Start Next block
                // - All data done?
                if (fifo->outputTotal >= fifo->dataTotal) {

                    fifo->outputState = MSDIO_IDLE;
                    commandState->length = 0;
                    TRACE_INFO_WP("dDone ");
                }
                // - Send next?
                else if (fifo->outputTotal < fifo->inputTotal) {

                    fifo->outputState = MSDIO_START;
                    TRACE_INFO_WP("dStart ");
                }
                // - Buffer Null?
                else {
                    fifo->outputState = MSDIO_IDLE;
                    fifo->nullCnt ++;

                    TRACE_DEBUG_WP("dfNull%d ", fifo->outputNdx);
                }
            }
        }
        break; // MSDIO_NEXT

    //------------------
    case MSDIO_ERROR:
    //------------------
        break;
    }

    return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a READ (10) command on specified LUN.
//!
//!         The data is first read from the media and then sent to the USB host.
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_Read10(MSDLun          *lun,
                                MSDCommandState *commandState)
{
    unsigned char status;
    unsigned char result = MSDD_STATUS_INCOMPLETE;
    SBCRead10 *command = (SBCRead10 *) commandState->cbw.pCommand;
    MSDTransfer *transfer = &(commandState->transfer);
    MSDTransfer *disktransfer = &(commandState->disktransfer);
    MSDIOFifo   *fifo = &lun->ioFifo;
    Media *media = lun->media;

    // Init command state
    if (commandState->state == 0) {

        commandState->state = SBC_STATE_READ;

        if (!SBCLunIsReady(lun)) {

            return MSDD_STATUS_RW;
        }
        else {

            // Initialize FIFO
            fifo->dataTotal = commandState->length;
            fifo->blockSize = lun->blockSize * media->blockSize;
          #ifdef MSDIO_READ10_CHUNK_SIZE
            if (   fifo->dataTotal >= 64*1024
                && fifo->blockSize < MSDIO_READ10_CHUNK_SIZE)
                fifo->chunkSize = MSDIO_READ10_CHUNK_SIZE;
            else
                fifo->chunkSize = fifo->blockSize;
          #endif
            fifo->fullCnt = 0;
            fifo->nullCnt = 0;

          #ifdef MSDIO_FIFO_OFFSET
            // Enable offset if total size >= 2*bufferSize
            if (fifo->dataTotal / fifo->bufferSize >= 2)
                fifo->bufferOffset = MSDIO_FIFO_OFFSET;
            else
                fifo->bufferOffset = 0;
          #endif

            // Initialize FIFO output (USB)
            fifo->outputNdx = 0;
            fifo->outputTotal = 0;
            fifo->outputState = MSDIO_IDLE;
            transfer->semaphore = 0;

            // Initialize FIFO input (Disk)
            fifo->inputNdx = 0;
            fifo->inputTotal = 0;
            fifo->inputState = MSDIO_START;
            disktransfer->semaphore = 0;
        }
    }

    // Check length
    if (commandState->length == 0) {

        // Perform the callback!
        if (lun->dataMonitor) {

            lun->dataMonitor(1, fifo->dataTotal, fifo->nullCnt, fifo->fullCnt);
        }
        return MSDD_STATUS_SUCCESS;
    }

    // Disk reading task
    switch(fifo->inputState) {

    //------------------
    case MSDIO_IDLE:
    //------------------
        if (fifo->inputTotal < fifo->dataTotal &&
            fifo->inputTotal - fifo->outputTotal < fifo->bufferSize) {

            fifo->inputState = MSDIO_START;
        }
        break;

    //------------------
    case MSDIO_START:
    //------------------
        // Read one block of data from the media
        if (media->mappedRD) {

            // Directly write, no read needed
            MSDDriver_Callback(disktransfer, MED_STATUS_SUCCESS, 0, 0);
            status = LUN_STATUS_SUCCESS;
        }
        else {
          #ifdef MSDIO_READ10_CHUNK_SIZE
            status = SBC_READ_CHUNK(lun, DWORDB(command->pLogicalBlockAddress),
                                    fifo, MSDDriver_Callback, disktransfer);
          #else
            status = LUN_Read(lun,
                              DWORDB(command->pLogicalBlockAddress),
                              &fifo->pBuffer[fifo->inputNdx],
                              1,
                              (TransferCallback) MSDDriver_Callback,
                              (void *)disktransfer);
          #endif
        }

        // Check operation result code
        if (status != LUN_STATUS_SUCCESS) {

            TRACE_WARNING("RBC_Read10: Failed to start reading\n\r");

            if (SBCLunIsReady(lun)) {

                SBC_UpdateSenseData(&(lun->requestSenseData),
                                    SBC_SENSE_KEY_NOT_READY,
                                    SBC_ASC_LOGICAL_UNIT_NOT_READY,
                                    0);
            }

            fifo->inputState = MSDIO_ERROR;
        }
        else {

            TRACE_INFO_WP("dRd ");

            // Move to next command state
            fifo->inputState = MSDIO_WAIT;
        }
        break; // MSDIO_START

    //------------------
    case MSDIO_WAIT:
    //------------------
        // Check semaphore value
        if (disktransfer->semaphore > 0) {

            TRACE_INFO_WP("dOk ");

            // Take semaphore and move to next state
            disktransfer->semaphore--;
            fifo->inputState = MSDIO_NEXT;
        }
        break;

    //------------------
    case MSDIO_NEXT:
    //------------------
        // Check the operation result code
        if (disktransfer->status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Read10: Failed to read media\n\r");
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_RECOVERED_ERROR,
                                SBC_ASC_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE,
                                0);
            result = MSDD_STATUS_ERROR;
        }
        else {

            TRACE_INFO_WP("dNxt ");

            if (media->mappedRD) {

                // All data is ready
                fifo->inputState = MSDIO_IDLE;
                fifo->inputTotal = fifo->dataTotal;
            }
            else {

                // Update block address
              #ifdef MSDIO_READ10_CHUNK_SIZE
                STORE_DWORDB(DWORDB(command->pLogicalBlockAddress)
                                 + fifo->chunkSize/fifo->blockSize,
                             command->pLogicalBlockAddress);

                // Update input index
                MSDIOFifo_IncNdx(fifo->inputNdx,
                                 fifo->chunkSize,
                                 fifo->bufferSize);
                fifo->inputTotal += fifo->chunkSize;
              #else
                // Update block address
                STORE_DWORDB(DWORDB(command->pLogicalBlockAddress) + 1,
                             command->pLogicalBlockAddress);

                // Update input index
                MSDIOFifo_IncNdx(fifo->inputNdx,
                                 fifo->blockSize,
                                 fifo->bufferSize);
                fifo->inputTotal += fifo->blockSize;
              #endif

                // Start Next block
                // - All Data done?
                if (fifo->inputTotal >= fifo->dataTotal) {

                    TRACE_INFO_WP("dDone ");
                    fifo->inputState = MSDIO_IDLE;
                }
                // - Buffer full?
                else if (fifo->inputNdx == fifo->outputNdx) {

                    TRACE_INFO_WP("dfFull%d ", (int)fifo->inputNdx);
                    fifo->inputState = MSDIO_IDLE;
                    fifo->fullCnt ++;
                }
                // - More data to transfer?
                else if (fifo->inputTotal < fifo->dataTotal) {

                    TRACE_DEBUG_WP("dStart ");
                    fifo->inputState = MSDIO_START;
                }
            }

        }

        break;

    //------------------
    case MSDIO_ERROR:
    //------------------
        break;
    }

    // USB sending task
    switch(fifo->outputState) {

    //------------------
    case MSDIO_IDLE:
    //------------------
        if (fifo->outputTotal < fifo->inputTotal) {

          #ifdef MSDIO_FIFO_OFFSET
            // Offset buffer the input data
            if (fifo->bufferOffset) {
                if (fifo->inputTotal < fifo->bufferOffset) {
                    break;
                }
                fifo->bufferOffset = 0;
            }
          #endif
            fifo->outputState = MSDIO_START;
        }
        break;

    //------------------
    case MSDIO_START:
    //------------------
        // Should not start if there is any disk error
        if (fifo->outputState == MSDIO_ERROR) {

            fifo->inputState = MSDIO_ERROR;
            break;
        }

        // Send the block to the host
        if (media->mappedRD) {

            status = MSDD_Write((void*)
                                 ((media->baseAddress
                                    + (lun->baseAddress
                                       + DWORDB(command->pLogicalBlockAddress)
                                         * lun->blockSize
                                      )
                                   ) * media->blockSize
                                 ),
                                commandState->length,
                                (TransferCallback) MSDDriver_Callback,
                                (void *) transfer);
        }
        else {
          #ifdef MSDIO_READ10_CHUNK_SIZE
            status = SBC_TX_CHUNK(fifo, MSDDriver_Callback, transfer);
          #else
            status = MSDD_Write(&fifo->pBuffer[fifo->outputNdx],
                                fifo->blockSize,
                                (TransferCallback) MSDDriver_Callback,
                                (void *) transfer);
          #endif
        }
        // Check operation result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Read10: Failed to start to send\n\r");
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_HARDWARE_ERROR,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
        }
        else {

            TRACE_INFO_WP("uTx ");

            // Move to next command state
            fifo->outputState = MSDIO_WAIT;
        }
        break; // MSDIO_START

    //------------------
    case MSDIO_WAIT:
    //------------------
        // Check semaphore value
        if (transfer->semaphore > 0) {

            TRACE_INFO_WP("uOk ");

            // Take semaphore and move to next state
            transfer->semaphore--;
            fifo->outputState = MSDIO_NEXT;
        }
        break;

    //------------------
    case MSDIO_NEXT:
    //------------------
        // Check operation result code
        if (transfer->status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_Read10: Failed to send data\n\r");
            SBC_UpdateSenseData(&(lun->requestSenseData),
                                SBC_SENSE_KEY_HARDWARE_ERROR,
                                0,
                                0);
            result = MSDD_STATUS_ERROR;
        }
        else {

            TRACE_INFO_WP("uNxt ");

            if (media->mappedRD) {

                commandState->length = 0;
            }
            else {

                // Update output index
              #ifdef MSDIO_READ10_CHUNK_SIZE
                MSDIOFifo_IncNdx(fifo->outputNdx,
                                 fifo->chunkSize,
                                 fifo->bufferSize);
                fifo->outputTotal += fifo->chunkSize;
              #else
                MSDIOFifo_IncNdx(fifo->outputNdx,
                                 fifo->blockSize,
                                 fifo->bufferSize);
                fifo->outputTotal += fifo->blockSize;
              #endif

                // Start Next block
                // - All data done?
                if (fifo->outputTotal >= fifo->dataTotal) {

                    fifo->outputState = MSDIO_IDLE;
                    commandState->length = 0;
                    TRACE_INFO_WP("uDone ");
                }
                // - Buffer Null?
                else if (fifo->inputNdx == fifo->outputNdx) {

                    TRACE_INFO_WP("ufNull%d ", (int)fifo->outputNdx);
                    fifo->outputState = MSDIO_IDLE;
                    fifo->nullCnt ++;
                }
                // - Send next?
                else if (fifo->outputTotal < fifo->inputTotal) {

                    TRACE_DEBUG_WP("uStart ");
                    fifo->outputState = MSDIO_START;
                }
            }

        }
        break;

    //------------------
    case MSDIO_ERROR:
    //------------------
        break;
    }

    return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a READ CAPACITY (10) command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDD_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_ReadCapacity10(MSDLun               *lun,
                                        MSDCommandState *commandState)
{
    unsigned char result = MSDD_STATUS_INCOMPLETE;
    unsigned char status;
    MSDTransfer *transfer = &(commandState->transfer);
    
    if (!SBCLunIsReady(lun)) {
        
        TRACE_INFO("SBC_ReadCapacity10: Not Ready!\n\r");
        return MSDD_STATUS_RW;
    }

    // Initialize command state if needed
    if (commandState->state == 0) {

        commandState->state = SBC_STATE_WRITE;
    }

    // Identify current command state
    switch (commandState->state) {
    //-------------------
    case SBC_STATE_WRITE:
    //-------------------
        // Start the write operation
        status = MSDD_Write(&(lun->readCapacityData),
                            commandState->length,
                            (TransferCallback) MSDDriver_Callback,
                            (void *) transfer);

        // Check operation result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_ReadCapacity: Cannot start sending data\n\r");
            result = MSDD_STATUS_ERROR;
        }
        else {

            // Proceed to next command state
            TRACE_INFO_WP("Sending ");
            commandState->state = SBC_STATE_WAIT_WRITE;
        }
        break;

    //------------------------
    case SBC_STATE_WAIT_WRITE:
    //------------------------
        // Check semaphore value
        if (transfer->semaphore > 0) {

            // Take semaphore and terminate command
            transfer->semaphore--;

            if (transfer->status != USBD_STATUS_SUCCESS) {

                TRACE_WARNING("RBC_ReadCapacity: Cannot send data\n\r");
                result = MSDD_STATUS_ERROR;
            }
            else {

                TRACE_INFO_WP("Sent ");
                result = MSDD_STATUS_SUCCESS;
            }
            commandState->length -= transfer->transferred;
        }
        break;
    }

    return result;
}

//------------------------------------------------------------------------------
//! \brief  Handles an INQUIRY command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_Inquiry(MSDLun               *lun,
                                 MSDCommandState *commandState)
{
    unsigned char  result = MSDD_STATUS_INCOMPLETE;
    unsigned char  status;
    MSDTransfer *transfer = &(commandState->transfer);

    // Check if required length is 0
    if (commandState->length == 0) {

        // Nothing to do
        result = MSDD_STATUS_SUCCESS;
    }
    // Initialize command state if needed
    else if (commandState->state == 0) {

        commandState->state = SBC_STATE_WRITE;

        // Change additional length field of inquiry data
        lun->inquiryData->bAdditionalLength
            = (unsigned char) (commandState->length - 5);
    }

    // Identify current command state
    switch (commandState->state) {
    //-------------------
    case SBC_STATE_WRITE:
    //-------------------
        // Start write operation
        status = MSDD_Write((void *) lun->inquiryData,
                            commandState->length,
                            (TransferCallback) MSDDriver_Callback,
                            (void *) transfer);

        // Check operation result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "SPC_Inquiry: Cannot start sending data\n\r");
            result = MSDD_STATUS_ERROR;
        }
        else {

            // Proceed to next state
            TRACE_INFO_WP("Sending ");
            commandState->state = SBC_STATE_WAIT_WRITE;
        }
        break;

    //------------------------
    case SBC_STATE_WAIT_WRITE:
    //------------------------
        // Check the semaphore value
        if (transfer->semaphore > 0) {

            // Take semaphore and terminate command
            transfer->semaphore--;

            if (transfer->status != USBD_STATUS_SUCCESS) {

                TRACE_WARNING(
                    "SPC_Inquiry: Data transfer failed\n\r");
                result = MSDD_STATUS_ERROR;
            }
            else {

                TRACE_INFO_WP("Sent ");
                result = MSDD_STATUS_SUCCESS;
            }

            // Update length field
            commandState->length -= transfer->transferred;
        }
        break;
    }

    return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a REQUEST SENSE command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_RequestSense(MSDLun               *lun,
                                      MSDCommandState *commandState)
{
    unsigned char result = MSDD_STATUS_INCOMPLETE;
    unsigned char status;
    MSDTransfer *transfer = &(commandState->transfer);

    // Check if requested length is zero
    if (commandState->length == 0) {

        // Nothing to do
        result = MSDD_STATUS_SUCCESS;
    }
    // Initialize command state if needed
    else if (commandState->state == 0) {

        commandState->state = SBC_STATE_WRITE;
    }

    // Identify current command state
    switch (commandState->state) {
    //-------------------
    case SBC_STATE_WRITE:
    //-------------------
        // Start transfer
        status = MSDD_Write(&(lun->requestSenseData),
                            commandState->length,
                            (TransferCallback) MSDDriver_Callback,
                            (void *) transfer);

        // Check result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "RBC_RequestSense: Cannot start sending data\n\r");
            result = MSDD_STATUS_ERROR;
        }
        else {

            // Change state
            commandState->state = SBC_STATE_WAIT_WRITE;
        }
        break;

    //------------------------
    case SBC_STATE_WAIT_WRITE:
    //------------------------
        // Check the transfer semaphore
        if (transfer->semaphore > 0) {

            // Take semaphore and finish command
            transfer->semaphore--;

            if (transfer->status != USBD_STATUS_SUCCESS) {

                result = MSDD_STATUS_ERROR;
            }
            else {

                result = MSDD_STATUS_SUCCESS;
            }

            // Update length
            commandState->length -= transfer->transferred;
        }
        break;
    }

    return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a MODE SENSE (6) command.
//!
//!         This function operates asynchronously and must be called multiple
//!         times to complete. A result code of MSDDriver_STATUS_INCOMPLETE
//!         indicates that at least another call of the method is necessary.
//! \param  lun          Pointer to the LUN affected by the command
//! \param  commandState Current state of the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//! \see    MSDCommandState
//------------------------------------------------------------------------------
static unsigned char SBC_ModeSense6(MSDLun *lun, MSDCommandState *commandState)
{
    unsigned char      result = MSDD_STATUS_INCOMPLETE;
    unsigned char      status;
    MSDTransfer     *transfer = &(commandState->transfer);

    if (!SBCLunIsReady(lun)) {
        
        TRACE_INFO("SBC_ModeSense6: Not Ready!\n\r");
        return MSDD_STATUS_RW;
    }

    // Check if mode page is supported
    if (((SBCCommand *) commandState->cbw.pCommand)->modeSense6.bPageCode
        != SBC_PAGE_RETURN_ALL) {

        return MSDD_STATUS_PARAMETER;
    }

    // Initialize command state if needed
    if (commandState->state == 0) {

        commandState->state = SBC_STATE_WRITE;
    }

    // Check current command state
    switch (commandState->state) {
    //-------------------
    case SBC_STATE_WRITE:
    //-------------------
        // Start transfer
        status = MSDD_Write((void *) &modeParameterHeader6,
                            commandState->length,
                            (TransferCallback) MSDDriver_Callback,
                            (void *) transfer);

        // Check operation result code
        if (status != USBD_STATUS_SUCCESS) {

            TRACE_WARNING(
                "SPC_ModeSense6: Cannot start data transfer\n\r");
            result = MSDD_STATUS_ERROR;
        }
        else {

            // Proceed to next state
            commandState->state = SBC_STATE_WAIT_WRITE;
        }
        break;

    //------------------------
    case SBC_STATE_WAIT_WRITE:
    //------------------------
        TRACE_INFO_WP("Wait ");

        // Check semaphore value
        if (transfer->semaphore > 0) {

            // Take semaphore and terminate command
            transfer->semaphore--;

            if (transfer->status != USBD_STATUS_SUCCESS) {

                TRACE_WARNING(
                    "SPC_ModeSense6: Data transfer failed\n\r");
                result = MSDD_STATUS_ERROR;
            }
            else {

                result = MSDD_STATUS_SUCCESS;
            }

            // Update length field
            commandState->length -= transfer->transferred;

        }
        break;
    }

    return result;
}

//------------------------------------------------------------------------------
//! \brief  Performs a TEST UNIT READY COMMAND command.
//! \param  lun          Pointer to the LUN affected by the command
//! \return Operation result code (SUCCESS, ERROR, INCOMPLETE or PARAMETER)
//! \see    MSDLun
//------------------------------------------------------------------------------
static unsigned char SBC_TestUnitReady(MSDLun *lun)
{
    unsigned char result = MSDD_STATUS_RW;
    unsigned char senseKey = SBC_SENSE_KEY_NO_SENSE,
                  addSenseCode = 0,
                  addSenseCodeQual = 0;
    Media *media = lun->media;

    // Check current media state
    if (lun->status < LUN_CHANGED) {

        TRACE_INFO_WP("Ejc ");
        senseKey = SBC_SENSE_KEY_NOT_READY;
        addSenseCode = SBC_ASC_MEDIUM_NOT_PRESENT;
    }
    else if (lun->status == LUN_CHANGED) {

        TRACE_INFO_WP("Chg ");
        senseKey = SBC_SENSE_KEY_UNIT_ATTENTION;
        addSenseCode = SBC_ASC_NOT_READY_TO_READY_CHANGE;
        lun->status = LUN_READY;
    }
    else {

        switch(media->state) {
        //-------------------
        case MED_STATE_READY:
        //-------------------
            // Nothing to do
            TRACE_INFO_WP("Rdy ");
            result = MSDD_STATUS_SUCCESS;
            break;

        //------------------
        case MED_STATE_BUSY:
        //------------------
            TRACE_INFO_WP("Bsy ");
            senseKey = SBC_SENSE_KEY_NOT_READY;
            break;

        //------
        default:
        //------
            TRACE_INFO_WP("? ");
            senseKey = SBC_SENSE_KEY_NOT_READY;
            addSenseCode = SBC_ASC_MEDIUM_NOT_PRESENT;
            break;
        }
    }
    SBC_UpdateSenseData(&(lun->requestSenseData),
                        senseKey,
                        addSenseCode,
                        addSenseCodeQual);

    return result;
}

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//! \brief  Updates the sense data of a LUN with the given key and codes
//! \param  requestSenseData             Pointer to the sense data to update
//! \param  senseKey                     Sense key
//! \param  additionalSenseCode          Additional sense code
//! \param  additionalSenseCodeQualifier Additional sense code qualifier
//------------------------------------------------------------------------------
void SBC_UpdateSenseData(SBCRequestSenseData *requestSenseData,
                         unsigned char senseKey,
                         unsigned char additionalSenseCode,
                         unsigned char additionalSenseCodeQualifier)
{
    requestSenseData->bSenseKey = senseKey;
    requestSenseData->bAdditionalSenseCode = additionalSenseCode;
    requestSenseData->bAdditionalSenseCodeQualifier
        = additionalSenseCodeQualifier;
}

//------------------------------------------------------------------------------
//! \brief  Return information about the transfer length and direction expected
//!         by the device for a particular command.
//! \param  command Pointer to a buffer holding the command to evaluate
//! \param  length  Expected length of the data transfer
//! \param  type    Expected direction of data transfer
//! \param  lun     Pointer to the LUN affected by the command
//------------------------------------------------------------------------------
unsigned char SBC_GetCommandInformation(void          *command,
                               unsigned int  *length,
                               unsigned char *type,
                               MSDLun         *lun)
{
    SBCCommand *sbcCommand = (SBCCommand *) command;
    unsigned char          isCommandSupported = 1;
    Media *media = lun->media;

    // Identify command
    switch (sbcCommand->bOperationCode) {
    //---------------
    case SBC_INQUIRY:
    //---------------
        (*type) = MSDD_DEVICE_TO_HOST;

        // Allocation length is stored in big-endian format
        (*length) = WORDB(sbcCommand->inquiry.pAllocationLength);
        break;

    //--------------------
    case SBC_MODE_SENSE_6:
    //--------------------
        (*type) = MSDD_DEVICE_TO_HOST;
        if (sbcCommand->modeSense6.bAllocationLength >
            sizeof(SBCModeParameterHeader6)) {

            *length = sizeof(SBCModeParameterHeader6);
        }
        else {

            *length = sbcCommand->modeSense6.bAllocationLength;
        }
        break;

    //------------------------------------
    case SBC_PREVENT_ALLOW_MEDIUM_REMOVAL:
    case SBC_START_STOP:
    //------------------------------------
        (*type) = MSDD_NO_TRANSFER;
        break;

    //---------------------
    case SBC_REQUEST_SENSE:
    //---------------------
        (*type) = MSDD_DEVICE_TO_HOST;
        (*length) = sbcCommand->requestSense.bAllocationLength;
        break;

    //-----------------------
    case SBC_TEST_UNIT_READY:
    //-----------------------
        (*type) = MSDD_NO_TRANSFER;
        break;

    //---------------------
    case SBC_READ_CAPACITY_10:
    //---------------------
        (*type) = MSDD_DEVICE_TO_HOST;
        (*length) = sizeof(SBCReadCapacity10Data);
        break;

    //---------------
    case SBC_READ_10:
    //---------------
        (*type) = MSDD_DEVICE_TO_HOST;
        (*length) = WORDB(sbcCommand->read10.pTransferLength)
                     * lun->blockSize * media->blockSize;
        break;

    //----------------
    case SBC_WRITE_10:
    //----------------
        (*type) = MSDD_HOST_TO_DEVICE;
        (*length) = WORDB(sbcCommand->write10.pTransferLength)
                     * lun->blockSize * media->blockSize;
        break;

    //-----------------
    case SBC_VERIFY_10:
    //-----------------
        (*type) = MSDD_NO_TRANSFER;
        break;
  #if 0
    //---------------------
    case SBC_READ_FORMAT_CAPACITIES:
    //---------------------
        (*type) = MSDD_DEVICE_TO_HOST;
        (*length) = 1;
        break;
  #endif
    //------
    default:
    //------
        isCommandSupported = 0;
    }

    // If length is 0, no transfer is expected
    if ((*length) == 0) {

        (*type) = MSDD_NO_TRANSFER;
    }

    return isCommandSupported;
}

//------------------------------------------------------------------------------
//! \brief  Processes a SBC command by dispatching it to a subfunction.
//! \param  lun          Pointer to the affected LUN
//! \param  commandState Pointer to the current command state
//! \return Operation result code
//------------------------------------------------------------------------------
unsigned char SBC_ProcessCommand(MSDLun               *lun,
                                 MSDCommandState *commandState)
{
    unsigned char result = MSDD_STATUS_INCOMPLETE;
    SBCCommand *command = (SBCCommand *) commandState->cbw.pCommand;
    Media *media = lun->media;

    // Identify command
    switch (command->bOperationCode) {
    //---------------
    case SBC_READ_10:
    //---------------
        TRACE_DEBUG_WP("Read(10) ");

        // Perform the Read10 command
        result = SBC_Read10(lun, commandState);
        break;

    //----------------
    case SBC_WRITE_10:
    //----------------
        TRACE_DEBUG_WP("Write(10) ");

        // Perform the Write10 command
        result = SBC_Write10(lun, commandState);
        break;

    //---------------------
    case SBC_READ_CAPACITY_10:
    //---------------------
        TRACE_INFO_WP("RdCapacity(10) ");

        // Perform the ReadCapacity command
        result = SBC_ReadCapacity10(lun, commandState);
        break;

    //---------------------
    case SBC_VERIFY_10:
    //---------------------
        TRACE_INFO_WP("Verify(10) ");

        // Flush media
        MED_Flush(media);
        result = MSDD_STATUS_SUCCESS;
        break;

    //---------------
    case SBC_INQUIRY:
    //---------------
        TRACE_INFO_WP("Inquiry ");

        // Process Inquiry command
        result = SBC_Inquiry(lun, commandState);
        break;

    //--------------------
    case SBC_MODE_SENSE_6:
    //--------------------
        TRACE_INFO_WP("ModeSense(6) ");

        // Process ModeSense6 command
        result = SBC_ModeSense6(lun, commandState);
        break;

    //-----------------------
    case SBC_TEST_UNIT_READY:
    //-----------------------
        TRACE_INFO_WP("TstUnitRdy ");

        // Process TestUnitReady command
        //MED_Flush(media);
        result = SBC_TestUnitReady(lun);
        break;

    //---------------------
    case SBC_REQUEST_SENSE:
    //---------------------
        TRACE_INFO_WP("ReqSense ");

        // Perform the RequestSense command
        result = SBC_RequestSense(lun, commandState);
        break;

    //------------------------------------
    case SBC_PREVENT_ALLOW_MEDIUM_REMOVAL:
    //------------------------------------
        TRACE_INFO_WP("PrevAllowRem ");

        // Check parameter
        result = command->mediumRemoval.bPrevent ?
                    MSDD_STATUS_PARAMETER : MSDD_STATUS_SUCCESS;
        result = MSDD_STATUS_SUCCESS;
        break;

    case SBC_START_STOP:
        TRACE_INFO_WP("StartStop ");

        result = command->startStopCmd.startStop ;
        if (result == 0) {
          lun->status = LUN_EJECTED ;
        }
        else {
          lun->status = LUN_CHANGED ;
        }
        result = MSDD_STATUS_SUCCESS;
        break;

  #if 0
    //------------------------------------
    case SBC_READ_FORMAT_CAPACITIES:
    //------------------------------------
        TRACE_INFO_WP("RdFmtCap ");
        result = MSDD_STATUS_RW;
        break;
        /* TODO BSS Added if (!SBCLunIsReady(lun)) {

            result = MSDD_STATUS_RW;
            break;
        } */
  #endif
    //------
    default:
    //------
        result = MSDD_STATUS_PARAMETER;
    }

    return result;
}
