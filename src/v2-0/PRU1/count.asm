; PRU1 - liquid leakage detector
	.define r30.t8,		CLK		    ;P8.27
    .define	r31,        REFL_reg	;P8.28
    .define 10,         REFL_bit
    .define r16,		COUNT

	.global asm_count

asm_count:
	CLR 	CLK
    ZERO	&COUNT, 4

PULSE:
	SET	CLK

WAIT:
	ADD		COUNT, COUNT, 2
	QBBS	WAIT, REFL_reg, REFL_bit

PULSE_DETECTED:
	CLR     CLK
    SBBO	&COUNT, r14, 0, 4		;Store number of pulses
    JMP     r3.w2
