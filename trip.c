// ----- The Bus Trip -----

#include <stdio.h>  // I/O
#include <stdlib.h>  // Memory
#include <string.h>  // String Operations
#include <time.h>  // Date/Time

// Maximum limits
#define MAX_TRIPS 100
#define MAX_TICKETS 500
#define MAX_SEATS 50
#define MAX_STRING 100

// Trip structure
typedef struct {
    int tripID;
    char departurePoint[MAX_STRING];
    char arrivalPoint[MAX_STRING];
    char tripDate[MAX_STRING];
    char departureTime[MAX_STRING];
    char busLicensePlate[MAX_STRING];
    char driverName[MAX_STRING];
    int totalSeats;
    int availableSeats;
    float ticketPrice;
    int isActive; // 1 if trip exists, 0 if deleted
} Trip;

// Passenger structure
typedef struct {
    char fullName[MAX_STRING];
    char idNumber[MAX_STRING];
    char phoneNumber[MAX_STRING];
    char email[MAX_STRING];
} Passenger;

// Ticket structure
typedef struct {
    int ticketID;
    int tripID;
    int seatNumber;
    Passenger passenger;
    float price;
    char purchaseDate[MAX_STRING];
    int isActive; // 1 if ticket is valid, 0 if cancelled
} Ticket;

// Global arrays to store data
Trip trips[MAX_TRIPS];
Ticket tickets[MAX_TICKETS];
int tripCount = 0;
int ticketCount = 0;

// Function prototypes
void displayMenu();
void createTrip();
void updateTrip();
void deleteTrip();
void inquireTrip();
void listAllTrips();
void sellTicket();
void cancelTicket();
void createReceipt(int ticketID);
void saveTripsToFile();
void loadTripsFromFile();
void saveTicketsToFile();
void loadTicketsFromFile();
int findTripByID(int tripID);
int findTicketByID(int ticketID);
void clearInputBuffer();
void getCurrentDateTime(char *buffer, size_t);

int main() {
    int choice;
    
    // Load existing data from files
    loadTripsFromFile();
    loadTicketsFromFile();
    
    printf("========================================\n");
    printf("   BUS TICKETING SYSTEM\n");
    printf("========================================\n\n");
    
    while (1) {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1: createTrip(); break;
            case 2: updateTrip(); break;
            case 3: deleteTrip(); break;
            case 4: inquireTrip(); break;
            case 5: listAllTrips(); break;
            case 6: sellTicket(); break;
            case 7: cancelTicket(); break;
            case 0:
                printf("\nSaving data and exiting...\n");
                saveTripsToFile();
                saveTicketsToFile();
                printf("Thank you for using Bus Ticketing System!\n");
                return 0;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
        
        printf("\nPress Enter to continue...");
        getchar();
    }
    
    return 0;
}

// Display main menu
void displayMenu() {
    printf("\n========================================\n");
    printf("              MAIN MENU\n");
    printf("========================================\n");
    printf("1. Create New Trip\n");
    printf("2. Update Trip\n");
    printf("3. Delete Trip\n");
    printf("4. Trip Inquiry\n");
    printf("5. List All Trips\n");
    printf("6. Sell Ticket\n");
    printf("7. Cancel Ticket\n");
    printf("0. Exit\n");
    printf("========================================\n");
}

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Get current date and time
void getCurrentDateTime(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm t;

    if (localtime_r(&now, &t) == NULL) {
        strncpy(buffer, "00/00/0000 00:00:00", size);
        buffer[size - 1] = '\0';
        return;
    }

    snprintf(buffer, size,
             "%02d/%02d/%04d %02d:%02d:%02d",
             t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
             t.tm_hour, t.tm_min, t.tm_sec);
}

// Save all trips to file
void saveTripsToFile() {
    FILE *file = fopen("trips.txt", "w");
    if (file == NULL) {
        printf("Error: Could not open trips file for writing!\n");
        return;
    }
    
    // Write trip count
    fprintf(file, "%d\n", tripCount);
    
    // Write all trips
    for (int i = 0; i < tripCount; i++) {
        fprintf(file, "%d|%s|%s|%s|%s|%s|%s|%d|%d|%.2f|%d\n",
                trips[i].tripID,
                trips[i].departurePoint,
                trips[i].arrivalPoint,
                trips[i].tripDate,
                trips[i].departureTime,
                trips[i].busLicensePlate,
                trips[i].driverName,
                trips[i].totalSeats,
                trips[i].availableSeats,
                trips[i].ticketPrice,
                trips[i].isActive);
    }
    
    fclose(file);
}

