SPI master mode tests

While some of the disco boards have some form of spi device onboard,
which would, on the face of it, make testing easy, it's a different
device on each board, and there are boards without it.

Instead, use a known SPI peripheral on all boards, and require/expect
a known fixed SPI slave device.  Use a soft controller SPI slave device
for the far side, for double the test coverage!

the "trigger" pin is bounced when each iteration of the test code starts,
allowing synchronization with a sigrok script that helps assure that
results are as expected.

Debug is via SWO wherever possible, PA2 (tx only) on less capable cores

Pinouts:
		(red)  (orange) (yellow)(brown)		(black)
board		SCK	MISO	MOSI	CS	periph	trigger
f4-disco	PB13	PB14	PB15	PB12	spi2	PB11
l1-disco	PB13	PB14	PB15	PB12	spi2	PB11


Notes for monitoring with sigrok:
$ sigrok-cli -d fx2lafw -C D0=TRIG,D1=CS,D2=CLK,D3=MISO,D4=MOSI -c samplerate=12Mhz:captureratio=4 --time=150ms  -t TRIG=r -P spi:clk=CLK:miso=MISO:mosi=MOSI -A spi=mosi-data
