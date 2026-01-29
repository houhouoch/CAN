#ifndef __SCPI_OPTION_INTERFACE_H_
#define __SCPI_OPTION_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"


//获取电源工作模式
scpi_result_t Cmd_Set_System_Power_Mode(scpi_t *context);
scpi_result_t Cmd_Read_System_Power_Mode(scpi_t *context);
scpi_result_t Cmd_Set_System_Slave_ID(scpi_t *context);
scpi_result_t Cmd_Read_System_Slave_ID(scpi_t *context);
//上点输出
scpi_result_t Cmd_Set_System_Power_Out(scpi_t *context);
scpi_result_t Cmd_Read_System_Power_Out(scpi_t *context);
//假负载开关
scpi_result_t Cmd_Set_Power_eLoad(scpi_t *context);
scpi_result_t Cmd_Read_Power_eLoad(scpi_t *context);
//OVP保护延迟
scpi_result_t Cmd_Set_OVP_Delay(scpi_t *context);
scpi_result_t Cmd_Read_OVP_Delay(scpi_t *context);
//OCP保护延迟
scpi_result_t Cmd_Set_OCP_Delay(scpi_t *context);
scpi_result_t Cmd_Read_OCP_Delay(scpi_t *context);
//散热风扇
scpi_result_t Cmd_System_Set_FAN(scpi_t *context);
scpi_result_t Cmd_System_Read_FAN(scpi_t *context);
//系统语言
scpi_result_t Cmd_System_Set_Language(scpi_t *context);
scpi_result_t Cmd_System_Read_Language(scpi_t *context);

//远程控制模式
scpi_result_t Cmd_Set_System_Remote_Mode(scpi_t *context);
scpi_result_t Cmd_Set_System_Local_Mode(scpi_t *context);
//上锁
scpi_result_t Cmd_Set_System_Lock(scpi_t *context);
scpi_result_t Cmd_Set_System_UnLock(scpi_t *context);

//用户组
scpi_result_t Cmd_Set_System_Groups(scpi_t *context);
scpi_result_t Cmd_Read_System_Groups(scpi_t *context);
//提示音
scpi_result_t Cmd_Set_Beeper(scpi_t *context);
scpi_result_t Cmd_Read_Beeper(scpi_t *context);
scpi_result_t Cmd_System_Beeper(scpi_t *context);

//掉电检测
scpi_result_t Cmd_Set_Power_Down(scpi_t *context);
scpi_result_t Cmd_Read_Power_Down(scpi_t *context);
//屏幕背光
scpi_result_t Cmd_Set_Brightness(scpi_t *context);
scpi_result_t Cmd_Read_Brightness(scpi_t *context);

//应用LAN设置
scpi_result_t Cmd_Set_LAN_Apply(scpi_t *context);
//DHCP
scpi_result_t Cmd_Set_LAN_DHCP(scpi_t *context);
scpi_result_t Cmd_Read_LAN_DHCP(scpi_t *context);
//IP地址
scpi_result_t Cmd_Set_LAN_IP(scpi_t *context);
scpi_result_t Cmd_Read_LAN_IP(scpi_t *context);
//网关
scpi_result_t Cmd_Set_LAN_GateWay(scpi_t *context);
scpi_result_t Cmd_Read_LAN_GateWay(scpi_t *context);
//子网掩码
scpi_result_t Cmd_Set_LAN_NetMask(scpi_t *context);
scpi_result_t Cmd_Read_LAN_NetMask(scpi_t *context);




scpi_result_t Cmd_Set_SN(scpi_t *context);
scpi_result_t Cmd_Read_SN(scpi_t *context);
scpi_result_t Cmd_Input_Password(scpi_t *context);

#endif
