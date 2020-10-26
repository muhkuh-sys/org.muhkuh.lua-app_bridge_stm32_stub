	.section .init_code, "ax"

	.syntax unified
	.thumb

	.global start
	.extern __STACK_TOP__


    .func start, start
    .type start, STT_FUNC
start:
	ldr     sp, =__STACK_TOP__

@----------------------------------------------------------------------------
@
@ Clear the .bss section (zero init).
@
	mov     r0, #0
	ldr     r1, =__bss_start__
	ldr     r2, =__bss_end__
clear_bss:
	str     r0, [r1], #4
	cmp     r1, r2
	bmi     clear_bss


@----------------------------------------------------------------------------
@
@ Jump to the main routine.
@ The routine will not return.
@
	b       stub_main


@ =====================================================================
@
@ The exception vectors.
@
@ =====================================================================

.thumb_func
.global vector_NMI
.type vector_NMI, %function
vector_NMI:
	@ For now just stop.
	b       vector_NMI

.thumb_func
.global vector_hard_fault
.type vector_hard_fault, %function
vector_hard_fault:
	@ For now just stop.
	b       vector_hard_fault

.thumb_func
.global vector_memory_management_fault
.type vector_memory_management_fault, %function
vector_memory_management_fault:
	@ For now just stop.
	b       vector_memory_management_fault

.thumb_func
.global vector_bus_fault
.type vector_bus_fault, %function
vector_bus_fault:
	@ For now just stop.
	b       vector_bus_fault

.thumb_func
.global vector_usage_fault
.type vector_usage_fault, %function
vector_usage_fault:
	@ For now just stop.
	b       vector_usage_fault

.thumb_func
.global vector_svc
.type vector_svc, %function
vector_svc:
	@ For now just stop.
	b       vector_svc

.thumb_func
.global vector_debug_monitor
.type vector_debug_monitor, %function
vector_debug_monitor:
	@ For now just stop.
	b       vector_debug_monitor

.thumb_func
.global vector_pend_sv
.type vector_pend_sv, %function
vector_pend_sv:
	@ For now just stop.
	b       vector_pend_sv

.thumb_func
.global vector_sys_tick
.type vector_sys_tick, %function
vector_sys_tick:
	@ For now just stop.
	b       vector_sys_tick

.thumb_func
.global vector_irq
.type vector_irq, %function
vector_irq:
	@ For now just stop.
	b       vector_irq


@----------------------------------------------------------------------------

    .end


