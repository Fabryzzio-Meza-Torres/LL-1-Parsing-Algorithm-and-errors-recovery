#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Definición de tokens y scanner
class Token {
public:
    Token()=default;
    enum Type { ID, SEMICOLON, ASSIGN, PRINT, LPAREN, RPAREN, PLUS, MINUS, MULT, NUM, END, ERR };
    static const char* token_names[12];
    Type type;
    string lexema;
    Token(Type type):type(type) { }
    Token(Type type, const string& lex):type(type), lexema(lex) { }
};

const char* Token::token_names[12] = {"id", ";", "=", "print", "(", ")", "+", "-", "*", "Num", "$", "ERR"};

ostream& operator << (ostream& os, const Token& tok) {
    return os << Token::token_names[tok.type] << " (" << tok.lexema << ")";
}

// Simulando un escáner que tokeniza la entrada
class Scanner {
public:
    Scanner(const string& in) : input(in), pos(0) {}
    Token nextToken() {
        while (pos < input.size() && isspace(input[pos])) pos++;
        if (pos >= input.size()) return Token(Token::END);

        char current = input[pos];
        if (isalpha(current)) {
            string id;
            while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) id += input[pos++];
            if (id == "print") return Token(Token::PRINT, id);
            return Token(Token::ID, id);
        }
        if (isdigit(current)) {
            string num;
            while (pos < input.size() && isdigit(input[pos])) num += input[pos++];
            return Token(Token::NUM, num);
        }

        switch (current) {
            case ';': pos++; return Token(Token::SEMICOLON, ";");
            case '=': pos++; return Token(Token::ASSIGN, "=");
            case '(': pos++; return Token(Token::LPAREN, "(");
            case ')': pos++; return Token(Token::RPAREN, ")");
            case '+': pos++; return Token(Token::PLUS, "+");
            case '-': pos++; return Token(Token::MINUS, "-");
            case '*': pos++; return Token(Token::MULT, "*");
            default: return Token(Token::ERR, string(1, current));
        }
    }
private:
    string input;
    size_t pos;
};

// Parser LL1 con tabla predictiva
class ParserLL1 {
public:
    ParserLL1(const vector<Token>& tokens) : entrada(tokens), index(0) {
        inicializarTablaLL1();
        pila.push("$");
        pila.push("P");
    }

    void inicializarTablaLL1() {
        // LL1 Parsing Table
        LL1_Parsing_Table = {
            {"P", {{"id", {"SL"}}, {"print", {"SL"}}}},
            {"SL", {{"id", {"S", "SL'"}}, {"print", {"S", "SL'"}}}},
            {"SL'", {{";", {"S", "SL'"}}, {"$", {""}}}},
            {"S", {{"id", {"id", "=", "E"}}, {"print", {"print", "E"}}}},
            {"E", {{"id", {"T", "E'"}}, {"Num", {"T", "E'"}}, {"(", {"T", "E'"}}}},
            {"E'", {{";", {""}}, {")", {""}}, {"+", {"+", "T", "E'"}}, {"-", {"-", "T", "E'"}}, {"$", {""}}}},
            {"T", {{"id", {"F", "T'"}}, {"Num", {"F", "T'"}}, {"(", {"F", "T'"}}}},
            {"T'", {{"*", {"*", "F", "T'"}}, {";", {""}}, {")", {""}}, {"+", {""}}, {"-", {""}}, {"$", {""}}}},
            {"F", {{"id", {"id"}}, {"Num", {"Num"}}, {"(", {"(", "E", ")"}}}}
        };
    }

    void parsear() {
        cout << "Pila \t\t Entrada \t\t Acción" << endl;
        imprimirEstado(); // Imprimir el estado inicial

        while (!pila.empty()) {
            string top = pila.top();
            pila.pop();

            Token tokenActual = entrada[index];

            if (esTerminal(top)) {
                if (top == tokenActual.lexema || top == Token::token_names[tokenActual.type]) {
                    cout << "Match: " << top << endl;
                    index++;
                } else {
                    cout << "Error de sintaxis: se esperaba " << top << endl;
                    return;
                }
            } else {
                string entradaActual = Token::token_names[tokenActual.type];
                if (LL1_Parsing_Table[top][entradaActual].size() > 0) {
                    vector<string> produccion = LL1_Parsing_Table[top][entradaActual];
                    cout << top << " -> ";
                    for (auto &p : produccion) cout << p << " ";
                    cout << endl;

                    // Apilar producción en orden inverso
                    for (auto it = produccion.rbegin(); it != produccion.rend(); ++it) {
                        if (*it != "") pila.push(*it);
                    }
                } else {
                    cout << "Error de sintaxis: no se encontró entrada para " << entradaActual << endl;
                    return;
                }
            }

            imprimirEstado(); // Imprimir el estado después de cada acción
        }
    }

private:
    stack<string> pila;
    vector<Token> entrada;
    int index;

    unordered_map<string, unordered_map<string, vector<string>>> LL1_Parsing_Table;

    bool esTerminal(const string& simbolo) {
        for (const char* terminal : Token::token_names)
            if (simbolo == terminal) return true;
        return false;
    }

    void imprimirEstado() {
        // Imprimir el contenido de la pila
        stack<string> pilaTmp = pila;
        string pilaStr;
        while (!pilaTmp.empty()) {
            pilaStr = pilaTmp.top() + " " + pilaStr;
            pilaTmp.pop();
        }

        // Imprimir la entrada restante
        string entradaStr;
        for (size_t i = index; i < entrada.size(); ++i) {
            entradaStr += entrada[i].lexema + " ";
        }

        // Imprimir estado actual
        cout << pilaStr << "\t\t" << entradaStr << "\t\tAcción realizada" << endl;
    }
};

int main() {
    string input = "x = 4 + 3; print(x + 1);";
    Scanner scanner(input);
    vector<Token> tokens;
    Token token;
    while ((token = scanner.nextToken()).type != Token::END) {
        tokens.push_back(token);
    }
    tokens.push_back(Token(Token::END, "$"));  // Añadir token de fin

    ParserLL1 parser(tokens);
    parser.parsear();

    return 0;
}





