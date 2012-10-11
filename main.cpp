#include <QtCore>
#include <QtDebug>

#include "physfs.h"
#include "FileEngine.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PHYSFS_init(argv[0]);

    PHYSFS_mount("src.zip", "/", true);

    FileEngineHandler engine;

    char **files = PHYSFS_enumerateFiles("/");

    for (char **i = files; *i != NULL; i++) {
        QString file = QString::fromAscii(*i);
        qDebug() << file;
    }

    PHYSFS_freeList(files);


    qDebug() << "QFile::exists" << QFile::exists("physfs:/physfs.c");

    QFile::remove("physfs:/physfs.h");

    QFile file("physfs:/physfs.jj");
    file.open(QIODevice::WriteOnly);
    file.write("hello world");
    file.close();


    return 0;// a.exec();
}
