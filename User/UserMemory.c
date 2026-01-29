#include "UserMemory.h"
#include "cmsis_os.h"
/* Architecture specifics. */
#define UserMem_portSTACK_GROWTH            ( -1 )

#define UserMem_portBYTE_ALIGNMENT          8
#define UserMem_portBYTE_ALIGNMENT_MASK ( 0x0007 )
/* USER CODE BEGIN 1 */
///////////////////////////////////////////////////////
#define UserMem_portVECTACTIVE_MASK                 ( 0xFFUL )


/* Block sizes must not get too small. */
#define UserMem_heapMINIMUM_BLOCK_SIZE  ( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define UserMem_heapBITS_PER_BYTE       ( ( size_t ) 8 )

#ifndef configASSERT
    #define configASSERT( x ) if ((x) == 0) {/*taskDISABLE_INTERRUPTS();*/ for( ;; );}
#endif
/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK *pxNextFreeBlock;   /*<< The next free block in the list. */
    size_t xBlockSize;                      /*<< The size of the free block. */
} UserMem_BlockLink_t;

/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList(UserMem_BlockLink_t *pxBlockToInsert);

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize = (sizeof(UserMem_BlockLink_t) + ((size_t)(UserMem_portBYTE_ALIGNMENT - 1))) & ~((size_t) UserMem_portBYTE_ALIGNMENT_MASK);

/* Create a couple of list links to mark the start and end of the list. */
static UserMem_BlockLink_t xStart, *pxEnd = NULL;

/* Keeps track of the number of calls to allocate and free memory as well as the
number of free bytes remaining, but says nothing about fragmentation. */
size_t UserMemory_BytesRemaining = 0U;
size_t UserMemory_MaxBytes = 0U;
size_t UserMemory_MiniFreeBytes = 0U;
size_t UserMemory_Allocations = 0;
size_t UserMemory_Freed = 0;
uint8_t MemoryUsed_Persent = 0;
/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
   member of an BlockLink_t structure is set then the block belongs to the
   application.  When the bit is free the block is still part of the free heap
   space. */
static size_t xBlockAllocatedBit = 0;



typedef struct UserMem_HeapRegion
{
    uint8_t *pucStartAddress;
    size_t xSizeInBytes;
} UserMem_HeapRegion_t;

#define USERMEM_HEAP_SIZE  (14*1024*1024ul)
static uint8_t ucHeap[USERMEM_HEAP_SIZE] __attribute__((at(0xD0200000)));
static UserMem_HeapRegion_t UserMem_HeapRegions[] = {
    { ucHeap, USERMEM_HEAP_SIZE },
    { NULL,   0                     }
};


osMutexId_t UserMem_MutexHandle;
const osMutexAttr_t UserMem_Mutex_attributes = {
    .name = "UserMem_Mutex",
    .attr_bits = osMutexRecursive,
};

#if 1
static uint32_t uxCriticalNesting = 0 ;
static void vPortEnterCritical(void)
{
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    { osMutexAcquire(UserMem_MutexHandle, osWaitForever); }
    //    __disable_irq();
    //    uxCriticalNesting++;
    //    osKernelLock();
}

static void vPortExitCritical(void)
{
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    { osMutexRelease(UserMem_MutexHandle); }
    //    configASSERT(uxCriticalNesting);
    //    uxCriticalNesting--;
    //    if(uxCriticalNesting == 0)
    //    {
    //        __enable_irq();
    //    }
    //    osKernelUnlock();
}
#endif
uint8_t flag_UserMemoryInited = 0;

