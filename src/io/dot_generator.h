/*!
* \file
* \brief Данный файл содержит заголовки класса и методов DotGen для программы Java Source Code Parser
*/

#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include "../model/class_info.h"

/*!
 * @brief Генерирует DOT-диаграмму классов по результатам разбора Java-кода.
 *
 * Формирует файл structure.dot в указанном выходном каталоге.
 * Каждый класс представляется узлом-таблицей с полями, методами
 * и конструкторами. Связи между классами отражают наследование,
 * реализацию интерфейсов и вложенность.
 */
class DotGenerator
{
public:

    /*!
     * Создаёт объект DotGenerator с указанием выходного каталога.
     *
     * \param[in] outputDir — путь к каталогу для сохранения structure.dot
     */
    explicit DotGenerator(const QString &outputDir);

    /*!
     * Генерирует файл structure.dot по списку классов.
     *
     * Является точкой входа. Обходит все классы верхнего уровня,
     * строит узлы и рёбра, записывает результат в файл.
     *
     * \param[in] classes — список описаний классов верхнего уровня
     */
    void generate(const QList<ClassInfo> &classes);

private:

    /// Путь к выходному каталогу.
    QString m_outputDir;

    /// Буфер строк формируемого DOT-файла.
    QStringList m_lines;

    /// Счётчик узлов графа.
    int m_nodeCount = 0;

    /// Счётчик рёбер графа.
    int m_edgeCount = 0;

    /*!
     * Формирует узел-таблицу для класса и добавляет его в буфер.
     *
     * Таблица содержит имя класса, секцию полей, секцию методов
     * и секцию конструкторов (при наличии). Увеличивает счётчик узлов.
     *
     * \param[in] ci — описание класса
     */
    void buildNode(const ClassInfo &ci);

    /*!
     * Формирует все рёбра графа и добавляет их в буфер.
     *
     * Рекурсивно обходит вложенные классы.
     * Поддерживает три типа рёбер:
     * - extends   (solid,  normal,  чёрный)
     * - implements (dashed, empty,   синий)
     * - nested    (dotted, diamond, серый)
     *
     * \param[in] classes — список классов для обхода
     */
    void buildAllEdges(const QList<ClassInfo> &classes);

    /*!
     * Экранирует HTML-спецсимволы для использования внутри DOT-таблицы.
     *
     * Заменяет: & → &amp;  < → &lt;  > → &gt;  " → &quot;
     *
     * \param[in] s — исходная строка
     * \return      — экранированная строка
     */
    static QString escapeHtml(const QString &s);

    /*!
     * Формирует уникальный идентификатор узла для DOT-формата.
     *
     * Для вложенных классов: OuterClass__InnerClass.
     * Точки и пробелы заменяются на подчёркивания.
     *
     * \param[in] ci — описание класса
     * \return       — уникальный идентификатор узла
     */
    static QString nodeId(const ClassInfo &ci);
};