// Load all trips from file
void loadTripsFromFile() {
    FILE *file = fopen("trips.txt", "r");
    if (file == NULL) {
        // File doesn't exist yet, this is normal on first run
        tripCount = 0;
        return;
    }
    
    // Read trip count
    fscanf(file, "%d\n", &tripCount);
    
    // Read all trips
    for (int i = 0; i < tripCount; i++) {
        fscanf(file, "%d|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%d|%d|%f|%d\n",
               &trips[i].tripID,
               trips[i].departurePoint,
               trips[i].arrivalPoint,
               trips[i].tripDate,
               trips[i].departureTime,
               trips[i].busLicensePlate,
               trips[i].driverName,
               &trips[i].totalSeats,
               &trips[i].availableSeats,
               &trips[i].ticketPrice,
               &trips[i].isActive);
    }
    
    fclose(file);
    printf("Loaded %d trips from file.\n", tripCount);
}

// Save all tickets to file
void saveTicketsToFile() {
    FILE *file = fopen("tickets.txt", "w");
    if (file == NULL) {
        printf("Error: Could not open tickets file for writing!\n");
        return;
    }
    
    // Write ticket count
    fprintf(file, "%d\n", ticketCount);
    
    // Write all tickets
    for (int i = 0; i < ticketCount; i++) {
        fprintf(file, "%d|%d|%d|%s|%s|%s|%s|%.2f|%s|%d\n",
                tickets[i].ticketID,
                tickets[i].tripID,
                tickets[i].seatNumber,
                tickets[i].passenger.fullName,
                tickets[i].passenger.idNumber,
                tickets[i].passenger.phoneNumber,
                tickets[i].passenger.email,
                tickets[i].price,
                tickets[i].purchaseDate,
                tickets[i].isActive);
    }
    
    fclose(file);
}

// Load all tickets from file
void loadTicketsFromFile() {
    FILE *file = fopen("tickets.txt", "r");
    if (file == NULL) {
        // File doesn't exist yet, this is normal on first run
        ticketCount = 0;
        return;
    }
    
    // Read ticket count
    fscanf(file, "%d\n", &ticketCount);
    
    // Read all tickets
    for (int i = 0; i < ticketCount; i++) {
        fscanf(file, "%d|%d|%d|%99[^|]|%99[^|]|%99[^|]|%99[^|]|%f|%99[^|]|%d\n",
               &tickets[i].ticketID,
               &tickets[i].tripID,
               &tickets[i].seatNumber,
               tickets[i].passenger.fullName,
               tickets[i].passenger.idNumber,
               tickets[i].passenger.phoneNumber,
               tickets[i].passenger.email,
               &tickets[i].price,
               tickets[i].purchaseDate,
               &tickets[i].isActive);
    }
    
    fclose(file);
    printf("Loaded %d tickets from file.\n", ticketCount);
}

// Find trip by ID (returns index, -1 if not found)
int findTripByID(int tripID) {
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].tripID == tripID && trips[i].isActive == 1) {
            return i;
        }
    }
    return -1;
}

// Find ticket by ID (returns index, -1 if not found)
int findTicketByID(int ticketID) {
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].ticketID == ticketID && tickets[i].isActive == 1) {
            return i;
        }
    }
    return -1;
}

