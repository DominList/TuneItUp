//
//  main.c
//  TuneItUp!_unPL
//
//  Created by DPL on 11.06.2013.
//  Copyright (c) 2013 DPL. All rights reserved.
//

//
//  main.c
//  PROJECT TuneItUp!
//
//  Created by DPL on 31.05.2013.
//  Copyright (c) 2013 DPL. All rights reserved.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//------------------------------------ Deklaracja tablic akordow ------------------------------------------
const char *minorChords[] = {"c", "cis", "d", "dis", "e", "f", "fis", "g", "gis", "a", "b", "h"};
const char *majorChords[] = {"C", "Cis", "D", "Dis", "E", "F", "Fis", "G", "Gis", "A", "B", "H"};

//------------------------------------------- printChords ------------------------------------------------- 
// Funkcja wyswietlajaca akordy z tablicy. Za parametry przyjmuje tablice dwuwymiarowa typu char,
// wartosc poczatkowa dla licznika wierszy 'startValue' licznik wierszy w tablicy - 'rowCounter',
// wartosc inkremetacji 'jump' oraz strumien do ktorego nalezy skierowac dane

void printChords(char storeChords[][100], int startValue, int rowIndicator, int jump, FILE *output){
    for (int i = startValue; i<rowIndicator; i += jump) {
        fprintf(output, "%s" ,storeChords[i]);
    } // for
} // end printChords

//--------------------------------------- printStarLine --------------------------------------------------- 
// Funkcja wyswietla linie gwiazdek

void printStarLine(){
    printf("\n\n");
    for (int i = 0; i < 80; i++) {
        printf("*");
    } // for
    printf("\n\n");
} // end printStarLine

//-------------------------------------------- printInfo -------------------------------------------------- 
//Funkcja wyswietlajaca infrormacje o programie

void printInfo(){
    printStarLine();
    printf("\nProgram TuneItUp! ver0.0, autor: Dominik Listos\n");
    printf("Otworz plik \"*.txt\" i zmien tonacje utoworu zapisanego w akordach gitarowych.\n");
    printf("Program obsluguje notacje polska.\n");
    printStarLine();
} // end printInfo

//------------------------------------------- circIn12 ---------------------------------------------------- 

// Funkcja umozliwiajaca cyrkulacje wewnatrz tablicy dwunastoelementowej,
// Niezaleznie od podanej wartosci, zwraca liczbe z zakresu 0 do 11
// Przyjmuje za argumenty biezaca pozycycje tablicy i liczbe krokow,
// dotatnia lub ujemna, o ktore chcemy sie przemiescic
int circIn12 (unsigned short int currentPos, int moveSteps){
    
    if (currentPos >= 0 && currentPos < 12) {
        int paramSum = currentPos + moveSteps;
        if (moveSteps > 0)
            return (paramSum) % 12;
        else if (moveSteps < 0){
            if (paramSum >= 0) return paramSum;
            else if (paramSum >= -12) return 12 + paramSum;
            else return 11 + paramSum % 12;
        }
        
        else
            return currentPos;
    }else {
        fprintf(stderr, "Nieprawidlowy parametr funkcji circIn12!");
        exit(1);
    }
} // end circIn12

//----------------------------------------- chordsCompare ------------------------------------------------- 
// w argumentach mozna umiescic tablice akordow  oraz wskaznik na akord odczytany z pliku
// funkcja moze zwrocic numer z tablicy, zwraca -1, gdy wartosc nie jest rozpoznana

int chordsCompare ( char *currentChord, const char *chordsArray[] ){
    for(int i = 0; i < 12; i++){
        if (strncmp(currentChord, chordsArray[i], 1) == 0){
            if(strncmp( currentChord, chordsArray[circIn12(i, +1)], 2) == 0 ){
                return i+1;
            }else
                return i;
        } // end all if
    } // for
    return -1; // jesli szukany element nie wystepuje
} // end chordsCompare

//-------------------------------------------- shiftChord ------------------------------------------------- 
// Funkcja zmieniajaca wysokosc (skale) akordu o podana w parametrze wartosc
// Sprawdza w tablicach minorChords i majorChords i zwraca akord (string) z pozycji +semitones
// Obcina pozostale znaki zawarte w argumencie 'chord'
// Gdy nie rozpozna akordu zwraca ciag zadany w parametrze i wyswietla komunikat

