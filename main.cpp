#include <iostream>
#include <fstream>
#include "listas.hpp"
#include "rwstring.hpp"

using namespace std;

struct Producto {
    string sku;
    string descripcion;
    float costoFijo;
};

struct Reparacion {
    string cliente;
    int tipoProducto;
    string sku;
    float costoDirecto;
    float presupuestado;
};

const int lsku {10};
const int ldesc {20};
const int lclie {15};

fstream& operator>>(fstream& fs, Producto& p)
{
    p.sku = readstring(fs, lsku);
    p.descripcion = readstring(fs, ldesc);
    fs.read(reinterpret_cast<char*>(&p.costoFijo), sizeof(p.costoFijo));
    return fs;
}

ostream& operator<<(ostream& os, const Producto& p)
{
    os<< p.sku << "\t" <<p.descripcion << "\t" << p.costoFijo;
    return os;
}

fstream& operator>>(fstream& fs, Reparacion& r)
{
    r.cliente = readstring(fs, lclie);
    fs.read(reinterpret_cast<char*>(&r.tipoProducto), sizeof(r.tipoProducto));
    r.sku = readstring(fs, lsku);
    fs.read(reinterpret_cast<char*>(&r.costoDirecto), sizeof(r.costoDirecto));
    fs.read(reinterpret_cast<char*>(&r.presupuestado), sizeof(r.presupuestado));
    return fs;
}

ostream& operator<<(ostream& os, const Reparacion& r)
{
    os << r.cliente << "\t" << r.tipoProducto << "\t" << r.sku << "\t" << r.costoDirecto << "\t" << r.presupuestado;
    return os;
}

int critProdSKU(Producto a, Producto b)
{
    return a.sku.compare(b.sku);
}

int critReparaciones(Reparacion a, Reparacion b)
{
    if (a.cliente == b.cliente) {
        if (a.tipoProducto == b.tipoProducto)
            return a.sku.compare(b.sku);
        return a.tipoProducto - b.tipoProducto;
    }
    return a.cliente > b.cliente ? 1 : -1;
}

int critCliente(Reparacion a, Reparacion b)
{
    return a.cliente.compare(b.cliente);
}

string descTipo(int t)
{
    switch (t) {
        case 0: return "Electronico";
        case 1: return "Mecanico";
        case 2: return "Mecatronico";
    }
    return "Desconocido";
}

Producto* buscarProducto(Nodo<Producto>* lista, const string& sku)
{
    while (lista) {
        if (lista->dato.sku == sku)
            return &lista->dato;
        lista = lista->sig;
    }
    return nullptr;
}

string llenarEspacios(Reparacion rep){
   string palabraFin="";
   palabraFin=rep.cliente;
   int cantLetras=15;
   while(palabraFin.length()<cantLetras){
      palabraFin+=' ';
   }
   return palabraFin;
}

int main()
{
    Nodo<Producto>* listaProd = nullptr;
    Nodo<Reparacion>* listaRep = nullptr;

    Producto prod;
    Reparacion rep;
    fstream archi;

    archi.open("productos.bin", ios::in | ios::binary);
    if (!archi) {
        cout << "No se pudo abrir productos.bin\n";
        return EXIT_FAILURE;
    }
    while (archi >> prod){
        insertar(prod, listaProd, critProdSKU);}
    archi.close();

    archi.open("reparaciones.bin", ios::in | ios::binary);
    if (!archi) {
        cout << "No se pudo abrir reparaciones.bin\n";
        return EXIT_FAILURE;
    }
    while (archi >> rep){
        insertar(rep, listaRep, critReparaciones);}
    archi.close();

    mostrar(listaProd);
    mostrar(listaRep);
    cout << "Ingrese el nombre del cliente: ";
    while (getline(cin , rep.cliente)) {

       string palabraFinal=llenarEspacios(rep);
       rep.cliente=palabraFinal;//llenar espacios en blanco

        Nodo<Reparacion>* p = buscar(rep, listaRep, critReparaciones);

        if (!p) {
            cout << "Cliente no encontrado.";
        } else {
            float gananciaTotal = 0;

            while ( p->dato.cliente == rep.cliente) {
                Reparacion& r = p->dato;
                Producto* pr = buscarProducto(listaProd, r.sku);

                float costoFijo = pr ? pr->costoFijo : 0;
                float ganancia = r.presupuestado - (costoFijo + r.costoDirecto);
                gananciaTotal += ganancia;

                cout << "Cliente: " << r.cliente << endl;
                cout << "Tipo Producto: " << descTipo(r.tipoProducto) << endl;
                cout << "SKU: " << r.sku << endl;
                cout << "Producto: " << (pr ? pr->descripcion : "NO ENCONTRADO") << endl;
                cout << "Costo Fijo: " << costoFijo << endl;
                cout << "Costo Directo: " << r.costoDirecto << endl;
                cout << "Presupuestado: " << r.presupuestado << endl;

                p = p->sig;
            }
            cout << "Ganancia total: " << gananciaTotal << endl;
        }

        cout << "Ingrese el nombre del cliente: ";
    }

    while (listaProd) pop(listaProd);
    while (listaRep) pop(listaRep);

    return 0;
}
