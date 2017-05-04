/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : PWM2.c
**     Project     : AGV_v0_2
**     Processor   : MC9S08QE128CLK
**     Component   : PWM
**     Version     : Component 02.243, Driver 01.28, CPU db: 3.00.067
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2017-02-10, 15:06, # CodeGen: 33
**     Abstract    :
**         This component implements a pulse-width modulation generator
**         that generates signal with variable duty and fixed cycle. 
**     Settings    :
**         Used output pin             : 
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       33            |  PTC1_TPM3CH1
**             ----------------------------------------------------
**
**         Timer name                  : TPM3 [16-bit]
**         Counter                     : TPM3CNT   [$0061]
**         Mode register               : TPM3SC    [$0060]
**         Run register                : TPM3SC    [$0060]
**         Prescaler                   : TPM3SC    [$0060]
**         Compare register            : TPM3C1V   [$0069]
**         Flip-flop register          : TPM3C1SC  [$0068]
**
**         User handling procedure     : not specified
**
**         Port name                   : PTC
**         Bit number (in port)        : 1
**         Bit mask of the port        : $0002
**         Port data register          : PTCD      [$0004]
**         Port control register       : PTCDD     [$0005]
**
**         Initialization:
**              Output level           : high
**              Timer                  : Enabled
**              Event                  : Enabled
**         High speed mode
**             Prescaler               : divide-by-1
**             Clock                   : 16777216 Hz
**           Initial value of            period     pulse width
**             Xtal ticks              : 33         13
**             microseconds            : 1000       400
**             milliseconds            : 1          0
**             seconds (real)          : 0.000999987125 0.000400006771
**
**     Contents    :
**         SetRatio16 - byte PWM2_SetRatio16(word Ratio);
**         SetDutyUS  - byte PWM2_SetDutyUS(word Time);
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file PWM2.c
** @version 01.28
** @brief
**         This component implements a pulse-width modulation generator
**         that generates signal with variable duty and fixed cycle. 
*/         
/*!
**  @addtogroup PWM2_module PWM2 module documentation
**  @{
*/         


/* MODULE PWM2. */

#include "PE_Error.h"
#include "PWM2.h"

#pragma MESSAGE DISABLE C5703          /* WARNING C5703: Parameter _ declared in function _ but not referenced */
#pragma MESSAGE DISABLE C2705          /* WARNING C2705: Possible loss of data */
#pragma MESSAGE DISABLE C5919          /* WARNING C5919: Conversion of floating to unsigned integral */


/*lint -save  -esym(960,18.4) Disable MISRA rule (18.4) checking. */
typedef union {
  uint16_t Value;
  struct {
    uint8_t Hi;
    uint8_t Lo;
  } BB;
} TRatioValue;
/*lint -restore  +esym(960,18.4) Enable MISRA rule (18.4) checking. */

static TRatioValue ActualRatio;        /* Ratio of L-level to H-level - 16-bit unsigned value */

/* Internal method prototypes */

/*
** ===================================================================
**     Method      :  SetRatio (component PWM)
**
**     Description :
**         The method stores duty value to compare register(s) and sets 
**         necessary bits or (in List mode) call SetReg method for duty 
**         value storing.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SetRatio(void);

/* End of Internal methods declarations */

/*
** ===================================================================
**     Method      :  SetRatio (component PWM)
**
**     Description :
**         The method stores duty value to compare register(s) and sets 
**         necessary bits or (in List mode) call SetReg method for duty 
**         value storing.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SetRatio(void)
{
  if (ActualRatio.Value == 0xFFFFU) {  /* Duty = 100%? */
    TPM3C1V = 0xFFFFU;                 /* Store new value to the compare reg. */
  } else {
    TRatioValue Tmp1, Tmp2;
    uint16_t Result;
    Result = (uint16_t)((uint16_t)ActualRatio.BB.Hi * 0x41U); /* HI * HI */
    Tmp1.Value = (uint16_t)((uint16_t)ActualRatio.BB.Hi * 0x89U); /* HI * LO */
    Result += Tmp1.BB.Hi;
    Tmp2.Value = (uint16_t)((uint16_t)ActualRatio.BB.Lo * 0x41U); /* LO * HI */
    Result += Tmp2.BB.Hi;
    if ((Tmp2.BB.Lo += Tmp1.BB.Lo) < Tmp1.BB.Lo) {
      ++Result;                        /* carry to result */
    }
    Tmp1.Value = (uint16_t)((uint16_t)ActualRatio.BB.Lo * 0x89U); /* LO * LO */
    if ((Tmp1.BB.Hi += Tmp2.BB.Lo) < Tmp2.BB.Lo) {
      ++Result;                        /* carry to result */
    }
    if (Tmp1.BB.Hi >= 0x80U) {
      ++Result;                        /* round */
    }
    TPM3C1V = Result;
  }
}

/*
** ===================================================================
**     Method      :  PWM2_SetRatio16 (component PWM)
**     Description :
**         This method sets a new duty-cycle ratio. Ratio is expressed
**         as a 16-bit unsigned integer number. 0 - FFFF value is
**         proportional to ratio 0 - 100%. The method is available
**         only if it is not selected list of predefined values in
**         <Starting pulse width> property. 
**         Note: Calculated duty depends on the timer possibilities and
**         on the selected period.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Ratio           - Ratio to set. 0 - 65535 value is
**                           proportional to ratio 0 - 100%
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte PWM2_SetRatio16(word Ratio)
{
  ActualRatio.Value = Ratio;           /* Store new value of the ratio */
  SetRatio();                          /* Calculate and set up new appropriate values of the compare and modulo registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM2_SetDutyUS (component PWM)
**     Description :
**         This method sets the new duty value of the output signal.
**         The duty is expressed in microseconds as a 16-bit
**         unsigned integer number.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Time            - Duty to set [in microseconds]
**                      (0 to 1000 us in high speed mode)
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_MATH - Overflow during evaluation
**                           ERR_RANGE - Parameter out of range
** ===================================================================
*/
byte PWM2_SetDutyUS(word Time)
{
  dlong rtval;                         /* Result of two 32-bit numbers multiplication */
  if (Time > 0x03E8U) {                /* Is the given value out of range? */
    return ERR_RANGE;                  /* If yes then error */
  }
  PE_Timer_LngMul((dword)Time, 0x41896E98UL, &rtval); /* Multiply given value and High speed CPU mode coefficient */
  if (PE_Timer_LngHi3(rtval[0], rtval[1], &ActualRatio.Value)) { /* Is the result greater or equal than 65536 ? */
    ActualRatio.Value = 0xFFFFU;       /* If yes then use maximal possible value */
  }
  SetRatio();                          /* Calculate and set up new appropriate values of the compare and modulo registers */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  PWM2_Init (component PWM)
**
**     Description :
**         Initializes the associated peripheral(s) and the components 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PWM2_Init(void)
{
  /* TPM3C1SC: CH1F=0,CH1IE=0,MS1B=1,MS1A=1,ELS1B=1,ELS1A=0,??=0,??=0 */
  setReg8(TPM3C1SC, 0x38U);            /* Set up PWM mode with output signal level high */ 
  ActualRatio.Value = 0x6668U;         /* Store initial value of the ratio */
  SetRatio();                          /* Calculate and set up new values of the compare according to the selected speed CPU mode */
}

/* END PWM2. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