// Create a new trip
void createTrip() {
    printf("\n========================================\n");
    printf("         CREATE NEW TRIP\n");
    printf("========================================\n");
    
    // Check if we have space for more trips
    if (tripCount >= MAX_TRIPS) {
        printf("Error: Maximum trip limit reached!\n");
        return;
    }
    
    Trip newTrip;
    
    // Get Trip ID
    printf("Enter Trip ID: ");
    scanf("%d", &newTrip.tripID);
    clearInputBuffer();
    
    // Check if Trip ID already exists
    if (findTripByID(newTrip.tripID) != -1) {
        printf("Error: A trip with ID %d already exists!\n", newTrip.tripID);
        return;
    }
    
    // Get Departure Point
    printf("Enter Departure Point: ");
    fgets(newTrip.departurePoint, MAX_STRING, stdin);
    newTrip.departurePoint[strcspn(newTrip.departurePoint, "\n")] = 0; // Remove newline
    
    // Get Arrival Point
    printf("Enter Arrival Point: ");
    fgets(newTrip.arrivalPoint, MAX_STRING, stdin);
    newTrip.arrivalPoint[strcspn(newTrip.arrivalPoint, "\n")] = 0;
    
    // Get Trip Date
    printf("Enter Trip Date (DD/MM/YYYY): ");
    fgets(newTrip.tripDate, MAX_STRING, stdin);
    newTrip.tripDate[strcspn(newTrip.tripDate, "\n")] = 0;
    
    // Get Departure Time
    printf("Enter Departure Time (HH:MM): ");
    fgets(newTrip.departureTime, MAX_STRING, stdin);
    newTrip.departureTime[strcspn(newTrip.departureTime, "\n")] = 0;
    
    // Get Bus License Plate
    printf("Enter Bus License Plate: ");
    fgets(newTrip.busLicensePlate, MAX_STRING, stdin);
    newTrip.busLicensePlate[strcspn(newTrip.busLicensePlate, "\n")] = 0;
    
    // Get Driver's Full Name
    printf("Enter Driver's Full Name: ");
    fgets(newTrip.driverName, MAX_STRING, stdin);
    newTrip.driverName[strcspn(newTrip.driverName, "\n")] = 0;
    
    // Get Number of Seats
    printf("Enter Number of Seats: ");
    scanf("%d", &newTrip.totalSeats);
    clearInputBuffer();
    
    // Validate seat count
    if (newTrip.totalSeats <= 0 || newTrip.totalSeats > MAX_SEATS) {
        printf("Error: Invalid number of seats! Must be between 1 and %d.\n", MAX_SEATS);
        return;
    }
    
    // Get Ticket Price
    printf("Enter Ticket Price: ");
    scanf("%f", &newTrip.ticketPrice);
    clearInputBuffer();
    
    // Validate price
    if (newTrip.ticketPrice <= 0) {
        printf("Error: Ticket price must be greater than 0!\n");
        return;
    }
    
    // Set initial values
    newTrip.availableSeats = newTrip.totalSeats; // All seats available initially
    newTrip.isActive = 1; // Trip is active
    
    // Add trip to array
    trips[tripCount] = newTrip;
    tripCount++;
    
    // Save to file
    saveTripsToFile();
    
    printf("\n✓ Trip created successfully!\n");
    printf("Trip ID: %d\n", newTrip.tripID);
    printf("Route: %s -> %s\n", newTrip.departurePoint, newTrip.arrivalPoint);
    printf("Date: %s at %s\n", newTrip.tripDate, newTrip.departureTime);
}

