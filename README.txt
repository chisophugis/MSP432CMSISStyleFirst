This "first" started out as a "first" with MSP432. But I also am using
it to play with the CC1101.


The "CMSISStyleFirst" is because CCS showed two options for the
project template, one which was "CMSIS style" and the "regular" one.
I tried both but I ultimately was more comfortable with the
CMSIS-style one because it naturally groups the various peripherals in
a nice way at the language level. E.g.
  DIO->rPAOUT.b.bP1OUT ^= BIT0;
instead of
  P1OUT ^= BIT0;
which (at least at first) helps to keep all the peripherals (each one
is accessed through a top-level pointer, e.g. `DIO` accesses the
digital IO peripheral registers instead of having a bunch of random
macros, one for each register).
Now, I'm a bit more on the fence about the benefit of this, primarily
because the structs that are used for the peripherals are insane. As
you can see above: `DIO->rPAOUT.b.bP1OUT`. The need to redundantly go
through `rPAOUT` and then `.b` is just sort of crazy (yes, P1 is half
of PA, but still...).
Most of the peripherals seem to have a problem like this. E.g. for the
port mapping, I found myself doing something like this:

  volatile uint8_t *p2 = (volatile uint8_t *)&PMAP->rP2MAP01;
  p2[5] = PM_UCA1CLK;
  p2[6] = PM_UCA1SIMO;
  p2[7] = PM_UCA1SOMI;

If you look in the documentation, you can see that these PMAP
registers are logically an array of *bytes*. But the `PMAP` struct has
two-byte fields e.g. `->rP2MAP01`, which was requiring crazy
error-prone stuff like
  PMAP->rP2MAP45 = (PMAP->rP2MAP45 & 0x00FF) | (PM_UCA1CLK << 8);
or similar nonsense.
