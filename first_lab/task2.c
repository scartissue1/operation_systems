#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

typedef struct User {
    char * login;
    int password;
    int logged;
    int command_limit;
} User;

User * user_init(char * login, int password) {
    User * new_user = (User *)malloc(sizeof(User));
    if (!new_user) return NULL;
    new_user->login = login;
    new_user->password = password;
    new_user->logged = 0;
    new_user->command_limit = -1;
    return new_user;
}

int password_validation (int password) {
    while (password) {
        if (!isdigit(password % 10 + '0')) return 0;
        password /= 10;
    }
    return 1;
}

int login_validation (char * login) {
    int size = strlen(login);
    if (size > 6) return 0;
    int index = 0;
    while (index < size) {
        if (!isdigit(login[index]) && !isalpha(login[index])) return 0;
        index++;
    }
    return 1;
}

enum register_status_codes {
    reg_ok,
    reg_registered,
    reg_wrong_password,
    reg_wrong_login,
    reg_no_memory
};

enum register_status_codes _register(char * new_login, int new_password, 
    User ** user_array, int * user_array_size) {
    if (!login_validation(new_login)) return reg_wrong_login;
    if (*user_array_size > 1) {
        for (int i = 1; i < (*user_array_size); i++) {
            if (strcmp(user_array[i]->login, new_login) == 0) return reg_registered;
        }
    }
    if (!(0 <= new_password && new_password <= 1000000) || !password_validation(new_password)) return reg_wrong_password;
    User * new_user = user_init(new_login, new_password);
    if (!new_user) return reg_no_memory;
    (*user_array_size)++;
    User ** tmp = (User **)realloc(user_array, (*user_array_size));
    if (!tmp) {
        for (int i = 0; i < (*user_array_size); i++) {
            free(user_array[i]->login);
            free(user_array[i]);
        }
        return reg_no_memory;
    }
    user_array = tmp;
    user_array[(*user_array_size) - 1] = new_user;
    return reg_ok;
}

enum login_status_codes {
    log_ok,
    log_wrong_data,
    log_logged
};

enum login_status_codes _login(const char * income_login, const int income_password, 
    User ** user_array, const int user_array_size, int * curr_index) {
    if (user_array_size > 1) {
        for (int i = 1; i < user_array_size; i++) {
            if ((strcmp(user_array[i]->login, income_login) == 0) && (user_array[i]->password == income_password)) {
                if (!user_array[i]->logged) {
                    user_array[i]->logged = 1;
                    *curr_index = i;
                    return log_ok;
                }
                else return log_logged;
            }
        }
        return log_wrong_data;
    }
}

enum string_status_codes {
    str_ok,
    str_no_memory,
    str_wrong_data
};

enum string_status_codes get_a_string(FILE * in, char ** string) {
    if (!in) return str_wrong_data;
    int index = 0;
    int string_size = 1;
    *string = (char *)malloc(sizeof(char) * (string_size));
    if (!(*string)) return str_no_memory;

    char input = fgetc(in);
    while (input != '\n' && input != '\t' && input != ' ' && input != EOF) {
        string_size++;
        char * tmp = (char *)realloc((*string), sizeof(char) * (string_size));
        if (!tmp) {
            free(*string);
            *string = NULL;
            return str_no_memory;
        }
        *string = tmp;
        (*string)[index] = input;
        index++;
        input = fgetc(in);

    }
    (*string)[index] = '\0';
    return str_ok;

}

int date_validity(char * date) {
    int length = strlen(date);
    if (length != 11) return 0;
    if (!isdigit(date[0]) || !isdigit(date[1]) ||
    !isdigit(date[3]) || !isdigit(date[4]) ||
    !isdigit(date[6]) || !isdigit(date[7]) || !isdigit(date[8]) || !isdigit(date[9])) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;
    return 1;
}

long long int diff_in_sec(int date_day, int date_month, int date_year, int curr_date_day, int curr_date_month, int curr_date_year) {
    return fabs(curr_date_year - date_year) * 3.156e7 + 
    fabs(curr_date_month - date_month) * 2.628e6 + fabs(curr_date_day - date_day) * 86400;
}

long long int diff_in_min(int date_day, int date_month, int date_year, int curr_date_day, int curr_date_month, int curr_date_year) {
    return fabs(curr_date_year - date_year) * 525600 + 
    fabs(curr_date_month - date_month) * 43800 + fabs(curr_date_day - date_day) * 1440;
}

int diff_in_hour(int date_day, int date_month, int date_year, int curr_date_day, int curr_date_month, int curr_date_year) {
    return fabs(curr_date_year - date_year) * 8760 + 
    fabs(curr_date_month - date_month) * 730 + fabs(curr_date_day - date_day) * 24;
}

double diff_in_year(int date_year, int curr_date_year, int date_month, int curr_date_month, int date_day, int curr_date_day) {
    return fabs(curr_date_year - date_year) + (curr_date_month - date_month) / 12.0 + (curr_date_day - date_day) / 365.0;
}
enum command_status_codes {
    com_ok,
    com_no_memory,
    com_command_limit,
    com_error
};