// Update an existing trip
void updateTrip() {
    printf("\n========================================\n");
    printf("           UPDATE TRIP\n");
    printf("========================================\n");
    
    int tripID;
    printf("Enter Trip ID to update: ");
    scanf("%d", &tripID);
    clearInputBuffer();
    
    // Find the trip
    int index = findTripByID(tripID);
    if (index == -1) {
        printf("Error: Trip with ID %d not found!\n", tripID);
        return;
    }

    // Show warning if any ticket has sold
    if (trips[index].availableSeats != trips[index].totalSeats) {
        char confirm;
        printf("Warning: Some tickets has sold before for this trip.\nThis could cause some confusion.\nDo you still want to change this information: (Y/N) ");
        scanf("%c", &confirm);
        if (confirm == 'N' || confirm == 'n') {
            return;
        }
    }
    
    // Display current trip information
    printf("\n--- Current Trip Information ---\n");
    printf("Trip ID: %d\n", trips[index].tripID);
    printf("Departure Point: %s\n", trips[index].departurePoint);
    printf("Arrival Point: %s\n", trips[index].arrivalPoint);
    printf("Trip Date: %s\n", trips[index].tripDate);
    printf("Departure Time: %s\n", trips[index].departureTime);
    printf("Bus License Plate: %s\n", trips[index].busLicensePlate);
    printf("Driver Name: %s\n", trips[index].driverName);
    printf("Total Seats: %d\n", trips[index].totalSeats);
    printf("Available Seats: %d\n", trips[index].availableSeats);
    printf("Ticket Price: %.2f TL\n", trips[index].ticketPrice);
    
    while (1) {
    // Update menu
    int choice;
    printf("\n--- What would you like to update? ---\n");
    printf("1. Departure Point\n");
    printf("2. Arrival Point\n");
    printf("3. Trip Date\n");
    printf("4. Departure Time\n");
    printf("5. Bus License Plate\n");
    printf("6. Driver Name\n");
    printf("7. Total Seats\n");
    printf("8. Ticket Price\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    clearInputBuffer();
    
    switch(choice) {
        case 1:
            printf("Enter new Departure Point: ");
            fgets(trips[index].departurePoint, MAX_STRING, stdin);
            trips[index].departurePoint[strcspn(trips[index].departurePoint, "\n")] = 0;
            break;
            
        case 2:
            printf("Enter new Arrival Point: ");
            fgets(trips[index].arrivalPoint, MAX_STRING, stdin);
            trips[index].arrivalPoint[strcspn(trips[index].arrivalPoint, "\n")] = 0;
            break;
            
        case 3:
            printf("Enter new Trip Date (DD/MM/YYYY): ");
            fgets(trips[index].tripDate, MAX_STRING, stdin);
            trips[index].tripDate[strcspn(trips[index].tripDate, "\n")] = 0;
            break;
            
        case 4:
            printf("Enter new Departure Time (HH:MM): ");
            fgets(trips[index].departureTime, MAX_STRING, stdin);
            trips[index].departureTime[strcspn(trips[index].departureTime, "\n")] = 0;
            break;
            
        case 5:
            printf("Enter new Bus License Plate: ");
            fgets(trips[index].busLicensePlate, MAX_STRING, stdin);
            trips[index].busLicensePlate[strcspn(trips[index].busLicensePlate, "\n")] = 0;
            break;
            
        case 6:
            printf("Enter new Driver Name: ");
            fgets(trips[index].driverName, MAX_STRING, stdin);
            trips[index].driverName[strcspn(trips[index].driverName, "\n")] = 0;
            break;
            
        case 7:
            printf("Enter new Total Seats: ");
            int newSeats;
            scanf("%d", &newSeats);
            clearInputBuffer();
            
            if (newSeats <= 0 || newSeats > MAX_SEATS) {
                printf("Error: Invalid number of seats!\n");
                break;
            }
            
            // Check if new seat count is less than sold tickets
            int soldSeats = trips[index].totalSeats - trips[index].availableSeats;
            if (newSeats < soldSeats) {
                printf("Error: Cannot reduce seats below %d (already sold tickets)!\n", soldSeats);
                break;
            }
            
            // Update available seats proportionally
            trips[index].availableSeats = newSeats - soldSeats;
            trips[index].totalSeats = newSeats;
            break;
            
        case 8:
            printf("Enter new Ticket Price: ");
            float newPrice;
            scanf("%f", &newPrice);
            clearInputBuffer();
            
            if (newPrice <= 0) {
                printf("Error: Price must be greater than 0!\n");
                break;
            }
            trips[index].ticketPrice = newPrice;
            break;
            
        case 0:
            printf("Exiting.\n");
            return;
            
        default:
            printf("\n\t[!] Invalid choice!\n");
            break;
        }
    }
    
    // Save changes to file
    saveTripsToFile();
    
    printf("\n✓ Trip updated successfully!\n");
}

// Delete a trip
void deleteTrip() {
    printf("\n========================================\n");
    printf("           DELETE TRIP\n");
    printf("========================================\n");
    
    int tripID;
    printf("Enter Trip ID to delete: ");
    scanf("%d", &tripID);
    clearInputBuffer();
    
    // Find the trip
    int index = findTripByID(tripID);
    if (index == -1) {
        printf("Error: Trip with ID %d not found!\n", tripID);
        return;
    }
    
    // Display trip information
    printf("\n--- Trip Information ---\n");
    printf("Trip ID: %d\n", trips[index].tripID);
    printf("Route: %s -> %s\n", trips[index].departurePoint, trips[index].arrivalPoint);
    printf("Date: %s at %s\n", trips[index].tripDate, trips[index].departureTime);
    printf("Driver: %s\n", trips[index].driverName);
    printf("Available Seats: %d/%d\n", trips[index].availableSeats, trips[index].totalSeats);
    
    // Check if there are sold tickets for this trip
    int soldTickets = trips[index].totalSeats - trips[index].availableSeats;
    if (soldTickets > 0) {
        printf("\nWarning: This trip has %d sold ticket(s)!\n", soldTickets);
        printf("Deleting this trip will also cancel all related tickets.\n");
    }
    
    // Confirmation
    char confirm;
    printf("\nAre you sure you want to delete this trip? (Y/N): ");
    scanf(" %c", &confirm);
    clearInputBuffer();
    
    if (confirm == 'Y' || confirm == 'y') {
        // Mark trip as inactive (soft delete)
        trips[index].isActive = 0;
        
        // Cancel all tickets for this trip
        int cancelledCount = 0;
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].tripID == tripID && tickets[i].isActive == 1) {
                tickets[i].isActive = 0;
                cancelledCount++;
            }
        }
        
        // Save changes
        saveTripsToFile();
        saveTicketsToFile();
        
        printf("\n✓ Trip deleted successfully!\n");
        if (cancelledCount > 0) {
            printf("✓ %d ticket(s) have been cancelled.\n", cancelledCount);
        }
    } else {
        printf("\nDeletion cancelled.\n");
    }
}

