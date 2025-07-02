.cpu    cortex-m0
.fpu    softvfp
.syntax unified
.thumb
.text

.extern _mc_cur_sp
.extern mc_update_sp

.global _mc_start
.global _mc_yield

_mc_yield:
    MOV                 R3,R11
    MOV                 R2,R10
    MOV                 R1,R9
    MOV                 R0,R8
    PUSH                {R0-R7, LR}

    MRS R0, PSP
    LDR R1, =_mc_cur_sp
    STR R0, [R1]

    BL mc_update_sp
_mc_pop_stack:
    LDR R0, =_mc_cur_sp
    LDR R0, [R0]
    MSR PSP, R0

    POP                 {R0-R7}
    MOV                 R8,R0
    MOV                 R9,R1
    MOV                 R10,R2
    MOV                 R11,R3
    POP                 {PC}
    
_mc_start:
    LDR         R0, =_elos_cur_sp
    LDR         R0, [R0]
    MSR         PSP, R0
    MOVS        R0, #0x02
    MSR         CONTROL, R0
    ISB
    B           _mc_pop_stack 
