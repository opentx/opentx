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

//-----------------------------------------------------------------------------
//      Includes
//-----------------------------------------------------------------------------

#include "../../../usb/device/massstorage/MSDDStateMachine.h"

#include "../../../usb/device/massstorage/SBCMethods.h"

//-----------------------------------------------------------------------------
//      Internal functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// Returns the expected transfer length and direction (IN, OUT or don't care)
/// from the host point-of-view.
/// \param  cbw     Pointer to the CBW to examinate
/// \param  pLength Expected length of command
/// \param  pType   Expected direction of command
//-----------------------------------------------------------------------------
static void MSDD_GetCommandInformation(MSCbw *cbw,
                                       unsigned int  *length,
                                       unsigned char *type)
{
    // Expected host transfer direction and length
    (*length) = cbw->dCBWDataTransferLength;

    if (*length == 0) {

        (*type) = MSDD_NO_TRANSFER;
    }
    else if ((cbw->bmCBWFlags & MSD_CBW_DEVICE_TO_HOST) != 0) {

        (*type) = MSDD_DEVICE_TO_HOST;
    }
    else {

        (*type) = MSDD_HOST_TO_DEVICE;
    }
}

//-----------------------------------------------------------------------------
/// Pre-processes a command by checking the differences between the host and
/// device expectations in term of transfer type and length.
/// Once one of the thirteen cases is identified, the actions to do during the
/// post-processing phase are stored in the dCase variable of the command 
/// state.
/// \param  pMsdDriver Pointer to a MSDDriver instance
/// \return 1 if the command is supported, false otherwise
//-----------------------------------------------------------------------------
static unsigned char MSDD_PreProcessCommand(MSDDriver *pMsdDriver)
{
    unsigned int        hostLength = 0;
    unsigned int        deviceLength = 0;
    unsigned char       hostType;
    unsigned char       deviceType;
    unsigned char       isCommandSupported;
    MSDCommandState *commandState = &(pMsdDriver->commandState);
    MSCsw           *csw = &(commandState->csw);
    MSCbw           *cbw = &(commandState->cbw);
    MSDLun          *lun = &(pMsdDriver->luns[(unsigned char) cbw->bCBWLUN]);

    // Get information about the command
    // Host-side
    MSDD_GetCommandInformation(cbw, &hostLength, &hostType);

    // Device-side
    isCommandSupported = SBC_GetCommandInformation(cbw->pCommand,
                                                   &deviceLength,
                                                   &deviceType,
                                                   lun);

    // Initialize data residue and result status
    csw->dCSWDataResidue = 0;
    csw->bCSWStatus = MSD_CSW_COMMAND_PASSED;

    // Check if the command is supported
    if (isCommandSupported) {

        // Identify the command case
        if(hostType == MSDD_NO_TRANSFER) {

            // Case 1  (Hn = Dn)
            if(deviceType == MSDD_NO_TRANSFER) {

                //TRACE_WARNING("Case 1\n\r");
                commandState->postprocess = 0;
                commandState->length = 0;
            }
            else if(deviceType == MSDD_DEVICE_TO_HOST) {

                // Case 2  (Hn < Di)
                TRACE_WARNING(
                    "MSDD_PreProcessCommand: Case 2\n\r");
                commandState->postprocess = MSDD_CASE_PHASE_ERROR;
                commandState->length = 0;
            }
            else { //if(deviceType == MSDD_HOST_TO_DEVICE) {

                // Case 3  (Hn < Do)
                TRACE_WARNING(
                    "MSDD_PreProcessCommand: Case 3\n\r");
                commandState->postprocess = MSDD_CASE_PHASE_ERROR;
                commandState->length = 0;
            }
        }

        // Case 4  (Hi > Dn)
        else if(hostType == MSDD_DEVICE_TO_HOST) {

            if(deviceType == MSDD_NO_TRANSFER) {

                TRACE_WARNING(
                    "MSDD_PreProcessCommand: Case 4\n\r");
                commandState->postprocess = MSDD_CASE_STALL_IN;
                commandState->length = 0;
                csw->dCSWDataResidue = hostLength;
            }
            else if(deviceType == MSDD_DEVICE_TO_HOST) {

                TRACE_ERROR("cmd=0x%0x h=%d d=%d", cbw->pCommand[0], hostLength, deviceLength);

                if(hostLength > deviceLength) {

                    // Case 5  (Hi > Di)
                    TRACE_WARNING(
                        "MSDD_PreProcessCommand: Case 5\n\r");
                    commandState->postprocess = MSDD_CASE_STALL_IN;
                    commandState->length = deviceLength;
                    csw->dCSWDataResidue = hostLength - deviceLength;
                }
                else if(hostLength == deviceLength) {

                    // Case 6  (Hi = Di)
                    commandState->postprocess = 0;
                    commandState->length = deviceLength;
                }
                else { //if(hostLength < deviceLength) {

                    // Case 7  (Hi < Di)
                    TRACE_WARNING(
                        "MSDD_PreProcessCommand: Case 7\n\r");
                    commandState->postprocess = MSDD_CASE_PHASE_ERROR;
                    commandState->length = hostLength;
                }
            }
            else { //if(deviceType == MSDD_HOST_TO_DEVICE) {

                // Case 8  (Hi <> Do)
                TRACE_WARNING(
                    "MSDD_PreProcessCommand: Case 8\n\r");
                commandState->postprocess =
                    MSDD_CASE_STALL_IN | MSDD_CASE_PHASE_ERROR;
                commandState->length = 0;
            }
        }
        else if(hostType == MSDD_HOST_TO_DEVICE) {

            if(deviceType == MSDD_NO_TRANSFER) {

                // Case 9  (Ho > Dn)
                TRACE_WARNING(
                    "MSDD_PreProcessCommand: Case 9\n\r");
                commandState->postprocess = MSDD_CASE_STALL_OUT;
                commandState->length = 0;
                csw->dCSWDataResidue = hostLength;
            }
            else if(deviceType == MSDD_DEVICE_TO_HOST) {

                // Case 10 (Ho <> Di)
                TRACE_WARNING(
                    "MSDD_PreProcessCommand: Case 10\n\r");
                commandState->postprocess =
                    MSDD_CASE_STALL_OUT | MSDD_CASE_PHASE_ERROR;
                commandState->length = 0;
            }
            else { //if(deviceType == MSDD_HOST_TO_DEVICE) {

                if(hostLength > deviceLength) {

                    // Case 11 (Ho > Do)
                    TRACE_WARNING(
                        "MSDD_PreProcessCommand: Case 11\n\r");
                    commandState->postprocess = MSDD_CASE_STALL_OUT;
//                    commandState->length = deviceLength;
//                    csw->dCSWDataResidue = hostLength - deviceLength;
                    commandState->length = 0;
                    csw->dCSWDataResidue = deviceLength;
                }
                else if(hostLength == deviceLength) {

                    // Case 12 (Ho = Do)
                    //TRACE_WARNING(
                    //    "MSDD_PreProcessCommand: Case 12\n\r");
                    commandState->postprocess = 0;
                    commandState->length = deviceLength;
                }
                else { //if(hostLength < deviceLength) {

                    // Case 13 (Ho < Do)
                    TRACE_WARNING(
                        "MSDD_PreProcessCommand: Case 13\n\r");
                    commandState->postprocess = MSDD_CASE_PHASE_ERROR;
                    commandState->length = hostLength;
                }
            }
        }
    }

    return isCommandSupported;
}

