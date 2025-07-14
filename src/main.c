#include "../include/bus_management_system.h"

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

// Real Password Encryption Functions
void generate_key(unsigned char *key, int length) {
    // Generate a pseudo-random key based on system properties
    srand((unsigned int)time(NULL));
    for (int i = 0; i < length; i++) {
        key[i] = (unsigned char)(rand() % 256);
    }
}

void xor_encrypt_decrypt(char *input, char *output, unsigned char *key, int key_length) {
    int input_length = strlen(input);
    for (int i = 0; i < input_length; i++) {
        output[i] = input[i] ^ key[i % key_length];
    }
    output[input_length] = '\0';
}

// Simple hash function for password verification
void hash_password(char *password, char *hashed_password) {
    unsigned long hash = 5381;
    int c;
    char *str = password;
    
    // djb2 hash algorithm
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    // Convert hash to hexadecimal string
    sprintf(hashed_password, "%08lx", hash);
    
    // Add salt and rehash for extra security
    char salted[MAX_STRING_LENGTH * 2];
    sprintf(salted, "%s_SALT_2024_%s", hashed_password, password);
    
    hash = 5381;
    str = salted;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    sprintf(hashed_password, "%016lx", hash);
}

int verify_password(char *input_password, char *stored_hash) {
    char computed_hash[MAX_STRING_LENGTH];
    hash_password(input_password, computed_hash);
    return strcmp(computed_hash, stored_hash) == 0;
}

void secure_encrypt_password(char *password, char *encrypted_password) {
    // First, hash the password
    char hashed[MAX_STRING_LENGTH];
    hash_password(password, hashed);
    
    // Then encrypt the hash with XOR using a fixed key for consistency
    unsigned char encryption_key[] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 
                                     0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    int key_length = sizeof(encryption_key);
    
    xor_encrypt_decrypt(hashed, encrypted_password, encryption_key, key_length);
    
    // Convert to hexadecimal for safe storage
    char hex_output[MAX_STRING_LENGTH * 2];
    for (int i = 0; i < strlen(encrypted_password); i++) {
        sprintf(hex_output + (i * 2), "%02x", (unsigned char)encrypted_password[i]);
    }
    strcpy(encrypted_password, hex_output);
}

// User authentication functions
void write_users_to_file(User users[], int num_users) {
    FILE *fp = fopen(FILENAME, "w");
    if (fp == NULL) {
         return;
    }

    for (int i = 0; i < num_users; i++) {
        // Users array already contains encrypted passwords, write them directly
        fprintf(fp, "%s %s\n", users[i].username, users[i].password);
    }

    fclose(fp);
}

int read_users_from_file(User users[]) {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
    if (ftell(fp) == 0) {
        fclose(fp);
        return 0;
    }
    rewind(fp);

    int num_users = 0;
    char username[MAX_STRING_LENGTH], encrypted_password[MAX_STRING_LENGTH * 2];
    while (fscanf(fp, "%99s %199s", username, encrypted_password) == 2 && num_users < MAX_USERS) {
        strncpy(users[num_users].username, username, MAX_STRING_LENGTH - 1);
        users[num_users].username[MAX_STRING_LENGTH - 1] = '\0';
        
        // Store the encrypted password hash directly (no decryption needed)
        strncpy(users[num_users].password, encrypted_password, MAX_STRING_LENGTH * 2 - 1);
        users[num_users].password[MAX_STRING_LENGTH * 2 - 1] = '\0';
        
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
        if (strcmp(users[i].username, username) == 0) {
            // Verify password using the secure verification method
            char computed_hash[MAX_STRING_LENGTH * 2];
            secure_encrypt_password(password, computed_hash);
            
            if (strcmp(users[i].password, computed_hash) == 0) {
                return 1; // Authentication successful
            }
            break; // Username found but password doesn't match
        }
    }
    return 0; // Authentication failed
}

void register_user(User users[], int *num_users) {
    if (*num_users >= MAX_USERS) {
        printf("Maximum number of users reached (%d).\n", MAX_USERS);
        return;
    }

    print_header("REGISTER NEW ACCOUNT - ADVANCED ENCRYPTION");
    
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

    // Store username
    strncpy(users[*num_users].username, new_username, MAX_STRING_LENGTH - 1);
    users[*num_users].username[MAX_STRING_LENGTH - 1] = '\0';
    
    // Encrypt password and store the encrypted version in memory
    char encrypted_password[MAX_STRING_LENGTH * 2];
    secure_encrypt_password(new_password, encrypted_password);
    strncpy(users[*num_users].password, encrypted_password, MAX_STRING_LENGTH * 2 - 1);
    users[*num_users].password[MAX_STRING_LENGTH * 2 - 1] = '\0';
    
    (*num_users)++;
    write_users_to_file(users, *num_users);
    printf("User '%s' registered successfully!\n", new_username);
    printf("You can now login with your credentials.\n");
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
        printf("  No buses found in the system.\n");
        printf("You need to add buses first to view them.\n");
        printf("Please go to 'Bus Management' -> 'Add New Bus' to create your first bus.\n");
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
        printf("%-15d %-15.2f %02d/%02d/%-8d  %-15d\n",
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
        printf("  No buses found in the system.\n");
        printf("You need to add buses first before modifying them.\n");
        printf("Please go to 'Bus Management' -> 'Add New Bus' to create your first bus.\n");
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
        printf("  No buses found in the system.\n");
        printf("You need to add buses first before deleting them.\n");
        printf("Please go to 'Bus Management' -> 'Add New Bus' to create your first bus.\n");
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
        printf("  No buses found in the system.\n");
        printf("You need to add buses first before searching for them.\n");
        printf("Please go to 'Bus Management' -> 'Add New Bus' to create your first bus.\n");
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
        return;
    }

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
    }

    fclose(file);
}

