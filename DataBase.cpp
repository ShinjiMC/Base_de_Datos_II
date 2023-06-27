#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <chrono>
#include <vector>
#include <filesystem>
#include <sstream>
namespace fs = std::filesystem;
class structuras {
private:
    char Path[256];
    char name[256];
    int tamTotal;
    int cantidadAtributos;
    std::unique_ptr<int[]> tams;
    std::unique_ptr<std::string[]> names;
    std::unique_ptr<std::string[]> types;
public:
    structuras() : tamTotal(0) {}
    structuras(const char* path, const char* nombre) {
        strncpy(Path, path, sizeof(Path) - 1);
        strncpy(name, nombre, sizeof(name) - 1);
        //Contarlineas
        std::ifstream inputFile0(Path);
        if (!inputFile0) {
            std::cerr << "Error al abrir el archivo: " << Path << std::endl;
            return;
        }

        int lineCount = 0;
        char d;
        while (inputFile0.get(d)) {
            if (d == '\n') {
                lineCount++;
            }
        }
        inputFile0.clear();  // Restaurar el estado del flujo
        inputFile0.seekg(0, std::ios::beg);  // Volver al principio del archivo
        tams = std::make_unique<int[]>(lineCount);
        names = std::make_unique<std::string[]>(lineCount);
        types = std::make_unique<std::string[]>(lineCount);
        inputFile0.close();
        cantidadAtributos = lineCount;
        //Extraer valores
        std::ifstream inputFile(Path);

        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << Path << std::endl;
            return;
        }
        int espacioTotal = 0;
        char word[256];
        char c;
        int index = 0;
        int line = 0;
        int nW = 0;
        while (inputFile.get(c)) {
            if (c == '#' || c == '\n') {
                word[index] = '\0';  // Agrega el carácter nulo al final de la palabra
                //std::cout << "Palabra: " << word << std::endl; //nombre de Variable
                if (nW == 0) {
                    if (strcmp(word, "INT") == 0) {
                        espacioTotal += 4;
                        tams[line] = 4;
                        types[line] = word;
                        nW = 2;
                    }
                    else { types[line] = word; nW++; }
                }
                else if (nW == 1) { tams[line] = std::atoi(word); espacioTotal += std::atoi(word); nW++; }
                else if (nW == 2) { names[line] = word; }
                index = 0;  // Reinicia el índice para la siguiente palabra
                if (c == '\n') { line++; nW = 0; }
            }
            else {
                word[index] = c;
                index++;
            }
        }
        tamTotal = espacioTotal;
        inputFile.close();

    }

    const char* getPath() const {
        return Path;
    }

    const char* getName() const {
        return name;
    }

    int getTamTotal() const {
        return tamTotal;
    }

    int getTamAtIndex(int index) const {
        return tams[index];
    }

    void setPath(const char* path) {
        std::strcpy(Path, path);
    }

    void setTamTotal(int totalSize) {
        tamTotal = totalSize;
        tams = std::make_unique<int[]>(tamTotal);
    }

    void setTamAtIndex(int index, int size) {
        tams[index] = size;
    }

    void showData() {
        for (int i = 0; i < cantidadAtributos; i++) {
            std::cout << "Tipo " << types[i] << " Tamano: " << tams[i] << " Nombre: " << names[i] << std::endl;
        }
        std::cout << "Total: " << tamTotal << std::endl;
    }

    std::unique_ptr<std::string[]>& getNames() {
        return names;
    }

    std::unique_ptr<std::string[]>& getTypes() {
        return types;
    }

    std::unique_ptr<int[]>& getValues() {
        return tams;
    }
};

class Cache {
private:
    char Path[8] = "./Cache";
    std::unique_ptr<structuras[]> estructuras;
    int cantidadEstruct;
public:
    Cache() {
        int count = 0;
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                std::cout << "Directorio creado: " << Path << std::endl;
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
        else {
            for (const auto& entry : fs::directory_iterator(Path)) {
                if (fs::is_regular_file(entry)) {
                    count++;
                }
            }
        }
        cantidadEstruct = count;
    }

    structuras& getEstructura(const char* val) const {
        for (int n = 0; cantidadEstruct > n; n++) {
            if (std::strcmp(estructuras[n].getName(), val) == 0) {
                return estructuras[n];
            }
        }
        throw std::runtime_error("No se encontró la estructura con el nombre especificado");
    }

    int getCantidadEstruct() { return cantidadEstruct; }

    void InsertDataDisk(int numPlatos, int numPistas, int numSectores, int bytesSector, int numBloques, int numSectorXBloque) {
        char DataDisk[256];
        snprintf(DataDisk, sizeof(DataDisk), "%d#%d#%d#%d#%d#%d", numPlatos, numPistas, numSectores, bytesSector, numBloques, numSectorXBloque);
        char PATH[13] = "./Cache/DISK";
        std::ofstream file(PATH, std::ios::trunc);
        if (file.is_open()) {
            file << DataDisk;
            file.close();
        }
        else {
            std::cerr << "Error al crear o reiniciar el archivo: " << PATH << std::endl;
        }
        
    }

