#pragma once
#include <vector>
#include <string>
#include "types.hpp"
/*
Эта функция будет находить слова в данной ей строке, находить слова и забирать их из строки, при этом удаляя слово из строки для дальнейшей такой же манипуляции
Используем передачу одной строки из файла по ссылке, чтобы изменять оригинальную строку

This function will find words in a given string, find words and select them from strings, while removing the word from the string for further similar manipulations
We use passing a single string from a file by reference to modify the original string 
*/
std::string token(std::string& line, int line_num, bool& active, const std::vector<char>& special_chars, const std::vector<char>& active_chars);
/*
Функция, которая будет собирать слова в "vector" для дальнейших манипуляций
Используем передачу имени файла по константной ссылке (const string&) для чтения

A function that will collect words into a "vector" for further manipulation
We use the constant reference (const string&) for reading the file name 
*/
std::vector<ardor::Token> tokenize(const std::string& filename, const std::vector<char>& special_chars, const std::vector<char>& active_chars);