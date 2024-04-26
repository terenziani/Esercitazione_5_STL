#pragma once

#include <iostream>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;


namespace PolygonalLibrary
{
    struct PolygonalMesh
    {
        /*Per ogni tipo di cella, definisco le strutture che conterranno ID,coordinate, segmenti, vertici, lati e marker
        (a seconda di quali siano necessari per descrivere tale cella)

         Tutto verrà passato come attributo del struct PolygonalMesh*/


        //----------Celle 0D, caratterizzate da ID, coordinate e marker

        //numero di celle 0D totali
        unsigned int NumberCell0D = 0;
        //contenitore ID
        vector<unsigned int> Cell0DId = {}; //vettore di interi positivi di dimensione 1 x NumeroCelle0D
        //contenitore coordinate
        vector<Vector2d> Cell0DCoordinates = {}; //vettore di coppie di double di dimensione 2 x NumeroCelle0D
        //contenitore marker
        map<unsigned int, list<unsigned int>> Cell0DMarkers = {}; //mappa che associa ad ogni valore dei marker, la lista di tutti gli ID caratterizzati da tale marker



        //----------Celle 1D, caratterizzate da ID, segmenti del tipo (origin,end) e marker

        //numero di celle 1D totali
        unsigned int NumberCell1D = 0;
        //contenitore ID
        vector<unsigned int> Cell1DId = {}; //vettore di interi positivi di dimensione 1 x NumeroCelle1D
        //contenitore segmenti
        vector<array<unsigned int,2>> Cell1DVertici = {}; // vettore di coppie di ID (ID_origine,ID_end) di dimensione 2 x NumeroCelle1D
        //contenitore marker
        map<unsigned int, list<unsigned int>> Cell1DMarkers = {}; //mappa che associa ad ogni valore dei marker, la lista di tutti gli ID caratterizzati da tale marker


        //----------Celle 2D, caratterizzate da ID, vertici e lati. le celle 2D sono dunque dei poligoni

        //numero di celle 2D totali
        unsigned int NumberCell2D = 0;
        //contenitore ID
        vector<unsigned int> Cell2DId = {}; //vettore di interi positivi di dimensione 1 x NumeroCelle2D
        //contenitore lati
        int Num_lati_poligono;
        vector<vector<unsigned int>> Cell2DLati = {}; // vettore di vettori, ciascuno contenente i lati di ogni poligono.
                                                      //Utilizziamo un vettore e non un array, perchè il numero di lati dei poligoni non è costante e viene letta sul file input
                                                      // la dimensione è Num_lati_poligono x NumeroCelle2D

        //contenitore vertici (il numero di vertici coincide con il numero di lati)
        vector<vector<unsigned int>> Cell2DVertici = {}; // vettore di vettori, ciascuno contenente i vertici di ogni poligono.
                                                         //Utilizziamo un vettore e non un array, perchè il numero di lati dei poligoni non è costante e viene letta sul file input
                                                         // la dimensione è Num_Vertici_poligono x NumeroCelle2D

    };
    //NB! è inizializzato per avere tutti i vettori vuoti



}

