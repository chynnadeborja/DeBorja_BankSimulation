#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#define MAX 1000

using namespace std;

struct Account {
    int accountNumber;
    string accountName;
    string contactNum, birthday;
    double balance;
    string pinCode;
};

string encryptPIN(string pin, int shift) {
    string encryptedPIN = pin;
    for (char &c : encryptedPIN) {
        if (isdigit(c)) {
            c = ((c - '0') + shift) % 10 + '0';
        }
    }
    return encryptedPIN;
}

string decryptPIN(string pin, int shift) {
    string decryptedPIN = pin;
    for (char &c : decryptedPIN) {
        if (isdigit(c)) {
            c = ((c - '0') - shift + 10) % 10 + '0';
        }
    }
    return decryptedPIN;
}

class ATM {
private:
    Account acc[MAX];
    int account = 0;

public:
    void createAcc();
    void balance();
    void withdraw();
    void deposit();
    void fundTransfer();
    void changePin();
    void transactionMode();
    void compareAccountInfo();
    int checker();
    void loadAccounts();
    void saveAccounts();
};

void ATM::loadAccounts() {
    ifstream inputFile("E:\ATMSIMULATION.txt");
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            if (line.find("Account number: ") != string::npos) {
                int accountNumber;
                string accountName, contactNum, birthday, pinCode;
                double balance;

                accountNumber = stoi(line.substr(line.find(":") + 2));
                getline(inputFile, line);
                accountName = line.substr(line.find(":") + 2);
                getline(inputFile, line);
                contactNum = line.substr(line.find(":") + 2);
                getline(inputFile, line);
                birthday = line.substr(line.find(":") + 2);
                getline(inputFile, line);
                balance = stod(line.substr(line.find(":") + 2));
                getline(inputFile, line);
                pinCode = decryptPIN(line.substr(line.find(":") + 2), 3);


                acc[account] = {accountNumber, accountName, contactNum, birthday, balance, pinCode};
                account++;
            }
        }
        inputFile.close();
    }
}

void ATM::saveAccounts() {
    ofstream outputFile("E:\ATMSIMULATION.txt");
    if (outputFile.is_open()) {
        for (int i = 0; i < account; i++) {
            outputFile << "Account number: " << acc[i].accountNumber << endl;
            outputFile << "Name: " << acc[i].accountName << endl;
            outputFile << "Contact number: " << acc[i].contactNum << endl; // Swap order here
            outputFile << "Birthday: " << acc[i].birthday << endl;
            outputFile << "Balance: " << acc[i].balance << endl;
            outputFile << "Pin Code: " << acc[i].pinCode << endl;
            outputFile << "Encrypted Pin Code: " << encryptPIN(acc[i].pinCode, 3) << endl;
            outputFile << "Decrypted Pin Code: " << decryptPIN(acc[i].pinCode, 3) << endl;
            outputFile << "-----------------------------------" << endl;
        }
        outputFile.close();
    } else {
        cerr << "Failed to open the file for writing." << endl;
    }
}


void ATM::createAcc() {
    int accountNumber;
    string accountName, birthday, contactNum, pinCode;
    double initialBalance;

    if (account < MAX) {
        // Seed the random number generator with the current time
        srand(time(0));

        // Generate a random 5-digit account number
        accountNumber = rand() % 90000 + 10000;

        cout << "Enter account name: ";
        cin.ignore();
        getline(cin, accountName);

        cout << "Enter contact number: ";
        cin >> contactNum;

        cout << "Enter birthday (YYYY-MM-DD): ";
        cin >> birthday;

        while (true) {
            cout << "Enter initial balance (Minimum 5000): ";
            cin >> initialBalance;

            if (initialBalance >= 5000) {
                break;
            } else {
                cout << "Initial balance must be equal or greater than 5000. Try again." << endl;
            }
        }

        cin.ignore(); // Move cin.ignore() here

        cout << "Enter PIN (4 digits): ";
        char c;
        pinCode = "";
        while (true) {
            c = _getch();
            if (c == 13) { // Enter key pressed
                if (pinCode.size() == 4) { // Check if the PIN is 4 digits
                    pinCode = encryptPIN(pinCode, 3); // Encrypt the PIN with a shift of 3
                    break;
                } else {
                    cout << "\nPlease enter a 4-digit PIN." << endl;
                }
            } else if (c == 8) {
                if (!pinCode.empty()) {
                    cout << "\b \b";
                    pinCode.pop_back();
                }
            } else if (c >= '0' && c <= '9') {
                cout << '*';
                pinCode += c;
            }
        }

        acc[account] = { accountNumber, accountName, contactNum, birthday, initialBalance, pinCode };
        account++;
        cout << "\nAccount successfully created.\n";
        saveAccounts();
    } else {
        cout << "Cannot create more accounts. Maximum limit reached.\n";
    }
}

