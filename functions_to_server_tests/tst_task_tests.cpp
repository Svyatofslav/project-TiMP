#include <QTest>
#include "../Server/functions_to_server.h"
#include <cmath>

class tasks_test : public QObject
{
    Q_OBJECT

public:
    tasks_test();
    ~tasks_test();

private slots:
    void test_solver_task1_data();
    void test_solver_task1();

    void test_solver_task2_data();
    void test_solver_task2();

    void test_solver_task3_data();
    void test_solver_task3();

    void test_solver_trapezoid_data();
    void test_solver_trapezoid();

    void test_solver_simpson_data();
    void test_solver_simpson();

    void test_funcName_task1_data();
    void test_funcName_task1();

    void test_get_random_task1();
    void test_get_random_task3();
    void test_get_random_task4();
};

tasks_test::tasks_test() {}
tasks_test::~tasks_test() {}


// solver_task1 - метод средних прямоугольников


void tasks_test::test_solver_task1_data()
{
    QTest::addColumn<QString>("params");
    QTest::addColumn<double>("expected");
    QTest::addColumn<double>("eps");

    // f(x)=x^2, [0,1], n=1000 - точное 1/3
    QTest::newRow("x^2 [0,1] n=1000")
        << "0||0||100||1000" << (1.0/3.0) << 0.0001;

    // f(x)=x^3, [0,1], n=1000 - точное 0.25
    QTest::newRow("x^3 [0,1] n=1000")
        << "1||0||100||1000" << 0.25 << 0.0001;

    // f(x)=2x+1, [0,1], n=10 - точное 2.0 (метод средних точен для линейных)
    QTest::newRow("2x+1 [0,1] n=10 exact")
        << "2||0||100||10" << 2.0 << 1e-10;

    // f(x)=sin(x), [0,π], n=1000 - точное 2.0
    QTest::newRow("sin(x) [0,pi] n=1000")
        << "3||0||314||1000" << 2.0 << 0.001;

    // f(x)=x^2, n=1 - один прямоугольник по середине: 0.5^2 * 1 = 0.25
    QTest::newRow("x^2 [0,1] n=1 single rect")
        << "0||0||100||1" << 0.25 << 1e-10;
}

void tasks_test::test_solver_task1()
{
    QFETCH(QString, params);
    QFETCH(double, expected);
    QFETCH(double, eps);

    double result = solver_task1(params);
    QVERIFY2(std::abs(result - expected) < eps,
             qPrintable(QString("solver_task1(%1): got %2, expected %3, eps %4")
                            .arg(params)
                            .arg(result, 0, 'f', 8)
                            .arg(expected, 0, 'f', 8)
                            .arg(eps)));
}


// solver_task2 - метод левых прямоугольников


void tasks_test::test_solver_task2_data()
{
    QTest::addColumn<QString>("params");
    QTest::addColumn<double>("expected");
    QTest::addColumn<double>("eps");

    // f(x)=x^2, [0,1], n=10000 - 0.33
    QTest::newRow("x^2 [0,1] n=10000")
        << "0||0||100||10000" << (1.0/3.0) << 0.0001;

    // f(x)=x^3, [0,1], n=10000 - 0.25
    QTest::newRow("x^3 [0,1] n=10000")
        << "1||0||100||10000" << 0.25 << 0.0001;

    // f(x)=2x+1, [0,2], n=1000 - точное 6.0
    QTest::newRow("2x+1 [0,2] n=1000")
        << "2||0||200||1000" << 6.0 << 0.01;

    // f(x)=x^2, n=1 - левый на [0,1]: f(0)*1 = 0
    QTest::newRow("x^2 [0,1] n=1 left = 0")
        << "0||0||100||1" << 0.0 << 1e-10;

    // f(x)=x^2, [1,2], n=1 - левый: f(1)*1 = 1.0
    QTest::newRow("x^2 [1,2] n=1 left = 1")
        << "0||100||200||1" << 1.0 << 1e-10;
}

void tasks_test::test_solver_task2()
{
    QFETCH(QString, params);
    QFETCH(double, expected);
    QFETCH(double, eps);

    double result = solver_task2(params);
    QVERIFY2(std::abs(result - expected) < eps,
             qPrintable(QString("solver_task2(%1): got %2, expected %3, eps %4")
                            .arg(params)
                            .arg(result, 0, 'f', 8)
                            .arg(expected, 0, 'f', 8)
                            .arg(eps)));
}


