#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <cctype>
#include <algorithm>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <conio.h>
#include <cstdlib> // Para system("cls")
#include <windows.h>
#include <limits>
#include <chrono>
#include <thread>

using namespace std;

const string ANSI_RESET = "\033[0m";
const string ANSI_CYAN = "\033[38;2;0;200;255m";
const string ANSI_NEON = "\033[38;2;0;255;0m";
const string ANSI_GREEN = "\033[38;2;0;255;150m";
const string ANSI_YELLOW = "\033[38;2;255;200;0m";
const string ANSI_RED = "\033[38;2;255;0;60m";

void slowPrint(const string &text, int ms = 12)
{
    for (char c : text)
    {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
}

void printCentered(const string &s, int totalWidth = 46)
{
    int pad = max(0, (totalWidth - (int)s.size()) / 2);
    for (int i = 0; i < pad; ++i)
        cout << ' ';
    cout << s << '\n';
}
// ----------------------------------------------------------------------

string inputWithESC(const string &prefijo = "")
{
    cout << prefijo;
    string result = "";
    char c;

    while (true)
    {
        c = _getch();

        if (c == 27) // ESC
        {
            return "__ESC__";
        }
        else if (c == 13) // ENTER
        {
            cout << "\n";
            return result;
        }
        else if (c == 8) // BACKSPACE
        {
            if (!result.empty())
            {
                result.pop_back();
                cout << "\b \b";
            }
        }
        else if (isprint((unsigned char)c))
        {
            result.push_back(c);
            cout << c;
        }
    }
}

string inputPasswordWithESC()
{
    string result = "";
    char c;

    while (true)
    {
        c = _getch();

        if (c == 27) // ESC
        {
            return "__ESC__";
        }
        else if (c == 13) // ENTER
        {
            cout << "\n";
            return result;
        }
        else if (c == 8) // BACKSPACE
        {
            if (!result.empty())
            {
                result.pop_back();
                cout << "\b \b";
            }
        }
        else if (isprint((unsigned char)c))
        {
            result.push_back(c);
            cout << "*"; // oculta
        }
    }
}

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

void limpiarPantalla()
{
    system("cls"); // Limpia completamente la consola en Windows
}

// ==========================
//  MENU CON FLECHAS
// ==========================
void dibujarMenuPrincipal(int pos)
{
    limpiarPantalla();

    cout << "\033[38;2;0;255;150m===== MENU PRINCIPAL =====\033[0m\n\n";

    string opciones[] = {
        "Tipos de Farmacos",
        "Marcas",
        "Ubicaciones",
        "Medicamentos",
        "Pacientes",
        "Medicos",
        "Visitas",
        "Reporte General",
        "Consulta por Criterios",
        "Salir"};

    for (int i = 0; i < 10; i++)
    {
        if (i == pos)
        {
            cout << "\033[38;2;0;255;0m>> " << opciones[i] << "\033[0m\n"; // verde neon
        }
        else
        {
            cout << "   " << opciones[i] << "\n";
        }
    }
}

int menuPrincipalConFlechas()
{
    int pos = 0;
    bool cambio = true;

    string opciones[] = {
        "Tipos de Farmacos",
        "Marcas",
        "Ubicaciones",
        "Medicamentos",
        "Pacientes",
        "Medicos",
        "Visitas",
        "Reporte General",
        "Consulta por Criterios",
        "Salir"};

    while (true)
    {
        if (cambio)
        {
            limpiarPantalla();

            cout << "\033[38;2;0;255;150m===== MENU PRINCIPAL =====\033[0m\n\n";

            for (int i = 0; i < 10; i++)
            {
                if (i == pos)
                {
                    cout << "\033[38;2;0;255;0m>> " << opciones[i] << "\033[0m\n";
                }
                else
                {
                    cout << "   " << opciones[i] << "\n";
                }
            }

            cout << "\nUsa las flechas para moverse y ENTER para seleccionar.\n";

            cambio = false;
        }

        int t = _getch();

        if (t == 224)
        {
            int flecha = _getch();

            if (flecha == 72)
            { // ↑
                pos--;
                if (pos < 0)
                    pos = 9;
                cambio = true;
            }

            if (flecha == 80)
            { // ↓
                pos++;
                if (pos > 9)
                    pos = 0;
                cambio = true;
            }
        }

        if (t == 13)
        {                   // ENTER
            return pos + 1; // Devuelve la opción seleccionada
        }
    }
}

int menuSubmenuFlechas(string titulo, string opciones[], int cantidad)
{
    int pos = 0;
    bool cambio = true;

    while (true)
    {
        if (cambio)
        {
            limpiarPantalla();

            cout << "\033[38;2;0;200;255m=== " << titulo << " ===\033[0m\n\n";

            for (int i = 0; i < cantidad; i++)
            {
                if (i == pos)
                {
                    cout << "\033[38;2;0;255;0m>> " << opciones[i] << "\033[0m\n";
                }
                else
                {
                    cout << "   " << opciones[i] << "\n";
                }
            }

            cout << "\nUsa las flechas para moverte y ENTER para seleccionar.\n";

            cambio = false;
        }

        int t = _getch();

        if (t == 224)
        {
            int flecha = _getch();

            if (flecha == 72)
            { // ↑
                pos--;
                if (pos < 0)
                    pos = cantidad - 1;
                cambio = true;
            }

            if (flecha == 80)
            { // ↓
                pos++;
                if (pos >= cantidad)
                    pos = 0;
                cambio = true;
            }
        }

        if (t == 13)
        { // ENTER
            return pos + 1;
        }
    }
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

// Leer opción numérica de forma segura
int leerOpcionSeguro1()
{
    string input;
    int opcion;

    while (true)
    {
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
    opcion = leerOpcionSeguro1();

    for (auto &r : registros)
    {
        if (r.first == opcion)
            return opcion;
    }

    cout << "ID invalido." << endl;
    return -1;
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

string inputPassword()
{
    string pass = "";
    char c;
    while ((c = _getch()) != '\r') // Enter
    {
        if (c == '\b')
        {
            if (!pass.empty())
            {
                cout << "\b \b";
                pass.pop_back();
            }
        }
        else
        {
            pass.push_back(c);
            cout << "*";
        }
    }
    cout << endl;
    return pass;
}

int main()
{
    sqlite3 *DB;
    int opcionPrincipal, opcionTF, opcionMarca, opcionUbi, opcionMedicamento, opcionPaci, opcionMed, opcionV, opR;
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
        "Estado TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS Visitas ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "MedicoId INTEGER NOT NULL, "
        "PacienteId INTEGER NOT NULL, "
        "FechaVisita TEXT NOT NULL, "
        "HoraVisita TEXT NOT NULL, "
        "Sintomas TEXT, "
        "Medicamentos TEXT, "
        "Recomendaciones TEXT, "
        "Estado TEXT, "
        "FOREIGN KEY (MedicoId) REFERENCES Medicos(ID), "
        "FOREIGN KEY (PacienteId) REFERENCES Paciente(ID)"
        ");";

    ejecutarSQL(DB, sqlCreate);

    // ========== REGISTRO DE USUARIO (MEJORADO Y ESTÉTICO) ==========

    // Preguntar si desea registrar un usuario nuevo
    cout << ANSI_CYAN;
    cout << "\n  ||==============================================||\n";
    cout << "  ||        Deseas registrar un nuevo usuario?    ||\n";
    cout << "  ||==============================================||\n";
    cout << ANSI_RESET;

    cout << ANSI_YELLOW << "  Opcion (s/n): " << ANSI_RESET;

    char r;
    cin >> r;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // limpia buffer

    if (r == 's' || r == 'S')
    {
        string nuevoUsuario, nuevaPass, rol;

        cout << ANSI_GREEN << "\n  >> Nombre de usuario: " << ANSI_RESET;
        getline(cin, nuevoUsuario);

        // CONTRASEÑA OCULTA (usa tu inputPassword())
        cout << ANSI_GREEN << "  >> Contrasena: " << ANSI_RESET;
        nuevaPass = inputPassword();
        cout << "\n";

        // VALIDACIÓN DEL ROL
        while (true)
        {
            cout << ANSI_GREEN << "  >> Rol [admin / usuario]: " << ANSI_RESET;
            getline(cin, rol);

            // Normalizar a minúsculas
            for (auto &c : rol)
                c = tolower(c);

            if (rol == "admin" || rol == "usuario")
                break;

            cout << ANSI_RED << "   Rol inválido. Debe ser 'admin' o 'usuario'.\n"
                 << ANSI_RESET;
        }

        string hashedPass = hashPassword(nuevaPass);

        sqlite3_stmt *stmt;
        string sqlNuevo = "INSERT INTO usuarios (nombre, contrasena, rol) VALUES(?, ?, ?);";

        if (sqlite3_prepare_v2(DB, sqlNuevo.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, nuevoUsuario.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, hashedPass.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, rol.c_str(), -1, SQLITE_TRANSIENT);

            sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            cout << ANSI_NEON << "\n   Usuario registrado correctamente.\n\n"
                 << ANSI_RESET;
        }
        else
        {
            cerr << ANSI_RED << "   Error al registrar usuario.\n"
                 << ANSI_RESET;
        }
    }

    // ========== CREAR TABLA USUARIOS SI NO EXISTE ==========
    const char *sqlUsuarios =
        "CREATE TABLE IF NOT EXISTS usuarios("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nombre TEXT NOT NULL UNIQUE,"
        "contrasena TEXT NOT NULL,"
        "rol TEXT NOT NULL"
        ");";
    ejecutarSQL(DB, sqlUsuarios);

    // ========== INSERTAR ADMIN POR DEFECTO ==========
    string hashedAdmin = hashPassword("1234");
    string sqlInsertAdmin =
        "INSERT OR IGNORE INTO usuarios "
        "(id, nombre, contrasena, rol) VALUES (1, 'admin', ?, 'admin');";

    sqlite3_stmt *stmtAdmin;
    if (sqlite3_prepare_v2(DB, sqlInsertAdmin.c_str(), -1, &stmtAdmin, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmtAdmin, 1, hashedAdmin.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmtAdmin);
    }
    sqlite3_finalize(stmtAdmin);

    // ======== LOGIN MEJORADO ========
    cout << "\n";

    // CABECERA LOGIN (estética 'terminal moderna' coherente)
    cout << ANSI_CYAN;
    cout << "  ||==================================================||\n";
    cout << "  ||";
    printCentered("SISTEMA DE DISPENSARIO - UNAPEC", 50);
    cout << "  ||\n";
    cout << "  ||==================================================||\n\n";
    cout << ANSI_RESET;

    slowPrint("   >> Iniciando sesion (presiona ENTER para confirmar)...\n", 8);
    cout << '\n';

    // Prepara entradas
    cin.ignore((numeric_limits<streamsize>::max)(), '\n'); // limpia buffer seguro

    string usuario, contrasena;
    bool loginExitoso = false;
    string rolUsuario;

    // Intentos permitidos
    const int MAX_INTENTOS = 3;
    int intentos = 0;

    // Funcion comparacion de tiempo-constante simple
    auto constant_time_compare = [](const string &a, const string &b) -> bool
    {
        if (a.size() != b.size())
            return false;
        volatile unsigned char res = 0;
        for (size_t i = 0; i < a.size(); ++i)
            res |= a[i] ^ b[i];
        return res == 0;
    };

    while (intentos < MAX_INTENTOS && !loginExitoso)
    {
        cout << ANSI_GREEN << "  Usuario >> " << ANSI_RESET;
        if (!getline(cin, usuario))
            break;
        // trim simple (solo extremos)
        while (!usuario.empty() && isspace((unsigned char)usuario.front()))
            usuario.erase(usuario.begin());
        while (!usuario.empty() && isspace((unsigned char)usuario.back()))
            usuario.pop_back();

        cout << ANSI_GREEN << "  Contrasena >> " << ANSI_RESET;
        contrasena = inputPassword(); // tu función actual que muestra '*'

        // Hash de la contraseña ingresada
        string hashedPass = hashPassword(contrasena);

        // Recuperar hash y rol desde la DB (evita WHERE con pass directo)
        string sqlLogin = "SELECT contrasena, rol FROM usuarios WHERE nombre = ?;";
        sqlite3_stmt *stmtLogin = nullptr;
        string stored_hash = "", stored_role = "";

        if (sqlite3_prepare_v2(DB, sqlLogin.c_str(), -1, &stmtLogin, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmtLogin, 1, usuario.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmtLogin) == SQLITE_ROW)
            {
                const unsigned char *ch = sqlite3_column_text(stmtLogin, 0);
                const unsigned char *cr = sqlite3_column_text(stmtLogin, 1);
                stored_hash = ch ? reinterpret_cast<const char *>(ch) : "";
                stored_role = cr ? reinterpret_cast<const char *>(cr) : "";
            }
        }
        sqlite3_finalize(stmtLogin);

        // Comparacion en tiempo constante (si stored_hash vacio, falla igual)
        if (!stored_hash.empty() && constant_time_compare(hashedPass, stored_hash))
        {
            loginExitoso = true;
            rolUsuario = stored_role;
            break;
        }
        else
        {
            intentos++;
            int restantes = MAX_INTENTOS - intentos;
            cout << ANSI_RED << "\n  Credenciales incorrectas" << ANSI_RESET;
            if (restantes > 0)
            {
                cout << ANSI_YELLOW << "  (Intentos restantes: " << restantes << ")" << ANSI_RESET << "\n\n";
            }
            else
            {
                cout << "\n\n";
            }
        }
    }

    if (!loginExitoso)
    {
        cout << ANSI_RED << "\n  Has superado el limite de intentos. Saliendo...\n"
             << ANSI_RESET;
        sqlite3_close(DB);
        return 0;
    }

    // Mensaje de acceso concedido (estético)
    slowPrint("  Validando credenciales...\n", 12);
    for (int i = 0; i < 3; ++i)
    {
        cout << "." << flush;
        Sleep(200);
    }
    cout << "\n\n";

    cout << ANSI_CYAN;
    cout << "  ||=================================||\n";
    cout << "  || ";
    cout << "Acceso concedido - Bienvenido ";
    cout << ANSI_NEON << usuario << ANSI_CYAN;
    cout << " ||\n";
    cout << "  ||==================================||\n\n";
    cout << ANSI_RESET;

    limpiarPantalla();

    //===========
    // PRINCIPAL
    //===========
    do
    {
        sqlite3_stmt *stmt;
        string sql;

        limpiarPantalla();

        cout << "Cargando";
        for (int i = 0; i < 5; i++)
        {
            cout << "." << flush;
            Sleep(200);
        }

        opcionPrincipal = menuPrincipalConFlechas();

        switch (opcionPrincipal)
        {
        case 1:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Tipo de Farmaco",
                "Ver Tipos de Farmacos",
                "Eliminar Tipo de Farmaco",
                "Volver"};

            do
            {
                opcionTF = menuSubmenuFlechas("Tipos de Farmacos", opciones, 4);

                string Descripcion, Estado;
                switch (opcionTF)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Descripcion: ";
                    Descripcion = inputWithESC();

                    if (Descripcion == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    string sql = "INSERT INTO TiposFarmacos (Descripcion, Estado) VALUES('" + Descripcion + "', '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Registro agregado correctamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                case 2:
                {
                    cout << "ID | DESCRIPCION | ESTADO " << endl;
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
                    system("pause");
                    limpiarPantalla();
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
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }
            } while (opcionTF != 4);
            break;
        }

        case 2:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Marca",
                "Ver Marcas",
                "Eliminar Marca",
                "Volver"};
            do
            {
                opcionMarca = menuSubmenuFlechas("Marcas", opciones, 4);

                string Descripcion, Estado;
                switch (opcionMarca)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Descripcion: ";
                    Descripcion = inputWithESC();

                    if (Descripcion == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    string sql = "INSERT INTO Marcas (Descripcion, Estado) VALUES('" + Descripcion + "', '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Registro agregado correctamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                case 2:
                {
                    cout << "ID | DESCRIPCION | ESTADO " << endl;
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
                    system("pause");
                    limpiarPantalla();
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
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }
            } while (opcionMarca != 4);
            break;
        }

        case 3:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Ubicacion",
                "Ver Ubicaciones",
                "Eliminar Ubicacion",
                "Volver"};
            do
            {
                opcionUbi = menuSubmenuFlechas("Ubicaciones", opciones, 4);

                string Descripcion, Estado, Estante, Tramo, Celda;
                switch (opcionUbi)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Descripcion: ";
                    Descripcion = inputWithESC();

                    if (Descripcion == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Estante: ";
                    Estante = inputWithESC();

                    if (Estante == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Tramo: ";
                    Tramo = inputWithESC();

                    if (Tramo == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Celda: ";
                    Celda = inputWithESC();

                    if (Celda == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    string sql = "INSERT INTO Ubicaciones (Descripcion, Estante, Tramo, Celda, Estado) VALUES('" +
                                 Descripcion + "', '" + Estante + "', '" + Tramo + "', '" + Celda + "', '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Registro agregado correctamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                case 2:
                {
                    cout << "ID | DESCRIPCION | ESTANTE | TRAMO | CELDA | ESTADO " << endl;
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
                    system("pause");
                    limpiarPantalla();
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
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }
            } while (opcionUbi != 4);
            break;
        }

        case 4:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Medicamentos",
                "Ver Medicamentos",
                "Eliminar Medicamento",
                "Volver"};

            do
            {
                opcionMedicamento = menuSubmenuFlechas("Medicamentos", opciones, 4);

                string Descripcion, Dosis, Estado, TipoF, Marca, Ubicacion, sql;
                int TipoFId, MarcaId, UbicacionId;
                switch (opcionMedicamento)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Ingrese una descripcion: ";
                    Descripcion = inputWithESC();

                    if (Descripcion == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

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
                    Dosis = inputWithESC();

                    if (Dosis == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql = "INSERT INTO Medicamentos (Descripcion, TipoFId, MarcaId, UbicacionId, Dosis, Estado) VALUES('" + Descripcion + "' , '" + to_string(TipoFId) + "' , '" + to_string(MarcaId) + "' , '" + to_string(UbicacionId) + "' , '" + Dosis + "' , '" + Estado + "')";

                    ejecutarSQL(DB, sql);
                    cout << "Registrado exitosamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                case 2:
                {
                    cout << "ID | DESCRIPCION | TIPO | MARCA | UBICACION | DOSIS | ESTADO" << endl;
                    sqlite3_stmt *stmt;
                    const char *sqlSelect = "SELECT M.ID, M.Descripcion, "
                                            "TF.Descripcion AS TipoFarmaco, "
                                            "MC.Descripcion AS Marca, "
                                            "UB.Descripcion AS Ubicacion, "
                                            "M.Dosis, M.Estado "
                                            "FROM Medicamentos M "
                                            "LEFT JOIN TiposFarmacos TF ON M.TipoFId = TF.ID "
                                            "LEFT JOIN Marcas MC ON M.MarcaId = MC.ID "
                                            "LEFT JOIN Ubicaciones UB ON M.UbicacionId = UB.ID;";
                    if (sqlite3_prepare_v2(DB, sqlSelect, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            int id = sqlite3_column_int(stmt, 0);
                            Descripcion = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                            TipoF = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
                            Marca = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
                            Ubicacion = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
                            Dosis = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
                            Estado = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

                            cout << id << " | " << Descripcion << " | " << TipoF << " | " << Marca << " | " << Ubicacion << " | " << Dosis << " | " << Estado << endl;
                        }
                        if (!hayDatos)
                            cout << "No hay registros." << endl;
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
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
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }

            } while (opcionMedicamento != 4);
            break;
        }

        case 5:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Pacientes",
                "Ver Pacientes",
                "Eliminar Pacientes",
                "Volver"};

            do
            {

                opcionPaci = menuSubmenuFlechas("Pacientes", opciones, 4);

                string Nombre, Cedula, NoCarnet, TipoPaciente, Estado, sql;
                int seleccionablelist;

                switch (opcionPaci)
                {

                case 1:
                {
                    cin.ignore();
                    cout << "Ingrese su nombre completo: ";
                    Nombre = inputWithESC();

                    if (Nombre == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Ingrese su cedula: ";
                    Cedula = inputWithESC();

                    if (Cedula == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Insgrese su numero de carnet: ";
                    NoCarnet = inputWithESC();

                    if (NoCarnet == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
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
                        cout << "Opcion invalida, se asignara: Otros\n";
                        TipoPaciente = "Otros";
                        break;
                    }
                    cin.ignore();
                    cout << "Ingrese su estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql = "INSERT INTO Paciente (Nombre, Cedula, NoCarnet, TipoPaciente, Estado) VALUES ('" + Nombre + "' , '" + Cedula + "' , '" + NoCarnet + "' , '" + TipoPaciente + "' , '" + Estado + "');";

                    ejecutarSQL(DB, sql);
                    cout << "Resgistrado exitosamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                case 2:
                {
                    cout << "ID | NOMBRE | CEDULA | NO.CARNET | TIPOPACIENTE | ESTADO " << endl;
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
                    system("pause");
                    limpiarPantalla();
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
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }

            } while (opcionPaci != 4);
            break;
        }

        case 6:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Medico",
                "Ver Medico",
                "Eliminar Medico",
                "Volver"};
            do
            {
                opcionMed = menuSubmenuFlechas("Medico", opciones, 4);

                string Nombre, Cedula, Tanda, Especialidad, Estado, sql;
                switch (opcionMed)
                {
                case 1:
                {
                    cin.ignore();
                    cout << "Ingrese su nombre completo: ";
                    Nombre = inputWithESC();

                    if (Nombre == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Ingrese su cedula: ";
                    Cedula = inputWithESC();

                    if (Cedula == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Ingrese su Tanda: ";
                    Tanda = inputWithESC();

                    if (Tanda == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Ingrese su Especialidad: ";
                    Especialidad = inputWithESC();

                    if (Especialidad == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }
                    cout << "Ingrese su Estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql = "INSERT INTO Medicos (Nombre, Cedula, Tanda, Especialidad, Estado) VALUES ('" + Nombre + "' , '" + Cedula + "' , '" + Tanda + "' , '" + Especialidad + "' , '" + Estado + "');";
                    ejecutarSQL(DB, sql);
                    cout << "Resgistrado exitosamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                case 2:
                {
                    cout << "ID | NOMBRE | CEDULA | TANDA | ESPECIALIDAD | ESTADO " << endl;
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
                    system("pause");
                    limpiarPantalla();
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
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }
            } while (opcionMed != 4);
            break;
        }

        case 7:
        {
            limpiarPantalla();
            string opciones[] = {
                "Agregar Visitas",
                "Ver Visitas",
                "Eliminar Visitas",
                "Volver"};
            do
            {
                opcionV = menuSubmenuFlechas("Visitas", opciones, 4);

                int MedicoId, PacienteId;
                string FechaVisita, HoraVisita, Sintomas, Medicamentos, Recomendaciones, Estado, sql;

                switch (opcionV)
                {
                case 1: // REGISTRAR
                {
                    cout << "\n--- Registrar Visita ---\n";

                    // Seleccionar médico
                    MedicoId = seleccionTabla(DB, "Medicos", "Nombre");
                    if (MedicoId == -1)
                    {
                        cout << "Medico invalido.\n";
                        break;
                    }

                    // Seleccionar paciente
                    PacienteId = seleccionTabla(DB, "Paciente", "Nombre");
                    if (PacienteId == -1)
                    {
                        cout << "Paciente invalido.\n";
                        break;
                    }

                    cin.ignore();
                    cout << "Fecha (YYYY-MM-DD): ";
                    FechaVisita = inputWithESC();

                    if (FechaVisita == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Hora (HH:MM): ";
                    HoraVisita = inputWithESC();

                    if (HoraVisita == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Sintomas: ";
                    Sintomas = inputWithESC();

                    if (Sintomas == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Medicamentos Suministrados: ";
                    Medicamentos = inputWithESC();

                    if (Medicamentos == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Recomendaciones: ";
                    Recomendaciones = inputWithESC();

                    if (Recomendaciones == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Estado: ";
                    Estado = inputWithESC();

                    if (Estado == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql =
                        "INSERT INTO Visitas (MedicoId, PacienteId, FechaVisita, HoraVisita, Sintomas, Medicamentos, Recomendaciones, Estado) "
                        "VALUES ('" +
                        to_string(MedicoId) + "', '" +
                        to_string(PacienteId) + "', '" +
                        FechaVisita + "', '" +
                        HoraVisita + "', '" +
                        Sintomas + "', '" +
                        Medicamentos + "', '" +
                        Recomendaciones + "', '" +
                        Estado + "');";

                    ejecutarSQL(DB, sql);
                    cout << "Visita registrada correctamente.\n";
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                case 2: // VER VISITAS
                {
                    sqlite3_stmt *stmt;
                    const char *sql =
                        "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                        "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                        "FROM Visitas V "
                        "JOIN Paciente P ON V.PacienteId = P.ID "
                        "JOIN Medicos M ON V.MedicoId = M.ID "
                        "ORDER BY V.ID DESC;";

                    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        bool hayDatos = false;
                        cout << "\n--- LISTA DE VISITAS ---\n";
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            hayDatos = true;
                            cout << "ID: " << sqlite3_column_int(stmt, 0) << endl;
                            cout << "Paciente: " << sqlite3_column_text(stmt, 1) << endl;
                            cout << "Medico: " << sqlite3_column_text(stmt, 2) << endl;
                            cout << "Fecha: " << sqlite3_column_text(stmt, 3) << endl;
                            cout << "Hora: " << sqlite3_column_text(stmt, 4) << endl;
                            cout << "Sintomas: " << sqlite3_column_text(stmt, 5) << endl;
                            cout << "Medicamentos: " << sqlite3_column_text(stmt, 6) << endl;
                            cout << "Recomendaciones: " << sqlite3_column_text(stmt, 7) << endl;
                            cout << "Estado: " << sqlite3_column_text(stmt, 8) << endl;
                            cout << "------------------------\n";
                        }
                        if (!hayDatos)
                            cout << "No hay visitas registradas.\n";
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                case 3: // ELIMINAR VISITA
                {
                    int id = seleccionTabla(DB, "Visitas", "FechaVisita");
                    if (id != -1)
                    {
                        sql = "DELETE FROM Visitas WHERE ID = " + to_string(id) + ";";
                        ejecutarSQL(DB, sql);
                        cout << "Visita eliminada.\n";
                    }
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }

            } while (opcionV != 4);

            break;
        }

        case 8:
        {

            cout << "\n--- REPORTE GENERAL DE TODAS LAS VISITAS ---\n";

            sql =
                "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                "FROM Visitas V "
                "JOIN Paciente P ON V.PacienteId = P.ID "
                "JOIN Medicos M ON V.MedicoId = M.ID "
                "ORDER BY V.FechaVisita DESC, V.HoraVisita DESC;";

            if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
            {
                bool hayDatos = false;
                while (sqlite3_step(stmt) == SQLITE_ROW)
                {
                    hayDatos = true;
                    cout << "ID: " << sqlite3_column_int(stmt, 0) << endl;
                    cout << "Paciente: " << sqlite3_column_text(stmt, 1) << endl;
                    cout << "Medico: " << sqlite3_column_text(stmt, 2) << endl;
                    cout << "Fecha: " << sqlite3_column_text(stmt, 3) << endl;
                    cout << "Hora: " << sqlite3_column_text(stmt, 4) << endl;
                    cout << "Sintomas: " << sqlite3_column_text(stmt, 5) << endl;
                    cout << "Medicamentos: " << sqlite3_column_text(stmt, 6) << endl;
                    cout << "Recomendaciones: " << sqlite3_column_text(stmt, 7) << endl;
                    cout << "Estado: " << sqlite3_column_text(stmt, 8) << endl;
                    cout << "------------------------------------------\n";
                }
                if (!hayDatos)
                    cout << "No hay visitas registradas.\n";
            }
            sqlite3_finalize(stmt);
            system("pause");
            limpiarPantalla();
            break;
        }

        case 9:
        {
            limpiarPantalla();
            string opciones[] = {
                "Entre Fechas",
                "Por Medico",
                "Por paciente",
                "Por Estado",
                "Por Sintomas (contiene)",
                "Volver"};
            do
            {
                opR = menuSubmenuFlechas("Visitas", opciones, 6);

                sqlite3_stmt *stmt;
                string sql, desde, hasta, filtro;

                switch (opR)
                {
                // ============================
                // REPORTE ENTRE FECHAS
                // ============================
                case 1:
                {
                    cin.ignore();
                    cout << "Fecha Desde (YYYY-MM-DD): ";
                    desde = inputWithESC();

                    if (desde == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    cout << "Fecha Hasta (YYYY-MM-DD): ";
                    hasta = inputWithESC();

                    if (hasta == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql =
                        "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                        "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                        "FROM Visitas V "
                        "JOIN Paciente P ON V.PacienteId = P.ID "
                        "JOIN Medicos M ON V.MedicoId = M.ID "
                        "WHERE V.FechaVisita BETWEEN '" +
                        desde + "' AND '" + hasta + "';";

                    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        cout << "\n--- REPORTE ENTRE FECHAS ---\n";
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            cout << sqlite3_column_int(stmt, 0) << " | "
                                 << sqlite3_column_text(stmt, 1) << " | "
                                 << sqlite3_column_text(stmt, 2) << " | "
                                 << sqlite3_column_text(stmt, 3) << " | "
                                 << sqlite3_column_text(stmt, 4) << " | "
                                 << sqlite3_column_text(stmt, 5) << " | "
                                 << sqlite3_column_text(stmt, 6) << " | "
                                 << sqlite3_column_text(stmt, 7) << " | "
                                 << sqlite3_column_text(stmt, 8) << endl;
                        }
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                // ============================
                // REPORTE POR MEDICO
                // ============================
                case 2:
                {
                    int MedId = seleccionTabla(DB, "Medicos", "Nombre");
                    if (MedId == -1)
                        break;

                    sql =
                        "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                        "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                        "FROM Visitas V "
                        "JOIN Paciente P ON V.PacienteId = P.ID "
                        "JOIN Medicos M ON V.MedicoId = M.ID "
                        "WHERE M.ID = " +
                        to_string(MedId) + ";";

                    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        cout << "\n--- REPORTE POR MEDICO ---\n";
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            cout << sqlite3_column_int(stmt, 0) << " | "
                                 << sqlite3_column_text(stmt, 1) << " | "
                                 << sqlite3_column_text(stmt, 2) << " | "
                                 << sqlite3_column_text(stmt, 3) << " | "
                                 << sqlite3_column_text(stmt, 4) << " | "
                                 << sqlite3_column_text(stmt, 5) << " | "
                                 << sqlite3_column_text(stmt, 6) << " | "
                                 << sqlite3_column_text(stmt, 7) << " | "
                                 << sqlite3_column_text(stmt, 8) << endl;
                        }
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                // ============================
                // REPORTE POR PACIENTE
                // ============================
                case 3:
                {
                    int PacId = seleccionTabla(DB, "Paciente", "Nombre");
                    if (PacId == -1)
                        break;

                    sql =
                        "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                        "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                        "FROM Visitas V "
                        "JOIN Paciente P ON V.PacienteId = P.ID "
                        "JOIN Medicos M ON V.MedicoId = M.ID "
                        "WHERE P.ID = " +
                        to_string(PacId) + ";";

                    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        cout << "\n--- REPORTE POR PACIENTE ---\n";
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            cout << sqlite3_column_int(stmt, 0) << " | "
                                 << sqlite3_column_text(stmt, 1) << " | "
                                 << sqlite3_column_text(stmt, 2) << " | "
                                 << sqlite3_column_text(stmt, 3) << " | "
                                 << sqlite3_column_text(stmt, 4) << " | "
                                 << sqlite3_column_text(stmt, 5) << " | "
                                 << sqlite3_column_text(stmt, 6) << " | "
                                 << sqlite3_column_text(stmt, 7) << " | "
                                 << sqlite3_column_text(stmt, 8) << endl;
                        }
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                // ============================
                // REPORTE POR ESTADO
                // ============================
                case 4:
                {
                    cin.ignore();
                    cout << "Estado (Activo, Cerrado, Pendiente, etc.): ";
                    filtro = inputWithESC();

                    if (filtro == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql =
                        "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                        "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                        "FROM Visitas V "
                        "JOIN Paciente P ON V.PacienteId = P.ID "
                        "JOIN Medicos M ON V.MedicoId = M.ID "
                        "WHERE V.Estado LIKE '%" +
                        filtro + "%';";

                    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        cout << "\n--- REPORTE POR ESTADO ---\n";
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            cout << sqlite3_column_int(stmt, 0) << " | "
                                 << sqlite3_column_text(stmt, 1) << " | "
                                 << sqlite3_column_text(stmt, 2) << " | "
                                 << sqlite3_column_text(stmt, 3) << " | "
                                 << sqlite3_column_text(stmt, 4) << " | "
                                 << sqlite3_column_text(stmt, 5) << " | "
                                 << sqlite3_column_text(stmt, 6) << " | "
                                 << sqlite3_column_text(stmt, 7) << " | "
                                 << sqlite3_column_text(stmt, 8) << endl;
                        }
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
                    break;
                }

                // ============================
                // REPORTE POR SÍNTOMAS
                // ============================
                case 5:
                {
                    cin.ignore();
                    cout << "Buscar en sintomas (ej: fiebre): ";
                    filtro = inputWithESC();

                    if (filtro == "__ESC__")
                    {
                        cout << "\nOperacion cancelada.\n";
                        system("pause");
                        limpiarPantalla();
                        break;
                    }

                    sql =
                        "SELECT V.ID, P.Nombre, M.Nombre, V.FechaVisita, V.HoraVisita, "
                        "V.Sintomas, V.Medicamentos, V.Recomendaciones, V.Estado "
                        "FROM Visitas V "
                        "JOIN Paciente P ON V.PacienteId = P.ID "
                        "JOIN Medicos M ON V.MedicoId = M.ID "
                        "WHERE V.Sintomas LIKE '%" +
                        filtro + "%';";

                    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
                    {
                        cout << "\n--- REPORTE POR SINTOMAS ---\n";
                        while (sqlite3_step(stmt) == SQLITE_ROW)
                        {
                            cout << sqlite3_column_int(stmt, 0) << " | "
                                 << sqlite3_column_text(stmt, 1) << " | "
                                 << sqlite3_column_text(stmt, 2) << " | "
                                 << sqlite3_column_text(stmt, 3) << " | "
                                 << sqlite3_column_text(stmt, 4) << " | "
                                 << sqlite3_column_text(stmt, 5) << " | "
                                 << sqlite3_column_text(stmt, 6) << " | "
                                 << sqlite3_column_text(stmt, 7) << " | "
                                 << sqlite3_column_text(stmt, 8) << endl;
                        }
                    }
                    sqlite3_finalize(stmt);
                    system("pause");
                    limpiarPantalla();
                    break;
                }
                }

            } while (opR != 6);

            break;
        }

        case 10:
            cout << "Saliendo..." << endl;
            break;

        default:
            cout << "Opcion no valida.\n";
            break;
        }
    } while (opcionPrincipal != 10);

    sqlite3_close(DB);
    return 0;
}
