#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#pragma once

/**
 * @brief Prints the stack - DEBUG purposes
 * @param std::vector<std::string> stack
 * 
*/
void stack_print_char(std::vector<std::string> stack);

/**
 * @brief Evaluates the stack
 * @param std::vector<std::string> stack
 * @return std::vector<std::string> stack
*/
std::vector<std::string> evaluate(std::vector<std::string>& stack);

/**
 * @brief Multiple digits number
 * @param std::vector<std::string> stack
 * @param std::vector<int> num_stack
 * @return std::vector<std::string> stack
*/
std::vector<std::string> num(std::vector<std::string>& stack, std::vector<int>& num_stack);

/**
 * @brief Validates the message and processes it to evaluation
 * @param std::string mode
 * @param std::string message
*/
std::string parse_message(const std::string& mode, const std::string& message);
