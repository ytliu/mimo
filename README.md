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

    $ make install

It will compile the module and other useful program, make a device node and install the module.

##### Run

Run the activity query client program:

    $ ./run_aq.sh 10

`10` means the number of addresses you need to get.

------

### Other IOCTL, keep updating...
