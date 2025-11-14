#include "mainwindow.h"
#include "ui_mainwindow.h"

// ------- GLOBAL POINTER -------
static MainWindow* g_mainWindowInstance = nullptr;

extern "C"
    JNIEXPORT void JNICALL
    Java_org_verya_kotlinTest_TestBridge_onMessageFromKotlin(JNIEnv* env, jclass /*clazz*/, jstring msg)
{
    if (!g_mainWindowInstance)
        return;

    QString qmsg = QJniObject(msg).toString();

    // 🧵 انتقال امن به Thread اصلی UI با Qt
    QMetaObject::invokeMethod(g_mainWindowInstance, [=]() {
        //g_mainWindowInstance->ui->TxtResult->append(QStringLiteral("From Kotlin: %1").arg(qmsg));
        g_mainWindowInstance->appendFromKotlin(QStringLiteral("callback From Kotlin: %1").arg(qmsg));
    }, Qt::QueuedConnection);
}

extern "C"
    JNIEXPORT void JNICALL
    Java_org_verya_kotlinTest_TestBridge_nativeOnNotificationAction(JNIEnv *, jobject)
{
    qDebug() << "[JNI Callback] Notification action clicked!";

    // اطمینان از اجرای کد UI روی Thread اصلی Qt
    QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
        qDebug() << "[Qt C++] Action received in UI thread.";
        emit g_mainWindowInstance->notificationActionPressed();
        // هر عملیات UI دلخواه اینجا
    }, Qt::QueuedConnection);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->BtnExit,&QPushButton::clicked,this,&MainWindow::onBtnExitClicked);
    connect(ui->BtnRandomNumber,&QPushButton::clicked,this,&MainWindow::onBtnRandomNumberClicked);
    connect(ui->BtnSomeFunc,&QPushButton::clicked,this,&MainWindow::onBtnSomeFuncClicked);
    connect(ui->BtnStringTest,&QPushButton::clicked,this,&MainWindow::onBtnStringTestClicked);
    connect(ui->BtnNotification,&QPushButton::clicked,this,&MainWindow::onBtnNotificationClicked);
    connect(this,&MainWindow::notificationActionPressed,this,&MainWindow::onNotificationActionPressed);

    g_mainWindowInstance = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendFromKotlin(const QString &text)
{
    ui->TxtResult->append(text);
}

void MainWindow::onBtnExitClicked()
{
    exit(0);
}

void MainWindow::onBtnRandomNumberClicked()
{
    int value = QJniObject::callStaticMethod<jint>(
        "org/verya/kotlinTest/TestBridge",
        "getRandomValue",
        "()I"
        );
    ui->TxtResult->append(QString("Random value : %1").arg(value));
}

void MainWindow::onBtnSomeFuncClicked()
{
    // Class Path (match exactly the Kotlin package)
    const char* cls = "org/verya/kotlinTest/TestBridge";

    // (1) Two int params → int return
    jint resultSum = QJniObject::callStaticMethod<jint>(
        cls, "sum", "(II)I", 5, 7);
    //qDebug() << "sum result =" << resultSum; // -> 12
    ui->TxtResult->append(QString("sum result = %1").arg(resultSum));

    // (2) Two strings → string return
    QJniObject strA = QJniObject::fromString("Hello ");
    QJniObject strB = QJniObject::fromString("Ali");
    QJniObject resultConcat = QJniObject::callStaticObjectMethod(
        cls,
        "concat",
        "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
        strA.object<jstring>(),
        strB.object<jstring>());
    //qDebug() << "concat =" << resultConcat.toString();
    ui->TxtResult->append(QString("concat = %1").arg(resultConcat.toString()));

    // (3) Boolean param, no return
    QJniObject::callStaticMethod<void>(cls, "setFlag", "(Z)V", true);

    // (4) IntArray param → double return
    QJniEnvironment env;
    jintArray arr = env->NewIntArray(4);
    jint tmp[4] = {2, 4, 6, 8};
    env->SetIntArrayRegion(arr, 0, 4, tmp);
    jdouble avg = QJniObject::callStaticMethod<jdouble>(
        cls, "average", "([I)D", arr);
    //qDebug() << "average =" << avg;
    ui->TxtResult->append(QString("average = %1").arg(avg));
    env->DeleteLocalRef(arr);

    // (5) No param → string return
    QJniObject message = QJniObject::callStaticObjectMethod(
        cls,
        "getMessage",
        "()Ljava/lang/String;");
    //qDebug() << "message =" << message.toString();
    ui->TxtResult->append(QString("message = %1").arg(message.toString()));
}

void MainWindow::onBtnStringTestClicked()
{
    // Class Path (match exactly the Kotlin package)
    const char* cls = "org/verya/kotlinTest/TestBridge";

    QJniObject nameStr = QJniObject::fromString("Ali");
    QJniObject res = QJniObject::callStaticObjectMethod(
        cls,
        "getGreeting",
        "(Ljava/lang/String;)Ljava/lang/String;",
        nameStr.object<jstring>());
    ui->TxtResult->append(res.toString());
}

void MainWindow::onBtnNotificationClicked()
{
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
        return;

    QJniObject jTitle = QJniObject::fromString("Qt JNI Notification");
    QJniObject jMsg   = QJniObject::fromString("Ali just triggered Kotlin helper!");

    QJniObject::callStaticMethod<void>(
        "org/verya/kotlinTest/TestBridge",
        "postNotification",
        "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V",
        context.object(),
        jTitle.object<jstring>(),
        jMsg.object<jstring>()
        );
}

void MainWindow::onNotificationActionPressed()
{
    ui->TxtResult->append(QString("notification Action teriggered!"));
}
