#include "main.h"
#include "Commd.h"
#include "List_SCPI-Interface.h"
#include "List_Delayer_Interface.h"









/*
 * @brief       List开关
 * @details
 */
//结束状态
static const scpi_choice_def_t scpi_list_lastState_def[] = {
    {"OFF", LIST_ENDSTATE_OFF},     //关闭
    {"ON", LIST_ENDSTATE_ON},       //开启
    SCPI_CHOICE_LIST_END /* termination of option list */
};
//List功能开关
scpi_result_t Cmd_Set_List_State(scpi_t *context) {
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
    IF_List_Set_State(param1 ? LIST_STATE_RUNNING : LIST_STATE_STOP);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_List_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //返回:状态,当前点剩余,当前点,终止点,剩余次数,终止状态
    const char *state_str = "";
    switch(IF_List_Get_State()) {
        case LIST_STATE_PAUSE:
            state_str = "PAUSED";
            break;
        case LIST_STATE_RUNNING:
            state_str = "ON";
            break;
        case LIST_STATE_COMPLETED:
            state_str = "COMPLETED";
            break;
        case LIST_STATE_NULL:
        case LIST_STATE_STOP:
            state_str = "OFF";
            break;
    }
    float remainingTime = IF_List_Get_RemainTime();
    int currPoint = IF_List_Get_CurGroups();
    int endPoint = IF_List_Get_EndGroups();
    uint8_t endState = IF_List_Get_EndState();
    int remainingCycle = 0;
    if(listoutInfo.set.cycles != 0) {
        remainingCycle = listoutInfo.set.cycles - listoutInfo.now.curCycled;
    } else {
        remainingCycle = CYCLE_MAX;
    }
    const char *lastState_str = "";
    if(!SCPI_ChoiceToName(scpi_list_lastState_def, endState, &lastState_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, "%s,"TIME_DISP_FORM","POINT_DISP_FORM","POINT_DISP_FORM","CYCLES_DISP_FORM",%s",
                     state_str, remainingTime, currPoint, endPoint,
                     remainingCycle, lastState_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


/*
 * @brief       List基础参数
 * @details
 */
//设置基本参数
scpi_result_t Cmd_Set_List_Base(scpi_t *context) {
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
    int32_t param_laststate = 0;
    if(!SCPI_ParamChoice(context, scpi_list_lastState_def, &param_laststate, false)) {
        SCPI_ERR_RETURN;
    }
    //运行中不能改变参数
    if(ListOut_IsRunning() != 0) {
        //        IF_MsgBox_Tips_Mult(&mStr_list_noedit);
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_LIST);
    IF_List_Set_Start(param_start.content.value);
    IF_List_Set_Groups(param_count.content.value);
    IF_List_Set_Cycles(param_cycle.content.value);
    IF_List_Set_EndState(param_laststate);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_List_Base(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //返回:起始点,总点数,循环次数,停止状态
    uint16_t start = IF_List_Get_Start();
    uint16_t groups = IF_List_Get_Groups();
    uint16_t cycles = IF_List_Get_Cycles();
    uint8_t endState = IF_List_Get_EndState();
    const char *lastState_str = "";
    if(!SCPI_ChoiceToName(scpi_list_lastState_def, endState, &lastState_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, POINT_DISP_FORM","POINT_DISP_FORM","CYCLES_DISP_FORM",""%s",
                     start, groups, cycles, lastState_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}





/*
 * @brief       List点参数
 * @details
 */
//设置点参数
scpi_result_t Cmd_Set_List_PointParam(scpi_t *context) {
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
    if(ListOut_IsRunning() != 0) {
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_LIST);
    IF_List_Set_Voltage(param_index.content.value, param_volt.content.value);
    IF_List_Set_Current(param_index.content.value, param_curr.content.value);
    IF_List_Set_kTime(param_index.content.value, param_time.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_List_PointParam(scpi_t *context) {
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
                         idx, listoutInfo.set.points[idx].volt, listoutInfo.set.points[idx].curr, listoutInfo.set.points[idx].time);
        SCPI_ResultArbitraryBlock(context, buffer, length);
    }
    return SCPI_RES_OK;
}










/*
 * @brief       DELAYER开关
 * @details
 */
//结束状态
static const scpi_choice_def_t scpi_delay_lastState_def[] = {
    {"OFF", DELAYER_ENDSTATE_OFF},     //关闭
    {"ON", DELAYER_ENDSTATE_ON},        //打开
    {"LAST", DELAYER_ENDSTATE_LAST},   //保持
    SCPI_CHOICE_LIST_END /* termination of option list */
};
//Delay功能开关
scpi_result_t Cmd_Set_Delay_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //仅在普通模式下可以设置
    if(IF_Power_Get_Power_Mode() != POWER_MODE_NORMAL) {
        SCPI_ERR_RETURN;
    }
    //
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Delayer_Set_State(param1 ? DELAYER_STATE_RUNNING : DELAYER_STATE_STOP);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //返回:状态,当前点剩余,当前点,终止点,剩余次数,终止状态
    const char *state_str = "";
    switch(IF_Delayer_Get_State()) {
        case DELAYER_STATE_RUNNING:
            state_str = "ON";
            break;
        case DELAYER_STATE_NULL:
        case DELAYER_STATE_STOP:
            state_str = "OFF";
            break;
        case DELAYER_STATE_COMPLETED:
            state_str = "COMPLETED";
            break;
        case DELAYER_STATE_FAILED:
            state_str = "FAILED";
            break;
    }
    float remainingTime = Delayer_RemainingTime();
    uint16_t currPoint = IF_Delayer_Get_CurGroups();
    uint16_t endPoint = IF_Delayer_Get_Start() + IF_Delayer_Get_Groups();
    int remainingCycle = 0;
    if(IF_Delayer_Get_Cycles() != 0) {
        remainingCycle = IF_Delayer_Get_Cycles() - IF_Delayer_Get_CurCycled();
    } else {
        remainingCycle = CYCLE_MAX;
    }
    const char *lastState_str = "";
    if(!SCPI_ChoiceToName(scpi_delay_lastState_def, IF_Delayer_Get_EndState(), &lastState_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, "%s,"TIME_DISP_FORM","POINT_DISP_FORM","POINT_DISP_FORM","CYCLES_DISP_FORM",%s",
                     state_str, remainingTime, currPoint, endPoint,
                     remainingCycle, lastState_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}




/*
 * @brief       DELAY起始点
 * @details
 */
//设置输出起始点
scpi_result_t Cmd_Set_Delay_Start(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = 0;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = DELAYER_MAX_COUNT - IF_Delayer_Get_Groups();
                break;
        }
    }
    IF_Delayer_Set_Start(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Start(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = {0};
    uint16_t length = sprintf(buffer, POINT_DISP_FORM, IF_Delayer_Get_Start());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}







/*
 * @brief       DELAY总点数
 * @details
 */
//设置输出点数
scpi_result_t Cmd_Set_Delay_Count(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = 1;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = DELAYER_MAX_COUNT - IF_Delayer_Get_Start();
                break;
        }
    }
    IF_Delayer_Set_Groups(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Count(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = {0};
    uint16_t length = sprintf(buffer, POINT_DISP_FORM, IF_Delayer_Get_Groups());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}



/*
 * @brief       DELAY循环次数
 * @details
 */
//设置输出循环次数
scpi_result_t Cmd_Set_Delay_Cycle(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = CYCLE_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = CYCLE_MAX;
                break;
        }
    }
    IF_Delayer_Set_Cycles(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Cycle(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = {0};
    uint16_t length = sprintf(buffer, CYCLES_DISP_FORM, IF_Delayer_Get_Cycles());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}



/*
 * @brief       DELAY终止态
 * @details
 */
//设置终止态
scpi_result_t Cmd_Set_Delay_endState(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    int32_t endState = {0};
    if(!SCPI_ParamChoice(context, scpi_delay_lastState_def, &endState, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Delayer_Set_EndState(endState);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_endState(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *lastState_str = "";
    if(!SCPI_ChoiceToName(scpi_delay_lastState_def, IF_Delayer_Get_EndState(), &lastState_str) != 0) {
        SCPI_ERR_RETURN;
    }
    SCPI_ResultCharacters(context, lastState_str, strlen(lastState_str));
    return SCPI_RES_OK;
}





/*
 * @brief       DELAY结束条件
 * @details
 */
//结束状态
static const scpi_choice_def_t scpi_delay_stop_def[] = {
    {"NONE", DELAYER_STOP_NONE},     //关闭
    {"<V", DELAYER_STOP_MORE_V},     //电压大于
    {">V", DELAYER_STOP_LESS_V},     //电压小于
    {"<C", DELAYER_STOP_MORE_C},     //电流大于
    {">C", DELAYER_STOP_LESS_C},     //电流小于
    {"<P", DELAYER_STOP_MORE_P},     //功率大于
    {">P", DELAYER_STOP_LESS_P},     //功率小于
    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_Set_Delay_Stop(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //判定条件
    int32_t stop = {0};
    if(!SCPI_ParamChoice(context, scpi_delay_stop_def, &stop, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Delayer_Set_StopRule(stop);
    //数值
    scpi_number_t stopValue = {0};
    if(SCPI_ParamNumber(context, scpi_special_numbers_def, &stopValue, FALSE)) {
        IF_Delayer_Set_StopValue(stopValue.content.value);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Stop(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *stop = "";
    if(!SCPI_ChoiceToName(scpi_delay_stop_def, IF_Delayer_Get_StopRule(), &stop) != 0) {
        SCPI_ERR_RETURN;
    }
    SCPI_ResultCharacters(context, stop, strlen(stop));
    float stopValue = IF_Delayer_Get_StopValue();
    SCPI_ResultFloat(context, stopValue);
    return SCPI_RES_OK;
}

/*
 * @brief       DELAY点参数
 * @details
 */
//设置点参数
scpi_result_t Cmd_Set_Delay_Param(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //编号
    scpi_number_t idx = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &idx, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //开关
    scpi_bool_t onoff = {0};
    if(!SCPI_ParamBool(context, &onoff, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //时间
    scpi_number_t time = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &time, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Delayer_Set_Out(idx.content.value, onoff);
    IF_Delayer_Set_kTime(idx.content.value, time.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Param(scpi_t *context) {
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
        length = sprintf(buffer, POINT_DISP_FORM",%s"TIME_DISP_FORM";",
                         idx, (IF_Delayer_Get_Out(idx)) ? "ON" : "OFF", IF_Delayer_Get_kTime(idx));
        SCPI_ResultArbitraryBlock(context, buffer, length);
    }
    return SCPI_RES_OK;
}

