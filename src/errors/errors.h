/*!
* \file
* \brief Данный файл содержит возможные ошибки для программы Java Source Code Parser
*/

#pragma once
#include <QtGlobal>
#include <QString>
#include <QSet>



/**
 * @brief Типы ошибок приложения.
 *
 * Используются для унифицированного представления ошибок
 * при обработке аргументов командной строки и входных файлов.
 */
enum class AppError
{
    LineTooLong,   ///< Длина строки превышает допустимый предел.
    TooManyLines,  ///< Количество строк превышает допустимый предел.
    BadArgCount,   ///< Передано неверное количество аргументов.
    FileNotFound,  ///< Указанный файл не найден.
    FileNoAccess,  ///< Нет прав доступа к файлу.
    FileEmpty,     ///< Файл пуст.
    TooManyArgs    ///< Передано слишком много аргументов.
};



/**
 * @brief Возвращает текстовое описание ошибки.
 *
 * @param[in] e Код ошибки.
 * @return Локализованное сообщение об ошибке.
 */
QString errorMessage(AppError e);
