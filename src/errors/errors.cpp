/*!
* \file
* \brief Данный файл содержит возможные ошибки для программы Java Source Code Parser
*/
#include "errors.h"

QString errorMessage(AppError e) {
    //Перевод Enum значения в текстовый формат
    QString result = {};
    switch (e) {
    case AppError::LineTooLong:
        result = "Превышена максимально допустимая длина строки (1024 символа).";
        break;
    case AppError::TooManyLines:
        result = "Превышено максимально допустимое количество строк входного файла (10000).";
        break;
    case AppError::BadArgCount:
        result = "Недостаточно параметров командной строки. Укажите путь к входному файлу и выходной директории.";
        break;
    case AppError::FileNotFound:
        result = "Неверно указан входной файл. Проверьте, что файл существует.";
        break;
    case AppError::FileNoAccess:
        result = "Нет доступа к входному файлу.";
        break;
    case AppError::FileEmpty:
        result = "Входной файл пуст. Отсутствуют данные для анализа.";
        break;
    case AppError::TooManyArgs:
        result = "Неверное количество параметров командной строки. Ожидается два параметра.";
        break;
    }
    //Если ошибка не найденна - пустая строка
    return result;
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
