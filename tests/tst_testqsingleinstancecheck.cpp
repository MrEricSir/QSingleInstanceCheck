#include <QString>
#include <QTest>
#include <QSignalSpy>
#include <QUuid>

#include "../include/QSingleInstanceCheck/QSingleInstanceCheck.h"

class TestQSingleInstanceCheck : public QObject
{
    Q_OBJECT

public:
    TestQSingleInstanceCheck();

private slots:
    void cleanup();

    // Test cases
    void testFirstInstance();
    void testSecondInstance();
    void testNotifyFromSecondInstance();
    void testNotifyWithArguments();
    void testUniqueIDsIndependent();
    void testMultipleNotifications();

private:
    QString generateUniqueID();
};

TestQSingleInstanceCheck::TestQSingleInstanceCheck()
{
}

void TestQSingleInstanceCheck::cleanup()
{
    // Small delay to ensure shared memory is released
    QTest::qWait(100);
}

QString TestQSingleInstanceCheck::generateUniqueID()
{
    // Generate a unique ID for each test to avoid conflicts
    return QUuid::createUuid().toString();
}

// Test that the first instance is detected.
void TestQSingleInstanceCheck::testFirstInstance()
{
    QString id = generateUniqueID();
    QSingleInstanceCheck* check = new QSingleInstanceCheck(id);

    // First instance should not be "already running"
    QVERIFY(!check->isAlreadyRunning());

    delete check;
}

// Test that a second instance is detected.
void TestQSingleInstanceCheck::testSecondInstance()
{
    QString id = generateUniqueID();
    QSingleInstanceCheck* first = new QSingleInstanceCheck(id);

    QVERIFY(!first->isAlreadyRunning());

    // Create second instance with same ID
    QSingleInstanceCheck* second = new QSingleInstanceCheck(id);

    // Second instance should detect the first
    QVERIFY(second->isAlreadyRunning());

    delete second;
    delete first;
}

// Test that notify() from second instance triggers notified() on first
void TestQSingleInstanceCheck::testNotifyFromSecondInstance()
{
    QString id = generateUniqueID();
    QSingleInstanceCheck* first = new QSingleInstanceCheck(id);
    QSignalSpy spy(first, &QSingleInstanceCheck::notified);

    QVERIFY(!first->isAlreadyRunning());

    // Create second instance
    QSingleInstanceCheck* second = new QSingleInstanceCheck(id);
    QVERIFY(second->isAlreadyRunning());

    // Notify from second instance (no arguments)
    second->notify();

    // First instance should receive the notification with an empty argument list
    QVERIFY(spy.wait(5000));
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.at(0).at(0).toStringList().isEmpty());

    delete second;
    delete first;
}

// Test that notify() with arguments delivers those arguments to the first instance
void TestQSingleInstanceCheck::testNotifyWithArguments()
{
    QString id = generateUniqueID();
    QSingleInstanceCheck* first = new QSingleInstanceCheck(id);
    QSignalSpy spy(first, &QSingleInstanceCheck::notified);

    QVERIFY(!first->isAlreadyRunning());

    QSingleInstanceCheck* second = new QSingleInstanceCheck(id);
    QVERIFY(second->isAlreadyRunning());

    // Notify with arguments
    QStringList args = {"feed://example.com/rss", "--background"};
    second->notify(args);

    QVERIFY(spy.wait(5000));
    QCOMPARE(spy.count(), 1);

    QStringList received = spy.at(0).at(0).toStringList();
    QCOMPARE(received, args);

    delete second;
    delete first;
}

// Test that different unique IDs don't interfere with each other
void TestQSingleInstanceCheck::testUniqueIDsIndependent()
{
    QString id1 = generateUniqueID();
    QString id2 = generateUniqueID();

    QSingleInstanceCheck* check1 = new QSingleInstanceCheck(id1);
    QSingleInstanceCheck* check2 = new QSingleInstanceCheck(id2);

    // Both should be first instances since they have different IDs
    QVERIFY(!check1->isAlreadyRunning());
    QVERIFY(!check2->isAlreadyRunning());

    delete check2;
    delete check1;
}

// Test multiple notifications from second instance
void TestQSingleInstanceCheck::testMultipleNotifications()
{
    QString id = generateUniqueID();
    QSingleInstanceCheck* first = new QSingleInstanceCheck(id);
    QSignalSpy spy(first, &QSingleInstanceCheck::notified);

    QSingleInstanceCheck* second = new QSingleInstanceCheck(id);
    QVERIFY(second->isAlreadyRunning());

    // Send multiple notifications
    second->notify();
    second->notify();
    second->notify();

    // Wait for all notifications
    QTest::qWait(500);

    // Should have 3 notifications
    QCOMPARE(spy.count(), 3);

    delete second;
    delete first;
}

QTEST_MAIN(TestQSingleInstanceCheck)

#include "tst_testqsingleinstancecheck.moc"
