#pragma once
#include <QString>
#include <QStringList>
#include <QList>
#include "method_info.h"
#include "field_info.h"



/**
 * @brief Тип Java-сущности.
 */
enum class ClassType 
{
    CLASS,
    INTERFACE,
    ENUM,
};



/**
 * @brief Полное описание Java-класса, интерфейса или enum.
 */
struct ClassInfo
{
    QString name;
    ClassType type;
    QString packageName;
    QString outerClass;
    QStringList modifiers;
    QString extendsClass;
    QStringList implementsList;
    QList<FieldInfo> fields;
    QList<MethodInfo> methods;
    QList<MethodInfo> constructors;
    QList<ClassInfo> nestedClasses;
    QStringList enumConstants;
};