    void Reload() {
        if (cantidadEstruct > 0) {
            estructuras = std::make_unique<structuras[]>(cantidadEstruct);
            for (int n = 0; cantidadEstruct > n; n++) {
                int currentIndex = 0;
                for (const auto& entry : fs::directory_iterator(Path)) {
                    if (entry.is_regular_file()) {
                        if (currentIndex == n) {
                            const std::string& filePath = entry.path().string();
                            char* charArray = new char[filePath.size() + 1];
                            std::strcpy(charArray, filePath.c_str());
                            const std::string& fileName = entry.path().filename().string();
                            char* charName = new char[fileName.size() + 1];
                            std::strcpy(charName, fileName.c_str());
                            if (std::strcmp(charName, "DISK") != 0) {
                                estructuras[n] = structuras(charArray, charName);
                            }
                        }
                        currentIndex++;
                    }
                }

            }
        }
    }
    void AddEstruct(const char* cadena, const char* fileName) {
        std::ofstream outputFile;
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/%s", Path, fileName);
        outputFile.open(filePath);

        if (outputFile.is_open()) {
            int currentIndex = 0;
            int charIndex = 0;
            char currentChar;

            while ((currentChar = cadena[charIndex]) != '\0') {
                if (currentChar == '|') {
                    outputFile << std::endl;
                    currentIndex++;
                }
                else {
                    outputFile << currentChar;
                }

                charIndex++;
            }

            outputFile.close();
            cantidadEstruct++;
            std::cout << "Archivo \"" << fileName << "\" guardado exitosamente en la carpeta \"" << Path << "\"" << std::endl;
        }
        else {
            std::cout << "Error al abrir el archivo de salida." << std::endl;
        }
        Reload();
    }

    void showDataIndex(int n) {
        if (n < cantidadEstruct) {
            estructuras[n].showData();
        }
    }
};

class Sector {
private:
    char Path[256];
public:
    // Constructor por defecto
    Sector() {}

    // Constructor con parámetros
    Sector(const char* path, int idx) {
        strncpy(Path, path, sizeof(Path) - 1);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            std::ofstream file(Path);
            if (file.is_open()) {
                file.close();
            }
            else {
                std::cerr << "Error al crear el archivo: " << Path << std::endl;
            }
        }
    }

    ~Sector() {}
};

class Pista {
private:
    char Path[256];
public:
    // Constructor por defecto
    Pista() {}

    // Constructor con parámetros
    Pista(const char* path, int idx, int numSectores) {
        strncpy(Path, path, sizeof(Path) - 1);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < numSectores; i++) {
                    char sectorPath[256];
                    snprintf(sectorPath, sizeof(sectorPath), "%s/Sector_%d.txt", Path, i);
                    Sector s = Sector(sectorPath, i);
                }
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
    }

    ~Pista() {}
};

class Superficie {
private:
    char Path[256];
public:
    // Constructor por defecto
    Superficie() {}

    // Constructor con parámetros
    Superficie(const char* path, int idx, int numPistas, int numSectores) {
        strncpy(Path, path, sizeof(Path) - 1);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < numPistas; i++) {
                    char pistaPath[256];
                    snprintf(pistaPath, sizeof(pistaPath), "%s/Pista_%d", Path, i);
                    Pista pi = Pista(pistaPath, i, numSectores);
                }
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
    }

    ~Superficie() {}
};

class Plato {
private:
    char Path[256];
public:
    // Constructor por defecto
    Plato() {}

    // Constructor con parámetros
    Plato(const char* path, int idx, int numPistas, int numSectores) {
        strncpy(Path, path, sizeof(Path) - 1);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < 2; i++) {
                    char surfacePath[256];
                    snprintf(surfacePath, sizeof(surfacePath), "%s/Superficie_%d", Path, i);
                    Superficie su = Superficie(surfacePath, i, numPistas, numSectores);
                }
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
    }

    ~Plato() {}
};

class StackPlatos {
private:
    char Path[7] = "./Disk";
    int numPlatos, numSuperficies, numSectores, numPistas, numbytes;
public:
    // Constructor por defecto
    StackPlatos() : numPlatos(0), numSuperficies(0), numSectores(0), numPistas(0), numbytes(0) {}

    // Constructor con parámetros
    StackPlatos(int numPlatos, int numPistas, int numSectores, int numBytes)
        : numPlatos(numPlatos), numSuperficies(2), numPistas(numPistas), numSectores(numSectores), numbytes(numBytes) {
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < numPlatos; i++) {
                    char platoPath[256];
                    snprintf(platoPath, sizeof(platoPath), "%s/Plato_%d", Path, i);
                    Plato platos = Plato(platoPath, i, numPistas, numSectores);
                }
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
    }

    ~StackPlatos() {}
};

class Block {
private:
    char Path[256];
public:
    // Constructor por defecto
    Block() {}

    Block(const char* path) {
        strncpy(Path, path, sizeof(Path) - 1);
        Path[sizeof(Path) - 1] = '\0';
        if (!fs::exists(Path)) {
            std::ofstream file(Path);
            if (file.is_open()) {
                file.close();
            }
            else {
                std::cerr << "Error al crear el archivo: " << Path << std::endl;
            }
        }
    }

    ~Block() {}
};

class StackBlocks {
private:
    char Path[6] = "./Dir";
    int numBloques, numPlatos, numSuperficies, numSectores, numPistas, numBytesSector, numBytesBloque, numSectoresXBloque;
public:
    // Constructor por defecto
    StackBlocks() :numBloques(0), numPlatos(0), numSuperficies(0), numSectores(0), numPistas(0), numBytesSector(0), numBytesBloque(0) {}

    // Constructor con parámetros
    StackBlocks(int numBloques, int numPlatos, int numSectores, int numPistas, int numBytesSector, int numSectoresXBloque)
        : numBloques(numBloques), numPlatos(numPlatos), numSuperficies(2), numPistas(numPistas), numSectores(numSectores), numSectoresXBloque(numSectoresXBloque)
        , numBytesSector(numBytesSector), numBytesBloque(numSectoresXBloque * numBytesSector) {
        if (!fs::exists(Path)) {
            if (fs::create_directory(Path)) {
                for (int i = 0; i < numBloques; i++) {
                    char bloquePath[256];
                    snprintf(bloquePath, sizeof(bloquePath), "%s/Block_%d.txt", Path, i);
                    Block bloques = Block(bloquePath);
                }
            }
            else {
                std::cerr << "Error al crear el directorio: " << Path << std::endl;
            }
        }
    }