Bus* load_buses_from_file(Bus *head) {
    FILE *file = fopen(BUS_FILENAME, "r");
    if (file == NULL) {
        return head;
    }

    // Check if file is empty
    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
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
    
    while (fscanf(file, "%d\n%f\n%d/%d/%d\n%d\n", 
                  &license_plate, &price, &day, &month, &year, &seat_count) == 6) {
        new_bus = (Bus*)malloc(sizeof(Bus));
        if (new_bus == NULL) {
            fclose(file);
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
    }

    fclose(file);
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
        printf("  No clients found in the system.\n");
        printf("You need to add clients first to view them.\n");
        printf("Please go to 'Client Management' -> 'Add New Client' to create your first client.\n");
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
    printf("%-10s %-15s %-15s %-15s %-15s %-30s %-10s\n", 
           "ID", "First Name", "Last Name", "Phone", "City", "Province", "Postal Code");
    printf("%-10s %-15s %-15s %-15s %-15s %-30s %-10s\n", 
           "==", "==========", "=========", "=====", "====", "========", "===========");
    set_console_color(7);

    temp = head;
    while (temp != NULL) {
        printf("%-10d %-15s %-15s %-15s %-15s %-30s %-10d\n",
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
        printf("  No clients found in the system.\n");
        printf("You need to add clients first before modifying them.\n");
        printf("Please go to 'Client Management' -> 'Add New Client' to create your first client.\n");
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
        printf("  No clients found in the system.\n");
        printf("You need to add clients first before deleting them.\n");
        printf("Please go to 'Client Management' -> 'Add New Client' to create your first client.\n");
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
        printf("  No clients found in the system.\n");
        printf("You need to add clients first before searching for them.\n");
        printf("Please go to 'Client Management' -> 'Add New Client' to create your first client.\n");
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
        return;
    }

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
    }

    fclose(file);
}

Client* load_clients_from_file(Client *head) {
    FILE *file = fopen(CLIENT_FILENAME, "r");
    if (file == NULL) {
        return head;
    }

    // Check if file is empty
    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
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
    char buffer[MAX_STRING_LENGTH];
    
    while (fscanf(file, "%d\n", &client_id) == 1) {
        // Read first name
        if (fgets(first_name, MAX_STRING_LENGTH, file) == NULL) break;
        first_name[strcspn(first_name, "\n")] = '\0'; // Remove newline
        
        // Read last name
        if (fgets(last_name, MAX_STRING_LENGTH, file) == NULL) break;
        last_name[strcspn(last_name, "\n")] = '\0'; // Remove newline
        
        // Read phone
        if (fgets(phone, MAX_PHONE_LENGTH, file) == NULL) break;
        phone[strcspn(phone, "\n")] = '\0'; // Remove newline
        
        // Read city
        if (fgets(city, MAX_STRING_LENGTH, file) == NULL) break;
        city[strcspn(city, "\n")] = '\0'; // Remove newline
        
        // Read province
        if (fgets(province, MAX_STRING_LENGTH, file) == NULL) break;
        province[strcspn(province, "\n")] = '\0'; // Remove newline
        
        // Read postal code
        if (fscanf(file, "%d\n", &postal_code) != 1) break;
        
        new_client = (Client*)malloc(sizeof(Client));
        if (new_client == NULL) {
            fclose(file);
            return head;
        }

        new_client->client_id = client_id;
        strncpy(new_client->first_name, first_name, MAX_STRING_LENGTH - 1);
        new_client->first_name[MAX_STRING_LENGTH - 1] = '\0';
        strncpy(new_client->last_name, last_name, MAX_STRING_LENGTH - 1);
        new_client->last_name[MAX_STRING_LENGTH - 1] = '\0';
        strncpy(new_client->phone, phone, MAX_PHONE_LENGTH - 1);
        new_client->phone[MAX_PHONE_LENGTH - 1] = '\0';
        strncpy(new_client->city, city, MAX_STRING_LENGTH - 1);
        new_client->city[MAX_STRING_LENGTH - 1] = '\0';
        strncpy(new_client->province, province, MAX_STRING_LENGTH - 1);
        new_client->province[MAX_STRING_LENGTH - 1] = '\0';
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
    }

    fclose(file);
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

// Employee management functions
Employee* add_employee_at_end(Employee *head, Function *functions) {
    // Check prerequisites first
    if (!has_functions(functions)) {
        printf("  No job functions available in the system.\n");
        printf("You need to create job functions first before adding employees.\n");
        printf("Please go to 'Function Management' -> 'Add New Function' to add job functions.\n");
        return head;
    }

    Employee *new_employee = (Employee*)malloc(sizeof(Employee));
    if (new_employee == NULL) {
        printf("Memory allocation error. Cannot add employee.\n");
        return head;
    }

    print_header("ADD NEW EMPLOYEE");

    int employee_exists;
    do {
        employee_exists = 0;
        printf("Enter employee ID: ");
        new_employee->employee_id = safe_int_input();
        
        Employee *temp = head;
        while (temp != NULL) {
            if (new_employee->employee_id == temp->employee_id) {
                printf("Employee ID %d already exists. Please enter a different one.\n", new_employee->employee_id);
                employee_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (employee_exists);

    printf("Enter first name: ");
    scanf("%99s", new_employee->first_name);
    printf("Enter last name: ");
    scanf("%99s", new_employee->last_name);
    printf("Enter phone number: ");
    scanf("%14s", new_employee->phone);
    
    // Display available functions
    display_functions(functions);
    printf("Enter function ID: ");
    new_employee->function_id = safe_int_input();

    new_employee->next = NULL;
    
    if (head == NULL) {
        printf("Employee added successfully! This is your first employee.\n");
        return new_employee;
    } else {
        Employee *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_employee;
        printf("Employee added successfully!\n");
        return head;
    }
}

Employee* add_employee_at_beginning(Employee *head, Function *functions) {
    // Check prerequisites first
    if (!has_functions(functions)) {
        printf("  No job functions available in the system.\n");
        printf("You need to create job functions first before adding employees.\n");
        printf("Please go to 'Function Management' -> 'Add New Function' to add job functions.\n");
        return head;
    }

    Employee *new_employee = (Employee*)malloc(sizeof(Employee));
    if (new_employee == NULL) {
        printf("Memory allocation error.\n");
        return head;
    }

    int employee_exists;
    do {
        employee_exists = 0;
        printf("Enter employee ID: ");
        new_employee->employee_id = safe_int_input();
        
        Employee *temp = head;
        while (temp != NULL) {
            if (new_employee->employee_id == temp->employee_id) {
                printf("Employee ID already exists. Please enter a different one.\n");
                employee_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (employee_exists);

    printf("Enter first name: ");
    scanf("%99s", new_employee->first_name);
    printf("Enter last name: ");
    scanf("%99s", new_employee->last_name);
    printf("Enter phone number: ");
    scanf("%14s", new_employee->phone);
    
    display_functions(functions);
    printf("Enter function ID: ");
    new_employee->function_id = safe_int_input();

    new_employee->next = head;
    printf("Employee added successfully.\n");
    return new_employee;
}

void display_employees(Employee *head, Function *functions) {
    if (head == NULL) {
        printf("  No employees found in the system.\n");
        printf("You need to add employees first to view them.\n");
        printf("Please go to 'Employee Management' -> 'Add New Employee' to create your first employee.\n");
        return;
    }

    print_header("EMPLOYEE DATABASE");
    
    int employee_count = 0;
    Employee *temp = head;
    while (temp != NULL) {
        employee_count++;
        temp = temp->next;
    }
    
    printf("Total employees in database: %d\n\n", employee_count);
    
    set_console_color(2);
    printf("%-10s %-15s %-15s %-15s %-15s\n", 
           "ID", "First Name", "Last Name", "Phone", "Function");
    printf("%-10s %-15s %-15s %-15s %-15s\n", 
           "==", "==========", "=========", "=====", "========");
    set_console_color(7);

    temp = head;
    while (temp != NULL) {
        // Find function name
        char function_name[MAX_STRING_LENGTH] = "Unknown";
        Function *func = functions;
        while (func != NULL) {
            if (func->function_id == temp->function_id) {
                strncpy(function_name, func->function_name, MAX_STRING_LENGTH - 1);
                function_name[MAX_STRING_LENGTH - 1] = '\0';
                break;
            }
            func = func->next;
        }
        
        printf("%-10d %-15s %-15s %-15s %-15s\n",
               temp->employee_id,
               temp->first_name,
               temp->last_name,
               temp->phone,
               function_name);
        temp = temp->next;
    }
}

Employee* modify_employee(Employee *head, Function *functions) {
    if (head == NULL) {
        printf("No employees found in the system.\n");
        return head;
    }

    print_header("MODIFY EMPLOYEE");
    
    int employee_id;
    printf("Enter employee ID to modify: ");
    employee_id = safe_int_input();

    Employee *temp = head;
    while (temp != NULL && temp->employee_id != employee_id) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Employee with ID %d not found.\n", employee_id);
        return head;
    }

    printf("\nCurrent employee details:\n");
    printf("ID: %d\n", temp->employee_id);
    printf("Name: %s %s\n", temp->first_name, temp->last_name);
    printf("Phone: %s\n", temp->phone);
    printf("Function ID: %d\n", temp->function_id);
    
    printf("\nEnter new details:\n");
    printf("Enter new first name: ");
    scanf("%99s", temp->first_name);
    printf("Enter new last name: ");
    scanf("%99s", temp->last_name);
    printf("Enter new phone number: ");
    scanf("%14s", temp->phone);
    
    display_functions(functions);
    printf("Enter new function ID: ");
    temp->function_id = safe_int_input();

    printf("Employee information updated successfully!\n");
    return head;
}

Employee* delete_employee(Employee *head) {
    if (head == NULL) {
        printf("No employees found in the system.\n");
        return head;
    }

    print_header("DELETE EMPLOYEE");
    
    int employee_id;
    printf("Enter employee ID to delete: ");
    employee_id = safe_int_input();

    Employee *prev = NULL;
    Employee *temp = head;
    while (temp != NULL && temp->employee_id != employee_id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Employee with ID %d not found.\n", employee_id);
        return head;
    }

    printf("\nEmployee to be deleted:\n");
    printf("ID: %d\n", temp->employee_id);
    printf("Name: %s %s\n", temp->first_name, temp->last_name);
    printf("Phone: %s\n", temp->phone);
    
    char confirm;
    printf("\nAre you sure you want to delete this employee? (y/N): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        free(temp);
        printf("Employee deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
    
    return head;
}

void search_employee(Employee *head, Function *functions) {
    if (head == NULL) {
        printf("No employees found in the system.\n");
        return;
    }

    print_header("SEARCH EMPLOYEE");
    
    int employee_id;
    printf("Enter employee ID to search: ");
    employee_id = safe_int_input();

    Employee *temp = head;
    while (temp != NULL && temp->employee_id != employee_id) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Employee with ID %d not found.\n", employee_id);
    } else {
        printf("\nEmployee found!\n\n");
        set_console_color(2);
        printf("%-10s %-15s %-15s %-15s %-15s\n", 
               "ID", "First Name", "Last Name", "Phone", "Function");
        printf("%-10s %-15s %-15s %-15s %-15s\n", 
               "==", "==========", "=========", "=====", "========");
        set_console_color(7);
        
        char function_name[MAX_STRING_LENGTH] = "Unknown";
        Function *func = functions;
        while (func != NULL) {
            if (func->function_id == temp->function_id) {
                strncpy(function_name, func->function_name, MAX_STRING_LENGTH - 1);
                function_name[MAX_STRING_LENGTH - 1] = '\0';
                break;
            }
            func = func->next;
        }
        
        printf("%-10d %-15s %-15s %-15s %-15s\n",
               temp->employee_id,
               temp->first_name,
               temp->last_name,
               temp->phone,
               function_name);
    }
}

void save_employees_to_file(Employee *head) {
    FILE *file = fopen(EMPLOYEE_FILENAME, "w");
    if (file == NULL) {
        return;
    }

    Employee *temp = head;
    while (temp != NULL) {
        fprintf(file, "%d\n%s\n%s\n%s\n%d\n",
                temp->employee_id,
                temp->first_name,
                temp->last_name,
                temp->phone,
                temp->function_id);
        temp = temp->next;
    }

    fclose(file);
}

Employee* load_employees_from_file(Employee *head) {
    FILE *file = fopen(EMPLOYEE_FILENAME, "r");
    if (file == NULL) {
        return head;
    }

    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        return head;
    }
    rewind(file);

    free_employee_list(head);
    head = NULL;

    Employee *new_employee;
    int employee_id, function_id;
    char first_name[MAX_STRING_LENGTH], last_name[MAX_STRING_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    
    while (fscanf(file, "%d\n%99s\n%99s\n%14s\n%d\n", 
                  &employee_id, first_name, last_name, phone, &function_id) == 5) {
        new_employee = (Employee*)malloc(sizeof(Employee));
        if (new_employee == NULL) {
            fclose(file);
            return head;
        }

        new_employee->employee_id = employee_id;
        strncpy(new_employee->first_name, first_name, MAX_STRING_LENGTH - 1);
        strncpy(new_employee->last_name, last_name, MAX_STRING_LENGTH - 1);
        strncpy(new_employee->phone, phone, MAX_PHONE_LENGTH - 1);
        new_employee->function_id = function_id;
        new_employee->next = NULL;

        if (head == NULL) {
            head = new_employee;
        } else {
            Employee *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_employee;
        }
    }

    fclose(file);
    return head;
}

void free_employee_list(Employee *head) {
    Employee *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Function management functions
Function* add_function_at_beginning(Function *head) {
    Function *new_function = (Function*)malloc(sizeof(Function));
    if (new_function == NULL) {
        printf("Memory allocation error.\n");
        return head;
    }

    int function_exists;
    do {
        function_exists = 0;
        printf("Enter function ID: ");
        new_function->function_id = safe_int_input();
        
        Function *temp = head;
        while (temp != NULL) {
            if (new_function->function_id == temp->function_id) {
                printf("Function ID already exists. Please enter a different one.\n");
                function_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (function_exists);

    printf("Enter function name: ");
    scanf("%99s", new_function->function_name);
    printf("Enter salary: $");
    new_function->salary = safe_float_input();

    new_function->next = head;
    printf("Function added successfully.\n");
    return new_function;
}

Function* add_function_at_end(Function *head) {
    Function *new_function = (Function*)malloc(sizeof(Function));
    if (new_function == NULL) {
        printf("Memory allocation error. Cannot add function.\n");
        return head;
    }

    print_header("ADD NEW FUNCTION");

    int function_exists;
    do {
        function_exists = 0;
        printf("Enter function ID: ");
        new_function->function_id = safe_int_input();
        
        Function *temp = head;
        while (temp != NULL) {
            if (new_function->function_id == temp->function_id) {
                printf("Function ID %d already exists. Please enter a different one.\n", new_function->function_id);
                function_exists = 1;
                break;
            }
            temp = temp->next;
        }
    } while (function_exists);

    printf("Enter function name: ");
    scanf("%99s", new_function->function_name);
    printf("Enter salary: $");
    new_function->salary = safe_float_input();

    new_function->next = NULL;
    
    if (head == NULL) {
        printf("Function added successfully! This is your first function.\n");
        return new_function;
    } else {
        Function *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_function;
        printf("Function added successfully!\n");
        return head;
    }
}

void display_functions(Function *head) {
    if (head == NULL) {
        printf("  No functions found in the system.\n");
        printf("You need to add job functions first to view them.\n");
        printf("Please go to 'Function Management' -> 'Add New Function' to create your first function.\n");
        return;
    }

    print_header("FUNCTION DATABASE");
    
    // Count functions
    int function_count = 0;
    Function *temp = head;
    while (temp != NULL) {
        function_count++;
        temp = temp->next;
    }
    
    printf("Total functions in database: %d\n\n", function_count);
    
    set_console_color(2);
    printf("%-10s %-20s %-15s\n", "ID", "Function Name", "Salary ($)");
    printf("%-10s %-20s %-15s\n", "==", "=============", "==========");
    set_console_color(7);

    temp = head;
    while (temp != NULL) {
        printf("%-10d %-20s %-15.2f\n",
               temp->function_id,
               temp->function_name,
               temp->salary);
        temp = temp->next;
    }
}

Function* modify_function(Function *head) {
    if (head == NULL) {
        printf("  No functions found in the system.\n");
        printf("You need to add job functions first before modifying them.\n");
        printf("Please go to 'Function Management' -> 'Add New Function' to create your first function.\n");
        return head;
    }

    print_header("MODIFY FUNCTION");
    
    int function_id;
    printf("Enter function ID to modify: ");
    function_id = safe_int_input();

    Function *temp = head;
    while (temp != NULL && temp->function_id != function_id) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Function with ID %d not found.\n", function_id);
        printf("Tip: Check the function ID and try again.\n");
        return head;
    }

    // Show current function details
    printf("\nCurrent function details:\n");
    printf("ID: %d\n", temp->function_id);
    printf("Name: %s\n", temp->function_name);
    printf("Salary: $%.2f\n", temp->salary);
    
    printf("\nEnter new details:\n");
    printf("Enter new function name: ");
    scanf("%99s", temp->function_name);
    printf("Enter new salary: $");
    temp->salary = safe_float_input();

    printf("Function information updated successfully!\n");
    return head;
}

Function* delete_function(Function *head) {
    if (head == NULL) {
        printf("  No functions found in the system.\n");
        printf("You need to add job functions first before deleting them.\n");
        printf("Please go to 'Function Management' -> 'Add New Function' to create your first function.\n");
        return head;
    }

    print_header("DELETE FUNCTION");
    
    int function_id;
    printf("Enter function ID to delete: ");
    function_id = safe_int_input();

    Function *prev = NULL;
    Function *temp = head;
    while (temp != NULL && temp->function_id != function_id) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Function with ID %d not found.\n", function_id);
        printf("Tip: Check the function ID and try again.\n");
        return head;
    }

    // Show function details before deletion
    printf("\nFunction to be deleted:\n");
    printf("ID: %d\n", temp->function_id);
    printf("Name: %s\n", temp->function_name);
    printf("Salary: $%.2f\n", temp->salary);
    
    // Confirm deletion
    char confirm;
    printf("\nAre you sure you want to delete this function? (y/N): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        free(temp);
        printf("Function deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
    
    return head;
}

void search_function(Function *head) {
    if (head == NULL) {
        printf("  No functions found in the system.\n");
        printf("You need to add job functions first before searching for them.\n");
        printf("Please go to 'Function Management' -> 'Add New Function' to create your first function.\n");
        return;
    }

    print_header("SEARCH FUNCTION");
    
    int function_id;
    printf("Enter function ID to search: ");
    function_id = safe_int_input();

    Function *temp = head;
    while (temp != NULL && temp->function_id != function_id) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Function with ID %d not found.\n", function_id);
        printf("Tip: Check the function ID and try again.\n");
    } else {
        printf("\nFunction found!\n\n");
        set_console_color(2);
        printf("%-10s %-20s %-15s\n", "ID", "Function Name", "Salary ($)");
        printf("%-10s %-20s %-15s\n", "==", "=============", "==========");
        set_console_color(7);
        printf("%-10d %-20s %-15.2f\n",
               temp->function_id,
               temp->function_name,
               temp->salary);
    }
}

void save_functions_to_file(Function *head) {
    FILE *file = fopen(FUNCTION_FILENAME, "w");
    if (file == NULL) {
        return;
    }

    Function *temp = head;
    while (temp != NULL) {
        fprintf(file, "%d\n%s\n%.2f\n",
                temp->function_id,
                temp->function_name,
                temp->salary);
        temp = temp->next;
    }

    fclose(file);
}

Function* load_functions_from_file(Function *head) {
    FILE *file = fopen(FUNCTION_FILENAME, "r");
    if (file == NULL) {
        return head;
    }

    // Check if file is empty
    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        return head;
    }
    rewind(file);

    // Free existing list first
    free_function_list(head);
    head = NULL;

    Function *new_function;
    int function_id;
    char function_name[MAX_STRING_LENGTH];
    float salary;
    
    while (fscanf(file, "%d\n%99s\n%f\n", &function_id, function_name, &salary) == 3) {
        new_function = (Function*)malloc(sizeof(Function));
        if (new_function == NULL) {
            fclose(file);
            return head;
        }

        new_function->function_id = function_id;
        strncpy(new_function->function_name, function_name, MAX_STRING_LENGTH - 1);
        new_function->function_name[MAX_STRING_LENGTH - 1] = '\0';
        new_function->salary = salary;
        new_function->next = NULL;

        if (head == NULL) {
            head = new_function;
        } else {
            Function *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_function;
        }
    }

    fclose(file);
    return head;
}

void free_function_list(Function *head) {
    Function *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// DateTime validation functions
int is_valid_time(int hour, int minute) {
    if (hour < 0 || hour > 23) return 0;
    if (minute < 0 || minute > 59) return 0;
    return 1;
}

int is_valid_datetime(int day, int month, int year, int hour, int minute) {
    if (!is_valid_date(day, month, year)) return 0;
    if (!is_valid_time(hour, minute)) return 0;
    return 1;
}

// Trip management functions
Trip* add_trip_at_beginning(Trip *head, Bus *buses, Client *clients) {
    // Check prerequisites first
    if (!has_buses(buses)) {
        printf("  No buses available in the system.\n");
        printf("You need to add buses first before creating trips.\n");
        printf("Please go to 'Bus Management' -> 'Add New Bus' to add your first bus.\n");
        return head;
    }
    
    if (!has_clients(clients)) {
        printf("  No clients available in the system.\n");
        printf("You need to add clients first before creating trips.\n");
        printf("Please go to 'Client Management' -> 'Add New Client' to add your first client.\n");
        return head;
    }

    Trip *new_trip = (Trip*)malloc(sizeof(Trip));
    if (new_trip == NULL) {
        printf("Memory allocation error.\n");
        return head;
    }

    // Display available buses
    printf("Available buses:\n");
    display_buses(buses);
    printf("Enter bus license plate: ");
    new_trip->license_plate = safe_int_input();
    
    // Verify bus exists
    Bus *bus_temp = buses;
    int bus_found = 0;
    while (bus_temp != NULL) {
        if (bus_temp->license_plate == new_trip->license_plate) {
            bus_found = 1;
            break;
        }
        bus_temp = bus_temp->next;
    }
    
    if (!bus_found) {
        printf("Bus with license plate %d not found.\n", new_trip->license_plate);
        free(new_trip);
        return head;
    }

    // Display available clients
    printf("\nAvailable clients:\n");
    display_clients(clients);
    printf("Enter client ID: ");
    new_trip->client_id = safe_int_input();
    
    // Verify client exists
    Client *client_temp = clients;
    int client_found = 0;
    while (client_temp != NULL) {
        if (client_temp->client_id == new_trip->client_id) {
            client_found = 1;
            break;
        }
        client_temp = client_temp->next;
    }
    
    if (!client_found) {
        printf("Client with ID %d not found.\n", new_trip->client_id);
        free(new_trip);
        return head;
    }

    // Input departure details
    printf("\nEnter departure details:\n");
    printf("Departure city: ");
    scanf("%99s", new_trip->departure_city);
    
    DateTime departure;
    do {
        printf("Departure date and time:\n");
        printf("Year (1900-2100): ");
        departure.year = safe_int_input();
        printf("Month (1-12): ");
        departure.month = safe_int_input();
        printf("Day: ");
        departure.day = safe_int_input();
        printf("Hour (0-23): ");
        departure.hour = safe_int_input();
        printf("Minute (0-59): ");
        departure.minute = safe_int_input();
        
        if (!is_valid_datetime(departure.day, departure.month, departure.year, departure.hour, departure.minute)) {
            printf("Invalid date/time. Please enter valid values.\n");
        }
    } while (!is_valid_datetime(departure.day, departure.month, departure.year, departure.hour, departure.minute));

    new_trip->departure_time = departure;

    // Input arrival details
    printf("\nEnter arrival details:\n");
    printf("Arrival city: ");
    scanf("%99s", new_trip->arrival_city);
    
    DateTime arrival;
    do {
        printf("Arrival date and time:\n");
        printf("Year (1900-2100): ");
        arrival.year = safe_int_input();
        printf("Month (1-12): ");
        arrival.month = safe_int_input();
        printf("Day: ");
        arrival.day = safe_int_input();
        printf("Hour (0-23): ");
        arrival.hour = safe_int_input();
        printf("Minute (0-59): ");
        arrival.minute = safe_int_input();
        
        if (!is_valid_datetime(arrival.day, arrival.month, arrival.year, arrival.hour, arrival.minute)) {
            printf("Invalid date/time. Please enter valid values.\n");
        }
    } while (!is_valid_datetime(arrival.day, arrival.month, arrival.year, arrival.hour, arrival.minute));

    new_trip->arrival_time = arrival;

    new_trip->next = head;
    printf("Trip added successfully.\n");
    return new_trip;
}

Trip* add_trip_at_end(Trip *head, Bus *buses, Client *clients) {
    // Check prerequisites first
    if (!has_buses(buses)) {
        printf("  No buses available in the system.\n");
        printf("You need to add buses first before creating trips.\n");
        printf("Please go to 'Bus Management' -> 'Add New Bus' to add your first bus.\n");
        return head;
    }
    
    if (!has_clients(clients)) {
        printf("  No clients available in the system.\n");
        printf("You need to add clients first before creating trips.\n");
        printf("Please go to 'Client Management' -> 'Add New Client' to add your first client.\n");
        return head;
    }

    Trip *new_trip = (Trip*)malloc(sizeof(Trip));
    if (new_trip == NULL) {
        printf("Memory allocation error. Cannot add trip.\n");
        return head;
    }

    print_header("ADD NEW TRIP");

    // Display available buses
    printf("Available buses:\n");
    display_buses(buses);
    printf("Enter bus license plate: ");
    new_trip->license_plate = safe_int_input();
    
    // Verify bus exists
    Bus *bus_temp = buses;
    int bus_found = 0;
    while (bus_temp != NULL) {
        if (bus_temp->license_plate == new_trip->license_plate) {
            bus_found = 1;
            break;
        }
        bus_temp = bus_temp->next;
    }
    
    if (!bus_found) {
        printf("Bus with license plate %d not found.\n", new_trip->license_plate);
        free(new_trip);
        return head;
    }

    // Display available clients
    printf("\nAvailable clients:\n");
    display_clients(clients);
    printf("Enter client ID: ");
    new_trip->client_id = safe_int_input();
    
    // Verify client exists
    Client *client_temp = clients;
    int client_found = 0;
    while (client_temp != NULL) {
        if (client_temp->client_id == new_trip->client_id) {
            client_found = 1;
            break;
        }
        client_temp = client_temp->next;
    }
    
    if (!client_found) {
        printf("Client with ID %d not found.\n", new_trip->client_id);
        free(new_trip);
        return head;
    }

    // Input departure details
    printf("\nEnter departure details:\n");
    printf("Departure city: ");
    scanf("%99s", new_trip->departure_city);
    
    DateTime departure;
    do {
        printf("Departure date and time:\n");
        printf("Year (1900-2100): ");
        departure.year = safe_int_input();
        printf("Month (1-12): ");
        departure.month = safe_int_input();
        printf("Day: ");
        departure.day = safe_int_input();
        printf("Hour (0-23): ");
        departure.hour = safe_int_input();
        printf("Minute (0-59): ");
        departure.minute = safe_int_input();
        
        if (!is_valid_datetime(departure.day, departure.month, departure.year, departure.hour, departure.minute)) {
            printf("Invalid date/time. Please enter valid values.\n");
        }
    } while (!is_valid_datetime(departure.day, departure.month, departure.year, departure.hour, departure.minute));

    new_trip->departure_time = departure;

    // Input arrival details
    printf("\nEnter arrival details:\n");
    printf("Arrival city: ");
    scanf("%99s", new_trip->arrival_city);
    
    DateTime arrival;
    do {
        printf("Arrival date and time:\n");
        printf("Year (1900-2100): ");
        arrival.year = safe_int_input();
        printf("Month (1-12): ");
        arrival.month = safe_int_input();
        printf("Day: ");
        arrival.day = safe_int_input();
        printf("Hour (0-23): ");
        arrival.hour = safe_int_input();
        printf("Minute (0-59): ");
        arrival.minute = safe_int_input();
        
        if (!is_valid_datetime(arrival.day, arrival.month, arrival.year, arrival.hour, arrival.minute)) {
            printf("Invalid date/time. Please enter valid values.\n");
        }
    } while (!is_valid_datetime(arrival.day, arrival.month, arrival.year, arrival.hour, arrival.minute));

    new_trip->arrival_time = arrival;

    new_trip->next = NULL;
    
    if (head == NULL) {
        printf("Trip added successfully! This is your first trip.\n");
        return new_trip;
    } else {
        Trip *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_trip;
        printf("Trip added successfully!\n");
        return head;
    }
}

void display_trips(Trip *head, Bus *buses, Client *clients) {
    if (head == NULL) {
        printf("  No trips found in the system.\n");
        printf("You need to add trips first to view them.\n");
        printf("Please go to 'Trip Management' -> 'Add New Trip' to create your first trip.\n");
        return;
    }

    print_header("TRIP DATABASE");
    
    // Count trips
    int trip_count = 0;
    Trip *temp = head;
    while (temp != NULL) {
        trip_count++;
        temp = temp->next;
    }
    
    printf("Total trips in database: %d\n\n", trip_count);
    
    set_console_color(2);
    printf("%-10s %-10s %-15s %-15s %-20s %-20s\n", 
           "Bus", "Client", "Departure", "Arrival", "Departure Time", "Arrival Time");
    printf("%-10s %-10s %-15s %-15s %-20s %-20s\n", 
           "===", "======", "=========", "=======", "==============", "============");
    set_console_color(7);

    temp = head;
    while (temp != NULL) {
        printf("%-10d %-10d %-15s %-15s %02d/%02d/%d %02d:%02d     %02d/%02d/%d %02d:%02d\n",
               temp->license_plate,
               temp->client_id,
               temp->departure_city,
               temp->arrival_city,
               temp->departure_time.day,
               temp->departure_time.month,
               temp->departure_time.year,
               temp->departure_time.hour,
               temp->departure_time.minute,
               temp->arrival_time.day,
               temp->arrival_time.month,
               temp->arrival_time.year,
               temp->arrival_time.hour,
               temp->arrival_time.minute);
        temp = temp->next;
    }
}

Trip* modify_trip(Trip *head, Bus *buses, Client *clients) {
    if (head == NULL) {
        printf("  No trips found in the system.\n");
        printf("You need to add trips first before modifying them.\n");
        printf("Please go to 'Trip Management' -> 'Add New Trip' to create your first trip.\n");
        return head;
    }

    // Check prerequisites
    if (!has_buses(buses)) {
        printf("  No buses available in the system.\n");
        printf("You need buses available to modify trips.\n");
        return head;
    }
    
    if (!has_clients(clients)) {
        printf("  No clients available in the system.\n");
        printf("You need clients available to modify trips.\n");
        return head;
    }

    print_header("MODIFY TRIP");
    
    printf("Enter bus license plate of the trip to modify: ");
    int license_plate = safe_int_input();
    printf("Enter client ID of the trip to modify: ");
    int client_id = safe_int_input();

    Trip *temp = head;
    while (temp != NULL && (temp->license_plate != license_plate || temp->client_id != client_id)) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Trip not found.\n");
        return head;
    }

    printf("\nCurrent trip details:\n");
    printf("Bus: %d, Client: %d\n", temp->license_plate, temp->client_id);
    printf("Route: %s to %s\n", temp->departure_city, temp->arrival_city);
    printf("Departure: %02d/%02d/%d %02d:%02d\n", 
           temp->departure_time.day, temp->departure_time.month, temp->departure_time.year,
           temp->departure_time.hour, temp->departure_time.minute);
    printf("Arrival: %02d/%02d/%d %02d:%02d\n", 
           temp->arrival_time.day, temp->arrival_time.month, temp->arrival_time.year,
           temp->arrival_time.hour, temp->arrival_time.minute);

    printf("\nEnter new details:\n");
    printf("New departure city: ");
    scanf("%99s", temp->departure_city);
    printf("New arrival city: ");
    scanf("%99s", temp->arrival_city);

    // Update departure time
    DateTime departure;
    do {
        printf("New departure date and time:\n");
        printf("Year (1900-2100): ");
        departure.year = safe_int_input();
        printf("Month (1-12): ");
        departure.month = safe_int_input();
        printf("Day: ");
        departure.day = safe_int_input();
        printf("Hour (0-23): ");
        departure.hour = safe_int_input();
        printf("Minute (0-59): ");
        departure.minute = safe_int_input();
        
        if (!is_valid_datetime(departure.day, departure.month, departure.year, departure.hour, departure.minute)) {
            printf("Invalid date/time. Please enter valid values.\n");
        }
    } while (!is_valid_datetime(departure.day, departure.month, departure.year, departure.hour, departure.minute));

    temp->departure_time = departure;

    // Update arrival time
    DateTime arrival;
    do {
        printf("New arrival date and time:\n");
        printf("Year (1900-2100): ");
        arrival.year = safe_int_input();
        printf("Month (1-12): ");
        arrival.month = safe_int_input();
        printf("Day: ");
        arrival.day = safe_int_input();
        printf("Hour (0-23): ");
        arrival.hour = safe_int_input();
        printf("Minute (0-59): ");
        arrival.minute = safe_int_input();
        
        if (!is_valid_datetime(arrival.day, arrival.month, arrival.year, arrival.hour, arrival.minute)) {
            printf("Invalid date/time. Please enter valid values.\n");
        }
    } while (!is_valid_datetime(arrival.day, arrival.month, arrival.year, arrival.hour, arrival.minute));

    temp->arrival_time = arrival;

    printf("Trip information updated successfully!\n");
    return head;
}

Trip* delete_trip(Trip *head) {
    if (head == NULL) {
        printf("  No trips found in the system.\n");
        printf("You need to add trips first before deleting them.\n");
        printf("Please go to 'Trip Management' -> 'Add New Trip' to create your first trip.\n");
        return head;
    }

    print_header("DELETE TRIP");
    
    printf("Enter bus license plate of the trip to delete: ");
    int license_plate = safe_int_input();
    printf("Enter client ID of the trip to delete: ");
    int client_id = safe_int_input();

    Trip *prev = NULL;
    Trip *temp = head;
    while (temp != NULL && (temp->license_plate != license_plate || temp->client_id != client_id)) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Trip not found.\n");
        return head;
    }

   

    // Show trip details before deletion
    printf("\nTrip to be deleted:\n");
    printf("Bus: %d, Client: %d\n", temp->license_plate, temp->client_id);
    printf("Route: %s to %s\n", temp->departure_city, temp->arrival_city);
    printf("Departure: %02d/%02d/%d %02d:%02d\n", 
           temp->departure_time.day, temp->departure_time.month, temp->departure_time.year,
           temp->departure_time.hour, temp->departure_time.minute);
    printf("Arrival: %02d/%02d/%d %02d:%02d\n", 
           temp->arrival_time.day, temp->arrival_time.month, temp->arrival_time.year,
           temp->arrival_time.hour, temp->arrival_time.minute);
    
    // Confirm deletion
    char confirm;
    printf("\nAre you sure you want to delete this trip? (y/N): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        free(temp);
        printf("Trip deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
    
    return head;
}

void search_trip(Trip *head, Bus *buses, Client *clients) {
    if (head == NULL) {
        printf("  No trips found in the system.\n");
        printf("You need to add trips first before searching for them.\n");
        printf("Please go to 'Trip Management' -> 'Add New Trip' to create your first trip.\n");
        return;
    }

    print_header("SEARCH TRIP");
    
    printf("Enter bus license plate to search: ");
    int license_plate = safe_int_input();
    printf("Enter client ID to search: ");
    int client_id = safe_int_input();

    Trip *temp = head;
    while (temp != NULL && (temp->license_plate != license_plate || temp->client_id != client_id)) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Trip not found.\n");
    } else {
        printf("\nTrip found!\n\n");
        set_console_color(2);
        printf("%-10s %-10s %-15s %-15s %-20s %-20s\n", 
               "Bus", "Client", "Departure", "Arrival", "Departure Time", "Arrival Time");
        printf("%-10s %-10s %-15s %-15s %-20s %-20s\n", 
               "===", "======", "=========", "=======", "==============", "============");
        set_console_color(7);
        printf("%-10d %-10d %-15s %-15s %02d/%02d/%d %02d:%02d   %02d/%02d/%d %02d:%02d\n",
               temp->license_plate,
               temp->client_id,
               temp->departure_city,
               temp->arrival_city,
               temp->departure_time.day,
               temp->departure_time.month,
               temp->departure_time.year,
               temp->departure_time.hour,
               temp->departure_time.minute,
               temp->arrival_time.day,
               temp->arrival_time.month,
               temp->arrival_time.year,
               temp->arrival_time.hour,
               temp->arrival_time.minute);
    }
}

void save_trips_to_file(Trip *head) {
    FILE *file = fopen(TRIP_FILENAME, "w");
    if (file == NULL) {
        return;
    }

    Trip *temp = head;
    while (temp != NULL) {
        fprintf(file, "%d\n%d\n%d/%d/%d %d:%d\n%d/%d/%d %d:%d\n%s\n%s\n",
                temp->license_plate,
                temp->client_id,
                temp->departure_time.day,
                temp->departure_time.month,
                temp->departure_time.year,
                temp->departure_time.hour,
                temp->departure_time.minute,
                temp->arrival_time.day,
                temp->arrival_time.month,
                temp->arrival_time.year,
                temp->arrival_time.hour,
                temp->arrival_time.minute,
                temp->departure_city,
                temp->arrival_city);
        temp = temp->next;
    }

    fclose(file);
}

Trip* load_trips_from_file(Trip *head) {
    FILE *file = fopen(TRIP_FILENAME, "r");
    if (file == NULL) {
        return head;
    }

    // Check if file is empty
    fseek(file, 0L, SEEK_END);
    if (ftell(file) == 0) {
        fclose(file);
        return head;
    }
    rewind(file);

    // Free existing list first
    free_trip_list(head);
    head = NULL;

    Trip *new_trip;
    int license_plate, client_id;
    int dep_day, dep_month, dep_year, dep_hour, dep_minute;
    int arr_day, arr_month, arr_year, arr_hour, arr_minute;
    char departure_city[MAX_STRING_LENGTH], arrival_city[MAX_STRING_LENGTH];
    
    while (fscanf(file, "%d\n%d\n%d/%d/%d %d:%d\n%d/%d/%d %d:%d\n%99s\n%99s\n",
                  &license_plate, &client_id,
                  &dep_day, &dep_month, &dep_year, &dep_hour, &dep_minute,
                  &arr_day, &arr_month, &arr_year, &arr_hour, &arr_minute,
                  departure_city, arrival_city) == 14) {
        new_trip = (Trip*)malloc(sizeof(Trip));
        if (new_trip == NULL) {
            fclose(file);
            return head;
        }

        new_trip->license_plate = license_plate;
        new_trip->client_id = client_id;
        new_trip->departure_time.day = dep_day;
        new_trip->departure_time.month = dep_month;
        new_trip->departure_time.year = dep_year;
        new_trip->departure_time.hour = dep_hour;
        new_trip->departure_time.minute = dep_minute;
        new_trip->arrival_time.day = arr_day;
        new_trip->arrival_time.month = arr_month;
        new_trip->arrival_time.year = arr_year;
        new_trip->arrival_time.hour = arr_hour;
        new_trip->arrival_time.minute = arr_minute;
        strncpy(new_trip->departure_city, departure_city, MAX_STRING_LENGTH - 1);
        strncpy(new_trip->arrival_city, arrival_city, MAX_STRING_LENGTH - 1);
        new_trip->departure_city[MAX_STRING_LENGTH - 1] = '\0';
        new_trip->arrival_city[MAX_STRING_LENGTH - 1] = '\0';
        new_trip->next = NULL;

        if (head == NULL) {
            head = new_trip;
        } else {
            Trip *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_trip;
        }
    }

    fclose(file);
    return head;
}

void free_trip_list(Trip *head) {
    Trip *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

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
                    // printf("Loading system data...\n");
                    buses = load_buses_from_file(buses);
                    clients = load_clients_from_file(clients);
                    employees = load_employees_from_file(employees);
                    functions = load_functions_from_file(functions);
                    trips = load_trips_from_file(trips);
                    // printf("System ready!\n");
                    // pause_screen();
                    
                    main_menu(&buses, &clients, &employees, &functions, &trips);
                } else {
                    printf("\nInvalid username or password. Please try again.\n");
                }
                break;
            case 2:
                register_user(users, &num_users);
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
        // printf("Saving system data...\n");
        save_buses_to_file(buses);
        save_clients_to_file(clients);
        save_employees_to_file(employees);
        save_functions_to_file(functions);
        save_trips_to_file(trips);
        // printf("Data saved successfully!\n");
    }
    
    // Free memory
    free_bus_list(buses);
    free_client_list(clients);
    free_employee_list(employees);
    free_function_list(functions);
    free_trip_list(trips);
    
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
        printf("6. Save All Data\n");
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
                employee_menu(*buses, *clients, employees, *functions, *trips);
                break;
            case 4:
                function_menu(*buses, *clients, *employees, functions, *trips);
                break;
            case 5:
                trip_menu(*buses, *clients, *employees, *functions, trips);
                break;
            case 6:
                printf("Saving all data...\n");
                save_buses_to_file(*buses);
                save_clients_to_file(*clients);
                save_employees_to_file(*employees);
                save_functions_to_file(*functions);
                save_trips_to_file(*trips);
                printf("All data saved successfully!\n");
                break;
            case 0:
                printf("\nLogging out...\n");
                // // Auto-save before logout
                // printf("Auto-saving data...\n");
                save_buses_to_file(*buses);
                save_clients_to_file(*clients);
                save_employees_to_file(*employees);
                save_functions_to_file(*functions);
                save_trips_to_file(*trips);
                // printf("Data saved successfully!\n");
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

void employee_menu(Bus *buses, Client *clients, Employee **employees, Function *functions, Trip *trips) {
    int choice;
    do {
        print_header("EMPLOYEE MANAGEMENT");
        
        // Show current employee count
        int employee_count = 0;
        Employee *temp = *employees;
        while (temp != NULL) {
            employee_count++;
            temp = temp->next;
        }
        printf("Current employees in system: %d\n\n", employee_count);
        
        set_console_color(2);
        printf("1. Add New Employee\n");
        printf("2. View All Employees\n");
        printf("3. Modify Employee Information\n");
        printf("4. Delete Employee\n");
        printf("5. Search Employee\n");
        printf("6. Save Employees to File\n");
        printf("7. Reload Employees from File\n");
        printf("0. Back to Main Menu\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                *employees = add_employee_at_end(*employees, functions);
                break;
            case 2:
                display_employees(*employees, functions);
                break;
            case 3:
                *employees = modify_employee(*employees, functions);
                break;
            case 4:
                *employees = delete_employee(*employees);
                break;
            case 5:
                search_employee(*employees, functions);
                break;
            case 6:
                save_employees_to_file(*employees);
                break;
            case 7:
                *employees = load_employees_from_file(*employees);
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

void function_menu(Bus *buses, Client *clients, Employee *employees, Function **functions, Trip *trips) {
    int choice;
    do {
        print_header("FUNCTION MANAGEMENT");
        
        // Show current function count
        int function_count = 0;
        Function *temp = *functions;
        while (temp != NULL) {
            function_count++;
            temp = temp->next;
        }
        printf("Current functions in system: %d\n\n", function_count);
        
        set_console_color(2);
        printf("1. Add New Function\n");
        printf("2. View All Functions\n");
        printf("3. Modify Function Information\n");
        printf("4. Delete Function\n");
        printf("5. Search Function\n");
        printf("6. Save Functions to File\n");
        printf("7. Reload Functions from File\n");
        printf("0. Back to Main Menu\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                *functions = add_function_at_end(*functions);
                break;
            case 2:
                display_functions(*functions);
                break;
            case 3:
                *functions = modify_function(*functions);
                break;
            case 4:
                *functions = delete_function(*functions);
                break;
            case 5:
                search_function(*functions);
                break;
            case 6:
                save_functions_to_file(*functions);
                break;
            case 7:
                *functions = load_functions_from_file(*functions);
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

void trip_menu(Bus *buses, Client *clients, Employee *employees, Function *functions, Trip **trips) {
    int choice;
    do {
        print_header("TRIP MANAGEMENT");
        
        // Show current trip count
        int trip_count = 0;
        Trip *temp = *trips;
        while (temp != NULL) {
            trip_count++;
            temp = temp->next;
        }
        printf("Current trips in system: %d\n\n", trip_count);
        
        set_console_color(2);
        printf("1. Add New Trip\n");
        printf("2. View All Trips\n");
        printf("3. Modify Trip Information\n");
        printf("4. Delete Trip\n");
        printf("5. Search Trip\n");
        printf("6. Save Trips to File\n");
        printf("7. Reload Trips from File\n");
        printf("0. Back to Main Menu\n");
        set_console_color(7);
        printf("\nEnter your choice: ");
        choice = safe_int_input();
        
        switch (choice) {
            case 1:
                *trips = add_trip_at_end(*trips, buses, clients);
                break;
            case 2:
                display_trips(*trips, buses, clients);
                break;
            case 3:
                *trips = modify_trip(*trips, buses, clients);
                break;
            case 4:
                *trips = delete_trip(*trips);
                break;
            case 5:
                search_trip(*trips, buses, clients);
                break;
            case 6:
                save_trips_to_file(*trips);
                break;
            case 7:
                *trips = load_trips_from_file(*trips);
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

// Utility functions to check if data exists
int has_buses(Bus *head) {
    return head != NULL;
}

int has_clients(Client *head) {
    return head != NULL;
}

int has_functions(Function *head) {
    return head != NULL;
}
