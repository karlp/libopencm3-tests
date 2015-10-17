# Cross target tests for libopencm3

libopencm3(http://github.com/libopencm3/libopencm3) pull requests are often
hard to review.  It can be hard to see the impact on "user" land code, it can
be hard to see how the behaviour changes from a->b.

This repository contains testing frameworks I've put together to help with
cross target testing, with the aim of making them as automated as possible.

General plan is something like....

1. make -C libopencm3 clean all -j8
2. make -C tests/xxx clean all flash test V=1
3. cd libopencm3
4. git checkout proposed-new-code
5. return to step 1

Mileage _will_ vary.

