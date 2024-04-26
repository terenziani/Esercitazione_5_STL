#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cmath>
#include <Eigen/Eigen>



using namespace std;




//utilizzo il metodo dell'area di Gauss per calcolare l'area di un poligono di n lati
double Area_poligono(vector<Vector2d> vertici)
{
    int n= vertici.size();
    double area = 0;

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        area += vertici[i][0] * vertici[j][1] - vertici[j][0] * vertici[i][1];
    }

    return abs(area) / 2.0;
}



namespace PolygonalLibrary
{


//funzione che importa la mesh e richiama a cascata tutte le funzioni che hanno il compito di:
//A)verificare la corretta importazione
//B)verificare che esistano celle di ciascun tipo
//C)verificare che le celle non siano degeneri
//D)in caso sia tutto corretto, salvare i valori letti dal file input nelle apposite strutture
bool ImportMesh(const string& filepath,PolygonalMesh& mesh)
{
    //controllo celle0D
    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        //funzione di stampa

        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }

    }

    //controllo celle 1D
    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        //funzione di stampa

        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }

    }



    //controllo celle 2D

    //controllo su import
    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv",
                       mesh))
    {
        return false;
    }

    //controllo che non ci siano lati degeneri
    if(! MisuraLato(filepath,mesh))
    {
        return false;
    }

    //controllo non ci siano poligoni degeneri
    if(! Misura_area(filepath,mesh))
    {
        return false;
    }


    else
    {
        // Test:
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<unsigned int> edges = mesh.Cell2DLati[c];

            for(unsigned int e = 0; e < size(mesh.Cell2DLati[c]); e++)
            {
                const unsigned int origin = mesh.Cell1DVertici[edges[e]][0];
                const unsigned int end = mesh.Cell1DVertici[edges[e]][1];

                auto findOrigin = find(mesh.Cell2DVertici[c].begin(), mesh.Cell2DVertici[c].end(), origin);
                if(findOrigin == mesh.Cell2DVertici[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 2;
                }

                auto findEnd = find(mesh.Cell2DVertici[c].begin(), mesh.Cell2DVertici[c].end(), end);
                if(findEnd == mesh.Cell2DVertici[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 3;
                }

            }
        }
    }


    //se tutto è andato a buon fine
    return true;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool ImportCell0Ds(const string &filename,PolygonalMesh& mesh)
{
    //NB!! tutti i container sono stati già definiti come attributi della mesh in 'PolygonalMesh.hpp'
    //quindi dovremo semplicemente andare ad inserire, negli attributi della mesh passata come parametro per riferimento, i valori trovati

    ifstream file;
    file.open(filename);

    //ritorno false se c'è errore di apertura del file
    if(file.fail())
        return false;

    //creo la lista i cui elementi sono le righe del file (scartando la prima riga che è non desiderata)
    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);
    listLines.pop_front();


    //leggo la misura della lista, ovvero il numero di celle0D complessivo. (Questo mi permette di poter creare un vettore, e quindi accedere
    //più facilmente agli elementi). Ritorna false se il numero di celle0D è 0
    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    //riservo lo spazio per i vettori della dimensione appena trovata
    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);


    //trasformo le stringhe della lista in istringstream in modo da poterci lavorare sopra, separare i dati e inserirli negli appositi contenitori
    //Per fare ciò, gli elementi prima li leggo come stringhe, poi li converto
    for (const string& line : listLines)
    {
        istringstream isline(line);

        Vector2d coord;
        string ID_s;
        string coord_x_s;
        string coord_y_s;
        string marker_s;

        getline(isline, ID_s, ';');
        getline(isline, marker_s, ';');
        getline(isline, coord_x_s, ';');
        getline(isline, coord_y_s);

        //converto le stringhe
        unsigned int ID = stoul(ID_s);
        unsigned int marker = stoul(marker_s);
        double coord_x = stod(coord_x_s);
        double coord_y = stod(coord_y_s);

        //le aggiungo ai rispettivi container
        mesh.Cell0DId.push_back(ID);
        coord(0)=coord_x;
        coord(1)=coord_y;
        mesh.Cell0DCoordinates.push_back(coord);


        //per aggiungere alla mappa dei marker, devo verificare se è la prima volta che tale marker compare,
        //o se invece ci sono già degli ID associati ad esso
        if( marker != 0)
        {
            if (mesh.Cell0DMarkers.find(marker) == mesh.Cell0DMarkers.end())
                mesh.Cell0DMarkers.insert({marker, {ID}});

            else
                mesh.Cell0DMarkers[marker].push_back(ID);

        }

    }

    file.close();
    return true;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------


