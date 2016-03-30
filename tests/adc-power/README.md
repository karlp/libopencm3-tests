
For most boards, adc channels 1 and 5 are PA1 and PA5.

Not the F3.  oh no, that board is cray cray.  PA0 and PF4

For boards with a Cortex-M3 or better, debug output is on ITM
channel 0 (Yay!)
For M0/M0+ boards, debug is via a tx only UART on PA9.

https://github.com/libopencm3/libopencm3/pull/471
https://github.com/libopencm3/libopencm3/pull/457
https://github.com/libopencm3/libopencm3/pull/534

https://github.com/libopencm3/libopencm3/issues/456
https://github.com/libopencm3/libopencm3/issues/455
https://github.com/libopencm3/libopencm3/issues/454

