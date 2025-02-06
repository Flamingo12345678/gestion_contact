#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>
#include <algorithm>

using namespace std;

// Fonction pour initialiser la base de données
sqlite3 *initDatabase(const string &dbName)
{
    sqlite3 *db;
    if (sqlite3_open(dbName.c_str(), &db))
    {
        cerr << "Erreur lors de l'ouverture de la base de données : " << sqlite3_errmsg(db) << endl;
        exit(1);
    }

    // Créer la table "contacts" si elle n'existe pas
    const char *createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nom TEXT,
            prenom TEXT,
            numeroTelephone TEXT,
            email TEXT,
            ville TEXT,
            pays TEXT
        );
    )";
    char *errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        cerr << "Erreur lors de la création de la table : " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(1);
    }

    return db;
}

// Fonction pour ajouter un contact
void ajouterContact(sqlite3 *db)
{
    string nom, prenom, numero, email, ville, pays;

    cout << "Entrez votre Nom : ";
    cin >> nom;

    cout << "Entrez votre Prénom : ";
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

    string insertSQL = "INSERT INTO contacts (nom, prenom, numeroTelephone, email, ville, pays) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, nom.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, prenom.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, numero.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, ville.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, pays.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        cerr << "Erreur lors de l'ajout du contact : " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "Contact ajouté avec succès !" << endl;
    }

    sqlite3_finalize(stmt);
}

// Fonction pour afficher tous les contacts
void afficherContacts(sqlite3 *db)
{
    string selectSQL = "SELECT id, nom, prenom, numeroTelephone, email, ville, pays FROM contacts;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Erreur lors de la récupération des contacts : " << sqlite3_errmsg(db) << endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        string nom = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        string prenom = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        string numero = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        string email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
        string ville = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
        string pays = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

        cout << "ID: " << id
             << ", Nom: " << nom
             << ", Prénom: " << prenom
             << ", Téléphone: " << numero
             << ", Email: " << email
             << ", Ville: " << ville
             << ", Pays: " << pays << endl;
    }

    sqlite3_finalize(stmt);
}

// Fonction pour mettre à jour un contact
// Fonction pour mettre à jour un contact
void mettreAJourContact(sqlite3 *db)
{
    int id;
    cout << "Entrez l'ID du contact à mettre à jour : ";
    cin >> id;

    // Vérification de l'existence du contact
    string selectSQL = "SELECT * FROM contacts WHERE id = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        cout << "Contact avec ID " << id << " introuvable." << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Demander les nouvelles informations
    string nom, prenom, numero, email, ville, pays;

    cout << "Entrez le nouveau Nom (laisser vide pour conserver l'ancien) : ";
    cin.ignore();
    getline(cin, nom);

    cout << "Entrez le nouveau Prénom (laisser vide pour conserver l'ancien) : ";
    getline(cin, prenom);

    cout << "Entrez le nouveau Numéro de Téléphone (laisser vide pour conserver l'ancien) : ";
    getline(cin, numero);

    cout << "Entrez le nouvel E-mail (laisser vide pour conserver l'ancien) : ";
    getline(cin, email);

    cout << "Entrez la nouvelle Ville (laisser vide pour conserver l'ancien) : ";
    getline(cin, ville);

    cout << "Entrez le nouveau Pays (laisser vide pour conserver l'ancien) : ";
    getline(cin, pays);

    // Demander à l'utilisateur s'il souhaite continuer ou annuler la mise à jour
    char confirmation;
    cout << "Souhaitez-vous continuer la mise à jour ? (O/N) : ";
    cin >> confirmation;

    if (toupper(confirmation) == 'N')
    {
        cout << "Mise à jour annulée." << endl;
        return;
    }

    // Préparer la requête de mise à jour
    string updateSQL = R"(
        UPDATE contacts
        SET 
            nom = CASE WHEN ? <> '' THEN ? ELSE nom END,
            prenom = CASE WHEN ? <> '' THEN ? ELSE prenom END,
            numeroTelephone = CASE WHEN ? <> '' THEN ? ELSE numeroTelephone END,
            email = CASE WHEN ? <> '' THEN ? ELSE email END,
            ville = CASE WHEN ? <> '' THEN ? ELSE ville END,
            pays = CASE WHEN ? <> '' THEN ? ELSE pays END
        WHERE id = ?;
    )";

    sqlite3_prepare_v2(db, updateSQL.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, nom.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, nom.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, prenom.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, prenom.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, numero.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, numero.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, ville.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, ville.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, pays.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, pays.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 13, id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        cerr << "Erreur lors de la mise à jour du contact : " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "Contact mis à jour avec succès !" << endl;
    }

    sqlite3_finalize(stmt);
}


// Fonction pour supprimer un contact
void supprimerContact(sqlite3 *db)
{
    int id;
    cout << "Entrez l'ID du contact à supprimer : ";
    cin >> id;

    string deleteSQL = "DELETE FROM contacts WHERE id = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        cerr << "Erreur lors de la suppression du contact : " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "Contact supprimé avec succès !" << endl;
    }

    sqlite3_finalize(stmt);
}

// Fonction principale
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
    // Initialiser la base de données avec le nom "Gestion_Contact.db"
    sqlite3 *db = initDatabase("Gestion_Contact.db");

    int choix;
    do
    {
        afficherMenu();
        cin >> choix;

        switch (choix)
        {
        case 1:
            ajouterContact(db);
            break;
        case 2:
            afficherContacts(db);
            break;
        case 3:
            mettreAJourContact(db);
            break;
        case 4:
            supprimerContact(db);
            break;
        case 5:
            cout << "Au revoir !" << endl;
            break;
        default:
            cout << "Choix invalide. Veuillez réessayer." << endl;
        }
    } while (choix != 5);

    sqlite3_close(db);
    return 0;
}
