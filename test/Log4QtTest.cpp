#include "Log4QtTest.h"

static QLoggingCategory Test("test");

int TestProcessor::count() const
{
    return cnt;
}

void TestProcessor::log(QtMsgType, const QMessageLogContext&, const QString&)
{
    ++cnt;
}

Log4QtTest::Log4QtTest()
{
}

Log4QtTest::~Log4QtTest()
{
}

void Log4QtTest::engineProfile()
{
    TestProcessor processor;
    processor.start();
    int i = 0;
    double f = 3.14;
    QBENCHMARK {
        qDebug() << "test" << "engine"
                 << "i=" << i++
                 << "f=" << f++;
    }
}

void Log4QtTest::typeToString()
{
    QCOMPARE(log4qt::typeToString(QtDebugMsg), QStringLiteral("Debug"));
    QCOMPARE(log4qt::typeToString(QtWarningMsg), QStringLiteral("Warning"));
    QCOMPARE(log4qt::typeToString(QtCriticalMsg), QStringLiteral("Critical"));
    QCOMPARE(log4qt::typeToString(QtFatalMsg), QStringLiteral("Fatal"));
    QCOMPARE(log4qt::typeToString(QtInfoMsg), QStringLiteral("Information"));
}

void Log4QtTest::formatMessage()
{
    QMessageLogContext context(__FILE__, __LINE__, Q_FUNC_INFO, "default");
    QString buf = "hello world";
    QString pattern = QStringLiteral("[%1:%2] %3 %4(%5):%6(@%7) %8:%9 %10 - %11")
                      .arg(log4qt::PatternCategory)
                      .arg(log4qt::PatternType)
                      .arg(log4qt::PatternDateTime)
                      .arg(log4qt::PatternAppName)
                      .arg(log4qt::PatternPID)
                      .arg(log4qt::PatternThreadId)
                      .arg(log4qt::PatternThreadPtr)
                      .arg(log4qt::PatternFile)
                      .arg(log4qt::PatternLine)
                      .arg(log4qt::PatternFunction)
                      .arg(log4qt::PatternMessage);
    log4qt::LogMessage message(QtDebugMsg, context, buf);

    QString msg = QStringLiteral("[%1:%2] %3 %4(%5):%6(@%7) %8:%9 %10 - %11")
                  .arg(message.category)
                  .arg(log4qt::typeToString(message.type))
                  .arg(message.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")))
                  .arg(message.appName)
                  .arg(message.pid)
                  .arg(quintptr(message.threadId))
                  .arg(QStringLiteral("0x%1").arg(quintptr(message.threadPtr), sizeof(quintptr)*2, 16, QLatin1Char('0')))
                  .arg(message.file)
                  .arg(message.line)
                  .arg(message.function)
                  .arg(message.message);

    QString result = log4qt::formatMessage(pattern, message);
    QCOMPARE(msg, result);
}

void Log4QtTest::formatMessageProfileWithQtContext()
{
    QMessageLogContext context(__FILE__, __LINE__, Q_FUNC_INFO, "default");
    QString buf = "hello world";
    QString msg;
    QBENCHMARK {
        msg = log4qt::formatMessage(log4qt::DefaultPattern,
                                    QtDebugMsg,
                                    context,
                                    buf);
    }
}

void Log4QtTest::formatMessageProfileWithLogMessage()
{
    QMessageLogContext context(__FILE__, __LINE__, Q_FUNC_INFO, "default");
    QString buf = "hello world";
    QString msg;
    QBENCHMARK {
        msg = log4qt::formatMessage(
                  log4qt::DefaultPattern,
                  log4qt::LogMessage(
                      QtDebugMsg,
                      context,
                      buf)
                  );
    }
}

void Log4QtTest::logDebugOutput()
{
    log4qt::LogDebugOutput processor;

    QCOMPARE(processor.pattern(), log4qt::DefaultPattern);

    QString pattern = QStringLiteral("[%1:%2] %3 %4(%5):%6(@%7) %8:%9 %10 - %11")
                      .arg(log4qt::PatternCategory)
                      .arg(log4qt::PatternType)
                      .arg(log4qt::PatternDateTime)
                      .arg(log4qt::PatternAppName)
                      .arg(log4qt::PatternPID)
                      .arg(log4qt::PatternThreadId)
                      .arg(log4qt::PatternThreadPtr)
                      .arg(log4qt::PatternFile)
                      .arg(log4qt::PatternLine)
                      .arg(log4qt::PatternFunction)
                      .arg(log4qt::PatternMessage);
    processor.setPattern(pattern);
    QCOMPARE(processor.pattern(), pattern);

    QCOMPARE(processor.filter(), QtWarningMsg);
    processor.setFilter(QtDebugMsg);
    QCOMPARE(processor.filter(), QtDebugMsg);
}

void Log4QtTest::logDisplayBuffer()
{
    log4qt::LogDisplayBuffer buffer;
    buffer.start();

    qDebug() << "test" << "engine"
             << "i=" << 1
             << "f=" << 3.14;
    QCOMPARE(buffer.takeAll().size(), 1u);

    qDebug() << "test" << "engine"
             << "i=" << 1
             << "f=" << 3.14;
    QVERIFY(buffer.takeAll().empty());
}

void Log4QtTest::logDisplayPageColor_data()
{
    QTest::addColumn<QString>("property");
    QTest::addColumn<QColor>("color");

    QTest::newRow("debugForeGround1") << "debugForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("debugForeGround2") << "debugForeGround" << QColor(Qt::lightGray);
    QTest::newRow("warningForeGround1") << "warningForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("warningForeGround2") << "warningForeGround" << QColor(Qt::lightGray);
    QTest::newRow("criticalForeGround1") << "criticalForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("criticalForeGround2") << "criticalForeGround" << QColor(Qt::lightGray);
    QTest::newRow("fatalForeGround1") << "fatalForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("fatalForeGround2") << "fatalForeGround" << QColor(Qt::lightGray);
    QTest::newRow("informationForeGround1") << "informationForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("informationForeGround2") << "informationForeGround" << QColor(Qt::lightGray);

    QTest::newRow("debugBackground1") << "debugBackground" << QColor(Qt::darkYellow);
    QTest::newRow("debugBackground2") << "debugBackground" << QColor(Qt::lightGray);
    QTest::newRow("warningBackground1") << "warningBackground" << QColor(Qt::darkYellow);
    QTest::newRow("warningBackground2") << "warningBackground" << QColor(Qt::lightGray);
    QTest::newRow("criticalBackground1") << "criticalBackground" << QColor(Qt::darkYellow);
    QTest::newRow("criticalBackground2") << "criticalBackground" << QColor(Qt::lightGray);
    QTest::newRow("fatalBackground1") << "fatalBackground" << QColor(Qt::darkYellow);
    QTest::newRow("fatalBackground2") << "fatalBackground" << QColor(Qt::lightGray);
    QTest::newRow("informationBackground1") << "informationBackground" << QColor(Qt::darkYellow);
    QTest::newRow("informationBackground2") << "informationBackground" << QColor(Qt::lightGray);
}

void Log4QtTest::logDisplayPageColor()
{
    log4qt::LogDisplayPage page;

    QFETCH(QString, property);
    QFETCH(QColor, color);
    page.setProperty(property.toUtf8().constData(), color);
    QCOMPARE(page.property(property.toUtf8().constData()).value<QColor>(), color);
}

void Log4QtTest::logDisplayPage()
{
    log4qt::LogDisplayPage page;

    QCOMPARE(page.filter(), QtWarningMsg);
    page.setFilter(QtDebugMsg);
    QCOMPARE(page.filter(), QtDebugMsg);

    QCOMPARE(page.maxCount(), 10000u);
    page.setMaxCount(100);
    QCOMPARE(page.maxCount(), 100u);

    log4qt::LogDisplayBuffer buffer;
    buffer.start();
    int i = 0;
    double f = 3.14;
    qDebug() << "test" << "engine"
             << "i=" << i++
             << "f=" << f++;

    page.setBuffer(&buffer);
    QVERIFY(buffer.takeAll().empty());

    qDebug() << "test" << "engine"
             << "i=" << i++
             << "f=" << f++;
    QVERIFY(buffer.takeAll().empty());
}

void Log4QtTest::logDisplayWidgetColor_data()
{
    QTest::addColumn<QString>("property");
    QTest::addColumn<QColor>("color");

    QTest::newRow("debugForeGround1") << "debugForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("debugForeGround2") << "debugForeGround" << QColor(Qt::lightGray);
    QTest::newRow("warningForeGround1") << "warningForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("warningForeGround2") << "warningForeGround" << QColor(Qt::lightGray);
    QTest::newRow("criticalForeGround1") << "criticalForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("criticalForeGround2") << "criticalForeGround" << QColor(Qt::lightGray);
    QTest::newRow("fatalForeGround1") << "fatalForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("fatalForeGround2") << "fatalForeGround" << QColor(Qt::lightGray);
    QTest::newRow("informationForeGround1") << "informationForeGround" << QColor(Qt::darkYellow);
    QTest::newRow("informationForeGround2") << "informationForeGround" << QColor(Qt::lightGray);

    QTest::newRow("debugBackground1") << "debugBackground" << QColor(Qt::darkYellow);
    QTest::newRow("debugBackground2") << "debugBackground" << QColor(Qt::lightGray);
    QTest::newRow("warningBackground1") << "warningBackground" << QColor(Qt::darkYellow);
    QTest::newRow("warningBackground2") << "warningBackground" << QColor(Qt::lightGray);
    QTest::newRow("criticalBackground1") << "criticalBackground" << QColor(Qt::darkYellow);
    QTest::newRow("criticalBackground2") << "criticalBackground" << QColor(Qt::lightGray);
    QTest::newRow("fatalBackground1") << "fatalBackground" << QColor(Qt::darkYellow);
    QTest::newRow("fatalBackground2") << "fatalBackground" << QColor(Qt::lightGray);
    QTest::newRow("informationBackground1") << "informationBackground" << QColor(Qt::darkYellow);
    QTest::newRow("informationBackground2") << "informationBackground" << QColor(Qt::lightGray);
}

void Log4QtTest::logDisplayWidgetColor()
{
    log4qt::LogDisplayWidget widget;

    QFETCH(QString, property);
    QFETCH(QColor, color);
    widget.setProperty(property.toUtf8().constData(), color);
    QCOMPARE(widget.property(property.toUtf8().constData()).value<QColor>(), color);
}

void Log4QtTest::logDisplayWidget()
{
    log4qt::LogDisplayWidget widget;

    QCOMPARE(widget.filter(), QtWarningMsg);
    widget.setFilter(QtDebugMsg);
    QCOMPARE(widget.filter(), QtDebugMsg);

    QCOMPARE(widget.maxCount(), 10000);
    widget.setMaxCount(100);
    QCOMPARE(widget.maxCount(), 100);

    log4qt::LogDisplayBuffer buffer;
    buffer.start();
    int i = 0;
    double f = 3.14;
    qDebug() << "default" << "engine"
             << "i=" << i++
             << "f=" << f++;

    widget.setBuffer(&buffer);
    QVERIFY(buffer.takeAll().empty());

    qDebug() << "default" << "engine"
             << "i=" << i++
             << "f=" << f++;
    QVERIFY(buffer.takeAll().empty());

    QVERIFY(widget.findPage({}));

    QVERIFY(!widget.findPage("test"));
    qCDebug(Test) << "test" << "engine"
                  << "i=" << i++
                  << "f=" << f++;
    qApp->processEvents();
    QVERIFY(widget.findPage("test"));
}

QTEST_MAIN(Log4QtTest)
