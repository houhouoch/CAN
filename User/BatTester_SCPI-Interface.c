#include "main.h"
#include "Commd.h"
#include "BatTester_SCPI-Interface.h"
#include "BatTester_Interface.h"









/*
 * @brief       BatTester开关
 * @details
 */
//BatTester功能开关
scpi_result_t Cmd_Set_BatTester_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //仅在普通模式下可以设置
    if(IF_Power_Get_Power_Mode() != POWER_MODE_NORMAL) {
        SCPI_ERR_RETURN;
    }
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_LIST);
    IF_BatTester_Set_State(param1 ? BAT_TESTER_STATE_RUNNING : BAT_TESTER_STATE_STOP);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_BatTester_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //返回:状态,当前点剩余,当前点,终止点,剩余次数,终止状态
    const char *state_str = "";
    switch(IF_BatTester_Get_State()) {
        case BAT_TESTER_STATE_PAUSE:
            state_str = "PAUSED";
            break;
        case BAT_TESTER_STATE_RUNNING:
            state_str = "ON";
            break;
        case BAT_TESTER_STATE_COMPLETED:
            state_str = "COMPLETED";
            break;
        case BAT_TESTER_STATE_NULL:
        case BAT_TESTER_STATE_STOP:
            state_str = "OFF";
            break;
    }
    float remainingTime = IF_BatTester_Get_RemainTime();
    int currPoint = IF_BatTester_Get_CurGroups();
    int endPoint = IF_BatTester_Get_EndGroups();
    int remainingCycle = 0;
    if(BatTester.set.cycles != 0) {
        remainingCycle = BatTester.set.cycles - BatTester.now.curCycled;
    } else {
        remainingCycle = CYCLE_MAX;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, "%s,"TIME_DISP_FORM","POINT_DISP_FORM","POINT_DISP_FORM","CYCLES_DISP_FORM,
                     state_str, remainingTime, currPoint, endPoint,
                     remainingCycle);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


/*
 * @brief       BatTester基础参数
 * @details
 */
//设置基本参数
scpi_result_t Cmd_Set_BatTester_Base(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param_start = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_start, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_count = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_count, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_cycle = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_cycle, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //运行中不能改变参数
    if(BatTester_IsRunning() != 0) {
        //        IF_MsgBox_Tips_Mult(&mStr_list_noedit);
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_LIST);
    IF_BatTester_Set_Start(param_start.content.value);
    IF_BatTester_Set_Groups(param_count.content.value);
    IF_BatTester_Set_Cycles(param_cycle.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_BatTester_Base(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //返回:起始点,总点数,循环次数,停止状态
    uint16_t start = IF_BatTester_Get_Start();
    uint16_t groups = IF_BatTester_Get_Groups();
    uint16_t cycles = IF_BatTester_Get_Cycles();
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, POINT_DISP_FORM","POINT_DISP_FORM","CYCLES_DISP_FORM,
                     start, groups, cycles);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}





/*
 * @brief       BatTester点参数
 * @details
 */
//设置点参数
scpi_result_t Cmd_Set_BatTester_PointParam(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param_index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_volt = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_volt, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_curr = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_curr, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_time = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_time, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //运行中不能改变参数
    if(BatTester_IsRunning() != 0) {
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_LIST);
    IF_BatTester_Set_Voltage(param_index.content.value, param_volt.content.value);
    IF_BatTester_Set_Current(param_index.content.value, param_curr.content.value);
    IF_BatTester_Set_kTime(param_index.content.value, param_time.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_BatTester_PointParam(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    uint16_t readPoint = 0;
    uint16_t readCount = 0;
    scpi_number_t param_readIdx = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_readIdx, FALSE)) {
        SCPI_ERR_RETURN;
    } else {
        readPoint = param_readIdx.content.value;
    }
    scpi_number_t param_readCount = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_readCount, FALSE)) {
        readCount = 1;
    } else {
        readCount = param_readCount.content.value;
    }
    if(readCount > 5) {
        readCount = 5;
    }
    //发送数据
    for(uint16_t i = 0; i < readCount; ++i) {
        //返回:索引,电压,电流,时长
        char buffer[128] = {0};
        uint16_t length = 0;
        uint16_t idx = (readPoint + i);
        length = sprintf(buffer, POINT_DISP_FORM","LIST_VOL_SET_FORM","LIST_CUR_SET_FORM","TIME_DISP_FORM";",
                         idx, BatTester.set.points[idx].volt, BatTester.set.points[idx].curr, BatTester.set.points[idx].time);
        SCPI_ResultArbitraryBlock(context, buffer, length);
    }
    return SCPI_RES_OK;
}