const char *shiftChord ( char *chord, int semitones) {
    if ( chordsCompare(chord, minorChords) != -1){
        return minorChords[ circIn12( chordsCompare(chord, minorChords), semitones ) ];
    }
    else if ( chordsCompare(chord, majorChords) != -1) {
        return majorChords[ circIn12( chordsCompare(chord, majorChords), semitones ) ];
    }
    else{
        fprintf(stderr, "Nie rozpoznalem akordu \"%s\", zwracam oryginalny. \n", chord );
        return chord;
    }//if
} // shiftChord

//----------------------------------------- returnReminder ------------------------------------------------ 
// Funkcja zwraca roznice miedzy dwoma lanuchami w postaci lancucha
// Zakladamy, iz poczatatkowe znaki sa takie same, funkcja "wyrzuci" pozostale znaki
// W kazdym innym wypadku zwracany jest drugi lancuch w calosci
char *returnReminder (char *currentChord, char *longerString){
    if ( strncmp(currentChord, longerString, strlen(currentChord)) == 0 ){
        return ( longerString +  strlen(currentChord) );
    }else
        return longerString;
}  // returnReminder

//------------------------------------------- getFileName ------------------------------------------------- 
// Pobiera nazwe pliku razem ze spacjami wewnatz nazwy, odrzuca znak '\n'
void getFileName( char *fileName, int size ){
    char tempName[size];
    fgets(tempName, size, stdin);
    int startRead = 0;
    for (int i = 0; i < size; i++){
        if  ( (tempName[i] == ' ' || tempName[i] == '\n' ) && ( startRead == 0 ) ) continue;
        else if (tempName[i] !='\n' ){
            startRead = 1;
            *fileName = tempName[i];
            fileName++;
        } //else if
    } // for
} // end getFileName
//--------------------------------------------- readFile -------------------------------------------------- 
// Funkcja umieszczajaca zawartosc pliku w tablicy dwuwymiarowej typu char wiersz po wierszu,
// Zwraca liczbe uzytych wierszy lub wartosc -1, gdy plik nie istnieje,
// Kiedy liczba wczytanch wierszy osiaga wartosc maxRowNumber -1, zwraca -3 i przerywa dzialanie
// -2, gdy rozmiar wiersza zostal przekroczony
// Pojedynczy wiersz wczytywanego pliku moze zawierac maksymalnie 100 znakow

int readFile(char *fileName, char storeChords[][100], int maxRowsNumber) {
    char c;
    int currentCharCounter = 0;             // numer elementu przetwarzanego w obecnym i-tym wierszu
    unsigned int rowIndicator = 0;          // licznik zainicjowany tutaj, aby skorzystac z niego
    FILE *file;
    
    file = fopen ( fileName, "r" );
    if(file == NULL){
        return -1; // plik nie istenieje
    }//if
    
    do{
        c = fgetc(file);
        if (currentCharCounter == 99 &&  c != '\n')
            return -2; // rozmiar wiersza zostal przekroczony
        
        if (c == '\n'){
            //storeChords[rowIndicator][currentCharCounter] = c;
            rowIndicator++;
            currentCharCounter = 0;
        }
        if (c != EOF){
            storeChords[rowIndicator][currentCharCounter] = c;
            currentCharCounter++;
        }
        if (rowIndicator == maxRowsNumber - 1) {
            return -3; // rozmiar zadany zostal przekroczony
        }
    }while (!feof(file));
    
    fclose(file);
    return rowIndicator + 1;
} // end readFile


//------------------------------------------- writeFile --------------------------------------------------- 
// Funkcja zapisuje tablice dwuwymiarowa do pliku, jak sama nazwa wskazuje
// A parametry to oczywiscie tablica[][70], licznik wierszy i plik

void  writeFile ( char storeChords[][100], int rowIndicator, char *fileName) {
    FILE *file = fopen(fileName, "w");
    printChords(storeChords, 0, rowIndicator, 1, file);
    fclose(file);
    
} // end writeFile

//------------------------------------------- yesNoQuestion ----------------------------------------------- 
// Zadaje pytanie, wymaga odpowiedzi 't' lub 'n', zwraca 0 dla tak i 1 dla nie

