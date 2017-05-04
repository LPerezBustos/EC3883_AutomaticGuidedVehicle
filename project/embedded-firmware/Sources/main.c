/* ###################################################################
**     Filename    : main.c
**     Project     : AGV_v0_2
**     Processor   : MC9S08QE128CLK
**     Version     : Driver 01.12
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2017-02-06, 09:54, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.12
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "TI1.h"
#include "AD1.h"
#include "AS1.h"
#include "PWM1.h"
#include "PWM2.h"
#include "PWM3.h"
#include "PWM4.h"
#include "EInt1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

// CONTROLADOR
#define KP 4              		// ganancia proporcional
#define KI 0              		// ganancia integral
#define INTEGRAL_MAX 1      	// límite superior de la integral del error
#define INTEGRAL_MIN -1      	// límite inferior de la integral del error

// SOME DEFINITIONS
#define DUTY_INI_M1 400
#define DUTY_INI_M2 400
#define LIMIT_VALUE 80

unsigned char estado = 0;

// BOBINAS
unsigned int bobina_1, bobina_2;

// CONTROL PWM
signed int error = 0; 
signed int DT = 0, integral_error = 0; // <-- ** TIENEN QUE SER FLOAT **
unsigned int duty_m1 = 0, duty_m2 = 0, duty_m3 = 0, duty_m4 = 0;
unsigned char modo = ADELANTE; // modo inicial

// SENSOR DE DISTANCIA
unsigned char distance = 0, dist_fir[5], cnt = 0;

// COMUNICACION IR


// SEÑAL DE CRUCE
unsigned char line_deteted = 0;
bool turn_flag = FALSE; // TRUE == non-zero value ; FALSE == 0

// VARIABLES AUXILIARES
word snd = 0;

void main(void){
	/* Write your local variable definition here */
	for(cnt = 0; cnt < 4; cnt++){
		dist_fir[cnt] = 0;
	}
	cnt=0;
	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	/* For example: for(;;) { } */
	for(;;)
	{

		/*
		 *	CICLO DE LA MAQUINA DE ESTADOS
		 *  (0)	DO NOTHING
		 *	(1) LEE SEÑAL ELECTROMAGNETICA DE LAS BOBINAS
		 *	(2) CONTROL PWM DE LOS MOTORES
		 *	(3) LEE DISTANCIA DEL SENSOR SHARP
		 *	(4) COMUNICACION POR INFRAROJO
		 *	(5) EN CASO DE DETECTAR SEÑAL DE CRUCE
		 *	(6) TRANSMISION SERIAL PARA DEBUGGING
		 */
		  
		  switch(estado){
		  	  case ESTADO0:
		  		  estado = LEER_BOBINAS;
		  		// Hello World !
		  		break;
		  	  case LEER_BOBINAS:
		  		(void)AD1_MeasureChan(TRUE, 0x00); // REALIZA MEDICION CANAL_0
		  		(void)AD1_GetChanValue16(0x00,&bobina_1); // OBTIENE VALOR BOBINA_1
		  		bobina_1 = bobina_1 >> 8; // BIT SHIFTING
		  		(void)AD1_MeasureChan(TRUE, 0x01); // REALIZA MEDICION CANAL_1
		  		(void)AD1_GetChanValue8(0x01,&bobina_2); // OBTIENE VALOR BOBINA_2
		  		bobina_2 = bobina_2 >> 8;  // BIT SHIFTING
		  		estado = CONTROL_PWM; // CHANGE STATE
		  		break;
		  	  case CONTROL_PWM:
		  		  error = bobina_1-bobina_2; // RESTA DE SEÑAL EN BOBINAS
		  		  integral_error += error*DT;

		  		  // se limita la salida de los integradores según las cotas definidas antes
		  		  if(integral_error>INTEGRAL_MAX){
		  				integral_error=INTEGRAL_MAX;
		  			} else if(integral_error<INTEGRAL_MIN){
		  				integral_error=INTEGRAL_MIN;
		  			}

		  		  //Gestionamos modo hacia adelante o reversa
		  		  /* FUNCIONAMIENTO ADELANTE/REVERSA
		  		  ADELANTE:
		  		    MOTOR1:
		  		      PWM1 -> Señal PWM  | PWM2 -> NULL
		  		    MOTOR2:
		  		      PWM3 -> Señal PWM  | PWM4 -> NULL

		  		  REVERSA:
		  		    MOTOR1:
		  		      PWM1 -> NULL  | PWM2 -> Señal PWM
		  		    MOTOR2:
		  		      PWM3 -> NULL  | PWM4 -> Señal PWM
		  		  
		  		  TURN_RIGHT: *POR DEFINIR*
		  		    MOTOR1:
		  		      PWM1 ->   | PWM2 -> 
		  		    MOTOR2:
		  		      PWM3 ->   | PWM4 -> 

		  		  TURN_LEFT:  *POR DEFINIR*
		  		    MOTOR1:
		  		      PWM1 ->   | PWM2 -> 
		  		    MOTOR2:
		  		      PWM3 ->   | PWM4 -> 

		  		  DETENER:
		  		    MOTOR1:
		  		      PWM1 -> NULL  | PWM2 -> NULL
		  		    MOTOR2:
		  		      PWM3 -> NULL  | PWM4 -> NULL
		  		  */

		  		  if(modo == ADELANTE){
		  		    duty_m1 = DUTY_INI_M1 - (KP*error + KI*integral_error)/2;
		  		    duty_m2 = 0;
		  		    duty_m3 = DUTY_INI_M2 + (KP*error + KI*integral_error)/2;
		  		    duty_m4 = 0;
		  		  }

		  		  if (modo == REVERSA) {
		  		    duty_m1 = 0;
		  		    duty_m2 = DUTY_INI_M1 - (KP*error + KI*integral_error)/2;
		  		    duty_m3 = 0;
		  		    duty_m4 = DUTY_INI_M2 + (KP*error + KI*integral_error)/2;
		  		  }
		  		  
		  		  if (modo == TURN_RIGHT) {

		  		  }
		  		  
		  		  if (modo == TURN_LEFT) {

		  		  }

		  		  if (modo == DETENER) {
		  		    duty_m1 = 0;
		  		    duty_m2 = 0;
		  		    duty_m3 = 0;
		  		    duty_m4 = 0;
		  		  }
		  		  
		  		(void)PWM1_SetDutyUS(duty_m1);
		  		(void)PWM2_SetDutyUS(duty_m2);
		  		(void)PWM3_SetDutyUS(duty_m3);
		  		(void)PWM4_SetDutyUS(duty_m4);
		  		estado = LEER_DISTANCIA; // CHANGE STATE
		  		break;
		  	  case LEER_DISTANCIA:
		  		while(cnt < 4){ // MOVING AVERAGE FILTER
				  	(void)AD1_MeasureChan(TRUE, 0x02);
				  	(void)AD1_GetChanValue8(0x02,&distance); // DISTANCE ADQUISITION
				  	dist_fir[cnt] = distance;
				  	if(cnt == 3){ // AVERAGE VALUE OF 4 MEASURES
		  				distance = (dist_fir[0]+dist_fir[1]+dist_fir[2]+dist_fir[3])/4;
		  			}
				  	cnt++; // SE AUMENTA EL CONTADOR
		  		}
			  	if(distance > LIMIT_VALUE){
			  		modo = DETENER; // OBJECT DETECTED ! STOP VEHICLE AND WAIT
			  	} else{
			  		modo = ADELANTE; // GO AHEAD !
			  	}
			  	cnt = 0; // SE REINICIA EL CONTADOR
		  		estado = COMUNICACION_IR; // CHANGE STATE
		  		break;
		  	  case COMUNICACION_IR:
		  		
		  		estado = SENAL_CRUCE; // CHANGE STATE
		  		break;
		  	  case SENAL_CRUCE:
		  		if(turn_flag){
		  			modo = DETENER; // LINE DETECTED ! STOP VEHICLE AND WAIT
		  		} else if(!(modo == DETENER)){
		  			modo = ADELANTE; // GO AHEAD !
		  		}
		  		estado = TRANSMISION_SERIAL; // CHANGE STATE
		  		break;
		  	  case TRANSMISION_SERIAL:
		  		snd = 0; // VARIABLE AUXILIAR
				(void)AS1_SendBlock(&bobina_1,2,&snd); // VALOR BOBINA #1		  		  
		  		snd = 0;
			  	(void)AS1_SendBlock(&bobina_2,2,&snd); // VALOR BOBINA #2
		  		snd = 0;
			  	(void)AS1_SendBlock(&duty_m1,2,&snd); // VALOR DUTY CYCLE (us)
		  		snd = 0;
			  	(void)AS1_SendBlock(&duty_m2,2,&snd); // VALOR DUTY CYCLE (us)
		  		snd = 0;
			  	(void)AS1_SendBlock(&duty_m3,2,&snd); // VALOR DUTY CYCLE (us)
		  		snd = 0;
			  	(void)AS1_SendBlock(&duty_m4,2,&snd); // VALOR DUTY CYCLE (us)
			  	(void)AS1_SendChar(distance); // DISTANCIA AL OBJETO - SENSOR SHARP
		  		estado = ESTADO0; // CHANGE STATE
		  		break;
		  	  default :
		  		break;
		  }
	  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} 
