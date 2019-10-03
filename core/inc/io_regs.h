#ifndef __IO_REGS_H__
#define __IO_REGS_H__

#define IOREG_SB   0xFF01
#define IOREG_SC   0xFF02
#define IOREG_DIV  0xFF04
#define IOREG_TIMA 0xFF05
#define IOREG_TMA  0xFF06
#define IOREG_TAC  0xFF07
#define IOREG_IF   0xFF0F
#define IOREG_IE   0xFFFF

#define IOREG_TAC_STOP_MASK      0x04
#define IOREG_TAC_CLOCK_SEL_MASK 0x03
#define IOREG_TAC_STOP           0x00
#define IOREG_TAC_START          0x04
#define IOREG_TAC_CLOCK_4_KHZ    0x00
#define IOREG_TAC_CLOCK_256_KHZ  0x01
#define IOREG_TAC_CLOCK_64_KHZ   0x02
#define IOREG_TAC_CLOCK_16_KHZ   0x03

#define IOREG_IF_VBLANK_BIT 0x01
#define IOREG_IF_LCDC_BIT   0x02
#define IOREG_IF_TIMER_BIT  0x04
#define IOREG_IF_SERIAL_BIT 0x08
#define IOREG_IF_PIO_BIT    0x10

#endif /* __IO_REGS_H__ */