    ~StackBlocks() {}

    void Resset(int n) {
        char bloquePath[256];
        snprintf(bloquePath, sizeof(bloquePath), "%s/Block_%d.txt", Path, n);
        std::ofstream file(bloquePath, std::ios::trunc);
        if (file.is_open()) {
            file.close();
        }
        else {
            std::cerr << "Error al crear o reiniciar el archivo: " << bloquePath << std::endl;
        }
    }

    void InsertData(const char* content, int n) {
        char bloquePath[256];
        snprintf(bloquePath, sizeof(bloquePath), "%s/Block_%d.txt", Path, n);
        std::ofstream outputFile(bloquePath, std::ios::app);
        if (!outputFile) {
            std::cerr << "Error al abrir el archivo Bloque: " << bloquePath << std::endl;
            return;
        }
        outputFile << content;
        outputFile.close();
    }

    const char* getPathBloq(int n) {
        char* bloquePath = new char[256];
        snprintf(bloquePath, 256, "%s/Block_%d.txt", Path, n);
        return bloquePath;
    }

    // Getter para el atributo 'Path'
    const char* getPath() const {
        return Path;
    }

    // Getter para el atributo 'numPlatos'
    int getNumPlatos() const {
        return numPlatos;
    }

    // Getter para el atributo 'numSuperficies'
    int getNumSuperficies() const {
        return numSuperficies;
    }

    // Getter para el atributo 'numPistas'
    int getNumPistas() const {
        return numPistas;
    }

    // Getter para el atributo 'numSectores'
    int getNumSectores() const {
        return numSectores;
    }

    // Getter para el atributo 'numBytesSector'
    int getNumBytesSector() const {
        return numBytesSector;
    }

    // Getter para el atributo 'numBytesBloque'
    int getNumBytesBloque() const {
        return numBytesBloque;
    }
};

class CabezaDisk {
private:
    StackPlatos dishes;
    StackBlocks blocks;
    int numBloques, numSectoresXBloque, numPlatos, numPistas, numSectores, numBytes;
public:
    CabezaDisk() : numSectoresXBloque(0), numPlatos(0), numPistas(0), numSectores(0) {}
    CabezaDisk(int numPlatos, int numPistas, int numSectores, int numBytes, int numSectoresXBloque)
        : numSectoresXBloque(numSectoresXBloque), numPlatos(numPlatos), numPistas(numPistas), numSectores(numSectores), numBytes(numBytes) {
        numBloques = ((numPlatos * 2 * numPistas * numSectores) / numSectoresXBloque);
        dishes = StackPlatos(numPlatos, numPistas, numSectores, numBytes);
        blocks = StackBlocks(numBloques, numPlatos, numSectores, numPistas, numBytes, numSectoresXBloque);
    }

    ~CabezaDisk() {}

    const char* GetSectPATH(int indexBloque) {
        int i = indexBloque;
        int Pn = 0, Sn = 0, Pin = 0, Sectn = 0;
        while (i != 0) {
            if (Sn == 2) { Pn++; Sn = 0; }
            if (Pn == numPlatos) { Sectn++; Pn = 0; }
            if (Sectn == numSectores) { Pin++; Sectn = 0; }
            if (Pin == numPistas) { std::cout << "No existe en el Disco " << std::endl; }
            i--;
            if (i == 0) { break; }
            else { Sn++; }
        }
        char Path[7] = "./Disk";
        static char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/Plato_%d/Superficie_%d/Pista_%d/Sector_%d.txt", Path, Pn, Sn, Pin, Sectn);
        return filePath;
    }

    void ShowSectPATH(int n) {
        int numSuperficies = 2, numPlatos = 5, numPistas = 3, numSectores = 5;
        int i = n;
        int Pn = 0, Sn = 0, Pin = 0, Sectn = 0;
        while (i != 0) {
            if (Sn == 2) { Pn++; Sn = 0; }
            if (Pn == numPlatos) { Sectn++; Pn = 0; }
            if (Sectn == numSectores) { Pin++; Sectn = 0; }
            if (Pin == numPistas) { std::cout << "No existe en el Disco " << std::endl; }
            i--;
            if (i == 0) { break; }
            else { Sn++; }
        }
        char Path[7] = "./Disk";
        static char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/Plato_%d/Superficie_%d/Pista_%d/Sector_%d.txt", Path, Pn, Sn, Pin, Sectn);
        std::cout << "\t\tPlato:" << Pn << "\t" << "Superficie: " << Sn << "\t" << "Pista: " << Pin << "\t" << "Sector: " << Sectn << std::endl;
        std::cout << "\t\tPath: " << filePath << std::endl;
    }

    void ShowSectXBloq(int n) {
        for (int j = 1; j != numSectoresXBloque+1; j++) {
            std::cout << "\tSector:" << (n * numSectoresXBloque + j) - 1 << std::endl;
            ShowSectPATH(n * numSectoresXBloque + j);
        }
    }

