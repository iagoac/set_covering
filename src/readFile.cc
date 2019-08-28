#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<vector<int> > readFile(ifstream &myfile) {
     
    int j = 0;
    int n_rows, n_columns, n_covered, lixo = 0;
    vector<vector<int> > matrix;
    
    if (!myfile.is_open()) {   
        cout << "error! cannot open file" << endl;
    }
    else {
        myfile >> n_rows >> n_columns; 
        matrix.resize(n_rows, vector<int> (n_columns, 0));
        
        for (int i = 0; i < n_columns; i++) {
            myfile >> lixo;
        }

        for (int i = 0; i < n_rows; i++) {
            myfile >> n_covered;
            for (int c = 0; c < n_covered; c++) {
                myfile >> j;  
                matrix[i][j] = 1;
            }
        }
    }
     
    return matrix;
}

int printMatrix(vector<vector<int> > matrix) {
    
    int n_rows = matrix.size();
    int n_columns = matrix[0].size();

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_columns; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }

    return 0;   
}