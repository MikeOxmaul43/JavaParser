/*!
* \file
* \brief Данный файл содержит реализацию класса и методов парсера для программы Java Source Code Parser
*/
#include "parser.h"
#include <QtCore/qstring.h>

bool Parser::atEnd() const
{
    return m_pos >= m_text.size();
}

bool Parser::isClassKeyword(const QString &w)
{
    return w == "class" || w == "interface" || w == "enum";
}

bool Parser::isModifier(const QString &w)
{
    static const QStringList mods = {
        "public", "private", "protected",
        "static", "final", "abstract",
    };
    return mods.contains(w);
}



// Пропустить все пробелы и переносы строк
void Parser::skipSpaces()
{
    while (!atEnd() && m_text[m_pos].isSpace())
        ++m_pos;
}

// Пропустить строку до конца  (// комментарий)
void Parser::skipLineComment()
{
    while (!atEnd() && m_text[m_pos] != '\n')
        ++m_pos;
}

// Пропустить всё до */  (вызывается когда уже прочитали /*)
void Parser::skipBlockComment()
{
    while (!atEnd()) {
        if (m_text[m_pos] == '*' &&
            m_pos + 1 < m_text.size() &&
            m_text[m_pos + 1] == '/') {
            m_pos += 2;
            return;
        }
        ++m_pos;
    }
}

// Пропустить тело { } считая глубину вложенности
void Parser::skipBody()
{
    skipSpaces();
    if (!atEnd() && m_text[m_pos] == '{')
        ++m_pos;
    else
        return;

    int depth = 1;
    while (!atEnd() && depth > 0) {
        QChar c = m_text[m_pos++];
        if      (c == '{') ++depth;
        else if (c == '}') --depth;
        else if (c == '"') {
            while (!atEnd()) {
                QChar sc = m_text[m_pos++];
                if      (sc == '\\') { if (!atEnd()) ++m_pos; }
                else if (sc == '"')  break;
            }
        } else if (c == '\'') {
            while (!atEnd()) {
                QChar sc = m_text[m_pos++];
                if      (sc == '\\') { if (!atEnd()) ++m_pos; }
                else if (sc == '\'') break;
            }
        } else if (c == '/' && !atEnd()) {
            if      (m_text[m_pos] == '/') { ++m_pos; skipLineComment();  }
            else if (m_text[m_pos] == '*') { ++m_pos; skipBlockComment(); }
        }
    }
}

// Пропустить всё до ; включительно
void Parser::skipToSemicolon()
{
    while (!atEnd() && m_text[m_pos] != ';')
        ++m_pos;
    if (!atEnd()) ++m_pos;
}


QString Parser::readWord()
{
    // Точный набор одиночных символов из спецификации
    static const QString singles = "{}()[],;=@.";

    for (;;) {
        // 1. пропустить пробелы
        skipSpaces();

        // 2. конец текста
        if (atEnd()) return {};

        // 3. однострочный комментарий
        if (m_text[m_pos] == '/' &&
            m_pos + 1 < m_text.size() &&
            m_text[m_pos + 1] == '/') {
            m_pos += 2;
            skipLineComment();
            continue;  // ВЕРНУТЬ readWord() — рекурсия через цикл
        }

        // 4. блочный комментарий
        if (m_text[m_pos] == '/' &&
            m_pos + 1 < m_text.size() &&
            m_text[m_pos + 1] == '*') {
            m_pos += 2;
            skipBlockComment();
            continue;  // ВЕРНУТЬ readWord() — рекурсия через цикл
        }

        // 5. одиночный символ-токен
        if (singles.contains(m_text[m_pos]))
            return QString(m_text[m_pos++]);

        // 6. обычное слово
        QString word;
        while (!atEnd() &&
               !m_text[m_pos].isSpace() &&
               !singles.contains(m_text[m_pos]))
            word += m_text[m_pos++];
        return word;
    }
}

