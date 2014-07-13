Electronic timer
=====

A simple PIC based electronic timer

Supports daily timers with optional recuring on/off periods


Licence
--------
FreeBSD License. See licence.txt


Notes
------
Initial version. This still needs some work before I'll be happy with it. Documentation is also not yet complete.

NB this is the first version of my first embedded-C app (first C code I've written in +12 years) so there is a lot that I would like to improve.

E.g.
 - Using interrupts rather than polling for button presses
 - Cleanup the state machines
 - General refactoring


Unit tests
------

The unit tests are written with SeaTest (https://code.google.com/p/seatest/) and can be run with the _test/test bash script.
