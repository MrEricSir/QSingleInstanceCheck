// MIT License

// Copyright (c) 2026 Eric Gregory

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef QSINGLEINSTANCECHECK_H
#define QSINGLEINSTANCECHECK_H

#include <QSharedMemory>
#include <QString>
#include <QLocalServer>
#include <QLocalSocket>

// When running QDoc we have to ignore Q_OBJECT and signals: to generate header-only docs.
#ifdef Q_QDOC
#define Q_OBJECT
#define signals public
#endif

/*!
    \module QSingleInstanceCheck
    \title QSingleInstanceCheck C++ Classes
    \brief Header-only library for checking if an application is running as a single instance.
*/

/*!
    \class QSingleInstanceCheck
    \inmodule QSingleInstanceCheck

    The QSingleInstanceCheck class can be used to test if your app is already running, or if this
    instance is the first (or only) one running.

    This library consists of a header only.

    Under the hood, QSingleInstanceCheck uses shared memory to check if it's the first instance or
    not, and a local socket (domain socket or named pipe, depending on the platform) to notify the
    original instance that another one has been started.
*/
class QSingleInstanceCheck : public QObject
{
    Q_OBJECT

public:
    /*!
        \fn inline QSingleInstanceCheck::QSingleInstanceCheck(const QString uniqueID, QObject* parent = nullptr)

        QConstructs a SingleInstanceCheck

        All instances must use the same \a uniqueID to differentiate themselves from other
        applications. Hard code this in your application. It is recommended not to use special
        characters for maximum compatibility.

        Optionally, the \a parent is the owner of the object.
     */
    inline QSingleInstanceCheck(const QString uniqueID, QObject* parent = nullptr) :
        QObject(parent), uniqueID(uniqueID), isFirst(false)
    {
        QObject::connect(&server, &QLocalServer::newConnection,
                         this, &QSingleInstanceCheck::notified);

        sharedMemory.setKey(uniqueID);
        if (sharedMemory.create(1)) {
            // Delete any existing server (for *nix systems) and start our named pipe server.
            server.removeServer(uniqueID);
            if (!server.listen(uniqueID)) {
                emit error("Unable to listen to server: " + server.errorString());
            } else {
                // Shared memory and named pipe server are ready and we're the first instance.
                isFirst = true;
            }
        }
    }
    
    /*!
        \fn virtual QSingleInstanceCheck::~QSingleInstanceCheck()

        Default deconstructor.
     */
    virtual ~QSingleInstanceCheck() = default;
    
    /*!
       \fn inline bool QSingleInstanceCheck::isAlreadyRunning() const

       Returns \c true if another instance is running.
     */
    inline bool isAlreadyRunning() const { return !isFirst; }

    /*!
        \fn inline void QSingleInstanceCheck::notify()

        Sends a notification to the original instance. If this is the original instance, it's
        a no-op.

        \sa notified()
     */
    inline void notify()
    {
        if (isFirst) {
            return;
        }

        QLocalSocket socket;
        socket.connectToServer(uniqueID);
    }

signals:

    /*!
        \fn void QSingleInstanceCheck::notified()

        This signal is emitted in the main instance when a subsequent instance called its
        notify() method.

        \sa notify()
     */
    void notified();

    /*!
        \fn void QSingleInstanceCheck::error(QString message)

        The error signal is emitted when the local server or failed memory failed to
        initialize. The \a message provides details on what went wrong.
     */
    void error(QString message);
    
private:
    
    QSharedMemory sharedMemory;
    QLocalServer server;
    QString uniqueID;
    bool isFirst;
};

#endif // QSINGLEINSTANCECHECK_H
