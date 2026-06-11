/**
 * Модульные тесты для Parser
 * Приложения 1–9 из документа «Программа и методика испытаний»
 */

#include <QtTest/QtTest>
#include <QCoreApplication>
#include "../../src/parser/parser.h"

/// Разобрать строку и взять первый класс
#define PARSE_FIRST(src) \
    Parser p; \
    auto classes = p.parse(QString(src)); \
    QVERIFY(!classes.isEmpty()); \
    const ClassInfo &ci = classes.first()


// ════════════════════════════════════════════════════════════════════════════
// Приложение 1. Parser::readWord()
// Тестируется косвенно: parse разбирает строку, мы проверяем структуру.
// ════════════════════════════════════════════════════════════════════════════
class TestReadWord : public QObject
{
    Q_OBJECT
private slots:

    // 1-1 Простое слово
    void t11_simpleWord() {
        Parser p;
        auto r = p.parse("class A {}");
        QCOMPARE(r.size(), 1);
        QCOMPARE(r[0].name, QString("A"));
    }

    // 1-2 Идентификатор класса
    void t12_classIdentifier() {
        Parser p;
        auto r = p.parse("class MyClass {}");
        QCOMPARE(r[0].name, QString("MyClass"));
    }

    // 1-3 Открывающая фигурная скобка (класс разбирается без ошибок)
    void t13_openBrace() {
        Parser p;
        QVERIFY(!p.parse("class A {}").isEmpty());
    }

    // 1-4 Закрывающая фигурная скобка
    void t14_closeBrace() {
        Parser p;
        QVERIFY(!p.parse("class A {}").isEmpty());
    }

    // 1-5 Пропуск пробелов
    void t15_skipSpaces() {
        Parser p;
        auto r = p.parse("   class   A   {}");
        QCOMPARE(r[0].name, QString("A"));
    }

    // 1-6 Пропуск переноса строки
    void t16_skipNewlines() {
        Parser p;
        auto r = p.parse("\n\nclass A {}");
        QCOMPARE(r[0].name, QString("A"));
    }

    // 1-7 Пропуск line comment
    void t17_skipLineComment() {
        Parser p;
        auto r = p.parse("// test\nclass A {}");
        QCOMPARE(r[0].name, QString("A"));
    }

    // 1-8 Пропуск block comment
    void t18_skipBlockComment() {
        Parser p;
        auto r = p.parse("/* test */ class A {}");
        QCOMPARE(r[0].name, QString("A"));
    }

    // 1-9 Символ ( как отдельный токен — метод с пустыми скобками
    void t19_parenToken() {
        Parser p;
        auto r = p.parse("class A { void run(){} }");
        QCOMPARE(r[0].methods.size(), 1);
        QCOMPARE(r[0].methods[0].name, QString("run"));
    }

    // 1-10 Символ ; как отдельный токен
    void t110_semicolonToken() {
        Parser p;
        auto r = p.parse("class A { int x; }");
        QCOMPARE(r[0].fields.size(), 1);
        QCOMPARE(r[0].fields[0].name, QString("x"));
    }

    // 1-11 Символ , как отдельный токен
    void t111_commaToken() {
        Parser p;
        auto r = p.parse("class A { void f(int a, int b){} }");
        QCOMPARE(r[0].methods[0].params.size(), 2);
    }

    // 1-12 Разбор массива []
    void t112_arrayBrackets() {
        Parser p;
        auto r = p.parse("class A { int[] arr; }");
        QCOMPARE(r[0].fields[0].type, QString("int[]"));
    }

    // 1-13 Разбор varargs ...
    void t113_varargs() {
        Parser p;
        auto r = p.parse("class A { void log(String... args){} }");
        QCOMPARE(r[0].methods[0].params[0].type, QString("String..."));
    }

    // 1-14 Конец текста — пустой ввод
    void t114_endOfText() {
        Parser p;
        QVERIFY(p.parse("").isEmpty());
    }

    // 1-15 Большой блочный комментарий
    void t115_bigBlockComment() {
        Parser p;
        auto r = p.parse("/*Big\n *Block\n *Comment\n *void function();\n */ class A {}");
        QCOMPARE(r[0].name, QString("A"));
    }

