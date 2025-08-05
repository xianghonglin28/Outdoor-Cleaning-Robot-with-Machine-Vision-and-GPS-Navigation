#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

//for definition of IRQn_Type etc.
//#include "stm32f407xx.h" //for HALLIB project
#include "stm32f4xx.h" //for FWLIB project

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define OS_SYS_CLOCK                              (168000000UL)

#define  LOSCFG_KERNEL_PM                         0
//#define  LOSCFG_BASE_CORE_TICK_RESPONSE_MAX       (840000UL)
//#define  LOSCFG_BASE_CORE_TICK_RESPONSE_MAX       (210000UL)  //168/8
#define  LOSCFG_BASE_CORE_TICK_RESPONSE_MAX       (16777216UL)  //2^24
	
#define  LOSCFG_BASE_CORE_SWTMR                   1

#define	LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT       0
	
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _TARGET_CONFIG_H */
