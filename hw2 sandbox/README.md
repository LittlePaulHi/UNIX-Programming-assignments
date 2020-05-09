# Play in a Sandbox

- [Sanbox](https://github.com/LittlePaulHi/UNIX-Programming-assignments/blob/master/hw2%20sandbox/sandbox.c) will confine file relevant operations to a user provided directory.
- The shared object ([sandbox.so](https://github.com/LittlePaulHi/UNIX-Programming-assignments/blob/master/hw2%20sandbox/sandboxso.c)) will inject the shared library to check whther an operation attempts to escape from the sandbox.

## Monitored library calls

|Name|
|----|
|chdir|
|chmod|
|fchmodat|
|chown|
|lchown|
|fchownat|
|creat|
|creat64|
|fopen|
|fopen64|
|freopen|
|open|
|open64|
|openat|
|opendir|
|mkdir|
|rmdir|
|rename|
|remove|
|stat|
|__xstat|
|__xstat64|
|link|
|readlink|
|symlink|
|unlink|

## Always reject the use of the following functions

|Name|
|----|
|execl|
|execle|
|execlp|
|execv|
|system|
