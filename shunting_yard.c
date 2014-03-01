//
// shunting yard implementation
//

#include <mystd.h>
#include <stack.h>

// isspace
#include <ctype.h>

// pow
#include <math.h>

#define LEFT 1
#define RIGHT 2

bool is_operator (char ch)
{
    if (ch == '+' ||
        ch == '-' ||
        ch == '*' ||
        ch == '/' ||
        ch == '^'
        ) {
        return true;
    } else {
        return false;
    }
}

bool is_operator_str (char *token)
{
    if (strlen(token) == 1 && is_operator(token[0])) {
        return true;
    } else {
        return false;
    }
}

int op_precedence (char ch)
{
    if (ch == '+' || ch == '-') {
        return 2;
    } else if (ch == '*' || ch == '/') {
        return 3;
    } else if (ch == '^') {
        return 4;
    } else {
        die("Programmer error: op_precedence got invalid operator [%c]", ch);
    }
  
    return 0;
}

int op_associativity (char ch)
{
    if (ch == '+' || ch == '-') {
        return LEFT;
    } else if (ch == '*' || ch == '/') {
        return LEFT;
    } else if (ch == '^') {
        return RIGHT;
    } else {
        die("Programmer error: op_associativity got invalid operator [%c]", ch);
    }

    return 0;
}

void shunting_yard(char *expression, stack *out)
{
    int expression_length = strlen(expression);
    if (expression_length < 1) {
        die("Programmer error: shunting_yard expects non-empty expression");
    }

    stack in_tokenized;
    stack op_stack;
    stack_create(&in_tokenized, "input tokenized", out->max_size);
    stack_create(&op_stack, "operator stack", out->max_size);

    char previous_token[STRUCT_NAME_LENGTH] = "";
    char token[STRUCT_NAME_LENGTH] = "";
    
    char current_char_str[2];
    char current_char;
    int i=0;

    // split string into array (tokenize) -- actually I use stack instead of array,
    // which implies reversing it before I can access elements sequentially 
    CHAR: while(i < expression_length) {
        current_char = expression[i];

        strncpy(current_char_str, &expression[i], 1); 
        current_char_str[1] = 0;      

        if (isspace(current_char)) {
            i++;
            goto CHAR;
        }

        if (strlen(previous_token) > 0 && (current_char == '+' || current_char == '-')) {
            strncat(token, &current_char, 1);
            previous_token[0] = 0;
        } else if (
            is_operator(current_char) == true ||
            current_char == '(' ||
            current_char == ')'
            ) {
            if (strlen(token) > 0) {
                stack_push(&in_tokenized, token);
            }
            token[0] = 0;

            stack_push(&in_tokenized, &current_char_str[0]);
            strcpy(previous_token, current_char_str);
        } else {
            strncat(token, &current_char, 1);
            previous_token[0] = 0;
        }

        i++;
    }
    if (strlen(token) > 0) {
        stack_push(&in_tokenized, token);
    }

    stack in_tokenized_reversed;
    stack_create(&in_tokenized_reversed, "input tokenized, reversed", in_tokenized.max_size);
    char *tmp_token;

    // reverse input stack, convert array from infix to rpn
    while (stack_is_empty(&in_tokenized) == false) {
        tmp_token = stack_pop(&in_tokenized);
        stack_push(&in_tokenized_reversed, tmp_token);
    }
    stack_free(&in_tokenized);

    TOKEN: while (stack_is_empty(&in_tokenized_reversed) == false) {
        tmp_token = stack_pop(&in_tokenized_reversed);
//        printf("%s\n", tmp_token);

        if (is_operator_str(tmp_token)) {
            bool break_CHECK_OP_STACK_1 = false;
            CHECK_OP_STACK_1: while (stack_is_empty(&op_stack) == false && break_CHECK_OP_STACK_1 == false) {
                char *tmp_top_op = stack_top(&op_stack);

                if (is_operator_str(tmp_top_op) && 
                    (
                        (op_precedence(tmp_token[0]) == op_precedence(tmp_top_op[0]) && op_associativity(tmp_token[0]) == LEFT) ||
                        op_precedence(tmp_token[0]) < op_precedence(tmp_top_op[0])
                    )
                    ) {
                    
                    stack_push(out, stack_pop(&op_stack));
                } else {
                    break_CHECK_OP_STACK_1 = true;
                    goto CHECK_OP_STACK_1;
                }
            }

            stack_push(&op_stack, tmp_token);
        } else if (tmp_token[0] == '(') {
            stack_push(&op_stack, tmp_token);
        } else if (tmp_token[0] == ')') {
            bool found_matching_paren = false;
            bool break_CHECK_OP_STACK_2 = false;
            CHECK_OP_STACK_2: while (stack_is_empty(&op_stack) == false && break_CHECK_OP_STACK_2 == false) {
                char *tmp_top_op = stack_top(&op_stack);

                if (tmp_top_op[0] != '(') {
                    stack_push(out, stack_pop(&op_stack));
                } else {
                    found_matching_paren = true;
                    stack_pop(&op_stack);
                    break_CHECK_OP_STACK_2 = true;
                    goto CHECK_OP_STACK_2;
                }
            }
            if (found_matching_paren == false) {
                die("Programmer error: shunting_yard got invalid expression, unbalanced parenthesis: [%s]", expression);
            }
        } else {
            stack_push(out, tmp_token);
        }

        goto TOKEN;
    }
    while (stack_is_empty(&op_stack) == false) {
        stack_push(out, stack_pop(&op_stack));
    }

    stack_free(&op_stack);
    stack_free(&in_tokenized_reversed);
}

