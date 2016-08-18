
Tests whether legal clock config structures can actually be used.
This primarily is testing whether the order of steps taken for
turning on and selecting different clocks, power ranges and flash
wait state configuration are robust enough.

## PASSING
On boot you should get two slow blinks.
After boot, you should get continuous 5/2 fast/slow blinks as it
continuously shuffles and resets clock configurations.

## FAILING
The board stops blinking at any point in the sequence

