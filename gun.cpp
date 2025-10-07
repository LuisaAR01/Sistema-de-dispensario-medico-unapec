#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

// Ejecutar comandos SQL sin resultados
void ejecutarSQL(sqlite3 *DB, const string &sql)
{
    char *errorMessage = nullptr;
    int exit = sqlite3_exec(DB, sql.c_str(), nullptr, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error SQL: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

// Mostrar registros y devolver ID seleccionado
int seleccionTabla(sqlite3 *DB, const string &tabla, const string &campo)
{
    vector<pair<int, string>> registros;
    string sql = "SELECT ID, " + campo + " FROM " + tabla + ";";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *texto = sqlite3_column_text(stmt, 1);
            string valor = texto ? reinterpret_cast<const char *>(texto) : "(sin nombre)";
            registros.push_back({id, valor});
        }
    }
    sqlite3_finalize(stmt);

    if (registros.empty())
    {
        cout << "No hay registros en " << tabla << "." << endl;
        return -1;
    }

    cout << "\n=== " << tabla << " ===" << endl;
    for (auto &r : registros)
    {
        cout << r.first << " - " << r.second << endl;
    }

    cout << "Ingrese el ID: ";
    int opcion;
    cin >> opcion;

    for (auto &r : registros)
    {
        if (r.first == opcion)
            return opcion;
    }

    cout << "ID inválido." << endl;
    return -1;
}

// Leer opción numérica de forma segura
int leerOpcionSeguro()
{
    string input;
    int opcion;

    while (true)
    {
        cout << "Seleccione una opcion: ";
        cin >> input;

        bool esNumero = !input.empty() && all_of(input.begin(), input.end(), ::isdigit);
        if (!esNumero)
        {
            cout << "Ingrese un número valido.\n";
            continue;
        }

        opcion = stoi(input);
        return opcion;
    }
}

int main()
{
    sqlite3 *DB;
    int opcionPrincipal, opcionTF, opcionMarca, opcionUbi;
    int exit = sqlite3_open("mi_base.db", &DB);

    if (exit)
    {
        cerr << "Error al abrir la base de datos." << endl;
        return -1;
    }

    // Crear tablas si no existen
    const char *sqlCreate =
        "CREATE TABLE IF NOT EXISTS TiposFarmacos("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Descripcion TEXT NOT NULL,"
        "Estado TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS Marcas("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Descripcion TEXT NOT NULL,"
        "Estado TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS Ubicaciones("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Descripcion TEXT NOT NULL,"
        "Estante TEXT NOT NULL,"
        "Tramo TEXT NOT NULL,"
        "Celda TEXT NOT NULL,"
        "Estado TEXT NOT NULL);";

    ejecutarSQL(DB, sqlCreate);

    do
    {
        cout << "\n===== MENU PRINCIPAL =====" << endl;
        cout << "1. Tipos de Farmacos" << endl;
        cout << "2. Marcas" << endl;
        cout << "3. Ubicaciones" << endl;
        cout << "4. Salir" << endl;
        opcionPrincipal = leerOpcionSeguro();

        switch (opcionPrincipal)
        {
        case 1:
        {
            do
            {
                cout << "\n*** Tipos de Farmacos ***" << endl;
                cout << "1. Agregar" << endl;
                cout << "2. Ver" << endl;
                cout << "3. Eliminar" << endl;
                cout << "4. Volver" << endl;
                opcionTF = leerOpcionSeguro();

                string Descripcion, Estado;
                switch (opcionTF)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Descripcion: ";
                    getline(cin, Descripcion);
                    cout << "Estado: ";
                    getline(cin, Estado);

                    string sql = "INSERT INTO TiposFarmacos (Descripcion, Estado) VALUES('" + Descripcion + "', '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Registro agregado correctamente.\n";
                    break;
                }
                case 2:
                {
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT ID, Descripcion, Estado FROM TiposFarmacos;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            string desc = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            string estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            cout << id << " | " << desc << " | Estado: " << estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {
                    int id = seleccionTabla(DB, "TiposFarmacos", "Descripcion");
                    if (id != -1)
                    {
                        string sql = "DELETE FROM TiposFarmacos WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Registro eliminado correctamente.\n";
                    }
                    break;
                }
                }
            } while (opcionTF != 4);
            break;
        }

        case 2:
        {
            do
            {
                cout << "\n*** Marcas ***" << endl;
                cout << "1. Agregar" << endl;
                cout << "2. Ver" << endl;
                cout << "3. Eliminar" << endl;
                cout << "4. Volver" << endl;
                opcionMarca = leerOpcionSeguro();

                string Descripcion, Estado;
                switch (opcionMarca)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Descripcion: ";
                    getline(cin, Descripcion);
                    cout << "Estado: ";
                    getline(cin, Estado);

                    string sql = "INSERT INTO Marcas (Descripcion, Estado) VALUES('" + Descripcion + "', '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Registro agregado correctamente.\n";
                    break;
                }
                case 2:
                {
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT ID, Descripcion, Estado FROM Marcas;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            string desc = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            string estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            cout << id << " | " << desc << " | Estado: " << estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {
                    int id = seleccionTabla(DB, "Marcas", "Descripcion");
                    if (id != -1)
                    {
                        string sql = "DELETE FROM Marcas WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Registro eliminado correctamente.\n";
                    }
                    break;
                }
                }
            } while (opcionMarca != 4);
            break;
        }

        case 3:
        {
            do
            {
                cout << "\n*** Ubicaciones ***" << endl;
                cout << "1. Agregar" << endl;
                cout << "2. Ver" << endl;
                cout << "3. Eliminar" << endl;
                cout << "4. Volver" << endl;
                opcionUbi = leerOpcionSeguro();

                string Descripcion, Estado, Estante, Tramo, Celda;
                switch (opcionUbi)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Descripción: ";
                    getline(cin, Descripcion);
                    cout << "Estante: ";
                    getline(cin, Estante);
                    cout << "Tramo: ";
                    getline(cin, Tramo);
                    cout << "Celda: ";
                    getline(cin, Celda);
                    cout << "Estado: ";
                    getline(cin, Estado);

                    string sql = "INSERT INTO Ubicaciones (Descripcion, Estante, Tramo, Celda, Estado) VALUES('" +
                                 Descripcion + "', '" + Estante + "', '" + Tramo + "', '" + Celda + "', '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Registro agregado correctamente.\n";
                    break;
                }
                case 2:
                {
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT ID, Descripcion, Estante, Tramo, Celda, Estado FROM Ubicaciones;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            string desc = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            string estante = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            string tramo = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                            string celda = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                            string estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

                            cout << id << " | " << desc << " | " << estante << " | " << tramo << " | " << celda << " | " << estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {
                    int id = seleccionTabla(DB, "Ubicaciones", "Descripcion");
                    if (id != -1)
                    {
                        string sql = "DELETE FROM Ubicaciones WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Registro eliminado correctamente.\n";
                    }
                    break;
                }
                }
            } while (opcionUbi != 4);
            break;
        }

        case 4:
            cout << "Saliendo..." << endl;
            break;

        default:
            cout << "Opción no válida.\n";
            break;
        }
    } while (opcionPrincipal != 4);

    sqlite3_close(DB);
    return 0;
}
