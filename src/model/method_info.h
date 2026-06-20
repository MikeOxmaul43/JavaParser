/*!
* \file
* \brief Данный файл содержит описание метода и конструктора для программы Java Source Code Parser
*/
#pragma once

#include <QString>
#include <QList>
#include "param_info.h"



/**
 * @brief Метаданные метода или конструктора Java-класса.
 */
struct MethodInfo {
    QString          name;
    QString          returnType;
    QList<ParamInfo> params;
    QStringList      modifiers;
    bool             isStatic      = false;
    bool             isAbstract    = false;
    bool             isConstructor = false;
    bool             isOverloaded  = false;
};
