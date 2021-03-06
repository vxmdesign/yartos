/************************************************************************************
 * configs/nucleo-f4x1re/src/stm32_adc.c
 *
 *   Copyright (C) 2014 Gregory Nutt. All rights reserved.
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

#include <errno.h>
#include <debug.h>

#include <nuttx/analog/adc.h>
#include <arch/board/board.h>

#include "chip.h"
#include "up_arch.h"

#include "stm32_pwm.h"
#include "nucleo-f4x1re.h"

#ifdef CONFIG_STM32_ADC1

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* The number of ADC channels in the conversion list */

#ifdef CONFIG_ADC_DMA
#  define ADC1_NCHANNELS 2
#else
#  define ADC1_NCHANNELS 1
#endif

/************************************************************************************
 * Private Data
 ************************************************************************************/
/* Identifying number of each ADC channel. */

#ifdef CONFIG_STM32_ADC1
#ifdef CONFIG_AJOYSTICK
#ifdef CONFIG_ADC_DMA
/* The Itead analog joystick gets inputs on ADC_IN0 and ADC_IN1 */

static const uint8_t  g_adc1_chanlist[ADC1_NCHANNELS] = {0, 1};

/* Configurations of pins used byte each ADC channels */

static const uint32_t g_adc1_pinlist[ADC1_NCHANNELS]  = {GPIO_ADC1_IN0, GPIO_ADC1_IN0};

#else
/* Without DMA, only a single channel can be supported */

/* The Itead analog joystick gets input on ADC_IN0 */

static const uint8_t  g_adc1_chanlist[ADC1_NCHANNELS] = {0};

/* Configurations of pins used byte each ADC channels */

static const uint32_t g_adc1_pinlist[ADC1_NCHANNELS]  = {GPIO_ADC1_IN0};

#endif /* CONFIG_ADC_DMA */
#endif /* CONFIG_AJOYSTICK */
#endif /* CONFIG_STM32_ADC1*/

/************************************************************************************
 * Private Functions
 ************************************************************************************/

/************************************************************************************
 * Public Functions
 ************************************************************************************/

/************************************************************************************
 * Name: board_adc_initialize
 *
 * Description:
 *   Initialize and register the ADC driver
 *
 ************************************************************************************/

int board_adc_initialize(void)
{
  static bool initialized = false;
  struct adc_dev_s *adc;
  int ret;
  int i;

  /* Check if we have already initialized */

  if (!initialized)
    {
#ifdef CONFIG_STM32_ADC1
      /* Configure the pins as analog inputs for the selected channels */

      for (i = 0; i < ADC1_NCHANNELS; i++)
        {
          stm32_configgpio(g_adc1_pinlist[i]);
        }

      /* Call stm32_adcinitialize() to get an instance of the ADC interface */

      adc = stm32_adcinitialize(1, g_adc1_chanlist, ADC1_NCHANNELS);
      if (adc == NULL)
        {
          adbg("ERROR: Failed to get ADC interface\n");
          return -ENODEV;
        }

      /* Register the ADC driver at "/dev/adc0" */

      ret = adc_register("/dev/adc0", adc);
      if (ret < 0)
        {
          adbg("adc_register failed: %d\n", ret);
          return ret;
        }
#endif
      /* Now we are initialized */

      initialized = true;
    }

  return OK;
}

/************************************************************************************
 * Name: adc_devinit
 *
 * Description:
 *   All STM32 architectures must provide the following interface to work with
 *   examples/adc.
 *
 ************************************************************************************/

#ifdef CONFIG_EXAMPLES_ADC
int adc_devinit(void)
{
#ifdef CONFIG_SAMA5_ADC
  return board_adc_initialize();
#else
  return -ENOSYS;
#endif
}
#endif /* CONFIG_EXAMPLES_ADC */

#endif /* CONFIG_STM32_ADC1 */
