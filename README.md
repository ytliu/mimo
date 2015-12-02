### Overview

This module is used by user to invoking IOCTL to interact with kernel.

Currently it support following IOCTLs:

* Activity Query (MIMO\_IOCAQ): it is used to query inactive pages with specific `pid` and `count`. User-level example can be found in `run-aq.sh` and `aqclient.c`.
* PTE Manipulation (MIMO\_IOCPM): it is used to read PTE and set|clear access bit in PTE with specific `addr` and `ent`. User-level example can be found in `run-pm.sh` and `pmclient.c`.
* Information Query (MIMO\_IOCIQ): it is used to query some system information, especially read from specific MSR. Parameter is the No. for specific information. User-level example can be found in `run-iq.sh` and `iqclient.c`.


### AQ IOCTL Environment Setup

##### Change specific kernel function address

in mimodule.c, there're 5 kernel functions addresses need to be modified:

* my\_page\_lock\_anon\_vma\_read
* my\_next\_zones\_zonelist
* my\_anon\_vma\_interval\_tree\_iter\_first
* my\_size\_to\_hstate
* my\_anon\_vma\_interval\_tree\_iter\_next

to modified this address, you need to find them in the `/proc/kallsyms`:

    $ cat /proc/kallsyms | grep page_lock_anon_vma_read
    $ cat /proc/kallsyms | grep next_zones_zonelist
    $ cat /proc/kallsyms | grep anon_vma_interval_tree_iter_first
    $ cat /proc/kallsyms | grep size_to_hstate
    $ cat /proc/kallsyms | grep anon_vma_interval_tree_iter_next

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

### PM IOCTL Environment Setup

##### Compile and install module

Just execute following command:

    $ ./install_mimo.sh

It will compile the module and other useful program, make a device node and install the module.

##### Run

Initialize the `addr` and `ent` in `pmclient.c` file.

After that, run the PTE manipulation client program:

    $ ./run_pm.sh [pid] 

if `pid` is 0, then the module just take the current process.

------

### IQ IOCTL Environment Setup

##### Compile and install module

Just execute following command:

    $ ./install_mimo.sh

It will compile the module and other useful program, make a device node and install the module.

##### Run

After that, run the information query client program:

    $ ./run_iq.sh [type] [arg] 

`type` is the type of specific information, `arg` is the specific parameter for each type of information. You can find the definition in `mimo.h`.

------

### Other IOCTL, keep updating...