    void ResetBloqueASector(int index) {
        for (int j = 1; j != numSectoresXBloque+1; j++) {
            const char* filePath = GetSectPATH(index * numSectoresXBloque + j);
            std::ofstream file(filePath, std::ios::trunc);
            if (file.is_open()) {
                file.close();
            }
            else {
                std::cerr << "Error al crear o reiniciar el archivo: " << filePath << std::endl;
            }
        }
    }
    //Cargar de Bloque a Sector
    void BloqueASector(int indexBloque) {
        ResetBloqueASector(indexBloque);
        std::ifstream inputFile(blocks.getPathBloq(indexBloque), std::ios::in);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo al Update Bloque Sector: " << blocks.getPathBloq(indexBloque) << std::endl;
            return;
        }
        inputFile.seekg(0, std::ios::end);
        std::streampos fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);
        std::streampos partSize = blocks.getNumBytesSector();
        std::streampos bytesRead = 0;
        int currentPart = 0;
        int numRegistros = 0;
        char c;
        int J = 1;
        
        while (inputFile.get(c)) {
            std::ofstream outputFile(GetSectPATH(indexBloque * numSectoresXBloque + J), std::ios::app);
            if (!outputFile) {
                std::cerr << "Error al abrir el archivo del Sector: "<< GetSectPATH(indexBloque * numSectoresXBloque + J) << std::endl;
                inputFile.close();
                return;
            }
            if (bytesRead == partSize) {
                outputFile.close();
                bytesRead = 0;
                J++;
                std::ofstream outputFile(GetSectPATH(indexBloque * numSectoresXBloque + J), std::ios::app);
                if (!outputFile) {
                    std::cerr << "Error al abrir el archivo del Sector: " << GetSectPATH(indexBloque * numSectoresXBloque + J) << std::endl;
                    inputFile.close();
                    return;
                }
                outputFile << c;
                if (c == '\n') { bytesRead += 2; }
                else { bytesRead += 1; }
            }
            else {
                outputFile << c;
                if (c == '\n') { bytesRead += 2; }
                else { bytesRead += 1; }
            }
        }
        inputFile.close();
        return;
    }

    void InsertBlock(const char* content, int n) {
        blocks.InsertData(content,n);
    }

    void Reset(int n) {
        blocks.Resset(n);
    }

    // Getter para el atributo 'dishes'
    const StackPlatos& getDishes() const {
        return dishes;
    }

    // Getter para el atributo 'blocks'
    const StackBlocks& getBlocks() const {
        return blocks;
    }

    // Getter para el atributo 'numSectoresXBloque'
    int getNumSectoresXBloque() const {
        return numSectoresXBloque;
    }

    // Getter para el atributo 'numPlatos'
    int getNumPlatos() const {
        return numPlatos;
    }

    // Getter para el atributo 'numPistas'
    int getNumPistas() const {
        return numPistas;
    }

    // Getter para el atributo 'numSectores'
    int getNumSectores() const {
        return numSectores;
    }

    // Getter para el atributo 'numBloques'
    int getNumBloques() const {
        return numBloques;
    }

    // Getter para el atributo 'numBytes'
    int getNumBytes() const {
        return numBytes;
    }
};

class HardDisk {
private:
    CabezaDisk SDD;
public:
    HardDisk() {}

    HardDisk(int numPlatos, int numPistas, int numSectores, int numBytes, int numSectoresXBloque) {
        SDD = CabezaDisk(numPlatos, numPistas, numSectores, numBytes, numSectoresXBloque);
    }

    ~HardDisk() {}

    void chargeAllBlocks() {
        for (int i = 0; i < SDD.getNumBloques(); i++) {
            std::cout << "Bloque: " << i << std::endl;
            SDD.BloqueASector(i);
        }
    }

    void chargeBlock(int n) {
        SDD.BloqueASector(n);
    }

    int getBytesBloq() {
        return (SDD.getNumBytes() * SDD.getNumSectoresXBloque());
    }

    int getBytesBloqOcupados(int n) {
        char bloquePath[256];
        snprintf(bloquePath, sizeof(bloquePath), "./Dir/Block_%d.txt", n);
        std::ifstream inputFile(bloquePath, std::ios::binary | std::ios::ate);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo Bloque: " << bloquePath << std::endl;
            return -1;
        }
        std::streampos fileSize = inputFile.tellg();
        inputFile.close();
        return static_cast<int>(fileSize);
    }

    int getBytesSectOcupados(int n) {
        std::ifstream inputFile(SDD.GetSectPATH(n), std::ios::binary | std::ios::ate);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo Bloque: " << SDD.GetSectPATH(n) << std::endl;
            return -1;
        }
        std::streampos fileSize = inputFile.tellg();
        inputFile.close();
        return static_cast<int>(fileSize);
    }

    void ShowBloq(int n) {
        char bloquePath[256];
        snprintf(bloquePath, sizeof(bloquePath), "./Dir/Block_%d.txt", n);
        std::ifstream inputFile(bloquePath, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo Bloque: " << bloquePath << std::endl;
            return;
        }
        char character;
        while (inputFile.get(character)) {
            std::cout << character;
        }
        inputFile.close();
    }


    void ShowSectByBloq(int n) {
        SDD.ShowSectXBloq(n);
    }

    // Getter para el atributo 'SDD'
    CabezaDisk& getSDD() {
        return SDD;
    }
};

class Page {
private:
    std::string data;
    int index;
    int bytes;
    int bytesOcupados;
public:
    // Constructor por defecto
    Page() : index(0), bytes(0), bytesOcupados(0) {}

    void InsertData(char content) {
        data += content;
        bytesOcupados++;
    }

    // Constructor con parámetros
    Page(int idx, int numBytes)
        : index(idx), bytes(numBytes), bytesOcupados(0) {
        char Path[6] = "./Dir";
        char bloquePath[256];
        snprintf(bloquePath, sizeof(bloquePath), "%s/Block_%d.txt", Path, index);
        std::ifstream inputFile(bloquePath);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << bloquePath << std::endl;
            return;
        }
        char letter;
        while (inputFile.get(letter)) {
            InsertData(letter);
        }
        inputFile.close();
    }

    ~Page() {}

    // Getter para el atributo 'data'
    const std::string& getData() const {
        return data;
    }

    // Getter para el atributo 'index'
    int getIndex() const {
        return index;
    }

    // Getter para el atributo 'bytes'
    int getBytes() const {
        return bytes;
    }

    // Getter para el atributo 'bytesOcupados'
    int getBytesOcupados() const {
        return bytesOcupados;
    }

    void replaceData(std::string n) {
        data = n;
    }
};

