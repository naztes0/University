
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include<iomanip>
#include<cstring>
#include<stdio.h>

#define STRMAX 100

using namespace std;
struct tDate {
    int day;
    int month;
    int year;

    bool isValidDate() const
    {
        if (month < 1 || month > 12)
            return false;
        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)))
            daysInMonth[2] = 29;
        if (day < 1 || day > daysInMonth[month])
            return false;
        if (year < 1900 || year > 2100)
            return false;
        return true;
    }
};
struct tServe
{
    char fname[STRMAX];
    char lname[STRMAX];
    char otch[STRMAX];

    char city[STRMAX];
    char country[STRMAX];
    tDate birthday;
    
};


void addPerson()
{   
    tServe person;
    cout << "Enter first name: ";
    cin >> person.fname;
    cout << "Enter last name: ";
    cin >> person.lname;
    cout << "Enter second lastname: ";
    cin >> person.otch;
    cout << "===== Address of Person ===== \n";
    cout << "Enter city: ";
    cin >> person.city;
    cout << "Enter a coutnry: ";
    cin >> person.country;
    cout << "Enter birth date (example: 06 11 2005)";
    
    do {
        cin >> person.birthday.day >> person.birthday.month >> person.birthday.year;
        
    } while (!person.birthday.isValidDate());
    FILE* f = fopen("database.bin", "a+");
    fwrite(&person, sizeof(tServe), 1, f);
    fclose(f);

    cout << "\nPerson successfully added\n";

}
void outputHeader()
{
    cout << setw(16) << "Name|";
    cout << setw(16) << "Last name|";
    cout << setw(16) << "Otch|";
    cout << setw(21) << "City|";
    cout << setw(16) << "Country|";
    cout << setw(19) << "Date|";
    cout << '\n';
    for (int i = 0; i < 104; i++) {
        cout << '-';
    }
    cout << '\n';
}
void outputPerson(tServe const& person)
{
    cout << setw(15) << person.fname << '|';
    cout << setw(15) << person.lname << '|';
    cout << setw(15) << person.otch << '|';
    cout << setw(20) << person.city << '|';
    cout << setw(15) << person.country << '|';
    if (person.birthday.day > 9)
        cout << setw(10) << person.birthday.day << "\\";
    else
        cout << setw(9) << 0 << person.birthday.day << "\\";
    if (person.birthday.month > 9)
        cout << person.birthday.month << "\\";
    else
        cout << 0 << person.birthday.month << "\\";
    cout << person.birthday.year;

    cout << '\n';
    

}
void outputData()
{
    FILE* f = fopen("database.bin", "a+");
    if (f == NULL)
    {
        cout << "Data base is empty!\n";
    }
    else
    {
        outputHeader();
        tServe person;
        while (fread(&person, sizeof(tServe), 1, f) == 1)
        {
            outputPerson(person);
        }
    }
    fclose(f);
}

void searchPerson()
{
    char fname[STRMAX];
    char lname[STRMAX];
    cout << "Enter first name: ";
    cin >> fname;
    cout << "Enter last name: ";
    cin >> lname;

    bool writeHeader = false;

    tServe person;
    FILE* f = fopen("database.bin", "a+");
    if (f == NULL)
    {
        cout << "Base is empty!\n";
    }
    else
    {
        while (fread(&person, sizeof(tServe), 1, f) == 1)
        {
            if (strcmp(person.fname, fname) == 0 && strcmp(person.lname, lname) == 0) 
            {
                if (!writeHeader)
                {
                    outputHeader();
                    writeHeader = true;
                }
                outputPerson(person);
            }
           
        }
        if (!writeHeader) cout << "No matching!!!\n";
    }

}