// solver_task3 - сходимость несобственного интеграла (да/нет)


void tasks_test::test_solver_task3_data()
{
    QTest::addColumn<QString>("params");
    QTest::addColumn<QString>("expected");

    QTest::newRow("1/sqrt(x) [0,1] - да")
        << "1/√x||0.000||1.000||да" << "да";

    QTest::newRow("ln(x) [0,1] - да")
        << "ln(x)||0.000||1.000||да" << "да";

    QTest::newRow("1/x [0,1] - нет")
        << "1/x||0.000||1.000||нет" << "нет";

    QTest::newRow("1/x^2 [0,1] - нет")
        << "1/x²||0.000||1.000||нет" << "нет";
}

void tasks_test::test_solver_task3()
{
    QFETCH(QString, params);
    QFETCH(QString, expected);

    QCOMPARE(solver_task3(params), expected);
}


// solver_trapezoid - метод трапеций


void tasks_test::test_solver_trapezoid_data()
{
    QTest::addColumn<QString>("params");
    QTest::addColumn<double>("expected");
    QTest::addColumn<double>("eps");

    // f(x)=2x+1, [0,1], n=1 - метод трапеций точен для линейных
    QTest::newRow("2x+1 [0,1] n=1 exact")
        << "2||0||100||1" << 2.0 << 1e-10;

    // f(x)=2x+1, [1,3], n=10 - точное 10.0
    QTest::newRow("2x+1 [1,3] n=10 exact")
        << "2||100||300||10" << 10.0 << 1e-10;

    // f(x)=x^2, [0,1], n=10000 - точное 1/3
    QTest::newRow("x^2 [0,1] n=10000")
        << "0||0||100||10000" << (1.0/3.0) << 0.0001;

    // f(x)=x^3, [0,1], n=10000 - точное 0.25
    QTest::newRow("x^3 [0,1] n=10000")
        << "1||0||100||10000" << 0.25 << 0.0001;

    // f(x)=sin(x), [0,π], n=10000 - точное 2.0
    QTest::newRow("sin(x) [0,pi] n=10000")
        << "3||0||314||10000" << 2.0 << 0.001;
}

void tasks_test::test_solver_trapezoid()
{
    QFETCH(QString, params);
    QFETCH(double, expected);
    QFETCH(double, eps);

    double result = solver_trapezoid(params);
    QVERIFY2(std::abs(result - expected) < eps,
             qPrintable(QString("solver_trapezoid(%1): got %2, expected %3, eps %4")
                            .arg(params)
                            .arg(result, 0, 'f', 8)
                            .arg(expected, 0, 'f', 8)
                            .arg(eps)));
}


// solver_simpson - метод Симпсона


void tasks_test::test_solver_simpson_data()
{
    QTest::addColumn<QString>("params");
    QTest::addColumn<double>("expected");
    QTest::addColumn<double>("eps");

    // f(x)=2x+1, [0,1], n=2 - метод Симпсона точен для линейных
    QTest::newRow("2x+1 [0,1] n=2 exact")
        << "2||0||100||2" << 2.0 << 1e-10;

    // f(x)=x^2, [0,1], n=2 - метод Симпсона точен для квадратичных (погрешность 0)
    QTest::newRow("x^2 [0,1] n=2 exact")
        << "0||0||100||2" << (1.0/3.0) << 1e-10;

    // f(x)=x^3, [0,1], n=2 - точен для кубических
    QTest::newRow("x^3 [0,1] n=2 exact")
        << "1||0||100||2" << 0.25 << 1e-10;

    // f(x)=x^2, [0,2], n=1000 - точное 8/3
    QTest::newRow("x^2 [0,2] n=1000")
        << "0||0||200||1000" << (8.0/3.0) << 1e-8;

    // f(x)=sin(x), [0,π], n=1000 - точное 2.0
    QTest::newRow("sin(x) [0,pi] n=1000")
        << "3||0||314||1000" << 2.0 << 0.0001;
}

