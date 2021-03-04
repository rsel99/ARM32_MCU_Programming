.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.data
.align 4
.global value
value:  .word 0
.global source
source: .word 1, 2, 2, 4, 5, 9, 12, 8, 9, 10, 11
.align 1
.global str
str: 	.asciz "HELLO, 01234 WorlD! 56789+-\0"

.text
.global intsub
intsub:
	push {r4-r7, lr}
	for1:
		movs r0, #0							// i
	check1:
		cmp r0, #10
		bge endfor1
	body1:
		if1:
			movs r1, #2
			ands r1, r0
			cmp r1, #2

			bne else1
		then1:
			ldr r1, =value
			ldr r2, [r1]					// value

			ldr r1, =source
			movs r5, #4
			muls r5, r0
			ldr r3, [r1, r5]				// source[i]

			adds r5, #4
			ldr r4, [r1, r5]				// source[i+1]

			subs r4, r3						// source[i+1] - source[i]
			adds r2, r4						// value + source[i+1] - source[i]

			ldr r1, =value
			str r2, [r1]

			b endif1
		else1:
			ldr r1, =value
			ldr r2, [r1]					// value

			ldr r1, =source
			movs r5, #4
			muls r5, r0
			ldr r3, [r1, r5]				// source[i]

			adds r2, r3

			ldr r1, =value
			str r2, [r1]
		endif1:
	next1:
		adds r0, #1
		b check1
	endfor1:
		pop {r4-r7, pc}

.global charsub
charsub:
	push {r4-r7, lr}
	for2:
		movs r0, #0
	check2:
		ldr r1, =str
		ldrb r2, [r1, r0]
		cmp r2, #0
		beq endfor2
	body2:
		if2:
			cmp r2, #0x41
			blt endif2

			cmp r2, #0x5a
			bgt endif2
		then2:
			movs r2, #0x3d
			strb r2, [r1, r0]
		endif2:
	next2:
		adds r0, #1
		b check2
	endfor2:
	pop {r4-r7, pc}

.global login
login: .string "selvara2" // Make sure you put your login here.
.align 2
.global main
main:
    bl autotest
    bl intsub
    bl charsub
    bkpt


