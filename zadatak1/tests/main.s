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
 push r0
 add r0 , r5
 ldr r0 , $myStart
 ldr r1 , myStart
 ldr r2 , %myStart
 ldr r4 , 0x5A
 ldr r5 , $55
 ldr r6 , psw
 ldr r1 , [r1]
 ldr r2 , [ r2+ 54]
 ldr r3 , [r3 + 0xDA]
 ldr r4 , [ r4 +  myStart ]
 jmp *[ r3 ]
 jeq *[r2 + 0x51]
 jne *[ pc+myStart ]
 jne myStart
 jmp 0x5D
 jmp %myStart
 jmp *5432
 jmp *myStart
 jmp *pc
 halt
.section myData
myCounter:
 .word 0,myCounter,23,0xFF,0xAf42
 .skip 4
 .skip 0xf4F

.end