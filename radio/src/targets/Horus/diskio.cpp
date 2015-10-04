/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include <string.h> // memcpy
#include "stm32f4xx.h"
#include "sdio_sd.h"

#define BLOCK_SIZE            512 /* Block Size in Bytes */

/*-----------------------------------------------------------------------*/
/* Lock / unlock functions                                               */
/*-----------------------------------------------------------------------*/
#if !defined(BOOT)
static OS_MutexID ioMutex;
volatile int mutexCheck = 0;

int ff_cre_syncobj (BYTE vol, _SYNC_t *mutex)
{
  *mutex = ioMutex;
  return 1;
}

int ff_req_grant (_SYNC_t mutex)
{
  return CoEnterMutexSection(mutex) == E_OK;
}

void ff_rel_grant (_SYNC_t mutex)
{
  CoLeaveMutexSection(mutex);
}

int ff_del_syncobj (_SYNC_t mutex)
{
  return 1;
}
#endif


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
        BYTE drv                                /* Physical drive nmuber (0..) */
)
{
  DSTATUS stat = 0;

  /* Supports only single drive */
  if (drv)
  {
    stat |= STA_NOINIT;
  }
	
	/*-------------------------- SD Init ----------------------------- */
  if (SD_Init() != SD_OK)
  {
    TRACE("SD_Init() failed");
    stat |= STA_NOINIT;
  }

  return(stat);
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
        BYTE drv                /* Physical drive nmuber (0..) */
)
{
  DSTATUS stat = 0;
	
  if (SD_Detect() != SD_PRESENT)
    stat |= STA_NODISK;

  // STA_NOTINIT - Subsystem not initailized
  // STA_PROTECTED - Write protected, MMC/SD switch if available
	
  return(stat);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
        BYTE drv,               /* Physical drive nmuber (0..) */
        BYTE *buff,             /* Data buffer to store read data */
        DWORD sector,   				/* Sector address (LBA) */
        UINT count              /* Number of sectors to read (1..255) */
)
{
	SD_Error Status;

	// TRACE("disk_read %d %p %10d %d\n",drv,buff,sector,count);
	
	if (SD_Detect() != SD_PRESENT)
		return(RES_NOTRDY);

	if ((DWORD)buff & 3) // DMA Alignment failure, do single up to aligned buffer
	{
		DRESULT res = RES_OK;
		DWORD scratch[BLOCK_SIZE / 4]; // Alignment assured, you'll need a sufficiently big stack

		while(count--)
		{
			res = disk_read(drv, (BYTE *)scratch, sector++, 1);

			if (res != RES_OK) {
			  TRACE("disk_read() status=%d", res);
			  break;
			}

			memcpy(buff, scratch, BLOCK_SIZE);

			buff += BLOCK_SIZE;
		}

		return(res);
	}

  Status = SD_ReadMultiBlocksFIXED(buff, sector, BLOCK_SIZE, count); // 4GB Compliant

	if (Status == SD_OK)
	{
		SDTransferState State;

		Status = SD_WaitReadOperation(); // Check if the Transfer is finished

		while((State = SD_GetStatus()) == SD_TRANSFER_BUSY); // BUSY, OK (DONE), ERROR (FAIL)

		if ((State == SD_TRANSFER_ERROR) || (Status != SD_OK)) {
		  TRACE("SD_ReadMultiBlocksFIXED() wait error (State=%d Status=%d)", State, Status);
	          return(RES_ERROR);
		}
		else
			return(RES_OK);
	}
	else {
          TRACE("SD_ReadMultiBlocksFIXED() error (Status=%d)", Status);
	  return(RES_ERROR);
	}
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
        BYTE drv,                       /* Physical drive nmuber (0..) */
        const BYTE *buff,       /* Data to be written */
        DWORD sector,           /* Sector address (LBA) */
        UINT count                      /* Number of sectors to write (1..255) */
)
{
	SD_Error Status;

	// TRACE("disk_write %d %p %10d %d\n",drv,buff,sector,count);
	
	if (SD_Detect() != SD_PRESENT)
		return(RES_NOTRDY);

	if ((DWORD)buff & 3) // DMA Alignment failure, do single up to aligned buffer
	{
		DRESULT res = RES_OK;
		DWORD scratch[BLOCK_SIZE / 4]; // Alignment assured, you'll need a sufficiently big stack

		while(count--)
		{
			memcpy(scratch, buff, BLOCK_SIZE);

			res = disk_write(drv, (BYTE *)scratch, sector++, 1);

			if (res != RES_OK)
				break;

			buff += BLOCK_SIZE;
		}

		return(res);
	}

  Status = SD_WriteMultiBlocksFIXED((uint8_t *)buff, sector, BLOCK_SIZE, count); // 4GB Compliant

	if (Status == SD_OK)
	{
		SDTransferState State;

		Status = SD_WaitWriteOperation(); // Check if the Transfer is finished

		while((State = SD_GetStatus()) == SD_TRANSFER_BUSY); // BUSY, OK (DONE), ERROR (FAIL)

		if ((State == SD_TRANSFER_ERROR) || (Status != SD_OK))
			return(RES_ERROR);
		else
			return(RES_OK);
	}
	else
		return(RES_ERROR);
}
#endif /* _READONLY */




/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
        BYTE drv,               /* Physical drive nmuber (0..) */
        BYTE ctrl,              /* Control code */
        void *buff              /* Buffer to send/receive control data */
)
{
        return RES_OK;
}

// TODO everything here should not be in the driver layer ...

FATFS g_FATFS_Obj;
#if defined(SPORT_FILE_LOG)
FIL g_telemetryFile = {0};
#endif

#if defined(BOOT)
void sdInit(void)
{
  if (f_mount(&g_FATFS_Obj, "", 1) == FR_OK) {
    f_chdir("/");
  }
}
#else
// TODO shouldn't be there!
void sdInit(void)
{
  ioMutex = CoCreateMutex();
  if (ioMutex >= CFG_MAX_MUTEX ) {
    // sd error
    return;
  }

  TRACE("AVANT F_MOUNT");
  if (f_mount(&g_FATFS_Obj, "", 1) == FR_OK) {
    TRACE("APRES F_MOUNT");
    // call sdGetFreeSectors() now because f_getfree() takes a long time first time it's called
    sdGetFreeSectors();

    referenceSystemAudioFiles();

#if defined(SPORT_FILE_LOG)
    f_open(&g_telemetryFile, LOGS_PATH "/sport.log", FA_OPEN_ALWAYS | FA_WRITE);
    if (f_size(&g_telemetryFile) > 0) {
      f_lseek(&g_telemetryFile, f_size(&g_telemetryFile)); // append
    }
#endif
  }
  else {
    TRACE("f_mount() failed");
  }
}

void sdDone()
{
  if (sdMounted()) {
    audioQueue.stopSD();
#if defined(SPORT_FILE_LOG)
    f_close(&g_telemetryFile);
#endif
    f_mount(NULL, "", 0); // unmount SD
  }
}
#endif

uint32_t sdMounted()
{
  return g_FATFS_Obj.fs_type != 0;
}

uint32_t sdIsHC()
{
  return true; // TODO (CardType & CT_BLOCK);
}

uint32_t sdGetSpeed()
{
  return 330000;
}
