#ifndef __VECTORS_CM7_H__
#define __VECTORS_CM7_H__


typedef void (*CM7_VECT_T)(void);


typedef struct CM7_VECTORS_STRUCT
{
	void *pvStackTop;                    /* 0x00 */

	CM7_VECT_T pfnReset;                 /* 0x04 */
	CM7_VECT_T pfnNMI;                   /* 0x08 */
	CM7_VECT_T pfnHardFault;             /* 0x0C */
	CM7_VECT_T pfnMemManageFault;        /* 0x10 */
	CM7_VECT_T pfnBusFault;              /* 0x14 */
	CM7_VECT_T pfnUsageFault;            /* 0x18 */

	unsigned long aulReserved01C[4];     /* 0x1C - 0x28 */

	CM7_VECT_T pfnSVCall;                /* 0x2C */
	CM7_VECT_T pfnDebugMonitor;          /* 0x30 */

	unsigned long ulReserved034;         /* 0x34 */

	CM7_VECT_T pfnPendSV;                /* 0x38 */
	CM7_VECT_T pfnSysTick;               /* 0x3C */

	CM7_VECT_T apfnIRQ[32];              /* 0x40 - 0xBC */
} CM7_VECTORS_T;


#endif  /* __VECTORS_CM7_H__ */
