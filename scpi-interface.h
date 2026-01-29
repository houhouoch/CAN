#ifndef __SCPI_INTERFACE_H_
#define __SCPI_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"


//‘§…Ë÷µ
scpi_result_t Cmd_Preset_Apply(scpi_t *context);
scpi_result_t Cmd_Set_Preset_Volt(scpi_t *context);
scpi_result_t Cmd_Read_Preset_Volt(scpi_t *context);
//
scpi_result_t Cmd_Set_Preset_Curr(scpi_t *context);
scpi_result_t Cmd_Read_Preset_Curr(scpi_t *context);

scpi_result_t Cmd_Set_Preset_OVP(scpi_t *context);
scpi_result_t Cmd_Read_Preset_OVP(scpi_t *context);
scpi_result_t Cmd_Set_Preset_OCP(scpi_t *context);
scpi_result_t Cmd_Read_Preset_OCP(scpi_t *context);



scpi_result_t SCPI_Cmd_Meas_Parallel_Voltage(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Parallel_Current(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Parallel_Power(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Parallel_AllAsk(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Series_Voltage(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Series_Current(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Series_Power(scpi_t *context);
scpi_result_t SCPI_Cmd_Meas_Series_AllAsk(scpi_t *context);

scpi_result_t SCPI_Cmd_MeasVoltage(scpi_t *context);
scpi_result_t SCPI_Cmd_MeasCurrent(scpi_t *context);
scpi_result_t SCPI_Cmd_MeasPower(scpi_t *context);
scpi_result_t SCPI_Cmd_MeasDVM(scpi_t *context);            //
scpi_result_t SCPI_Cmd_MeasAllAsk(scpi_t *context);

scpi_result_t Cmd_Set_Voltage(scpi_t *context);
scpi_result_t Cmd_Read_Voltage(scpi_t *context);
scpi_result_t Cmd_Set_OVP(scpi_t *context);
scpi_result_t Cmd_Read_OVP(scpi_t *context);
scpi_result_t Cmd_Set_OVP_State(scpi_t *context);
scpi_result_t Cmd_Read_OVP_State(scpi_t *context);
scpi_result_t Cmd_Read_OVP_State_TRIPed(scpi_t *context);
scpi_result_t Cmd_Clear_OVP_State_TRIPed(scpi_t *context);

scpi_result_t Cmd_Set_Current(scpi_t *context);
scpi_result_t Cmd_Read_Current(scpi_t *context);
scpi_result_t Cmd_Set_OCP(scpi_t *context);
scpi_result_t Cmd_Read_OCP(scpi_t *context);
scpi_result_t Cmd_Set_OCP_State(scpi_t *context);
scpi_result_t Cmd_Read_OCP_State(scpi_t *context);
scpi_result_t Cmd_Read_OCP_State_TRIPed(scpi_t *context);
scpi_result_t Cmd_Clear_OCP_State_TRIPed(scpi_t *context);

scpi_result_t Cmd_Set_Onoff(scpi_t *context);
scpi_result_t Cmd_Read_OnOff(scpi_t *context);
scpi_result_t Cmd_Read_CCCV(scpi_t *context) ;


scpi_result_t Cmd_Set_Voltage_Step(scpi_t *context);
scpi_result_t Cmd_Read_Voltage_Step(scpi_t *context);
scpi_result_t Cmd_Voltage_Step_Up(scpi_t *context);
scpi_result_t Cmd_Voltage_Step_Down(scpi_t *context);

scpi_result_t Cmd_Set_Current_Step(scpi_t *context);
scpi_result_t Cmd_Read_Current_Step(scpi_t *context);
scpi_result_t Cmd_Current_Step_Up(scpi_t *context);
scpi_result_t Cmd_Current_Step_Down(scpi_t *context);

scpi_result_t Cmd_Set_Resistance(scpi_t *context);
scpi_result_t Cmd_Read_Resistance(scpi_t *context);

scpi_result_t Cmd_Set_SR_Mode(scpi_t *context);
scpi_result_t Cmd_Read_SR_Mode(scpi_t *context);
scpi_result_t Cmd_Set_Voltage_Rising(scpi_t *context);
scpi_result_t Cmd_Read_Voltage_Rising(scpi_t *context);
scpi_result_t Cmd_Set_Voltage_Falling(scpi_t *context);
scpi_result_t Cmd_Read_Voltage_Falling(scpi_t *context);
scpi_result_t Cmd_Set_Current_Rising(scpi_t *context);
scpi_result_t Cmd_Read_Current_Rising(scpi_t *context);
scpi_result_t Cmd_Set_Current_Falling(scpi_t *context);
scpi_result_t Cmd_Read_Current_Falling(scpi_t *context);

scpi_result_t Cmd_Calib_Voltage(scpi_t *context);
scpi_result_t Cmd_Calib_Current(scpi_t *context);
scpi_result_t Cmd_Calib_Delete(scpi_t *context);
scpi_result_t Cmd_Calib_ZeroCurrent(scpi_t *context);

#endif
