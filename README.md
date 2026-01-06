# QSingleInstanceCheck

[![Unit Tests](https://github.com/MrEricSir/QSingleInstanceCheck/workflows/Test/badge.svg)](https://github.com/MrEricSir/QSingleInstanceCheck/actions)

QSingleInstanceCheck can be used to test if your Qt app is already running, or if this instance is 
the second (or nth) instance.

Under the hood, it uses shared memory to check if it's the first instance or not, and a local
socket (domain socket or named pipe, depending on the platform) to notify the original instance
that another one has been started.

## Project Origins

I originally developed for the Fang newsreader to provide a simple cross-playform way to
make sure only a single instance of the app would run. Various other solutions existed, but I
wanted something simpler that would fit into a single header file.
