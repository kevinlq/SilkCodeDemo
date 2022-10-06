#include <QCoreApplication>

#include <SilkAudioCode.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int nResult = -1;

    SilkAudioCode code;
    //nResult = code.encode("test01.wav", "test01.silk");
    nResult = code.decode("test01.silk", "test01.pcm");

    qDebug() << "#nResult " << nResult;

    return 0;
}