class StackPages {
private:
    std::vector<Page> paginas;
    int numPaginas;
public:
    // Constructor por defecto
    StackPages() : numPaginas(0) {}

    // Constructor con parámetros
    StackPages(int numPaginas, int numBytes) : numPaginas(numPaginas) {
        for (int i = 0; i < numPaginas; i++) {
            Page tmp = Page(i, numBytes);
            paginas.push_back(tmp);
        }
    }

    ~StackPages() {}

    void insertDataPage(char content, int nPage) {
        if (nPage >= 0 && nPage < numPaginas) {
            paginas[nPage].InsertData(content);
        }
        else {
            std::cerr << "Error: Pagina no encontrada." << nPage << std::endl;
        }
    }

    // Getter para el atributo 'pagina'
    Page getPageAtIndex(int n) {
        if (n >= 0 && n < numPaginas) {
            return paginas[n];
        }
        else {
            std::cerr << "Error: Índice de página no válido: " << n << std::endl;
            // Devuelve una página inválida o lanza una excepción según tus necesidades
            return Page(-1, 0);
        }
    }

    // Getter para el atributo 'paginas'
    const std::vector<Page>& getPages() const {
        return paginas;
    }

    std::vector<Page>& gettingPages() {
        return paginas;
    }

    // Getter para el atributo 'numPaginas'
    int getNumPages() const {
        return numPaginas;
    }
};

class BufferManager {
private:
    Cache diccionarios;
    StackPages AllPages;
    HardDisk Disco;
public:
    BufferManager() {
        diccionarios = Cache();
        diccionarios.Reload();
    }
    BufferManager(int numBytes, int numPages) {
        diccionarios = Cache();
        diccionarios.Reload();
        AllPages = StackPages(numPages, numBytes);
    }

    ~BufferManager() {}

    const StackPages& getAllPages() const {
        return AllPages;
    }
    
    void MakeDisco() {
        std::vector<std::string> carpetas = { "./Dir","./disk" };
        for (const auto& carpeta : carpetas) {
            if (fs::exists(carpeta)) {
                try {
                    fs::remove_all(carpeta);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error al eliminar la carpeta: " << carpeta << std::endl;
                    std::cerr << e.what() << std::endl;
                }
            }
            else {
                std::cout << "La carpeta no existe: " << carpeta << std::endl;
            }
        }
        int numPlatos = 0, numPistas = 0, numSectores = 0, numBytes = 0, numSectoresXBloque = 0;
        std::cout << "Ingrese la cantidad de Platos: ";
        std::cin >> numPlatos;
        std::cout << "\nIngrese la cantidad de Pistas por Superficie: ";
        std::cin >> numPistas;
        std::cout << "\nIngrese la cantidad de Sectores por Pista: ";
        std::cin >> numSectores;
        std::cout << "\nIngrese la cantidad de Bytes por Sector: ";
        std::cin >> numBytes;
        std::cout << "\nIngrese la cantidad de Sectores por Bloque: ";
        std::cin >> numSectoresXBloque;
        diccionarios = Cache();
        diccionarios.Reload();
        diccionarios.InsertDataDisk(numPlatos, numPistas, numSectores, numBytes, (numPlatos * numPistas * numSectores * 2) / numSectoresXBloque, numSectoresXBloque);
        Disco = HardDisk(numPlatos, numPistas, numSectores, numBytes, numSectoresXBloque);
        AllPages = StackPages((numPlatos * numPistas * numSectores * 2) / numSectoresXBloque, numBytes * numSectoresXBloque);
    }
    
    void LoadDisk() {
        int numPlatos = 0, numPistas = 0, numSectores = 0, bytesSector = 0, numBloques = 0, numSectorXBloque = 0;
        std::ifstream inputFile("./Cache/DISK");
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << "./Cache/DISK" << std::endl;
            return;
        }
        char valor[256];
        int contador = 0;
        while (inputFile.getline(valor, sizeof(valor), '#')) {
            switch (contador) {
            case 0:
                numPlatos = std::atoi(valor);
                break;
            case 1:
                numPistas = std::atoi(valor);
                break;
            case 2:
                numSectores = std::atoi(valor);
                break;
            case 3:
                bytesSector = std::atoi(valor);
                break;
            case 4:
                numBloques = std::atoi(valor);
                break;
            case 5:
                numSectorXBloque = std::atoi(valor);
                break;
            default:
                break;
            }
            contador++;
        }