bool ImportCell1Ds(const string &filename,PolygonalMesh& mesh)
{
    //inizialmente faccio gli stessi passaggi svolti in ImportCell0Ds.
    //(per il commento fare riferimento a quanto scritto in precedenza)

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);
    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertici.reserve(mesh.NumberCell1D);

    //ATTENZIONE: parte diversa rispetto a ImportCell0Ds
    //non abbiamo più le coordinate da salvare, bensì la coppia (origin,end) del segmento

    for (const string& line : listLines)
    {
        istringstream isline(line);

        array<unsigned int,2> segmento;
        string ID_s;
        string origin_s;
        string end_s;
        string marker_s;

        getline(isline, ID_s, ';');
        getline(isline, marker_s, ';');
        getline(isline, origin_s, ';');
        getline(isline, end_s);

        //converto le stringhe
        unsigned int ID = stoul(ID_s);
        unsigned int marker = stoul(marker_s);
        unsigned int origin = stoul(origin_s);
        unsigned int end = stoul(end_s);

        //le aggiungo ai rispettivi container
        mesh.Cell1DId.push_back(ID);
        segmento[0]=origin;
        segmento[1]=end;
        mesh.Cell1DVertici.push_back(segmento);


        if( marker != 0)
        {
            if (mesh.Cell1DMarkers.find(marker) == mesh.Cell1DMarkers.end())
                mesh.Cell1DMarkers.insert({marker, {ID}});

            else
                mesh.Cell1DMarkers[marker].push_back(ID);

        }

    }

    file.close();
    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------

//ATTENZIONE: qua cambia rispetto ai casi precedenti, in quanto non possiamo sapere a priori la dimensione del poligono
//(ovvero il suo numero di lati e vertici), quindi lo dobbiamo leggere da file


bool ImportCell2Ds(const string &filename,PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);
    listLines.pop_front();

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVertici.reserve(mesh.NumberCell2D);
    mesh.Cell2DLati.reserve(mesh.NumberCell2D);


    //ora per ogni riga, converto e salvo i dati negli appositi vettori
    //NB! Il numero di elementi per ogni riga è variabile, a seconda di quanti
    //sono il numero di lati e di vertici del poligono
    for (const string& line : listLines)
    {
        istringstream isline(line);
        //le prime 3 posizioni sono uguali per ogni riga, ovvero ID;marker;Num_vertici
        string ID_s;
        string marker_s;
        string num_vertici_s;

        getline(isline, ID_s, ';');
        getline(isline, marker_s, ';');
        getline(isline, num_vertici_s, ';');

        unsigned int ID = stoul(ID_s);
        unsigned int marker = stoul(marker_s);
        unsigned int num_vertici = stoul(num_vertici_s);


        //ora creo un vettore in cui contenere l'elenco dei vertici (che saranno tanti quanti num_vertici)
        //e faccio la stessa cosa per quanto riguarda i lati.
        //Come sempre, prima li salverò come stringhe e poi li convertirò in unsigned int
        vector<string> vettore_vertici_s(num_vertici);
        vector<string> vettore_lati_s(num_vertici);

        vector<unsigned int> vettore_vertici(num_vertici);
        vector<unsigned int> vettore_lati(num_vertici);

        for (int i = 0; i < num_vertici; ++i)
        {
            getline(isline, vettore_vertici_s[i], ';');
            vettore_vertici[i]=stoul(vettore_vertici_s[i]);

        }


        //leggo il numero di lati e verifico che coincida con il numero di vertici, altrimenti segnalo l'errore
        string num_lati_s;
        getline(isline,num_lati_s,';');
        unsigned int num_lati=stoul(num_lati_s);

        if (num_lati != num_vertici)
        {
            cerr<<"Il numero di lati e di vertici del poligono cella2D non coincidono"<<'\n';
        }

        else


            for (int i = 0; i < num_vertici; ++i)
            {
                getline(isline, vettore_lati_s[i], ';');
                vettore_lati[i]=stoul(vettore_lati_s[i]);

            }

        //Ora che ho tutti i dati, li posso aggiungere agli attributi della cella2D

        mesh.Cell2DId.push_back(ID);
        mesh.Cell2DVertici.push_back(vettore_vertici);
        mesh.Cell2DLati.push_back(vettore_lati);

    }


    file.close();
    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------------

