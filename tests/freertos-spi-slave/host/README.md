## Host - SPI controller side

This is effectively just a "USB to SPI" bridge solution.  I mean, that's a
project on it's own, and maybe we should have looked harder for existing ones,
but this gives us an example of how that could look, and lets us test all the
controller code too.

## API notes
Lol.

## Hardware expectations
As this is the host side, this is pretty much only targetting the "hw1" and
"regresso2" host boards.  The DUT boards are then "all stm32 nucleos"

Build is configured to be used as a DFU payload on boards configured with
[devanlai's dapboot project](https://github.com/devanlai/dapboot)
(One less cable for _using_ as this is the host side)

FreeRTOS for USB, more exercise for that code.
pyusb for the host code.
