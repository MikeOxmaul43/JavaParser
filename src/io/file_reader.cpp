/*!
* \file
* \brief Данный файл содержит реализацию методов file_reader для программы Java Source Code Parser
*/
#include "file_reader.h"

QString readInputFile(const QString &path, QSet<AppError> &errors)
{
    QFile file(path);
    // Проверка существования файла
        if (!file.exists()) {
            errors.insert(AppError::FileNotFound);
            return {};
        }

        // Попытка открыть файл в текстовом режиме только для чтения
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            errors.insert(AppError::FileNoAccess);
            return {};
        }

        // Чтение всего содержимого через QTextStream
        QString text = QTextStream(&file).readAll();
        file.close();
        // Проверка, что файл не состоит только из пробельных символов
        if (text.trimmed().isEmpty()) {
            errors.insert(AppError::FileEmpty);
            return {};
        }

        // Разбиваем текст на строки (по символу '\n')
        QStringList lines = text.split('\n');

        // Проверка количества строк
        if (lines.size() > 10000) {
            errors.insert(AppError::TooManyLines);
        }

        // Проверка длины каждой строки
        for (const QString &line : lines) {
            if (line.size() > 1024) {
                errors.insert(AppError::LineTooLong);
            }
        }
        // Возвращаем текст
        return text;
}