void zvitPerson(int choice)
{
    char str[STRMAX];
    int digit;
    bool writeHeader = false;

    tServe person;
    FILE* f = fopen("database.bin", "a+");
    if (f == NULL)
    {
        cout << "Base is empty!\n";
    }
    else
    {
        switch(choice){
            case 1: {
                cout << "Enter name: ";
                cin >> str;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (strcmp(person.fname, str) == 0)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 2 :
            {
                cout << "Enter last name: ";
                cin >> str;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (strcmp(person.lname, str) == 0)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 3:
            {
                cout << "Enter second last name: ";
                cin >> str;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (strcmp(person.otch, str) == 0)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 4:
            {
                cout << "Enter city: ";
                cin >> str;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (strcmp(person.city, str) == 0)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 5:
            {
                cout << "Enter country: ";
                cin >> str;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (strcmp(person.country, str) == 0)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 6:
            {
                cout << "Enter day of birth: ";
                cin >> digit;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (person.birthday.day==digit)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 7:
            {
                cout << "Enter month of birth: ";
                cin >> digit;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (person.birthday.month == digit)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);
                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }
            case 8:
            {
                cout << "Enter year of birth: ";
                cin >> digit;
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (person.birthday.year == digit)
                    {
                        if (!writeHeader)
                        {
                            outputHeader();
                            writeHeader = true;
                        }
                        outputPerson(person);

                    }

                }
                if (!writeHeader) cout << "No matching!!!\n";
                break;
            }

        }
    }

}

void zvit() {
    int choice;
    do {

        cout << "\nChoose what Zvit do you want: \n";
        cout << "1 - Name\n";
        cout << "2 - Last name\n";
        cout << "3 - Otch\n";
        cout << "4 - City\n";
        cout << "5 - Country\n";
        cout << "6 - Day of birth\n";
        cout << "7 - Month of birth\n";
        cout << "8 - Year of birth\n";
        cout << "9 - Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.fail())
        {
            cout << "Invalid choice. Please enter a number between 1 and 7.\n";
            cin.clear();
            cin.ignore(STRMAX, '\n');

        }
        else {

            switch (choice) {
            case 1:
                zvitPerson( 1);
                break;
            case 2:
                zvitPerson( 2);
                break;
            case 3:
                zvitPerson( 3);
                break;
            case 4:
                zvitPerson( 4);
                break;
            case 5:
                zvitPerson( 5);
                break;
            case 6:
                zvitPerson( 6);
                break;
            case 7:
                zvitPerson( 7);
                break;
            case 8:
                zvitPerson( 8);
                break;
                
            case 9:
                cout << "Back\n";
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 7.\n";
                break;
            }
        }


    } while (choice != 9);
}

void changingBinFileData(char* fname, char* lname, int field) {
    tServe person;
    FILE* f = fopen("database.bin", "a+");
    FILE* k = fopen("newdatabase.bin", "a+");
    if (f == NULL || k == NULL) {
        cout << "PROBLEMS!!!!!";
    }

    else {
        while (fread(&person, sizeof(tServe), 1, f) == 1)
        {
            
            if (strcmp(person.fname, fname) == 0 && strcmp(person.lname, lname) == 0) 
            {
                cout << "Enter new data: ";
                switch (field) {
                case 1:
                {
                    cin >> person.fname;
                    strcpy(fname,  person.fname);
                    break;
                }
                case 2:
                {
                    cin >> person.lname;
                    strcpy(lname, person.lname);
                    break;
                }
                case 3:
                    cin >> person.otch;
                    break;
                case 4:
                    cin >> person.city;
                    break;
                case 5:
                    cin >> person.country;
                    break;
                case 6:
                    do {
                        cin >> person.birthday.day >> person.birthday.month >> person.birthday.year;

                    } while (!person.birthday.isValidDate());
                    break;
                default:
                    cout << "Invalid field.\n";
                    break;
                }
            }
            fwrite(&person, sizeof(tServe), 1, k);
        }
        fclose(f);
        remove("database.bin");
        fclose(k);
        rename("newdatabase.bin", "database.bin");
        remove("newdatabase.bin");
        cout << "\n Data was changed \n";
    }
}


void changeData() {
    cout << "==Enter name and last name of person, whose data you want to change==\n";
    char fname[STRMAX];
    char lname[STRMAX];
    cout << "Enter first name: ";
    cin >> fname;
    cout << "Enter last name: ";
    cin >> lname;

   
    int choice;
    do {

        cout << "\nChoose what you want to change: \n";
        cout << "1 - Name\n";
        cout << "2 - Last name\n";
        cout << "3 - Otch\n";
        cout << "4 - City\n";
        cout << "5 - Country\n";
        cout << "6 - Date of birth\n";
        cout << "7 - Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.fail())
        {
            cout << "Invalid choice. Please enter a number between 1 and 7.\n";
            cin.clear();
            cin.ignore(STRMAX, '\n');
            
        }
        else{

            switch (choice) {
            case 1:
                changingBinFileData(fname, lname, 1);
                break;
            case 2:
                changingBinFileData(fname, lname, 2);
                break;
            case 3:
                changingBinFileData(fname, lname, 3);
                break;
            case 4:
                changingBinFileData(fname, lname, 4);
                break;
            case 5:
                changingBinFileData(fname, lname, 5);
                break;
            case 6:
                changingBinFileData(fname, lname, 6);
                break;
            case 7:
                cout << "Back\n";
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 7.\n";
                break;
            }
        }
        

    } while (choice != 7);
}

void deleteData() {
 
    int choice;
    do {
        
        cout << "\nChoose what you want to delete: \n";
        cout << "1 - Delete person`s data \n";
        cout << "2 - Delete whole data\n";
        cout << "3 - Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1: { 
            cout << "==Enter name and last name of person, whose data you want to delete==\n";
            char fname[STRMAX];
            char lname[STRMAX];
            cout << "Enter first name: ";
            cin >> fname;
            cout << "Enter last name: ";
            cin >> lname;
            tServe person;
            FILE* f = fopen("database.bin", "a+");
            FILE* k = fopen("newdatabase.bin", "a+");
            if (f == NULL || k == NULL) {
                cout << "PROBLEMS!!!!!";
            }

            else {
                while (fread(&person, sizeof(tServe), 1, f) == 1)
                {
                    if (strcmp(person.fname, fname) == 0 || (person.lname, lname) == 0) {
                        continue;
                    }
                    fwrite(&person, sizeof(tServe), 1, k);
                }
                fclose(f);
                remove("database.bin");
                fclose(k);
                rename("newdatabase.bin", "database.bin");
                remove("newdatabase.bin");
                cout << "\n Data was changed \n";
            }
            break; 
        }
        case 2:
            cout << "Are you shure? After deleting whole data, restore won`t be possible \n";
            int choicein;
            cout << "1 - Yes \n";
            cout << "2 - No\n";
            cout << "Enter your choice: ";
            cin >> choicein;
            switch (choicein) {
            case 1:
                remove("database.bin");
                break;
                   
            }
            break;
        case 3: 
            cout << "Exit";
            break;
        default:
            cout << "Invalid choice\n";
        }
    } while (choice != 3);


}

void printMenu() {
    cout << "\n===== Main Menu =====\n";
    cout << "1 - Add Person\n";
    cout << "2 - Show information about person\n";
    cout << "3 - Change Data\n";
    cout << "4 - Delete Data\n";
    cout << "5 - Show whole list\n";
    cout << "6 - Zvit \n";
    cout << "7 - Exit\n";
    cout << "Enter your choice: ";

    
}


int main() {
    tServe person;
    int choice;

    do {
        printMenu();
        cin >> choice;
        if (cin.fail())
        {
            cout << "Invalid choice. Please enter a number between 1 and 7.\n";
            cin.clear();
            cin.ignore(STRMAX, '\n');

        }


        switch (choice) {
        case 1:
            addPerson();
            break;
              case 2:
                  searchPerson();
                  break;
              case 3:
                  changeData();
                  break;
              case 4:
                  deleteData();
                  break;
        case 5:
            outputData();
            break;
        case 6:
            zvit();
            break;
        case 7:
            cout << "Exiting the program.\n";
            break;
        default:
            cout << "Invalid choice. Please enter a number between 1 and 6.\n";
        }

    } while (choice != 7);
    printMenu();
    cin >> choice;

    return 0;
}
/*1
Nazar
Teslenko
Olekh
Kyiv
UA
11 10 2000
1
John
Smith
Davidovich
London
UK
12 05 1995
1
Anna
Ivanova
Petrovna
Paris
France
03 08 1987
1
Michael
Brown
Michailovich
Los_Angeles
USA
15 09 1990
1
Sophia
Garcia
Maria
Madrid
Spain
25 03 2002
1
Alex
Kuznetsov
Vladimirovich
Kyiv
UA
09 11 1978
1
Emma
Taylor
Emmanuilovna
Paris
France
28 06 1993
1
Daniel
Lee
Chang
Seoul
South_Korea
17 04 2001
1
Olivia
Johnson
Olegovna
New_York_City
USA
20 12 1980
1
Liam
Wong
Leonidovich
Beijing
China
05 02 1975
1
Mia
Nguyen
Maximovna
Ho_Chi_Minh_City
Vietnam
10 10 1998
1
Ethan
Khan
Ali
Karachi
Pakistan
14 07 1983
1
Ava
Wu
Yan
Shanghai
China
06 09 2005
1
Logan
Gonzalez
Loganovich
Mexico_City
Mexico
19 01 1992
1
Isabella
Martinez
Sofia
Bogota
Colombia
23 11 1970
1
Lucas
Chen
Lukich
Tokyo
Japan
30 04 1986
1
Charlotte
Lopez
Charlottovna
Toronto
Canada
02 03 1999
1
Jackson
Zhang
Wei
Beijing
China
08 07 2004
1
Amelia
Hernandez
Maria
Madrid
Spain
11 12 1979
1
Luna
Gomez
Lukianovna
Buenos_Aires
Argentina
13 05 1996
1
Mateo
Ng
Minh
Ho_Chi_Minh_City
Vietnam
18 08 2003
1
Harper
Chan
Harperovich
Singapore
Singapore
21 09 1988
1
Evelyn
Rajan
Evelynovna
Mumbai
India
27 06 2000
1
Elijah
Patel
Ramesh
Delhi
India
22 03 1991
1
Avery
Singh
Averyovna
Lahore
Pakistan
04 11 1982
1
Aria
Kim
Artemovna
Seoul
South_Korea
09 02 1977
1
Carter
Li
Cartovich
Shanghai
China
15 10 1994
1
Scarlett
Das
Scarlettovna
Dhaka
Bangladesh
24 07 1985
1
Matthew
Ma
Matthewovna
Sydney
Australia
28 01 2003
1
Addison
Ahmed
Addisonovna
Karachi
Pakistan
03 04 1974
1
Riley
Mittal
Rileyovna
Mumbai
India
12 12 1989
1
Madison
Lin
Madisonovna
Tokyo
Japan
19 08 1997
1
Zoey
Gupta
Zoeyovna
Seoul
South_Korea
26 05 2007
1
Sebastian
Wang
Sebastianovna
Singapore
Singapore
30 09 1984
1
Leah
Patil
Leahovna
Dhaka
Bangladesh
06 06 1992
1
Gabriel
Chen
Gabrielovna
Shanghai
China
11 03 1980
*/
