#include "scpi-interface.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gui_guider.h"

#include "Commd.h"



#if 0

    Cmd_Set_Preset_OVP
    Cmd_Read_Preset_OVP
    Cmd_Set_Preset_OCP
    Cmd_Read_Preset_OCP
#endif

//预设值
scpi_result_t Cmd_Preset_Apply(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    uint8_t groups = IF_Option_Get_Groups();
    IF_Preset_Load(groups, number);
    return SCPI_RES_OK;
}

//电压
scpi_result_t Cmd_Set_Preset_Volt(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t vol = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &vol, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(vol.special != FALSE) {
        switch(vol.content.tag) {
            case SCPI_NUM_MIN:
                vol.content.value = VOL_MIN;
                break;
            case SCPI_NUM_MAX:
                vol.content.value = VOL_MAX;
                break;
        }
    }
    uint8_t groups = IF_Option_Get_Groups();
    IF_Preset_Set_Voltage(groups, number, vol.content.value);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Preset_Volt(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    uint8_t groups = IF_Option_Get_Groups();
    SCPI_ResultFloat(context, IF_Preset_Get_Voltage(groups, number));
    return SCPI_RES_OK;
}

//电流
scpi_result_t Cmd_Set_Preset_Curr(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t current = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &current, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(current.special != FALSE) {
        switch(current.content.tag) {
            case SCPI_NUM_MIN:
                current.content.value = VOL_MIN;
                break;
            case SCPI_NUM_MAX:
                current.content.value = VOL_MAX;
                break;
        }
    }
    uint8_t groups = IF_Option_Get_Groups();
    IF_Preset_Set_Current(groups, number, current.content.value);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Preset_Curr(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    uint8_t groups = IF_Option_Get_Groups();
    SCPI_ResultFloat(context, IF_Preset_Get_Current(groups, number));
    return SCPI_RES_OK;
}

#if 1
scpi_result_t Cmd_Set_Preset_OVP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    scpi_bool_t onoff = 0;
    if(! SCPI_ParamBool(context, &onoff, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t vol = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &vol, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(vol.special != FALSE) {
        switch(vol.content.tag) {
            case SCPI_NUM_MIN:
                vol.content.value = VOL_MIN;
                break;
            case SCPI_NUM_MAX:
                vol.content.value = VOL_MAX;
                break;
        }
    }
    uint8_t groups = IF_Option_Get_Groups();
    IF_Preset_Set_OVP_onoff(groups, number, onoff);
    IF_Preset_Set_OVP(groups, number, vol.content.value);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Preset_OVP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    const char *onoff_str = "";
    uint8_t groups = IF_Option_Get_Groups();
    SCPI_ChoiceToName(scpi_bool_def, IF_Preset_Get_OVP_onoff(groups, number), &onoff_str);
    SCPI_ResultCharacters(context, onoff_str, strlen(onoff_str));
    SCPI_ResultFloat(context, IF_Preset_Get_OVP(groups, number));
    return SCPI_RES_OK;
}


scpi_result_t Cmd_Set_Preset_OCP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    scpi_bool_t onoff = 0;
    if(! SCPI_ParamBool(context, &onoff, FALSE)) {
        SCPI_ERR_RETURN;
    }
    scpi_number_t amp = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &amp, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(amp.special != FALSE) {
        switch(amp.content.tag) {
            case SCPI_NUM_MIN:
                amp.content.value = VOL_MIN;
                break;
            case SCPI_NUM_MAX:
                amp.content.value = VOL_MAX;
                break;
        }
    }
    uint8_t groups = IF_Option_Get_Groups();
    IF_Preset_Set_OCP_onoff(groups, number, onoff);
    IF_Preset_Set_OCP(groups, number, amp.content.value);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Preset_OCP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    uint16_t num_offset = context->param_list.cmd_raw.position;
    int number = 0;
    uint16_t num_Count = context->param_list.cmd_raw.length - num_offset;
    while(1) {
        char ch = context->param_list.cmd_raw.data[num_offset];
        if((ch == ';') || (ch == ' ') || (ch == '\r') || (ch == '\n')) {
            break;
        }
        if(num_Count > 0) { num_Count--; }
        else { break; }
        if((ch >= '0') && (ch <= '9')) {
            sscanf(&context->param_list.cmd_raw.data[num_offset], "%d", &number);
        }
        num_offset++;
    }
    if((number < 0) || (number > (PRESET_COUNT - 1))) {
        SCPI_ERR_RETURN;
    }
    const char *onoff_str = "";
    uint8_t groups = IF_Option_Get_Groups();
    SCPI_ChoiceToName(scpi_bool_def, IF_Preset_Get_OCP_onoff(groups, number), &onoff_str);
    SCPI_ResultCharacters(context, onoff_str, strlen(onoff_str));
    SCPI_ResultFloat(context, IF_Preset_Get_OCP(groups, number));
    return SCPI_RES_OK;
}



#endif





scpi_result_t SCPI_Cmd_Meas_Parallel_Voltage(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    char volt_str[64] = {0};
    float volt_read = PowerInfo.Read.volt_read_total;
    //__Voltage_Display_Fmt(volt_str, volt_read);
    sprintf(volt_str, "%0.4f", volt_read);
    int length = strlen(volt_str);
    SCPI_ResultCharacters(context, volt_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_Meas_Parallel_Current(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    char curr_str[64] = {0};
    float curr_read = PowerInfo.Read.curr_read_total;
    //__Current_Display_Fmt(curr_str, curr_read);
    sprintf(curr_str, "%0.4f", curr_read);
    int length = strlen(curr_str);
    SCPI_ResultCharacters(context, curr_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_Meas_Parallel_Power(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    char power_str[64] = {0};
    float power_read = PowerInfo.Read.power_read_total;
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    int length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_Meas_Parallel_AllAsk(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    int length = 0;
    //
    char volt_str[64] = {0};
    float volt_read = PowerInfo.Read.volt_read_total;
    //__Voltage_Display_Fmt(volt_str, volt_read);
    sprintf(volt_str, "%0.4f", volt_read);
    length = strlen(volt_str);
    SCPI_ResultCharacters(context, volt_str, length);
    //
    char curr_str[64] = {0};
    float curr_read = PowerInfo.Read.curr_read_total;
    //__Current_Display_Fmt(curr_str, curr_read);
    sprintf(curr_str, "%0.4f", curr_read);
    length = strlen(curr_str);
    SCPI_ResultCharacters(context, curr_str, length);
    //
    char power_str[64] = {0};
    float power_read = PowerInfo.Read.power_read_total;
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}



scpi_result_t SCPI_Cmd_Meas_Series_Voltage(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    char volt_str[64] = {0};
    float volt_read = PowerInfo.Read.volt_read_total;
    //__Voltage_Display_Fmt(volt_str, volt_read);
    sprintf(volt_str, "%0.4f", volt_read);
    int length = strlen(volt_str);
    SCPI_ResultCharacters(context, volt_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_Meas_Series_Current(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    char curr_str[64] = {0};
    float curr_read = PowerInfo.Read.curr_read_total;
    //__Current_Display_Fmt(curr_str, curr_read);
    sprintf(curr_str, "%0.4f", curr_read);
    int length = strlen(curr_str);
    SCPI_ResultCharacters(context, curr_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_Meas_Series_Power(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    char power_str[64] = {0};
    float power_read = PowerInfo.Read.power_read_total;
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    int length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_Meas_Series_AllAsk(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_SLAVE)) {
        SCPI_ERR_RETURN;
    }
    int length = 0;
    //
    char volt_str[64] = {0};
    float volt_read = PowerInfo.Read.volt_read_total;
    //__Voltage_Display_Fmt(volt_str, volt_read);
    sprintf(volt_str, "%0.4f", volt_read);
    length = strlen(volt_str);
    SCPI_ResultCharacters(context, volt_str, length);
    //
    char curr_str[64] = {0};
    float curr_read = PowerInfo.Read.curr_read_total;
    //__Current_Display_Fmt(curr_str, curr_read);
    sprintf(curr_str, "%0.4f", curr_read);
    length = strlen(curr_str);
    SCPI_ResultCharacters(context, curr_str, length);
    //
    char power_str[64] = {0};
    float power_read = PowerInfo.Read.power_read_total;
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}






scpi_result_t SCPI_Cmd_MeasVoltage(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    __SCPI_Sample_Acquire(500);
    char volt_str[64] = {0};
    float volt_read = IF_Power_Read_Volt();
    //__Voltage_Display_Fmt(volt_str, volt_read);
    sprintf(volt_str, "%0.4f", volt_read);
    int length = strlen(volt_str);
    SCPI_ResultCharacters(context, volt_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_MeasCurrent(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    __SCPI_Sample_Acquire(500);
    char curr_str[64] = {0};
    float curr_read = IF_Power_Read_Curr();
    //__Current_Display_Fmt(curr_str, curr_read);
    sprintf(curr_str, "%0.4f", curr_read);
    int length = strlen(curr_str);
    SCPI_ResultCharacters(context, curr_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_MeasPower(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    __SCPI_Sample_Acquire(500);
    char power_str[64] = {0};
    float power_read = IF_Power_Read_Power();
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    int length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}


scpi_result_t SCPI_Cmd_MeasDVM(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char power_str[64] = {0};
    float power_read = 0;
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    int length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Cmd_MeasAllAsk(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    __SCPI_Sample_Acquire(500);
    int length = 0;
    //
    char volt_str[64] = {0};
    float volt_read = IF_Power_Read_Volt();
    //__Voltage_Display_Fmt(volt_str, volt_read);
    sprintf(volt_str, "%0.4f", volt_read);
    length = strlen(volt_str);
    SCPI_ResultCharacters(context, volt_str, length);
    //
    char curr_str[64] = {0};
    float curr_read = IF_Power_Read_Curr();
    //__Current_Display_Fmt(curr_str, curr_read);
    sprintf(curr_str, "%0.4f", curr_read);
    length = strlen(curr_str);
    SCPI_ResultCharacters(context, curr_str, length);
    //
    char power_str[64] = {0};
    float power_read = IF_Power_Read_Power();
    //__Power_Display_Fmt(power_str, power_read);
    sprintf(power_str, "%0.4f", power_read);
    length = strlen(power_str);
    SCPI_ResultCharacters(context, power_str, length);
    return SCPI_RES_OK;
}






/*
 * @brief       scpi_result_t Cmd_Set_Onoff(scpi_t *context)
 * @details     设置电源输出
                若出现过压过流保护,则SCPI_ERROR_EXECUTION_ERROR执行错误
 */
scpi_result_t Cmd_Set_Onoff(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //
    //    if(IF_Power_Get_OVP_TRIPed() || IF_Power_Get_OCP_TRIPed()) {
    //        SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
    //        SCPI_ERR_RETURN;
    //    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_On_Off(param1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OnOff(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_bool_t value = IF_Power_Get_On_Off_Now();
    SCPI_ResultBool(context, value);
    return SCPI_RES_OK;
}

/*
 * @brief       scpi_result_t Cmd_Read_CCCV(scpi_t *context)
 * @details     获取电源当前模式
 */
scpi_result_t Cmd_Read_CCCV(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%s", (IF_Power_Read_CCCV() != 0) ? "CC" : "CV");
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}





/*
 * @brief
 * @details     电压设置
 */
scpi_result_t Cmd_Set_Voltage(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = VOL_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = VOL_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_OutVolt(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Voltage(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, __Get_Voltage_Set_Fmt(), IF_Power_Get_OutVolt());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

/*
 * @brief
 * @details     OVP设置
 */
scpi_result_t Cmd_Set_OVP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = VOL_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = VOL_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_OVP(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OVP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, __Get_Voltage_Set_Fmt(), IF_Power_Get_OVP());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


scpi_result_t Cmd_Set_OVP_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_OVP_onoff(param1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OVP_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_bool_t value = IF_Power_Get_OVP_onoff();
    SCPI_ResultBool(context, value);
    return SCPI_RES_OK;
}




/*
 * @brief
 * @details     OVP状态
 */
scpi_result_t Cmd_Read_OVP_State_TRIPed(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    SCPI_ResultUInt8(context, (IF_Power_Get_OVP_TRIPed() != 0));
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Clear_OVP_State_TRIPed(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    IF_Power_OVP_Clean();
    return SCPI_RES_OK;
}







/*
 * @brief
 * @details     电流设置
 */
scpi_result_t Cmd_Set_Current(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = CUR_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = CUR_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_OutCurr(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Current(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, __Get_Current_Set_Fmt(), IF_Power_Get_OutCurr());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


/*
 * @brief
 * @details     OCP设置
 */
scpi_result_t Cmd_Set_OCP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = CUR_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = CUR_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_OCP(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OCP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, __Get_Current_Set_Fmt(), IF_Power_Get_OCP());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Set_OCP_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //    IF_Display_Go_to_Page(PAGE_HOME);
    IF_Power_Set_OCP_onoff(param1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OCP_State(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_bool_t value = IF_Power_Get_OCP_onoff();
    SCPI_ResultBool(context, value);
    return SCPI_RES_OK;
}
/*
 * @brief
 * @details     OCP状态
 */
scpi_result_t Cmd_Read_OCP_State_TRIPed(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    SCPI_ResultUInt8(context, (IF_Power_Get_OCP_TRIPed() != 0));
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Clear_OCP_State_TRIPed(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    IF_Power_OCP_Clean();
    return SCPI_RES_OK;
}






/*
 * @brief       scpi_result_t Cmd_Set_Voltage_Step(scpi_t *context)
 * @details     设置电压步进值
 */
scpi_result_t Cmd_Set_Voltage_Step(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    PowerInfo.Read.voltage_step = param1.content.value;
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Voltage_Step(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, __Get_Voltage_Set_Fmt(), PowerInfo.Read.voltage_step);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

/*
 * @brief       scpi_result_t Cmd_Voltage_Step_Up(scpi_t *context)
 * @details     设置电压向上步进
 */
scpi_result_t Cmd_Voltage_Step_Up(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    float vol = IF_Power_Get_OutVolt() + PowerInfo.Read.voltage_step;
    IF_Power_Set_OutVolt(vol);
    return SCPI_RES_OK;
}
/*
 * @brief       scpi_result_t Cmd_Voltage_Step_Down(scpi_t *context)
 * @details     设置电压向下步进
 */
scpi_result_t Cmd_Voltage_Step_Down(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    float vol = IF_Power_Get_OutVolt() - PowerInfo.Read.voltage_step;
    IF_Power_Set_OutVolt(vol);
    return SCPI_RES_OK;
}



/*
 * @brief       scpi_result_t Cmd_Set_Current_Step(scpi_t *context)
 * @details     设置电流步进值
 */
scpi_result_t Cmd_Set_Current_Step(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    PowerInfo.Read.current_step = param1.content.value;
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Current_Step(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, __Get_Current_Set_Fmt(), PowerInfo.Read.current_step);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}
/*
 * @brief       scpi_result_t Cmd_Current_Step_Up(scpi_t *context)
 * @details     设置电流向上步进
 */
scpi_result_t Cmd_Current_Step_Up(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    float curr = IF_Power_Get_OutCurr() + PowerInfo.Read.current_step;
    IF_Power_Set_OutCurr(curr);
    return SCPI_RES_OK;
}
/*
 * @brief       scpi_result_t Cmd_Current_Step_Down(scpi_t *context)
 * @details     设置电流向下步进
 */
scpi_result_t Cmd_Current_Step_Down(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    float curr = IF_Power_Get_OutCurr() - PowerInfo.Read.current_step;
    IF_Power_Set_OutCurr(curr);
    return SCPI_RES_OK;
}




/*
 * @brief
 * @details     电源输出内阻
 */
scpi_result_t Cmd_Set_Resistance(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
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
                param1.content.value = 1;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_OUTOPTION);
    IF_Power_Set_ResIn(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Resistance(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, RES_IN_SET_FORM, IF_Power_Get_ResIn());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}



/*
 * @brief
 * @details     电源斜率模式
 */
static const scpi_choice_def_t scpi_SR_Mode_type_def[] = {
    {"VHS", SRMODE_CV_HIGHSPEED},   //CV电压高速
    {"IHS", SRMODE_CC_HIGHSPEED},   //CC电流高速
    {"VSR", SRMODE_CV_SLEWRATE},    //CV电压斜率
    {"ISR", SRMODE_CC_SLEWRATE},    //CC电流斜率

    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_Set_SR_Mode(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    int32_t type = 0;
    if(SCPI_ParamChoice(context, scpi_SR_Mode_type_def, &type, false)) {
        //        IF_Display_Go_to_Page(PAGE_OUTOPTION);
        IF_Power_Set_SlewRate_Mode(type);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_SR_Mode(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    const char *buffer = "";
    if(!SCPI_ChoiceToName(scpi_SR_Mode_type_def, IF_Power_Get_SlewRate_Mode(), &buffer) != 0) {
        SCPI_ERR_RETURN;
    } else {
        uint16_t length = strlen(buffer);
        SCPI_ResultCharacters(context, buffer, length);
    }
    return SCPI_RES_OK;
}






/*
 * @brief       电压上升斜率
 * @details
 */
scpi_result_t Cmd_Set_Voltage_Rising(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = VSR_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = VSR_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_OUTOPTION);
    IF_Power_Set_VSR_Up(param1.content.value);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Voltage_Rising(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, VSR_SET_FORM, IF_Power_Get_VSR_Up());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

/*
 * @brief       电压下降斜率
 * @details
 */
scpi_result_t Cmd_Set_Voltage_Falling(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = VSR_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = VSR_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_OUTOPTION);
    IF_Power_Set_VSR_Down(param1.content.value);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Voltage_Falling(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, VSR_SET_FORM, IF_Power_Get_VSR_Down());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

/*
 * @brief       电流上升斜率
 * @details
 */
scpi_result_t Cmd_Set_Current_Rising(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = ISR_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = ISR_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_OUTOPTION);
    IF_Power_Set_ISR_Up(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Current_Rising(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, ISR_SET_FORM, IF_Power_Get_ISR_Up());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

/*
 * @brief
 * @details     电流下降斜率
 */
scpi_result_t Cmd_Set_Current_Falling(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = ISR_MIN;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = ISR_MAX;
                break;
            default:
                break;
        }
    }
    //    IF_Display_Go_to_Page(PAGE_OUTOPTION);
    IF_Power_Set_ISR_Down(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Current_Falling(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, ISR_SET_FORM, IF_Power_Get_ISR_Down());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}



/*
 * @brief       校准
 * @details
 */
scpi_result_t Cmd_Calib_Voltage(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Power_Calib_Voltage(param1.content.value);
    IF_Power_Calib_Voltage_Drv(param1.content.value);
    const char *result_str = "";
    if(PowerInfo.Read.voltCalib_state == CALIB_RES_OK) {
        result_str = "OK";
    } else {
        result_str = "ERROR";
    }
    SCPI_ResultCharacters(context, result_str, strlen(result_str));
    return SCPI_RES_OK;
}


scpi_result_t Cmd_Calib_Current(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Power_Calib_Current(param1.content.value);
    const char *result_str = "";
    if(PowerInfo.Read.currCalib_state == CALIB_RES_OK) {
        result_str = "OK";
    } else {
        result_str = "ERROR";
    }
    SCPI_ResultCharacters(context, result_str, strlen(result_str));
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Calib_Delete(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    const char *pStr = "";
    pStr = context->parser_state.programData.ptr;
    IF_Power_Calib_Delete(pStr);
    return SCPI_RES_OK;
}


scpi_result_t Cmd_Calib_ZeroCurrent(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Power_Calib_ZeroCurrent(param1.content.value);
    return SCPI_RES_OK;
}
