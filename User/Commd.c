#include "Commd.h"

void GUI_KeySend(lv_obj_t *obj, lv_key_t key)
{
    if((obj != NULL) && (key != 0)) {
        lv_event_send(obj, LV_EVENT_KEY, &key);
    }
}


const int scpiPort_step[4] = {1, 10, 100, 1000};


const float VSR_step[5] = {0.01, 0.1, 1.0, 10.0, 100.0};
const float ASR_step[5] = {0.01, 0.1, 1.0, 10.0, 100.0};

const float Sense_Volt_step[2] = {0.1, 1};
const float Temperature_step[2] = {1, 10};
const int delayTime_step[3] = {1, 10, 100};

const float P_step_HH[4] = {0.1, 1.0, 10.0, 100.0};
const float P_step_H[5] = {0.01, 0.1, 1.0, 10.0, 100.0};
const float P_step[5] = {0.001, 0.01, 0.1, 1.0, 10.0};
const float P_step_L[4] = {0.001, 0.01, 0.1, 1.0};

const float V_step_H[5] = {0.01, 0.1, 1.0, 10.0, 100.0};
const float V_step[5] = {0.001, 0.01, 0.1, 1.0, 10.0};
const float V_step_L[4] = {0.001, 0.01, 0.1, 1.0};

const float A_step_H[5] = {0.01, 0.1, 1.0, 10.0, 100.0};
const float A_step[5] = {0.001, 0.01, 0.1, 1.0, 10.0};
const float A_step_L[4] = {0.001, 0.01, 0.1, 1.0};





const float Rin_step[4] = {0.001, 0.01, 0.1, 1.0};


//电压
const float *__Get_Voltage_Set_Step(int *step_len) {
    if(PowerInfo.Model.maxVolt > 99.995f) {
        *step_len = ARRAY_COUNT(V_step_H);
        return V_step_H;
    } else
        if(PowerInfo.Model.maxVolt > 9.9995f) {
            *step_len = ARRAY_COUNT(V_step);
            return V_step;
        } else {
            *step_len = ARRAY_COUNT(V_step_L);
            return V_step_L;
        }
}
const char *__Get_Voltage_Set_Fmt(void) {
    if(PowerInfo.Model.maxVolt > 99.995f) {
        return "%6.2f";
    } else
        if(PowerInfo.Model.maxVolt > 9.9995f) {
            return "%6.3f";
        } else {
            return "%6.3f";
        }
}
//电流
const float *__Get_Current_Set_Step(int *step_len) {
    if(PowerInfo.Model.maxCurr > 99.995f) {
        *step_len = ARRAY_COUNT(A_step_H);
        return A_step_H;
    } else
        if(PowerInfo.Model.maxCurr > 9.9995f) {
            *step_len = ARRAY_COUNT(A_step);
            return A_step;
        } else {
            *step_len = ARRAY_COUNT(A_step_L);
            return A_step_L;
        }
}
const char *__Get_Current_Set_Fmt(void) {
    if(PowerInfo.Model.maxCurr > 99.995f) {
        return "%6.2f";
    } else
        if(PowerInfo.Model.maxCurr > 9.9995f) {
            return "%6.3f";
        } else {
            return "%6.3f";
        }
}
//功率
const float *__Get_Power_Set_Step(int *step_len) {
    if(PowerInfo.Model.maxPower > 999.95f) {
        *step_len = ARRAY_COUNT(P_step_HH);
        return P_step_HH;
    }
    if(PowerInfo.Model.maxPower > 99.995f) {
        *step_len = ARRAY_COUNT(P_step_H);
        return P_step_H;
    } else
        if(PowerInfo.Model.maxPower > 9.9995f) {
            *step_len = ARRAY_COUNT(P_step);
            return P_step;
        } else {
            *step_len = ARRAY_COUNT(P_step_L);
            return P_step_L;
        }
}
const char *__Get_Power_Set_Fmt(void) {
    if(PowerInfo.Model.maxPower > 999.95f) {
        return "%6.1f";
    } else
        if(PowerInfo.Model.maxPower > 99.995f) {
            return "%6.2f";
        } else
            if(PowerInfo.Model.maxPower > 9.9995f) {
                return "%6.3f";
            } else {
                return "%6.3f";
            }
}






