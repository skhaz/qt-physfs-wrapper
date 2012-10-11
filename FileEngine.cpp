#include "FileEngine.h"



const QString FileEngineHandler::scheme = "physfs:/";

FileEngine::FileEngine(const QString& filename)
: fp(NULL)
, flags(0)
{
    setFileName(filename);
}

FileEngine::~FileEngine()
{
    close();
}

bool FileEngine::open(QIODevice::OpenMode openMode)
{
    close();

    if (openMode & QIODevice::WriteOnly) {
        fp = PHYSFS_openWrite(filename.toUtf8().constData());
        flags = QAbstractFileEngine::WriteOwnerPerm | QAbstractFileEngine::WriteUserPerm | QAbstractFileEngine::FileType;
    }

    else if (openMode & QIODevice::ReadOnly) {
        fp = PHYSFS_openRead(filename.toUtf8().constData());
    }

    else if (openMode & QIODevice::Append) {
        fp = PHYSFS_openAppend(filename.toUtf8().constData());
    }

    else {
        qWarning("Bad file open mode: %d", (int)openMode);
    }

    if (!fp) {
        qWarning("Failed to open %s, reason: %s", filename.toUtf8().constData(), PHYSFS_getLastError());
        return false;
    }

    return true;
}

bool FileEngine::close()
{
    if (isOpened()) {
        int result = PHYSFS_close(fp);
        fp = NULL;
        return result != 0;
    }

    return true;
}

bool FileEngine::flush()
{
    return PHYSFS_flush(fp) != 0;
}

qint64 FileEngine::size() const
{
    return fileSize;
}

qint64 FileEngine::pos() const
{
    return PHYSFS_tell(fp);
}

bool FileEngine::seek(qint64 pos)
{
    return PHYSFS_seek(fp, pos) != 0;
}

bool FileEngine::isSequential() const
{
    return true;
}

bool FileEngine::remove()
{
    return PHYSFS_delete(filename.toUtf8().constData()) != 0;
}

bool FileEngine::mkdir(const QString &dirName, bool createParentDirectories) const
{
    Q_UNUSED(createParentDirectories);
    return PHYSFS_mkdir(dirName.toUtf8().constData()) != 0;
}

bool FileEngine::rmdir(const QString &dirName, bool recurseParentDirectories) const
{
    Q_UNUSED(recurseParentDirectories);
    return PHYSFS_delete(dirName.toUtf8().constData()) != 0;
}

bool FileEngine::caseSensitive() const
{
    return true;
}

bool FileEngine::isRelativePath() const
{
    return true;
}

QStringList FileEngine::entryList(QDir::Filters filters, const QStringList &filterNames) const
{
    QString file;
    QStringList result;
    char **files = PHYSFS_enumerateFiles("");

    for (char **i = files; *i != NULL; i++) {
        file = QString::fromAscii(*i);
        if (QDir::match(filterNames, file)) {
            result << file;
        }
    }

    PHYSFS_freeList(files);
    return result;
}

QAbstractFileEngine::FileFlags FileEngine::fileFlags(FileFlags type) const
{
    return type & flags;
}

QString FileEngine::fileName(FileName file) const
{
    if (file == QAbstractFileEngine::AbsolutePathName)
        return PHYSFS_getWriteDir();

    return filename;
}

QDateTime FileEngine::fileTime(FileTime time) const
{
    switch (time)
    {
        case QAbstractFileEngine::ModificationTime:
        default:
            return datetime;
            break;
    };
}

void FileEngine::setFileName(const QString &file)
{
    filename = file;
    PHYSFS_Stat stat;
    if (PHYSFS_stat(filename.toUtf8().constData(), &stat) != 0) {
        fileSize = stat.filesize;
        datetime = QDateTime::fromTime_t(stat.modtime);
        flags |= QAbstractFileEngine::ExistsFlag;

        switch (stat.filetype)
        {
            case PHYSFS_FILETYPE_REGULAR:
                flags |= QAbstractFileEngine::FileType;
                break;

            case PHYSFS_FILETYPE_DIRECTORY:
                flags |= QAbstractFileEngine::DirectoryType;
                break;
            case PHYSFS_FILETYPE_SYMLINK:
                flags |= QAbstractFileEngine::LinkType;
                break;
        };
    }
}

bool FileEngine::atEnd() const
{
    return PHYSFS_eof(fp) != 0;
}

qint64 FileEngine::read(char *data, qint64 maxlen)
{
    return PHYSFS_read(fp, data, 1, maxlen);
}

qint64 FileEngine::readLine(char *data, qint64 maxlen)
{
    // TODO
}

qint64 FileEngine::write(const char *data, qint64 len)
{
    return PHYSFS_write(fp, data, 1, len);
}

bool FileEngine::isOpened() const
{
    return fp != NULL;
}

QFile::FileError FileEngine::error() const
{
    return QFile::UnspecifiedError;
}

QString FileEngine::errorString() const
{
    return PHYSFS_getLastError();
}

bool FileEngine::supportsExtension(Extension extension) const
{
    return extension == QAbstractFileEngine::AtEndExtension;
}

QAbstractFileEngine* FileEngineHandler::create(const QString &filename) const
{
    if (filename.startsWith(scheme)) {
        return new FileEngine(filename.right(scheme.size()));
    }

    return NULL;
}
