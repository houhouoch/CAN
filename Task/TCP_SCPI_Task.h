#ifndef _TCP_SCPI_TASK_H_
#define _TCP_SCPI_TASK_H_

#include "Commd.h"
#include "Task_Manager.h"

#include "scpi/scpi.h"
#include "scpi-def.h"

/*
 * @brief       osThreadId_t TCP_SCPI_Task_Create(void)
 * @details     创建SCPI TCP SOCKET任务
 */
osThreadId_t TCP_SCPI_Task_Create(void);

// SCPI连接结构体
typedef struct {
    int socket;
    osThreadId_t thread_id;
    scpi_t scpi_context;
    uint8_t is_active;
} SCPI_Connection_t;

// 最大连接数
#define MAX_SCPI_CONNECTIONS 4
#endif