void __Voltage_Display_Fmt(char *pbuf, float volt) {
#define VOLT_DISP_HH_FORM   "%6.1f"
#define VOLT_DISP_H_FORM    "%6.2f"
#define VOLT_DISP_M_FORM    "%6.3f"
#define VOLT_DISP_L_FORM    "%6.3f"
    if(volt < 9.99995f) {
        snprintf(pbuf, 5 + 1 + 1, VOLT_DISP_L_FORM, volt);
    } else
        if(volt < 99.9995f) {
            snprintf(pbuf, 5 + 1 + 1, VOLT_DISP_M_FORM, volt);
        } else
            if(volt < 999.995f) {
                snprintf(pbuf, 5 + 1 + 1, VOLT_DISP_H_FORM, volt);
            } else {
                snprintf(pbuf, 5 + 1 + 1, VOLT_DISP_HH_FORM, volt);
            }
}
void __Current_Display_Fmt(char *pbuf, float curr) {
#define CURR_DISP_HH_FORM   "%6.1f"
#define CURR_DISP_H_FORM    "%6.2f"
#define CURR_DISP_M_FORM    "%6.3f"
#define CURR_DISP_L_FORM    "%6.3f"
    if(curr < 9.99995f) {
        snprintf(pbuf, 5 + 1 + 1, CURR_DISP_L_FORM, curr);
    } else
        if(curr < 99.9995f) {
            snprintf(pbuf, 5 + 1 + 1, CURR_DISP_M_FORM, curr);
        } else
            if(curr < 999.995f) {
                snprintf(pbuf, 5 + 1 + 1, CURR_DISP_H_FORM, curr);
            } else {
                snprintf(pbuf, 5 + 1 + 1, CURR_DISP_HH_FORM, curr);
            }
}
void __Power_Display_Fmt(char *pbuf, float power) {
#define POWER_DISP_HH_FORM  "%6.1f"
#define POWER_DISP_H_FORM   "%6.2f"
#define POWER_DISP_M_FORM   "%6.3f"
#define POWER_DISP_L_FORM   "%6.3f"
    if(power < 9.99995f) {
        snprintf(pbuf, 5 + 1 + 1, POWER_DISP_L_FORM, power);
    } else
        if(power < 99.9995f) {
            snprintf(pbuf, 5 + 1 + 1, POWER_DISP_M_FORM, power);
        } else
            if(power < 999.995f) {
                snprintf(pbuf, 5 + 1 + 1, POWER_DISP_H_FORM, power);
            } else {
                snprintf(pbuf, 5 + 1 + 1, POWER_DISP_HH_FORM, power);
            }
}






const float Calib_V_step[6] = {0.0001, 0.001, 0.01, 0.1, 1.0, 10.0};
const float Calib_A_step[6] = {0.0001, 0.001, 0.01, 0.1, 1.0, 10.0};
const int Calib_Passwd_step[5] = {1, 10, 100, 1000, 10000};

const int point_step[3] = {1, 10, 100};
const int cycle_step[4] = {1, 10, 100, 1000};
const float time_step_HH[5] = {1.0, 10.0, 100.0, 1000.0, 10000.0};
const float time_step_H[5] = {0.1, 1.0, 10.0, 100.0, 1000.0};
const float time_step_M[5] = {0.01, 0.1, 1.0, 10.0, 100.0};
const float time_step_L[5] = {0.001, 0.01, 0.1, 1.0, 10.0};

//时间
const float *__Get_Time_Set_Step(float value, int *step_len) {
    if(value > 9999.95f) {
        *step_len = ARRAY_COUNT(time_step_HH);
        return time_step_HH;
    } else
        if(value > 999.995f) {
            *step_len = ARRAY_COUNT(time_step_H);
            return time_step_H;
        } else
            if(value > 99.9995f) {
                *step_len = ARRAY_COUNT(time_step_M);
                return time_step_M;
            } else {
                *step_len = ARRAY_COUNT(time_step_L);
                return time_step_L;
            }
}
const char *__Get_Time_Set_Fmt(float value) {
    if(value > 9999.95f) {
        return TIME_DISP_FORM_HH;
    } else
        if(value > 999.995f) {
            return TIME_DISP_FORM_H;
        } else
            if(value > 99.9995f) {
                return TIME_DISP_FORM_M;
            } else {
                return TIME_DISP_FORM_L;
            }
}


