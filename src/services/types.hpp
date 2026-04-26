//Отдельный файл для струткур
//Separate file for structures
#pragma once
#include <vector>
namespace ardor {
//Пишем, чтобы не писать std
//Using "using" to avoid typing std
using string = std::string;
    //Структура для токена
    //Token structure
    struct Token {
        int line_num;
        string word;
    };
    //Структура для идентифицированного токена
    //Identified token structure
    struct IdentifiedToken {
        string word;
        string type;
        string translate;
    };
    //Структура для классификаций
    //Classificatoin structure
    struct Classification {
        string word;
        string type;
        string translate;
    };
    //Структура для паттернов
    //Pattern structure
    struct Pattern {
        string line;
        string code;
    };
    //Структура для параметров в модуле
    //Module parameter structure
    struct Parameters {
        string word;
        string translate;
    };
    //Структура для модулей
    //Module structure
    struct Module {
        string name;
        std::vector<Parameters> params;
    };
}