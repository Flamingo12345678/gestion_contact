#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <sql lite3.h>

using namespace std;

//fonction pour initialiser la base de donnees
sqlite3* initDatabase(const string& filename = "Gestion_Contact.db")
{
    sqlite3* db;
    int rc = sqlite3_open(filename, &db);
    if (rc)
    {
        cerr << "Erreur lors de l'ouverture de la base de données : " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    return db;
}

// Definition de la classe Contact
class Contact
{
public:
  string nom;
  string prenom;
  string numeroTelephone;
  string email;
  string ville;
  string pays;

  // Constructeur
  Contact(string n, string p, string num, string e, string v, string c)
      : nom(n), prenom(p), numeroTelephone(num), email(e), ville(v), pays(c) {}
  // Affichage des informations du contact
  void afficher() const
  {
    cout << ", Nom : " << nom
         << ", Prénom : " << prenom
         << ", Téléphone : " << numeroTelephone
         << ", E-mail : " << email
         << ", Ville : " << ville
         << ", Pays : " << pays << endl;
  }
};

// Fonction pour charger les contacts depuis un fichier
void chargerContacts(vector<Contact> &contacts, const string &filename = "contacts.txt")
{
  ifstream fichier(filename);
  if (fichier.is_open())
  {
    string nom, prenom, numero, email, ville, pays;
    while (fichier >> nom >> prenom >> numero >> email >> ville >> pays)
    {
      contacts.emplace_back(nom, prenom, numero, email, ville, pays);
    }
    fichier.close();
  }
}
// Fonction pour sauvegarder les contacts dans un fichier
void sauvegarderContacts(const vector<Contact> &contacts, const string &filename = "contacts.txt")
{
  ofstream fichier(filename);
  if (fichier.is_open())
  {
    for (const auto &contact : contacts)
    {
      fichier << contact.nom << " " << contact.prenom << " "
              << contact.numeroTelephone << " " << contact.email << " "
              << contact.ville << " " << contact.pays << endl;
    }
    fichier.close();
  }
}

// Fonction pour ajouter un contact
void ajouterContact(vector<Contact> &contacts)
{
  string nom, prenom, numero, email, ville, pays;
  cout << "Entrez votre Nom : ";
  cin >> nom;
  cout << "Entrez votre premier Prénom : ";
  cin >> prenom;
  prenom[0] = toupper(prenom[0]); // Mettre la première lettre en majuscule

  string prenom2;
  cout << "Entrez votre deuxième Prénom (appuyez sur Entrée si vous n'en avez pas) : ";
  cin.ignore();          // Ignore the newline character left in the buffer
  getline(cin, prenom2); // Permet de lire une ligne entière
  if (!prenom2.empty())
  {
    prenom2[0] = toupper(prenom2[0]); // Mettre la première lettre en majuscule
    prenom += " " + prenom2;
  }

  // Vérification de la validité du numéro de téléphone
  bool numeroValide = false;
  do
  {
    cout << "Entrez votre Numéro de Téléphone (format: XX-XX-XX-XX-XX) : ";
    cin >> numero;
    if (numero.size() == 14 && numero[2] == '-' && numero[5] == '-' && numero[8] == '-' && numero[11] == '-')
    {
      numeroValide = true;
    }
    else
    {
      cout << "Numéro de téléphone invalide. Veuillez réessayer." << endl;
    }
  } while (!numeroValide);

  // Vérification de la validité de l'email
  bool emailValide = false;
  do
  {
    cout << "Entrez votre E-mail : ";
    cin >> email;

    // Vérification basique de la validité de l'email
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    if (atPos != string::npos && dotPos != string::npos && dotPos > atPos)
    {
      emailValide = true;
    }
    else
    {
      cout << "Adresse e-mail invalide. Veuillez réessayer." << endl;
    }
  } while (!emailValide);

  cout << "Entrez votre Ville : ";
  cin >> ville;

  cout << "Entrez votre Pays : ";
  cin >> pays;

  contacts.emplace_back(nom, prenom, numero, email, ville, pays);
  cout << "Contact ajout� avec succ�s !" << endl;
}

// Fonction pour afficher tous les contacts
void afficherContacts(const vector<Contact> &contacts)
{
  if (contacts.empty())
  {
    cout << "Aucun contact � afficher." << endl;
  }
  else
  {
    for (const auto &contact : contacts)
    {
      contact.afficher();
    }
  }
}

// Fonction pour mettre � jour un contact
void mettreAJourContact(vector<Contact> &contacts)
{
  string nom;
  cout << "Entrez le Nom du contact � mettre � jour : ";
  cin >> nom;

  for (auto &contact : contacts)
  {
    if (contact.nom == nom)
    {
      cout << "Entrez le nouveau Nom : ";
      cin >> contact.nom;

      cout << "Entrez le nouveau Pr�nom : ";
      cin >> contact.prenom;

      cout << "Entrez le nouveau Num�ro de T�l�phone : ";
      cin >> contact.numeroTelephone;

      cout << "Entrez le nouvel E-mail : ";
      cin >> contact.email;

      cout << "Entrez votre Ville : ";
      cin >> contact.ville;

      cout << "Entrez votre Pays : ";
      cin >> contact.pays;

      cout << "Contact mis � jour avec succ�s !" << endl;
      return;
    }
  }
  cout << "Contact introuvable." << endl;
}

// Fonction pour supprimer un contact
void supprimerContact(vector<Contact> &contacts)
{
  string nom;

  cout << "Entrez le Nom du contact � supprimer : ";
  cin >> nom;

  auto it = remove_if(contacts.begin(), contacts.end(),
                      [&nom](const Contact &contact)
                      {
                        return contact.nom == nom;
                      });

  if (it != contacts.end())
  {
    contacts.erase(it, contacts.end());
    cout << "Contact supprim� avec succ�s !" << endl;
  }
  else
  {
    cout << "Contact introuvable." << endl;
  }
}

// Fonction principale pour g�rer le menu
void afficherMenu()
{
  cout << "\nMenu de Gestion des Contacts :\n";
  cout << "1. Ajouter un contact\n";
  cout << "2. Afficher les contacts\n";
  cout << "3. Mettre � jour un contact\n";
  cout << "4. Supprimer un contact\n";
  cout << "5. Charger les contacts\n";
  cout << "6. Sauvegarder et Quitter\n";
  cout << "Quel est votre choix : ";
}

int main()
{
  vector<Contact> contacts;

  // Charger les contacts depuis le fichier
  chargerContacts(contacts, "contacts.txt");

  int choix;
  do
  {
    afficherMenu();
    cin >> choix;

    switch (choix)
    {
    case 1:
      ajouterContact(contacts);
      break;
    case 2:
      afficherContacts(contacts);
      break;
    case 3:
      mettreAJourContact(contacts);
      break;
    case 4:
      supprimerContact(contacts);
      break;
    case 5:
      chargerContacts(contacts, "contacts.txt");
      cout << "Contacts chargés avec succès !" << endl;
      break;
    case 6:
      sauvegarderContacts(contacts, "contacts.txt");
      cout << "Contacts sauvegardés. Au revoir !" << endl;
      break;
    default:
      cout << "Choix invalide. Veuillez réessayer." << endl;
    }

  } while (choix != 6);

  return 0;
}