        inputFile.close();
        diccionarios = Cache();
        diccionarios.Reload();
        Disco = HardDisk(numPlatos, numPistas, numSectores, bytesSector, numSectorXBloque);
        AllPages = StackPages(numBloques, bytesSector * numSectorXBloque);
    }
    
    void InsertStruct(const char* content, const char* filename) {
        diccionarios.AddEstruct(content, filename);
    }
    
    void CargarArchivo(const char* filename) {
        diccionarios = Cache();
        diccionarios.Reload();
        const char extensionTXT[] = ".txt";
        std::size_t indicePunto = std::strcspn(filename, ".");
        std::size_t longitudMaxima = 256;
        char nombre2TXT[256];
        if (indicePunto < std::strlen(filename)) {
            std::strncpy(nombre2TXT, filename, indicePunto);
            nombre2TXT[indicePunto] = '\0';
        }
        else {
            std::strncpy(nombre2TXT, filename, longitudMaxima);
            nombre2TXT[longitudMaxima - 1] = '\0';
        }
        char nombreTXT[] = "new.txt";

        std::ifstream entrada(filename);
        if (!entrada) {
            std::cout << "No se pudo abrir el archivo." << std::endl;
            return;
        }
        std::ofstream salida(nombreTXT);
        std::string linea;
        char c;

        // Ignorar la primera línea del archivo CSV
        entrada.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Procesar las líneas restantes
        bool dentroDeComillas = false;
        while (entrada.get(c)) {
            if (c == '\n') {
                // Escribir la línea procesada en el archivo de salida
                salida << linea << '\n';
                linea.clear();
            }
            else {
                size_t pos = linea.size();


                // Reemplazar comas por #
                if (c == ',') {
                    // Verificar si estamos dentro de una cadena entre comillas
                    if (dentroDeComillas) {
                        linea.push_back(c);
                    }
                    else {
                        linea.push_back('#');
                    }
                }
                else if (c == '"') {
                    // Cambiar el estado de dentroDeComillas
                    dentroDeComillas = !dentroDeComillas;
                    linea.push_back(c);
                }
                else {
                    linea.push_back(c);
                }
            }
        }

        // Escribir la última línea procesada en el archivo de salida
        if (!linea.empty()) {
            salida << linea << '\n';
        }

        // Cerrar los archivos
        entrada.close();
        salida.close();

        //std::cout << "Archivo convertido exitosamente. Nombre del archivo TXT: " << nombreTXT << std::endl;

        const char* archivoAnterior2 = "new.txt";
        const char* archivoNuevo2 = strcat(strcpy(new char[strlen(nombre2TXT) + 5], nombre2TXT), ".txt");
        std::ifstream entrada2(archivoAnterior2);
        std::ofstream salida2(archivoNuevo2);
        char c2;
        int i2 = 0;
        int j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
        while (entrada2.get(c2)) {
            if (c2 == '\n') {
                while (j2 != 0) {
                    salida2 << ' ';
                    j2--;
                }
                salida2 << '\n';
                i2 = 0;
                j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
            }
            else if (c2 == '#') {
                if (j2 == 0) {
                    i2++;
                    j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
                }
                else {
                    while (j2 != 0) {
                        salida2 << ' ';
                        j2--;
                    }
                    i2++;
                    j2 = diccionarios.getEstructura(nombre2TXT).getTamAtIndex(i2);
                }
            }
            else {
                salida2 << c2;
                j2--;
            }
        }

        // Cerrar los archivos
        entrada2.close();
        salida2.close();
        int bytess = 3280 * 3;
        std::ifstream pro(archivoNuevo2, std::ios::binary);
        pro.seekg(0, std::ios::end);
        std::streampos fileSize = pro.tellg();
        pro.seekg(0, std::ios::beg);
        int inde = -1;
        for (int jai = 0; jai < AllPages.getNumPages(); jai++) {
            if (AllPages.getPageAtIndex(jai).getBytesOcupados() != AllPages.getPageAtIndex(jai).getBytes()) {
                inde = jai; break;
            }
        }
        if (inde != -1) {
            char car;
            int nbytes = 0;
            if (fileSize < (AllPages.getPageAtIndex(inde).getBytes() * AllPages.getNumPages())) {
                while (pro.get(car)) {
                    if (AllPages.getPageAtIndex(inde).getBytesOcupados() == AllPages.getPageAtIndex(inde).getBytes()) {
                        inde++;
                        if (inde == AllPages.getNumPages()) {
                            break;
                        }
                        nbytes = 0;
                        AllPages.insertDataPage(car, inde);
                        nbytes++;
                    }
                    else {
                        AllPages.insertDataPage(car, inde);
                        nbytes++;
                    }
                }
            }
            else {
                std::cout << "El documento es muy grande para cargarlo" << std::endl;
            }
        }
        else {
            std::cout << "Las páginas ya están completas." << std::endl;
        }
        pro.close();

        try {
            std::filesystem::remove(nombreTXT);
        }
        catch (const std::filesystem::filesystem_error& error) {
            std::cout << "Error al eliminar el archivo: " << error.what() << std::endl;
        }
        try {
            std::filesystem::remove(archivoNuevo2);
        }
        catch (const std::filesystem::filesystem_error& error) {
            std::cout << "Error al eliminar el archivo: " << error.what() << std::endl;
        }
    }
    
    void showPage(int n) {
        std::cout << "Pagina " << n << ": " << std::endl;
        std::cout << "Bytes Total: " << AllPages.getPages()[n].getBytes() << std::endl;
        std::cout << "Bytes Ocupados: " << AllPages.getPages()[n].getBytesOcupados() << std::endl;
        const std::string& data = AllPages.getPages()[n].getData();
        const char* dataPtr = data.c_str();
        while (*dataPtr != '\0') {
            std::cout << *dataPtr;
            ++dataPtr;
        }
    }
    
    void Page_to_Bloq(int n) {
        const std::string& data = AllPages.getPages()[n].getData();
        std::string blockData;

        // Copiar la cadena de datos de la página al bloque
        for (char c : data) {
            if (c != '\r' && c != '\r\n') {
                blockData.push_back(c);
            }
        }
        Disco.getSDD().Reset(n);
        Disco.getSDD().InsertBlock(blockData.c_str(), n);
        Disco.chargeBlock(n);
    }
    
    void allPages_to_bloqs() {
        for (int i = 0; i < AllPages.getNumPages(); i++) {
            Page_to_Bloq(i);
        }
    }

    void MetaDataBloq(int n) {
        if (AllPages.getNumPages() <= n && 0 <= n) {
            std::cout << "Bloque " << n << " No existe." << std::endl;
        }
        else {
            std::cout << "Bloque " << n << ": " << std::endl;
            std::cout << "Bytes Total: " << Disco.getBytesBloq() << " bytes" << std::endl;
            std::cout << "Bytes Ocupados: " << Disco.getBytesBloqOcupados(n) << " bytes" << std::endl;
            std::cout << "Sectores: " << std::endl;
            Disco.ShowSectByBloq(n);
            std::cout << "Contenido: " << std::endl;
            Disco.ShowBloq(n);
        }
    }

    void MetaDataSect(int n) {
        if (AllPages.getNumPages()*Disco.getSDD().getNumSectoresXBloque() <= n && 0 <= n) {
            std::cout << "Sector " << n - 1 << " No existe." << std::endl;
        }
        else {
            std::cout << "Sector " << n - 1 << ": " << std::endl;
            std::cout << "Bytes Total: " << Disco.getSDD().getNumBytes() << " bytes" << std::endl;
            std::cout << "Bytes Ocupados: " << Disco.getBytesSectOcupados(n) << " bytes" << std::endl;
            std::cout << "Path: " << Disco.getSDD().GetSectPATH(n) << std::endl;
            std::cout << "Contenido: " << std::endl;
            std::ifstream inputFile(Disco.getSDD().GetSectPATH(n), std::ios::binary);
            if (!inputFile) {
                std::cerr << "Error al abrir el archivo Bloque: " << Disco.getSDD().GetSectPATH(n) << std::endl;
                return;
            }
            char character;
            while (inputFile.get(character)) {
                std::cout << character;
            }
            inputFile.close();
        }
    }

    void ShowFILE(const char* filename, int n) {
        int reg = 0;
        std::ifstream inputFile(filename, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error al abrir el archivo: " << filename << std::endl;
            return;
        }
        
        std::size_t indicePunto = std::strcspn(filename, ".");
        std::size_t longitudMaxima = 256;
        char nombre2TXT[256];
        if (indicePunto < std::strlen(filename)) {
            std::strncpy(nombre2TXT, filename, indicePunto);
            nombre2TXT[indicePunto] = '\0';
        }
        else {
            std::strncpy(nombre2TXT, filename, longitudMaxima);
            nombre2TXT[longitudMaxima - 1] = '\0';
        }
        std::cout << "Registro " << n << ": " << std::endl;
        char character;
        while (inputFile.get(character)) {
            if (character == '\n') {
                reg++;
            }
            if (reg == n) {
                std::cout << character;
            }
        }
        inputFile.close();
        std::cout << std::endl;
        std::cout << "# de bytes por Registro: " << diccionarios.getEstructura(nombre2TXT).getTamTotal() << " bytes" << std::endl;
        std::cout << "# de bits por Registro: " << diccionarios.getEstructura(nombre2TXT).getTamTotal() * 8 << " bits" << std::endl;
        std::cout << "# de bytes por FILE " << filename << ": " << diccionarios.getEstructura(nombre2TXT).getTamTotal() * reg << " bytes" << std::endl;
        std::cout << "# de bits por FILE " << filename << ": " << diccionarios.getEstructura(nombre2TXT).getTamTotal() * 8 * reg << " bits" << std::endl;
    }

    std::string extraerLinea(const std::string& texto, int n) {
        std::string lineaExtraida;
        std::size_t inicio = 0;
        std::size_t fin = texto.find('\n');

        for (int i = 1; i < n; i++) {
            if (fin == std::string::npos)
                break;
            inicio = fin + 1;
            fin = texto.find('\n', inicio);
        }

        if (fin == std::string::npos)
            lineaExtraida = texto.substr(inicio);
        else
            lineaExtraida = texto.substr(inicio, fin - inicio);

        return lineaExtraida;
    }

    void intercambiarLineas(std::string& texto1, std::string& texto2, int n1, int n2) {
        std::string linea1;
        std::string linea2;
        std::size_t inicio1 = 0;
        std::size_t fin1 = texto1.find('\n');
        for (int i = 1; i < n1; i++) {
            if (fin1 == std::string::npos) { break; }
            inicio1 = fin1 + 1;
            fin1 = texto1.find('\n', inicio1);
        }
        if (fin1 == std::string::npos) { linea1 = texto1.substr(inicio1); }
        else { linea1 = texto1.substr(inicio1, fin1 - inicio1); }
        std::size_t inicio2 = 0;
        std::size_t fin2 = texto2.find('\n');
        for (int i = 1; i < n2; i++) {
            if (fin2 == std::string::npos) { break; }
            inicio2 = fin2 + 1;
            fin2 = texto2.find('\n', inicio2);
        }
        if (fin2 == std::string::npos) { linea2 = texto2.substr(inicio2); }
        else { linea2 = texto2.substr(inicio2, fin2 - inicio2); }
        if (fin1 == std::string::npos) { texto1.replace(inicio1, texto1.length() - inicio1, linea2); }
        else { texto1.replace(inicio1, fin1 - inicio1, linea2); }
        if (fin2 == std::string::npos) { texto2.replace(inicio2, texto2.length() - inicio2, linea1); }
        else { texto2.replace(inicio2, fin2 - inicio2, linea1); }
    }

    void MoverRegistro(int n1, int n2) {
        std::string line1, line2;
        int ind1 = 0, ind2 = 0;
        for (int i = 0; i < AllPages.getNumPages(); i++) {
            std::string text = AllPages.getPageAtIndex(i).getData();
            int lineCount = std::count(text.begin(), text.end(), '\n');
            if (0 < n1 - lineCount) {
                n1 -= lineCount;
            }
            else {
                line1 = AllPages.getPageAtIndex(i).getData();
                ind1 = i;
                break;
            }
        }
        for (int i = 0; i < AllPages.getNumPages(); i++) {
            std::string text = AllPages.getPageAtIndex(i).getData();
            int lineCount = std::count(text.begin(), text.end(), '\n');
            if (0 < n2 - lineCount) {
                n2 -= lineCount;
            }
            else {
                line2 = AllPages.getPageAtIndex(i).getData();
                ind2 = i;
                break;
            }
        }
        intercambiarLineas(line1, line2, n1, n2);
        AllPages.gettingPages()[ind1].replaceData(line1);
        AllPages.gettingPages()[ind2].replaceData(line2);

    }
};

