/*!
* \file
* \brief Данный файл содержит реализацию класса и методов FileWriter для программы Java Source Code Parser
*/
#include "file_writer.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

FileWriter::FileWriter(const QString &outputDir)
    : m_outputDir(outputDir)
{
}


void FileWriter::writeAll(const QList<ClassInfo> &classes)
{
    //Проходимся по всем классам и записываем их
    for (const ClassInfo &ci : classes)
        writeClass(ci, m_outputDir);
}

void FileWriter::writeClass(const ClassInfo &ci, const QString &parentPath)
{
    // Создаем директорию для текущего класса.
    QString classDir = parentPath + QDir::separator() + ci.name;
    QDir().mkpath(classDir);

    // Записываем общую информацию о классе.
    writeClassMeta(ci, classDir);

    // Сохраняем поля в подкаталог fields.
    if (!ci.fields.isEmpty()) {
        QString fieldsDir = classDir + QDir::separator() + "fields";
        QDir().mkpath(fieldsDir);

        for (const FieldInfo &fi : ci.fields)
            writeField(fi, fieldsDir);
    }

    // Сохраняем методы в подкаталог methods.
    if (!ci.methods.isEmpty()) {
        QString methodsDir = classDir + QDir::separator() + "methods";
        QDir().mkpath(methodsDir);

        for (const MethodInfo &mi : ci.methods)
            writeMethod(mi, methodsDir);
    }

    // Сохраняем конструкторы в отдельный подкаталог.
    if (!ci.constructors.isEmpty()) {
        QString ctorsDir = classDir + QDir::separator() + "constructors";
        QDir().mkpath(ctorsDir);

        for (const MethodInfo &mi : ci.constructors)
            writeConstructor(mi, ctorsDir);
    }

    // Рекурсивно обрабатываем вложенные классы.
    for (const ClassInfo &nested : ci.nestedClasses)
        writeClass(nested, classDir);
}

void FileWriter::writeClassMeta(const ClassInfo &ci, const QString &classDir)
{
    QFile file(classDir + QDir::separator() + "class.txt");

    // Если файл не удалось создать, пропускаем запись.
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    // Основная информация о классе.
    out << "name: "     << ci.name        << "\n";

    // Преобразуем тип класса в текстовое представление.
    QString typeStr;
    switch (ci.type) {
        case ClassType::CLASS:     typeStr = "class";     break;
        case ClassType::INTERFACE: typeStr = "interface"; break;
        case ClassType::ENUM:      typeStr = "enum";      break;
    }

    out << "type: "     << typeStr        << "\n";
    out << "package: "  << ci.packageName << "\n";

    // Записываем только заполненные свойства.
    if (!ci.outerClass.isEmpty())
        out << "outerClass: " << ci.outerClass << "\n";

    if (!ci.modifiers.isEmpty())
        out << "modifiers: " << ci.modifiers.join(' ') << "\n";

    if (!ci.extendsClass.isEmpty())
        out << "extends: " << ci.extendsClass << "\n";

    if (!ci.implementsList.isEmpty())
        out << "implements: " << ci.implementsList.join(' ') << "\n";
}


void FileWriter::writeField(const FieldInfo &fi, const QString &fieldsDir)
{
    QFile file(fieldsDir + QDir::separator() + fi.name + ".txt");

    // Создаем файл с именем поля.
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    // Записываем основные характеристики поля.
    out << "name: " << fi.name << "\n";
    out << "type: " << fi.type << "\n";

    // Записываем модификаторы поля.
    if (!fi.modifiers.isEmpty())
        out << "modifiers: " << fi.modifiers.join(' ') << "\n";

    if (fi.isStatic)
        out << "static: true\n";

    if (fi.isFinal)
        out << "final: true\n";
}


static QString resolveFileName(const QString &dir, const QString &baseName)
{
    // Для первого метода используем обычное имя.
    QString path = dir + QDir::separator() + baseName + ".txt";

    if (!QFile::exists(path))
        return path;

    // Если файл уже существует (например, перегрузка метода),
    // добавляем числовой суффикс.
    for (int n = 2; ; ++n) {
        path = dir + QDir::separator() +
               baseName + "_" + QString::number(n) + ".txt";

        if (!QFile::exists(path))
            return path;
    }
}

void FileWriter::writeMethod(const MethodInfo &mi, const QString &methodsDir)
{
    // Получаем уникальное имя файла с учетом перегрузок.
    QString filePath = resolveFileName(methodsDir, mi.name);

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    // Общая информация о методе.
    out << "name: "       << mi.name       << "\n";
    out << "returnType: " << mi.returnType << "\n";

    // Записываем модификаторы метода
    if (!mi.modifiers.isEmpty())
        out << "modifiers: " << mi.modifiers.join(' ') << "\n";

    if (mi.isStatic)
        out << "static: true\n";

    if (mi.isAbstract)
        out << "abstract: true\n";

    // Сохраняем список параметров метода.
    out << "params: " << mi.params.size() << "\n";

    for (const ParamInfo &p : mi.params)
        out << "  param: " << p.type << " " << p.name << "\n";
}


void FileWriter::writeConstructor(const MethodInfo &mi, const QString &ctorsDir)
{
    // Используем ту же схему именования, что и для методов.
    QString filePath = resolveFileName(ctorsDir, mi.name);

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    // Конструктор не имеет возвращаемого типа,
    // поэтому сохраняем только имя, модификаторы и параметры.
    out << "name: " << mi.name << "\n";

    // Записываем модификаторы конструктора
    if (!mi.modifiers.isEmpty())
        out << "modifiers: " << mi.modifiers.join(' ') << "\n";

    out << "params: " << mi.params.size() << "\n";

    for (const ParamInfo &p : mi.params)
        out << "  param: " << p.type << " " << p.name << "\n";
}
