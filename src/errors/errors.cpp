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