// Заглянуть вперёд без движения позиции
QString Parser::peekWord()
{
    int saved = m_pos;
    QString w = readWord();
    m_pos = saved;
    return w;
}

// Следующий непробельный символ без сдвига позиции
QChar Parser::peekChar() const
{
    int p = m_pos;
    for (;;) {
        while (p < m_text.size() && m_text[p].isSpace()) ++p;

        if (p + 1 < m_text.size() && m_text[p] == '/' && m_text[p + 1] == '/') {
            p += 2;
            while (p < m_text.size() && m_text[p] != '\n') ++p;
            continue;
        }

        if (p + 1 < m_text.size() && m_text[p] == '/' && m_text[p + 1] == '*') {
            p += 2;
            while (p + 1 < m_text.size() &&
                   !(m_text[p] == '*' && m_text[p + 1] == '/'))
                ++p;
            p = (p + 1 < m_text.size()) ? p + 2 : m_text.size();
            continue;
        }

        break;
    }

    if (p < m_text.size()) return m_text[p];
    return {};
}

void Parser::parsePackage()
{
    QString name;
    QString tok;
    while ((tok = readWord()) != ";" && !tok.isEmpty())
        name += tok;
    m_package = name;
}


void Parser::skipImports()
{
    while (peekWord() == "import") {
        readWord(); // "import"
        skipToSemicolon();
    }
}



QList<ClassInfo> Parser::parse(const QString &text)
{
    // 1. Запомнить текст, встать на начало
    m_text = text;
    m_pos  = 0;
    m_package.clear();

    // 2. Пустой список
    QList<ClassInfo> result;

    // 3. Имя пакета
    if (peekWord() == "package") {
        readWord(); // "package"
        parsePackage();
    }

    // 4. Пропустить импорты
    skipImports();

    // 5. Основной цикл
    while (!atEnd()) {
        // Прочитать модификаторы
        QStringList mods;
        while (isModifier(peekWord()))
            mods << readWord();

        // ЕСЛИ следующее слово является объявлением класса/интерфейса/перечисления
        if (isClassKeyword(peekWord())) {
            result << parseClass(mods, {});
        } else {
            // ИНАЧЕ → прочитать слово и пропустить его
            QString w = readWord();
            if (w.isEmpty()) break;
        }
    }

    // 6. Вернуть список классов
    return result;
}


ClassInfo Parser::parseClass(const QStringList &modifiers, const QString &outerClass)
{
    // 1-2.
    ClassInfo ci;
    ci.modifiers   = modifiers;
    ci.outerClass  = outerClass;
    ci.packageName = m_package;

    // 3. Прочитать "class" / "interface" / "enum"
    QString kw = readWord();
    if      (kw == "interface") ci.type = ClassType::INTERFACE;
    else if (kw == "enum")      ci.type = ClassType::ENUM;
    else                        ci.type = ClassType::CLASS;

    // 4. Имя класса
    ci.name = readWord();

    // 5. extends
    if (peekWord() == "extends") {
        readWord();             // "extends"
        ci.extendsClass = readWord(); // родительский класс
    }

    // 6. implements
    if (peekWord() == "implements") {
        readWord(); // "implements"
        // ПОКА следующее слово не открывающая фигурная скобка
        while (peekWord() != "{" && !atEnd()) {
            QString tok = readWord();
            // ЕСЛИ это не запятая → добавить в список интерфейсов
            if (tok != ",")
                ci.implementsList << tok;
        }
    }

    // 7. Прочитать {
    readWord(); // "{"

    if (ci.type == ClassType::ENUM) {
        parseEnum(ci);
        return ci;
    }

    // 8. ПОКА следующее слово не }
    while (peekWord() != "}" && !atEnd()) {
        // Пропустить аннотации
        while (peekChar() == '@')
            skipAnnotation();

        // Читать модификаторы
        QStringList mods;
        while (isModifier(peekWord()))
            mods << readWord();

        // Пропустить аннотации
        while (peekChar() == '@')
            skipAnnotation();

        QString w = peekWord();
        if (w.isEmpty() || w == "}") break;

        if (isClassKeyword(w)) {
            // Разобрать вложенный класс, добавить в список вложенных
            ci.nestedClasses << parseClass(mods, ci.name);
        } else if (w == "{") {
            // ИНАЧЕ ЕСЛИ следующее слово — открывающая фигурная скобка
            // (статический / экземплярный инициализатор — пропускаем)
            skipBody();
        } else {
            // Определить и разобрать поле, метод или конструктор
            parseMember(ci, mods);
        }
    }

    // 9. Прочитать }
    readWord(); // "}"
    // 10. Вернуть
    return ci;
}


