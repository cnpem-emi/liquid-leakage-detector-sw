#define OWN_RAM              		0x000
#define OTHER_RAM           		0x020
#define SHARED_RAM          	 	0x100

#define PULSE_REQUEST			0
#define PULSE_LOOPS			1

#define CLK				r30.t15		// P8.11
#define REFL				r31.t14		// P8.16

#define A				r16
#define B				r17
#define	COUNT				r18
#define I				r19
#define	J				r20
#define TIMEOUT_REG			r21


.setcallreg r29.w0					// PC saving register (CALL instructions)				
.origin 0						// start of program in PRU memory
.entrypoint START					// program entry point (for a debugger)

#include "Reflexao.hp"
#define AM33XX


START:	 
// ----- Initial Configuration --------------------------------------------------------------------

// Clear CLK
	CLR	CLK
 
//  Shared Memory Config
	SHRAM_CONFIG
					

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// :::::::::::::::::::::::::::::::: START :::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	
// ----- PULSE REQUEST --------------------------------------------------------------------
REQUEST: 
	ZERO	&COUNT,4
	ZERO	&A, 4 	
	ZERO	&I, 4 
        LBCO    I, SHRAM_BASE, PULSE_REQUEST, 1         // Load Pulse Request Status
	QBEQ	REQUEST,I,A				// Wait until != 0
	
// ----- SEND A PULSE --------------------------------------------------------------------
	SET	CLK
	
// ----- WAIT FOR REFLEXION --------------------------------------------------------------------
WAIT:
	ADD	COUNT,COUNT,2
	QBBS	WAIT,REFL

// ----- REFLEXION DETECTED --------------------------------------------------------------------
PULSE_DETECTED:
	SBCO	COUNT,SHRAM_BASE,PULSE_LOOPS,4		// Store number of pulses 

// ----- FINAL --------------------------------------------------------------------
FINAL:
	SBCO	A,SHRAM_BASE,PULSE_REQUEST,1		// Clear Pulse request
	CLR	CLK					// Clear CLK

	
	JMP	REQUEST
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


// Never reached	
END:
	HALT
	
	

	
	
