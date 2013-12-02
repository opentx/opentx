/**
 *******************************************************************************
 * @file       OsKernelHeap.h
 * @version   V1.1.4    
 * @date      2011.04.20
 * @brief      Header file related to memory management	
 * @details    This file including some defines and function declare related to 
 *             kernel heap management. 
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef  _KERNELHEAP_H
#define  _KERNELHEAP_H


typedef struct KennelHeap
{
  U32   startAddr;
  U32   endAddr;
}KHeap,*P_KHeap;


typedef struct UsedMemBlk
{
  void* nextMB;
  void* preMB;	
}UMB,*P_UMB;


typedef struct FreeMemBlk
{
  struct FreeMemBlk* nextFMB;
  struct UsedMemBlk* nextUMB;
  struct UsedMemBlk* preUMB;
}FMB,*P_FMB;

/*---------------------------- Function Declare ------------------------------*/
extern void   CoCreateKheap(void);

#endif  /* _KERNELHEAP_H */

