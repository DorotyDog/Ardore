#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
//Подключаем пользовательские библиотеки
//Connecting user libraries
#include "emmiter.h"
using namespace std;
namespace fs = std::filesystem;
using ifs = std::ifstream;
using ofs = std::ofstream;
using ss = std::stringstream;
//Прописываем функцию
//We write the function
vector<ardor::Module> modules() {
    //Создаем переменную "line" и vector "module_name", для временного хранения имени файлов внутри "modules.txt", и "module" для полноценного хранения
    //We create a variable "line" and a vector "module_name" for temporary storage of file names inside "modules.txt", and "module" for full storage
    string line;
    vector<string> module_name;
    vector<ardor::Module> module;
    //Проверяем существование файла "modules.txt"
    //Checking the existence of the "modules.txt" file
    if(!fs::exists("syntax/modules.txt")) {
        throw runtime_error("SYSTEM ERROR: System file \"modules.txt\" not found");
    }
    //Проверяем на пустой файл
    //Checking for an empty file
    if(fs::file_size("syntax/modules.txt") == 0) { return module; }
    //Открываем файл
    //Open the file
    ifs mod("syntax/modules.txt");
    //Проверяем открылся ли файл
    //Checking if the file opened
    if(!mod.is_open()) {
        throw runtime_error("SYSTEM ERROR: System file \"modules.txt\" not opened");
    }
    //Добавляем имена файлов
    //Adding file names
    while(getline(mod, line)) {
        module_name.push_back(line);
    }
    line.clear();
    int i = 0;
    for(auto mod_n : module_name) {
        if(!fs::exists("syntax/modules/" + mod_n)) {
            //Отправляем ошибку если файла не существует
            //We send an error if the file does not exist
            ss error;
            error << "SYSTEM ERROR: Syntax file \"" << mod_n << "\" not found";
            throw runtime_error(error.str());
        }
        //Пропускаем этот файл, если он пустой
        //Skip this file if it is empty
        if(fs::file_size("syntax/modules/" + mod_n) == 0) {
            continue;
        }
        //Открываем файл
        //Open the file
        ifs m("syntax/modules/" + mod_n);
        if(!m.is_open()) {
            ss error;
            error << "SYSTEM ERROR: System file \"" << mod_n << "\" not opened";
            throw runtime_error(error.str());
        }
        ss name;
        name << "module" << i;
        //Создаем объект типа Module
        //Creating an object of type Module
        ardor::Module mod;
        mod.name = name.str();
        //Создаем прямую ссылку на второй vector
        //Create a direct link to the second vector
        auto& parameters = mod.params;
        while(getline(m, line)) {
            //Создаем необходимые переменные
            //Create the necessary variables
            string word;
            string translate;
            bool caretaker = false;
            //Читаем строку посимвольно
            //We read the line character by character
            for(auto l : line) {
                //Пропускаем ненужные пробелы
                //Skipping unnecessary spaces
                if(l == ' ' && word.empty()) {
                    continue;
                } else if(l == ' ' && translate.empty()) {
                    continue;
                }
                //Определяем разделитель
                //Defining the separator
                if(l == ':' && l != line[0] && !caretaker) { 
                    caretaker = !caretaker; 
                    continue;
                }
                //Отправляем символ в нужную строку
                //We send the symbol to the desired line
                if(caretaker) {
                    translate += l;
                    continue;
                }
                word += l;
            }
            //Добавляем во вложенный vector
            //Add to nested vector
            parameters.push_back({word, translate});
        }
        i++;
        module.push_back(mod);
    }
    //Возвращаем созданный vector
    //Return the created vector
    return module;
}
vector<pair<string, string>> trigger(const vector<ardor::Module>& modules) {
    //Создаем вектора
    //Creating vectors
    vector<pair<string, string>> trig;
    //Перечисляем вектор с модулями
    //Iterating over the vector of modules
    for(auto mod : modules) {
        //Перечисляем вектор с параметрами модуля
        //Iterating over the vector of module parameters
        for(auto [word, translate] : mod.params) {
            if(translate == "trigger") {
                //Добавляем слово в вектор, если оно триггер-слово
                //Adding a word to the vector if it is a trigger word
                trig.push_back({word, mod.name});
            }
        }
    }
    return trig;
}
vector<ardor::IdentifiedToken> classifier(const vector<ardor::Classification>& classific, vector<ardor::Token>& tokens, int& line_num) {
    //Создаем вектор для хранения идентифицированных токенов
    //Creating a vector to store identified tokens
    vector<ardor::IdentifiedToken> Itokens;
    //Используем полу ручную итерацию для удаления идентифицированного токена
    //Using a semi-manual iteration to remove the identified token
    for(auto t = tokens.begin(); t != tokens.end();) {
        bool caretaker = false;
        //Проверяем строку, если больше заданной, то останавливаем цикл
        //Checking the string; if it exceeds the specified length, we stop the loop
        if(t->line_num > line_num) break;
        for(const auto& cl : classific) {
            //Если находим слово, то вставляем его и помечаем, что нашли
            //If we find a word, we insert it and mark it as found
            if(t->word == cl.word) {
                Itokens.push_back({t->word, cl.type, cl.translate});
                caretaker = true;
                break;
            }
        }
        //Проверяем, если не нашли, то вставляем его в вектор под типом WORD
        // If not found, we insert it into the vector as type WORD
        if(caretaker) {
            t = tokens.erase(t);
        } else {
            Itokens.push_back({t->word, "WORD", t->word});
            t = tokens.erase(t);
        }
    }
    //Возвращаем наш вектор
    //Returning our vector
    return Itokens;

}
void translater(vector<ardor::IdentifiedToken>& tokens, const vector<pair<string, string>>& triggers, const vector<ardor::Module>& modules) {
    //Создаем нужные переменные
    //Creating the necessary variables
    bool caretaker = false;
    string mod_name;
    //Перечисляем вектор с триггерами
    //Iterating over the vector of triggers
    for(const auto& [trig, module] : triggers) {
        //Перечисляем вектор с токенами
        //Iterating over the vector of tokens
        for(auto& t : tokens) {
            //Если находим помечаем и копируем имя модуля
            //If found, we mark and copy the module name
            if(t.type == trig) {
                caretaker = true;
                mod_name = module;
                break;
            }
        }
        //Выходим из цикла если нашли совпадение
        //Exiting the loop if a match is found
        if(caretaker) break;
    }
    if(caretaker) {
        //Перечисляем модули чтобы найти сохраненное имя
        //Iterating over the modules to find the saved name
        for(const auto& mod : modules) {
            if(mod_name == mod.name) {
                //Находим и перечисляем параметры этого модуля
                //Finding and iterating over the parameters of this module
                for(const auto& [word, translate] : mod.params) {
                    //Перечисляем токены, чтобы найти совпадения
                    //Iterating over the tokens to find matches
                    for(auto& t : tokens) {
                            //Присваиваем новые значения, если есть сходства
                            //Assigning new values if there are similarities
                        if(t.word == word && translate != "trigger") {
                            t.word = translate;
                            t.translate = translate;
                        }
                    }
                }
            }
        }
    }
}
string patterner(vector<ardor::IdentifiedToken>& tokens, const vector<ardor::Pattern>& patterns, int& line_num) {
    //Создаем нужные переменные
    //Create the necessary variables
    string pattern;
    string line_code;
    string code;
    bool coincidence = false;
    //Перечисляем вектор токенов и формируем паттерн
    //Iterate over the token vector and form a pattern
    for(const auto& t : tokens) {
        pattern += t.type + " ";
    }
    //Стираем последний пробел
    //Remove the last space
    pattern.pop_back();
    //Перечисляем все паттерны и находим совпадения
    //Iterate over all patterns and find matches
    for(const auto& [line, code] : patterns) {
        //Отмечаем совпадение и копируем код
        //Mark the match and copy the code
        if(pattern == line) {
            coincidence = true;
            line_code = code;
        }
    }
    //Проверяем нашли ли совпадение, если нет то выводим ошибку
    //Check if a match was found; if not, output an error
    if(!coincidence) {
        ss error;
        cout << pattern << '\n';
        error << "ERROR: Error in a line " << line_num;
        throw runtime_error(error.str());
    }
    //Читаем строку посимвольно
    //Read the string character by character
    for(size_t i = 0; i < line_code.size(); i++) {
        //Добвавляем перевод строки
        //Adding a line break
        if(line_code[i] == '\\' && line_code[i+1] == 'n') {
            code += '\n';
            i++;
            continue;
        }
        //Если находим спецсимвол подставляем вместо него слово
        //If a special character is found, substitute a word in its place
        if(line_code[i] == '%') {
            //Определяем номер подставляемого слова
            //Determine the index/number of the substituted word
            char index = line_code[i+1];
            int num = index - '0';
            int j = 0;
            //Перечисляем вектор токенов
            //Iterate over the token vector
            for(const auto& t : tokens) {
                if(j == num) {
                    //Добавляем целое слово, если номер совпадает
                    //Add the whole word if the number matches
                    code += t.word;
                    break;
                }
                j++;
            }
            i++;
        } else {
            code += line_code[i];
        }
    }
    return code;
}
//Прописываем функцию
//We write the function
void generate(const vector<ardor::Classification>& classific, const vector<ardor::Pattern>& patterns, vector<ardor::Token>& tokens, string filename) {
    //Загружаем модули в вектор
    //Load modules into a vector
    auto loaded_modules = modules();
    int line_num = 1;
    //Загружаем триггер слова в вектор
    //Load trigger words into a vector
    vector<pair<string, string>> triggers = trigger(loaded_modules);
    string extension;
    string name;
    //Читаем файл с расширением
    //Read a file with an extension
    ifs ext("syntax/extension.txt");
    getline(ext, extension);
    name = "data/" + filename;
    while(name.back() != '.') {
        name.pop_back();
    }
    name += extension;
    //Создаем или перезаписываем файл с кодом
    //Create or overwrite a code file
    ofs file_code(name, ios::trunc);
    if(!file_code.is_open()) {
        ss error;
        error << "SYSTEM ERROR: File \"" << name << "\" not open";
        throw runtime_error(error.str());
    }
    //Повторяем пока вектор tokens не пустой
    //Repeat while the tokens vector is not empty
    while(!tokens.empty()) {
        string code;
        vector<ardor::IdentifiedToken> Itokens = classifier(classific, tokens, line_num);
        translater(Itokens, triggers, loaded_modules);
        code = patterner(Itokens, patterns, line_num);
        //Добавляем строчку кода
        //Add a line of code
        file_code << code << "\n";
        line_num++;
    }
}