/*!
* \file
* \brief Данный файл содержит заголовки методов file_reader для программы Java Source Code Parser
*/
#pragma once
#include <QFile>
#include <QTextStream>

#include "../errors/errors.h"


/**
 * @brief Считывает содержимое входного файла.
 *
 * Выполняет чтение файла по указанному пути и возвращает
 * его содержимое в виде строки. При возникновении ошибок
 * соответствующие коды добавляются в параметр @p errors.
 *
 * @param path Путь к входному файлу.
 * @param[out] errors Множество ошибок, обнаруженных в процессе
 * чтения файла.
 *
 * @return Содержимое файла. В случае ошибки может быть возвращена
 * пустая строка.
 */
QString readInputFile(const QString &path, QSet<AppError> &errors);
