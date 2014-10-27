### Overview

This module is used by user to invoking IOCTL to interact with 3.2 kernel.

Currently it support following IOCTLs:

* Activity Query (MIMO\_IOCAQ): it is used to query inactive pages with specific `pid` and `count`. User-level example can be found in `run-aq.sh` and `aqclient.c`.


### AQ IOCTL Environment Setup

##### Change specific kernel function address

in mimodule.c, there're 3 kernel functions addresses in VM\_3\_2 need to be modified:

* my\_page\_get\_anon\_vma
* my\_next\_zones\_zonelist
* my\_size\_to\_hstate

to modified this address, you need to find them in the `/proc/kallsyms`:

    $ cat /proc/kallsyms | grep page_get_anon_vma
    $ cat /proc/kallsyms | grep next_zones_zonelist
    $ cat /proc/kallsyms | grep size_to_hstate

and replace their respective address.

##### Compile and install module

Just execute following command:

    $ ./install_mimo.sh

It will compile the module and other useful program, make a device node and install the module.

##### Run

Run the activity query client program:

    $ ./run_aq.sh 10

`10` means the number of addresses you need to get.

------

### Other IOCTL, keep updating...