void *UserMemory_Malloc(size_t xWantedSize)
{
    UserMem_BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
    void *pvReturn = NULL;
    /* The heap must be initialised before the first call to
    prvPortMalloc(). */
    //configASSERT(pxEnd);
    vPortEnterCritical();
    {
        /* Check the requested block size is not so large that the top bit is
        set.  The top bit of the block size member of the BlockLink_t structure
        is used to determine who owns the block - the application or the
        kernel, so it must be free. */
        if((xWantedSize & xBlockAllocatedBit) == 0) {
            /* The wanted size is increased so it can contain a BlockLink_t
            structure in addition to the requested amount of bytes. */
            if(xWantedSize > 0) {
                xWantedSize += xHeapStructSize;
                /* Ensure that blocks are always aligned to the required number
                of bytes. */
                if((xWantedSize & UserMem_portBYTE_ALIGNMENT_MASK) != 0x00) {
                    /* Byte alignment required. */
                    xWantedSize += (UserMem_portBYTE_ALIGNMENT - (xWantedSize & UserMem_portBYTE_ALIGNMENT_MASK));
                } else {
                    //mtCOVERAGE_TEST_MARKER();
                }
            } else {
                //mtCOVERAGE_TEST_MARKER();
            }
            if((xWantedSize > 0) && (xWantedSize <= UserMemory_BytesRemaining)) {
                /* Traverse the list from the start (lowest address) block until
                one of adequate size is found. */
                pxPreviousBlock = &xStart;
                pxBlock = xStart.pxNextFreeBlock;
                while((pxBlock->xBlockSize < xWantedSize) && (pxBlock->pxNextFreeBlock != NULL)) {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                }
                /* If the end marker was reached then a block of adequate size
                was not found. */
                if(pxBlock != pxEnd) {
                    /* Return the memory space pointed to - jumping over the
                    BlockLink_t structure at its start. */
                    pvReturn = (void *)(((uint8_t *) pxPreviousBlock->pxNextFreeBlock) + xHeapStructSize);
                    /* This block is being returned for use so must be taken out
                    of the list of free blocks. */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
                    /* If the block is larger than required it can be split into
                    two. */
                    if((pxBlock->xBlockSize - xWantedSize) > UserMem_heapMINIMUM_BLOCK_SIZE) {
                        /* This block is to be split into two.  Create a new
                        block following the number of bytes requested. The void
                        cast is used to prevent byte alignment warnings from the
                        compiler. */
                        pxNewBlockLink = (void *)(((uint8_t *) pxBlock) + xWantedSize);
                        /* Calculate the sizes of two blocks split from the
                        single block. */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;
                        /* Insert the new block into the list of free blocks. */
                        prvInsertBlockIntoFreeList((pxNewBlockLink));
                    } else {
                        //mtCOVERAGE_TEST_MARKER();
                    }
                    UserMemory_BytesRemaining -= pxBlock->xBlockSize;
                    if(UserMemory_BytesRemaining < UserMemory_MiniFreeBytes) {
                        UserMemory_MiniFreeBytes = UserMemory_BytesRemaining;
                    } else {
                        //mtCOVERAGE_TEST_MARKER();
                    }
                    /* The block is being returned - it is allocated and owned
                    by the application and has no "next" block. */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;
                    pxBlock->pxNextFreeBlock = NULL;
                    UserMemory_Allocations++;
                } else {
                    //mtCOVERAGE_TEST_MARKER();
                }
            } else {
                //mtCOVERAGE_TEST_MARKER();
            }
        } else {
            //mtCOVERAGE_TEST_MARKER();
        }
        //traceMALLOC(pvReturn, xWantedSize);
    }
    (void) vPortExitCritical();
    if(pvReturn == NULL) {
        //ÄÚ´æÉêÇëÊ§°Ü
    } else {
        //mtCOVERAGE_TEST_MARKER();
    }
    MemoryUsed_Persent = UserMemory_GetUsedPersent();
    return pvReturn;
}
/*-----------------------------------------------------------*/

void UserMemory_Free(void *pv)
{
    uint8_t *puc = (uint8_t *) pv;
    UserMem_BlockLink_t *pxLink;
    if(pv != NULL)
    {
        /* The memory being freed will have an BlockLink_t structure immediately
        before it. */
        puc -= xHeapStructSize;
        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = (void *) puc;
        /* Check the block is actually allocated. */
        configASSERT((pxLink->xBlockSize & xBlockAllocatedBit) != 0);
        configASSERT(pxLink->pxNextFreeBlock == NULL);
        if((pxLink->xBlockSize & xBlockAllocatedBit) != 0) {
            if(pxLink->pxNextFreeBlock == NULL) {
                /* The block is being returned to the heap - it is no longer
                allocated. */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;
                vPortEnterCritical();
                {
                    /* Add this block to the list of free blocks. */
                    UserMemory_BytesRemaining += pxLink->xBlockSize;
                    //traceFREE(pv, pxLink->xBlockSize);
                    prvInsertBlockIntoFreeList(((UserMem_BlockLink_t *) pxLink));
                    UserMemory_Freed++;
                }
                (void) vPortExitCritical();
            } else {
                //mtCOVERAGE_TEST_MARKER();
            }
        } else {
            //mtCOVERAGE_TEST_MARKER();
        }
    }
}

/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList(UserMem_BlockLink_t *pxBlockToInsert)
{
    UserMem_BlockLink_t *pxIterator;
    uint8_t *puc;
    /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
    for(pxIterator = &xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock) {
        /* Nothing to do here, just iterate to the right position. */
    }
    /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
    puc = (uint8_t *) pxIterator;
    if((puc + pxIterator->xBlockSize) == (uint8_t *) pxBlockToInsert) {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    } else {
        //mtCOVERAGE_TEST_MARKER();
    }
    /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
    puc = (uint8_t *) pxBlockToInsert;
    if((puc + pxBlockToInsert->xBlockSize) == (uint8_t *) pxIterator->pxNextFreeBlock) {
        if(pxIterator->pxNextFreeBlock != pxEnd) {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        } else {
            pxBlockToInsert->pxNextFreeBlock = pxEnd;
        }
    } else {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }
    /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
    if(pxIterator != pxBlockToInsert) {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    } else {
        //mtCOVERAGE_TEST_MARKER();
    }
}
/*-----------------------------------------------------------*/








