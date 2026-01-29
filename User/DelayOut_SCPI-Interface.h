#ifndef _DELAYOUT_SCPI_H_
#define _DELAYOUT_SCPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"

scpi_result_t Cmd_Set_Delay_State(scpi_t *context);
scpi_result_t Cmd_Read_Delay_State(scpi_t *context);

scpi_result_t Cmd_Set_Delay_Start(scpi_t *context);
scpi_result_t Cmd_Read_Delay_Start(scpi_t *context);

scpi_result_t Cmd_Set_Delay_Count(scpi_t *context);
scpi_result_t Cmd_Read_Delay_Count(scpi_t *context);

scpi_result_t Cmd_Set_Delay_Cycle(scpi_t *context);
scpi_result_t Cmd_Read_Delay_Cycle(scpi_t *context);

scpi_result_t Cmd_Set_Delay_endState(scpi_t *context);
scpi_result_t Cmd_Read_Delay_endState(scpi_t *context);

scpi_result_t Cmd_Set_Delay_Stop(scpi_t *context);
scpi_result_t Cmd_Read_Delay_Stop(scpi_t *context);

scpi_result_t Cmd_Set_Delay_Param(scpi_t *context);
scpi_result_t Cmd_Read_Delay_Param(scpi_t *context);


scpi_result_t Cmd_Generate_State(scpi_t *context);
scpi_result_t Cmd_Generate_Fix(scpi_t *context);
scpi_result_t Cmd_Generate_Inc(scpi_t *context);
scpi_result_t Cmd_Generate_Dec(scpi_t *context);
scpi_result_t Cmd_Read_Generate(scpi_t *context);

#endif

