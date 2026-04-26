#pragma once
#include <vector>
#include <string>
#include "types.hpp"
//Функция для чтения отдельных модулей языка программирования
//A function for reading individual modules of a programming language
std::vector<ardor::Module> modules();
//Функция для идентификации токенов
//Token identification function
std::vector<ardor::IdentifiedToken> classifier(const std::vector<ardor::Classification>& classific, std::vector<ardor::Token>& tokens, int& line_num);
//Функция для нахождения триггер-слов в модулях
//Function for finding trigger words in modules
std::vector<std::pair<std::string, std::string>> trigger(const std::vector<ardor::Module>& modules);
//Функция для перевода строки, если в ней используется логика из модулей
//Function for translating a string if it uses logic from modules
void translater(std::vector<ardor::IdentifiedToken>& tokens, const std::vector<std::pair<std::string, std::string>>& triggers, const std::vector<ardor::Module>& modules);
//Функция для генерации паттерна
//Function for generating a pattern
std::string patterner(std::vector<ardor::IdentifiedToken>& tokens, const std::vector<ardor::Pattern>& patterns, int& line_num);
//
//
void recursion(std::string& code, char& rec, const std::vector<ardor::Classification>& classific);
/*
Функция для генерации кода на другом языке программирования
Передаем все нужные "vector"
Функция "void" потому, что ничего не возвращает

A function for generating code in another programming language
We pass all the necessary "vectors"
The function is "void" because it returns nothing
*/
void generate(const std::vector<ardor::Classification>& classific, const std::vector<ardor::Pattern>& patterns, std::vector<ardor::Token>& tokens, std::string filename);