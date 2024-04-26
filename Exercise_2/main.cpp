/* s284817 Andrea Terenziani    Esercitazione 5

Il programma è finalizzato ad importare una mesh da file input, verificare la correttezza nell'importazione di essa,
memorizzare le caratteristiche di ogni cella negli appositi contenitori, e controllare che nessuna cella risulti degenere (ovvero lato di lunghezza nulla
oppure area della cella2D nulla.

A)Verificare corretta importazione:
affinchè la mesh sia correttamente importata, ogni sua componente deve essere correttamente importata.
Se anche solo una tra celle0D, celle1D e celle2D ritorna false, l'import della mesh sarà fallito (e quindi avrà return false).
I problemi che ogni cella può dare sono 3:
1)il file input non è stato aperto correttamente;
2)il file input non contiene nessuna cella di quel determinato tipo
3)-il lato importato è degenere
  -il poligono importato è degenere



B)verificare che nessuna celle 2D sia degenere:
1)se ha un lato nullo, ovvero se la distanza tra due vertici di un lato è minore della precisione di macchina eps
2)se ha area nulla, ovvero se l'area del triangolo è minore di eps (NB! Non eps^2 perchè (10^-16)^2 = 10^-32 < 10^-16, che è la misura minima rappresentabile dalla macchina)


C)Memorizzare caratteristiche negli appositi contenitori:
1)ID: vettore di dimensione fissata a num_tot_ID elementi, (non sapendo qual è il numero complessivo di ID, prima creo una lista per poter aggiungere memoria in modo agevole,
poi, finita la lista, creo un vettore in modo da poter accedere ai valori del vettore senza costi computazionali elevati)
2)coordinate: vettore di dimensione num_tot_ID, contenente vettori bidimensionali (con rispettivamente la prima e seconda coordinata).
Usiamo vector2d e non array perchè vogliamo poter svolgere operazioni matematiche sulle coordinate
3)segmenti: vettore di array bidimensionali contenenti il punto origin e il punto end. Uso array perchè la dimensione è fissa e non devo svolgere operazioni matematiche su essi
4)celle2D vertici/lati:
se sapessi a priori il numero n dei lati del poligono: vettore di array n-dimensionali (contenente i vertici/lati)
se devo leggere da file il numero n dei lati del poligono: vettore di vettori n-dimensionali (contenente i vertici/lati), poichè la dimensione dei container interni deve poter essere flessibile
5)marker: mappa che associa ad ogni valore dei marker, la lista di tutti gli ID caratterizzati da tale marker
(questo vale sia per celle0D che per celle1D. Per celle2D invece tutti i marker sono uguali a zero, in quanto ogni poligono è interno, e dunque le celle2D non sono definite dai marker)



*/




#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;




//Il programma da return 0 solo ed esclusivamente se la mesh rispetta tutte le condizioni richieste dalla consegna.
//Tutte le condizioni sono state inserite e richiamate a cascata all'interno della funzione ImportMash
//Se anche solo una non è rispettata, il return sarà 1

int main()
{
    PolygonalMesh mesh;

    //controllo che la mesh sia importata correttamente
    string filepath = "PolygonalMesh";
    if(!ImportMesh(filepath, mesh))
    {
        return 1;
    }

  return 0;
}
