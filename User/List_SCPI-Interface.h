#ifndef _LISTOUT_SCPI_INTERFACE_H_
#define _LISTOUT_SCPI_INTERFACE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"


//List
scpi_result_t Cmd_Set_List_State(scpi_t *context);
scpi_result_t Cmd_Read_List_State(scpi_t *context);

scpi_result_t Cmd_Set_List_Base(scpi_t *context);
scpi_result_t Cmd_Read_List_Base(scpi_t *context);

scpi_result_t Cmd_Set_List_PointParam(scpi_t *context);
scpi_result_t Cmd_Read_List_PointParam(scpi_t *context);


//Delayer
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

#endif
