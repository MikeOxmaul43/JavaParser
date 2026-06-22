/*!
* \file
* \brief Данный файл содержит заголовки класса и методов парсера для программы Java Source Code Parser
*/
#pragma once
#include <QString>
#include <QStringList>
#include <QList>
#include "../model/class_info.h"


/*!
 * @brief Выполняет разбор Java-подобного исходного кода.
 *
 * Извлекает информацию о пакетах, классах, интерфейсах,
 * перечислениях, полях, методах, конструкторах и вложенных классах.
 */
class Parser
{
public:

    /*!
     * Разбирает текст исходного файла.
     *
     * Выполняет полный синтаксический анализ текста и формирует
     * список найденных классов.
     *
     * \param[in] text — текст исходного файла
     * \return         — список описаний найденных классов
     */
    QList<ClassInfo> parse(const QString &text);

private:

    /// Исходный текст файла.
    QString m_text;

    /// Текущая позиция чтения.
    int m_pos = 0;

    /// Имя текущего пакета.
    QString m_package;

    /*!
     * @brief Считывает следующий токен из текста.
     *
     * Пропускает пробелы и комментарии.
     *
     * \return — следующий токен либо пустая строка при конце текста
     */
    QString readWord();

    /*!
     * @brief Возвращает следующий токен без изменения позиции.
     *
     * \return — следующий токен
     */
    QString peekWord();

    /*!
     * @brief Возвращает следующий непробельный символ без сдвига позиции.
     *
     * \return — следующий символ либо пустой символ
     */
    QChar peekChar() const;

    /*!
     * @brief Пропускает все пробельные символы.
     */
    void skipSpaces();

    /*!
     * @brief Пропускает однострочный комментарий.
     */
    void skipLineComment();

    /*!
     * @brief Пропускает блочный комментарий.
     */
    void skipBlockComment();

    /*!
     * @brief Пропускает тело блока в фигурных скобках.
     *
     * Корректно учитывает вложенность блоков,
     * строки, символы и комментарии.
     */
    void skipBody();

    /*!
     * @brief Пропускает текст до символа ';' включительно.
     */
    void skipToSemicolon();

    /*!
     * @brief Разбирает объявление пакета.
     */
    void parsePackage();

    /*!
     * @brief Пропускает все инструкции import.
     */
    void skipImports();

    /*!
     * @brief Разбирает объявление класса, интерфейса или enum.
     *
     * \param[in] modifiers — список модификаторов
     * \param[in] outerClass — имя внешнего класса
     * \return               — описание класса
     */
    ClassInfo parseClass(const QStringList &modifiers,
                         const QString &outerClass);

    /*!
     * @brief Разбирает член класса.
     *
     * Определяет, является ли элемент:
     * - полем;
     * - методом;
     * - конструктором.
     *
     * \param[in,out] ci     — описание класса
     * \param[in] modifiers  — модификаторы члена класса
     */
    void parseMember(ClassInfo &ci,
                     const QStringList &modifiers);

    /*!
     * Разбирает поле класса.
     *
     * \param[in] modifiers — модификаторы поля
     * \param[in] type      — тип поля
     * \param[in] name      — имя поля
     * \return              — описание поля
     */
    FieldInfo parseField(const QStringList &modifiers,
                         const QString &type,
                         const QString &name);

    /*!
     * Разбирает метод класса.
     *
     * \param[in] modifiers  — модификаторы метода
     * \param[in] returnType — тип возвращаемого значения
     * \param[in] name       — имя метода
     * \return               — описание метода
     */
    MethodInfo parseMethod(const QStringList &modifiers,
                           const QString &returnType,
                           const QString &name);

    /*!
     * Разбирает конструктор класса.
     *
     * \param[in] modifiers — модификаторы конструктора
     * \param[in] name      — имя конструктора
     * \return              — описание конструктора
     */
    MethodInfo parseConstructor(const QStringList &modifiers,
                                const QString &name);

    /*!
     * Разбирает список параметров метода или конструктора.
     *
     * Поддерживает массивы, varargs, аннотации
     * и final-параметры.
     *
     * \return — список параметров
     */
    QList<ParamInfo> parseParams();

    /*!
     * Разбирает перечисление enum.
     *
     * Извлекает константы и дополнительные члены enum.
     *
     * \param[in,out] ci — описание enum
     */
    void parseEnum(ClassInfo &ci);

    /*!
     * Проверяет достижение конца текста.
     *
     * \return — true, если достигнут конец текста
     */
    bool atEnd() const;

    /*!
     * Проверяет, является ли слово ключевым словом типа.
     *
     * Поддерживаются:
     * - class
     * - interface
     * - enum
     *
     * \param[in] w — проверяемое слово
     * \return      — true, если слово является объявлением типа
     */
    static bool isClassKeyword(const QString &w);

    /*!
     * Проверяет, является ли слово модификатором.
     *
     * \param[in] w — проверяемое слово
     * \return      — true, если слово является модификатором
     */
    static bool isModifier(const QString &w);

    /*!
     * Пропускает аннотацию.
     *
     * Читает @ и имя аннотации, при наличии скобок
     * пропускает их содержимое с учётом вложенности.
     */
    void skipAnnotation();

    /*!
     * Обрабатывает очередной символ тела класса или метода.
     *
     * Обновляет глубину вложенности фигурных скобок и
     * пропускает вложенные литералы и комментарии.
     *
     * \param[in] c      — обрабатываемый символ
     * \param[in,out] depth — текущая глубина вложенности
     */
    void processBodyChar(QChar c, int &depth);

    /*!
     * Пропускает комментарий, если текущая позиция находится
     * после символа '/'.
     *
     * Поддерживаются:
     * - однострочные комментарии (// ...)
     * - многострочные комментарии (/* ... *\/)
     */
    void skipComment();

    /*!
     * Пропускает строковый или символьный литерал.
     *
     * Предполагается, что открывающая кавычка уже прочитана.
     * Корректно обрабатывает экранированные символы.
     *
     * \param[in] terminator — символ закрывающей кавычки
     */
    void skipQuotedLiteral(QChar terminator);
};
