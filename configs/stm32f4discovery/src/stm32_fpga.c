/************************************************************************************
 * configs/stm32f4disovery/src/stm32_extmem.c
 *
 *   Copyright (C) 2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ************************************************************************************/

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <assert.h>
#include <debug.h>

#include <arch/board/board.h>

#include "chip.h"
#include "up_arch.h"

#include "stm32_fsmc.h"
#include "stm32_gpio.h"
#include "stm32.h"
#include "stm32f4discovery.h"

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

static const uint32_t g_psramconfig[] = { 
  GPIO_FSMC_CLK, 
  GPIO_FSMC_D0, GPIO_FSMC_D1, GPIO_FSMC_D2, GPIO_FSMC_D3,
  GPIO_FSMC_D4, GPIO_FSMC_D5, GPIO_FSMC_D6, GPIO_FSMC_D7,
  GPIO_FSMC_D8, GPIO_FSMC_D9, GPIO_FSMC_D10, GPIO_FSMC_D11,
  GPIO_FSMC_D12, GPIO_FSMC_D13, GPIO_FSMC_D14, GPIO_FSMC_D15,
  GPIO_FSMC_A16, GPIO_FSMC_A17, GPIO_FSMC_A18, GPIO_FSMC_A19,
  GPIO_FSMC_A20, GPIO_FSMC_A21, GPIO_FSMC_A22, GPIO_FSMC_A23,
  GPIO_FSMC_A24, GPIO_FSMC_A25, 
  GPIO_FSMC_NOE, GPIO_FSMC_NWE, GPIO_FSMC_NL,
  GPIO_FSMC_NBL0, GPIO_FSMC_NBL1,
  GPIO_FSMC_NE1, GPIO_FSMC_NE2  
};


const static int psramconfig_len = sizeof(g_psramconfig) / sizeof(uint32_t);


/************************************************************************************
 * Private Data
 ************************************************************************************/

/************************************************************************************
 * Private Functions
 ************************************************************************************/

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: stm32_extmemgpios
 *
 * Description:
 *   Initialize GPIOs for external memory usage
 *
 ************************************************************************************/

void stm32_fpgagpios(const uint32_t *gpios, int ngpios){
  int i;
  /* Configure GPIOs */
  
  for (i = 0; i < ngpios; i++){
    stm32_configgpio(gpios[i]);
  }
}

/************************************************************************************
 * Name: stm32_enablefsmc
 *
 * Description:
 *  enable clocking to the FSMC module
 *
 ************************************************************************************/

void up_fpgainitialize(int i){
  stm32_fpgagpios(g_psramconfig, psramconfig_len);
  uint32_t foo0 = 0x00000000,foo1 = 0x00000000;
  
  foo0 = FSMC_BCR_PSRAM | FSMC_BCR_MWID16 | FSMC_BCR_CCLKEN | FSMC_BCR_CBURSTRW | FSMC_BCR_WREN  | FSMC_BCR_MUXEN | FSMC_BCR_MBKEN | FSMC_BCR_BURSTEN | FSMC_BCR_WAITPOL | 0x80;
  printf("putreg32( 0x%x , STM32_FSMC_BCR1);\n",foo0);
  putreg32( foo0 , STM32_FSMC_BCR1); 
  foo0 = getreg32( STM32_FSMC_BCR1 );
  printf("getreg32 = 0x%x\n",foo0);
  
  foo1 = FSMC_BTR_CLKDIV(i) | FSMC_BTR_ADDHLD(2) | FSMC_BTR_DATLAT(2);
  printf("putreg32( 0x%x  , STM32_FSMC_BTR1);\n",foo1);
  putreg32( foo1 , STM32_FSMC_BTR1);
  foo1 = getreg32( STM32_FSMC_BTR1 );
  printf("getreg32 = 0x%x\n",foo1);
  putreg32( 0xffffffff, STM32_FSMC_BWTR1);
}

void set_fsmcdatlat(int i){
  unsigned int foo1;
  foo1 = getreg32(STM32_FSMC_BTR1);
  foo1 = foo1 & ~FSMC_BTR_DATLAT_MASK;
  if(i < 2){
    i = 2;
  }
  if(i > 17){
    i = 17;
  }
  foo1 |= FSMC_BTR_DATLAT(i);
  putreg32(foo1, STM32_FSMC_BTR1);
}

void stm32_enablefpga(void){
  uint32_t regval;

  /* Enable AHB clocking to the FSMC */
  
  regval  = getreg32( STM32_RCC_AHB3ENR);
  regval |= RCC_AHB3ENR_FSMCEN;
  putreg32(regval, STM32_RCC_AHB3ENR);
  
  
}

/************************************************************************************
 * Name: stm32_disablefsmc
 *
 * Description:
 *  enable clocking to the FSMC module
 *
 ************************************************************************************/

void stm32_disablefsmc(void)
{
  uint32_t regval;

  /* Disable AHB clocking to the FSMC */

  regval  = getreg32(STM32_RCC_AHB3ENR);
  regval &= ~RCC_AHB3ENR_FSMCEN;
  putreg32(regval, STM32_RCC_AHB3ENR);
}
