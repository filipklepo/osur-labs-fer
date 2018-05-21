/*! Interrupt handling - 'arch' layer (only basic operations) */

#define _ARCH_INTERRUPTS_C_
#include "interrupt.h"

#include <arch/processor.h>
#include <kernel/errno.h>
#include <lib/list.h>
#include <kernel/memory.h>
#include <kernel/kprint.h>

/*! Interrupt controller device */
extern arch_ic_t IC_DEV;
static arch_ic_t *icdev = &IC_DEV;

struct ihndlr
{
	void *device;
	int (*ihandler) ( unsigned int, void *device );

	list_h list;
};

struct prio_irq {
	void *device;
	int irq_num;
	int prio;
	uint8 is_processed;
	int (*ihandler) ( unsigned int, void *device );
};

/* Used for list_t's priority-queue-like behaviour */
int cmp_by_prio ( void *fst, void *snd) {
	struct prio_irq *fst_prio_rq = fst, *snd_prio_rq = snd;
	return snd_prio_rq->prio - fst_prio_rq->prio;
}

/*! interrupt handlers */
static list_t ihandlers[INTERRUPTS];
/* priority for each irq_num*/
static int prio[INTERRUPTS];
static list_t prio_requests;

/*! Initialize interrupt subsystem (in 'arch' layer) */
void arch_init_interrupts ()
{
	int i;

	icdev->init ();

	for ( i = 0; i < INTERRUPTS; i++ )
		list_init ( &ihandlers[i] );

	for (i = 0; i < INTERRUPTS; i++ )
		prio[i] = INTERRUPTS - 1 - i;

	list_init( &prio_requests );
}

/*!
 * enable and disable interrupts generated outside processor, controller by
 * interrupt controller (PIC or APIC or ...)
 */
void arch_irq_enable ( unsigned int irq )
{
	icdev->enable_irq ( irq );
}
void arch_irq_disable ( unsigned int irq )
{
	icdev->disable_irq ( irq );
}

/*! Register handler function for particular interrupt number */
void arch_register_interrupt_handler ( unsigned int inum, void *handler,
				       void *device )
{
	struct ihndlr *ih;

	if ( inum < INTERRUPTS )
	{
		ih = kmalloc ( sizeof (struct ihndlr) );
		ASSERT ( ih );

		ih->device = device;
		ih->ihandler = handler;

		list_append ( &ihandlers[inum], ih, &ih->list );
	}
	else {
		LOG ( ERROR, "Interrupt %d can't be used!\n", inum );
		halt ();
	}
}

/*! Unregister handler function for particular interrupt number */
void arch_unregister_interrupt_handler ( unsigned int irq_num, void *handler,
					 void *device )
{
	struct ihndlr *ih, *next;

	ASSERT ( irq_num >= 0 && irq_num < INTERRUPTS );

	ih = list_get ( &ihandlers[irq_num], FIRST );

	while ( ih )
	{
		next = list_get_next ( &ih->list );

		if ( ih->ihandler == handler && ih->device == device )
			list_remove ( &ihandlers[irq_num], FIRST, &ih->list );

		ih = next;
	}
}

/*!
 * "Forward" interrupt handling to registered handler
 * (called from interrupts.S)
 */
void arch_interrupt_handler ( int irq_num )
{
	struct ihndlr *ih;
	struct prio_irq *pi;
	list_h *pi_lh;

	if ( irq_num < INTERRUPTS && (ih = list_get (&ihandlers[irq_num], FIRST)) )
	{
		/* enable interrupts on PIC immediately since program may not
		 * return here immediately */
		if ( icdev->at_exit )
			icdev->at_exit ( irq_num );

		/* Call registered handlers */
		while ( ih )
		{
			//OLD: ih->ihandler ( irq_num, ih->device );
			pi = kmalloc( sizeof( struct prio_irq ) );
			pi_lh = kmalloc(sizeof( list_h ) );

			ASSERT( pi );
			ASSERT( pi_lh );
			pi->device = ih->device;
			pi->irq_num = irq_num;
			pi->prio = prio[irq_num];
			pi->is_processed = 0;
			pi->ihandler = ih->ihandler;
			list_sort_add(&prio_requests, pi, pi_lh, &cmp_by_prio);

			ih = list_get_next ( &ih->list );
		}

		pi = list_get(&prio_requests, FIRST);
		while ( pi && !pi->is_processed )
		{
			pi->is_processed = 1;
			enable_interrupts();
			pi->ihandler ( pi->irq_num, pi->device );
            disable_interrupts();
            list_remove(&prio_requests, FIRST, NULL);
			kfree( pi );
			pi = list_get(&prio_requests, FIRST);
		}
	}

	else if ( irq_num < INTERRUPTS )
	{
		LOG ( ERROR, "Unregistered interrupt: %d - %s!\n",
		      irq_num, icdev->int_descr ( irq_num ) );
		halt ();
	}
	else {
		LOG ( ERROR, "Unregistered interrupt: %d !\n", irq_num );
		halt ();
	}
}