//-----------------------------------------------------------------------------
/// Post-processes a command given the case identified during the
/// pre-processing step.
/// Depending on the case, one of the following actions can be done:
///           - Bulk IN endpoint is stalled
///           - Bulk OUT endpoint is stalled
///           - CSW status set to phase error
/// \param  pMsdDriver Pointer to a MSDDriver instance
/// \return If the device is halted
//-----------------------------------------------------------------------------
static unsigned char MSDD_PostProcessCommand(MSDDriver *pMsdDriver)
{
    MSDCommandState *commandState = &(pMsdDriver->commandState);
    MSCsw           *csw = &(commandState->csw);
    unsigned char haltStatus = 0;

    // STALL Bulk IN endpoint ?
    if ((commandState->postprocess & MSDD_CASE_STALL_IN) != 0) {

        TRACE_INFO_WP("StallIn ");
        MSDD_Halt(MSDD_CASE_STALL_IN);
        haltStatus = 1;
    }

    // STALL Bulk OUT endpoint ?
    if ((commandState->postprocess & MSDD_CASE_STALL_OUT) != 0) {

        TRACE_INFO_WP("StallOut ");
        MSDD_Halt(MSDD_CASE_STALL_OUT);
        haltStatus = 1;
    }

    // Set CSW status code to phase error ?
    if ((commandState->postprocess & MSDD_CASE_PHASE_ERROR) != 0) {

        TRACE_INFO_WP("PhaseErr ");
        csw->bCSWStatus = MSD_CSW_PHASE_ERROR;
    }

    return haltStatus;
}