void Parser::parseMember(ClassInfo &ci, const QStringList &modifiers)
{
    // 1. Прочитать первое слово
    QString first = readWord();

    // 2. ПОКА следующий символ [
    while (peekChar() == '[') {
        readWord(); // "["
        readWord(); // "]"
        first += "[]";
    }

    // 3. Проверяем конструктор
    if (first == ci.name && peekChar() == '(') {
        ci.constructors << parseConstructor(modifiers, first);
        return; // ВЫЙТИ ИЗ МЕТОДА
    }

    // 4. Прочитать второе слово
    QString second = readWord();

    // 5. ПОКА следующий символ [
    while (peekChar() == '[') {
        readWord(); // "["
        readWord(); // "]"
        first += "[]";
    }

    // 6. ЕСЛИ следующий символ (
    if (peekChar() == '(') {
        ci.methods << parseMethod(modifiers, first, second);
        return; // ВЫЙТИ ИЗ МЕТОДА
    }

    // 7. Разобрать как поле
    ci.fields << parseField(modifiers, first, second);
}


FieldInfo Parser::parseField(const QStringList &modifiers,
                              const QString &type,
                              const QString &name)
{
    // 1. Создать описание
    FieldInfo fi;

    // 2. Записать модификаторы, тип, имя
    fi.modifiers = modifiers;
    fi.type      = type;
    fi.name      = name;

    // 3. ЕСЛИ среди модификаторов "static"
    fi.isStatic = modifiers.contains("static");

    // 4. ЕСЛИ среди модификаторов "final"
    fi.isFinal = modifiers.contains("final");

    // 5. ПОКА следующий символ [
    while (peekChar() == '[') {
        readWord(); // "["
        readWord(); // "]"
        fi.type += "[]";
    }

    // 6. ЕСЛИ следующий символ =
    if (peekChar() == '=') {
        skipToSemicolon(); // пропустить всё до ; включительно
    } else {
        // 7. ИНАЧЕ → прочитать ;
        readWord(); // ";"
    }

    // 8. Вернуть
    return fi;
}


MethodInfo Parser::parseMethod(const QStringList &modifiers,
                                const QString &returnType,
                                const QString &name)
{
    // 1. Создать описание
    MethodInfo mi;

    // 2. Записать модификаторы, тип возврата, имя
    mi.modifiers  = modifiers;
    mi.returnType = returnType;
    mi.name       = name;

    // 3. Отметить isConstructor = false
    mi.isConstructor = false;

    // 4. ЕСЛИ среди модификаторов "static"
    mi.isStatic = modifiers.contains("static");

    // 5. ЕСЛИ среди модификаторов "abstract"
    mi.isAbstract = modifiers.contains("abstract");

    // 6. Разобрать и записать список параметров
    mi.params = parseParams();

    // 7. Пропустить тело метода целиком
    // (абстрактный метод / метод интерфейса завершается ";")
    if (peekChar() == ';')
        readWord(); // ";"
    else
        skipBody();

    // 8. Вернуть
    return mi;
}