    // 1-16 Много переносов строк
    void t116_manyNewlines() {
        Parser p;
        auto r = p.parse("\n\n\n\n\n\n\n\n\n\n\n\n\n\nclass A {}");
        QCOMPARE(r[0].name, QString("A"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 2. Parser::parseClass()
// ════════════════════════════════════════════════════════════════════════════
class TestParseClass : public QObject
{
    Q_OBJECT
private slots:

    void t11_simpleClass() {
        PARSE_FIRST("class User {}");
        QCOMPARE(ci.name, QString("User"));
        QCOMPARE(ci.type, ClassType::CLASS);
    }

    void t12_publicClass() {
        PARSE_FIRST("public class User {}");
        QCOMPARE(ci.name, QString("User"));
        QVERIFY(ci.modifiers.contains("public"));
    }

    void t13_abstractClass() {
        PARSE_FIRST("abstract class Base {}");
        QVERIFY(ci.modifiers.contains("abstract"));
        QCOMPARE(ci.name, QString("Base"));
    }

    void t14_interface() {
        PARSE_FIRST("interface Runnable {}");
        QCOMPARE(ci.type, ClassType::INTERFACE);
        QCOMPARE(ci.name, QString("Runnable"));
    }

    void t15_enum() {
        PARSE_FIRST("enum Color { RED }");
        QCOMPARE(ci.type, ClassType::ENUM);
        QCOMPARE(ci.name, QString("Color"));
        QVERIFY(ci.enumConstants.contains("RED"));
    }

    void t16_extends() {
        PARSE_FIRST("class A extends Base {}");
        QCOMPARE(ci.extendsClass, QString("Base"));
    }

    void t17_implementsOne() {
        PARSE_FIRST("class A implements Runnable {}");
        QVERIFY(ci.implementsList.contains("Runnable"));
    }

    void t18_implementsMultiple() {
        PARSE_FIRST("class A implements Runnable, Serializable {}");
        QVERIFY(ci.implementsList.contains("Runnable"));
        QVERIFY(ci.implementsList.contains("Serializable"));
    }

    void t19_nestedClass() {
        PARSE_FIRST("class Outer { class Inner {} }");
        QCOMPARE(ci.nestedClasses.size(), 1);
        QCOMPARE(ci.nestedClasses[0].name, QString("Inner"));
        QCOMPARE(ci.nestedClasses[0].outerClass, QString("Outer"));
    }

    void t110_classWithField() {
        PARSE_FIRST("class A { int x; }");
        QCOMPARE(ci.fields.size(), 1);
        QCOMPARE(ci.fields[0].type, QString("int"));
        QCOMPARE(ci.fields[0].name, QString("x"));
    }

    void t111_classWithMethod() {
        PARSE_FIRST("class A { void run(){} }");
        QCOMPARE(ci.methods.size(), 1);
        QCOMPARE(ci.methods[0].name, QString("run"));
    }

    void t112_classWithConstructor() {
        PARSE_FIRST("class A { A(){} }");
        QCOMPARE(ci.constructors.size(), 1);
        QVERIFY(ci.constructors[0].isConstructor);
    }

    void t113_staticNestedClass() {
        PARSE_FIRST("class A { static class B {} }");
        QCOMPARE(ci.nestedClasses.size(), 1);
        QVERIFY(ci.nestedClasses[0].modifiers.contains("static"));
    }

    void t114_packageNameSaved() {
        Parser p;
        auto r = p.parse("package test.core; class A {}");
        QCOMPARE(r[0].packageName, QString("test.core"));
    }

    void t115_publicAbstractClass() {
        PARSE_FIRST("public abstract class Shape {}");
        QVERIFY(ci.modifiers.contains("public"));
        QVERIFY(ci.modifiers.contains("abstract"));
    }

    void t116_extendsAndImplements() {
        PARSE_FIRST("public class Dog extends Animal implements Runnable, Cloneable {}");
        QCOMPARE(ci.extendsClass, QString("Animal"));
        QVERIFY(ci.implementsList.contains("Runnable"));
        QVERIFY(ci.implementsList.contains("Cloneable"));
        QVERIFY(ci.modifiers.contains("public"));
    }

    void t117_privateNestedClass() {
        PARSE_FIRST("class A { private class B {} }");
        QCOMPARE(ci.nestedClasses[0].name, QString("B"));
        QVERIFY(ci.nestedClasses[0].modifiers.contains("private"));
        QCOMPARE(ci.nestedClasses[0].outerClass, QString("A"));
    }

    void t118_newlinesInDeclaration() {
        PARSE_FIRST("public\nclass\nMyClass\nextends\nBase\n{}");
        QCOMPARE(ci.name, QString("MyClass"));
        QCOMPARE(ci.extendsClass, QString("Base"));
        QVERIFY(ci.modifiers.contains("public"));
    }

    void t119_commentsInDeclaration() {
        PARSE_FIRST("/* hdr */ public class Foo /* ok */ {}");
        QCOMPARE(ci.name, QString("Foo"));
        QVERIFY(ci.modifiers.contains("public"));
    }

    void t120_twoFieldsAndMethod() {
        PARSE_FIRST("class Point { int x; int y; void move(){} }");
        QCOMPARE(ci.fields.size(), 2);
        QCOMPARE(ci.methods.size(), 1);
    }

    // 2-21 Аннотация без параметров на классе
        void t121_annotationOnClass() {
            PARSE_FIRST("@Override class A {}");
            QCOMPARE(ci.name, QString("A"));
            QCOMPARE(ci.type, ClassType::CLASS);
        }
    
        // 2-22 Аннотация с параметром на классе
        void t122_annotationWithParamOnClass() {
            PARSE_FIRST("@SuppressWarnings(\"unchecked\") public class A {}");
            QCOMPARE(ci.name, QString("A"));
            QVERIFY(ci.modifiers.contains("public"));
        }
    
        // 2-23 Несколько аннотаций перед классом
        void t123_multipleAnnotationsOnClass() {
            PARSE_FIRST("@Deprecated @SuppressWarnings(\"all\") public class Foo {}");
            QCOMPARE(ci.name, QString("Foo"));
            QVERIFY(ci.modifiers.contains("public"));
        }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 3. Parser::parseMember()
// ════════════════════════════════════════════════════════════════════════════
class TestParseMember : public QObject
{
    Q_OBJECT
private slots:

    void t11_field() {
        PARSE_FIRST("class A { int x; }");
        QCOMPARE(ci.fields[0].type, QString("int"));
        QCOMPARE(ci.fields[0].name, QString("x"));
    }

    void t12_method() {
        PARSE_FIRST("class A { void run(){} }");
        QCOMPARE(ci.methods[0].returnType, QString("void"));
        QCOMPARE(ci.methods[0].name, QString("run"));
    }

    void t13_constructor() {
        PARSE_FIRST("class A { A(){} }");
        QCOMPARE(ci.constructors.size(), 1);
        QCOMPARE(ci.constructors[0].name, QString("A"));
    }

    void t14_arrayField() {
        PARSE_FIRST("class A { int[] arr; }");
        QCOMPARE(ci.fields[0].type, QString("int[]"));
        QCOMPARE(ci.fields[0].name, QString("arr"));
    }

    void t15_arrayMethod() {
        PARSE_FIRST("class A { int[] values(){} }");
        QCOMPARE(ci.methods[0].returnType, QString("int[]"));
        QCOMPARE(ci.methods[0].name, QString("values"));
    }

    void t16_staticField() {
        PARSE_FIRST("class A { static int count; }");
        QVERIFY(ci.fields[0].isStatic);
    }

    void t17_finalField() {
        PARSE_FIRST("class A { final int MAX; }");
        QVERIFY(ci.fields[0].isFinal);
    }

    void t18_staticMethod() {
        PARSE_FIRST("class A { static void log(){} }");
        QVERIFY(ci.methods[0].isStatic);
    }

    void t19_abstractMethod() {
        PARSE_FIRST("abstract class A { abstract void run(); }");
        QVERIFY(ci.methods[0].isAbstract);
    }

    void t110_packagePrivateField() {
        PARSE_FIRST("class A { int value; }");
        QVERIFY(ci.fields[0].modifiers.isEmpty());
    }

    void t111_publicStaticFinalField() {
        PARSE_FIRST("class A { public static final int SIZE = 10; }");
        QVERIFY(ci.fields[0].modifiers.contains("public"));
        QVERIFY(ci.fields[0].modifiers.contains("static"));
        QVERIFY(ci.fields[0].modifiers.contains("final"));
        QCOMPARE(ci.fields[0].type, QString("int"));
        QCOMPARE(ci.fields[0].name, QString("SIZE"));
    }

    void t112_protectedField() {
        PARSE_FIRST("class A { protected String label; }");
        QVERIFY(ci.fields[0].modifiers.contains("protected"));
    }

    void t113_privateFinalMethod() {
        PARSE_FIRST("class A { private final void lock(){} }");
        QVERIFY(ci.methods[0].modifiers.contains("private"));
        QVERIFY(ci.methods[0].modifiers.contains("final"));
    }

    void t114_methodWith4Params() {
        PARSE_FIRST("class A { void f(String n, int a, boolean f2, double v){} }");
        QCOMPARE(ci.methods[0].params.size(), 4);
    }

    void t115_methodWithNewlines() {
        PARSE_FIRST("class A {\n  void run()\n  {}}");
        QCOMPARE(ci.methods[0].name, QString("run"));
    }

    void t116_abstractMethodNoBody() {
        PARSE_FIRST("abstract class A { abstract int compute(); }");
        QVERIFY(ci.methods[0].isAbstract);
        QCOMPARE(ci.methods[0].returnType, QString("int"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 4. Parser::parseField()
// ════════════════════════════════════════════════════════════════════════════
class TestParseField : public QObject
{
    Q_OBJECT
private slots:

    void t11_simpleField() {
        PARSE_FIRST("class A { int count; }");
        QCOMPARE(ci.fields[0].type, QString("int"));
        QCOMPARE(ci.fields[0].name, QString("count"));
    }

    void t12_privateField() {
        PARSE_FIRST("class A { private int value; }");
        QVERIFY(ci.fields[0].modifiers.contains("private"));
    }

    void t13_staticFinalField() {
        PARSE_FIRST("class A { static final int MAX; }");
        QVERIFY(ci.fields[0].isStatic);
        QVERIFY(ci.fields[0].isFinal);
    }

    void t14_arrayField() {
        PARSE_FIRST("class A { int[] arr; }");
        QCOMPARE(ci.fields[0].type, QString("int[]"));
    }

    void t15_arraySuffixAfterName() {
        PARSE_FIRST("class A { int arr[]; }");
        QCOMPARE(ci.fields[0].type, QString("int[]"));
        QCOMPARE(ci.fields[0].name, QString("arr"));
    }

    void t16_fieldWithInitializer() {
        PARSE_FIRST("class A { int x = 10; }");
        QCOMPARE(ci.fields[0].type, QString("int"));
        QCOMPARE(ci.fields[0].name, QString("x"));
    }

    void t17_stringField() {
        PARSE_FIRST("class A { String name; }");
        QCOMPARE(ci.fields[0].type, QString("String"));
    }

    void t18_booleanField() {
        PARSE_FIRST("class A { boolean active; }");
        QCOMPARE(ci.fields[0].type, QString("boolean"));
    }

    void t19_underscoreField() {
        PARSE_FIRST("class A { int user_id; }");
        QCOMPARE(ci.fields[0].name, QString("user_id"));
    }

    void t110_multipleSpaces() {
        PARSE_FIRST("class A { int   count ; }");
        QCOMPARE(ci.fields[0].name, QString("count"));
    }

    void t111_publicStaticFinalString() {
        PARSE_FIRST("class A { public static final String TAG = \"X\"; }");
        QCOMPARE(ci.fields[0].type, QString("String"));
        QCOMPARE(ci.fields[0].name, QString("TAG"));
        QVERIFY(ci.fields[0].modifiers.contains("public"));
        QVERIFY(ci.fields[0].modifiers.contains("static"));
        QVERIFY(ci.fields[0].modifiers.contains("final"));
    }

    void t112_protectedInt() {
        PARSE_FIRST("class A { protected int score; }");
        QVERIFY(ci.fields[0].modifiers.contains("protected"));
    }

    void t113_privateLong() {
        PARSE_FIRST("class A { private long timestamp; }");
        QVERIFY(ci.fields[0].modifiers.contains("private"));
        QCOMPARE(ci.fields[0].type, QString("long"));
    }

    void t114_fieldWithNewlines() {
        PARSE_FIRST("class A {\n  int\n  count;\n}");
        QCOMPARE(ci.fields[0].name, QString("count"));
    }

    void t115_fieldWithInlineComment() {
        PARSE_FIRST("class A { int x; // coord\n}");
        QCOMPARE(ci.fields[0].name, QString("x"));
    }

    void t116_genericField() {
        PARSE_FIRST("class A { List<String> items; }");
        QCOMPARE(ci.fields[0].type, QString("List<String>"));
        QCOMPARE(ci.fields[0].name, QString("items"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 5. Parser::parseMethod()
// ════════════════════════════════════════════════════════════════════════════
class TestParseMethod : public QObject
{
    Q_OBJECT
private slots:

    void t11_voidMethod() {
        PARSE_FIRST("class A { void run(){} }");
        QCOMPARE(ci.methods[0].returnType, QString("void"));
        QCOMPARE(ci.methods[0].name, QString("run"));
    }

    void t12_intReturnType() {
        PARSE_FIRST("class A { int sum(){} }");
        QCOMPARE(ci.methods[0].returnType, QString("int"));
        QCOMPARE(ci.methods[0].name, QString("sum"));
    }

    void t13_staticMethod() {
        PARSE_FIRST("class A { static void log(){} }");
        QVERIFY(ci.methods[0].isStatic);
    }

    void t14_abstractMethod() {
        PARSE_FIRST("abstract class A { abstract void run(); }");
        QVERIFY(ci.methods[0].isAbstract);
    }

    void t15_methodWithParams() {
        PARSE_FIRST("class A { void test(int a, String b){} }");
        QCOMPARE(ci.methods[0].params.size(), 2);
        QCOMPARE(ci.methods[0].params[0].type, QString("int"));
        QCOMPARE(ci.methods[0].params[1].type, QString("String"));
    }

    void t16_arrayReturnType() {
        PARSE_FIRST("class A { int[] values(){} }");
        QCOMPARE(ci.methods[0].returnType, QString("int[]"));
    }

    void t17_varargsParam() {
        PARSE_FIRST("class A { void log(String... args){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("String..."));
    }

    void t18_noParams() {
        PARSE_FIRST("class A { void reset(){} }");
        QVERIFY(ci.methods[0].params.isEmpty());
    }

    void t19_finalParam() {
        PARSE_FIRST("class A { void set(final int x){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("int"));
        QCOMPARE(ci.methods[0].params[0].name, QString("x"));
    }

    void t110_methodWithComments() {
        PARSE_FIRST("class A { void run(){ /* comment */ } }");
        QCOMPARE(ci.methods[0].name, QString("run"));
    }

    void t111_protectedMethod() {
        PARSE_FIRST("class A { protected void init(){} }");
        QVERIFY(ci.methods[0].modifiers.contains("protected"));
    }

    void t112_privateMethod() {
        PARSE_FIRST("class A { private int compute(){} }");
        QVERIFY(ci.methods[0].modifiers.contains("private"));
    }

    void t113_publicMethod() {
        PARSE_FIRST("class A { public String getName(){} }");
        QVERIFY(ci.methods[0].modifiers.contains("public"));
        QCOMPARE(ci.methods[0].returnType, QString("String"));
    }

    void t114_manyParams() {
        PARSE_FIRST("class A { void create(String n, int a, long t, boolean f, double v){} }");
        QCOMPARE(ci.methods[0].params.size(), 5);
    }

    void t115_bodyWithComments() {
        PARSE_FIRST("class A { void process() { // step1\n /* step2 */ } }");
        QCOMPARE(ci.methods[0].name, QString("process"));
    }

    void t116_publicStaticFinalMethod() {
        PARSE_FIRST("class A { public static final void log(){} }");
        QVERIFY(ci.methods[0].modifiers.contains("public"));
        QVERIFY(ci.methods[0].modifiers.contains("static"));
        QVERIFY(ci.methods[0].modifiers.contains("final"));
    }

    void t117_annotatedParams() {
        PARSE_FIRST("class A { void save(@NotNull String s, @Min(0) int n){} }");
        QCOMPARE(ci.methods[0].params.size(), 2);
        QCOMPARE(ci.methods[0].params[0].type, QString("String"));
        QCOMPARE(ci.methods[0].params[1].type, QString("int"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 6. Parser::parseConstructor()
// ════════════════════════════════════════════════════════════════════════════
class TestParseConstructor : public QObject
{
    Q_OBJECT
private slots:

    void t11_emptyConstructor() {
        PARSE_FIRST("class User { User(){} }");
        QVERIFY(ci.constructors[0].isConstructor);
        QCOMPARE(ci.constructors[0].name, QString("User"));
        QVERIFY(ci.constructors[0].returnType.isEmpty());
    }

    void t12_withParam() {
        PARSE_FIRST("class User { User(String name){} }");
        QCOMPARE(ci.constructors[0].params.size(), 1);
        QCOMPARE(ci.constructors[0].params[0].type, QString("String"));
    }

    void t13_privateConstructor() {
        PARSE_FIRST("class A { private A(){} }");
        QVERIFY(ci.constructors[0].modifiers.contains("private"));
    }

    void t14_protectedConstructor() {
        PARSE_FIRST("class A { protected A(){} }");
        QVERIFY(ci.constructors[0].modifiers.contains("protected"));
    }

    void t15_arrayParam() {
        PARSE_FIRST("class A { A(int[] arr){} }");
        QCOMPARE(ci.constructors[0].params[0].type, QString("int[]"));
    }

    void t16_varargsParam() {
        PARSE_FIRST("class A { A(String... args){} }");
        QCOMPARE(ci.constructors[0].params[0].type, QString("String..."));
    }

    void t17_finalParam() {
        PARSE_FIRST("class A { A(final int x){} }");
        QCOMPARE(ci.constructors[0].params[0].name, QString("x"));
    }

    void t18_withComments() {
        PARSE_FIRST("class A { A(){ // test\n } }");
        QCOMPARE(ci.constructors[0].name, QString("A"));
    }

    void t19_nestedBlocks() {
        PARSE_FIRST("class A { A(){ if(true){ } } }");
        QCOMPARE(ci.constructors[0].name, QString("A"));
    }

    void t110_packagePrivate() {
        PARSE_FIRST("class A { A(){} }");
        QVERIFY(ci.constructors[0].modifiers.isEmpty());
    }

    void t111_publicWith2Params() {
        PARSE_FIRST("class Point { public Point(int x, int y){} }");
        QVERIFY(ci.constructors[0].modifiers.contains("public"));
        QCOMPARE(ci.constructors[0].params.size(), 2);
    }

    void t112_fourParams() {
        PARSE_FIRST("class A { A(String n, int a, boolean f, double v){} }");
        QCOMPARE(ci.constructors[0].params.size(), 4);
    }

    void t113_newlines() {
        PARSE_FIRST("class A { A(int x,\n   int y){} }");
        QCOMPARE(ci.constructors[0].params.size(), 2);
    }

    void t114_publicWithInlineComment() {
        PARSE_FIRST("class A { public A(int x) { // init\n } }");
        QVERIFY(ci.constructors[0].modifiers.contains("public"));
        QCOMPARE(ci.constructors[0].params[0].type, QString("int"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 7. Parser::parseParams()
// ════════════════════════════════════════════════════════════════════════════
class TestParseParams : public QObject
{
    Q_OBJECT
private slots:

    void t11_noParams() {
        PARSE_FIRST("class A { void run(){} }");
        QVERIFY(ci.methods[0].params.isEmpty());
    }

    void t12_oneParam() {
        PARSE_FIRST("class A { void test(int x){} }");
        QCOMPARE(ci.methods[0].params.size(), 1);
        QCOMPARE(ci.methods[0].params[0].type, QString("int"));
        QCOMPARE(ci.methods[0].params[0].name, QString("x"));
    }

    void t13_twoParams() {
        PARSE_FIRST("class A { void test(int a, String b){} }");
        QCOMPARE(ci.methods[0].params.size(), 2);
        QCOMPARE(ci.methods[0].params[0].type, QString("int"));
        QCOMPARE(ci.methods[0].params[1].type, QString("String"));
    }

    void t14_arrayParam() {
        PARSE_FIRST("class A { void test(int[] arr){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("int[]"));
    }

    void t15_arraySuffixAfterName() {
        PARSE_FIRST("class A { void test(int arr[]){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("int[]"));
        QCOMPARE(ci.methods[0].params[0].name, QString("arr"));
    }

    void t16_varargs() {
        PARSE_FIRST("class A { void log(String... args){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("String..."));
        QCOMPARE(ci.methods[0].params[0].name, QString("args"));
    }

    void t17_finalParam() {
        PARSE_FIRST("class A { void set(final int x){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("int"));
    }

    void t18_annotatedParam() {
        PARSE_FIRST("class A { void test(@NotNull String s){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("String"));
        QCOMPARE(ci.methods[0].params[0].name, QString("s"));
    }

    void t19_spacesAroundParams() {
        PARSE_FIRST("class A { void t( int a , String b ){} }");
        QCOMPARE(ci.methods[0].params.size(), 2);
    }

    void t110_primitiveVarargs() {
        PARSE_FIRST("class A { void sum(int... nums){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("int..."));
    }

    void t111_threeParams() {
        PARSE_FIRST("class A { void f(int a, String b, boolean c){} }");
        QCOMPARE(ci.methods[0].params.size(), 3);
    }

    void t112_fiveParams() {
        PARSE_FIRST("class A { void f(int a, int b, int c, String d, double e){} }");
        QCOMPARE(ci.methods[0].params.size(), 5);
    }

    void t113_paramsWithNewlines() {
        PARSE_FIRST("class A { void f(\n  int a,\n  String b){} }");
        QCOMPARE(ci.methods[0].params.size(), 2);
    }

    void t114_annotationAndFinal() {
        PARSE_FIRST("class A { void f(@NotNull final String s){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("String"));
        QCOMPARE(ci.methods[0].params[0].name, QString("s"));
    }

    void t115_genericParam() {
        PARSE_FIRST("class A { void f(List<String> items){} }");
        QCOMPARE(ci.methods[0].params[0].type, QString("List<String>"));
        QCOMPARE(ci.methods[0].params[0].name, QString("items"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 8. Parser::parseEnum()
// ════════════════════════════════════════════════════════════════════════════
class TestParseEnum : public QObject
{
    Q_OBJECT
private slots:

    void t11_simpleEnum() {
        PARSE_FIRST("enum Color { RED, GREEN, BLUE }");
        QCOMPARE(ci.enumConstants, QStringList({"RED","GREEN","BLUE"}));
    }

    void t12_oneConstant() {
        PARSE_FIRST("enum State { ON }");
        QVERIFY(ci.enumConstants.contains("ON"));
    }

    void t13_emptyEnum() {
        PARSE_FIRST("enum Empty {}");
        QVERIFY(ci.enumConstants.isEmpty());
    }

    void t14_enumWithField() {
        PARSE_FIRST("enum A { X; int code; }");
        QVERIFY(ci.enumConstants.contains("X"));
        QCOMPARE(ci.fields[0].type, QString("int"));
        QCOMPARE(ci.fields[0].name, QString("code"));
    }

    void t15_enumWithMethod() {
        PARSE_FIRST("enum A { X; void run(){} }");
        QVERIFY(ci.enumConstants.contains("X"));
        QCOMPARE(ci.methods[0].name, QString("run"));
    }

    void t17_multipleConstants() {
        PARSE_FIRST("enum A { X, Y, Z }");
        QCOMPARE(ci.enumConstants.size(), 3);
    }

    void t18_staticField() {
        PARSE_FIRST("enum A { X; static int count; }");
        QVERIFY(ci.fields[0].isStatic);
    }

    void t19_finalField() {
        PARSE_FIRST("enum A { X; final int code = 1; }");
        QVERIFY(ci.fields[0].isFinal);
    }

    void t110_nestedClass() {
        PARSE_FIRST("enum A { X; class Inner{} }");
        QCOMPARE(ci.nestedClasses.size(), 1);
        QCOMPARE(ci.nestedClasses[0].name, QString("Inner"));
    }

    void t111_publicEnum() {
        PARSE_FIRST("public enum Status { ACTIVE, INACTIVE }");
        QVERIFY(ci.modifiers.contains("public"));
        QVERIFY(ci.enumConstants.contains("ACTIVE"));
        QVERIFY(ci.enumConstants.contains("INACTIVE"));
    }

    void t112_enumWithMethodAndField() {
        PARSE_FIRST("enum Planet { EARTH; double mass; double radius(){ return 0; } }");
        QVERIFY(ci.enumConstants.contains("EARTH"));
        QCOMPARE(ci.fields[0].type, QString("double"));
        QCOMPARE(ci.methods[0].name, QString("radius"));
    }

    void t113_enumWithComments() {
        PARSE_FIRST("/* day */ enum Day { /* w */ MON, TUE, WED }");
        QCOMPARE(ci.enumConstants.size(), 3);
        QVERIFY(ci.enumConstants.contains("MON"));
        QVERIFY(ci.enumConstants.contains("WED"));
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Приложение 9. Parser::parse()
// ════════════════════════════════════════════════════════════════════════════
class TestParse : public QObject
{
    Q_OBJECT
private slots:

    void t11_oneClass() {
        Parser p; auto r = p.parse("class A {}");
        QCOMPARE(r.size(), 1);
        QCOMPARE(r[0].type, ClassType::CLASS);
        QCOMPARE(r[0].name, QString("A"));
    }

    void t12_package() {
        Parser p; auto r = p.parse("package com.test; class A {}");
        QCOMPARE(r[0].packageName, QString("com.test"));
    }

    void t13_importSkipped() {
        Parser p; auto r = p.parse("import java.util.List; class A {}");
        QCOMPARE(r.size(), 1);
        QCOMPARE(r[0].name, QString("A"));
    }

    void t14_multipleImports() {
        Parser p; auto r = p.parse("import a.A; import b.B; class Test {}");
        QCOMPARE(r.size(), 1);
        QCOMPARE(r[0].name, QString("Test"));
    }

    void t15_twoTopLevelClasses() {
        Parser p; auto r = p.parse("class A {} class B {}");
        QCOMPARE(r.size(), 2);
        QCOMPARE(r[0].name, QString("A"));
        QCOMPARE(r[1].name, QString("B"));
    }

    void t16_classFields() {
        Parser p; auto r = p.parse("class User { int age; String name; }");
        QCOMPARE(r[0].fields.size(), 2);
    }

    void t17_classMethods() {
        Parser p; auto r = p.parse("class A { void run(){} int sum(){} }");
        QCOMPARE(r[0].methods.size(), 2);
    }

    void t18_constructor() {
        Parser p; auto r = p.parse("class User { User(){} }");
        QCOMPARE(r[0].constructors.size(), 1);
        QVERIFY(r[0].constructors[0].isConstructor);
    }

    void t19_nestedClass() {
        Parser p; auto r = p.parse("class Outer { class Inner {} }");
        QCOMPARE(r[0].nestedClasses.size(), 1);
        QCOMPARE(r[0].nestedClasses[0].name, QString("Inner"));
    }

    void t110_interface() {
        Parser p; auto r = p.parse("interface Runnable { void run(); }");
        QCOMPARE(r[0].type, ClassType::INTERFACE);
        QCOMPARE(r[0].methods[0].name, QString("run"));
    }

    void t111_enum() {
        Parser p; auto r = p.parse("enum Color { RED, GREEN }");
        QCOMPARE(r[0].type, ClassType::ENUM);
        QVERIFY(r[0].enumConstants.contains("RED"));
        QVERIFY(r[0].enumConstants.contains("GREEN"));
    }

    void t112_enumWithMethod() {
        Parser p; auto r = p.parse("enum A { X; void run(){} }");
        QVERIFY(r[0].enumConstants.contains("X"));
        QCOMPARE(r[0].methods[0].name, QString("run"));
    }

    void t113_methodWithParams() {
        Parser p; auto r = p.parse("class A { void test(int a, String b){} }");
        QCOMPARE(r[0].methods[0].params.size(), 2);
    }

    void t114_varargs() {
        Parser p; auto r = p.parse("class A { void log(String... args){} }");
        QCOMPARE(r[0].methods[0].params[0].type, QString("String..."));
    }

    void t115_arrayField() {
        Parser p; auto r = p.parse("class A { int[] arr; }");
        QCOMPARE(r[0].fields[0].type, QString("int[]"));
    }

    void t116_lineCommentOnly() {
        Parser p; auto r = p.parse("// comment\nclass A {}");
        QCOMPARE(r[0].name, QString("A"));
    }

    void t117_blockCommentOnly() {
        Parser p; auto r = p.parse("/* comment class A */");
        QVERIFY(r.isEmpty());
    }

    void t118_commentsInsideMethod() {
        Parser p; auto r = p.parse("class A { void run(){ /*x*/ } }");
        QCOMPARE(r[0].methods[0].name, QString("run"));
    }

    void t119_stringWithBraces() {
        Parser p; auto r = p.parse("class A { void t(){ String s = \"{x}\"; } }");
        QCOMPARE(r[0].methods[0].name, QString("t"));
    }

    void t120_charLiteralWithBrace() {
        Parser p; auto r = p.parse("class A { void t(){ char c = '}'; } }");
        QCOMPARE(r[0].methods[0].name, QString("t"));
    }

    void t121_bigIntegration() {
        const QString src = R"(
package com.example.app;
import java.util.List;
import java.io.Serializable;
/** Main */
public abstract class BaseService extends Object implements Serializable {
    public static final int MAX = 100;
    private String name;
    protected boolean active;
    public BaseService(String name, boolean active) {}
    public abstract void process(int id, String data);
    protected static List<String> getAll(int page, int size) { return null; }
    private boolean validate(String s, int min, int max) { return true; }
    public static class Builder {
        private String bname;
        public Builder setName(String n) { return this; }
    }
    enum State { INIT, RUNNING, DONE }
}
)";
        Parser p; auto r = p.parse(src);
        QCOMPARE(r.size(), 1);
        const ClassInfo &ci = r[0];
        QCOMPARE(ci.name, QString("BaseService"));
        QCOMPARE(ci.packageName, QString("com.example.app"));
        QVERIFY(ci.modifiers.contains("public"));
        QVERIFY(ci.modifiers.contains("abstract"));
        QCOMPARE(ci.extendsClass, QString("Object"));
        QVERIFY(ci.implementsList.contains("Serializable"));
        QCOMPARE(ci.fields.size(), 3);
        QCOMPARE(ci.constructors.size(), 1);
        QCOMPARE(ci.constructors[0].params.size(), 2);
        // process, getAll, validate — 3 direct methods; setName belongs to nested Builder
        QVERIFY(ci.methods.size() >= 3);
        QCOMPARE(ci.nestedClasses.size(), 2); // Builder + State
        bool hasBuilder = false, hasState = false;
        for (auto &nc : ci.nestedClasses) {
            if (nc.name == "Builder") hasBuilder = true;
            if (nc.name == "State") hasState = true;
        }
        QVERIFY(hasBuilder);
        QVERIFY(hasState);
    }

    void t122_newlinesEverywhere() {
        Parser p; auto r = p.parse("public\nclass\nFoo\n{int x;}");
        QCOMPARE(r[0].name, QString("Foo"));
        QVERIFY(r[0].modifiers.contains("public"));
        QCOMPARE(r[0].fields[0].name, QString("x"));
    }

    void t123_mixedComments() {
        Parser p; auto r = p.parse("/* pkg */ package a.b; // ignore\nclass C { /* f */ int x; }");
        QCOMPARE(r[0].name, QString("C"));
        QCOMPARE(r[0].packageName, QString("a.b"));
        QCOMPARE(r[0].fields[0].name, QString("x"));
    }

    void t124_fiveParamMethod() {
        Parser p; auto r = p.parse("class A { void f(int a,int b,int c,String d,boolean e){} }");
        QCOMPARE(r[0].methods[0].params.size(), 5);
    }

    void t125_publicStaticFinalDouble() {
        Parser p; auto r = p.parse("public class Const { public static final double PI = 3.14; }");
        QVERIFY(r[0].fields[0].modifiers.contains("public"));
        QVERIFY(r[0].fields[0].modifiers.contains("static"));
        QVERIFY(r[0].fields[0].modifiers.contains("final"));
        QCOMPARE(r[0].fields[0].type, QString("double"));
    }

    void t126_nestedEnum() {
        Parser p; auto r = p.parse("class A { enum Status { ON, OFF } }");
        QCOMPARE(r[0].nestedClasses.size(), 1);
        QCOMPARE(r[0].nestedClasses[0].type, ClassType::ENUM);
        QCOMPARE(r[0].nestedClasses[0].name, QString("Status"));
        QVERIFY(r[0].nestedClasses[0].enumConstants.contains("ON"));
        QVERIFY(r[0].nestedClasses[0].enumConstants.contains("OFF"));
    }

    void t127_newlinesInClassDeclaration() {
        Parser p; auto r = p.parse("public class Bar\nextends Base\nimplements I1, I2\n{}");
        QCOMPARE(r[0].name, QString("Bar"));
        QVERIFY(r[0].modifiers.contains("public"));
        QCOMPARE(r[0].extendsClass, QString("Base"));
        QVERIFY(r[0].implementsList.contains("I1"));
        QVERIFY(r[0].implementsList.contains("I2"));
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int status = 0;
    auto run = [&](QObject *obj){
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    run(new TestReadWord);
    run(new TestParseClass);
    run(new TestParseMember);
    run(new TestParseField);
    run(new TestParseMethod);
    run(new TestParseConstructor);
    run(new TestParseParams);
    run(new TestParseEnum);
    run(new TestParse);

    return status;
}

#include "tst_parser.moc"
