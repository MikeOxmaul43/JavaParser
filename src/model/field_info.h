/*!
* \file
* \brief Данный файл содержит описание поля для программы Java Source Code Parser
*/
#pragma once
#include <QString>
#include <QStringList>


/**
 * @brief Метаданные поля Java-класса.
 */
struct FieldInfo
{
    QString name;
    QString type;
    QString returnType;
    QStringList modifiers;
    bool isStatic = false;
    bool isFinal = false;
};
