#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool motExiste(const string &ligne, const string &mot)
{
    if (mot.empty())
        return true;
    if (mot.size() > ligne.size())
        return false;
    for (size_t i = 0; i + mot.size() <= ligne.size(); i++)
    {
        size_t j = 0;
        while (j < mot.size() && ligne[i + j] == mot[j])
            j++;
        if (j == mot.size())
            return true;
    }
    return false;
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
        if (motExiste(ligne, mot))
        {
            nbOcc++;
            cout << "Trouve ligne " << numLigne << " : " << ligne << endl;
        }
    }
    if (nbOcc > 0)
        cout << "Le mot existe dans le fichier (" << nbOcc << " ligne(s))." << endl;
    else
        cout << "Le mot n'existe pas dans le fichier." << endl;
    return 0;
}
