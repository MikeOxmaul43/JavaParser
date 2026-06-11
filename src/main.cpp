#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "errors/errors.h"
#include "io/file_reader.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QSet<AppError> errors;

    QStringList args = QCoreApplication::arguments();
    if (args.size() < 3) {
        errors.insert(AppError::BadArgCount);
    } else if (args.size() > 3) {
        errors.insert(AppError::TooManyArgs);
    }

    QString text;
    if(errors.isEmpty())
        text = readInputFile(args[1], errors);

    if (!errors.isEmpty()) {
        for (AppError e: errors)
            qCritical().noquote() << errorMessage(e);
            return 1;
    }
    
    return 0;
}