//-----------------------------------------------------------------------------
/// Processes the latest command received by the %device.
/// \param  pMsdDriver Pointer to a MSDDriver instance
/// \return 1 if the command has been completed, false otherwise.
//-----------------------------------------------------------------------------
static unsigned char MSDD_ProcessCommand(MSDDriver * pMsdDriver)
{
    unsigned char   status;
    MSDCommandState *commandState = &(pMsdDriver->commandState);
    MSCbw           *cbw = &(commandState->cbw);
    MSCsw           *csw = &(commandState->csw);
    MSDLun          *lun = &(pMsdDriver->luns[(unsigned char) cbw->bCBWLUN]);
    unsigned char   isCommandComplete = 0;

    // Check if LUN is valid
    if (cbw->bCBWLUN > pMsdDriver->maxLun) {

        TRACE_WARNING(
            "MSDD_ProcessCommand: LUN %d not exist\n\r", cbw->bCBWLUN);
        status = MSDD_STATUS_ERROR;
    }
    else {

        // Process command
        if (pMsdDriver->maxLun > 0) {

            TRACE_INFO_WP("LUN%d ", cbw->bCBWLUN);
        }

        status = SBC_ProcessCommand(lun, commandState);
    }

    // Check command result code
    if (status == MSDD_STATUS_PARAMETER) {

        TRACE_WARNING(
            "MSDD_ProcessCommand: Unknown cmd 0x%02X\n\r",
            cbw->pCommand[0]);

        // Update sense data
        SBC_UpdateSenseData(&(lun->requestSenseData),
                            SBC_SENSE_KEY_ILLEGAL_REQUEST,
                            SBC_ASC_INVALID_FIELD_IN_CDB,
                            0);

        // Result codes
        csw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
        isCommandComplete = 1;

        // stall the request, IN or OUT
        if (((cbw->bmCBWFlags & MSD_CBW_DEVICE_TO_HOST) == 0)
            && (cbw->dCBWDataTransferLength > 0)) {

            // Stall the OUT endpoint : host to device
            // MSDD_Halt(MSDD_CASE_STALL_OUT);
            commandState->postprocess = MSDD_CASE_STALL_OUT;
            TRACE_INFO_WP("StaOUT ");
        }
        else {

            // Stall the IN endpoint : device to host
            // MSDD_Halt(MSDD_CASE_STALL_IN);
            commandState->postprocess = MSDD_CASE_STALL_IN;
            TRACE_INFO_WP("StaIN ");
        }
    }
    else if (status == MSDD_STATUS_ERROR) {

        TRACE_WARNING("MSD_ProcessCommand: Cmd %x fail\n\r",
                   ((SBCCommand*)commandState->cbw.pCommand)->bOperationCode);

        // Update sense data
        SBC_UpdateSenseData(&(lun->requestSenseData),
                            SBC_SENSE_KEY_MEDIUM_ERROR,
                            SBC_ASC_INVALID_FIELD_IN_CDB,
                            0);

        // Result codes
        csw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
        isCommandComplete = 1;
    }
    else if (status == MSDD_STATUS_RW) {

        csw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
        isCommandComplete = 1;
    }
    else {

        // Update sense data
        SBC_UpdateSenseData(&(lun->requestSenseData),
                            SBC_SENSE_KEY_NO_SENSE,
                            0,
                            0);

        // Is command complete ?
        if (status == MSDD_STATUS_SUCCESS) {

            isCommandComplete = 1;
        }
    }

    // Check if command has been completed
    if (isCommandComplete) {

        TRACE_INFO_WP("Cplt ");

        // Adjust data residue
        if (commandState->length != 0) {

            csw->dCSWDataResidue += commandState->length;

            // STALL the endpoint waiting for data
            if ((cbw->bmCBWFlags & MSD_CBW_DEVICE_TO_HOST) == 0) {

                // Stall the OUT endpoint : host to device
                // MSDD_Halt(MSDD_CASE_STALL_OUT);
                commandState->postprocess = MSDD_CASE_STALL_OUT;
                TRACE_INFO_WP("StaOUT ");
            }
            else {

                // Stall the IN endpoint : device to host
                // MSDD_Halt(MSDD_CASE_STALL_IN);
                commandState->postprocess = MSDD_CASE_STALL_IN;
                TRACE_INFO_WP("StaIN ");
            }
        }

        // Reset command state
        commandState->state = 0;
    }

    return isCommandComplete;
}