// Inquire trip details
void inquireTrip() {
    printf("\n========================================\n");
    printf("         TRIP INQUIRY\n");
    printf("========================================\n");
    
    int tripID;
    printf("Enter Trip ID to inquire: ");
    scanf("%d", &tripID);
    clearInputBuffer();
    
    // Find the trip
    int index = findTripByID(tripID);
    if (index == -1) {
        printf("Error: Trip with ID %d not found!\n", tripID);
        return;
    }
    
    // Display detailed trip information
    printf("\n========================================\n");
    printf("       TRIP DETAILS - ID: %d\n", trips[index].tripID);
    printf("========================================\n\n");
    
    printf("Route Information:\n");
    printf("  Departure Point : %s\n", trips[index].departurePoint);
    printf("  Arrival Point   : %s\n", trips[index].arrivalPoint);
    printf("  Trip Date       : %s\n", trips[index].tripDate);
    printf("  Departure Time  : %s\n", trips[index].departureTime);
    
    printf("\nBus Information:\n");
    printf("  License Plate   : %s\n", trips[index].busLicensePlate);
    printf("  Driver Name     : %s\n", trips[index].driverName);
    
    printf("\nSeat Information:\n");
    printf("  Total Seats     : %d\n", trips[index].totalSeats);
    printf("  Available Seats : %d\n", trips[index].availableSeats);
    printf("  Sold Seats      : %d\n", trips[index].totalSeats - trips[index].availableSeats);
    printf("  Occupancy Rate  : %.1f%%\n", 
           ((float)(trips[index].totalSeats - trips[index].availableSeats) / trips[index].totalSeats) * 100);
    
    printf("\nPricing Information:\n");
    printf("  Ticket Price    : %.2f TL\n", trips[index].ticketPrice);
    
    // Show passengers list if there are sold tickets
    int soldSeats = trips[index].totalSeats - trips[index].availableSeats;
    if (soldSeats > 0) {
        printf("\nPassenger List:\n");
        printf("  %-5s %-25s %-15s %-15s\n", "Seat", "Passenger Name", "ID Number", "Phone");
        printf("  %s\n", "------------------------------------------------------------------------");
        
        int passengerCount = 0;
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].tripID == tripID && tickets[i].isActive == 1) {
                printf("  %-5d %-25s %-15s %-15s\n", 
                       tickets[i].seatNumber,
                       tickets[i].passenger.fullName,
                       tickets[i].passenger.idNumber,
                       tickets[i].passenger.phoneNumber);
                passengerCount++;
            }
        }
        printf("  %s\n", "------------------------------------------------------------------------");
        printf("  Total Passengers: %d\n", passengerCount);
    } else {
        printf("\n  No tickets sold for this trip yet.\n");
    }
    
    printf("\n========================================\n");
}

// List all trips
void listAllTrips() {
    printf("\n========================================\n");
    printf("         ALL TRIPS LIST\n");
    printf("========================================\n");
    
    // Count active trips
    int activeTrips = 0;
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].isActive == 1) {
            activeTrips++;
        }
    }
    
    // Check if there are any trips
    if (activeTrips == 0) {
        printf("\nNo trips found in the system.\n");
        return;
    }
    
    printf("\nTotal Active Trips: %d\n\n", activeTrips);
    
    // Table header
    printf("%-6s %-15s %-15s %-12s %-8s %-10s %-5s %-10s\n",
           "ID", "From", "To", "Date", "Time", "Price", "Seats", "Status");
    printf("%-6s %-15s %-15s %-12s %-8s %-10s %-5s %-10s\n",
           "------", "---------------", "---------------", "------------", 
           "--------", "----------", "-----", "----------");
    
    // Display all active trips
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].isActive == 1) {
            // Calculate status
            char status[20];
            int soldSeats = trips[i].totalSeats - trips[i].availableSeats;
            float occupancy = ((float)soldSeats / trips[i].totalSeats) * 100;
            
            if (trips[i].availableSeats == 0) {strcpy(status, "FULL");
            } else if (occupancy >= 80) {strcpy(status, "Almost Full");
            } else if (occupancy >= 50) {strcpy(status, "Half Full");
            } else {strcpy(status, "Available");
            }
            
            // Truncate long city names for table format
            char fromCity[16], toCity[16];
            strncpy(fromCity, trips[i].departurePoint, 15);
            fromCity[15] = '\0';
            strncpy(toCity, trips[i].arrivalPoint, 15);
            toCity[15] = '\0';
            
            printf("%-6d %-15s %-15s %-12s %-8s %-10.2f %d/%-3d %-10s\n",
                   trips[i].tripID,
                   fromCity,
                   toCity,
                   trips[i].tripDate,
                   trips[i].departureTime,
                   trips[i].ticketPrice,
                   trips[i].availableSeats,
                   trips[i].totalSeats,
                   status);
        }
    }
    
    printf("\n========================================\n");
    
    // Summary statistics
    int totalSeats = 0;
    int totalAvailable = 0;
    float totalRevenue = 0;
    
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].isActive == 1) {
            totalSeats += trips[i].totalSeats;
            totalAvailable += trips[i].availableSeats;
            
            // Calculate revenue from sold tickets
            int soldSeats = trips[i].totalSeats - trips[i].availableSeats;
            totalRevenue += soldSeats * trips[i].ticketPrice;
        }
    }
    
    printf("\nSummary:\n");
    printf("  Total Capacity    : %d seats\n", totalSeats);
    printf("  Available Seats   : %d seats\n", totalAvailable);
    printf("  Sold Seats        : %d seats\n", totalSeats - totalAvailable);
    printf("  Overall Occupancy : %.1f%%\n", 
           totalSeats > 0 ? ((float)(totalSeats - totalAvailable) / totalSeats) * 100 : 0);
    printf("  Total Revenue     : %.2f TL\n", totalRevenue);
    printf("\n========================================\n");
}

