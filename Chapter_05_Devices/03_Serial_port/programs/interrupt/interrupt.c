/* Demonstarting interrupts with priority. */

#include <stdio.h>
#include <time.h>
#include <api/prog_info.h>
#include <arch/processor.h>
#include <arch/interrupt.h>

void higher_prio_handler() {
    printf("HIGH PRIO START\n");
    for (int i = 0; i < 1000000000; i++);
    printf("HIGH PRIO END\n");
}

void lower_prio_handler() {
    printf("LOW PRIO START\n");
    for (int i = 0; i < 1000000000; i++);
    printf("LOW PRIO END\n");
}

void lower_prio_raise() {
    printf("called lower prio raise!\n");
    raise_interrupt(40);
}

void higher_prio_raise() {
    printf("called higher prio raise!\n");
    raise_interrupt(37);
}

int interrupt() {
    arch_register_interrupt_handler(37, higher_prio_handler, NULL);
    arch_register_interrupt_handler(40, lower_prio_handler, NULL);

    sigevent_t evp, evp2;
    timer_t timer, timer2;
    itimerspec_t t, t2;
    timespec_t ts;

    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = lower_prio_raise;
    evp.sigev_notify_attributes = NULL;

    t.it_interval.tv_sec = 0;
    t.it_interval.tv_nsec = 0;
    t.it_value.tv_sec = 3;
    t.it_value.tv_nsec = 0;

    timer_create ( CLOCK_REALTIME, &evp, &timer );
    timer_settime ( &timer, 0, &t, NULL );

    evp2.sigev_notify = SIGEV_THREAD;
    evp2.sigev_notify_function = higher_prio_raise;
    evp2.sigev_notify_attributes = NULL;

    t2.it_interval.tv_sec = 0;
    t2.it_interval.tv_nsec = 0;
    t2.it_value.tv_sec = 5;
    t2.it_value.tv_nsec = 0;

    timer_create ( CLOCK_REALTIME, &evp2, &timer2 );
    timer_settime ( &timer2, 0, &t2, NULL );

    ts.tv_sec = 26;
    ts.tv_nsec = 0;

    while ( TIME_IS_SET (&ts) )
        clock_nanosleep ( CLOCK_REALTIME, 0, &ts, &ts );

    return 0;
}