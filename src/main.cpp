/*!
* \file
* \brief Данный файл содержит главную функцию программы KNPO_Java_Parser.
*
* \mainpage Документация для программы "Java Source Code Parser (KNPO_Java_Parser)"
Программа предназначена для структурного анализа исходного кода на языке Java
и разбиения его на отдельные элементы программы с сохранением каждого элемента
в отдельный текстовый файл, а также формирования табличной диаграммы в формате
DOT (Graphviz), отражающей структуру программы.
Для функционирования программы необходим фреймворк Qt 6 (модуль Core).
Программа разработана на языке С++ с использованием фреймворка Qt в среде
разработки Zed.
Программа должна получать два аргумента командной строки: путь к входному
файлу с исходным кодом на языке Java и путь к выходному каталогу для
сохранения результатов работы программы.
Пример команды запуска программы:
* \code
./KNPO_Java_Parser ./Example.java ./output/
* \endcode
* \author Blinov Lev
* \date June 2026
* \version 1.0
*/
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "errors/errors.h"
#include "io/file_reader.h"
#include "io/file_writer.h"
#include "io/dot_generator.h"
#include "parser/parser.h"


/*!
 * \brief Главная функция программы
 * \param [in] argc - количество переданных аргументов командной строки
 * \param [in] argv - аргументы командной строки
 * \param [in] argv[0] - аргумент запуска программы
 * \param [in] argv[1] - путь к входному файлу с исходным кодом на языке Java
 * \param [in] argv[2] - путь к выходному каталогу
 * \return 0 - программа завершилась без исключений
 * \return 1 - во входных данных обнаружены ошибки
 */
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
