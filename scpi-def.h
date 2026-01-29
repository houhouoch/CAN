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

#ifndef __SCPI_DEF_H_
#define __SCPI_DEF_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "Setting.h"
#include "scpi/scpi.h"

#if 1
enum {
    INTERFACE_NULL = 0,
    INTERFACE_SOCKET = 1,
    INTERFACE_USB = 2,
    INTERFACE_VXI = 3,
    INTERFACE_WEB = 4,
    INTERFACE_CAN = 4,
};

#define __MULT_SCPI_ADDR_CHECK() {}
#endif



#define SCPI_IDN1 PowerInfo.about.company
#define SCPI_IDN2 PowerInfo.about.model_name
#define SCPI_IDN3 PowerInfo.about.serialNumber
#define SCPI_IDN4 PowerInfo.about.lcd_version

#define TEST_SCPI_INPUT(x,cmd)    SCPI_Input(x, cmd, strlen(cmd))
extern const scpi_command_t scpi_commands[];


#define SCPI_ERR_RETURN     do{ \
        return SCPI_RES_ERR;    \
    }while(0);

void SCPI_Core_Reg_Update(scpi_t *context);


#ifdef __cplusplus
}
#endif
#endif /* __SCPI_DEF_H_ */