void UserMemory_Init(void)
{
    UserMem_MutexHandle = osMutexNew(&UserMem_Mutex_attributes);
    const UserMem_HeapRegion_t *const pxHeapRegions = UserMem_HeapRegions;
    //
    UserMem_BlockLink_t *pxFirstFreeBlockInRegion = NULL, *pxPreviousFreeBlock;
    size_t xAlignedHeap;
    size_t xTotalRegionSize, xTotalHeapSize = 0;
    int32_t xDefinedRegions = 0;
    size_t xAddress;
    const UserMem_HeapRegion_t *pxHeapRegion;
    /* Can only call once! */
    configASSERT(pxEnd == NULL);
    pxHeapRegion = &(pxHeapRegions[ xDefinedRegions ]);
    while(pxHeapRegion->xSizeInBytes > 0)
    {
        xTotalRegionSize = pxHeapRegion->xSizeInBytes;
        /* Ensure the heap region starts on a correctly aligned boundary. */
        xAddress = (size_t) pxHeapRegion->pucStartAddress;
        if((xAddress & UserMem_portBYTE_ALIGNMENT_MASK) != 0) {
            xAddress += (UserMem_portBYTE_ALIGNMENT - 1);
            xAddress &= ~UserMem_portBYTE_ALIGNMENT_MASK;
            /* Adjust the size for the bytes lost to alignment. */
            xTotalRegionSize -= xAddress - (size_t) pxHeapRegion->pucStartAddress;
        }
        xAlignedHeap = xAddress;
        /* Set xStart if it has not already been set. */
        if(xDefinedRegions == 0) {
            /* xStart is used to hold a pointer to the first item in the list of
            free blocks.  The void cast is used to prevent compiler warnings. */
            xStart.pxNextFreeBlock = (UserMem_BlockLink_t *) xAlignedHeap;
            xStart.xBlockSize = (size_t) 0;
        } else {
            /* Should only get here if one region has already been added to the
            heap. */
            configASSERT(pxEnd != NULL);
            /* Check blocks are passed in with increasing start addresses. */
            configASSERT(xAddress > (size_t) pxEnd);
        }
        /* Remember the location of the end marker in the previous region, if
        any. */
        pxPreviousFreeBlock = pxEnd;
        /* pxEnd is used to mark the end of the list of free blocks and is
        inserted at the end of the region space. */
        xAddress = xAlignedHeap + xTotalRegionSize;
        xAddress -= xHeapStructSize;
        xAddress &= ~UserMem_portBYTE_ALIGNMENT_MASK;
        pxEnd = (UserMem_BlockLink_t *) xAddress;
        pxEnd->xBlockSize = 0;
        pxEnd->pxNextFreeBlock = NULL;
        /* To start with there is a single free block in this region that is
        sized to take up the entire heap region minus the space taken by the
        free block structure. */
        pxFirstFreeBlockInRegion = (UserMem_BlockLink_t *) xAlignedHeap;
        pxFirstFreeBlockInRegion->xBlockSize = xAddress - (size_t) pxFirstFreeBlockInRegion;
        pxFirstFreeBlockInRegion->pxNextFreeBlock = pxEnd;
        /* If this is not the first region that makes up the entire heap space
        then link the previous region to this region. */
        if(pxPreviousFreeBlock != NULL) {
            pxPreviousFreeBlock->pxNextFreeBlock = pxFirstFreeBlockInRegion;
        }
        xTotalHeapSize += pxFirstFreeBlockInRegion->xBlockSize;
        /* Move onto the next HeapRegion_t structure. */
        xDefinedRegions++;
        pxHeapRegion = &(pxHeapRegions[ xDefinedRegions ]);
    }
    UserMemory_MaxBytes = xTotalHeapSize;
    UserMemory_MiniFreeBytes = xTotalHeapSize;
    UserMemory_BytesRemaining = xTotalHeapSize;
    /* Check something was actually defined before it is accessed. */
    configASSERT(xTotalHeapSize);
    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * UserMem_heapBITS_PER_BYTE) - 1);
}
/*-----------------------------------------------------------*/





/*-----------------------------------------------------------*/

size_t UserMemory_GetFreeHeapSize(void) {
    return UserMemory_BytesRemaining;
}
size_t UserMemory_GetMiniFreeHeapSize(void) {
    return UserMemory_MiniFreeBytes;
}
size_t UserMemory_GetMaxHeapSize(void) {
    return UserMemory_MaxBytes;
}
float UserMemory_GetUsedPersent(void) {
    return ((float)UserMemory_BytesRemaining / (float)UserMemory_MaxBytes) * 100.0f;
}