float calc_rpn(stack *rpn)
{
    stack rpn_reversed;
    stack calc;

    stack_create(&rpn_reversed, "rpn reversed", rpn->max_size);
    stack_create(&calc, "calculator", rpn->max_size);

    while(stack_is_empty(rpn) == false) {
        stack_push(&rpn_reversed, stack_pop(rpn));
    }

    char *token;
    while (stack_is_empty(&rpn_reversed) == false) {
        token = stack_pop(&rpn_reversed);
        stack_push(rpn, token);

        if (is_operator_str(token)) {
            char *op1 = stack_pop(&calc);
            char *op2 = stack_pop(&calc);
        
            errno = 0;
            float op1_f = strtod(op1, (char **) NULL);
            if (errno != 0) {
                die_explaining_errno("not a number [%s] while calculating rpn", op1);
            }
            
            errno = 0;
            float op2_f = strtod(op2, (char **) NULL);
            if (errno != 0) {
                die_explaining_errno("not a number [%s] while calculating rpn", op2);
            }

            if (strcmp(token, "+") == 0) {
                token = (char *) malloc(MAX_MSG_SIZE);
                sprintf(token, "%f", op2_f + op1_f);
            } else if (strcmp(token, "-") == 0) {
                token = (char *) malloc(MAX_MSG_SIZE);
                sprintf(token, "%f", op2_f - op1_f);
            } else if (strcmp(token, "*") == 0) {
                token = (char *) malloc(MAX_MSG_SIZE);
                sprintf(token, "%f", op2_f * op1_f);
            } else if (strcmp(token, "/") == 0) {
                token = (char *) malloc(MAX_MSG_SIZE);
                sprintf(token, "%f", op2_f / op1_f);
            } else if (strcmp(token, "^") == 0) {
                token = (char *) malloc(MAX_MSG_SIZE);
                sprintf(token, "%f", pow(op2_f, op1_f));
            } else {
                die("Programmer error: operator [%s] is not implemented", token);
            }

            stack_push(&calc, token);
            free(token);
        } else {
            stack_push(&calc, token);
        }
    }

    token = stack_pop(&calc);
    errno = 0;
    float result = strtod(token, (char **) NULL);
    if (errno != 0) {
        die_explaining_errno("not a number [%s] while evaluating result", token);
    }
    
    stack_free(&rpn_reversed);
    stack_free(&calc);
    
    return result;
}

bool shunting_yard_test()
{
    struct struct_test {
        char *expression;
        float value;
    } tests[] = {
        { "2+3", 5 },
        { "2+3*5-12", 5 },
        { "3+4*2/(1-5)", 1 },
        { "3+4*2/(1-5)^2^3", 3+1.0/8192 },
        { "1+-1", 0 },
        { "3+2*5-2/2", 12 },
    };

    bool failed = false;
    stack rpn;
    float f;

    int n_tests = sizeof(tests) / sizeof(struct struct_test);

    int i;
    for (i=0; i<n_tests; i++) {
        stack_create(&rpn, "RPN", 100);
        shunting_yard(tests[i].expression, &rpn);
        f = calc_rpn(&rpn);

        if (f == tests[i].value) {
            printf("test %d passed: %s == %f\n", i+1, tests[i].expression, tests[i].value);
        } else {
            printf("test %d failed: %s: got %f, expected %f\n", i+1, tests[i].expression, f, tests[i].value);
            failed = true;
        }
        
        stack_free(&rpn);
    }

    if (failed == true) {
        return false;
    } else {
      return true;
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("test", argv[1]) == 0) {
        if (shunting_yard_test() == true) {
            printf("all tests are ok.\n");
        }

        return 0;
    } else {
        char *expression;
        
        int arg_size = 0;
        int i;
        for (i=1; i<argc; i++) {
            arg_size = arg_size + strlen(argv[i]);
        }
        
        expression = (char *) malloc(arg_size + 1);
        for (i=1; i<argc; i++) {
            strcat(expression, argv[i]);
        }

        stack rpn;
        stack_create(&rpn, "RPN", 100);
        shunting_yard(expression, &rpn);
        float f = calc_rpn(&rpn);
        stack_dump(&rpn);

        printf("expression: %s\n", expression);
        printf("value: %f\n", f);
    }

    return 0;
}
