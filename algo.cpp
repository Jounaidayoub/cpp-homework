
#include <iostream>
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

int main()
{
    string texte, mot;
    while (true)
    {
        cout << "Entrez la premiere chaine : ";
        if (!getline(cin, texte))
            break;
        cout << "Entrez la deuxieme chaine : ";
        if (!getline(cin, mot))
            break;

        if (contient(texte, mot))
            cout << "La deuxieme chaine existe dans la premiere." << endl;
        else
            cout << "La deuxieme chaine n'existe pas dans la premiere." << endl;
    }
    return 0;
}
