#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <iomanip>
#include <algorithm>
using namespace std;

//Our arrays with terminals and non-terminals
const std::string terminal[]= {"id",";","=","print","(",")","+","-","*","Num"," $"};
const std::string non_terminals[]= {"P","SL","SL'","S","E","E'","T","T'","F"};

//LL1 Parsing Table for the given grammar
std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> LL1_Parsing_Table {
    {"P", {{"id", {"SL"}}, {"print", {"SL"}}}},
    {"SL", {{"id", {"S", "SL'"}}, {"print", {"S", "SL'"}}}},
    {"SL'", {{";",{"S", "SL'"}},{"$",{""}}}},
    {"S",{{"id",{"id","=","E"}},{"print",{"print","E"}}}},
    {"E",{{"id",{"T","E'"}},{"Num",{"T","E'"}},{"(",{"T","E'"}}}},
    {"E'",{{";",{""}},{")",{""}},{"+",{"+","T","E'"}},{"-",{"-","T","E'"}},{"$",{""}}}},
    {"T",{{"id",{"F","T'"}},{"Num",{"F","T'"}},{"(",{"F","T'"}}}},
    {"T'",{{"*",{"*","F","T'"}},{";",{""}},{")",{""}},{"+",{""}},{"-",{""}},{"$",{""}},{"*",{""}}}},
    {"F",{{"id",{"id"}},{"Num",{"Num"}},{"(",{"(","E",")"}}}}
};

//Function to check if the given string is a terminal
bool isterminal(const std::string& symbol){
    return symbol=="id"||symbol==";"||symbol=="="||symbol=="print"||symbol=="("||symbol==")"||symbol=="+"||symbol=="-"||symbol=="*"||symbol=="Num"||symbol=="$";
}

//function for print the stack,input and action
void imprimirTabla(const std::stack<std::string>& pila, const std::vector<std::string>& entrada, int indiceEntrada, const std::string& accion) {
    // Imprimir la pila
    std::stack<std::string> pilaCopia = pila;
    std::vector<std::string> pilaElementos;
    while (!pilaCopia.empty()) {
        pilaElementos.push_back(pilaCopia.top());
        pilaCopia.pop();
    }
    reverse(pilaElementos.begin(), pilaElementos.end());
    for (const auto& elem : pilaElementos) {
        cout << elem;
    }
    cout << setw(15 - pilaElementos.size());  // Ajuste de formato

    // Imprimir la entrada
    for (int i = indiceEntrada; i < entrada.size(); i++) {
        cout << entrada[i];
    }
    cout << setw(15 - (entrada.size() - indiceEntrada));  // Ajuste de formato

    // Imprimir la acción
    cout << accion << endl;
}


