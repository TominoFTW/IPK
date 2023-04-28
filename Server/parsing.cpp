#include "parsing.hpp"

void stack_print_char(std::vector<std::string> stack){
    printf("stack: \n");
    for (int i = 0; i < int(stack.size()); i++) {
        std::cout << stack[i] << " ";
    }
    printf("\n");
}

std::vector<std::string> evaluate(std::vector<std::string>& stack) {
    std::vector<std::string> tmp_stack;
    while (stack.back() != std::string(1,'(')) {
        tmp_stack.push_back(stack.back()); // pop till '('
        stack.pop_back();
    }

    stack.pop_back(); // pop '('
    auto op = tmp_stack.back();
    tmp_stack.pop_back();

    int result = stoi(tmp_stack.back());
    tmp_stack.pop_back();

    if (op == std::string(1,'+')) {
        while (tmp_stack.size() > 0) {
            result = result + stoi(tmp_stack.back());
            tmp_stack.pop_back();
        }
        stack.push_back(std::to_string(result));
    }

    else if (op == std::string(1,'-')) {
        while (tmp_stack.size() > 0) {
            result -= stoi(tmp_stack.back());
            tmp_stack.pop_back();
        }
        stack.push_back(std::to_string(result));
    }

    else if (op == std::string(1,'*')) {
        while (tmp_stack.size() > 0) {
            result *= stoi(tmp_stack.back());
            tmp_stack.pop_back();
        }
        stack.push_back(std::to_string(result));
    }

    else if (op == std::string(1,'/')) {
        while (tmp_stack.size() > 0) {
            if (stoi(tmp_stack.back()) == 0) {
                stack.push_back("-3-(032)");
                return stack;
            }
            result = result / stoi(tmp_stack.back());
            tmp_stack.pop_back();
        }
        stack.push_back(std::to_string(result));
    }

    return stack;
}

std::vector<std::string> num(std::vector<std::string>& stack, std::vector<int>& num_stack) {
    if (!num_stack.empty()) {
        stack.push_back(std::string(num_stack.begin(), num_stack.end()));
        num_stack.clear();
    }

    return stack;
}

std::string parse_message(const std::string& mode, const std::string& message) {
    std::vector<int> tmp_num_stack;
    std::vector<std::string> stack;
    bool error_flag = false;
    bool op = false;
    bool sp = false;
    int bracket_count = 0;
    std::string str = message;

    if (mode == "tcp") {
        if (str.substr(0, 6) != "SOLVE ") {
            error_flag = true;
        }
        str = str.substr(6);
    }

    for (int i = 0; i < int(str.size()); i++) {
        if (!error_flag) {
            /* last char ONLY ')' is valid */
            if (i == int(str.size() - 1) && str[i] != ')') {
                error_flag = true;
            }
            /* opening bracket */
            if (str[i] == '(') {
                bracket_count++;
                op = false;
                sp = false;
                stack = num(stack, tmp_num_stack);
                std::vector<int> tmp_num_stack;
                stack.push_back(std::string (1,str[i]));

                if (str[i+1] != '+' && str[i+1] != '-' && str[i+1] != '*' && str[i+1] != '/') {
                    error_flag = true;
                }

            }
            /* closing bracket */
            else if (str[i] == ')') {
                bracket_count--;
                sp = false;
                stack = num(stack, tmp_num_stack);
                tmp_num_stack.clear();
                if (stack.back() == std::string (1,'(') or stack.size() < 4) {
                    error_flag = true;
                }
                else
                    stack = evaluate(stack);
            }

            /* operator */
            else if ((str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/') && !op) {
                op = true;
                sp = false;
                stack.push_back(std::string (1,str[i]));
            }

            /* SP */
            else if (str[i] == ' ' && !sp) {
                sp = true;
                stack = num(stack, tmp_num_stack);
                tmp_num_stack.clear();
            }

            /* number */
            else if (isdigit(str[i])) {
                sp = false;
                tmp_num_stack.push_back(int(str[i]));
            }
            else {
                error_flag = true;
            }
        }
        else {
            error_flag = true;
            break;
        }
    }

    if (!bracket_count and !error_flag and stoi(stack.back()) >= 0) {
        return stack.back();
    }
    else {
        return "Error: Invalid message format";
    }
}
