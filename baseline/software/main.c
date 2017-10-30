#include <stdio.h>
#include <unistd.h>           // close
#include <fcntl.h>            // O_RDWR, O_SYNC
#include <sys/mman.h>         // PROT_READ, PROT_WRITE

#include "socal/socal.h"      // alt_write
#include "socal/hps.h"
#include "socal/alt_gpio.h"

#include "hps_0.h"            // definitions for LED_PIO and SWITCH_PIO

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )


int main(int argc, char **argv) {
  void *virtual_base;
  volatile unsigned long *led_pio, *switch_pio, i;
  int fd;

  if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
    printf( "ERROR: could not open \"/dev/mem\"...\n" );
    return( 1 );
  }
  virtual_base = mmap( NULL,
		       HW_REGS_SPAN,
		       ( PROT_READ | PROT_WRITE ),
		       MAP_SHARED,
		       fd,
		       HW_REGS_BASE );	
  if( virtual_base == MAP_FAILED ) {
    printf( "ERROR: mmap() failed...\n" );
    close( fd );
    return(1);
  }
  
  led_pio = virtual_base +
    ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + LED_PIO_0_BASE) & ( unsigned long)( HW_REGS_MASK ) );
  switch_pio = virtual_base +
    ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + SWITCH_PIO_1_BASE) & ( unsigned long)( HW_REGS_MASK ) );

  while (1) {
    *led_pio = *switch_pio;
    if (*switch_pio == 0x2AA)
      break;
  }

  *led_pio = 0x3FF;

  if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
    printf( "ERROR: munmap() failed...\n" );
    close( fd );
    return( 1 );
    
  }
  close( fd );
  return 0;
}
