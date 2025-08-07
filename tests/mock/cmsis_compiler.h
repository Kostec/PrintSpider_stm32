#ifndef __CMSIS_COMPILER_MOCK_H
#define __CMSIS_COMPILER_MOCK_H

#include <stdint.h>

#define __STATIC_INLINE static inline
#ifndef __RAM_FUNC
  #define __RAM_FUNC
#endif
  #define __ASM(...)
  #define __INLINE inline
  #define __NO_RETURN
  #define __NO_RESTRICT
  #define __RESTRICT
  #define __ASM(...)
  #define __weak
  // заглушаем все ARM-барьеры
  #define __ISB(...)
  #define __DSB(...)
  #define __SEV(...)
  #define __NOP(...)
  #define __WFE(...)
  #define __DMB(...)
  #define __WFI(...)

  #define __COMPILER_BARRIER(...)

  #define __disable_irq(...)

  #define S_IFCHR  _S_IFCHR
  #define __LDREXW(...) 0
  #define __STREXW(...) 0


#endif //__CMSIS_COMPILER_MOCK_H

