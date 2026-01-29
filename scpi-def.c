/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   scpi-def.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser test
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"



/**
 * Reimplement IEEE488.2 *TST?
 *
 * Result should be 0 if everything is ok
 * Result should be 1 if something goes wrong
 *
 * Return SCPI_RES_OK
 */
static scpi_result_t My_CoreTstQ(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    SCPI_ResultInt32(context, 0);
    if((PowerInfo.State.bits.noCalib_Volt != 0)
            || (PowerInfo.State.bits.noCalib_Curr != 0)
            || (PowerInfo.State.bits.noCalib_Drv != 0)
            || (PowerInfo.State.bits.acFault != 0))
    {
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}


scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    (void) context;
    //SCPI_ResultInt32(context, 5026);
    return SCPI_RES_ERR;
}



/*
 * @brief       void SCPI_Core_Reg_Update(scpi_t *context)
 * @details     更新SCPI寄存器
 */
#include "Setting.h"
void SCPI_Core_Reg_Update(scpi_t *context)
{
    //更新QUESC寄存器
    {
        int32_t scpi_reg_quesc = 0;
        if(IF_Power_Get_On_Off_Now() != 0) {
            //CV模式(0)
            scpi_reg_quesc |= ((PowerInfo.State.bits.cv != 0) << 0);
            //CC模式(1)
            scpi_reg_quesc |= ((PowerInfo.State.bits.cc != 0) << 1);
        }
        //TEMPERATURE(2) 散热风扇故障
        scpi_reg_quesc |= ((PowerInfo.State.bits.fan_error != 0) << 2);
        //TEMPERATURE(4) 过温保护
        scpi_reg_quesc |= (PowerInfo.State.bits.otp << 4);
        //PFC Hot(5) PFC过热
        scpi_reg_quesc |= ((PowerInfo.State.bits.pfc_hot != 0) << 5);
        //Mos Hot(6) MOS过热
        scpi_reg_quesc |= ((PowerInfo.State.bits.mos_hot != 0) << 6);
        //OVER POWER(7) 过功率保护
        scpi_reg_quesc |= (PowerInfo.State.bits.opp << 7);
        //OVER SENSE(8) 线损补偿过大
        scpi_reg_quesc |= (PowerInfo.State.bits.sense << 8);
        //OVER VOLTAGE(9) 过压保护
        scpi_reg_quesc |= (PowerInfo.State.bits.ovp << 9);
        //OVER CURRENT(10)过流保护
        scpi_reg_quesc |= (PowerInfo.State.bits.ocp << 10);
        //OVER FONT CURRENT(11) 前输出电流过大
        scpi_reg_quesc |= (PowerInfo.State.bits.font_ocp << 11);
        //Voltage_Calib_NULL(12) 无电压校准点
        scpi_reg_quesc |= (PowerInfo.State.bits.noCalib_Volt << 12);
        scpi_reg_quesc |= (PowerInfo.State.bits.noCalib_Drv << 12);
        //Current_Calib_NULL(13) 无电流校准点
        scpi_reg_quesc |= (PowerInfo.State.bits.noCalib_Curr << 13);
        SCPI_RegSet(context, SCPI_REG_QUESC, scpi_reg_quesc);
    }
    //更新SCPI_REG_OPERC
    {
        int32_t scpi_reg_quesc = 0;
        //校准模式
        scpi_reg_quesc |= (PowerInfo.workMode == WORK_MODE_ENGINEERING) << 0;
        //电源输出状态
        scpi_reg_quesc |= (IF_Power_Get_On_Off_Now() != 0) << 1;
        SCPI_RegSet(context, SCPI_REG_OPERC, scpi_reg_quesc);
    }
}

/*
 * @brief       static scpi_result_t scpi_stub_callback(scpi_t *context){
 * @details     常态标志位(实时显示电源状态)
 */
static scpi_result_t scpi_stub_callback(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //SCPI返回
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_QUESC));
    return SCPI_RES_OK;
}

