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
 * @file   config.h
 * @date   Wed Mar 20 12:21:26 UTC 2013
 *
 * @brief  SCPI Configuration
 *
 *
 */

#ifndef __SCPI_USER_CONFIG_H_
#define __SCPI_USER_CONFIG_H_

#ifdef  __cplusplus
extern "C" {
#endif


#define SCPI_LINE_ENDING                        "\n"
#define USE_FULL_ERROR_LIST                     1
#define USE_USER_ERROR_LIST                     0
#define USE_DEVICE_DEPENDENT_ERROR_INFORMATION  0
#define USE_MEMORY_ALLOCATION_FREE              0
#define USE_COMMAND_TAGS                        0
#define USE_DEPRECATED_FUNCTIONS                0
#define USE_CUSTOM_DTOSTRE                      0
#define USE_UNITS_IMPERIAL                      0
#define USE_UNITS_ANGLE                         0
#define USE_UNITS_PARTICLES                     0
#define USE_UNITS_DISTANCE                      0
#define USE_UNITS_MAGNETIC                      0
#define USE_UNITS_LIGHT                         0
#define USE_UNITS_ENERGY_FORCE_MASS             0
#define USE_UNITS_TIME                          0
#define USE_UNITS_TEMPERATURE                   0
#define USE_UNITS_RATIO                         0
#define USE_UNITS_POWER                         1
#define USE_UNITS_FREQUENCY                     0
#define USE_UNITS_ELECTRIC                      1
#define USE_UNITS_ELECTRIC_CHARGE_CONDUCTANCE   0


#define isuserchr(_C)   ((_C == '<') || (_C == '>') || (_C == '='))




#ifdef  __cplusplus
}
#endif

#endif // __SCPI_USER_CONFIG_H_
