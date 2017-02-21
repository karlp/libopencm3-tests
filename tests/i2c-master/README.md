i2c master mode tests.

While many of the disco boards have some form of i2c device onboard,
which would, on the face of it, make testing easy, it's a different
device on each board, and there are boards without it.

Instead, use a known I2C peripheral on all boards, and require/expect
a known fixed i2c slave device.  (Eventually, this will be a soft
controllable i2c slave in the auto test setup ;)

Debug is via SWO wherever possible, PA2 (tx only) on less capable cores

Pinouts:

board		SCLK	SDA
f4-disco	PB8	PB9	i2c1

