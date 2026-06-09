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
 * @brief Вычисляет хеш-код для значения AppError.
 *
 * Позволяет использовать AppError в контейнерах Qt,
 * основанных на хеш-таблицах, таких как QHash и QSet.
 *
 * @param e Значение перечисления.
 * @param seed Начальное значение хеша.
 * @return Вычисленный хеш.
 */
Q_DECL_CONST_FUNCTION Q_DECL_CONSTEXPR uint qHash(
    AppError e,
    uint seed = 0
) noexcept
{
    return static_cast<uint>(e) ^ seed;
}

/**
 * @brief Возвращает текстовое описание ошибки.
 *
 * @param e Код ошибки.
 * @return Локализованное сообщение об ошибке.
 */
QString errorMessage(AppError e);
