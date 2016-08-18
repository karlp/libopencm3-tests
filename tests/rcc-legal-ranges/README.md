
Tests whether legal clock config structures can actually be used.
This primarily is testing whether the order of steps taken for
turning on and selecting different clocks, power ranges and flash
wait state configuration are robust enough.

## PASSING
The board should issue a series of blinks at various rates before
settling on a steady rate

## FAILING
The board stops blinking at any point in the sequence