// Sell a ticket
void sellTicket() {
    printf("\n========================================\n");
    printf("           SELL TICKET\n");
    printf("========================================\n");
    
    // Check if we have space for more tickets
    if (ticketCount >= MAX_TICKETS) {
        printf("Error: Maximum ticket limit reached!\n");
        return;
    }
    
    int tripID;
    printf("Enter Trip ID: ");
    scanf("%d", &tripID);
    clearInputBuffer();
    
    // Find the trip
    int tripIndex = findTripByID(tripID);
    if (tripIndex == -1) {
        printf("Error: Trip with ID %d not found!\n", tripID);
        return;
    }
    
    // Check if seats are available
    if (trips[tripIndex].availableSeats <= 0) {
        printf("Error: No available seats for this trip!\n");
        return;
    }
    
    // Display trip information
    printf("\n--- Trip Information ---\n");
    printf("Route: %s -> %s\n", trips[tripIndex].departurePoint, trips[tripIndex].arrivalPoint);
    printf("Date: %s at %s\n", trips[tripIndex].tripDate, trips[tripIndex].departureTime);
    printf("Available Seats: %d/%d\n", trips[tripIndex].availableSeats, trips[tripIndex].totalSeats);
    printf("Price: %.2f TL\n", trips[tripIndex].ticketPrice);
    
    // Show occupied seats
    printf("\nOccupied Seats: ");
    int hasOccupied = 0;
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].tripID == tripID && tickets[i].isActive == 1) {
            printf("%d ", tickets[i].seatNumber);
            hasOccupied = 1;
        }
    }
    if (!hasOccupied) {
        printf("None (All seats available)");
    }
    printf("\n\n");
    
    // Create new ticket
    Ticket newTicket;
    
    // Generate unique ticket ID
    newTicket.ticketID = ticketCount + 1;
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].ticketID >= newTicket.ticketID) {
            newTicket.ticketID = tickets[i].ticketID + 1;
        }
    }
    
    newTicket.tripID = tripID;
    
    // Get seat number
    int seatNumber;
    int validSeat = 0;
    while (!validSeat) {
        printf("Enter Seat Number (1-%d): ", trips[tripIndex].totalSeats);
        scanf("%d", &seatNumber);
        clearInputBuffer();
        
        // Validate seat number
        if (seatNumber < 1 || seatNumber > trips[tripIndex].totalSeats) {
            printf("Error: Invalid seat number! Please choose between 1 and %d.\n", trips[tripIndex].totalSeats);
            continue;
        }
        
        // Check if seat is already occupied
        int isOccupied = 0;
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].tripID == tripID && 
                tickets[i].seatNumber == seatNumber && 
                tickets[i].isActive == 1) {
                isOccupied = 1;
                break;
            }
        }
        
        if (isOccupied) {
            printf("Error: Seat %d is already occupied! Please choose another seat.\n", seatNumber);
        } else {
            validSeat = 1;
            newTicket.seatNumber = seatNumber;
        }
    }
    
    // Get passenger information
    printf("\n--- Passenger Information ---\n");
    
    printf("Full Name: ");
    fgets(newTicket.passenger.fullName, MAX_STRING, stdin);
    newTicket.passenger.fullName[strcspn(newTicket.passenger.fullName, "\n")] = 0;
    
    printf("ID Number (TC Kimlik): ");
    fgets(newTicket.passenger.idNumber, MAX_STRING, stdin);
    newTicket.passenger.idNumber[strcspn(newTicket.passenger.idNumber, "\n")] = 0;
    
    printf("Phone Number: ");
    fgets(newTicket.passenger.phoneNumber, MAX_STRING, stdin);
    newTicket.passenger.phoneNumber[strcspn(newTicket.passenger.phoneNumber, "\n")] = 0;
    
    printf("Email: ");
    fgets(newTicket.passenger.email, MAX_STRING, stdin);
    newTicket.passenger.email[strcspn(newTicket.passenger.email, "\n")] = 0;
    
    // Set price and date
    newTicket.price = trips[tripIndex].ticketPrice;
    getCurrentDateTime(newTicket.purchaseDate, MAX_STRING);
    newTicket.isActive = 1;
    
    // Add ticket to array
    tickets[ticketCount] = newTicket;
    ticketCount++;
    
    // Update trip available seats
    trips[tripIndex].availableSeats--;
    
    // Save to files
    saveTicketsToFile();
    saveTripsToFile();
    
    printf("\n✓ Ticket sold successfully!\n");
    printf("Ticket ID: %d\n", newTicket.ticketID);
    printf("Passenger: %s\n", newTicket.passenger.fullName);
    printf("Seat Number: %d\n", newTicket.seatNumber);
    printf("Price: %.2f TL\n", newTicket.price);
    
    // Ask if user wants to create receipt
    char createReceiptChoice;
    printf("\nWould you like to create a receipt? (Y/N): ");
    scanf(" %c", &createReceiptChoice);
    clearInputBuffer();
    
    if (createReceiptChoice == 'Y' || createReceiptChoice == 'y') {
        createReceipt(newTicket.ticketID);
    }
}

