#pragma once

#include <QString>
#include <QList>
#include "../model/class_info.h"

/*!
 * @brief Записывает результаты разбора Java-кода в файловую систему.
 *
 * Для каждого класса создаёт отдельную папку с подпапками
 * fields, methods и constructors. Каждый элемент сохраняется
 * в отдельный текстовый файл. Поддерживает вложенные классы
 * через рекурсивный обход.
 */
class FileWriter
{
public:

    /*!
     * Создаёт объект FileWriter с указанием выходного каталога.
     *
     * \param[in] outputDir — путь к каталогу для записи результатов
     */
    explicit FileWriter(const QString &outputDir);

    /*!
     * Записывает все классы верхнего уровня в файловую систему.
     *
     * Является точкой входа. Для каждого класса из списка
     * вызывает writeClass().
     *
     * \param[in] classes — список описаний классов верхнего уровня
     */
    void writeAll(const QList<ClassInfo> &classes);

private:

    /// Путь к выходному каталогу.
    QString m_outputDir;

    /*!
     * Создаёт папку класса и записывает все его элементы.
     *
     * Рекурсивно обрабатывает вложенные классы.
     * Создаёт подпапки fields, methods, constructors при наличии
     * соответствующих элементов.
     *
     * \param[in] ci         — описание класса
     * \param[in] parentPath — путь к родительской папке
     */
    void writeClass(const ClassInfo &ci, const QString &parentPath);

    /*!
     * Записывает метаданные класса в файл class.txt.
     *
     * Сохраняет имя, тип, пакет, модификаторы,
     * родительский класс и список реализуемых интерфейсов.
     *
     * \param[in] ci       — описание класса
     * \param[in] classDir — путь к папке класса
     */
    void writeClassMeta(const ClassInfo &ci, const QString &classDir);

    /*!
     * Записывает описание поля в текстовый файл.
     *
     * Имя файла совпадает с именем поля.
     * Сохраняет имя, тип, модификаторы и флаги static, final.
     *
     * \param[in] fi        — описание поля
     * \param[in] fieldsDir — путь к папке fields
     */
    void writeField(const FieldInfo &fi, const QString &fieldsDir);

    /*!
     * Записывает описание метода в текстовый файл.
     *
     * При перегрузке добавляет числовой суффикс к имени файла.
     * Сохраняет имя, тип возврата, модификаторы и список параметров.
     *
     * \param[in] mi         — описание метода
     * \param[in] methodsDir — путь к папке methods
     */
    void writeMethod(const MethodInfo &mi, const QString &methodsDir);

    /*!
     * Записывает описание конструктора в текстовый файл.
     *
     * При перегрузке добавляет числовой суффикс к имени файла.
     * Сохраняет имя, модификаторы и список параметров.
     *
     * \param[in] mi      — описание конструктора
     * \param[in] ctorsDir — путь к папке constructors
     */
    void writeConstructor(const MethodInfo &mi, const QString &ctorsDir);
};