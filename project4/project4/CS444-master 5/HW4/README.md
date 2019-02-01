# Best fit SLOB

Modified ```mm/slob.c``` to implement best fit algorithm.  First fit still present in ```mm/slobf.c```

Additionally, modified ```mm/Makefile```, ```int\Kconfig```, and ```include\linux\slab.h``` in order to account for two different SLOBs.

## Selection

In ```make menuconfig``` go to ```General Setup --> Select SLAB Allocator``` to choose ```SLOB``` for best fit or ```SLAB``` for first fit.

## Testing

On the vm, compile and run ```test_slob.c``` to see fragmentation as percentage of unused allocated memory as percentage of total allocated memory.
