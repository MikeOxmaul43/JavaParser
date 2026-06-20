/*!
* \file
* \brief Данный файл содержит реализацию класса и методов DotGen для программы Java Source Code Parser
*/
#include "dot_generator.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

DotGenerator::DotGenerator(const QString &outputDir)
    : m_outputDir(outputDir)
{
}


void DotGenerator::generate(const QList<ClassInfo> &classes)
{
    m_lines.clear();
    m_nodeCount = 0;
    m_edgeCount = 0;

    m_lines << "digraph {";

    // Узлы — верхний уровень и их вложенные классы
    for (const ClassInfo &ci : classes) {
        buildNode(ci);
        for (const ClassInfo &nested : ci.nestedClasses)
            buildNode(nested);
    }

    // Рёбра
    buildAllEdges(classes);

    m_lines << "}";
    m_lines << QString("// Total nodes: %1, total edges: %2")
                   .arg(m_nodeCount)
                   .arg(m_edgeCount);

    // Запись файла
    QDir().mkpath(m_outputDir);
    QFile file(m_outputDir + QDir::separator() + "structure.dot");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const QString &line : m_lines)
        out << line << "\n";

    file.close();
}


void DotGenerator::buildNode(const ClassInfo &ci)
{
    QString id = nodeId(ci);

    // Заголовок: тип + имя класса
    QString typePrefix;
    switch (ci.type) {
        case ClassType::INTERFACE: typePrefix = "&laquo;interface&raquo;<BR/>"; break;
        case ClassType::ENUM:      typePrefix = "&laquo;enum&raquo;<BR/>";      break;
        default: break;
    }
    // abstract — добавляем курсив
    bool isAbstract = ci.modifiers.contains("abstract");

    QString header = typePrefix;
    if (isAbstract)
        header += "<i>" + escapeHtml(ci.name) + "</i>";
    else
        header += "<b>" + escapeHtml(ci.name) + "</b>";

    QStringList rows;
    rows << QString("  %1 [label=<<TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"0\">").arg(id);

    // ── Секция: имя класса ────────────────────
    rows << QString("    <TR><TD><b>%1</b></TD></TR>").arg(header);

    // ── Секция: поля ──────────────────────────
    if (!ci.fields.isEmpty()) {
        // разделитель секций
        rows << "    <TR><TD BGCOLOR=\"#DDDDDD\"><i>fields</i></TD></TR>";
        for (const FieldInfo &fi : ci.fields) {
            QString mods;
            if (fi.isStatic && fi.isFinal)      mods = "{static final} ";
            else if (fi.isStatic)               mods = "{static} ";
            else if (fi.isFinal)                mods = "{final} ";

            QString line = escapeHtml(mods + fi.name + " : " + fi.type);
            rows << QString("    <TR><TD ALIGN=\"LEFT\">%1</TD></TR>").arg(line);
        }
    }

    // ── Секция: методы ────────────────────────
    if (!ci.methods.isEmpty()) {
        rows << "    <TR><TD BGCOLOR=\"#DDDDDD\"><i>methods</i></TD></TR>";
        for (const MethodInfo &mi : ci.methods) {
            // параметры
            QStringList paramStrs;
            for (const ParamInfo &p : mi.params)
                paramStrs << escapeHtml(p.name + " : " + p.type);

            QString sig = escapeHtml(mi.name) +
                          "(" + paramStrs.join(", ") + ")" +
                          " : " + escapeHtml(mi.returnType);

            if (mi.isAbstract) sig = "<i>" + sig + "</i>";
            if (mi.isStatic)   sig = "{static} " + sig;

            rows << QString("    <TR><TD ALIGN=\"LEFT\">%1</TD></TR>").arg(sig);
        }
    }

    // ── Секция: конструкторы ──────────────────
    if (!ci.constructors.isEmpty()) {
        rows << "    <TR><TD BGCOLOR=\"#DDDDDD\"><i>constructors</i></TD></TR>";
        for (const MethodInfo &mi : ci.constructors) {
            QStringList paramStrs;
            for (const ParamInfo &p : mi.params)
                paramStrs << escapeHtml(p.name + " : " + p.type);

            QString sig = escapeHtml(mi.name) +
                          "(" + paramStrs.join(", ") + ")";

            rows << QString("    <TR><TD ALIGN=\"LEFT\">%1</TD></TR>").arg(sig);
        }
    }

    rows << "  </TABLE>>];";

    m_lines << rows;
    ++m_nodeCount;
}


void DotGenerator::buildAllEdges(const QList<ClassInfo> &classes)
{
    for (const ClassInfo &ci : classes) {
        QString from = nodeId(ci);

        // extends
        if (!ci.extendsClass.isEmpty()) {
            m_lines << QString("  %1 -> %2 [ style=\"solid\", arrowhead=\"normal\", "
                               "label=\"extends\", color=\"black\" ];")
                           .arg(from, ci.extendsClass);
            ++m_edgeCount;
        }

        // implements
        for (const QString &iface : ci.implementsList) {
            m_lines << QString("  %1 -> %2 [ style=\"dashed\", arrowhead=\"empty\", "
                               "label=\"implements\", color=\"blue\" ];")
                           .arg(from, iface);
            ++m_edgeCount;
        }

        // nested
        for (const ClassInfo &nested : ci.nestedClasses) {
            m_lines << QString("  %1 -> %2 [ style=\"dotted\", arrowhead=\"diamond\", "
                               "label=\"nested\", color=\"gray\" ];")
                           .arg(from, nodeId(nested));
            ++m_edgeCount;

            // рекурсия для вложенных
            buildAllEdges(QList<ClassInfo>() << nested);
        }
    }
}


QString DotGenerator::escapeHtml(const QString &s)
{
    QString result = s;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    result.replace("\"", "&quot;");
    return result;
}


QString DotGenerator::nodeId(const ClassInfo &ci)
{
    QString id;
    if (!ci.outerClass.isEmpty())
        id = ci.outerClass + "__" + ci.name;
    else
        id = ci.name;

    id.replace('.', '_');
    id.replace(' ', '_');
    return id;
}
