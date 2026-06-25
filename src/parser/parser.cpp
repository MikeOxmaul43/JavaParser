/*!
* \file
* \brief Данный файл содержит реализацию класса и методов парсера для программы Java Source Code Parser
*/
#include "parser.h"
#include <QtCore/qstring.h>

bool Parser::atEnd() const
{
    //проверяет находимся ли в конце текста
    return m_pos >= m_text.size();
}

bool Parser::isClassKeyword(const QString &w)
{
    //является ли слово объявлением класс(интерфейс, перечисление)
    return w == "class" || w == "interface" || w == "enum";
}

bool Parser::isModifier(const QString &w)
{
    // Список возможных модификаторов
    static const QStringList mods = {
        "public", "private", "protected",
        "static", "final", "abstract",
    };
    return mods.contains(w);
}



void Parser::skipSpaces()
{
    // Пропускает все пробельные символы с текущей позиции m_pos
    while (!atEnd() && m_text[m_pos].isSpace())
        ++m_pos;
}

void Parser::skipLineComment()
{
    // Пропускает однострочный комментарий (начиная с "//" до конца строки).
    // Предполагается, что текущая позиция уже стоит на символе после "//".
    while (!atEnd() && m_text[m_pos] != '\n')
        ++m_pos;
}

void Parser::skipBlockComment()
{
    // Пропускает многострочный комментарий (/* ... */).
    // Предполагается, что текущая позиция уже после "/*".
    // Останавливается на закрывающей последовательности "*/", сдвигая позицию за неё.
    while (!atEnd()) {
        if (m_text[m_pos] == '*' &&
            m_pos + 1 < m_text.size() &&
            m_text[m_pos + 1] == '/') {
            m_pos += 2; // перешагиваем через "*/"
            return;
        }
        ++m_pos;
    }
}

void Parser::skipBody()
{
    // Пропускаем пробелы перед возможной открывающей скобкой
    skipSpaces();

    // Если тела нет — выходим
    if (atEnd() || m_text[m_pos] != '{')
        return;

    // Переходим за открывающую скобку
    ++m_pos;

    // Текущая глубина вложенности
    int depth = 1;

    while (!atEnd() && depth > 0)
        processBodyChar(m_text[m_pos++], depth);
}

void Parser::skipToSemicolon()
{
    // Пропускает все символы до ближайшей точки с запятой (включительно).
    while (!atEnd() && m_text[m_pos] != ';')
        ++m_pos;
    if (!atEnd()) ++m_pos;
}


QString Parser::readWord()
{
    // Точный набор одиночных символов из спецификации
    static const QString singles = "{}()[],;=@.";

    // Пропустить неважные символы
    skipIgnored();

    // если в конце то ничего не возвращаем
    if (atEnd())
        return {};

    // одиночный символ-токен
    if (singles.contains(m_text[m_pos]))
        return QString(m_text[m_pos++]);

    // обычное слово
    QString word;

    while (!atEnd() &&
           !m_text[m_pos].isSpace() &&
           !singles.contains(m_text[m_pos]))
    {
        word += m_text[m_pos++];
    }

    return word;
}

// Заглянуть вперёд без движения позиции
QString Parser::peekWord()
{
    int saved = m_pos;
    QString w = readWord();
    m_pos = saved;
    return w;
}

QChar Parser::peekChar() const
{
    int p = m_pos;  // локальная копия позиции — не меняем состояние парсера

    while (p < m_text.size()) {

        // Пропускаем пробелы
        skipSpaces(p);

        // Пропускаем однострочный комментарий
        if (isLineComment(p)) {
            p += 2;
            skipLineComment(p);
            continue;
        }

        // Пропускаем многострочный комментарий
        if (isBlockComment(p)) {
            p += 2;
            skipBlockComment(p);
            continue;
        }

        return (p < m_text.size()) ? m_text[p] : QChar{};
    }

    return {};
}
void Parser::parsePackage()
{
    QString name;
    QString tok;
    // Читаем слова, пока не встретим ';' или пока не закончатся токены
    while ((tok = readWord()) != ";" && !tok.isEmpty())
        name += tok;          // добавляем прочитанное слово к имени
     m_package = name;         // сохраняем результат
}


