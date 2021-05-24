# file: main.s
.global myStart,myCounter
.section myCode
.equ tim_cfg, 0xFF10
myStart:
 ldr r0, $0x1
 str r0, tim_cfg
wait:
 ldr r0, myCounter
 cmp r0, 5
 jne wait
 halt
.section myData
myCounter:
 .word 0,myCounter,23,0xFF,0xAf42
 .skip 4
 .skip 0xf4F

.end