#ifndef _USB_TMC_TASK_H_
#define _USB_TMC_TASK_H_

#include "Commd.h"
#include "Task_Manager.h"

#include "usbd_cdc_if.h"
#include "usb_device_tmc.h"

/*
 * @brief       osThreadId_t USB_TMC_Task_Create(void)
 * @details     创建USB TMC任务
 */
osThreadId_t USB_TMC_Task_Create(void);

#endif