void tasks_test::test_solver_simpson()
{
    QFETCH(QString, params);
    QFETCH(double, expected);
    QFETCH(double, eps);

    double result = solver_simpson(params);
    QVERIFY2(std::abs(result - expected) < eps,
             qPrintable(QString("solver_simpson(%1): got %2, expected %3, eps %4")
                            .arg(params)
                            .arg(result, 0, 'f', 8)
                            .arg(expected, 0, 'f', 8)
                            .arg(eps)));
}


// funcName_task1 - имена функций по ID


void tasks_test::test_funcName_task1_data()
{
    QTest::addColumn<int>("funcId");
    QTest::addColumn<QString>("expected");

    QTest::newRow("id=0 - x^2")          << 0  << "f(x) = x^2";
    QTest::newRow("id=3 - sin")          << 3  << "f(x) = sin(x)";
    QTest::newRow("id=99 - default x^2") << 99 << "f(x) = x^2";
}

void tasks_test::test_funcName_task1()
{
    QFETCH(int, funcId);
    QFETCH(QString, expected);

    QCOMPARE(funcName_task1(funcId), expected);
}


// get_random_task1 - корректность формата генерируемых параметров


void tasks_test::test_get_random_task1()
{
    for (int i = 0; i < 5; ++i) {
        QString raw = get_random_task1();
        QStringList parts = raw.split("||");

        // ровно 4 части
        QCOMPARE(parts.size(), 4);

        // funcId в диапазоне [0, 4]
        int funcId = parts[0].toInt();
        QVERIFY2(funcId >= 0 && funcId <= 4,
                 qPrintable(QString("funcId вне диапазона: %1").arg(funcId)));

        // a < b
        int a_cents = parts[1].toInt();
        int b_cents = parts[2].toInt();
        QVERIFY2(a_cents < b_cents,
                 qPrintable(QString("a >= b: %1 >= %2").arg(a_cents).arg(b_cents)));

        // n в диапазоне [2, 8]
        int n = parts[3].toInt();
        QVERIFY2(n >= 2 && n <= 8,
                 qPrintable(QString("n вне диапазона: %1").arg(n)));
    }
}


// get_random_task3 — корректность формата генерируемых параметров


void tasks_test::test_get_random_task3()
{
    for (int i = 0; i < 5; ++i) {
        QString raw = get_random_task3();
        QStringList parts = raw.split("||");

        // ровно 4 части
        QCOMPARE(parts.size(), 4);

        // ответ только "да" или "нет"
        QString answer = parts[3];
        QVERIFY2(answer == "да" || answer == "нет",
                 qPrintable(QString("неожиданный ответ: %1").arg(answer)));
    }
}


// get_random_task4 - корректность формата генерируемых параметров


void tasks_test::test_get_random_task4()
{
    for (int i = 0; i < 5; ++i) {
        QString raw = get_random_task4();
        QStringList parts = raw.split("||");

        // ровно 9 частей: funcId||a||b||n||correctPos||order0..order3
        QCOMPARE(parts.size(), 9);

        // funcId в диапазоне [0, 4]
        int funcId = parts[0].toInt();
        QVERIFY2(funcId >= 0 && funcId <= 4,
                 qPrintable(QString("funcId вне диапазона: %1").arg(funcId)));

        // a < b
        int a_cents = parts[1].toInt();
        int b_cents = parts[2].toInt();
        QVERIFY2(a_cents < b_cents,
                 qPrintable(QString("a >= b: %1 >= %2").arg(a_cents).arg(b_cents)));

        // n чётное и >= 4
        int n = parts[3].toInt();
        QVERIFY2(n >= 4 && n % 2 == 0,
                 qPrintable(QString("n некорректно: %1").arg(n)));

        // correctPos в диапазоне [0, 3]
        int correctPos = parts[4].toInt();
        QVERIFY2(correctPos >= 0 && correctPos <= 3,
                 qPrintable(QString("correctPos вне диапазона: %1").arg(correctPos)));

        // order — перестановка [0,1,2,3]
        int order[4] = { parts[5].toInt(), parts[6].toInt(),
                        parts[7].toInt(), parts[8].toInt() };
        int sum = order[0] + order[1] + order[2] + order[3];
        QVERIFY2(sum == 6,
                 qPrintable(QString("order не является перестановкой [0,1,2,3]: сумма %1").arg(sum)));
    }
}


QTEST_MAIN(tasks_test)

#include "tst_task_tests.moc"
