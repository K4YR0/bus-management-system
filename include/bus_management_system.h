#ifndef BUS_MANAGEMENT_SYSTEM_H
#define BUS_MANAGEMENT_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#endif

// Constants
#define MAX_USERS 100
#define MAX_STRING_LENGTH 100
#define MAX_PHONE_LENGTH 15
#define FILENAME "../data/users.txt"
#define BUS_FILENAME "../data/buses.txt"
#define CLIENT_FILENAME "../data/clients.txt"
#define EMPLOYEE_FILENAME "../data/employees.txt"
#define FUNCTION_FILENAME "../data/functions.txt"
#define TRIP_FILENAME "../data/trips.txt"

// Structure definitions
typedef struct User {
    char username[MAX_STRING_LENGTH];
    char password[MAX_STRING_LENGTH];
} User;

typedef struct PurchaseDate {
    int day;
    int month;
    int year;
} PurchaseDate;

typedef struct DateTime {
    int day;
    int month;
    int year;
    int hour;
    int minute;
} DateTime;

typedef struct Bus {
    int license_plate;
    float price;
    PurchaseDate purchase_date;
    int seat_count;
    struct Bus *next;
} Bus;

typedef struct Client {
    int client_id;
    char first_name[MAX_STRING_LENGTH];
    char last_name[MAX_STRING_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char city[MAX_STRING_LENGTH];
    char province[MAX_STRING_LENGTH];
    int postal_code;
    struct Client *next;
} Client;

typedef struct Employee {
    int employee_id;
    char first_name[MAX_STRING_LENGTH];
    char last_name[MAX_STRING_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    int function_id;
    struct Employee *next;
} Employee;

typedef struct Function {
    int function_id;
    char function_name[MAX_STRING_LENGTH];
    float salary;
    struct Function *next;
} Function;

typedef struct Trip {
    int license_plate;
    int client_id;
    DateTime departure_time;
    DateTime arrival_time;
    char departure_city[MAX_STRING_LENGTH];
    char arrival_city[MAX_STRING_LENGTH];
    struct Trip *next;
} Trip;

// Function prototypes

// Utility functions
void safe_string_input(char *buffer, size_t buffer_size);
int safe_int_input(void);
float safe_float_input(void);
void clear_input_buffer(void);
void set_console_color(int color);
void print_header(const char* title);
void pause_screen(void);

// Utility functions to check data existence
int has_buses(Bus *head);
int has_clients(Client *head);
int has_functions(Function *head);

// Date validation functions
int is_valid_date(int day, int month, int year);
int is_leap_year(int year);
int get_days_in_month(int month, int year);

// User authentication functions
void write_users_to_file(User users[], int num_users);
int read_users_from_file(User users[]);
int authenticate_user(User users[], int num_users);
void register_user(User users[], int *num_users);

// Bus management functions
Bus* add_bus_at_beginning(Bus *head);
Bus* add_bus_at_end(Bus *head);
void display_buses(Bus *head);
Bus* modify_bus(Bus *head);
Bus* delete_bus(Bus *head);
void search_bus(Bus *head);
void save_buses_to_file(Bus *head);
Bus* load_buses_from_file(Bus *head);
void free_bus_list(Bus *head);

// Client management functions
Client* add_client_at_beginning(Client *head);
Client* add_client_at_end(Client *head);
void display_clients(Client *head);
Client* modify_client(Client *head);
Client* delete_client(Client *head);
void search_client(Client *head);
void save_clients_to_file(Client *head);
Client* load_clients_from_file(Client *head);
void free_client_list(Client *head);

// Employee management functions
Employee* add_employee_at_beginning(Employee *head, Function *functions);
Employee* add_employee_at_end(Employee *head, Function *functions);
void display_employees(Employee *head, Function *functions);
Employee* modify_employee(Employee *head, Function *functions);
Employee* delete_employee(Employee *head);
void search_employee(Employee *head, Function *functions);
void save_employees_to_file(Employee *head);
Employee* load_employees_from_file(Employee *head);
void free_employee_list(Employee *head);

// Function management functions
Function* add_function_at_beginning(Function *head);
Function* add_function_at_end(Function *head);
void display_functions(Function *head);
Function* modify_function(Function *head);
Function* delete_function(Function *head);
void search_function(Function *head);
void save_functions_to_file(Function *head);
Function* load_functions_from_file(Function *head);
void free_function_list(Function *head);

// Trip management functions
Trip* add_trip_at_beginning(Trip *head, Bus *buses, Client *clients);
Trip* add_trip_at_end(Trip *head, Bus *buses, Client *clients);
void display_trips(Trip *head, Bus *buses, Client *clients);
Trip* modify_trip(Trip *head, Bus *buses, Client *clients);
Trip* delete_trip(Trip *head);
void search_trip(Trip *head, Bus *buses, Client *clients);
void save_trips_to_file(Trip *head);
Trip* load_trips_from_file(Trip *head);
void free_trip_list(Trip *head);

// Menu functions
void main_menu(Bus **buses, Client **clients, Employee **employees, Function **functions, Trip **trips);
void bus_menu(Bus **buses, Client *clients, Employee *employees, Function *functions, Trip *trips);
void client_menu(Bus *buses, Client **clients, Employee *employees, Function *functions, Trip *trips);
void employee_menu(Bus *buses, Client *clients, Employee **employees, Function *functions, Trip *trips);
void function_menu(Bus *buses, Client *clients, Employee *employees, Function **functions, Trip *trips);
void trip_menu(Bus *buses, Client *clients, Employee *employees, Function *functions, Trip **trips);
void bus_choice_menu(Bus **buses, Client *clients, Employee *employees, Function *functions, Trip *trips);
void client_choice_menu(Bus *buses, Client **clients, Employee *employees, Function *functions, Trip *trips);
void employee_choice_menu(Bus *buses, Client *clients, Employee **employees, Function *functions, Trip *trips);
void function_choice_menu(Bus *buses, Client *clients, Employee *employees, Function **functions, Trip *trips);
void trip_choice_menu(Bus *buses, Client *clients, Employee *employees, Function *functions, Trip **trips);

#endif // BUS_MANAGEMENT_SYSTEM_H
