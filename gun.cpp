#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <utility>

using namespace std;

// Función para ejecutar comandos SQL sin resultados
void ejecutarSQL(sqlite3 *DB, const char *sql)
{
    char *errorMessage;
    int exit = sqlite3_exec(DB, sql, NULL, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        cerr << "Error SQL: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
    else
    {
        cout << "Realizado con exito." << endl;
    }
}

// Este codigo es para seleccionar un registro cualquiera de una tabla (Hable bien parce)
int seleccionTabla(sqlite3 *DB, const string &tabla, const string &campo)
{
    vector<pair<int, string>> registros;
    string sql = "SELECT ID, " + campo + " FROM " + tabla + ";";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int id = sqlite3_column_int(stmt, 0);
            string nombre = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            registros.push_back({id, nombre});
        }
    }
    sqlite3_finalize(stmt);

    if (registros.empty())
    {
        cout << "No hay registros en " << tabla << endl;
        return -1;
    }

    cout << "\n=== Lista de " << tabla << "===\n";
    for (size_t i = 0; i < registros.size(); i++)
    {
        cout << registros[i].first << " , " << registros[i].second << endl;
    }

    int opcion;
    cout << "seleccione el ID: ";
    cin >> opcion;

    for (auto &r : registros)
    {
        if (r.first == opcion)
        {
            return opcion;
        }
    }

    cout << "ID invalido\n";
    return -1;
}

