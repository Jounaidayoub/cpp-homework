#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool contient(const string &texte, const string &mot)
{
    if (mot.empty())
        return true;

    size_t j = 0;
    for (size_t i = 0; i < texte.length() && j < mot.length(); i++)
    {
        if (texte[i] == mot[j])
            j++;
    }
    return j == mot.length();
}

int main(int argc, char *argv[])
{
    string nomFichier, mot;

    if (argc >= 3)
    {
        nomFichier = argv[1];
        mot = argv[2];
    } 
    else
    {
        cout << "Nom du fichier : ";
        if (!getline(cin, nomFichier))
            return 0;
        cout << "Mot a chercher : ";
        if (!getline(cin, mot))
            return 0;
    }

    ifstream f(nomFichier);
    if (!f.is_open())
    {
        cerr << "Impossible d'ouvrir le fichier : " << nomFichier << endl;
        return 1;
    }

    string ligne;
    int numLigne = 0;
    int nbOcc = 0;
    while (getline(f, ligne))
    {
        numLigne++;
        if (contient(ligne, mot))
        {
            nbOcc++;
            cout << "Trouve ligne " << numLigne << " : " << ligne << endl;
        }
    }

    if (nbOcc > 0)
        cout << "La deuxieme chaine existe dans le fichier (" << nbOcc << " ligne(s))." << endl;
    else
        cout << "La deuxieme chaine n'existe pas dans le fichier." << endl;

    return 0;
}