#include "cmsis_os.h"
static scpi_result_t Cmd_Read_MEM(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    uint32_t freeHeap = xPortGetMinimumEverFreeHeapSize();
    //
    char buffer[64] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d Bytes\r\n", freeHeap);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


scpi_result_t SCPI_CoreIdnQ_MultCh_Master(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    int i;
    context->idn[1] = PowerInfo.about.model_name;
    for(i = 0; i < 4; i++) {
        if(context->idn[i]) {
            SCPI_ResultMnemonic(context, context->idn[i]);
        } else {
            SCPI_ResultMnemonic(context, "0");
        }
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_CoreIdnQ_MultCh(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    int i;
    static char temp_modelName[128] = {0};
    snprintf(temp_modelName, sizeof(temp_modelName) - 1, "%s_%d", PowerInfo.about.model_name, IF_Power_Get_Hw_ID());
    context->idn[1] = temp_modelName;
    for(i = 0; i < 4; i++) {
        if(context->idn[i]) {
            SCPI_ResultMnemonic(context, context->idn[i]);
        } else {
            SCPI_ResultMnemonic(context, "0");
        }
    }
    return SCPI_RES_OK;
}


scpi_result_t Cmd_Read_ErrorFlag(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    uint32_t errorFlag = IF_Debug_Get_ErrorFlag();
    //
    char buffer[64] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%#X\r\n", errorFlag);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


#include "scpi-interface.h"
#include "List_SCPI-interface.h"
#include "BatTester_SCPI-Interface.h"
#include "scpi-option_interface.h"
const scpi_command_t scpi_commands[] = {



    #if MULT_CHANNEL

    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    {.pattern = "*CLS", .callback = SCPI_CoreCls,},
    //
    {.pattern = "*ESE", .callback = SCPI_CoreEse,},
    {.pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    {.pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    //
    {.pattern = "*IDN?", .callback = SCPI_CoreIdnQ_MultCh_Master,},
    {.pattern = "*OPC", .callback = SCPI_CoreOpc,},
    {.pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    {.pattern = "*RST", .callback = SCPI_CoreRst,},
    {.pattern = "*SRE", .callback = SCPI_CoreSre,},
    {.pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    {.pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    {.pattern = "*TST?", .callback = My_CoreTstQ,},        //自检测试
    {.pattern = "*WAI", .callback = SCPI_CoreWai,},
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    {.pattern = "[:ADDR#]:CLS", .callback = SCPI_CoreCls,},
    //
    {.pattern = "[:ADDR#]:ESE", .callback = SCPI_CoreEse,},
    {.pattern = "[:ADDR#]:ESE?", .callback = SCPI_CoreEseQ,},
    {.pattern = "[:ADDR#]:ESR?", .callback = SCPI_CoreEsrQ,},
    //
    {.pattern = "[:ADDR#]:IDN?", .callback = SCPI_CoreIdnQ_MultCh,},
    {.pattern = "[:ADDR#]:OPC", .callback = SCPI_CoreOpc,},
    {.pattern = "[:ADDR#]:OPC?", .callback = SCPI_CoreOpcQ,},
    {.pattern = "[:ADDR#]:RST", .callback = SCPI_CoreRst,},
    {.pattern = "[:ADDR#]:SRE", .callback = SCPI_CoreSre,},
    {.pattern = "[:ADDR#]:SRE?", .callback = SCPI_CoreSreQ,},
    {.pattern = "[:ADDR#]:STB?", .callback = SCPI_CoreStbQ,},
    {.pattern = "[:ADDR#]:TST?", .callback = My_CoreTstQ,},        //自检测试
    {.pattern = "[:ADDR#]:WAI", .callback = SCPI_CoreWai,},

    #else
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    {.pattern = "*CLS", .callback = SCPI_CoreCls,},
    //
    {.pattern = "*ESE", .callback = SCPI_CoreEse,},
    {.pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    {.pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    //
    {.pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    {.pattern = "*OPC", .callback = SCPI_CoreOpc,},
    {.pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    {.pattern = "*RST", .callback = SCPI_CoreRst,},
    {.pattern = "*SRE", .callback = SCPI_CoreSre,},
    {.pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    {.pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    {.pattern = "*TST?", .callback = My_CoreTstQ,},        //自检测试
    {.pattern = "*WAI", .callback = SCPI_CoreWai,},
    #endif


    #if 1

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    //错误查询
    {.pattern = "[:ADDR#]:SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "[:ADDR#]:SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    //    {.pattern = "[:ADDR#]:SYSTem:COMMunication:TCPIP:CONTROL?", .callback = SCPI_SystemCommTcpipControlQ,},
    {.pattern = "[:ADDR#]:SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},
    //
    {.pattern = "[:ADDR#]:STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    {.pattern = "[:ADDR#]:STATus:QUEStionable:CONDition?", .callback = scpi_stub_callback,},
    {.pattern = "[:ADDR#]:STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "[:ADDR#]:STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},
    #if 0
    //
    {.pattern = "[:ADDR#]:STATus:OPERation:CONDition?", .callback = SCPI_StatusOperationConditionQ,},
    {.pattern = "[:ADDR#]:STATus:OPERation[:EVENt]?", .callback = SCPI_StatusOperationEventQ,},
    {.pattern = "[:ADDR#]:STATus:OPERation:ENABle?", .callback = SCPI_StatusOperationEnableQ,},
    {.pattern = "[:ADDR#]:STATus:OPERation:ENABle", .callback = SCPI_StatusOperationEnable,},

    {.pattern = "[:ADDR#]:STATus:PRESet", .callback = SCPI_StatusPreset,},
    #endif

    #endif


    //测量指令
    {.pattern = "[:ADDR#]:MEASure:VOLTage?",   .callback = SCPI_Cmd_MeasVoltage},
    {.pattern = "[:ADDR#]:MEASure:CURRent?",   .callback = SCPI_Cmd_MeasCurrent},
    {.pattern = "[:ADDR#]:MEASure:POWEr?",     .callback = SCPI_Cmd_MeasPower},
    //    {.pattern = "[:ADDR#]:MEASure:DVM?",       .callback = SCPI_Cmd_MeasDVM},                 //---------------
    {.pattern = "[:ADDR#]:MEASure:ALL?",       .callback = SCPI_Cmd_MeasAllAsk},

    //串/并联指令
    #if 1
    {.pattern = "[:ADDR#]:MEASure:PARALLEL:VOLTage?",  .callback = SCPI_Cmd_Meas_Parallel_Voltage},
    {.pattern = "[:ADDR#]:MEASure:PARALLEL:CURRent?",  .callback = SCPI_Cmd_Meas_Parallel_Current},
    {.pattern = "[:ADDR#]:MEASure:PARALLEL:POWEr?",    .callback = SCPI_Cmd_Meas_Parallel_Power},
    {.pattern = "[:ADDR#]:MEASure:PARALLEL:ALL?",      .callback = SCPI_Cmd_Meas_Parallel_AllAsk},

    {.pattern = "[:ADDR#]:MEASure:SERIES:VOLTage?",    .callback = SCPI_Cmd_Meas_Series_Voltage},
    {.pattern = "[:ADDR#]:MEASure:SERIES:CURRent?",    .callback = SCPI_Cmd_Meas_Series_Current},
    {.pattern = "[:ADDR#]:MEASure:SERIES:POWEr?",      .callback = SCPI_Cmd_Meas_Series_Power},
    {.pattern = "[:ADDR#]:MEASure:SERIES:ALL?",        .callback = SCPI_Cmd_Meas_Series_AllAsk},
    #endif


    #if 1
    //电压
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:STEP",      .callback = Cmd_Set_Voltage_Step},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:STEP?",     .callback = Cmd_Read_Voltage_Step},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:UP",        .callback = Cmd_Voltage_Step_Up},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:DOWN",      .callback = Cmd_Voltage_Step_Down},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage[:LEVel][:IMMediate][:AMPLitude]",     .callback = Cmd_Set_Voltage},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage[:LEVel][:IMMediate][:AMPLitude]?",    .callback = Cmd_Read_Voltage},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:PROTection[:LEVel]",      .callback = Cmd_Set_OVP},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:PROTection[:LEVel]?",     .callback = Cmd_Read_OVP},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:PROTection:STATe",        .callback = Cmd_Set_OVP_State},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:PROTection:STATe?",       .callback = Cmd_Read_OVP_State},
    //OVP保护状态触发与状态清除
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:PROTection:TRIPed?",   .callback = Cmd_Read_OVP_State_TRIPed},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:PROTection:CLEar",     .callback = Cmd_Clear_OVP_State_TRIPed},
    //电压斜率
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:SLEW:RISing",         .callback = Cmd_Set_Voltage_Rising},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:SLEW:RISing?",        .callback = Cmd_Read_Voltage_Rising},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:SLEW:FALLing",        .callback = Cmd_Set_Voltage_Falling},
    {.pattern = "[:ADDR#][:SOURce]:VOLTage:SLEW:FALLing?",       .callback = Cmd_Read_Voltage_Falling},
    #endif


    #if 1
    //电流
    {.pattern = "[:ADDR#][:SOURce]:CURRent:STEP",      .callback = Cmd_Set_Current_Step},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:STEP?",     .callback = Cmd_Read_Current_Step},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:UP",        .callback = Cmd_Current_Step_Up},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:DOWN",      .callback = Cmd_Current_Step_Down},
    {.pattern = "[:ADDR#][:SOURce]:CURRent[:LEVel][:IMMediate][:AMPLitude]",     .callback = Cmd_Set_Current},
    {.pattern = "[:ADDR#][:SOURce]:CURRent[:LEVel][:IMMediate][:AMPLitude]?",    .callback = Cmd_Read_Current},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:PROTection[:LEVel]",      .callback = Cmd_Set_OCP},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:PROTection[:LEVel]?",     .callback = Cmd_Read_OCP},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:PROTection:STATe",        .callback = Cmd_Set_OCP_State},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:PROTection:STATe?",       .callback = Cmd_Read_OCP_State},
    //OCP保护状态触发与状态清除
    {.pattern = "[:ADDR#][:SOURce]:CURRent:PROTection:TRIPed?",   .callback = Cmd_Read_OCP_State_TRIPed},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:PROTection:CLEar",     .callback = Cmd_Clear_OCP_State_TRIPed},
    //电流斜率
    {.pattern = "[:ADDR#][:SOURce]:CURRent:SLEW:RISing",         .callback = Cmd_Set_Current_Rising},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:SLEW:RISing?",        .callback = Cmd_Read_Current_Rising},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:SLEW:FALLing",        .callback = Cmd_Set_Current_Falling},
    {.pattern = "[:ADDR#][:SOURce]:CURRent:SLEW:FALLing?",       .callback = Cmd_Read_Current_Falling},
    #endif

    #if 1
    //输出内阻
    {.pattern = "[:ADDR#][:SOURce]:RESistance[:LEVel][:IMMediate][:AMPLitude]",    .callback = Cmd_Set_Resistance},
    {.pattern = "[:ADDR#][:SOURce]:RESistance[:LEVel][:IMMediate][:AMPLitude]?",   .callback = Cmd_Read_Resistance},
    #endif

    #if 1
    //输出设置
    {.pattern = "[:ADDR#]:OUTPut[:STATe]",           .callback = Cmd_Set_Onoff},
    {.pattern = "[:ADDR#]:OUTPut[:STATe]?",          .callback = Cmd_Read_OnOff},
    {.pattern = "[:ADDR#]:OUTPut:CVCC?",             .callback = Cmd_Read_CCCV},
    {.pattern = "[:ADDR#]:OUTPut:MODE",              .callback = Cmd_Set_SR_Mode},
    {.pattern = "[:ADDR#]:OUTPut:MODE?",             .callback = Cmd_Read_SR_Mode},

    //电压
    {.pattern = "[:ADDR#]:OUTPut:OVP:VALue",         .callback = Cmd_Set_OVP},
    {.pattern = "[:ADDR#]:OUTPut:OVP:VALue?",        .callback = Cmd_Read_OVP},
    {.pattern = "[:ADDR#]:OUTPut:OVP[:STATe]",       .callback = Cmd_Set_OVP_State},
    {.pattern = "[:ADDR#]:OUTPut:OVP[:STATe]?",      .callback = Cmd_Read_OVP_State},
    {.pattern = "[:ADDR#]:OUTPut:OVP:TRIPed?",       .callback = Cmd_Read_OVP_State_TRIPed},
    {.pattern = "[:ADDR#]:OUTPut:OVP:CLEar",         .callback = Cmd_Clear_OVP_State_TRIPed},
    //电流
    {.pattern = "[:ADDR#]:OUTPut:OCP:VALue",         .callback = Cmd_Set_OCP},
    {.pattern = "[:ADDR#]:OUTPut:OCP:VALue?",        .callback = Cmd_Read_OCP},
    {.pattern = "[:ADDR#]:OUTPut:OCP[:STATe]",       .callback = Cmd_Set_OCP_State},
    {.pattern = "[:ADDR#]:OUTPut:OCP[:STATe]?",      .callback = Cmd_Read_OCP_State},
    {.pattern = "[:ADDR#]:OUTPut:OCP:TRIPed?",       .callback = Cmd_Read_OCP_State_TRIPed},
    {.pattern = "[:ADDR#]:OUTPut:OCP:CLEar",         .callback = Cmd_Clear_OCP_State_TRIPed},

    //    {.pattern = "[:ADDR#]:OUTPut:TIMer",         .callback = Cmd_Set_Timer_State},
    //    {.pattern = "[:ADDR#]:OUTPut:TIMer?",        .callback = Cmd_Read_Timer_State},
    //    {.pattern = "[:ADDR#]:OUTPut:TIMer:DATA",    .callback = Cmd_Set_Timer_Data},
    //    {.pattern = "[:ADDR#]:OUTPut:TIMer:DATA?",   .callback = Cmd_Read_Timer_Data},
    #endif

    #if 1
    //BATTester参数设置
    {.pattern = "[:ADDR#]:BATTester[:STATe]",          .callback = Cmd_Set_BatTester_State},
    {.pattern = "[:ADDR#]:BATTester[:STATe]?",         .callback = Cmd_Read_BatTester_State},
    {.pattern = "[:ADDR#]:BATTester:BASE",             .callback = Cmd_Set_BatTester_Base},
    {.pattern = "[:ADDR#]:BATTester:BASE?",            .callback = Cmd_Read_BatTester_Base},
    {.pattern = "[:ADDR#]:BATTester:PARAmeter",        .callback = Cmd_Set_BatTester_PointParam},
    {.pattern = "[:ADDR#]:BATTester:PARAmeter?",       .callback = Cmd_Read_BatTester_PointParam},

    {.pattern = "[:ADDR#]:BATTester:GOBALProtect",     .callback = Cmd_Set_BatTester_Gobal_Protect},
    {.pattern = "[:ADDR#]:BATTester:GOBALProtect?",    .callback = Cmd_Read_BatTester_Gobal_Protect},
    {.pattern = "[:ADDR#]:BATTester:GROUPProtect",    .callback = Cmd_Set_BatTester_Group_Protect},
    {.pattern = "[:ADDR#]:BATTester:GROUPProtect?",   .callback = Cmd_Read_BatTester_Group_Protect},
    #endif

    #if 1
    //ListOut参数设置
    {.pattern = "[:ADDR#]:LISTout[:STATe]",    .callback = Cmd_Set_List_State},
    {.pattern = "[:ADDR#]:LISTout[:STATe]?",   .callback = Cmd_Read_List_State},
    {.pattern = "[:ADDR#]:LISTout:BASE",       .callback = Cmd_Set_List_Base},
    {.pattern = "[:ADDR#]:LISTout:BASE?",      .callback = Cmd_Read_List_Base},
    {.pattern = "[:ADDR#]:LISTout:PARAmeter",  .callback = Cmd_Set_List_PointParam},
    {.pattern = "[:ADDR#]:LISTout:PARAmeter?", .callback = Cmd_Read_List_PointParam},
    #endif

    #if 1
    //DelayOut参数设置
    {.pattern = "[:ADDR#]:DELAY[:STATe]",      .callback = Cmd_Set_Delay_State},
    {.pattern = "[:ADDR#]:DELAY[:STATe]?",     .callback = Cmd_Read_Delay_State},
    {.pattern = "[:ADDR#]:DELAY:STARt",        .callback = Cmd_Set_Delay_Start},
    {.pattern = "[:ADDR#]:DELAY:STARt?",       .callback = Cmd_Read_Delay_Start},
    {.pattern = "[:ADDR#]:DELAY:GROUPs",       .callback = Cmd_Set_Delay_Count},
    {.pattern = "[:ADDR#]:DELAY:GROUPs?",      .callback = Cmd_Read_Delay_Count},
    {.pattern = "[:ADDR#]:DELAY:CYCLEs",       .callback = Cmd_Set_Delay_Cycle},
    {.pattern = "[:ADDR#]:DELAY:CYCLEs?",      .callback = Cmd_Read_Delay_Cycle},
    {.pattern = "[:ADDR#]:DELAY:ENDState",     .callback = Cmd_Set_Delay_endState},
    {.pattern = "[:ADDR#]:DELAY:ENDState?",    .callback = Cmd_Read_Delay_endState},
    {.pattern = "[:ADDR#]:DELAY:STOP",         .callback = Cmd_Set_Delay_Stop},
    {.pattern = "[:ADDR#]:DELAY:STOP?",        .callback = Cmd_Read_Delay_Stop},
    {.pattern = "[:ADDR#]:DELAY:PARAmeter",    .callback = Cmd_Set_Delay_Param},
    {.pattern = "[:ADDR#]:DELAY:PARAmeter?",   .callback = Cmd_Read_Delay_Param},
    #endif


    #if 1
    //预设值
    {.pattern = "[:ADDR#]:PRESet#[:APPLy]",      .callback = Cmd_Preset_Apply},
    {.pattern = "[:ADDR#]:PRESet#:SET:VOLTage",  .callback = Cmd_Set_Preset_Volt},
    {.pattern = "[:ADDR#]:PRESet#:SET:VOLTage?", .callback = Cmd_Read_Preset_Volt},
    {.pattern = "[:ADDR#]:PRESet#:SET:CURRent",  .callback = Cmd_Set_Preset_Curr},
    {.pattern = "[:ADDR#]:PRESet#:SET:CURRent?", .callback = Cmd_Read_Preset_Curr},

    {.pattern = "[:ADDR#]:PRESet#:SET:OVP",  .callback = Cmd_Set_Preset_OVP},
    {.pattern = "[:ADDR#]:PRESet#:SET:OVP?", .callback = Cmd_Read_Preset_OVP},
    {.pattern = "[:ADDR#]:PRESet#:SET:OCP",  .callback = Cmd_Set_Preset_OCP},
    {.pattern = "[:ADDR#]:PRESet#:SET:OCP?", .callback = Cmd_Read_Preset_OCP},
    #endif

    #if 1
    {.pattern = "[:ADDR#]:SYSTem:GROUps",      .callback = Cmd_Set_System_Groups},
    {.pattern = "[:ADDR#]:SYSTem:GROUps?",       .callback = Cmd_Read_System_Groups},
    //远程控制/本地控制
    {.pattern = "[:ADDR#]:SYSTem:REMote",      .callback = Cmd_Set_System_Remote_Mode},
    {.pattern = "[:ADDR#]:SYSTem:LOCal",       .callback = Cmd_Set_System_Local_Mode},
    {.pattern = "[:ADDR#]:SYSTem:LOCK",        .callback = Cmd_Set_System_Lock},
    {.pattern = "[:ADDR#]:SYSTem:UNLOCK",      .callback = Cmd_Set_System_UnLock},
    //蜂鸣器
    {.pattern = "[:ADDR#]:SYSTem:BEEPer:TEST",         .callback = Cmd_System_Beeper},
    {.pattern = "[:ADDR#]:SYSTem:BEEPer[:STATe]",      .callback = Cmd_Set_Beeper},
    {.pattern = "[:ADDR#]:SYSTem:BEEPer[:STATe]?",     .callback = Cmd_Read_Beeper},

    //屏幕背光
    {.pattern = "[:ADDR#]:SYSTem:BRIGhtness",        .callback = Cmd_Set_Brightness},
    {.pattern = "[:ADDR#]:SYSTem:BRIGhtness?",       .callback = Cmd_Read_Brightness},

    //系统语言
    {.pattern = "[:ADDR#]:SYSTem:LANGuage",      .callback = Cmd_System_Set_Language},
    {.pattern = "[:ADDR#]:SYSTem:LANGuage?",     .callback = Cmd_System_Read_Language},


    {.pattern = "[:ADDR#]:SYSTem:POWER:MODE",          .callback = Cmd_Set_System_Power_Mode},
    {.pattern = "[:ADDR#]:SYSTem:POWER:MODE?",         .callback = Cmd_Read_System_Power_Mode},
    {.pattern = "[:ADDR#]:SYSTem:POWER:ID",            .callback = Cmd_Set_System_Slave_ID},
    {.pattern = "[:ADDR#]:SYSTem:POWER:ID?",           .callback = Cmd_Read_System_Slave_ID},
    {.pattern = "[:ADDR#]:SYSTem:POWER:POWEROut",      .callback = Cmd_Set_System_Power_Out},
    {.pattern = "[:ADDR#]:SYSTem:POWER:POWEROut?",     .callback = Cmd_Read_System_Power_Out},
    {.pattern = "[:ADDR#]:SYSTem:POWER:OVPDelay",      .callback = Cmd_Set_OVP_Delay},
    {.pattern = "[:ADDR#]:SYSTem:POWER:OVPDelay?",     .callback = Cmd_Read_OVP_Delay},
    {.pattern = "[:ADDR#]:SYSTem:POWER:OCPDelay",      .callback = Cmd_Set_OCP_Delay},
    {.pattern = "[:ADDR#]:SYSTem:POWER:OCPDelay?",     .callback = Cmd_Read_OCP_Delay},

    {.pattern = "[:ADDR#]:SYSTem:POWER:ELOAD[:STATe]",         .callback = Cmd_Set_Power_eLoad},
    {.pattern = "[:ADDR#]:SYSTem:POWER:ELOAD[:STATe]?",        .callback = Cmd_Read_Power_eLoad},
    {.pattern = "[:ADDR#]:SYSTem:POWER:POWERDown[:STATe]",     .callback = Cmd_Set_Power_Down},
    {.pattern = "[:ADDR#]:SYSTem:POWER:POWERDown[:STATe]?",    .callback = Cmd_Read_Power_Down},


    {.pattern = "[:ADDR#]:SYSTem:POWER:ZEROCURRent",    .callback = Cmd_Calib_ZeroCurrent},


    //应用已设置的网络参数
    {.pattern = ":SYSTem:COMMunicate:LAN:APPLy",         .callback = Cmd_Set_LAN_Apply},
    //DHCP
    {.pattern = ":SYSTem:COMMunicate:LAN:DHCP[:STATe]",    .callback = Cmd_Set_LAN_DHCP},
    {.pattern = ":SYSTem:COMMunicate:LAN:DHCP[:STATe]?",   .callback = Cmd_Read_LAN_DHCP},
    //网络地址
    {.pattern = ":SYSTem:COMMunicate:LAN:IPADdress",   .callback = Cmd_Set_LAN_IP},
    {.pattern = ":SYSTem:COMMunicate:LAN:IPADdress?",  .callback = Cmd_Read_LAN_IP},
    //子网掩码
    {.pattern = ":SYSTem:COMMunicate:LAN:SMASK",     .callback = Cmd_Set_LAN_NetMask},
    {.pattern = ":SYSTem:COMMunicate:LAN:SMASK?",    .callback = Cmd_Read_LAN_NetMask},
    //网关
    {.pattern = ":SYSTem:COMMunicate:LAN:GATEway",   .callback = Cmd_Set_LAN_GateWay},
    {.pattern = ":SYSTem:COMMunicate:LAN:GATEway?",  .callback = Cmd_Read_LAN_GateWay},
    #endif




    #if 1
    //{.pattern = "[:ADDR#]:TEST:GOTO[:PAGE]",       .callback = Cmd_Goto_Page},
    {.pattern = "[:ADDR#]:TEST:MEM?",              .callback = Cmd_Read_MEM},
    {.pattern = "[:ADDR#]:TEST:ERROR:FLAG?",        .callback = Cmd_Read_ErrorFlag},
    #endif

    #if 1
    //工厂模式
    {.pattern = "[:ADDR#]:FACTORY:INPUT:PASSWD",        .callback = Cmd_Input_Password},
    {.pattern = ":FACTORY:MIS:SN",                      .callback = Cmd_Set_SN},
    {.pattern = ":FACTORY:MIS:SN?",                     .callback = Cmd_Read_SN},
    {.pattern = "[:ADDR#]:FACTORY:CAL:VOLT",            .callback = Cmd_Calib_Voltage},
    {.pattern = "[:ADDR#]:FACTORY:CAL:CURR",            .callback = Cmd_Calib_Current},
    {.pattern = "[:ADDR#]:FACTORY:CAL:DELETE",          .callback = Cmd_Calib_Delete},

    //散热风扇
    {.pattern = "[:ADDR#]:SYSTem:FAN[:STATe]",      .callback = Cmd_System_Set_FAN},
    {.pattern = "[:ADDR#]:SYSTem:FAN[:STATe]?",     .callback = Cmd_System_Read_FAN},
    #endif

    SCPI_CMD_LIST_END
};





