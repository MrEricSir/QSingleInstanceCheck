![Morpheus in The Matrix saying "He is the one"](https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExemRuZnV1eXMwZm54bXg4aXNuajR3cXQ1NTFrMzlibHNlOWp5NHJnbiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/aqLEgEvlGAFZ6zS4E5/giphy.gif)

# QSingleInstanceCheck

[![Unit Tests](https://github.com/MrEricSir/QSingleInstanceCheck/workflows/Test/badge.svg)](https://github.com/MrEricSir/QSingleInstanceCheck/actions)

QSingleInstanceCheck can be used to test if your Qt app is already running, or if this instance is 
the second (or nth) instance. Cross-platform for Windows, macOS, and Linux.

Under the hood, it uses shared memory to check if it's the first instance or not, and a local
socket (domain socket or named pipe, depending on the platform) to notify the original instance
that another one has been started.

## Project Origins

I originally developed this for the [Fang newsreader](https://github.com/MrEricSir/Fang) to provide a simple 
cross-playform way to make sure only a single instance of the app would run. Various other solutions existed, 
but I wanted something simpler that would fit into a single header file.

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

## How To Include

There are two option to include this in your project:

1. Copy and paste the header file
1. Git Submodule

Let's go over these in more detail.

### Copy and paste the header file

Grab the `QSingleInstanceCheck.h` in this project and copy the file into your own. That's all there is to it!

Now this does come with one downside -- there's no easy way to update your copy of the file. Which brings us to:

### Git Submodule

In this example we'll add a submodule to an exiting `git` repo that contains your project. It will live in a
directory called `external/QSingleInstanceCheck`.

In the root directory of your project:

````bash
git submodule add git@github.com:MrEricSir/QSingleInstanceCheck.git external/QSingleInstanceCheck
git add .gitmodules
git commit -m "Add QSingleInstanceCheck submodule"
````

***Note:*** You can then update the header file just as you would with any other submodule. In addition you can 
use the unit tests in your project if you feel so inclined. Both of these are beyond the scope of this document.

Assuming you're using `CMake` to build your project, modify your CMakeLists.txt file

````cmake
# Add QSingleInstanceCheck directory
add_subdirectory(external/QSingleInstanceCheck)

# Add it to MyApp's link libraries
target_link_libraries(MyApp PRIVATE
  QSingleInstanceCheck
  # ...etc.
)
````

With this approach, you can include the header in your C++ code with:

````cpp
include <QSingleInstanceCheck/QSingleInstanceCheck.h>
````

## Contributing

Found a bug? Want to propose an improvement? File [a ticket](https://github.com/MrEricSir/QSingleInstanceCheck/issues)
and/or submit [a pull request](https://github.com/MrEricSir/QSingleInstanceCheck/pulls).

Contributions are welcome!
