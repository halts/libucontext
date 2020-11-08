# `libucontext`

`libucontext` is a library which provides the `ucontext.h` C API without colliding with ucontext.
It is a fork of [kaniini/libucontext][1].


## supported architectures

Adding support for new architectures is easy, but you need to know assembly language to do it.

Right now these archs are supported and should work on bare metal:

 * x86_64
 * aarch64

Other archs are supported in [kaniini/libucontext][1] and can be easily ported over if necessary.


## building

`libucontext` uses a simple makefile build system.  You should define `ARCH=` at build time, otherwise
the build system will attempt to guess using `uname -m`.

```
$ make ARCH=x86_64
$ make ARCH=x86_64 check
$ make ARCH=x86_64 DESTDIR=out install
```

[1]: https://github.com/kaniini/libucontext