// Cancel a ticket
void cancelTicket() {
    printf("\n========================================\n");
    printf("         CANCEL TICKET\n");
    printf("========================================\n");
    
    int ticketID;
    printf("Enter Ticket ID to cancel: ");
    scanf("%d", &ticketID);
    clearInputBuffer();
    
    // Find the ticket
    int ticketIndex = findTicketByID(ticketID);
    if (ticketIndex == -1) {
        printf("Error: Ticket with ID %d not found!\n", ticketID);
        return;
    }
    
    // Find the related trip
    int tripIndex = findTripByID(tickets[ticketIndex].tripID);
    if (tripIndex == -1) {
        printf("Error: Related trip not found!\n");
        return;
    }
    
    // Display ticket information
    printf("\n--- Ticket Information ---\n");
    printf("Ticket ID       : %d\n", tickets[ticketIndex].ticketID);
    printf("Trip ID         : %d\n", tickets[ticketIndex].tripID);
    printf("Route           : %s -> %s\n", 
           trips[tripIndex].departurePoint, 
           trips[tripIndex].arrivalPoint);
    printf("Date            : %s at %s\n", 
           trips[tripIndex].tripDate, 
           trips[tripIndex].departureTime);
    printf("Seat Number     : %d\n", tickets[ticketIndex].seatNumber);
    printf("Passenger Name  : %s\n", tickets[ticketIndex].passenger.fullName);
    printf("ID Number       : %s\n", tickets[ticketIndex].passenger.idNumber);
    printf("Phone           : %s\n", tickets[ticketIndex].passenger.phoneNumber);
    printf("Price           : %.2f TL\n", tickets[ticketIndex].price);
    printf("Purchase Date   : %s\n", tickets[ticketIndex].purchaseDate);
    
    // Confirmation
    char confirm;
    printf("\nAre you sure you want to cancel this ticket? (Y/N): ");
    scanf(" %c", &confirm);
    clearInputBuffer();
    
    if (confirm == 'Y' || confirm == 'y') {
        // Mark ticket as inactive (cancelled)
        tickets[ticketIndex].isActive = 0;
        
        // Increase available seats for the trip
        trips[tripIndex].availableSeats++;
        
        // Save changes
        saveTicketsToFile();
        saveTripsToFile();
        
        printf("\n✓ Ticket cancelled successfully!\n");
        printf("Seat %d is now available for Trip ID %d.\n", 
               tickets[ticketIndex].seatNumber, 
               tickets[ticketIndex].tripID);
        printf("Refund amount: %.2f TL\n", tickets[ticketIndex].price);
    } else {
        printf("\nCancellation aborted.\n");
    }
}

