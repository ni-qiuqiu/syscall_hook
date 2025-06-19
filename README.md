## syscall hook by replace func pointer
> which is suitable for aarch64 linux ,especially Android.

### about cfi
I'm not going to open source for bypass cfi

If you know KernelPatch or SKRoot well enough,pass cfi is very easy


### show time
```
[  211.427014] [hook] openb /dev/__properties__/u:object_r:libc_debug_prop:s0
[  211.427095] [hook] openb /dev/__properties__/u:object_r:heapprofd_prop:s0
[  211.427207] [hook] openb /system/lib64/libnetd_client.so
[  211.428009] [hook] openb /dev/__properties__/u:object_r:build_prop:s0
[  211.428314] [hook] openb /dev/__properties__/u:object_r:vendor_socket_hook_prop:s0
[  211.429727] [hook] openb /dev/__properties__/u:object_r:libc_debug_prop:s0
[  211.429768] [hook] openb /dev/__properties__/u:object_r:heapprofd_prop:s0
[  211.429824] [hook] openb /system/lib64/libnetd_client.so
[  211.430470] [hook] openb /dev/__properties__/u:object_r:build_prop:s0
[  211.430696] [hook] openb /dev/__properties__/u:object_r:vendor_socket_hook_prop:s0
[  211.431693] [hook] openb /dev/kmsg
oriole:/ # uname -rm                                                           
6.1.112-android14-11-g948f34696492 aarch64

```