int main()
{
    sqlite3 *DB;
    int opcionPrincipal, opcionTF, opcionMarca, opcionUbi, opcionMedicamento, opcionP, opcionMedico, opcionV, opcionCriterio, opcionReporteV;
    int exit = sqlite3_open("mi_base.db", &DB);

    if (exit)
    {
        cerr << "Error al abrir la base de datos." << endl;
        return -1;
    }

    // Crear tabla si no existe
    const char *sqlCreate = "CREATE TABLE IF NOT EXISTS TiposFarmacos("
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
                            "Estado TEXT NOT NULL);"

                            "CREATE TABLE IF NOT EXISTS Medicamentos("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "Descripcion TEXT NOT NULL,"
                            "TipoFId INTEGER,"
                            "MarcaId INTEGER,"
                            "UbicacionId INTEGER,"
                            "Dosis TEXT NOT NULL,"
                            "Estado TEXT NOT NULL,"
                            "FOREIGN KEY(TipoFId) REFERENCES TiposFarmacos(ID),"
                            "FOREIGN KEY(MarcaId) REFERENCES Marcas(ID),"
                            "FOREIGN KEY(UbicacionId) REFERENCES Ubicaciones(ID));"

                            "CREATE TABLE IF NOT EXISTS Paciente("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "Nombre TEXT NOT NULL,"
                            "Cedula TEXT NOT NULL,"
                            "NoCarnet TEXT NOT NULL,"
                            "TipoPaciente TEXT NOT NULL,"
                            "Estado TEXT NOT NULL);"

                            "CREATE TABLE IF NOT EXISTS Medicos("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "Nombre TEXT NOT NULL,"
                            "Cedula TEXT NOT NULL,"
                            "Tanda TEXT NOT NULL,"
                            "Especialidad TEXT NOT NULL,"
                            "Estado TEXT NOT NULL);"

                            "CREATE TABLE IF NOT EXISTS Visitas("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "MedicoId INTEGER,"
                            "paciente INTEGER,"
                            "Fecha TEXT NOT NULL,"
                            "Hora TEXT NOT NULL,"
                            "Sintomas TEXT NOT NULL,"
                            "MedicamentoSuministrado TEXT,"
                            "Recomendaciones TEXT ,"
                            "Estado TEXT NOT NULL,"
                            "FOREIGN KEY(MedicoId) REFERENCES Medicos(ID),"
                            "FOREIGN KEY(Paciente) REFERENCES Paciente(ID));";

    ejecutarSQL(DB, sqlCreate);

    do
    {
        cout << "\n===== MENU PRINCIPAL =====" << endl;
        cout << "1. Gestion de Tipos de Farmacos(pastillas, capsulas, jarabe, etc.)" << endl;
        cout << "2. Gestion de Marcas (Labotorios fabricantes)" << endl;
        cout << "3. Gestion de Ubicaciones" << endl;
        cout << "4. Gestion de Medicamentos" << endl;
        // cout << "5. Gestion de Pacientes" << endl;
        // cout << "6. Gestion de Medicos" << endl;
        // cout << "7. Registro de Visitas" << endl;
        // cout << "8. Una consulta por criterios(filtro por topicos)" << endl;
        // cout << "9. Reporte de Visitas" << endl;
        cout << "10. Salir de eta baina" << endl;
        cout << "Selecciona una opcion por favor: ";
        cin >> opcionPrincipal;

        switch (opcionPrincipal)
        {
        case 1:
        {
            do
            {
                cout << "\n***TIPOS DE FARMACOS***" << endl;
                cout << "1. Agregar tipo de farmaco" << endl;
                cout << "2. Ver Tipos de farmacos" << endl;
                cout << "3. Eliminar tipo de farmaco" << endl;
                cout << "4. Volver al menú" << endl;
                cin >> opcionTF;

                string Descripcion, Estado, sqlInsert;
                int ID;
                switch (opcionTF)
                {
                case 1:
                {

                    cout << "*** Tipos de Farmacos ***" << endl;
                    cout << "Puedes cancelar con la palabra 'CANCELAR'" << endl;
                    cout << "Ingrese una descripcion del tipo de farmaco: ";
                    cin.ignore();
                    getline(cin, Descripcion);

                    if (Descripcion == "CANCELAR" || Descripcion == "cancelar")
                    {
                        cout << "cancelado" << endl;
                        break;
                    }

                    cout << "Ingrese estado: ";
                    cin.ignore();
                    getline(cin, Estado);

                    char confirm;
                    cout << "Quieres guardar esto? (s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {
                        sqlInsert = "INSERT INTO TiposFarmacos (Descripcion, Estado) VALUES('" + Descripcion + "', '" + Estado + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "Realizado" << endl;
                    }
                    else
                    {
                        cout << "Cancelado" << endl;
                    }
                    break;
                }
                case 2:
                {
                    cout << "*** Tipos de Fármacos ***" << endl;
                    const char *sqlSelect = "SELECT ID, Descripcion, Estado FROM TiposFarmacos;";

                    sqlite3_stmt *stmt;
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, 0) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            string descripcion = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            string estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

                            cout << id << " | " << descripcion << " | Estado: " << estado << endl;
                        }
                        if (!hayDatos)
                        {
                            cout << "No hay tipos de fármacos registrados." << endl;
                        }
                    }
                    else
                    {
                        cerr << "Error al ejecutar SELECT: " << sqlite3_errmsg(DB) << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {

                    cout << "*** Tipos de Farmacos ***" << endl;
                    cout << "Cancelalo con el cero (0)" << endl;
                    cout << "Ingrese el ID del tipo de farmaco a eliminar: ";
                    cin >> ID;

                    if (ID == 0)
                    {
                        cout << "cancelado" << endl;
                        break;
                    }

                    char confirm;
                    cout << "¿Estas seguro de eliminar el ID?" << ID << "(s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {

                        sqlInsert = "DELETE  FROM TiposFarmacos ( '" + to_string(ID) + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "Eliminado con exito" << endl;
                    }
                    // :)
                    else
                    {
                        cout << "Se cancelo la eliminación" << endl;
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
                cout << "\n*** Marca ***" << endl;
                cout << "1. Agregar Marca" << endl;
                cout << "2. Ver Marcas" << endl;
                cout << "3. Eliminar Marca" << endl;
                cout << "4. Volver al menú" << endl;
                cin >> opcionMarca;

                string Descripcion, Estado, sqlInsert;
                int ID;
                switch (opcionMarca)
                {
                case 1:
                {

                    cout << "*** Marca ***" << endl;
                    cout << "Cancelalo con la palabra CANCELAR" << endl;
                    cout << "Ingrese una descripcion de la marca : ";
                    cin.ignore();
                    getline(cin, Descripcion);
                    if (Descripcion == "CANCELAR" || Descripcion == "cancelar")
                    {
                        cout << "cancelado" << endl;
                        break;
                    }

                    cout << "Ingrese estado: ";
                    cin.ignore();
                    getline(cin, Estado);

                    char confirm;
                    cout << "Quieres guardar esto? (s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {
                        sqlInsert = "INSERT INTO Marcas (Descripcion, Estado) VALUES('" + Descripcion + "', '" + Estado + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "Realizado con exito" << endl;
                    }
                    else
                    {
                        cout << "Se ha cancelado con exito" << endl;
                    }
                    break;
                }
                case 2:
                {
                    cout << "*** Marca ***" << endl;
                    const char *sqlSelect = "SELECT ID, Descripcion, Estado FROM Marcas;";

                    sqlite3_stmt *stmt;
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, 0) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            string descripcion = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            string estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

                            cout << id << " | " << descripcion << " | Estado: " << estado << endl;
                        }
                        if (!hayDatos)
                        {
                            cout << "⚠️ No hay tipos de fármacos registrados." << endl;
                        }
                    }
                    else
                    {
                        cerr << "Error al ejecutar SELECT: " << sqlite3_errmsg(DB) << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {

                    cout << "*** Marca ***" << endl;
                    cout << "Cancelalo con el cero (0)" << endl;
                    cout << "Ingrese el ID del tipo de la marca a eliminar: ";
                    cin >> ID;

                    if (ID == 0)
                    {
                        cout << "cancelado" << endl;
                        break;
                    }

                    char confirm;
                    cout << "¿Estas seguro de eliminar el ID?" << ID << "(s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {
                        sqlInsert = "DELETE  FROM Marcas ( '" + to_string(ID) + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "Eliminado con exito" << endl;
                    }
                    else
                    {
                        cout << "Se cancelo la eliminacion" << endl;
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
                cout << "1. Agregar Ubicacion" << endl;
                cout << "2. Ver Ubicaciones" << endl;
                cout << "3. Eliminar Ubicacion" << endl;
                cout << "4. Volver al menú" << endl;
                cin >> opcionUbi;

                string Descripcion, Estado, sqlInsert, Estante, Tramo, Celda;
                int ID;
                switch (opcionUbi)
                {
                case 1:
                {

                    cout << "*** Ubicaciones ***" << endl;
                    cout << "Puede cancelar con la palabra 'CANCELAR'" << endl;
                    cout << "Ingrese una descripcion de la Ubicacion: ";
                    cin.ignore();
                    getline(cin, Descripcion);
                    if (Descripcion == "CANCELAR" || Descripcion == "cancelar")
                    {
                        cout << "cancelado" << endl;
                        break;
                    }
                    cout << "Ingrese su Estante: ";
                    cin.ignore();
                    getline(cin, Estante);
                    cout << "Ingrese su Tramo:";
                    cin.ignore();
                    getline(cin, Tramo);
                    cout << "Ingresa su Celda: ";
                    cin.ignore();
                    getline(cin, Celda);
                    cout << "Ingrese su Estado: ";
                    cin.ignore();
                    getline(cin, Estado);

                    char confirm;
                    cout << "Quieres guardar esto? (s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {
                        sqlInsert = "INSERT INTO Ubicaciones (Descripcion, Estante, Tramo, Celda, Estado) VALUES('" + Descripcion + "', '" + Estante + "', '" + Tramo + "', '" + Celda + "', '" + Estado + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "Realizado con exito" << endl;
                    }
                    else
                    {
                        cout << "Cancelado" << endl;
                    }
                    break;
                }
                case 2:
                {
                    cout << "*** Ubicacion ***" << endl;
                    const char *sqlSelect = "SELECT ID, Descripcion, Estante, Tramo, Celda, Estado FROM Ubicaciones;";

                    sqlite3_stmt *stmt;
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, 0) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            string descripcion = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            string estante = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            string tramo = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                            string celda = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                            string estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

                            cout << id << " | Descripcion: " << descripcion << " | Estante: " << estante << " | Tramo: " << tramo << " | Celda: " << celda << " | Estado: " << estado << endl;
                        }
                        if (!hayDatos)
                        {
                            cout << "⚠️ No hay tipos de fármacos registrados." << endl;
                        }
                    }
                    else
                    {
                        cerr << "Error al ejecutar SELECT: " << sqlite3_errmsg(DB) << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {

                    cout << "*** Ubicaciones ***" << endl;
                    cout << "Cancelalo con el cero (0)" << endl;
                    cout << "Ingrese el ID del tipo de farmaco a eliminar: ";
                    cin >> ID;

                    if (ID == 0)
                    {
                        cout << "Se ha cancelado" << endl;
                        break;
                    }

                    char confirm;
                    cout << "¿Estas seguro de eliminar el ID?" << ID << "(s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {
                        sqlInsert = "DELETE  FROM Ubicaciones ( '" + to_string(ID) + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "eliminado exitosamente(si existia)" << endl;
                    }
                    else
                    {
                        cout << "Se cancelo la eliminacion" << endl;
                    }
                    break;
                }
                }

            } while (opcionUbi != 4);
            break;
        }
        case 4:
        {
            do
            {

                cout << "\n*** Medicamentos ***" << endl;
                cout << "1. Agregar Medicamento" << endl;
                cout << "2. Ver medicamentos" << endl;
                cout << "3. Eliminar Medicamento" << endl;
                cout << "4. Volver al menú" << endl;
                cin >> opcionMedicamento;

                string Descripcion, sqlInsert, Dosis, Estado;
                int ID;
                switch (opcionMedicamento)
                {
                case 1:
                {

                    cout << "*** Medicamentos ***" << endl;
                    cout << "Puede cancelar con la palabra 'CANCELAR'" << endl;
                    cout << "Ingresa cuantas dosis tiene que tomarse el infeliz: ";
                    cin.ignore();
                    getline(cin, Dosis);
                    if (Dosis == "CANCELAR" || Dosis == "cancelar")
                    {
                        cout << "cancelado" << endl;
                        break;
                    }
                    cout << "Ingresa el estado: ";
                    cin.ignore();
                    getline(cin, Estado);
                    cout << "Ingrese una descripcion del tipo de farmaco: ";
                    cin.ignore();
                    getline(cin, Descripcion);
                    int TipoFId = seleccionTabla(DB, "TiposFarmacos", "Descripcion");
                    int MarcaId = seleccionTabla(DB, "Marcas", "Descripcion");
                    int UbicacionId = seleccionTabla(DB, "Ubicaciones", "Descripcion");

                    if (TipoFId == -1 || MarcaId == -1 || UbicacionId == -1)
                    {
                        cout << " No se pudo registrar porque faltan datos" << endl;
                        break;
                    }

                    char confirm;
                    cout << "Quieres guardar esto? (s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {

                        sqlInsert = "INSERT INTO Medicamentos (Descripcion, Dosis, Estado, TipoFId, MarcaId, UbicacionId) VALUES('" + Descripcion + "', '" + Dosis + "', '" + Estado + "', '" + to_string(TipoFId) + "', '" + to_string(MarcaId) + "', '" + to_string(UbicacionId) + "');";

                        ejecutarSQL(DB, sqlInsert.c_str());
                        cout << "Realizado con exito" << endl;
                    }
                    else
                    {
                        cout << "Cancelado" << endl;
                    }
                    break;
                }
                case 2:
                {
                    cout << "*** Medicamentos ***" << endl;
                    const char *sqlSelect =
                        "SELECT M.ID, M.Descripcion, T.Descripcion AS TipoDesc, Ma.Descripcion AS MarcaDesc, U.Descripcion AS UbicacionDesc "
                        "FROM Medicamentos M "
                        "LEFT JOIN TiposFarmacos T ON M.TipoFID = T.ID "
                        "LEFT JOIN Marcas Ma ON M.MarcaID = Ma.ID "
                        "LEFT JOIN Ubicaciones U ON M.UbicacionID = U.ID;";

                    sqlite3_stmt *stmt = nullptr;
                    int rc = sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, NULL);
                    if (rc != SQLITE_OK)
                    {
                        cerr << "Error al preparar SELECT: " << sqlite3_errmsg(DB) << endl;
                        break;
                    }

                    bool any = false;
                    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
                    {
                        any = true;
                        int id = sqlite3_column_int(stmt, 0);

                        const unsigned char *pNombre = sqlite3_column_text(stmt, 1);
                        const unsigned char *pTipo = sqlite3_column_text(stmt, 2);
                        const unsigned char *pMarca = sqlite3_column_text(stmt, 3);
                        const unsigned char *pUbic = sqlite3_column_text(stmt, 4);

                        string nombre = pNombre ? reinterpret_cast<const char *>(pNombre) : "Vacio/Nulo";
                        string tipo = pTipo ? reinterpret_cast<const char *>(pTipo) : "Vacio/Nulo";
                        string marca = pMarca ? reinterpret_cast<const char *>(pMarca) : "Vacio/Nulo";
                        string ubic = pUbic ? reinterpret_cast<const char *>(pUbic) : "Vacio/Nulo";

                        cout << id << " | " << nombre
                             << " | Tipo: " << tipo
                             << " | Marca: " << marca
                             << " | Ubicación: " << ubic << endl;
                    }

                    if (!any)
                    {
                        cout << "No hay medicamentos registrados." << endl;
                    }
                    else if (rc != SQLITE_DONE)
                    {
                        cerr << "Error al recorrer resultados: " << sqlite3_errmsg(DB) << endl;
                    }

                    sqlite3_finalize(stmt);
                    break;
                }

                case 3:
                {
                    int ID;
                    cout << "*** Medicamentos ***" << endl;
                    cout << "Cancelalo con el cero (0)" << endl;
                    cout << "Ingrese el ID del medicamento a eliminar: ";
                    cin >> ID;

                    if (ID == 0)
                    {
                        cout << "cancelado" << endl;
                        break;
                    }

                    char confirm;
                    cout << "¿Estas seguro de eliminar el ID?" << ID << "(s/n): ";
                    cin >> confirm;

                    if (confirm == 's' || confirm == 'S')
                    {
                        string sqlDelete = "DELETE FROM Medicamentos WHERE ID = " + to_string(ID) + ";";
                        ejecutarSQL(DB, sqlDelete.c_str());
                        cout << " Medicamento eliminado" << endl;
                    }
                    else
                    {
                        cout << "Se cancelo la eliminacion" << endl;
                    }
                    break;
                }
                case 4:
                {
                    cout << "🔙 Volviendo al menu principal..." << endl;
                    break;

                default:
                    cout << "Opcion incorecta o inexistente" << endl;
                }
                }
            } while (opcionMedicamento != 4);
        }
        case 5:
        {
        }
        default:
            cout << "Opcion incorecta o inexistente" << endl;
            break;
        }

    } while (opcionPrincipal != 10);

    return 0;
}