const float Value_step[5] = {0.001, 0.01, 0.1, 1.0, 10.0};

const uint16_t UpdateRate_step[3] = {5, 10, 100};

const uint32_t passwd_step[6] = {1, 10, 100, 1000, 10000, 100000};
/*
 * @brief       void label_setSelNum(lv_obj_t* label,uint16_t numpos,const char* text)
 * @details     Label选择特定数字(用于设置参数)
 */
void label_setSelNum(lv_obj_t *label, uint16_t numpos, const char *text)
{
    if(label == NULL) {
        return;
    }
    uint16_t strLen = strlen(text);
    uint16_t bitindex = 0;
    for(int16_t i = strLen - 1; i >= 0; --i)
    {
        if((('0' <= text[i]) && (text[i] <= '9')) || (text[i] == ' '))
        {
            if(bitindex == numpos)
            {
                lv_label_set_text_sel_start(label, i);
                lv_label_set_text_sel_end(label, i + 1);
                return ;
            }
            bitindex++;
        }
    }
}

/*
 * @brief       void label_setSelStr(lv_obj_t* label,const char* str,const char* text)
 * @details     Label选择特定字符(用于设置参数)
 */
void label_setSelStr(lv_obj_t *label, const char *str, const char *text)
{
    if(label == NULL) {
        return;
    }
    uint16_t strLen = strlen(str);
    uint16_t textLen = strlen(text);
    uint16_t bitindex = 0;
    for(uint16_t i = 0; i < textLen; ++i)
    {
        if(0 == strncmp(str, &text[i], strLen))
        {
            lv_label_set_text_sel_start(label, i);
            lv_label_set_text_sel_end(label, i + strLen);
            return ;
        }
        bitindex++;
    }
}

/*
 * @brief       void label_setSelIdx(lv_obj_t* label,const char* str,int idx)
 * @details     Label选择特定索引
 */
void label_setSelIdx(lv_obj_t *label, const char *str, int idx)
{
    if(label == NULL) {
        return;
    }
    if(idx != LV_LABEL_TEXT_SEL_OFF) {
        lv_label_set_text_sel_start(label, idx);
        lv_label_set_text_sel_end(label, idx + 1);
    } else {
        lv_label_set_text_sel_start(label, LV_LABEL_TEXT_SEL_OFF);
        lv_label_set_text_sel_end(label, LV_LABEL_TEXT_SEL_OFF);
    }
}

void UI_paramBtn_LoadStyle(lv_obj_t *button)
{
    if(button == NULL) { return ; }
    extern lv_font_t MultFont_16;
    //按键
    lv_obj_set_style_local_value_align(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
    lv_obj_set_style_local_value_ofs_x(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_value_color(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 255, 0));
    lv_obj_set_style_local_value_font(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &MultFont_16);
    //
    lv_obj_t *label = lv_obj_get_child(button, NULL);
    if(label == NULL) { return ; }
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, lv_obj_get_width(button) - 10);
    lv_obj_align(label, button, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_label_set_align(label, LV_LABEL_ALIGN_RIGHT);
    lv_obj_set_style_local_text_sel_bg_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 255, 255));
    lv_obj_set_style_local_text_sel_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));
    //
    lv_obj_invalidate(button);
    lv_obj_invalidate(label);
}


void UI_editBtn_LoadStyle(lv_obj_t *button)
{
    if(button == NULL) { return ; }
    extern lv_font_t MultFont_16;
    //按键
    lv_obj_set_style_local_value_align(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
    lv_obj_set_style_local_value_ofs_x(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_value_color(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 255, 255));
    lv_obj_set_style_local_value_font(button, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &MultFont_16);
    //
    lv_obj_t *label = lv_obj_get_child(button, NULL);
    if(label == NULL) { return ; }
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, lv_obj_get_width(button) - 10);
    lv_obj_align(label, button, LV_ALIGN_IN_RIGHT_MID, -10, 0);
    lv_label_set_align(label, LV_LABEL_ALIGN_RIGHT);
    lv_obj_set_style_local_text_sel_bg_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 255, 255));
    lv_obj_set_style_local_text_sel_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));
    //
    lv_obj_invalidate(button);
    lv_obj_invalidate(label);
}