// Create a receipt for ticket
void createReceipt(int ticketID) {
    // Find the ticket
    int ticketIndex = findTicketByID(ticketID);
    if (ticketIndex == -1) {
        printf("Error: Ticket with ID %d not found!\n", ticketID);
        return;
    }
    
    // Find the related trip
    int tripIndex = findTripByID(tickets[ticketIndex].tripID);
    if (tripIndex == -1) {
        printf("Error: Related trip not found!\n");
        return;
    }
    
    // Create filename
    char filename[MAX_STRING];
    sprintf(filename, "receipt_ticket_%d.txt", ticketID);
    
    // Open file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not create receipt file!\n");
        return;
    }
    
    // Write receipt header
    fprintf(file, "========================================\n");
    fprintf(file, "          BUS TICKET RECEIPT\n");
    fprintf(file, "========================================\n\n");
    
    // Ticket information
    fprintf(file, "TICKET INFORMATION\n");
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "Ticket ID        : %d\n", tickets[ticketIndex].ticketID);
    fprintf(file, "Purchase Date    : %s\n", tickets[ticketIndex].purchaseDate);
    fprintf(file, "Status           : %s\n", tickets[ticketIndex].isActive ? "ACTIVE" : "CANCELLED");
    fprintf(file, "\n");
    
    // Trip information
    fprintf(file, "TRIP INFORMATION\n");
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "Trip ID          : %d\n", trips[tripIndex].tripID);
    fprintf(file, "Departure Point  : %s\n", trips[tripIndex].departurePoint);
    fprintf(file, "Arrival Point    : %s\n", trips[tripIndex].arrivalPoint);
    fprintf(file, "Trip Date        : %s\n", trips[tripIndex].tripDate);
    fprintf(file, "Departure Time   : %s\n", trips[tripIndex].departureTime);
    fprintf(file, "Seat Number      : %d\n", tickets[ticketIndex].seatNumber);
    fprintf(file, "\n");
    
    // Bus information
    fprintf(file, "BUS & DRIVER INFORMATION\n");
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "Bus License Plate: %s\n", trips[tripIndex].busLicensePlate);
    fprintf(file, "Driver Name      : %s\n", trips[tripIndex].driverName);
    fprintf(file, "\n");
    
    // Passenger information
    fprintf(file, "PASSENGER INFORMATION\n");
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "Full Name        : %s\n", tickets[ticketIndex].passenger.fullName);
    fprintf(file, "ID Number        : %s\n", tickets[ticketIndex].passenger.idNumber);
    fprintf(file, "Phone Number     : %s\n", tickets[ticketIndex].passenger.phoneNumber);
    fprintf(file, "Email            : %s\n", tickets[ticketIndex].passenger.email);
    fprintf(file, "\n");
    
    // Payment information
    fprintf(file, "PAYMENT INFORMATION\n");
    fprintf(file, "----------------------------------------\n");
    fprintf(file, "Ticket Price     : %.2f TL\n", tickets[ticketIndex].price);
    fprintf(file, "Tax (18%%)        : %.2f TL\n", tickets[ticketIndex].price * 0.18);
    fprintf(file, "Total Amount     : %.2f TL\n", tickets[ticketIndex].price * 1.18);
    fprintf(file, "\n");
    
    // Footer
    fprintf(file, "========================================\n");
    fprintf(file, "     Thank you for choosing us!\n");
    fprintf(file, "     Have a safe journey!\n");
    fprintf(file, "========================================\n\n");
    
    // Important notices
    fprintf(file, "IMPORTANT NOTICES:\n");
    fprintf(file, "- Please arrive at the departure point\n");
    fprintf(file, "  at least 30 minutes before departure.\n");
    fprintf(file, "- This ticket is non-transferable.\n");
    fprintf(file, "- Please bring your ID for verification.\n");
    fprintf(file, "- Keep this receipt for your records.\n");
    fprintf(file, "\n");
    
    // Contact information
    fprintf(file, "For questions and support:\n");
    fprintf(file, "Phone: +90 (212) 555-0000\n");
    fprintf(file, "Email: support@busticket.com\n");
    fprintf(file, "Website: www.busticket.com\n");
    fprintf(file, "\n");
    
    // Barcode simulation (just a simple pattern)
    fprintf(file, "========================================\n");
    fprintf(file, "BARCODE: |||| || ||| | |||| | ||| ||||\n");
    fprintf(file, "         %010d\n", ticketID);
    fprintf(file, "========================================\n");
    
    fclose(file);
    
    printf("\n✓ Receipt created successfully!\n");
    printf("File saved as: %s\n", filename);
}
