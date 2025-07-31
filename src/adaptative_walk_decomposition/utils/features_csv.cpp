#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "../headers/utils/features_csv.h"

using namespace std;

void build_csv(const vector<double> &mo_features, const vector<string> &column_names, 
               const string &rootfolder, const string &instance, const string &folder, 
               const string &subfolder, const string &subsubfolder, const string &filename) {

    string separator = "/"; //Para linux
    string aux = ".." + separator; // Subindo dois níveis.
    string full_path = rootfolder + separator + instance + separator + folder + separator + subfolder + separator + subsubfolder;
    string file_path = aux + full_path + separator + filename;

    // Sobrescrever a linha de zeros com os valores fornecidos
    ofstream file(file_path);

    if (file.is_open()) {
        // Escrever os nomes das colunas, incluindo "Instance"
        vector<string> updated_column_names = column_names;
        updated_column_names.insert(updated_column_names.begin(), "Instance");

        for (size_t i = 0; i < updated_column_names.size(); ++i) {
            file << updated_column_names[i];
            if (i != updated_column_names.size() - 1) {
                file << ",";
            }
        }
        file << "\n";

        // Escrever o valor da instância e os dados do vetor de doubles, verificando NaN
        file << fixed << setprecision(10);
        file << instance;  // Adiciona o valor da instância na primeira coluna
        for (size_t i = 0; i < mo_features.size(); ++i) {
            // Substituir NaN por 0.0
            double value = isnan(mo_features[i]) ? 0.0 : mo_features[i];
            file << "," << value;
        }
        file << "\n";

        file.close();
        // cout << "Arquivo CSV criado com valores fornecidos em: " << file_path << endl;
    } else {
        cerr << "Erro ao abrir o arquivo para escrita: " << file_path << endl;
    }
}
