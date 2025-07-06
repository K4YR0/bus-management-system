#include "../include/bus_management_system.h"

// Tic-tac-toe game variables
char board[3][3];
const char PLAYER = 'X';
const char COMPUTER = 'O';

// Utility functions
void safe_string_input(char *buffer, size_t buffer_size) {
    if (fgets(buffer, buffer_size, stdin) != NULL) {
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

int safe_int_input(void) {
    int value;
    while (scanf("%d", &value) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    return value;
}

float safe_float_input(void) {
    float value;
    while (scanf("%f", &value) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    return value;
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

#ifdef _WIN32
void set_console_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#else
void set_console_color(int color) {
    // For non-Windows systems, use ANSI colors
    switch(color) {
        case 2: printf("\033[0;32m"); break; // Green
        case 3: printf("\033[0;33m"); break; // Yellow
        case 4: printf("\033[0;31m"); break; // Red
        case 7: printf("\033[0;37m"); break; // White
        default: printf("\033[0m"); break;   // Reset
    }
}
#endif

void print_header(const char* title) {
    set_console_color(3);
    printf("\n");
    printf("================================================================================\n");
    printf("                              %s\n", title);
    printf("================================================================================\n");
    set_console_color(7);
}

void pause_screen(void) {
    printf("\nPress any key to continue...");
    #ifdef _WIN32
        getch();
        system("cls");
    #else
        getchar();
        system("clear");
    #endif
}

// Date validation functions
int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int get_days_in_month(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) {
        return 29;
    }
    return days[month - 1];
}

int is_valid_date(int day, int month, int year) {
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > get_days_in_month(month, year)) return 0;
    return 1;
}

// User authentication functions
void write_users_to_file(User users[], int num_users) {
    FILE *fp = fopen(FILENAME, "w");
    if (fp == NULL) {
        printf("Warning: Could not save user data to file.\n");
        return;
    }

    for (int i = 0; i < num_users; i++) {
        fprintf(fp, "%s %s\n", users[i].username, users[i].password);
    }

    fclose(fp);
    printf("User data saved successfully.\n");
}

int read_users_from_file(User users[]) {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("No existing user data found. Starting fresh.\n");
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
    if (ftell(fp) == 0) {
        fclose(fp);
        printf("User data file is empty. Starting fresh.\n");
        return 0;
    }
    rewind(fp);

    int num_users = 0;
    char username[MAX_STRING_LENGTH], password[MAX_STRING_LENGTH];
    while (fscanf(fp, "%99s %99s", username, password) == 2 && num_users < MAX_USERS) {
        strncpy(users[num_users].username, username, MAX_STRING_LENGTH - 1);
        strncpy(users[num_users].password, password, MAX_STRING_LENGTH - 1);
        users[num_users].username[MAX_STRING_LENGTH - 1] = '\0';
        users[num_users].password[MAX_STRING_LENGTH - 1] = '\0';
        num_users++;
    }

    fclose(fp);
    return num_users;
}

int authenticate_user(User users[], int num_users) {
    if (num_users == 0) {
        printf("No users registered yet. Please register first.\n");
        return 0;
    }
    
    char username[MAX_STRING_LENGTH], password[MAX_STRING_LENGTH];
    
    printf("Enter username: ");
    scanf("%99s", username);
    printf("Enter password: ");
    scanf("%99s", password);

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            strcmp(users[i].password, password) == 0) {
            return 1; // Authentication successful
        }
    }
    return 0; // Authentication failed
}

void register_user(User users[], int *num_users) {
    if (*num_users >= MAX_USERS) {
        printf("Maximum number of users reached (%d).\n", MAX_USERS);
        return;
    }

    print_header("REGISTER NEW ACCOUNT");
    
    char new_username[MAX_STRING_LENGTH], new_password[MAX_STRING_LENGTH];
    
    while (1) {
        printf("Enter username: ");
        scanf("%99s", new_username);
        
        int username_exists = 0;
        for (int i = 0; i < *num_users; i++) {
            if (strcmp(users[i].username, new_username) == 0) {
                username_exists = 1;
                break;
            }
        }
        
        if (username_exists) {
            printf("Username '%s' already exists. Please choose a different username.\n", new_username);
        } else {
            break;
        }
    }

    printf("Enter password: ");
    scanf("%99s", new_password);

    strncpy(users[*num_users].username, new_username, MAX_STRING_LENGTH - 1);
    strncpy(users[*num_users].password, new_password, MAX_STRING_LENGTH - 1);
    users[*num_users].username[MAX_STRING_LENGTH - 1] = '\0';
    users[*num_users].password[MAX_STRING_LENGTH - 1] = '\0';
    
    (*num_users)++;
    write_users_to_file(users, *num_users);
    printf("User '%s' registered successfully!\n", new_username);
    printf("You can now login with your credentials.\n");
}

void display_users(User users[], int num_users) {
    if (num_users == 0) {
        printf("No users registered yet.\n");
        return;
    }
    
    print_header("REGISTERED USERS (ADMIN VIEW)");
    printf("Total registered users: %d\n\n", num_users);
    
    set_console_color(2);
    printf("%-20s %-20s\n", "Username", "Password");
    printf("%-20s %-20s\n", "========", "========");
    set_console_color(7);
    
    for (int i = 0; i < num_users; i++) {
        printf("%-20s %-20s\n", users[i].username, users[i].password);
    }
}

// Bus management functions
Bus* add_bus_at_beginning(Bus *head) {
    Bus *new_bus = (Bus*)malloc(sizeof(Bus));
    if (new_bus == NULL) {
        printf("Memory allocation error.\n");
        return head;
    }

    PurchaseDate date;
    int license_exists;

    // Input and validate license plate
    do {
        license_exists = 0;
        printf("Enter bus license plate number: ");
        new_bus->license_plate = safe_int_input();
        
        Bus *temp = head;
        while (temp != NULL) {
            if (new_bus->license_plate == temp->license_plate) {
                printf("License plate already exists. Please enter a different one.\n");
                license_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (license_exists);

    printf("Enter bus purchase price: $");
    new_bus->price = safe_float_input();

    // Input and validate purchase date
    printf("Enter purchase date:\n");
    do {
        printf("Year (1900-2100): ");
        date.year = safe_int_input();
        printf("Month (1-12): ");
        date.month = safe_int_input();
        printf("Day: ");
        date.day = safe_int_input();
        
        if (!is_valid_date(date.day, date.month, date.year)) {
            printf("Invalid date. Please enter a valid date.\n");
        }
    } while (!is_valid_date(date.day, date.month, date.year));

    new_bus->purchase_date = date;

    printf("Enter number of seats: ");
    new_bus->seat_count = safe_int_input();

    new_bus->next = head;
    printf("Bus added successfully.\n");
    return new_bus;
}

Bus* add_bus_at_end(Bus *head) {
    Bus *new_bus = (Bus*)malloc(sizeof(Bus));
    if (new_bus == NULL) {
        printf("Memory allocation error. Cannot add bus.\n");
        return head;
    }

    print_header("ADD NEW BUS");
    
    PurchaseDate date;
    int license_exists;

    // Input and validate license plate
    do {
        license_exists = 0;
        printf("Enter bus license plate number: ");
        new_bus->license_plate = safe_int_input();
        
        Bus *temp = head;
        while (temp != NULL) {
            if (new_bus->license_plate == temp->license_plate) {
                printf("License plate %d already exists. Please enter a different one.\n", new_bus->license_plate);
                license_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (license_exists);

    printf("Enter bus purchase price: $");
    new_bus->price = safe_float_input();

    // Input and validate purchase date
    printf("\nEnter purchase date:\n");
    do {
        printf("Year (1900-2100): ");
        date.year = safe_int_input();
        printf("Month (1-12): ");
        date.month = safe_int_input();
        printf("Day: ");
        date.day = safe_int_input();
        
        if (!is_valid_date(date.day, date.month, date.year)) {
            printf("Invalid date. Please enter a valid date.\n");
        }
    } while (!is_valid_date(date.day, date.month, date.year));

    new_bus->purchase_date = date;

    printf("Enter number of seats: ");
    new_bus->seat_count = safe_int_input();

    new_bus->next = NULL;
    
    if (head == NULL) {
        printf("Bus added successfully! This is your first bus.\n");
        return new_bus;
    } else {
        Bus *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_bus;
        printf("Bus added successfully!\n");
        return head;
    }
}

void display_buses(Bus *head) {
    if (head == NULL) {
        printf("No buses found in the system.\n");
        printf("Tip: Use 'Add New Bus' to add your first bus!\n");
        return;
    }

    print_header("BUS FLEET OVERVIEW");
    
    // Count buses
    int bus_count = 0;
    Bus *temp = head;
    while (temp != NULL) {
        bus_count++;
        temp = temp->next;
    }
    
    printf("Total buses in fleet: %d\n\n", bus_count);
    
    set_console_color(2);
    printf("%-15s %-15s %-15s %-15s\n", "License Plate", "Price ($)", "Purchase Date", "Seats");
    printf("%-15s %-15s %-15s %-15s\n", "=============", "=========", "=============", "=====");
    set_console_color(7);

    temp = head;
    while (temp != NULL) {
        printf("%-15d %-15.2f %02d/%02d/%-8d %-15d\n",
               temp->license_plate,
               temp->price,
               temp->purchase_date.day,
               temp->purchase_date.month,
               temp->purchase_date.year,
               temp->seat_count);
        temp = temp->next;
    }
}

Bus* modify_bus(Bus *head) {
    if (head == NULL) {
        printf("No buses found in the system.\n");
        printf("Tip: Add some buses first before modifying!\n");
        return head;
    }

    print_header("MODIFY BUS");
    
    int license_plate;
    printf("Enter license plate of the bus to modify: ");
    license_plate = safe_int_input();

    Bus *temp = head;
    while (temp != NULL && temp->license_plate != license_plate) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Bus with license plate %d not found.\n", license_plate);
        printf("Tip: Check the license plate number and try again.\n");
        return head;
    }

    // Show current bus details
    printf("\nCurrent bus details:\n");
    printf("License Plate: %d\n", temp->license_plate);
    printf("Price: $%.2f\n", temp->price);
    printf("Purchase Date: %02d/%02d/%d\n", temp->purchase_date.day, temp->purchase_date.month, temp->purchase_date.year);
    printf("Seats: %d\n", temp->seat_count);
    
    printf("\nEnter new details:\n");
    printf("Enter new price: $");
    temp->price = safe_float_input();

    printf("\nEnter new purchase date:\n");
    PurchaseDate date;
    do {
        printf("Year (1900-2100): ");
        date.year = safe_int_input();
        printf("Month (1-12): ");
        date.month = safe_int_input();
        printf("Day: ");
        date.day = safe_int_input();
        
        if (!is_valid_date(date.day, date.month, date.year)) {
            printf("Invalid date. Please enter a valid date.\n");
        }
    } while (!is_valid_date(date.day, date.month, date.year));

    temp->purchase_date = date;

    printf("Enter new number of seats: ");
    temp->seat_count = safe_int_input();

    printf("Bus information updated successfully!\n");
    return head;
}

Bus* delete_bus(Bus *head) {
    if (head == NULL) {
        printf("No buses found in the system.\n");
        printf("Tip: Add some buses first before deleting!\n");
        return head;
    }

    print_header("DELETE BUS");
    
    int license_plate;
    printf("Enter license plate of the bus to delete: ");
    license_plate = safe_int_input();

    Bus *prev = NULL;
    Bus *temp = head;
    while (temp != NULL && temp->license_plate != license_plate) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Bus with license plate %d not found.\n", license_plate);
        printf("Tip: Check the license plate number and try again.\n");
        return head;
    }

    // Show bus details before deletion
    printf("\nBus to be deleted:\n");
    printf("License Plate: %d\n", temp->license_plate);
    printf("Price: $%.2f\n", temp->price);
    printf("Purchase Date: %02d/%02d/%d\n", temp->purchase_date.day, temp->purchase_date.month, temp->purchase_date.year);
    printf("Seats: %d\n", temp->seat_count);
    
    // Confirm deletion
    char confirm;
    printf("\nAre you sure you want to delete this bus? (y/N): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        free(temp);
        printf("Bus deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
    
    return head;
}

void search_bus(Bus *head) {
    if (head == NULL) {
        printf("No buses found in the system.\n");
        printf("Tip: Add some buses first before searching!\n");
        return;
    }

    print_header("SEARCH BUS");
    
    int license_plate;
    printf("Enter license plate number to search: ");
    license_plate = safe_int_input();

    Bus *temp = head;
    while (temp != NULL && temp->license_plate != license_plate) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Bus with license plate %d not found.\n", license_plate);
        printf("Tip: Check the license plate number and try again.\n");
    } else {
        printf("\nBus found!\n\n");
        set_console_color(2);
        printf("%-15s %-15s %-15s %-15s\n", "License Plate", "Price ($)", "Purchase Date", "Seats");
        printf("%-15s %-15s %-15s %-15s\n", "=============", "=========", "=============", "=====");
        set_console_color(7);
        printf("%-15d %-15.2f %02d/%02d/%-8d %-15d\n",
               temp->license_plate,
               temp->price,
               temp->purchase_date.day,
               temp->purchase_date.month,
               temp->purchase_date.year,
               temp->seat_count);
    }
}

void save_buses_to_file(Bus *head) {
    FILE *file = fopen(BUS_FILENAME, "w");
    if (file == NULL) {
        printf("Warning: Could not save buses to file. Data might be lost.\n");
        return;
    }

    int saved_count = 0;
    Bus *temp = head;
    while (temp != NULL) {
        fprintf(file, "%d\n%.2f\n%d/%d/%d\n%d\n",
                temp->license_plate,
                temp->price,
                temp->purchase_date.day,
                temp->purchase_date.month,
                temp->purchase_date.year,
                temp->seat_count);
        temp = temp->next;
        saved_count++;
    }

    fclose(file);
    printf("Successfully saved %d buses to file.\n", saved_count);
}

Bus* load_buses_from_file(Bus *head) {
    FILE *file = fopen(BUS_FILENAME, "r");
    if (file == NULL) {
        printf("No existing bus data file found. Starting with empty database.\n");
        return head;
    }

    // Check if file is empty
    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        printf("Bus data file is empty. Starting with empty database.\n");
        return head;
    }
    rewind(file);

    // Free existing list first
    free_bus_list(head);
    head = NULL;

    Bus *new_bus;
    int license_plate, seat_count;
    float price;
    int day, month, year;
    int loaded_count = 0;
    
    while (fscanf(file, "%d\n%f\n%d/%d/%d\n%d\n", 
                  &license_plate, &price, &day, &month, &year, &seat_count) == 6) {
        new_bus = (Bus*)malloc(sizeof(Bus));
        if (new_bus == NULL) {
            fclose(file);
            printf("Memory allocation error while loading buses.\n");
            return head;
        }

        new_bus->license_plate = license_plate;
        new_bus->price = price;
        new_bus->purchase_date.day = day;
        new_bus->purchase_date.month = month;
        new_bus->purchase_date.year = year;
        new_bus->seat_count = seat_count;
        new_bus->next = NULL;

        if (head == NULL) {
            head = new_bus;
        } else {
            Bus *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_bus;
        }
        loaded_count++;
    }

    fclose(file);
    printf("Successfully loaded %d buses from file.\n", loaded_count);
    return head;
}

void free_bus_list(Bus *head) {
    Bus *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Client management functions (similar pattern to bus functions)
Client* add_client_at_beginning(Client *head) {
    Client *new_client = (Client*)malloc(sizeof(Client));
    if (new_client == NULL) {
        printf("Memory allocation error.\n");
        return head;
    }

    int client_exists;
    do {
        client_exists = 0;
        printf("Enter client ID: ");
        new_client->client_id = safe_int_input();
        
        Client *temp = head;
        while (temp != NULL) {
            if (new_client->client_id == temp->client_id) {
                printf("Client ID already exists. Please enter a different one.\n");
                client_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (client_exists);

    printf("Enter first name: ");
    scanf("%99s", new_client->first_name);
    printf("Enter last name: ");
    scanf("%99s", new_client->last_name);
    printf("Enter phone number: ");
    scanf("%14s", new_client->phone);
    printf("Enter city: ");
    scanf("%99s", new_client->city);
    printf("Enter province: ");
    scanf("%99s", new_client->province);
    printf("Enter postal code: ");
    new_client->postal_code = safe_int_input();

    new_client->next = head;
    printf("Client added successfully.\n");
    return new_client;
}

Client* add_client_at_end(Client *head) {
    Client *new_client = (Client*)malloc(sizeof(Client));
    if (new_client == NULL) {
        printf("Memory allocation error. Cannot add client.\n");
        return head;
    }

    print_header("ADD NEW CLIENT");

    int client_exists;
    do {
        client_exists = 0;
        printf("Enter client ID: ");
        new_client->client_id = safe_int_input();
        
        Client *temp = head;
        while (temp != NULL) {
            if (new_client->client_id == temp->client_id) {
                printf("Client ID %d already exists. Please enter a different one.\n", new_client->client_id);
                client_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (client_exists);

    printf("Enter first name: ");
    scanf("%99s", new_client->first_name);
    printf("Enter last name: ");
    scanf("%99s", new_client->last_name);
    printf("Enter phone number: ");
    scanf("%14s", new_client->phone);
    printf("Enter city: ");
    scanf("%99s", new_client->city);
    printf("Enter province: ");
    scanf("%99s", new_client->province);
    printf("Enter postal code: ");
    new_client->postal_code = safe_int_input();

    new_client->next = NULL;
    
    if (head == NULL) {
        printf("Client added successfully! This is your first client.\n");
        return new_client;
    } else {
        Client *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_client;
        printf("Client added successfully!\n");
        return head;
    }
}

void display_clients(Client *head) {
    if (head == NULL) {
        printf("No clients found in the system.\n");
        printf("Tip: Use 'Add New Client' to add your first client!\n");
        return;
    }

    print_header("CLIENT DATABASE");
    
    // Count clients
    int client_count = 0;
    Client *temp = head;
    while (temp != NULL) {
        client_count++;
        temp = temp->next;
    }
    
    printf("Total clients in database: %d\n\n", client_count);
    
    set_console_color(2);
    printf("%-10s %-15s %-15s %-15s %-15s %-15s %-10s\n", 
           "ID", "First Name", "Last Name", "Phone", "City", "Province", "Postal Code");
    printf("%-10s %-15s %-15s %-15s %-15s %-15s %-10s\n", 
           "==", "==========", "=========", "=====", "====", "========", "===========");
    set_console_color(7);

    temp = head;
    while (temp != NULL) {
        printf("%-10d %-15s %-15s %-15s %-15s %-15s %-10d\n",
               temp->client_id,
               temp->first_name,
               temp->last_name,
               temp->phone,
               temp->city,
               temp->province,
               temp->postal_code);
        temp = temp->next;
    }
}

Client* modify_client(Client *head) {
    if (head == NULL) {
        printf("No clients found in the system.\n");
        printf("Tip: Add some clients first before modifying!\n");
        return head;
    }

    print_header("MODIFY CLIENT");
    
    int client_id;
    printf("Enter client ID to modify: ");
    client_id = safe_int_input();

    Client *temp = head;
    while (temp != NULL && temp->client_id != client_id) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Client with ID %d not found.\n", client_id);
        printf("Tip: Check the client ID and try again.\n");
        return head;
    }

    // Show current client details
    printf("\nCurrent client details:\n");
    printf("ID: %d\n", temp->client_id);
    printf("Name: %s %s\n", temp->first_name, temp->last_name);
    printf("Phone: %s\n", temp->phone);
    printf("Location: %s, %s %d\n", temp->city, temp->province, temp->postal_code);
    
    printf("\nEnter new details:\n");
    printf("Enter new first name: ");
    scanf("%99s", temp->first_name);
    printf("Enter new last name: ");
    scanf("%99s", temp->last_name);
    printf("Enter new phone number: ");
    scanf("%14s", temp->phone);
    printf("Enter new city: ");
    scanf("%99s", temp->city);
    printf("Enter new province: ");
    scanf("%99s", temp->province);
    printf("Enter new postal code: ");
    temp->postal_code = safe_int_input();

    printf("Client information updated successfully!\n");
    return head;
}

Client* delete_client(Client *head) {
    if (head == NULL) {
        printf("No clients found in the system.\n");
        printf("Tip: Add some clients first before deleting!\n");
        return head;
    }

    print_header("DELETE CLIENT");
    
    int client_id;
    printf("Enter client ID to delete: ");
    client_id = safe_int_input();

    Client *prev = NULL;
    Client *temp = head;
    while (temp != NULL && temp->client_id != client_id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Client with ID %d not found.\n", client_id);
        printf("Tip: Check the client ID and try again.\n");
        return head;
    }

    // Show client details before deletion
    printf("\nClient to be deleted:\n");
    printf("ID: %d\n", temp->client_id);
    printf("Name: %s %s\n", temp->first_name, temp->last_name);
    printf("Phone: %s\n", temp->phone);
    printf("Location: %s, %s %d\n", temp->city, temp->province, temp->postal_code);
    
    // Confirm deletion
    char confirm;
    printf("\nAre you sure you want to delete this client? (y/N): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        free(temp);
        printf("Client deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
    
    return head;
}

void search_client(Client *head) {
    if (head == NULL) {
        printf("No clients found in the system.\n");
        printf("Tip: Add some clients first before searching!\n");
        return;
    }

    print_header("SEARCH CLIENT");
    
    int client_id;
    printf("Enter client ID to search: ");
    client_id = safe_int_input();

    Client *temp = head;
    while (temp != NULL && temp->client_id != client_id) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Client with ID %d not found.\n", client_id);
        printf("Tip: Check the client ID and try again.\n");
    } else {
        printf("\nClient found!\n\n");
        set_console_color(2);
        printf("%-10s %-15s %-15s %-15s %-15s %-15s %-10s\n", 
               "ID", "First Name", "Last Name", "Phone", "City", "Province", "Postal Code");
        printf("%-10s %-15s %-15s %-15s %-15s %-15s %-10s\n", 
               "==", "==========", "=========", "=====", "====", "========", "===========");
        set_console_color(7);
        printf("%-10d %-15s %-15s %-15s %-15s %-15s %-10d\n",
               temp->client_id,
               temp->first_name,
               temp->last_name,
               temp->phone,
               temp->city,
               temp->province,
               temp->postal_code);
    }
}

void save_clients_to_file(Client *head) {
    FILE *file = fopen(CLIENT_FILENAME, "w");
    if (file == NULL) {
        printf("Warning: Could not save clients to file. Data might be lost.\n");
        return;
    }

    int saved_count = 0;
    Client *temp = head;
    while (temp != NULL) {
        fprintf(file, "%d\n%s\n%s\n%s\n%s\n%s\n%d\n",
                temp->client_id,
                temp->first_name,
                temp->last_name,
                temp->phone,
                temp->city,
                temp->province,
                temp->postal_code);
        temp = temp->next;
        saved_count++;
    }

    fclose(file);
    printf("Successfully saved %d clients to file.\n", saved_count);
}

Client* load_clients_from_file(Client *head) {
    FILE *file = fopen(CLIENT_FILENAME, "r");
    if (file == NULL) {
        printf("No existing client data file found. Starting with empty database.\n");
        return head;
    }

    // Check if file is empty
    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        printf("Client data file is empty. Starting with empty database.\n");
        return head;
    }
    rewind(file);

    // Free existing list first
    free_client_list(head);
    head = NULL;

    Client *new_client;
    int client_id, postal_code;
    char first_name[MAX_STRING_LENGTH], last_name[MAX_STRING_LENGTH];
    char phone[MAX_PHONE_LENGTH], city[MAX_STRING_LENGTH], province[MAX_STRING_LENGTH];
    int loaded_count = 0;
    
    while (fscanf(file, "%d\n%99s\n%99s\n%14s\n%99s\n%99s\n%d\n", 
                  &client_id, first_name, last_name, phone, city, province, &postal_code) == 7) {
        new_client = (Client*)malloc(sizeof(Client));
        if (new_client == NULL) {
            fclose(file);
            printf("Memory allocation error while loading clients.\n");
            return head;
        }

        new_client->client_id = client_id;
        strncpy(new_client->first_name, first_name, MAX_STRING_LENGTH - 1);
        strncpy(new_client->last_name, last_name, MAX_STRING_LENGTH - 1);
        strncpy(new_client->phone, phone, MAX_PHONE_LENGTH - 1);
        strncpy(new_client->city, city, MAX_STRING_LENGTH - 1);
        strncpy(new_client->province, province, MAX_STRING_LENGTH - 1);
        new_client->postal_code = postal_code;
        new_client->next = NULL;

        if (head == NULL) {
            head = new_client;
        } else {
            Client *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_client;
        }
        loaded_count++;
    }

    fclose(file);
    printf("Successfully loaded %d clients from file.\n", loaded_count);
    return head;
}

void free_client_list(Client *head) {
    Client *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// I'll continue with the remaining functions and menu system in the next part
// due to length constraints. Let me create the main function and basic menu structure first.

int main() {
    Bus *buses = NULL;
    Client *clients = NULL;
    Employee *employees = NULL;
    Function *functions = NULL;
    Trip *trips = NULL;
    
    User users[MAX_USERS];
    int num_users = 0;
    
    // Load users from file
    num_users = read_users_from_file(users);
    
    int choice;
    do {
        print_header("BUS MANAGEMENT SYSTEM - LOGIN");
        set_console_color(2);
        printf("Welcome to the Bus Management System\n\n");
        printf("1. Login\n");
        printf("2. Register New Account\n");
        printf("3. View Registered Users (Admin)\n");
        printf("0. Exit\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                if (authenticate_user(users, num_users)) {
                    printf("\nLogin successful! Welcome to the system.\n");
                    pause_screen();
                    
                    // Auto-load all data when user logs in
                    printf("Loading system data...\n");
                    buses = load_buses_from_file(buses);
                    clients = load_clients_from_file(clients);
                    // employees = load_employees_from_file(employees);
                    // functions = load_functions_from_file(functions);
                    // trips = load_trips_from_file(trips);
                    printf("System ready!\n");
                    pause_screen();
                    
                    main_menu(&buses, &clients, &employees, &functions, &trips);
                } else {
                    printf("\nInvalid username or password. Please try again.\n");
                }
                break;
            case 2:
                register_user(users, &num_users);
                break;
            case 3:
                display_users(users, num_users);
                break;
            case 0:
                printf("\nThank you for using Bus Management System. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
        
        if (choice != 0) {
            pause_screen();
        }
    } while (choice != 0);
    
    // Auto-save all data before exit
    if (buses || clients || employees || functions || trips) {
        printf("Saving system data...\n");
        save_buses_to_file(buses);
        save_clients_to_file(clients);
        // save_employees_to_file(employees);
        // save_functions_to_file(functions);
        // save_trips_to_file(trips);
        printf("Data saved successfully!\n");
    }
    
    // Free memory
    free_bus_list(buses);
    free_client_list(clients);
    // free_employee_list(employees);
    // free_function_list(functions);
    // free_trip_list(trips);
    
    return 0;
}

// Basic menu structure - this needs to be expanded with all menu functions
void main_menu(Bus **buses, Client **clients, Employee **employees, Function **functions, Trip **trips) {
    int choice;
    do {
        print_header("BUS MANAGEMENT SYSTEM - MAIN MENU");
        set_console_color(2);
        printf("Welcome to the Bus Management System\n\n");
        printf("1. Bus Management\n");
        printf("2. Client Management\n");
        printf("3. Employee Management\n");
        printf("4. Function Management\n");
        printf("5. Trip Management\n");
        printf("6. Play Tic-Tac-Toe\n");
        printf("7. Play Rock Paper Scissors\n");
        printf("8. Save All Data\n");
        printf("0. Logout\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                bus_menu(buses, *clients, *employees, *functions, *trips);
                break;
            case 2:
                client_menu(*buses, clients, *employees, *functions, *trips);
                break;
            case 3:
                printf("Employee management coming soon...\n");
                break;
            case 4:
                printf("Function management coming soon...\n");
                break;
            case 5:
                printf("Trip management coming soon...\n");
                break;
            case 6:
                play_tic_tac_toe();
                break;
            case 7:
                play_rock_paper_scissors();
                break;
            case 8:
                printf("Saving all data...\n");
                save_buses_to_file(*buses);
                save_clients_to_file(*clients);
                printf("All data saved successfully!\n");
                break;
            case 0:
                printf("\nLogging out...\n");
                // Auto-save before logout
                printf("Auto-saving data...\n");
                save_buses_to_file(*buses);
                save_clients_to_file(*clients);
                printf("Data saved successfully!\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
        
        if (choice != 0) {
            pause_screen();
        }
    } while (choice != 0);
}

void bus_menu(Bus **buses, Client *clients, Employee *employees, Function *functions, Trip *trips) {
    int choice;
    do {
        print_header("BUS MANAGEMENT");
        
        // Show current bus count
        int bus_count = 0;
        Bus *temp = *buses;
        while (temp != NULL) {
            bus_count++;
            temp = temp->next;
        }
        printf("Current buses in system: %d\n\n", bus_count);
        
        set_console_color(2);
        printf("1. Add New Bus\n");
        printf("2. View All Buses\n");
        printf("3. Modify Bus Information\n");
        printf("4. Delete Bus\n");
        printf("5. Search Bus\n");
        printf("6. Save Buses to File\n");
        printf("7. Reload Buses from File\n");
        printf("0. Back to Main Menu\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                *buses = add_bus_at_end(*buses);
                break;
            case 2:
                display_buses(*buses);
                break;
            case 3:
                *buses = modify_bus(*buses);
                break;
            case 4:
                *buses = delete_bus(*buses);
                break;
            case 5:
                search_bus(*buses);
                break;
            case 6:
                save_buses_to_file(*buses);
                break;
            case 7:
                *buses = load_buses_from_file(*buses);
                break;
            case 0:
                return;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
        
        if (choice != 0) {
            pause_screen();
        }
    } while (choice != 0);
}

void client_menu(Bus *buses, Client **clients, Employee *employees, Function *functions, Trip *trips) {
    int choice;
    do {
        print_header("CLIENT MANAGEMENT");
        
        // Show current client count
        int client_count = 0;
        Client *temp = *clients;
        while (temp != NULL) {
            client_count++;
            temp = temp->next;
        }
        printf("Current clients in system: %d\n\n", client_count);
        
        set_console_color(2);
        printf("1. Add New Client\n");
        printf("2. View All Clients\n");
        printf("3. Modify Client Information\n");
        printf("4. Delete Client\n");
        printf("5. Search Client\n");
        printf("6. Save Clients to File\n");
        printf("7. Reload Clients from File\n");
        printf("0. Back to Main Menu\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                *clients = add_client_at_end(*clients);
                break;
            case 2:
                display_clients(*clients);
                break;
            case 3:
                *clients = modify_client(*clients);
                break;
            case 4:
                *clients = delete_client(*clients);
                break;
            case 5:
                search_client(*clients);
                break;
            case 6:
                save_clients_to_file(*clients);
                break;
            case 7:
                *clients = load_clients_from_file(*clients);
                break;
            case 0:
                return;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
        
        if (choice != 0) {
            pause_screen();
        }
    } while (choice != 0);
}

// Tic-tac-toe game functions
void reset_board() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

void print_board() {
    printf("\n");
    printf(" %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
    printf("\n");
}

int check_free_spaces() {
    int free_spaces = 9;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != ' ') {
                free_spaces--;
            }
        }
    }
    return free_spaces;
}

void player_move() {
    int x, y;
    do {
        printf("Enter row (1-3): ");
        x = safe_int_input() - 1;
        printf("Enter column (1-3): ");
        y = safe_int_input() - 1;
        
        if (x < 0 || x > 2 || y < 0 || y > 2) {
            printf("Invalid coordinates!\n");
        } else if (board[x][y] != ' ') {
            printf("Invalid move!\n");
        } else {
            board[x][y] = PLAYER;
            break;
        }
    } while (1);
}

void computer_move() {
    srand(time(0));
    int x, y;
    
    if (check_free_spaces() > 0) {
        do {
            x = rand() % 3;
            y = rand() % 3;
        } while (board[x][y] != ' ');
        
        board[x][y] = COMPUTER;
    }
}

char check_winner() {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            return board[i][0];
        }
    }
    
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[0][i] == board[1][i] && board[0][i] == board[2][i]) {
            return board[0][i];
        }
    }
    
    // Check diagonals
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        return board[0][2];
    }
    
    return ' ';
}

void print_winner(char winner) {
    if (winner == PLAYER) {
        printf("YOU WIN!\n");
    } else if (winner == COMPUTER) {
        printf("YOU LOSE!\n");
    } else {
        printf("IT'S A TIE!\n");
    }
}

void play_tic_tac_toe() {
    char winner = ' ';
    char response;
    
    do {
        winner = ' ';
        response = ' ';
        reset_board();
        
        while (winner == ' ' && check_free_spaces() != 0) {
            print_board();
            player_move();
            winner = check_winner();
            if (winner != ' ' || check_free_spaces() == 0) {
                break;
            }
            
            computer_move();
            winner = check_winner();
            if (winner != ' ' || check_free_spaces() == 0) {
                break;
            }
        }
        
        print_board();
        print_winner(winner);
        
        printf("\nWould you like to play again? (Y/N): ");
        scanf(" %c", &response);
        response = toupper(response);
        
    } while (response == 'Y');
    
    printf("Thanks for playing!\n");
}

void play_rock_paper_scissors() {
    printf("Rock Paper Scissors Game\n");
    printf("1. Rock\n");
    printf("2. Paper\n");
    printf("3. Scissors\n");
    
    int player_choice;
    printf("Enter your choice (1-3): ");
    player_choice = safe_int_input();
    
    if (player_choice < 1 || player_choice > 3) {
        printf("Invalid choice!\n");
        return;
    }
    
    srand(time(0));
    int computer_choice = (rand() % 3) + 1;
    
    const char* choices[] = {"Rock", "Paper", "Scissors"};
    printf("You chose: %s\n", choices[player_choice - 1]);
    printf("Computer chose: %s\n", choices[computer_choice - 1]);
    
    if (player_choice == computer_choice) {
        printf("It's a tie!\n");
    } else if ((player_choice == 1 && computer_choice == 3) ||
               (player_choice == 2 && computer_choice == 1) ||
               (player_choice == 3 && computer_choice == 2)) {
        printf("You win!\n");
    } else {
        printf("You lose!\n");
    }
}
