#include <string.h>

#include "vectors_cm7.h"


extern unsigned long __STACK_TOP__[];

void start(void);
void vector_NMI(void);
void vector_hard_fault(void);
void vector_memory_management_fault(void);
void vector_bus_fault(void);
void vector_usage_fault(void);
void vector_svc(void);
void vector_debug_monitor(void);
void vector_pend_sv(void);
void vector_sys_tick(void);
void vector_irq(void);



const CM7_VECTORS_T tVectors __attribute__ ((section(".vector_table"))) =
{
	.pvStackTop = __STACK_TOP__,

	.pfnReset          = start,
	.pfnNMI            = vector_NMI,
	.pfnHardFault      = vector_hard_fault,
	.pfnMemManageFault = vector_memory_management_fault,
	.pfnBusFault       = vector_bus_fault,
	.pfnUsageFault     = vector_usage_fault,

	.aulReserved01C =
	{
		0xffffffffU,
		0xffffffffU,
		0xffffffffU,
		0xffffffffU
	},

	.pfnSVCall = vector_svc,
	.pfnDebugMonitor = vector_debug_monitor,

	.ulReserved034 = 0xffffffffU,

	.pfnPendSV = vector_pend_sv,
	.pfnSysTick = vector_sys_tick,

	.apfnIRQ =
	{
		vector_irq,    /* IRQ  0 */
		vector_irq,    /* IRQ  1 */
		vector_irq,    /* IRQ  2 */
		vector_irq,    /* IRQ  3 */
		vector_irq,    /* IRQ  4 */
		vector_irq,    /* IRQ  5 */
		vector_irq,    /* IRQ  6 */
		vector_irq,    /* IRQ  7 */
		vector_irq,    /* IRQ  8 */
		vector_irq,    /* IRQ  9 */
		vector_irq,    /* IRQ 10 */
		vector_irq,    /* IRQ 11 */
		vector_irq,    /* IRQ 12 */
		vector_irq,    /* IRQ 13 */
		vector_irq,    /* IRQ 14 */
		vector_irq,    /* IRQ 15 */
		vector_irq,    /* IRQ 16 */
		vector_irq,    /* IRQ 17 */
		vector_irq,    /* IRQ 18 */
		vector_irq,    /* IRQ 19 */
		vector_irq,    /* IRQ 20 */
		vector_irq,    /* IRQ 21 */
		vector_irq,    /* IRQ 22 */
		vector_irq,    /* IRQ 23 */
		vector_irq,    /* IRQ 24 */
		vector_irq,    /* IRQ 25 */
		vector_irq,    /* IRQ 26 */
		vector_irq,    /* IRQ 27 */
		vector_irq,    /* IRQ 28 */
		vector_irq,    /* IRQ 29 */
		vector_irq,    /* IRQ 30 */
		vector_irq     /* IRQ 31 */
	}
};