void ATM::withdraw() {
    int withdrawAmount;
    cout << "Enter the amount to withdraw: ";
    cin >> withdrawAmount;

    if (withdrawAmount > 0 && (acc[account - 1].balance - withdrawAmount) >= 5000) {
        acc[account - 1].balance -= withdrawAmount;
        cout << "Withdrawal successful. New balance: " << acc[account - 1].balance << endl;
        system("pause");
        saveAccounts();
    } else if ((acc[account - 1].balance - withdrawAmount) < 5000) {
        cout << "Transaction can't be processed. (Minimum balance: 5000)" << endl;
        system("pause");
    } else {
        cout << "Invalid withdrawal amount." << endl;
        system("pause");
    }
}

void ATM::deposit() {
    int depositAmount;
    cout << "Enter the amount to deposit: ";
    cin >> depositAmount;

    if (depositAmount > 0) {
        acc[account - 1].balance += depositAmount;
        cout << "Deposit successful. New balance: " << acc[account - 1].balance << endl;
        system("pause");
        saveAccounts();
    } else {
        cout << "Invalid deposit amount." << endl;
        system("pause");
    }
}

void ATM::balance() {
    if (account > 0) {
        cout << "Account Balance: " << acc[account - 1].balance << endl;
        system("pause");
    } else {
        cout << "No account is currently logged in. Please log in before checking the balance." << endl;
        system("pause");
    }
}

void ATM::fundTransfer() {
    if (account < 2) {
        cout << "You need at least two accounts to perform a fund transfer." << endl;
        return;
    }

    int senderAccountNumber, receiverAccountNumber;
    double transferAmount;

    cout << "Enter your account number: ";
    cin >> senderAccountNumber;

    // Find the sender's account index
    int senderIndex = -1;
    for (int i = 0; i < account; i++) {
        if (acc[i].accountNumber == senderAccountNumber) {
            senderIndex = i;
            break;
        }
    }

    if (senderIndex == -1) {
        cout << "Sender account not found." << endl;
        return;
    }

    cout << "Enter the recipient's account number: ";
    cin >> receiverAccountNumber;

    // Find the receiver's account index
    int receiverIndex = -1;
    for (int i = 0; i < account; i++) {
        if (acc[i].accountNumber == receiverAccountNumber) {
            receiverIndex = i;
            break;
        }
    }

    if (receiverIndex == -1) {
        cout << "Receiver account not found." << endl;
        return;
    }

    cout << "Enter the amount to transfer: ";
    cin >> transferAmount;

    if (transferAmount <= 0 || transferAmount > acc[senderIndex].balance) {
        cout << "Invalid transfer amount or insufficient balance." << endl;
    } else {
        acc[senderIndex].balance -= transferAmount;
        acc[receiverIndex].balance += transferAmount;
        cout << "Fund transfer successful." << endl;

        // Update account information in the file
        saveAccounts();
    }
}


void ATM::changePin() {
    int accountNumber;
    string currentPin, newPin;

    cout << "Enter your account number: ";
    cin >> accountNumber;

    // Find the account index
    int accountIndex = -1;
    for (int i = 0; i < account; i++) {
        if (acc[i].accountNumber == accountNumber) {
            accountIndex = i;
            break;
        }
    }

    if (accountIndex == -1) {
        cout << "Account not found." << endl;
        return;
    }

    int attempts = 3; // Number of attempts allowed
    bool correctPin = false;

    while (attempts > 0 && !correctPin) {
        cout << "Enter your current PIN: ";
        cin.ignore();
        char c;
        currentPin = "";
        while (true) {
            c = _getch();
            if (c == 13) // Enter key pressed
                break;
            else if (c == 8) { // Backspace key pressed
                if (!currentPin.empty()) {
                    cout << "\b \b"; // Move the cursor back, erase character, move cursor back
                    currentPin.pop_back();
                }
            } else if (c >= '0' && c <= '9') {
                cout << '*';
                currentPin += c;
            }
        }

        if (currentPin == acc[accountIndex].pinCode) {
            correctPin = true;
        } else {
            cout << "\nIncorrect current PIN. Attempts left: " << attempts - 1 << endl;
            attempts--;
        }
    }

    if (!correctPin) {
        cout << "\nToo many incorrect attempts. Returning to main menu." << endl;
        return;
    }

    cout << "\nEnter your new PIN: ";
    newPin = "";
    while (true) {
        char c = _getch();
        if (c == 13) // Enter key pressed
            break;
        else if (c == 8) { // Backspace key pressed
            if (!newPin.empty()) {
                cout << "\b \b"; // Move the cursor back, erase character, move cursor back
                newPin.pop_back();
            }
        } else if (c >= '0' && c <= '9') {
            cout << '*';
            newPin += c;
        }
    }

    acc[accountIndex].pinCode = encryptPIN(newPin, 3); // Encrypt the new PIN before saving
    cout << "\nPIN changed successfully." << endl;

    // Update account information in the file
    saveAccounts();
}