///*
// * @brief       uint16_t userGetIntTemp(uint16_t adc_data)
// * @details     输入ADC转换值,计算片内温度
// */
//uint16_t userGetIntTemp(uint16_t adc_data)
//{
//    uint16_t temp_cal1 = *TEMPSENSOR_CAL1_ADDR;
//    uint16_t temp_cal2 = *TEMPSENSOR_CAL2_ADDR;
//    uint16_t temperature = 30 + (float)((adc_data >> 4) - temp_cal1) / (float)(temp_cal2 - temp_cal1);
//    return temperature;
//}

#ifdef MULT_CHANNEL

uint8_t __Get_Model_Hw_ID(float ADVolt)
{
    if(fabs(ADVolt - 1.65f) < 0.1f) {
        return MODEL_ID_0;
    } else
        //        if(fabs(ADVolt - 1.164f) < 0.1f) {
        //            return MODEL_ID_4;
        //        } else
        //            if(fabs(ADVolt - 2.48f) < 0.1f) {
        //                return MODEL_ID_3;
        //            } else
        if(fabs(ADVolt - 2.475f) < 0.1f) {
            return MODEL_ID_2;
        } else
            if(fabs(ADVolt - 2.18f) < 0.1f) {
                return MODEL_ID_1;
            } else
            { return MODEL_ID_UNKNOW; }
}

uint8_t __Get_Model_Hw_Channels(float ADVolt)
{
    if(fabs(ADVolt - 2.475f) < 0.1f) {
        return MODEL_CHANNELS_1;
    } else
        //        if(fabs(ADVolt - 2.45f) < 0.1f) {
        //            return MODEL_CHANNELS_5;
        //        } else
        //            if(fabs(ADVolt - 1.30f) < 0.1f) {
        //                return MODEL_CHANNELS_4;
        //            } else
        if(fabs(ADVolt - 1.65f) < 0.1f) {
            return MODEL_CHANNELS_3;
        } else
            if(fabs(ADVolt - 2.18f) < 0.1f) {
                return MODEL_CHANNELS_2;
            } else
            { return MODEL_CHANNELS_UNKNOW; }
}
#endif

/*
 * @brief       uint16_t userGetIntTemp(uint16_t adc_data)
 * @details     输入ADC转换值,计算片内温度
 */
float __Get_Chip_Temp(uint16_t adcValue) {
    float Vtemp_sensor = 0;
    uint16_t TS_CAL1;
    uint16_t TS_CAL2;
    uint32_t   uwConvertedValue;
    TS_CAL1 = *(__IO uint16_t *)(0x1FF1E820);
    TS_CAL2 = *(__IO uint16_t *)(0x1FF1E840);
    uwConvertedValue = adcValue;  /* 读取数值 */
    Vtemp_sensor = (110.0 - 30.0) * (uwConvertedValue - TS_CAL1) / (TS_CAL2 - TS_CAL1) + 30;  /* 转换 */
    return Vtemp_sensor;
}


static const uint8_t userCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const uint8_t userCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

/*
 * @brief       uint16_t userCRC_Calculate(uint8_t *pucFrame, uint16_t usLen)
 * @details     计算CRC
 */
uint16_t userCRC_Calculate(uint8_t *pucFrame, uint16_t usLen)
{
    uint8_t ucCRCHi = 0xFF;
    uint8_t ucCRCLo = 0xFF;
    int iIndex;
    while(usLen--)
    {
        iIndex = ucCRCLo ^ *(pucFrame++);
        ucCRCLo = (uint8_t)(ucCRCHi ^ userCRCHi[iIndex]);
        ucCRCHi = userCRCLo[iIndex];
    }
    return (uint16_t)(ucCRCHi << 8 | ucCRCLo);
}