enum command_status_codes get_command(int curr_index, User ** user_array, int user_array_size) {
    char * command = NULL;
    int command_count = 1;
    while (1) {
        enum string_status_codes comm = get_a_string(stdin, &command);
        if (command_count == user_array[curr_index]->command_limit) return com_command_limit;
        time_t now;
        time(&now);
        struct tm * local = localtime(&now);
        if (strcmp(command, "Time") == 0) {
            printf("Current time is %d:%d:%d UTC+3\n", local->tm_hour, local->tm_min, local->tm_sec);
        }
        else if (strcmp(command, "Date") == 0) {
            printf("Date is: %02d/%02d/%d\n", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
        }
        else if (strcmp(command, "Sanctions") == 0) {
            char * login = NULL;
            char * limit = NULL;
            enum string_status_codes log = get_a_string(stdin, &login);
            if (log == str_no_memory) return com_no_memory;
            else if (log == str_wrong_data) com_error;
            enum string_status_codes lim = get_a_string(stdin, &limit);
            if (lim == str_no_memory) {
                free(command);
                free(login);
                return com_no_memory;
            }
            else if (lim == str_wrong_data) {
                free(command);
                free(login);
                free(limit);
                return com_error;
            }
            int lim_int = atoi(limit);
            printf("Password:\n");
            char * password = NULL;
            enum string_status_codes pass = get_a_string(stdin, &password);
            if (pass == str_no_memory) {
                free(command);
                free(login);
                free(limit);
                return com_no_memory;
            }
            else if (pass == str_wrong_data) {
                free(command);
                free(login);
                free(limit);
                free(password);
                return com_error;
            }
            int pass_int = atoi(password);
            if (pass_int != 12345) return com_error;
            int flag = 0;
            for (int i = 1; i < user_array_size; i++) {
                if (strcmp(login, user_array[i]->login) == 0) {
                    user_array[i]->command_limit = lim_int;
                    flag = 1;
                    printf("Success\n");
                    break;
                }
            }
            if (!flag) printf("Cannot find such login\n");
            free(password);
            free(login);
            free(limit);
        }
        else if (strcmp(command, "Logout") == 0) {
            user_array[curr_index]->logged = 0;
            break;
        }
        else if (strcmp(command, "Howmuch") == 0) {
            char * date;
            enum string_status_codes date_status = get_a_string(stdin, &date);
            if (date_status == str_no_memory) return com_no_memory;
            else if (date_status == str_wrong_data) return com_error;
            if (!date_validity) {
                printf("Wrong date format\n");
            }
            char * flag;
            enum string_status_codes flag_status = get_a_string(stdin, &flag);
            if (flag_status == str_no_memory) {
                free(date);
                return com_no_memory;
            }
            else if (flag_status == str_wrong_data) {
                free(date);
                free(flag);
                return com_error;
            }
            int date_day_int = (date[0] - '0') * 10 + (date[1] - '0');
            int date_month_int = (date[3] - '0') * 10 + (date[4] - '0');
            int date_year_int = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
            int curr_date_day = local->tm_mday, curr_date_month = local->tm_mon + 1, curr_date_year = local->tm_year + 1900;
            if (strcmp(flag, "-s") == 0) {
                printf("Between %s and %02d/%02d/%d is %lld seconds\n", date, curr_date_day, curr_date_month, curr_date_year, diff_in_sec(date_day_int, date_month_int, date_year_int, curr_date_day, curr_date_month, curr_date_year));
            }
            else if (strcmp(flag, "-m") == 0) {
                printf("Between %s and %02d/%02d/%d is %lld minuts\n", date, curr_date_day, curr_date_month, curr_date_year, diff_in_min(date_day_int, date_month_int, date_year_int, curr_date_day, curr_date_month, curr_date_year));
            }
            else if (strcmp(flag, "-h") == 0) {
                printf("Between %s and %02d/%02d/%d is %d hours\n", date, curr_date_day, curr_date_month, curr_date_year, diff_in_hour(date_day_int, date_month_int, date_year_int, curr_date_day, curr_date_month, curr_date_year));
            }
            else if (strcmp(flag, "-y") == 0) {
                printf("Between %s and %02d/%02d/%d is %lf years\n", date, curr_date_day, curr_date_month, curr_date_year, diff_in_year(date_year_int, curr_date_year, date_month_int, curr_date_month, date_day_int, curr_date_day));
            }
            else printf("Wrong flag\n");
            free(date);
            free(flag);

        }
        else printf("Wrong command\n");
        command_count++;
    }
    free(command);
    return com_ok;
}

void clear_user_array(User ** user_array, int user_array_size) {
    for (int i = 1; i < user_array_size; i++) {
        //free(user_array[i]->login);
        free(user_array[i]);
    }
}

void print_login_menu() {
    printf("__________________________\n");
    printf("Type Register <login> <password> to register new account\n");
    printf("Type Login <login> <password> to enter already existing account\n");
    printf("Type Exit to exit from programm\n");
    printf("__________________________\n");
}

void print_flag_menu() {
    printf("__________________________\n");
    printf("Next flags are available:\n");
    printf("Time - current UTC+3 time\n");
    printf("Date - current date\n");
    printf("Howmuch <time> <flag> - shows how much time between current date and <time>\nTime should be in dd/mm/yyyy format\n-s - difference in seconds\n-m - difference in minuts\n-h - difference in hours\n-y - difference in full years\n");
    printf("Logout - logout from account\n");
    printf("Sanctions <login> <number> - make a limit for user with that login for <number> operations.\n");
    printf("__________________________\n");
}

int main(int argc, char * argv[]) {
    print_login_menu();
    char * string = NULL;
    User ** user_array = (User **)malloc(sizeof(User *));
    
    int user_array_size = 1;
    while (1) {
        enum string_status_codes status = get_a_string(stdin, &string);
        if (status == str_ok) {
            if (strcmp(string, "Exit") != 0) {
                if (strcmp(string, "Register") == 0) {
                    char * new_login = NULL;
                    enum string_status_codes login = get_a_string(stdin, &new_login);
                    if (login == str_ok) {
                        char * new_password_str = NULL;
                        enum string_status_codes password = get_a_string(stdin, &new_password_str);
                        if (password == str_ok) {
                            int new_password = atoi(new_password_str);
                            free(new_password_str);
                            enum register_status_codes reg_status = _register(new_login, new_password, user_array, &user_array_size);
                            if (reg_status == reg_ok) {
                                printf("Registered\n");
                                print_login_menu();
                            }
                            else if (reg_status == reg_no_memory) {
                                printf("No memory\n");
                                free(new_login);
                                free(string);
                                clear_user_array(user_array, user_array_size);
                                return -1;
                            }
                            else if (reg_status == reg_registered) {
                                printf("Already registered\n");
                                print_login_menu();
                            }
                            else {
                                printf("Password should be in [0, 10^6] and login should contain only numbers or letters and have size less then 6\n");
                                print_login_menu();
                            }
                        }
                        else if (password == str_no_memory) {
                            free(new_login);
                            free(string);
                            clear_user_array(user_array, user_array_size);
                            return -1;
                        }
                        else {
                            printf("Something wrong with stdin\n");
                            return -1;
                        }
                    }
                    else if (login == str_no_memory) {
                        free(string);
                        clear_user_array(user_array, user_array_size);
                        return -1;
                    }
                    else {
                        printf("Something wrong with stdin\n");
                        return -1;
                    }
                }
                else if (strcmp(string, "Login") == 0) {
                    char * new_login_log = NULL;
                    enum string_status_codes login = get_a_string(stdin, &new_login_log);
                    if (login == str_ok) {
                        char * new_password_log_str = NULL;
                        enum string_status_codes password = get_a_string(stdin, &new_password_log_str);
                        if (password == str_ok) {
                            int new_password_log = atoi(new_password_log_str);
                            int curr_index = 0;
                            free(new_password_log_str);
                            enum login_status_codes log_status = _login(new_login_log, new_password_log, user_array, user_array_size, &curr_index);
                            if (log_status == log_ok) {
                                printf("Logged in!\n");
                                print_flag_menu();
                                enum command_status_codes fl = get_command(curr_index, user_array, user_array_size);
                                if (fl == com_ok) {
                                    print_login_menu();
                                }
                                else if (fl == com_command_limit) {
                                    printf("Your command limit is set to %d, unable to type commands in that session\n", user_array[curr_index]->command_limit);
                                    print_login_menu();
                                }
                                else if (fl == com_error) {
                                    printf("Error\n");
                                    print_login_menu();
                                }
                                else {
                                    printf("No memory\n");
                                    free(string);
                                    free(new_login_log);
                                    clear_user_array(user_array, user_array_size);
                                    return -1;
                                }
                            }
                            else if (log_status == log_logged) {
                                printf("Already logged in.\n");
                                print_login_menu();
                            }
                            else {
                                printf("Wrong password or login\n");
                                print_login_menu();
                            }
                        }
                        else if (password == str_no_memory) {
                            free(string);
                            free(new_login_log);
                            clear_user_array(user_array, user_array_size);
                            return -1;
                        }
                        else {
                            printf("something wrong with stdin\n");
                            return -1;
                        }
                    }
                    else if (login == str_no_memory) {
                        free(string);
                        clear_user_array(user_array, user_array_size);
                        return -1;
                    }
                    else {
                        printf("smth wrong with stdin\n");
                        return -1;
                    }
                }
                else printf("Wrong command\n");
            }
            else {
                clear_user_array(user_array, user_array_size);
                free(string);
                break;
            }
        }
    }
    return 0;
}