//-----------------------------------------------------------------------------
/// State machine for the MSD %device driver
/// \param  pMsdDriver Pointer to a MSDDriver instance
//-----------------------------------------------------------------------------
void MSDD_StateMachine(MSDDriver * pMsdDriver)
{
    MSDCommandState *commandState = &(pMsdDriver->commandState);
    MSCbw           *cbw = &(commandState->cbw);
    MSCsw           *csw = &(commandState->csw);
    MSDTransfer     *transfer = &(commandState->transfer);
    unsigned char   status;

    // Identify current driver state
    switch (pMsdDriver->state) {
    //----------------------
    case MSDD_STATE_READ_CBW:
    //----------------------
        // Start the CBW read operation
        transfer->semaphore = 0;
        status = MSDD_Read(cbw,
                           MSD_CBW_SIZE,
                           (TransferCallback) MSDDriver_Callback,
                           (void *) transfer);

        // Check operation result code
        if (status == USBD_STATUS_SUCCESS) {

            // If the command was successful, wait for transfer
            pMsdDriver->state = MSDD_STATE_WAIT_CBW;
        }
        break;

    //----------------------
    case MSDD_STATE_WAIT_CBW:
    //----------------------
        // Check transfer semaphore
        if (transfer->semaphore > 0) {

            // Take semaphore and terminate transfer
            transfer->semaphore--;

            // Check if transfer was successful
            if (transfer->status == USBD_STATUS_SUCCESS) {

                TRACE_INFO_WP("------------------------------\n\r");

                // Process received command
                pMsdDriver->state = MSDD_STATE_PROCESS_CBW;
            }
            else if (transfer->status == USBD_STATUS_RESET) {

                TRACE_INFO("MSDD_StateMachine: EP resetted\n\r");
                pMsdDriver->state = MSDD_STATE_READ_CBW;
            }
            else {

                TRACE_WARNING(
                    "MSDD_StateMachine: Failed to read CBW\n\r");
                pMsdDriver->state = MSDD_STATE_READ_CBW;
            }
        }
        break;

    //-------------------------
    case MSDD_STATE_PROCESS_CBW:
    //-------------------------
        // Check if this is a new command
        if (commandState->state == 0) {

            // Copy the CBW tag
            csw->dCSWTag = cbw->dCBWTag;

            // Check that the CBW is 31 bytes long
            if ((transfer->transferred != MSD_CBW_SIZE) ||
                (transfer->remaining != 0)) {

                TRACE_WARNING(
                    "MSDD_StateMachine: Invalid CBW (len %d)\n\r",
                    (int)transfer->transferred);

                // Wait for a reset recovery
                pMsdDriver->waitResetRecovery = 1;

                // Halt the Bulk-IN and Bulk-OUT pipes
                MSDD_Halt(MSDD_CASE_STALL_OUT | MSDD_CASE_STALL_IN);

                csw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
                pMsdDriver->state = MSDD_STATE_READ_CBW;

            }
            // Check the CBW Signature
            else if (cbw->dCBWSignature != MSD_CBW_SIGNATURE) {

                TRACE_WARNING(
                    "MSD_BOTStateMachine: Invalid CBW (Bad signature)\n\r");

                // Wait for a reset recovery
                pMsdDriver->waitResetRecovery = 1;

                // Halt the Bulk-IN and Bulk-OUT pipes
                MSDD_Halt(MSDD_CASE_STALL_OUT | MSDD_CASE_STALL_IN);

                csw->bCSWStatus = MSD_CSW_COMMAND_FAILED;
                pMsdDriver->state = MSDD_STATE_READ_CBW;
            }
            else {

                // Pre-process command
                MSDD_PreProcessCommand(pMsdDriver);
            }
        }

        // Process command
        if (csw->bCSWStatus == MSDD_STATUS_SUCCESS) {

            if (MSDD_ProcessCommand(pMsdDriver)) {

                // Post-process command if it is finished
                if (MSDD_PostProcessCommand(pMsdDriver)) {

                    TRACE_INFO_WP("WaitHALT ");
                    pMsdDriver->state = MSDD_STATE_WAIT_HALT;
                }
                else {

                    pMsdDriver->state = MSDD_STATE_SEND_CSW;
                }
            }
            TRACE_INFO_WP("\n\r");
        }

        break;

    //----------------------
    case MSDD_STATE_SEND_CSW:
    //----------------------
        // Set signature
        csw->dCSWSignature = MSD_CSW_SIGNATURE;

        // Start the CSW write operation
        status = MSDD_Write(csw,
                            MSD_CSW_SIZE,
                            (TransferCallback) MSDDriver_Callback,
                            (void *) transfer);

        // Check operation result code
        if (status == USBD_STATUS_SUCCESS) {

            TRACE_INFO_WP("SendCSW ");

            // Wait for end of transfer
            pMsdDriver->state = MSDD_STATE_WAIT_CSW;
        }
        break;

    //----------------------
    case MSDD_STATE_WAIT_CSW:
    //----------------------
        // Check transfer semaphore
        if (transfer->semaphore > 0) {

            // Take semaphore and terminate transfer
            transfer->semaphore--;

            // Check if transfer was successful
            if (transfer->status == USBD_STATUS_RESET) {

                TRACE_INFO("MSDD_StateMachine: EP resetted\n\r");
            }
            else if (transfer->status == USBD_STATUS_ABORTED) {

                TRACE_WARNING(
                    "MSDD_StateMachine: Failed to send CSW\n\r");
            }
            else {

                TRACE_INFO_WP("ok");
            }

            // Read new CBW
            pMsdDriver->state = MSDD_STATE_READ_CBW;
        }
        break;

    //----------------------
    case MSDD_STATE_WAIT_HALT:
    //----------------------
        if (MSDD_IsHalted() == 0) {

            pMsdDriver->state = MSDD_STATE_SEND_CSW;
        }
        break;
    }
}
