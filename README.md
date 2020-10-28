# Command
 - `strace [cmd]`: trace system calls of [cmd].
 - `insmod [module].ko`: load the driver.
 - `lsmod`: check drivers.
 - `rmmod [module]`: unload the driver, [module] can be seen in `lsmod`.
 - `du -sh`: check current directory size.

# Note
 - `di, si, dx, r10, r8, r9`: [pt_regs structure](https://www.codenong.com/cs106088896/)

# Progress
 - Overload `sys_openat()` to block `ls`, `vim`, `cat`, `cp`, `touch`, but users can still use `cd`, `mkdir`, `ln`, `ln -s`
 - Overload `sys_chdir()` to block `cd`
 - Overload `sys_mkdir()` to block `mkdir`
 - Overload `sys_symlinkat()` to block `ln -s`
 - Overload `symlinkat()` to block `ln`

# Error
