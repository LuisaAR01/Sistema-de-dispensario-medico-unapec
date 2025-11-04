#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <cctype>
#include <algorithm>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

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

    cout << "ID invalido." << endl;
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
            cout << "Ingrese un numero valido.\n";
            continue;
        }

        opcion = stoi(input);
        return opcion;
    }
}

string hashPassword(const string &password)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(password.c_str()), password.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

int main()
{
    sqlite3 *DB;
    int opcionPrincipal, opcionTF, opcionMarca, opcionUbi, opcionMedicamento, opcionPaci, opcionMed;
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
        "Estado TEXT NOT NULL);";

    ejecutarSQL(DB, sqlCreate);

    cout << "\n¿Deseas registrar un nuevo usuario? (s/n): ";
    char r;
    cin >> r;
    if (r == 's' || r == 'S')
    {
        string nuevoUsuario, nuevaPass, rol;
        cout << "Nombre de usuario: ";
        cin >> nuevoUsuario;
        cout << "Contraseña: ";
        cin >> nuevaPass;
        cout << "Rol (admin/usuario): ";
        cin >> rol;

        string hashedPass = hashPassword(nuevaPass);

        sqlite3_stmt *stmt;
        string sqlNuevo = "INSERT INTO usuarios (nombre, contrasena, rol) VALUES(?, ?, ?);";
        if (sqlite3_prepare_v2(DB, sqlNuevo.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, nuevoUsuario.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, hashedPass.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, rol.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            cout << "Usuario registrado correctamente.\n";
        }
        else
        {
            cerr << "Error al registrar usuario.\n";
        }
    }

    // ======== LOGIN DE USUARIOS ========
    // Crear tabla de usuarios si no existe
    const char *sqlUsuarios =
        "CREATE TABLE IF NOT EXISTS usuarios("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nombre TEXT NOT NULL UNIQUE,"
        "contrasena TEXT NOT NULL,"
        "rol TEXT NOT NULL"
        ");";
    ejecutarSQL(DB, sqlUsuarios);

    // Insertar usuario admin por defecto si no existe
    string hashedAdmin = hashPassword("1234");
    string sqlInsertAdmin = "INSERT OR IGNORE INTO usuarios (id, nombre, contrasena, rol) VALUES (1, 'admin', ?, 'admin');";
    sqlite3_stmt *stmtAdmin;
    if (sqlite3_prepare_v2(DB, sqlInsertAdmin.c_str(), -1, &stmtAdmin, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmtAdmin, 1, hashedAdmin.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmtAdmin);
    }
    sqlite3_finalize(stmtAdmin);

        cout << "===== LOGIN =====" << endl;
    string usuario, contrasena;
    bool loginExitoso = false;
    string rolUsuario;

    for (int intentos = 0; intentos < 3 && !loginExitoso; intentos++)
    {
        cout << "Usuario: ";
        cin >> usuario;
        cout << "Contraseña: ";
        cin >> contrasena;

        string hashedPass = hashPassword(contrasena);

        string sqlLogin = "SELECT rol FROM usuarios WHERE nombre = ? AND contrasena = ?;";
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(DB, sqlLogin.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, usuario.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, hashedPass.c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                rolUsuario = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
                loginExitoso = true;
            }
        }
        sqlite3_finalize(stmt);

        if (!loginExitoso)
            cout << "Credenciales incorrectas. Intentos restantes: " << (2 - intentos) << endl;
    }

    if (!loginExitoso)
    {
        cout << "Demasiados intentos fallidos. Cerrando programa." << endl;
        sqlite3_close(DB);
        return 0;
    }

    cout << "\nBienvenido, " << usuario << " (" << rolUsuario << ")" << endl;

    do
    {
        cout << "\n===== MENU PRINCIPAL =====" << endl;
        cout << "1. Tipos de Farmacos" << endl;
        cout << "2. Marcas" << endl;
        cout << "3. Ubicaciones" << endl;
        cout << "4. Medicamentos" << endl;
        cout << "5. Pacientes" << endl;
        cout << "6. Medicos" << endl;
        cout << "7. Salir" << endl;
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
                    cout << "Descripcion: ";
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
        {
            do
            {
                cout << "\n*** Medicamentos ***" << endl;
                cout << "1. Agregar" << endl;
                cout << "2. Ver" << endl;
                cout << "3. Eliminar" << endl;
                cout << "4. Volver" << endl;
                opcionMedicamento = leerOpcionSeguro();

                string Descripcion, Dosis, Estado, sql;
                int TipoFId, MarcaId, UbicacionId;
                switch (opcionMedicamento)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Ingrese una descripcion: ";
                    getline(cin, Descripcion);

                    TipoFId = seleccionTabla(DB, "TiposFarmacos", "Descripcion");
                    if (TipoFId == -1)
                    {
                        cout << "Selecciona el tipo de farmaco valido.\n";
                        break;
                    }
                    MarcaId = seleccionTabla(DB, "Marcas", "Descripcion");
                    if (MarcaId == -1)
                    {
                        cout << "Selecciona una marca valida.\n";
                        break;
                    }
                    UbicacionId = seleccionTabla(DB, "Ubicaciones", "Descripcion");
                    if (UbicacionId == -1)
                    {
                        cout << "Selecciona una ubicacion valida.\n";
                        break;
                    }
                    cin.ignore();
                    cout << "Dosis: ";
                    getline(cin, Dosis);

                    cout << "Estado: ";
                    getline(cin, Estado);

                    sql = "INSERT INTO Medicamentos (Descripcion, TipoFId, MarcaId, UbicacionId, Dosis, Estado) VALUES('" + Descripcion + "' , '" + to_string(TipoFId) + "' , '" + to_string(MarcaId) + "' , '" + to_string(UbicacionId) + "' , '" + Dosis + "' , '" + Estado + "')";

                    ejecutarSQL(DB, sql);
                    cout << "Registrado exitosamente.\n";
                    break;
                }
                case 2:
                {
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT ID, Descripcion, TipoFId, MarcaId, UbicacionId, Dosis, Estado FROM Medicamentos;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            Descripcion = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            to_string(TipoFId) = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            to_string(MarcaId) = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                            to_string(UbicacionId) = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                            Dosis = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
                            Estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

                            cout << id << " | " << Descripcion << " | " << TipoFId << " | " << MarcaId << " | " << UbicacionId << " | " << Dosis << " | " << Estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                case 3:
                {
                    int id = seleccionTabla(DB, "Medicamentos", "Descripcion");
                    if (id != -1)
                    {
                        string sql = "DELETE FROM Medicamentos WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Registro eliminado correctamente.\n";
                    }
                    break;
                }
                }

            } while (opcionMedicamento != 4);
            break;
        }

        case 5:
        {
            do
            {
                cout << "\n*** Pacientes ***" << endl;
                cout << "1. Agregar" << endl;
                cout << "2. Ver" << endl;
                cout << "3. Eliminar" << endl;
                cout << "4. Volver" << endl;
                cin >> opcionPaci;

                string Nombre, Cedula, NoCarnet, TipoPaciente, Estado, sql;
                int seleccionablelist;

                switch (opcionPaci)
                {

                case 1:
                {
                    cin.ignore();
                    cout << "Ingrese su nombre completo: ";
                    getline(cin, Nombre);
                    cout << "Ingrese su cedula: ";
                    getline(cin, Cedula);
                    cout << "Insgrese su numero de carnet: ";
                    getline(cin, NoCarnet);
                    cout << "\nSelecciona tu tipo de paciente.\n";
                    cout << "1. Estudiante.\n";
                    cout << "2. Empleado.\n";
                    cout << "3. Profesor.\n";
                    cout << "4. Otros.\n";
                    seleccionablelist = leerOpcionSeguro();

                    switch (seleccionablelist)
                    {
                    case 1:
                    {
                        TipoPaciente = "Estudiante";
                    }
                    break;
                    case 2:
                    {
                        TipoPaciente = "Empleado";
                    }
                    break;
                    case 3:
                    {
                        TipoPaciente = "Profesor";
                    }
                    break;
                    case 4:
                    {
                        TipoPaciente = "Otros";
                    }

                    default:
                        cout << "Opcion invalida, se asignará: Otros\n";
                        TipoPaciente = "Otros";
                        break;
                    }
                    cin.ignore();
                    cout << "Ingrese su estado: ";
                    getline(cin, Estado);

                    sql = "INSERT INTO Paciente (Nombre, Cedula, NoCarnet, TipoPaciente, Estado) VALUES ('" + Nombre + "' , '" + Cedula + "' , '" + NoCarnet + "' , '" + TipoPaciente + "' , '" + Estado + "');";

                    ejecutarSQL(DB, sql);
                    cout << "Resgistrado exitosamente.\n";
                    break;
                }

                case 2:
                {
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT ID, Nombre, Cedula, NoCarnet, TipoPaciente, Estado FROM Paciente;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            Nombre = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            Cedula = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            NoCarnet = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                            TipoPaciente = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                            Estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

                            cout << id << " | " << Nombre << " | " << Cedula << " | " << NoCarnet << " | " << TipoPaciente << " | " << Estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                    int id;
                case 3:
                {
                    id = seleccionTabla(DB, "Paciente", "Nombre");
                    if (id != -1)
                    {
                        sql = "DELETE FROM Paciente WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Registro eliminado correctamente.\n";
                    }
                    break;
                }
                }

            } while (opcionPaci != 4);
            break;
        }

        case 6:
        {
            do
            {

                cout << "\n*** Medico ***" << endl;
                cout << "1. Agregar" << endl;
                cout << "2. Ver" << endl;
                cout << "3. Eliminar" << endl;
                cout << "4. Volver" << endl;
                opcionMed = leerOpcionSeguro();

                string Nombre, Cedula, Tanda, Especialidad, Estado, sql;
                switch (opcionMed)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Ingrese su nombre completo: ";
                    getline(cin, Nombre);
                    cout << "Ingrese su cedula: ";
                    getline(cin, Cedula);
                    cout << "Ingrese su Tanda: ";
                    getline(cin, Tanda);
                    cout << "Ingrese su Especialidad: ";
                    getline(cin, Especialidad);
                    cout << "Ingrese su Estado: ";
                    getline(cin, Estado);

                    sql = "INSERT INTO Medicos (Nombre, Cedula, Tanda, Especialidad, Estado) VALUES ('" + Nombre + "' , '" + Cedula + "' , '" + Tanda + "' , '" + Especialidad + "' , '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Resgistrado exitosamente.\n";
                    break;
                }
                case 2:
                {
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT ID, Nombre, Cedula, Tanda, Especialidad, Estado FROM Medicos;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            Nombre = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            Cedula = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            Tanda = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                            Especialidad = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                            Estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

                            cout << id << " | " << Nombre << " | " << Cedula << " | " << Tanda << " | " << Especialidad << " | " << Estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    break;
                }
                    int id;
                case 3:
                {
                    id = seleccionTabla(DB, "Medicos", "Nombre");
                    if (id != -1)
                    {
                        sql = "DELETE FROM Medicos WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Registro eliminado correctamente.\n";
                    }
                    break;
                }
                }
            } while (opcionMed != 4);
            break;
        }
        case 7:
            cout << "Saliendo..." << endl;
            break;

        default:
            cout << "Opcion no valida.\n";
            break;
        }
    } while (opcionPrincipal != 7);

    sqlite3_close(DB);
    return 0;
}
