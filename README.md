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

## Usage

Include the header file in your project and use it to check if an instance is already running. If it is,
notify the main instance.

We'll assume your project name is `MyApp` for the purpose of this example.

````cpp
#include "QSingleInstanceCheck.h"

// Somewhere in your application startup code...

QSingleInstanceCheck single("MyApp");
if (single.isAlreadyRunning()) {
  // Already running, notify the existing instance and quit.
  single.notify();
  return -1;
}

// Since it's not already running, we must be the main instance. We'll use this signal to get notified
// if and when any additional instances call the notify() method.
QObject::connect(&single, &QSingleInstanceCheck::notified, this, &MyApp::onSecondInstance);

// Somewhere in your application where you handle signals..

// MyApp's signal handler for the notified() signal:
void MyApp::onSecondInstance()
{
  qDebug() << "Another instance started!";
}
````

In your code you may wish to respond to the `notified()` signal to show the window, open a dialog box, etc.
That part is entirely up to you and will depend on your use case.