int yesNoQuestion ( const char *question ){
    char doOrNot;
    do {
        printf("%s (t/n): ", question);
        scanf(" %c", &doOrNot);
    } while (doOrNot != 'n' && doOrNot != 't');
    if(doOrNot == 'n') return 0;
    else return 1;
}// end yesNoQuestion



//========================================================================================================================
//**********************************************  MAIN *******************************************************************
//========================================================================================================================
int main(int argc, const char * argv[])
{
    char storeChords[300][100]; // tablica wczytanych akordow
    int rowIndicator = 0;       // liczik wczytanych wierszy, potrzebny przy przetwarzaniu i zapisie danych
    char fileName[200];         // nazwa wczytywanego pliku
    
    //----------------------------------------- Informacja o programie --------------------------------------------------
    printInfo();
    // -------------------------------------- Otwieranie pliku tekstowego------------------------------------------------
    do{
        
        printf("Wprowadz nazwe pliku, sciezke dostepu lub '0' aby zakonczyc: ");
        getFileName(fileName, 200);
        
        if ( strncmp ( fileName, "0", 1 ) == 0 ) {
            puts("Do zobaczenia!");
            return 0;
        }
        rowIndicator = readFile(fileName, storeChords, 300);
        if (rowIndicator == -1)
            printf("Niestety, nazwa \"%s\" jest nieprawidlowa lub plik nie istnieje. \nTry again! ;)\n", fileName);
        else if (rowIndicator == -2){
            puts("Oj, chyba za duzo znakow jest w ktoryms z wierszy pliku. \nDopuszczalna ilosc to 100 (99 + return).");
            puts("Sprobuj to naprawic i odpal program jeszcze raz!");
            exit(1);
        }else if ( rowIndicator == -3){
            fprintf(stderr, "Musze zakonczyc prace :( zbyt wiele wierszy w pliku!");
            exit(1);
        }// if
    }while ( rowIndicator < 0 );
    //--------------------------------------- Wyswietlenie tytulu utworu -----------------------------------------------
    printStarLine();
    printf("Biezacy utwor to:  %s ", storeChords[0]);
    printStarLine();
    
    //---------------------------------- Wyswietlanie akordow pobranych z pliku ----------------------------------------
    printf("Akordy odczytane z pliku:");
    printStarLine();
    printChords(storeChords, 1, rowIndicator, 2, stdout);
    printStarLine();
    
    //--------------------------------- Przetwarzanie wysokosci akordow w pliku ----------------------------------------
    char tempRow[100] = {"\n"};
    char *charPointer;
    char tempChord[20];
    int semitones;
    
    
    printf("Wprowadz liczbe poltonow by zmienic skale (np. \"1\" lub \"-4\") \nlub \"0\" aby zakonczyc: ");
    scanf(" %d", &semitones);
    if (semitones == 0){
        puts("Koniec.");
        return 0;
    }
    
    while(semitones != 0){   // lecimy co drugi wiersz w storeChords, wczytujemy i podmieniamy akordy na nowe
        
        for( int counter = 1; counter < rowIndicator; counter += 2){
            charPointer = strtok(storeChords[counter], " \n");
            while (charPointer != NULL) {
                strncpy(tempChord, shiftChord( charPointer, 0 ), 20);
                strcat(tempRow, shiftChord(charPointer, semitones) );
                strcat(tempRow, returnReminder(tempChord, charPointer));
                strcat(tempRow, " "); // dodaj spacje miedzy akordami
                charPointer = strtok( NULL, " ");
            }// while
            strncpy(storeChords[counter], tempRow, 100);
            strcpy( tempRow, "\n" );
        } //tutaj koniec petli i zamiany akordow
        
        // wyswietlenie akordow z tablicy storeChords
        printStarLine();
        printf("Oto przetworzone akordy zapisane w tablicy programu:");
        printStarLine();
        printChords(storeChords, 1, rowIndicator, 2, stdout);
        printStarLine();
        
        printf("Wprowdadz liczbe poltonow lub \"0\", aby zakonczyc: ");
        scanf(" %d", &semitones);
        
    }// while
    
    //-----------------------------------------   Zapis do pliku  -----------------------------------------
    if (yesNoQuestion("Czy zapisac wynik do pliku?")) {
        writeFile(storeChords, rowIndicator, fileName);
        puts("Zapisano!");
    }
    
    puts( "No to finisz, narka! ;)");
    return 0;
    
}   // main ()



