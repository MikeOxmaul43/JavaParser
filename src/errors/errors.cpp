/*!
* \file
* \brief Данный файл содержит возможные ошибки для программы Java Source Code Parser
*/
#include "errors.h"

QString errorMessage(AppError e) {
    switch (e) {
    case AppError::LineTooLong:
        return "Превышена максимально допустимая длина строки (1024 символа).";
    case AppError::TooManyLines:
        return "Превышено максимально допустимое количество строк входного файла (10000).";
    case AppError::BadArgCount:
        return "Недостаточно параметров командной строки. Укажите путь к входному файлу и выходной директории.";
    case AppError::FileNotFound:
        return "Неверно указан входной файл. Проверьте, что файл существует.";
    case AppError::FileNoAccess:
        return "Нет доступа к входному файлу.";
    case AppError::FileEmpty:
        return "Входной файл пуст. Отсутствуют данные для анализа.";
    case AppError::TooManyArgs:
        return "Неверное количество параметров командной строки. Ожидается два параметра.";
    }
    return {};
}


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
