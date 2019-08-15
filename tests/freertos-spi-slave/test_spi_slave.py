import array
import datetime
import random
import usb.core
import usb.util as uu
import random
import sys

import unittest

VENDOR_ID=0xcafe
PRODUCT_ID=0xcafe

DUT_SERIAL = "myserial"

# Control requests to do stuff....
SS_INIT = 1
SS_INIT2 = 2
SS_TRIGGER = 3
SS_XFER = 10
SS_XFER2 = 12
SS_READ = 11

class find_by_serial(object):
    def __init__(self, serial):
        self._serial = serial

    def __call__(self, device):
        return usb.util.get_string(device, device.iSerialNumber)


class TestSpiBasic(unittest.TestCase):
    def setUp(self):
        self.dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID, custom_match=find_by_serial(DUT_SERIAL))
        self.assertIsNotNone(self.dev, "Couldn't find locm3 gadget0 device")
        self.longMessage = True
        self.dev.set_configuration()  # iirc, this will set to the first/only config
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_TRIGGER, 0, 0, [1])

    def tearDown(self):
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_TRIGGER, 0, 0, [0])
        uu.dispose_resources(self.dev)

    def test_spi_software_nss_output(self):
        # type, request, value, index, length/data (baud divider, cpol, cpha, 8/16, msb/lsb first
        # init master mode, baud = div32 = 1Mhz
        x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_INIT, 0, 0, [5<<3, 0, 0, 0, 0])
        self.assertEqual(x, 5)

        # now write some shit...
        x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER, 0, 0, [0xaa, 0x22, 0x55, 0xa5])
        print("ok, wrote... and got x", x)

        # Write again, should just toss out last received data
        x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER, 0, 0, [0xcc, 0xdd, 0x22, 0x33, 0x55, 0x99, 0x12, 0x34])

        x = self.dev.ctrl_transfer(uu.CTRL_IN | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_READ, 0, 0, x) # fixme -get from write stage!
        print("ok, read, and got x", x)


    def test_spi_hardware_nss_output(self):
        # type, request, value, index, length/data (baud divider, cpol, cpha, 8/16, msb/lsb first
        # init master mode, baud = div32 = 1Mhz
        x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_INIT2, 0, 0, [5<<3, 0, 0, 0, 0])
        self.assertEqual(x, 5)

        # now write some shit...
        x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER2, 0, 0, [0xbb, 0x11, 0xaa, 0xca])
        print("ok, wrote... and got x", x)

        # Write again, should just toss out last received data
        x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER2, 0, 0, [0xca, 0xfe, 0xfa, 0xce, 0xbe, 0xef, 0x12, 0x34])

        x = self.dev.ctrl_transfer(uu.CTRL_IN | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_READ, 0, 0, x) # fixme -get from write stage!
        print("ok, read, and got x", x)

    def test_reg_read_write(self):
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_INIT, 0, 0, [6 << 3, 0, 0, 0, 0])

        def read_reg(reg, delay=0):
            x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER, 0, delay, [reg, 0])
            x = self.dev.ctrl_transfer(uu.CTRL_IN | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_READ, 0, 0, x)
            return x

        print(read_reg(3,1))
        print(read_reg(3,1))
        print(read_reg(3,1))

        # issue a read of register 2
        #x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER, 0, 0, [0x2, 0])
        #print("did a reg 'read' which is a usb write only", x)

        #x = self.dev.ctrl_transfer(uu.CTRL_IN | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_READ, 0, 0, x)
        #print("reading the reply backk in again, got", x)

        # write somethign new back again
        #x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER2, 4, 0, [0x2|0x80, 0, 0xab])
        #print("ok, a new value to a register", x)
        # Read it backk again.
        #x = self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_XFER2, 4, 0, [0x2, 0, 0])
        #print("issued a read of that register", x)
        #x = self.dev.ctrl_transfer(uu.CTRL_IN | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, SS_READ, 0, 0, x)
        #print("ok, read, and got x", x)

    def test_raw_toggle2(self):
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, 4, 0, 0, [1])
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, 4, 0, 0, [0])
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, 4, 0, 0, [1])
        self.dev.ctrl_transfer(uu.CTRL_OUT | uu.CTRL_RECIPIENT_INTERFACE | uu.CTRL_TYPE_VENDOR, 4, 0, 0, [0])


if __name__ == "__main__":
    if len(sys.argv) > 1:
        DUT_SERIAL = sys.argv.pop()
        print("Running tests for DUT: ", DUT_SERIAL)
        unittest.main()
    else:
        # scan for available and try them all!
        devs = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID, find_all=True)
        for dev in devs:
            DUT_SERIAL = dev.serial_number
            print("Running tests for DUT: ", DUT_SERIAL)
            unittest.main(exit=False)
