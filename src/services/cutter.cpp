#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
//Подключаем кастомные библиотеки
//Connecting custom libraries
#include "cutter.h"
using namespace std;
//Для удобства использования, пишем: ifs = std::ifstream и ss = std::stringstream
//For ease of use, we write: ifs = std::ifstream and ss = std::stringstream
using ifs = std::ifstream;
using ss = std::stringstream;
//Прописываем логику для функции "token()""
//We write the logic for the "token()"" function
string token(string& line, int line_num, bool& active, const vector<char>& special_chars, const vector<char>& active_chars) {
    string word;
    //"size_t" лучше подходит для итерации
    //"size_t" is better for iteration
    size_t i = 0;
    //Читаем строку по символам
    //Reading a line character by character
    while(i < line.size()) {
        //Пропускаем пробелы
        //Skip spaces
        if(line[i] == ' ' && word.empty()) {
            i++;
            continue;
        }
        //Возвращаем слово как только оно заканчивается
        //We return the word as soon as it ends
        if(line[i] == ' ' && !active) {
            line.erase(0, i);
            return word;
        } else if(line[i] == ' ') {
            //Добавляем символ к слову если активен трекер
            //
            word += line[i];
            i++;
            continue;
        }
        //Все знаки ниже преобразуем в отдельный токен, если они не состоят в слове
        //We transform all the characters below into a separate token if they are not part of a word
        for(auto s_c : special_chars) {
            if(line[i] == s_c && word.empty()) {
                word += line[i];
                i++;
                line.erase(0, i);
                //Ведем учет активных символов
                //We keep track of active symbols
                for(auto a_c : active_chars) {
                    if(s_c == a_c) {
                        active = !active;
                    }
                }
                return word;
            } else if(line[i] == s_c) {
                //Проверяем активность кавычки
                //Checking the activity of the quotation mark
                if(active) {
                    for(auto a_c : active_chars) {
                        if(s_c == a_c) {
                            //Не увеличиваем значение 'i', чтобы при следующем вызове "token()"" он забрал активный символ в отдельный токен
                            //We don't increase the value of 'i' so that the next time token() is called, it will take the active character into a separate token.
                            line.erase(0, i); 
                            return word;
                        }
                    }
                    word += line[i];
                    i++;
                    continue;
                } else {
                    line.erase(0, i);
                    return word;
                }
            }
        }
        //Добавляем обычный символ
        //Adding a regular symbol
        word += line[i];
        i++;
    }
    line.clear();
    return word;
}
//Прописываем логику для функции "tokenize()""
//We write the logic for the "tokenize()"" function
vector<ardor::Token> tokenize(const string& filename, const vector<char>& special_chars, const vector<char>& active_chars) {
    //Создаем "vector" для хранения слов из файла и номера их строки
    //Create a "vector" to store words from a file and their line numbers
    vector<ardor::Token> tokens;
    //Читаем файл
    //Reading the file
    ifs readfile(filename);
    //Проверяем открылся ли файл
    //Checking if the file opened
    if(!readfile.is_open()) {
        //Выдаем ошибку если не открылся
        //We give an error if it doesn't open
        ss error;
        error << "ERROR: File don't open: " << filename;
        throw runtime_error(error.str());
    }
    string line;
    int line_num = 1;
    bool active = false;
    //Читаем текст файла построчно
    //Reading the file text line by line
    while(getline(readfile, line)) {
        string word;
        //Находим и забираем слова пока строка не кончится
        //Find and collect words until the line ends
        while(!line.empty()) {
            word = token(line, line_num, active, special_chars, active_chars);
            //Проверяем не пустое ли слово
            //Checking if a word is empty
            if(!word.empty()) {
                tokens.push_back({line_num, word});
            }
        }
        //Переходим на другую строку
        //Let's move to another line
        line_num++;
    }
    //Возвращаем "vector" со словами из файла
    //Returning a "vector" of words from a file
    return tokens;
}