#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>

using namespace std;

enum class TokenType {
    INTEGER,
    FLOAT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    IDENTIFIER,
    EOF_TOKEN
};

struct Token {
    TokenType type;
    string value;
};

class EtlInstance {
public:
    EtlInstance(const string& text) : text(text), position(0), current_token() {}

    Token get_next_token()
    {
        while (position < text.length() && isspace(text[position])) {
            position++;
        }

        if (position >= text.length()) {
            return { TokenType::EOF_TOKEN };
        }

        char current_char = text[position];

        if (isdigit(current_char) || current_char == '.') {
            string number_value;

            while (position < text.length() && (isdigit(text[position]) || text[position] == '.')) {
                number_value += text[position];
                position++;
            }

            if (number_value.find('.') != string::npos) {
                return { TokenType::FLOAT, number_value };
            } else {
                return { TokenType::INTEGER, number_value };
            }
        }

        // TODO: variables

        switch (current_char) {
        case '+':
            position++;
            return { TokenType::PLUS, "+" };
        case '-':
            position++;
            return { TokenType::MINUS, "-" };
        case '*':
            position++;
            return { TokenType::MULTIPLY, "*" };
        case '/':
            position++;
            return { TokenType::DIVIDE, "/" };
        case '%':
            position++;
            return { TokenType::MODULO, "%" };
        case '=':
            position++;
            return { TokenType::ASSIGN, "=" };
        default:
            // TODO: fancy exceptions handler
            cerr << "Syntax error" << endl;
            exit(1);
        }
    }

    void eat(TokenType expected_type)
    {
        if (current_token.type == expected_type) {
            current_token = get_next_token();
        }
    }

    float factor()
    {
        Token token = current_token;

        if (token.type == TokenType::INTEGER) {
            eat(TokenType::INTEGER);

            return stoi(token.value);
        } else if (token.type == TokenType::FLOAT) {
            eat(TokenType::FLOAT);

            return stof(token.value);
        } else if (token.type == TokenType::IDENTIFIER) {
            // TODO: variables
        }
    }

    float term()
    {
        float result = factor();

        while (current_token.type == TokenType::MULTIPLY || current_token.type == TokenType::DIVIDE || current_token.type == TokenType::MODULO) {
            Token op = current_token;

            if (op.type == TokenType::MULTIPLY) {
                eat(TokenType::MULTIPLY);
                result *= factor();
            } else if (op.type == TokenType::DIVIDE) {
                eat(TokenType::DIVIDE);
                float divisor = factor();
                if (divisor == 0.0f) {
                    cerr << "Division by zero!" << endl;
                    return NAN;
                }
                result /= divisor;
            } else if (op.type == TokenType::MODULO) {
                eat(TokenType::MODULO);
                float divisor = factor();
                int int_result = static_cast<int>(result);
                int int_divisor = static_cast<int>(divisor);
                if (int_divisor == 0) {
                    cerr << "Modulo by zero!" << endl;
                    return NAN;
                }
                result = static_cast<float>(int_result % int_divisor);
            }
        }

        return result;
    }

    auto expr()
    {
        current_token = get_next_token();
        float result = term();
        Token op;

        while (current_token.type == TokenType::PLUS || current_token.type == TokenType::MINUS) {
            op = current_token;

            if (op.type == TokenType::PLUS) {
                eat(TokenType::PLUS);
                result += term();
            } else if (op.type == TokenType::MINUS) {
                eat(TokenType::MINUS);
                result -= term();
            }
        }

        return result;
    }

    float eval()
    {
        return expr();
    }

private:
    string text;
    size_t position;
    Token current_token;
    map<string, float> variables_table;
};

int main(int argc, char* argv[])
{
    string input;
    float result;

    // TODO: read file with script

    while (true) {
        cout << "> ";
        getline(cin, input);
        EtlInstance etl(input);
        result = etl.eval();
        cout << result << endl;
    }
}