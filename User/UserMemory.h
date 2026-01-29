
#ifndef __USER_MEMORY_H
#define __USER_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

void UserMemory_Init(void);
void *UserMemory_Malloc(size_t xWantedSize);
void UserMemory_Free(void *pv);

size_t UserMemory_GetFreeHeapSize(void);
size_t UserMemory_GetMiniFreeHeapSize(void);
size_t UserMemory_GetMaxHeapSize(void);
float UserMemory_GetUsedPersent(void);

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

