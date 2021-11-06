#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define notasArch "registroArchivo.dat"

/// estructuras

typedef struct
{
    int nota;
    int legajo;
    char nombreApe[30];
    char nomMateria[30];
    int idMateria;
} registroArchivo;
typedef struct
{
    int nota;
    int legajo;
    char nombreApe[30];
} notaAlumno;
typedef struct _nodo
{
    notaAlumno dato;
    struct _nodo *sig;
} nodo;
typedef struct
{
    int idMateria;
    char nomMateria[30];
    nodo * listaDeNotas;
} Materia;

/// funciones

void generarArchivo()
{
    FILE* fp = fopen("notasnuevas.bin", "ab");

    registroArchivo regisBuff;

    if (fp)
    {
        for (int i = 0; i < 5; i++)
        {
            printf("Nota: ");
            scanf("%i", &regisBuff.nota);
            printf("Legajo: ");
            scanf("%i", &regisBuff.legajo);
            printf("Nombre Alumno: ");
            fflush(stdin);
            scanf("%s", regisBuff.nombreApe);
            printf("Nombre Materia: ");
            fflush(stdin);
            scanf("%s", regisBuff.nomMateria);
            printf("ID Materia: ");
            scanf("%i", &regisBuff.idMateria);

            fwrite(&regisBuff, sizeof(registroArchivo), 1, fp);
        }

        fclose(fp);
    }
}

// 1)

int retCantidadRegistros()
{
    FILE* fp = fopen(notasArch, "rb");
    //FILE* fp = fopen("notasnuevas.bin", "rb");

    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        return ftell(fp) / sizeof(registroArchivo);
    }
    else
    {
        printf("ERROR.\n");
        return -1;
    }
}

nodo* crearNodo (notaAlumno dato)
{
    nodo* nuevo = (nodo*)malloc(sizeof(nodo));
    nuevo->dato = dato;
    nuevo->sig = NULL;
    return nuevo;
}

void insertarPorLegajo (nodo** listaNodos, notaAlumno dato)
{
    if (*listaNodos)
    {
        if ((*listaNodos)->dato.legajo > dato.legajo)
        {
            nodo* nuevo = crearNodo(dato);
            nuevo->sig = *listaNodos;
            *listaNodos = nuevo;
        }
        else
            insertarPorLegajo(&(*listaNodos)->sig, dato);
    }
    else
        *listaNodos = crearNodo(dato);
}

int retIdMateriaBuscado(Materia mats[], int validos, int idMateria)
{
    int i = 0;

    while (i < validos && mats[i].idMateria < idMateria)
        i++;

    return i;
}

notaAlumno generarNota(int notaNum, int legajo, char* nombreAp)
{
    notaAlumno nota;
    nota.legajo = legajo;
    nota.nota = notaNum;
    strcpy(nota.nombreApe, nombreAp);
    return nota;
}

void despersistirArchivo(Materia mats[], int* validos)
{
    FILE* fp = fopen(notasArch, "rb");
    //FILE* fp = fopen("notasnuevas.bin", "rb");

    notaAlumno notaNueva;
    registroArchivo regisBuff;
    int ultPos, posInsertar;

    if (fp)
    {
        while(fread(&regisBuff, sizeof(registroArchivo), 1, fp) > 0)
        {
            notaNueva = generarNota(regisBuff.nota, regisBuff.legajo, regisBuff.nombreApe);

            posInsertar = retIdMateriaBuscado(mats, *validos, regisBuff.idMateria);

            if (posInsertar < *validos && mats[posInsertar].idMateria == regisBuff.idMateria)
            {
                insertarPorLegajo(&mats[posInsertar].listaDeNotas, notaNueva);
            }
            else
            {
                ultPos = *validos - 1;
                (*validos)++;

                while (posInsertar <= ultPos)
                {
                    mats[ultPos+1] = mats[ultPos];
                    ultPos--;
                }

                mats[posInsertar].idMateria = regisBuff.idMateria;
                strcpy(mats[posInsertar].nomMateria, regisBuff.nomMateria);
                mats[posInsertar].listaDeNotas = crearNodo(notaNueva);
            }
        }

        fclose(fp);
    }
    else
        printf("ERROR.\n");
}

// 2)

int retornarCantNotasMayorA_lista (nodo* lista, int valor)
{
    if (lista)
    {
        if (lista->dato.nota > valor)
            return 1 + retornarCantNotasMayorA_lista(lista->sig, valor);
        else
            return retornarCantNotasMayorA_lista(lista->sig, valor);
    }
    else
        return 0;
}

void informarCantNotasMayoresA (Materia mats[], int validos)
{
    int valor;
    printf("Ingrese un valor: ");
    scanf("%i", &valor);

    int cantNotas = 0;

    for (int i = 0; i < validos; i++)
        cantNotas += retornarCantNotasMayorA_lista(mats[i].listaDeNotas, valor);

    printf("LA CANTIDAD DE NOTAS MAYORES A %i ES: %i\n", valor, cantNotas);
}

// 3)

int buscarLegajoEnLista (nodo* lista, int legajo)
{
    if (lista)
    {
        if (lista->dato.legajo == legajo)
            return 1;
        else
            return buscarLegajoEnLista(lista->sig, legajo);
    }
    else
        return 0;
}

void informarMateriasDeLegajo (Materia mats[], int validos)
{
    int legajo;

    printf("INGRESE LEGAJO A BUSCAR: ");
    scanf("%i", &legajo);

    for (int i = 0; i < validos; i++)
    {
        if (buscarLegajoEnLista(mats[i].listaDeNotas, legajo))
            printf("EL LEGAJO SE ENCUENTRA EN: %s\n", mats[i].nomMateria);
    }
}

// 4)

void persistirNotas_lista (nodo* lista, FILE* fp)
{
    while (lista)
    {
        if (lista->dato.nota >= 6)
            fwrite(&lista->dato, sizeof(notaAlumno), 1, fp);

        lista = lista->sig;
    }
}

void persistirNotas (Materia mats[], int validos)
{
    FILE* fp = fopen("aprobados.bin", "wb");

    if (fp)
    {
        for (int i = 0; i < validos; i++)
            persistirNotas_lista(mats[i].listaDeNotas, fp);

        fclose(fp);
    }
    else
        printf("ERROR.\n");
}

/// main

int main()
{
    //generarArchivo();

    int dim = retCantidadRegistros();

    Materia* materias = (Materia*)malloc(sizeof(Materia)*dim);
    int validos = 0;

    despersistirArchivo(materias, &validos);

    //informarCantNotasMayoresA(materias, validos);

    printf("%i\n", materias[0].idMateria);
    printf("%i", materias[1].idMateria);

    //informarMateriasDeLegajo(materias, validos);

    //persistirNotas(materias, validos);

    return 0;
}
