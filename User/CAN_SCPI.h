#ifndef __CAN_SCPI_H_
#define __CAN_SCPI_H_

#include "scpi/scpi.h"
#include "scpi-def.h"



uint8_t __CAN_Send_MultPack_M2S(uint8_t id, const uint8_t *pdata, uint8_t len);
uint8_t __CAN_Send_MultPack_S2M(uint8_t id, const uint8_t *pdata, uint8_t len);

/*
 * @brief       void CAN_SCPI_Init(void)
 * @details     CAN SCPI≥ı ºªØ
 */
void CAN_SCPI_Init(void);

void CAN_SCPI_Process(const uint8_t *pdata, uint16_t len);
#endif