static const scpi_choice_def_t scpi_BatTester_logic_def[] = {
    {"OR",  _STOP_LOGIC_OR},  //开启
    {"AND", _STOP_LOGIC_AND}, //关闭
    SCPI_CHOICE_LIST_END /* termination of option list */
};
static const scpi_choice_def_t scpi_BatTester_stop_def[] = {
    {"NONE", __STOP_NONE},

    {"Vo>", __STOP_MORE_V},
    {"Vo<", __STOP_LESS_V},

    {"Io>", __STOP_MORE_C},
    {"Io<", __STOP_LESS_C},

    {"Po>", __STOP_MORE_P},
    {"Po<", __STOP_LESS_P},

    {"<Vo", __STOP_MORE_V},
    {">Vo", __STOP_LESS_V},

    {"<Io", __STOP_MORE_C},
    {">Io", __STOP_LESS_C},

    {"<Po", __STOP_MORE_P},
    {">Po", __STOP_LESS_P},


    SCPI_CHOICE_LIST_END /* termination of option list */
};


scpi_result_t Cmd_Set_BatTester_Gobal_Protect(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param_index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int32_t param_logic = {0};
    if(!SCPI_ParamChoice(context, scpi_BatTester_logic_def, &param_logic, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int32_t param_stop = {0};
    char stop_str[20] = {0};
    size_t stop_len = 0;
    if(!SCPI_ParamCopyText(context, stop_str, sizeof(stop_str) - 1, &stop_len, FALSE)) {
        SCPI_ERR_RETURN;
    }
    for(uint8_t i = 0; i < ARRAY_COUNT(scpi_BatTester_stop_def); ++i) {
        if(strcmp(scpi_BatTester_stop_def[i].name, stop_str) == 0) {
            param_stop = scpi_BatTester_stop_def[i].tag;
        }
    }
    scpi_number_t param_value = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_value, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_BATTESTER);
    IF_BatTester_Set_Global_Protect(param_index.content.value, param_logic, param_stop, param_value.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_BatTester_Gobal_Protect(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param_index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param_index.content.value >= ARRAY_COUNT(BatTester.set.stopCfg)) {
        SCPI_ERR_RETURN;
    }
    //
    uint16_t index = param_index.content.value;
    const char *logic_str = "";
    if(!SCPI_ChoiceToName(scpi_BatTester_logic_def, BatTester.set.stopCfg[index].logic, &logic_str) != 0) {
        SCPI_ERR_RETURN;
    }
    const char *stop_str = "";
    if(!SCPI_ChoiceToName(scpi_BatTester_stop_def, BatTester.set.stopCfg[index].stop, &stop_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, "%s,%s,%0.3f", logic_str, stop_str, BatTester.set.stopCfg[index].value);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}




scpi_result_t Cmd_Set_BatTester_Group_Protect(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param_no = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_no, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int32_t param_logic = {0};
    if(!SCPI_ParamChoice(context, scpi_BatTester_logic_def, &param_logic, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int32_t param_stop = {0};
    char stop_str[20] = {0};
    size_t stop_len = 0;
    if(!SCPI_ParamCopyText(context, stop_str, sizeof(stop_str) - 1, &stop_len, FALSE)) {
        SCPI_ERR_RETURN;
    }
    for(uint8_t i = 0; i < ARRAY_COUNT(scpi_BatTester_stop_def); ++i) {
        if(strcmp(scpi_BatTester_stop_def[i].name, stop_str) == 0) {
            param_stop = scpi_BatTester_stop_def[i].tag;
        }
    }
    scpi_number_t param_volt = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_volt, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_BATTESTER);
    IF_BatTester_Set_Group_Protect(param_no.content.value, param_index.content.value,
                                   param_logic, param_stop, param_volt.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_BatTester_Group_Protect(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param_no = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_no, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t param_index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    uint16_t index = param_index.content.value;
    uint16_t no = param_no.content.value;
    if(no >= ARRAY_COUNT(BatTester.set.points)) {
        SCPI_ERR_RETURN;
    }
    if(index >= ARRAY_COUNT(BatTester.set.points[index].stopCfg)) {
        SCPI_ERR_RETURN;
    }
    BatTester_Stop_Def *stopCfg = &BatTester.set.points[no].stopCfg[index];
    //
    const char *logic_str = "";
    if(!SCPI_ChoiceToName(scpi_BatTester_logic_def, BatTester.set.points[no].stopCfg[index].logic, &logic_str) != 0) {
        SCPI_ERR_RETURN;
    }
    const char *stop_str = "";
    if(!SCPI_ChoiceToName(scpi_BatTester_stop_def, BatTester.set.points[no].stopCfg[index].stop, &stop_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, "%s,%s,%0.3f", logic_str, stop_str, BatTester.set.points[no].stopCfg[index].value);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}



