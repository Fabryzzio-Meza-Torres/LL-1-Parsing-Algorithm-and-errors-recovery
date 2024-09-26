#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>  
#include <iomanip> 
#include <stdexcept>   

using namespace std;

class Token{
public:
    Token() = default;
    enum Type { ID, SEMICOLON, ASSIGN, PRINT, LPAREN, RPAREN, PLUS, MINUS, MULT, NUM, END, ERR };
    static const char* token_names[12];
    Type type;
    string lexema;
    explicit Token(Type type) : type(type) { }
    explicit Token(Type type, const string& lex) : type(type), lexema(lex) {}
};

const char* Token::token_names[12] = { "id", ";", "=", "print", "(", ")", "+", "-", "*", "Num", "$", "Err" };

ostream& operator<<(ostream& os, Token tokencito) {
    return os << Token::token_names[tokencito.type] << "(" << tokencito.lexema << ")";
}

struct Scanner {
private:
    string input;
    size_t pos;
    
public:
    Scanner(const string& uwu) : input(uwu), pos(0) {}
    Token next_Token() {
        while (pos < input.size() && isspace(input[pos])) pos++;
        if (pos >= input.size()) {
            return Token(Token::END);
        }

        char current = input[pos];
        if (isalpha(current)) {
            string id;
            while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
                id += input[pos++];
            }
            if (id == "print") {
                return Token(Token::PRINT, id);
            }
            return Token(Token::ID, id);
        }

        if (isdigit(current)) {
            string num;
            while (pos < input.size() && isdigit(input[pos])) {
                num += input[pos++];
            }
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
};

struct ParserLL1 {
private:
    stack<string> stack;
    vector<Token> input;
    int index;
    map<string, map<string, vector<string>>> LL1_Parsing_Table;

    bool isterminal(const string& symbol) {
        for (const char* terminal : Token::token_names) {
            if (symbol == terminal) {
                return true;
            }
        }
        return false;
    }

    void print_state(const string& action) {
        std::stack<string> stacktemp = this->stack;
        string stackstr;
        while (!stacktemp.empty()) {
            stackstr = stacktemp.top() + " " + stackstr;
            stacktemp.pop();
        }

        string inputstr;
        for (size_t i = index; i < input.size(); ++i) {
            inputstr += input[i].lexema + " ";
        }

        cout << setw(40)<< stackstr << setw(40) << inputstr << setw(40) << action << "\n";
    }

    void error_recovery(const string& top, Token tokenActual) {
        if(top=="$"){
            throw std::runtime_error("Cadena no aceptada!");
        }
        if(isterminal(top)){
            stack.pop();
            return;
        }
        if (tokenActual.type == Token::SEMICOLON || tokenActual.type == Token::RPAREN ||tokenActual.type == Token::END) {
            if(tokenActual.type == Token::END){
               throw std::runtime_error("Cadena no aceptada!");
            }
            index++; //Explorar
        }
        cout << "Error: inesperado " << tokenActual.lexema<< ", se esperaba " << top << endl;
        stack.pop(); //Extraer

    }

public:
    ParserLL1(const vector<Token>& tokens) : input(tokens), index(0) {
        initialize_LL1_Parsing_Table();
        stack.push("$");
        stack.push("P");
    }

void initialize_LL1_Parsing_Table() {
    LL1_Parsing_Table = {
        {"P", {
            {"id", {"SL"}},
            {"print", {"SL"}}
        }},
        {"SL", {
            {"id", {"S", "SL'"}},
            {"print", {"S", "SL'"}}
        }},
        {"SL'", {
            {";", {";","S", "SL'"}},
            {"$", {""}}
        }},
        {"S", {
            {"id", {"id", "=", "E"}},
            {"print", {"print", "E"}}
        }},
        {"E", {
            {"id", {"T", "E'"}},
            {"Num", {"T", "E'"}},
            {"(", {"T", "E'"}}
        }},
        {"E'", {
            {";", {""}},
            {")", {""}},
            {"+", {"+", "T", "E'"}},
            {"-", {"-", "T", "E'"}},
            {"$", {""}}
        }},
        {"T", {
            {"id", {"F", "T'"}},
            {"Num", {"F", "T'"}},
            {"(", {"F", "T'"}}
        }},
        {"T'", {
            {"*", {"*", "F", "T'"}},
            {";", {""}},
            {")", {""}},
            {"+", {""}},
            {"-", {""}},
            {"$", {""}}
        }},
        {"F", {
            {"id", {"id"}},
            {"Num", {"Num"}},
            {"(", {"(", "E", ")"}}
        }}
    };
}

    void parse() {
        cout << setw(40) << "Pila" << setw(40) << "Entrada" << setw(40) << "Acción" << endl;

        while (!stack.empty()) {
            string top = stack.top();

            Token tokenActual = input[index];

            if (isterminal(top)) {
                if (top == tokenActual.lexema || top == Token::token_names[tokenActual.type]) {
                    print_state("match " + top);
                    index++;
                    stack.pop();
                } else {
                    error_recovery(top, tokenActual);
                }
            } else {
                string entradaActual = Token::token_names[tokenActual.type];
                if (LL1_Parsing_Table[top].find(entradaActual) != LL1_Parsing_Table[top].end()) {
                    vector<string> produccion = LL1_Parsing_Table[top][entradaActual];
                    string action = top + " -> ";
                    for (const auto& p : produccion){
                        if(p==""){
                            action += "ε";
                            break;
                        }
                        action += p + " ";
                    } 
                    print_state(action);
                    stack.pop();

                    for (auto it = produccion.rbegin(); it != produccion.rend(); ++it) {
                        if (*it != "") stack.push(*it);
                    }
                } else {
                    error_recovery(top, tokenActual);
                }
            }
        }

        cout <<"Cadena aceptada!" << endl;
    }
};

int main() {
    string codigo = "x=(1+-*5-*3);print(x)";
    Scanner scanner(codigo);
    vector<Token> tokens;
    Token tokencito;
    while ((tokencito = scanner.next_Token()).type != Token::END) {
        tokens.push_back(tokencito);
    }
    tokens.push_back(Token(Token::END, "$"));

    ParserLL1 parser(tokens);
    parser.parse();

    return 0;
}
