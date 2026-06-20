/*!
* \file
* \brief Данный файл содержит реализацию методов file_reader для программы Java Source Code Parser
*/
#include "file_reader.h"

QString readInputFile(const QString &path, QSet<AppError> &errors)
{
    QFile file(path);

    if (!file.exists()) {
        errors.insert(AppError::FileNotFound);
        return {};
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errors.insert(AppError::FileNoAccess);
        return {};
    }

    QString text = QTextStream(&file).readAll();
    file.close();

    if (text.trimmed().isEmpty()) {
        errors.insert(AppError::FileEmpty);
        return {};
    }

    QStringList lines = text.split('\n');
    if (lines.size() > 10000)
        errors.insert(AppError::TooManyLines);

    for (const QString &line : lines)
        if (line.size() > 1024)
            errors.insert(AppError::LineTooLong);

    return errors.isEmpty() ? text : QString{};
}
