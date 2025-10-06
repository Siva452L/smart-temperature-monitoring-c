#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FILENAME 50

typedef struct TempNode {
    int id;
    float temperature;
    char timestamp[20];
    struct TempNode *next;
} TempNode;

TempNode *head = NULL;
float highThreshold = 50.0; // default high limit
float lowThreshold = 0.0;   // default low limit
int nextId = 1;

// Function to get current timestamp
void getTimestamp(char *buffer, int size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%02d-%02d-%04d %02d:%02d:%02d",
             tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Add a new temperature reading
void addReading(float temp) {
    TempNode *newNode = (TempNode*)malloc(sizeof(TempNode));
    newNode->id = nextId++;
    newNode->temperature = temp;
    getTimestamp(newNode->timestamp, sizeof(newNode->timestamp));
    newNode->next = head;
    head = newNode;

    // Alert check
    if (temp > highThreshold)
        printf("ALERT: Temperature %.2f°C exceeds high threshold %.2f°C!\n", temp, highThreshold);
    else if (temp < lowThreshold)
        printf("ALERT: Temperature %.2f°C below low threshold %.2f°C!\n", temp, lowThreshold);
}

// Display all readings
void displayReadings() {
    if (!head) {
        printf("No temperature readings recorded.\n");
        return;
    }
    printf("\n--- Temperature History ---\n");
    TempNode *temp = head;
    while (temp) {
        printf("ID: %d | Temp: %.2f°C | Time: %s\n", temp->id, temp->temperature, temp->timestamp);
        temp = temp->next;
    }
}

// Set high and low thresholds
void setThresholds(float low, float high) {
    lowThreshold = low;
    highThreshold = high;
    printf("Thresholds updated: Low = %.2f°C, High = %.2f°C\n", lowThreshold, highThreshold);
}

// Save readings to file
void saveToFile(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file for writing.\n");
        return;
    }
    TempNode *temp = head;
    while (temp) {
        fprintf(fp, "%d,%.2f,%s\n", temp->id, temp->temperature, temp->timestamp);
        temp = temp->next;
    }
    fclose(fp);
    printf("Data saved to %s successfully.\n", filename);
}

// Load readings from file
void loadFromFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return;
    int id;
    float tempVal;
    char timestamp[20];
    while (fscanf(fp, "%d,%f,%19[^\n]\n", &id, &tempVal, timestamp) == 3) {
        TempNode *newNode = (TempNode*)malloc(sizeof(TempNode));
        newNode->id = id;
        newNode->temperature = tempVal;
        strncpy(newNode->timestamp, timestamp, sizeof(newNode->timestamp));
        newNode->next = head;
        head = newNode;
        if (id >= nextId) nextId = id + 1;
    }
    fclose(fp);
}

// Free linked list memory
void freeList() {
    TempNode *temp = head;
    while (temp) {
        TempNode *next = temp->next;
        free(temp);
        temp = next;
    }
    head = NULL;
}

int main() {
    char filename[MAX_FILENAME] = "temp_data.txt";
    loadFromFile(filename);

    int choice;
    float temp, low, high;

    while (1) {
        printf("\n--- Smart Temperature Monitoring System ---\n");
        printf("1. Add Temperature Reading\n");
        printf("2. Display All Readings\n");
        printf("3. Set Temperature Thresholds\n");
        printf("4. Save & Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter Temperature (°C): ");
                scanf("%f", &temp);
                addReading(temp);
                break;
            case 2:
                displayReadings();
                break;
            case 3:
                printf("Enter Low Threshold: ");
                scanf("%f", &low);
                printf("Enter High Threshold: ");
                scanf("%f", &high);
                setThresholds(low, high);
                break;
            case 4:
                saveToFile(filename);
                freeList();
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
