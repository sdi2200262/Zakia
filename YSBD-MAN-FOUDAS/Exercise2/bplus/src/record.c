#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <record.h>

const char* names[] = {
    "Alexandros",  // Αλέξανδρος
    "Sofia",       // Σοφία
    "Dimitris",    // Δημήτρης
    "Anna",        // Αννα
    "Konstantinos",// Κωνσταντίνος
    "Maria",       // Μαρία
    "Georgios",    // Γεώργιος
    "Eleni",       // Ελένη
    "Petros",      // Πέτρος
    "Evangelia"    // Ευαγγελία
};


const char* surnames[] = {
    "Papadopoulos",   // Παπαδόπουλος
    "Georgiou",       // Γεωργίου
    "Dimitriou",      // Δημητρίου
    "Anagnostopoulos",// Αναγνωστόπουλος
    "Karagiannis",    // Καραγιάννης
    "Mavromatis",     // Μαυρομάτης
    "Nikolaou",       // Νικολάου
    "Christodoulou",  // Χριστοδούλου
    "Kostopoulos",    // Κωστόπουλος
    "Stamatopoulos"   // Σταματόπουλος
};

const char* cities[] = {
    "Athina",     // Αθήνα
    "Patra",      // Πάτρα
    "Irakleio",   // Ηράκλειο
    "Larisa",     // Λάρισα
    "Volos",      // Βόλος
    "Ioannina",   // Ιωάννινα
    "Chania",     // Χανιά
    "Kalamata",   // Καλαμάτα
    "Rodos"       // Ρόδος
};


static int id = 0;

Record randomRecord(){
    Record record;
    // create a record
    record.id = rand()%1000;
    int r = rand() % (sizeof(names) / sizeof(names[0]));
    memcpy(record.name, names[r], strlen(names[r]) + 1);
    //
    r = rand() %  (sizeof(surnames) / sizeof(surnames[0]));
    memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
    //
    r = rand() %  (sizeof(cities) / sizeof(cities[0]));
    memcpy(record.city, cities[r], strlen(cities[r]) + 1);
    return record;
}

void printRecord(Record record){
    printf("(%d,%s,%s,%s)\n",record.id,record.name,record.surname,record.city);

}



