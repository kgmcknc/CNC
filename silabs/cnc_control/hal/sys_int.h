/**
 * @file sys_int.h
 *
 * @copyright 2013 by Sensus. All rights reserved.
 *
 * Functions used by components for control of hardware global interrupts.
 *
 */

#ifndef SYS_INT_H // {
#define SYS_INT_H

#include "em_device.h"
#include "cmsis_gcc.h"


#ifndef INLINE
/// Forces a function to be inlined in GCC.
#define INLINE __attribute__((always_inline)) inline static
#endif

// might want to combine with inline macro
#ifndef UNUSED_OK
#define UNUSED_OK __attribute__((unused))
#endif


/**
 * Stores current interrupt state as returned by Sys_IntQuery() and Sys_IntDisable().
 */
typedef uint32_t Sys_IntFlags_t;

/**
 * Gets the global interrupt state.
 * On ARM, this is the state of PRIMASK.
 */
INLINE Sys_IntFlags_t Sys_IntQuery(void)
{
   return __get_PRIMASK();
}

/**
 * Forces interrupts off.
 * @see Sys_IntDisable()
 */
INLINE void Sys_IntDisableForce(void)
{
   __disable_irq();
}

/**
 * Disable interrupts and return the previous state of interrupts.
 *
 * @return current interrupt state so that it can be restored later.
 *
 * @see Sys_IntEnable()
 */
UNUSED_OK INLINE Sys_IntFlags_t Sys_IntDisable(void)
{
   Sys_IntFlags_t prevState = Sys_IntQuery();
   Sys_IntDisableForce();
   return prevState;
}

/**
 * Forces interrupts on.
 * @see Sys_IntEnable().
 */
UNUSED_OK INLINE void Sys_IntEnableForce(void)
{
   __enable_irq();
}

/**
 * Restores previous interrupt state, as obtained by Sys_IntQuery() or Sys_IntDisable().
 *
 * @param newState state of interrupts after returning
 */
UNUSED_OK INLINE void Sys_IntEnable(Sys_IntFlags_t newState)
{
   __set_PRIMASK(newState);
}

#endif // SYS_INT_H }