void Parser::skipImports()
{
    // Пока следующий токен — "import", обрабатываем одну директиву
    while (peekWord() == "import") {
        readWord();          // съедаем "import"
        skipToSemicolon();   // пропускаем остаток до ';'
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
    parseClassBody(ci);

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

        // ЕСЛИ следующий символ @ → пропустить аннотацию
        if (peekChar() == '@') {
            skipAnnotation();
            continue;
        }

        // ЕСЛИ следующее слово "final" → прочитать и пропустить его
        if (peekWord() == "final")
            readWord();

        // Создать новый параметр
        ParamInfo pi;

        // Прочитать слово → записать как тип параметра
        pi.type = readWord();

        // Прочитать суффиксы массива после типа
        pi.type += readArraySuffix();

        // Прочитать varargs (...)
        if (readVarArgs())
            pi.type += "...";

        // Прочитать слово → записать как имя параметра
        pi.name = readWord();

        // Если имя параметра содержит []
        pi.type += readArraySuffix();

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

        if (w == "}" || w.isEmpty())
            break;

        if (w == ",")
            continue;

        if (w == ";") {
            parseEnumMembers(ci);
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

void Parser::skipQuotedLiteral(QChar terminator)
{
     // Идём по символам, пока не достигнем конца текста
    while (!atEnd()) {
        QChar c = m_text[m_pos++];

        // Экранированный символ: пропускаем следующий символ (он не является закрывающей кавычкой)
        if (c == '\\') {
            if (!atEnd())
                ++m_pos;
        }
        else if (c == terminator) {
            // Нашли закрывающую кавычку — завершаем пропуск
            return;
        }
    }
}

void Parser::skipComment()
{
    // Если в конце - выход
    if (atEnd())
        return;
    //если это начало однотсрочного комментария - пропустить его
    if (m_text[m_pos] == '/') {
        ++m_pos;
        skipLineComment();
    }
    //если это начало многостроного комментария - пропустить его
    else if (m_text[m_pos] == '*') {
        ++m_pos;
        skipBlockComment();
    }
}

void Parser::processBodyChar(QChar c, int &depth)
{
    // Если открывающая фигурная скобка - увеличить глубину
    if (c == '{') {
        ++depth;
    }
    // Если закрывающая фигурная скобка - кменьшить глубину
    else if (c == '}') {
        --depth;
    }
    // Пропуск строкового литерала
    else if (c == '"') {
        skipQuotedLiteral('"');
    }
    // Пропуск строкового литерала
    else if (c == '\'') {
        skipQuotedLiteral('\'');
    }
    // Пропуск комментария
    else if (c == '/') {
        skipComment();
    }
}


void Parser::parseClassBody(ClassInfo &ci)
{
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
        if (w.isEmpty() || w == '}')
            break;

        if (isClassKeyword(w)) {
            ci.nestedClasses << parseClass(mods, ci.name);
        }
        else if (w == "{") {
            skipBody();
        }
        else {
            parseMember(ci, mods);
        }
    }
}
void Parser::skipSpaces(int &pos) const
{
    // Пропуск пробелов
    while (pos < m_text.size() && m_text[pos].isSpace())
        ++pos;
}

void Parser::skipLineComment(int &pos) const
{
    // Пропуск однострочного комментария
    while (pos < m_text.size() && m_text[pos] != '\n')
        ++pos;
}

void Parser::skipBlockComment(int &pos) const
{
    // Пропускает многострочный комментарий (/* ... */).
    // Предполагается, что текущая позиция уже после "/*".
    while (pos < m_text.size()) {
        if (pos + 1 < m_text.size() &&
            m_text[pos] == '*' &&
            m_text[pos + 1] == '/') {
            pos += 2;
            return;
        }

        ++pos;
    }
}

bool Parser::isLineComment(int pos) const
{
    // Проверка что это однострочный комментарий
    return pos + 1 < m_text.size() &&
           m_text[pos] == '/' &&
           m_text[pos + 1] == '/';
}

bool Parser::isBlockComment(int pos) const
{
    // Проверка что это многострочный комментарий
    return pos + 1 < m_text.size() &&
           m_text[pos] == '/' &&
           m_text[pos + 1] == '*';
}


void Parser::skipIgnored()
{
    for (;;) {
        // Пропускаем все пробельные символы в начале
        skipSpaces();

         // Если достигнут конец текста, выходим
        if (atEnd())
            return;

         // Проверяем, начинается ли с текущей позиции однострочный комментарий "//"
        if (isLineComment(m_pos)) {
            m_pos += 2;
            skipLineComment();
            continue;
        }

        // Проверяем, начинается ли многострочный комментарий "/*"
        if (isBlockComment(m_pos)) {
            m_pos += 2;
            skipBlockComment();
            continue;
        }

        // Если ни пробелов, ни комментариев — значит достигнут значимый символ, выходим
        return;
    }
}

QString Parser::readArraySuffix()
{
    QString suffix;

    while (peekChar() == '[') {
        readWord(); // "["
        readWord(); // "]"
        suffix += "[]";
    }

    return suffix;
}

bool Parser::readVarArgs()
{
    if (peekChar() != '.')
        return false;

    readWord(); // "."
    readWord(); // "."
    readWord(); // "."

    return true;
}

void Parser::parseEnumMembers(ClassInfo &ci)
{
    // Читаем члены перечисления до тех пор, пока не встретим закрывающую '}'
    while (peekWord() != "}" && !atEnd()) {

        // Собираем все модификаторы (public, static, final и т.д.), которые могут предшествовать члену
        QStringList mods;

        while (isModifier(peekWord()))
            mods << readWord();

        // Смотрим на следующий токен без его извлечения
        QString peek = peekWord();

        if (peek.isEmpty() || peek == "}")
            break;

        // Если следующий токен — ключевое слово класса (class, interface, enum),
        // значит это вложенное объявление, парсим его как отдельный класс
        if (isClassKeyword(peek))
            ci.nestedClasses << parseClass(mods, ci.name);
        // Иначе — обычный член перечисления (константа, метод и т.п.)
        else
            parseMember(ci, mods);
    }
}