int main() {
    int opcion = 0;
    BufferManager* PC = new BufferManager();
    while (true) {
        // Mostrar opciones disponibles
        std::cout << "--------------------------------------------------------------------------" << std::endl;
        std::cout << "Opciones disponibles:" << std::endl;
        std::cout << "1. Crear Disco" << std::endl;
        std::cout << "2. Cargar Disco" << std::endl;
        std::cout << "3. Crear Tabla" << std::endl;
        std::cout << "4. Insertar Documento" << std::endl;
        std::cout << "5. Mostrar Datos del FILE" << std::endl;
        std::cout << "6. Mostrar Bloque" << std::endl;
        std::cout << "7. Mostrar Sector" << std::endl;
        std::cout << "8. Mostrar Pagina" << std::endl;
        std::cout << "9. Guardar Cambios de Pagina" << std::endl;
        std::cout << "10. Guardar Cambios de Todas las Paginas" << std::endl;
        std::cout << "11. Mover Registros" << std::endl;
        std::cout << "12. Eliminar Registro" << std::endl;
        std::cout << "20. Apagar" << std::endl;

        // Solicitar opción al usuario
        std::cout << "Ingrese el numero de opcion: ";
        std::cin >> opcion;
        // Evaluar la opción ingresada
        if (opcion == 1) {
            std::cout << "Selecciono la opcion: Crear Disco" << std::endl;
            PC->MakeDisco();
        }
        else if (opcion == 2) {
            std::cout << "Selecciono la opcion: Cargar Disco" << std::endl;
            PC->LoadDisk();
        }
        else if (opcion == 3) {
            std::cout << "Selecciono la opcion: Crear Tabla" << std::endl;
            char nombreArchivo[256];
            std::string values;
            std::cout << "Ingrese el nombre de la Estructura: ";
            std::cin >> nombreArchivo;
            std::cout << std::endl;
            std::cout << "Ingrese la composicion (tipo#valor#nombre|tipo2#valor2#nombre2|...): ";
            std::cin >> values;
            std::cout << std::endl;
            PC->InsertStruct(values.c_str(), nombreArchivo);
        }
        else if (opcion == 4) {
            std::cout << "Selecciono la opcion: Insertar Documento" << std::endl;
            char nombreArchivo[256];
            std::cout << "Ingrese el nombre del Documento con extension: ";
            std::cin >> nombreArchivo;
            std::cout << std::endl;
            PC->CargarArchivo(nombreArchivo);
        }
        else if (opcion == 5) {
            std::cout << "Selecciono la opcion: Mostrar Datos del FILE" << std::endl;
            char nombreArchivo[256];
            int opt = 0;
            std::cout << "Ingrese el nombre del Documento con extension: ";
            std::cin >> nombreArchivo;
            std::cout << "Ingrese el numero de Registro: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->ShowFILE(nombreArchivo,opt);
        }
        else if (opcion == 6) {
            std::cout << "Selecciono la opcion: Mostrar Bloque" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Bloque: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->MetaDataBloq(opt);
        }
        else if (opcion == 7) {
            std::cout << "Selecciono la opcion: Mostrar Sector" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Sector: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->MetaDataSect(opt+1);
        }
        else if (opcion == 8) {
            std::cout << "Selecciono la opcion: Mostrar Pagina" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Pagina: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->showPage(opt);
        }
        else if (opcion == 9) {
            std::cout << "Selecciono la opcion: Guardar Cambios de Pagina" << std::endl;
            int opt = 0;
            std::cout << "Ingrese el numero de Pagina a Guardar: ";
            std::cin >> opt;
            std::cout << std::endl;
            PC->Page_to_Bloq(opt);
        }
        else if (opcion == 10) {
            std::cout << "Selecciono la opcion: Guardar Cambios de Todas las Paginas" << std::endl;
            PC->allPages_to_bloqs();
        }
        else if (opcion == 11) {
            std::cout << "Selecciono la opcion: Mover Registros" << std::endl;
            int opt = 0, opt2 = 0;
            std::cout << "Ingrese el numero del Primer Registro: ";
            std::cin >> opt;
            std::cout << "Ingrese el numero del Segundo Registro: ";
            std::cin >> opt2;
            std::cout << std::endl;
            PC->MoverRegistro(opt,opt2);
        }
        else if (opcion == 12) {
            std::cout << "Selecciono la opcion: Mover Registros" << std::endl;
            int opt = 0, opt2 = 0;
            std::cout << "Ingrese el numero del Primer Registro: ";
            std::cin >> opt;
            std::cout << "Ingrese el numero del Segundo Registro: ";
            std::cin >> opt2;
            std::cout << std::endl;
            PC->MoverRegistro(opt, opt2);
        }
        else if (opcion == 20) {
            std::cout << "Selecciono la opcion: Apagar" << std::endl;
            std::cout << "Apagando Sistema..." << std::endl;
            break;
        }
        else {
            std::cout << "Opcion invalida. Por favor, ingrese un numero de opcion valido." << std::endl;
        }
    }

    return 0;
}
//INT#PassengerId|STR#2#Survived|STR#2#Survived|STR#84#Name|STR#7#Sex|STR#5#Age|STR#2#SibSp|STR#2#Parch|STR#20#Ticket|STR#12#Fare|STR#20#Cabin|STR#2#Embarked|
//_CRT_SECURE_NO_WARNINGS
