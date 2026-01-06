# QSingleInstanceCheck

[![Unit Tests](https://github.com/MrEricSir/QSingleInstanceCheck/workflows/Test/badge.svg)](https://github.com/MrEricSir/QSingleInstanceCheck/actions)

QSingleInstanceCheck can be used to test if your app is already running, or if this instance is the 
first (or only) one running.

Under the hood, it uses shared memory to check if it's the first instance or not, and a local
socket (domain socket or named pipe, depending on the platform) to notify the original instance
that another one has been started.
