#include "DelayOut_SCPI.h"

#include "main.h"
#include "Commd.h"

#include "Delay_Interface.h"
#include "DelayTemp_Interface.h"

/*
 * @brief       DELAY开关
 * @details
 */
//结束状态
static const scpi_choice_def_t scpi_delay_lastState_def[] = {
    {"OFF", DELAY_ENDSTATE_OFF},     //关闭
    {"ON", DELAY_ENDSTATE_ON},        //打开
    {"LAST", DELAY_ENDSTATE_LAST},   //保持
    SCPI_CHOICE_LIST_END /* termination of option list */
};
//Delay功能开关
scpi_result_t Cmd_Set_Delay_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_On_Off(param1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //返回:状态,当前点剩余,当前点,终止点,剩余次数,终止状态
    const char *state_str = "";
    switch(delayoutInfo.state) {
        case DELAY_STATE_HALT:
            state_str = "HALT";
            break;
        case DELAY_STATE_RUNNING:
            state_str = "ON";
            break;
        case DELAY_STATE_STOP:
            state_str = "OFF";
            break;
    }
    float remainingTime = DelayOut_RemainingTime();
    uint16_t currPoint = delayoutInfo.current;
    uint16_t endPoint = delayoutInfo.startpoint + delayoutInfo.countpoint;
    int remainingCycle = 0;
    if(delayoutInfo.cycles != 0) {
        remainingCycle = delayoutInfo.cycles - delayoutInfo.loop;
    } else {
        remainingCycle = CYCLE_MAX;
    }
    const char *lastState_str = "";
    if(!SCPI_ChoiceToName(scpi_delay_lastState_def, delayoutInfo.lastState, &lastState_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[128] = {0};
    uint16_t length = 0;
    length = sprintf(buffer, "%s,"DELAY_TIME_FORM","POINT_DISP_FORM","POINT_DISP_FORM","CYCLES_DISP_FORM",%s",
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
                param1.content.value = DELAY_POINT_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = DELAY_POINT_MAX;
                break;
        }
    }
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_Start(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Start(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = {0};
    uint16_t length = sprintf(buffer, POINT_DISP_FORM, delayoutInfo.startpoint);
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
                param1.content.value = DELAY_POINT_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = DELAY_POINT_MAX;
                break;
        }
    }
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_Count(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Count(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = {0};
    uint16_t length = sprintf(buffer, POINT_DISP_FORM, delayoutInfo.countpoint);
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
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_Cycle(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Cycle(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = {0};
    uint16_t length = sprintf(buffer, CYCLES_DISP_FORM, delayoutInfo.cycles);
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
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_lastState(endState);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_endState(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *lastState_str = "";
    if(!SCPI_ChoiceToName(scpi_delay_lastState_def, delayoutInfo.lastState, &lastState_str) != 0) {
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
    {"NONE", DELAY_STOP_NONE},     //关闭
    {"<V", DELAY_STOP_MORE_V},     //电压大于
    {">V", DELAY_STOP_LESS_V},     //电压小于
    {"<C", DELAY_STOP_MORE_C},     //电流大于
    {">C", DELAY_STOP_LESS_C},     //电流小于
    {"<P", DELAY_STOP_MORE_P},     //功率大于
    {">P", DELAY_STOP_LESS_P},     //功率小于
    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_Set_Delay_Stop(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //判定条件
    int32_t stop = {0};
    if(!SCPI_ParamChoice(context, scpi_delay_stop_def, &stop, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_Stop(stop);
    //数值
    scpi_number_t stopValue = {0};
    if(SCPI_ParamNumber(context, scpi_special_numbers_def, &stopValue, FALSE)) {
        if(stopValue.special != FALSE) {
            switch(stopValue.content.tag) {
                case SCPI_NUM_MIN:
                    stopValue.content.value = DELAY_POINT_MIN;
                    break;
                case SCPI_NUM_MAX:
                    stopValue.content.value = DELAY_POINT_MAX;
                    break;
            }
        }
        IF_Delay_Set_StopValue(stopValue.content.value);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Delay_Stop(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *stop = "";
    if(!SCPI_ChoiceToName(scpi_delay_stop_def, delayoutInfo.stopCfg.stop, &stop) != 0) {
        SCPI_ERR_RETURN;
    }
    SCPI_ResultCharacters(context, stop, strlen(stop));
    if(delayoutInfo.lastState != DELAY_STOP_NONE) {
        float stopValue = DelayOut_GetStopVal();
        SCPI_ResultFloat(context, stopValue);
    }
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
    IF_Display_Go_to_Page(PAGE_DELAY);
    IF_Delay_Set_PointParam(idx.content.value, onoff, time.content.value);
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
        length = sprintf(buffer, POINT_DISP_FORM",%s"DELAY_TIME_FORM";",
                         idx, (delayoutInfo.points[idx].out) ? "ON" : "OFF", delayoutInfo.points[idx].dT);
        SCPI_ResultArbitraryBlock(context, buffer, length);
    }
    return SCPI_RES_OK;
}


/*
 * @brief       DELAY模板波形码
 * @details
 */
//模板
static const scpi_choice_def_t scpi_delay_code_def[] = {
    {"01P", DELAYTEMP_CODE_O1P},
    {"10P", DELAYTEMP_CODE_10P},
};

scpi_result_t Cmd_Generate_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t count = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &count, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int32_t code = {0};
    if(!SCPI_ParamChoice(context, scpi_delay_code_def, &code, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_DELAY_TEMP);
    IF_DelayTemp_Set_Type(DELAYTEMP_TYPE_STATE);
    IF_DelayTemp_Set_Start(index.content.value);
    IF_DelayTemp_Set_Count(count.content.value);
    IF_DelayTemp_Set_Code(code);
    IF_DelayTemp_Generate();
    return SCPI_RES_OK;
}

/*
 * @brief       DELAY模板固定时长
 * @details
 */
scpi_result_t Cmd_Generate_Fix(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t count = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &count, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t ton = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &ton, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t toff = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &toff, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_DELAY_TEMP);
    IF_DelayTemp_Set_Type(DELAYTEMP_TYPE_FIXT);
    IF_DelayTemp_Set_Start(index.content.value);
    IF_DelayTemp_Set_Count(count.content.value);
    IF_DelayTemp_Set_Ton(ton.content.value);
    IF_DelayTemp_Set_Toff(toff.content.value);
    IF_DelayTemp_Generate();
    return SCPI_RES_OK;
}

/*
 * @brief       DELAY模板递增时长
 * @details
 */
scpi_result_t Cmd_Generate_Inc(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t count = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &count, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t tbase = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &tbase, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t tstep = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &tstep, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_DELAY_TEMP);
    IF_DelayTemp_Set_Type(DELAYTEMP_TYPE_INCT);
    IF_DelayTemp_Set_Start(index.content.value);
    IF_DelayTemp_Set_Count(count.content.value);
    IF_DelayTemp_Set_Base(tbase.content.value);
    IF_DelayTemp_Set_Step(tstep.content.value);
    IF_DelayTemp_Generate();
    return SCPI_RES_OK;
}

/*
 * @brief       DELAY模板递降时长
 * @details
 */
scpi_result_t Cmd_Generate_Dec(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t index = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &index, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t count = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &count, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t tbase = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &tbase, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t tstep = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &tstep, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Display_Go_to_Page(PAGE_DELAY_TEMP);
    IF_DelayTemp_Set_Type(DELAYTEMP_TYPE_DECT);
    IF_DelayTemp_Set_Start(index.content.value);
    IF_DelayTemp_Set_Count(count.content.value);
    IF_DelayTemp_Set_Base(tbase.content.value);
    IF_DelayTemp_Set_Step(tstep.content.value);
    IF_DelayTemp_Generate();
    return SCPI_RES_OK;
}



static const scpi_choice_def_t scpi_delay_type_def[] = {
    {"STAT", DELAYTEMP_TYPE_STATE},
    {"FIX", DELAYTEMP_TYPE_FIXT},
    {"INC", DELAYTEMP_TYPE_INCT},
    {"DEC", DELAYTEMP_TYPE_DECT},
};
scpi_result_t Cmd_Read_Generate(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *type_str = "";
    if(!SCPI_ChoiceToName(scpi_delay_type_def, DelayTempInfo.temp_type, &type_str) != 0) {
        SCPI_ERR_RETURN;
    }
    SCPI_ResultCharacters(context, type_str, strlen(type_str));
    SCPI_ResultUInt16(context, DelayTempInfo.startpoint);
    SCPI_ResultUInt16(context, DelayTempInfo.totalCount);
    switch(DelayTempInfo.temp_type) {
        case DELAYTEMP_TYPE_STATE: {
            const char *code_str = "";
            if(!SCPI_ChoiceToName(scpi_delay_code_def, DelayTempInfo.code, &code_str) != 0) {
                SCPI_ERR_RETURN;
            }
            SCPI_ResultCharacters(context, code_str, strlen(code_str));
        }
        break;
        case DELAYTEMP_TYPE_FIXT: {
            SCPI_ResultFloat(context, DelayTempInfo.fix.time_on);
            SCPI_ResultFloat(context, DelayTempInfo.fix.time_off);
        }
        break;
        case DELAYTEMP_TYPE_INCT:
        case DELAYTEMP_TYPE_DECT: {
            SCPI_ResultFloat(context, DelayTempInfo.inc_dec.time_base);
            SCPI_ResultFloat(context, DelayTempInfo.inc_dec.time_step);
        }
        break;
    }
    return SCPI_RES_OK;
}


