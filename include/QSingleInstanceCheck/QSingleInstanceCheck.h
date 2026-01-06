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

/**
 * @brief QSingleInstanceCheck can be used to test if your app is already running, or if this
 * instance is the first (or only) one running.
 *
 * Under the hood, it uses shared memory to check if it's the first instance or not, and a local
 * socket (domain socket or named pipe, depending on the platform) to notify the original instance
 * that another one has been started.
 */
class QSingleInstanceCheck : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief QSingleInstanceCheck
     * @param uniqueID Hard code this in your application as this string will identify your
     * instance to others
     * @param parent (Optional) Owner of the object.
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
    
    virtual ~QSingleInstanceCheck() = default;
    
    /**
     * @return True if another instance is running.
     */
    inline bool isAlreadyRunning() const { return !isFirst; }

    /**
     * @brief Sends a message to the original instance.
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

    /**
     * @brief Fired when a second instance called its notify() method.
     */
    void notified();

    void error(QString message);
    
private:
    
    QSharedMemory sharedMemory;
    QLocalServer server;
    QString uniqueID;
    bool isFirst;
};

#endif // QSINGLEINSTANCECHECK_H