//questa funzione restituisce true solo se
//tutti i lati dei poligoni hanno lunghezza maggiore della precisione di macchina, altrimenti espone a video i lati problematici

//NB!! E' necessario utilizzare un vettore perchè il numero di elementi di ciascuno di essi può essere variabile

bool MisuraLato(const string &filename,PolygonalMesh& mesh)
{

    for (int i = 0; i< mesh.NumberCell2D; ++i)
    {
        //sono dentro ad un singolo vettore contenente i lati del poligono

        //inizializzo il vettore in cui andro ad inserire le coordinate di ogni vertice del poligono
        vector<Vector2d> vertici={};



        for ( unsigned int lato : mesh.Cell2DLati[i])
        {

            //ora ho un singolo lato, che corrisponde alla posizione all'interno di Cell1DId
            unsigned int origin = mesh.Cell1DVertici[lato][0];
            unsigned int end = mesh.Cell1DVertici[lato][1];


            //ciascuna posizione, quindi sia origin che end, ha due coordinate
            double coord_x_origin= mesh.Cell0DCoordinates[origin][0];
            double coord_y_origin= mesh.Cell0DCoordinates[origin][1];
            double coord_x_end= mesh.Cell0DCoordinates[end][0];
            double coord_y_end= mesh.Cell0DCoordinates[end][1];


            //calcolo le distanze
            double dist_x=abs(coord_x_origin-coord_x_end);
            double dist_y=abs(coord_y_origin-coord_y_end);


            //verifico che il lato non sia degenere
            if (dist_x<numeric_limits<double>::epsilon() && dist_y<numeric_limits<double>::epsilon())
            {
                cerr<<" il lato "<<lato<<" e' degenere";
                return false;
            }

        }

    }

    return true;
}

    ;

//--------------------------------------------------------------------------------------------------------------------------------------------------

//questa funzione restituisce true solo se tutte le aree dei poligoni hanno dimensione maggiore della precisione di macchina
//(NB! Non al quadrato perchè (10^-16)^2 = 10^-32 < 10^-16, che è la misura minima rappresentabile dal computer)
//altrimenti espone a video i poligoni problematici

//Analogo a misura lato, ma possiamo passare direttamente dal vettore dei vertici delle celle2D per risparmiare un passaggio
bool Misura_area (const string &filename,PolygonalMesh& mesh)
{
    for (int i = 0; i< mesh.NumberCell2D; ++i)
    {
        vector<Vector2d> vertici_poligono={};
        for (unsigned int vertice : mesh.Cell2DVertici[i])
        {
            double coord_x= mesh.Cell0DCoordinates[vertice][0];
            double coord_y= mesh.Cell0DCoordinates[vertice][1];

            Vector2d punto= {coord_x,coord_y};
            vertici_poligono.push_back(punto);
        }

        double area=Area_poligono(vertici_poligono);


        if (area< numeric_limits<double>::epsilon())
        {

            cout<<" e il poligono "<<i<<" ha area nulla";
            return false;
        }

    }
    return true;
}


}
