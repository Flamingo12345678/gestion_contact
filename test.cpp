#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

// Definition de la classe Contact
class Contact
{
public:
  string nom;
  string prenom;
  string numeroTelephone;
  string email;

  // Constructeur
  Contact(string n, string p, string num, string e)
      : nom(n), prenom(p), numeroTelephone(num), email(e) {}

  // Affichage des informations du contact
  void afficher() const
  {
    cout << "Nom : " << nom
         << ", Prénom : " << prenom
         << ", Téléphone : " << numeroTelephone
         << ", E-mail : " << email << endl;
  }
};

// Fonction pour charger les contacts depuis un fichier
void chargerContacts(vector<Contact> &contacts, const string &nomFichier)
{
  ifstream fichier(nomFichier);
  if (fichier.is_open())
  {
    string nom, prenom, numero, email;
    while (fichier >> nom >> prenom >> numero >> email)
    {
      contacts.emplace_back(nom, prenom, numero, email);
    }
    fichier.close();
  }
}

// Fonction pour sauvegarder les contacts dans un fichier
void sauvegarderContacts(const vector<Contact> &contacts, const string &nomFichier)
{
  ofstream fichier(nomFichier);
  if (fichier.is_open())
  {
    for (const auto &contact : contacts)
    {
      fichier << contact.nom << " " << contact.prenom << " "
              << contact.numeroTelephone << " " << contact.email << endl;
    }
    fichier.close();
  }
}

// Fonction pour ajouter un contact
void ajouterContact(vector<Contact> &contacts)
{
  string nom, prenom, numero, email;
  cout << "Entrez le nom : ";
  cin >> nom;
  cout << "Entrez le prénom : ";
  cin >> prenom;
  cout << "Entrez le numéro de téléphone : ";
  cin >> numero;
  cout << "Entrez l'e-mail : ";
  cin >> email;

  contacts.emplace_back(nom, prenom, numero, email);
  cout << "Contact ajouté avec succès !" << endl;
}

// Fonction pour afficher tous les contacts
void afficherContacts(const vector<Contact> &contacts)
{
  if (contacts.empty())
  {
    cout << "Aucun contact à afficher." << endl;
  }
  else
  {
    for (const auto &contact : contacts)
    {
      contact.afficher();
    }
  }
}

// Fonction pour mettre à jour un contact
void mettreAJourContact(vector<Contact> &contacts)
{
  string nom;
  cout << "Entrez le nom du contact à mettre à jour : ";
  cin >> nom;

  for (auto &contact : contacts)
  {
    if (contact.nom == nom)
    {
      cout << "Entrez le nouveau prénom : ";
      cin >> contact.prenom;
      cout << "Entrez le nouveau numéro de téléphone : ";
      cin >> contact.numeroTelephone;
      cout << "Entrez le nouvel e-mail : ";
      cin >> contact.email;

      cout << "Contact mis à jour avec succès !" << endl;
      return;
    }
  }
  cout << "Contact introuvable." << endl;
}

// Fonction pour supprimer un contact
void supprimerContact(vector<Contact> &contacts)
{
  string nom;
  cout << "Entrez le nom du contact à supprimer : ";
  cin >> nom;

  auto it = remove_if(contacts.begin(), contacts.end(),
                      [&nom](const Contact &contact)
                      {
                        return contact.nom == nom;
                      });

  if (it != contacts.end())
  {
    contacts.erase(it, contacts.end());
    cout << "Contact supprimé avec succès !" << endl;
  }
  else
  {
    cout << "Contact introuvable." << endl;
  }
}

// Fonction principale pour gérer le menu
void afficherMenu()
{
  cout << "\nMenu de Gestion des Contacts :\n";
  cout << "1. Ajouter un contact\n";
  cout << "2. Afficher les contacts\n";
  cout << "3. Mettre à jour un contact\n";
  cout << "4. Supprimer un contact\n";
  cout << "5. Quitter\n";
  cout << "Quel est votre choix : ";
}

int main()
{
  vector<Contact> contacts;
  const string nomFichier = "contacts.txt";

  // Charger les contacts depuis le fichier
  chargerContacts(contacts, nomFichier);

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
      sauvegarderContacts(contacts, nomFichier);
      cout << "Contacts sauvegardés. Au revoir !" << endl;
      break;
    default:
      cout << "Choix invalide. Veuillez réessayer." << endl;
    }
  } while (choix != 5);

  return 0;
}