int ATM::checker() {
    ifstream file;
    int p;
    bool driveOpened = false;
    do {
        file.open ("E:\ATMSIMULATION.txt");
;        if (file.is_open()) {
            driveOpened = true;
        } else {
            cout << "\n\n\t\t\t\t\tPLEASE INSERT YOUR ATM CARD";
            driveOpened = false;
        }
        file >> p >> p;
        file.close();
    } while (!driveOpened);
    return 0;
}

void ATM::compareAccountInfo() {
    bool accountFound = false;
    bool exitFlag = false; // Flag to check if the user wants to exit

    while (!accountFound && !exitFlag) {
        int inputAccountNumber;
        string inputPinCode;
        cout << "Enter account number: ";
        cin >> inputAccountNumber;
        cout << "Enter PIN: ";
        cin.ignore(); // Clear any previous newline character
        char c;
        inputPinCode = "";
        while (true) {
            c = _getch(); // Read a character without displaying it
            if (c == 13) // Enter key pressed
                break;
            else if (c == 8) { // Backspace key pressed
                if (!inputPinCode.empty()) {
                    cout << "\b \b"; // Move the cursor back, erase character, move cursor back
                    inputPinCode.pop_back();
                }
            } else if (c >= '0' && c <= '9') { // Valid PIN character
                cout << '*'; // Display an asterisk
                inputPinCode += c;
            }
        }

        ifstream inputFile("E:\\ATMSIMULATION.txt");
        if (!inputFile.is_open()) {
            cerr << "Error: Unable to open the file." << endl;
            // Handle the error, maybe exit the program or return an error code.
        }

        bool found = false;
        string line;

        while (getline(inputFile, line)) {
            if (line.find("Account number: ") != string::npos) {
                int fileAccountNumber;
                string accountName, birthday, contactNum;
                double initialBalance;
                string filePinCode;

                // Extract the account number
                fileAccountNumber = stoi(line.substr(line.find(":") + 2));

                // Read other information
                getline(inputFile, line); // Read the name line
                accountName = line.substr(line.find(":") + 2);

                getline(inputFile, line); // Read the birthday line
                contactNum = line.substr(line.find(":") + 2);

                getline(inputFile, line); // Read the birthday line
                birthday = line.substr(line.find(":") + 2);

                getline(inputFile, line); // Read the balance line
                initialBalance = stod(line.substr(line.find(":") + 2));

                getline(inputFile, line); // Read the PIN line
                filePinCode = decryptPIN(line.substr(line.find(":") + 2), 3); // Decrypt the PIN with a shift of 3

                // Check if the entered PIN matches the stored PIN
                if (inputAccountNumber == fileAccountNumber && inputPinCode == filePinCode) {
                    found = true;
                    cout << "\nAccount found:\n";
                    cout << "You can start your transaction\n";
                    system("pause");
                    accountFound = true;
                    break;
                }
            }
        }

        inputFile.close();

        if (!found) {
            cout << "\nAccount not found or incorrect PIN." << endl;
            cout << "Press '1' to try again, '2' to quit: ";
            char choice;
            cin >> choice;

            if (choice == '2') {
                exitFlag = true; // Set the exit flag if the user chooses to quit
            }
        }
    }

    if (accountFound) {
        transactionMode();
    }
}


void ATM::transactionMode() {
    int choice;
    while (true)
     {
        system("cls");
        cout << "\n\n\t\t\t\t\tTRANSACTION MODE";
        cout << "\n\n\t\t\t\t\t1. WITHDRAW";
        cout << "\n\n\t\t\t\t\t2. CHECK BALANCE";
        cout << "\n\n\t\t\t\t\t3. DEPOSIT";
        cout << "\n\n\t\t\t\t\t4. FUND TRANSFER";
        cout << "\n\n\t\t\t\t\t5. CHANGE PIN";
        cout << "\n\n\t\t\t\t\t6. EXIT";

        cout << "\n\n\t\t\t\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                system("cls");
                withdraw();
                break;
            case 2:
                system("cls");
                balance();
                break;
            case 3:
                system("cls");
                deposit();
                break;
            case 4:
                fundTransfer();
                break;
            case 5:
                changePin();
                break;
            case 6:
                exit(0);
                break;

            default:
                cout << "\n\n\t\t\t\t\tInvalid choice. Please select a valid option.";
        }
     }
}

int main() {
    ATM atm;
    int choice;
    atm.loadAccounts();

    while (true) {
        system("cls");
        cout << "\n\n\t\t\t\t\tMenu:\n";
        cout << "\n\n\t\t\t\t\t1. Create Account\n";
        cout << "\n\n\t\t\t\t\t2. Transaction Mode\n";
        cout << "\n\n\t\t\t\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                atm.checker();
                system("cls");
                atm.createAcc();
                system("pause");
                break;
            case 2:
                atm.checker();
                system("cls");
                atm.compareAccountInfo();
                break;
            default:
                cout << "Invalid choice. Please select a valid option." << endl;
        }
    }
}