MethodInfo Parser::parseConstructor(const QStringList &modifiers,
                                     const QString &name)
{
    // 1. Создать описание
    MethodInfo mi;

    // 2. Записать модификаторы и имя
    mi.modifiers = modifiers;
    mi.name      = name;

    // 3. Пустая строка как тип возврата
    mi.returnType = QString();

    // 4. Отметить isConstructor = true
    mi.isConstructor = true;

    // 5. Отметить isStatic = false, isAbstract = false
    mi.isStatic   = false;
    mi.isAbstract = false;

    // 6. Разобрать и записать список параметров
    mi.params = parseParams();

    // 7. Пропустить тело конструктора целиком
    skipBody();

    // 8. Вернуть
    return mi;
}

QList<ParamInfo> Parser::parseParams()
{
    // 1. Создать пустой список
    QList<ParamInfo> params;

    // 2. Прочитать (
    readWord(); // "("

    // 3. ЕСЛИ следующий символ )
    if (peekChar() == ')') {
        readWord(); // ")"
        return params; // вернуть пустой список
    }

    // 4. ПОКА следующий символ не )
    while (peekChar() != ')' && !atEnd()) {

        // ЕСЛИ следующий символ @ → прочитать и пропустить аннотацию
        if (peekChar() == '@') {
            readWord(); // "@"
            readWord(); // имя аннотации
            if (peekChar() == '(') {
                readWord(); // "("
                int d = 1;
                while (d > 0 && !atEnd()) {
                    QString t = readWord();
                    if      (t == "(") ++d;
                    else if (t == ")") --d;
                }
            }
            continue;
        }

        // ЕСЛИ следующее слово "final" → прочитать и пропустить его
        if (peekWord() == "final")
            readWord();

        // Создать новый параметр
        ParamInfo pi;

        // Прочитать слово → записать как тип параметра
        pi.type = readWord();

        // ПОКА следующий символ [
        while (peekChar() == '[') {
            readWord(); // "["
            readWord(); // "]"
            pi.type += "[]";
        }

        // ЕСЛИ следующее слово "..."
        // Примечание: readWord() разбивает "..." на три отдельных токена "."
        // (т.к. "." входит в список одиночных символов), поэтому читаем три точки
        if (peekChar() == '.') {
            readWord(); // "."
            readWord(); // "."
            readWord(); // "."
            pi.type += "...";
        }

        // Прочитать слово → записать как имя параметра
        pi.name = readWord();
        // Если имя параметра содержит []
        while (peekChar() == '[') {
            readWord(); // "["
            readWord(); // "]"
            pi.type += "[]";
        }

        // Добавить параметр в список
        params << pi;

        // ЕСЛИ следующий символ , → прочитать ,
        if (peekChar() == ',')
            readWord();
    }

    // 5. Прочитать )
    readWord(); // ")"

    // 6. Вернуть список
    return params;
}


void Parser::parseEnum(ClassInfo &ci)
{
    // уже прочитали '{'
    // читаем константы через запятую пока не '}' или ';'
    while (!atEnd()) {
        QString w = readWord();
        if (w == "}" || w.isEmpty()) break;
        if (w == ",") continue;
        if (w == ";") {
            // после ';' могут идти обычные члены enum — разбираем их
            while (peekWord() != "}" && !atEnd()) {
                QStringList mods;
                while (isModifier(peekWord()))
                    mods << readWord();
                QString peek = peekWord();
                if (peek.isEmpty() || peek == "}") break;

                if(isClassKeyword(peek)){
                    ci.nestedClasses << parseClass(mods, ci.name);
                } else {
                     parseMember(ci, mods);
                }
            }
            readWord(); // '}'
            return;
        }
        // это константа — просто запоминаем имя
        ci.enumConstants << w;
    }
}

void Parser::skipAnnotation()
{
    readWord(); // "@"
    readWord(); // имя аннотации
    if (peekChar() == '(') {
        readWord(); // "("
        int d = 1;
        while (d > 0 && !atEnd()) {
            QString t = readWord();
            if      (t == "(") ++d;
            else if (t == ")") --d;
        }
    }
}
