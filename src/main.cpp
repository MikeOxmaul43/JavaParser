#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "errors/errors.h"
#include "io/file_reader.h"
#include "io/file_writer.h"
#include "io/dot_generator.h"
#include "parser/parser.h"

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

    Parser parser;
    QList<ClassInfo> classes = parser.parse(text);

    FileWriter writer(args[2]);
    writer.writeAll(classes);

    DotGenerator dotGen(args[2]);
    dotGen.generate(classes);
    return 0;
}