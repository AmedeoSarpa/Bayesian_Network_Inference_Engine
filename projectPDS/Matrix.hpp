#ifndef MATRIX
#define MATRIX
#include <vector>
#include <string>
#include <iostream>
#include <memory>

class Matrix{
private :
    std::string label; //nome della matrice
    std::shared_ptr<std::shared_ptr<double[]>[]> values; //matrice dei valori
    int nRows, nColumns;
    std::vector<std::vector<std::string>> rowLabels; //capire bene a cosa servono questi
    std::vector<std::vector<std::string>> colLabels;
    bool labelsSet,rowLabelsSet,colLabelsSet;

public :
    Matrix(){  nRows=0;nColumns=0;};

    Matrix(int rows , int col) :labelsSet(false),rowLabelsSet(false),colLabelsSet(false) , nRows(rows), nColumns(col){
        values =  std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[rows]);
        for (int i = 0; i < rows ; i++) {
            values[i] = std::shared_ptr<double[]>(new double[col]);
            rowLabels.push_back(std::vector<std::string>());  //incerto
            for (int j = 0; j < col; j++) values[i][j] = -1;
        }
        for (int i = 0; i < col ; i++) {
            colLabels.push_back(std::vector<std::string>()); //incerto
        }

    }
    void operator()(){
        nRows = 0;
        nColumns = 0;
    }



    /*
    ~Matrix(){
        if (values != nullptr){
            for (int i = 0 ; i < nRows ; i++) delete[] values[i];
            delete [] values;
            values = nullptr;
        }
    }
    */


    Matrix (Matrix && source){
        label = source.label;
        labelsSet = source.labelsSet;
        colLabels = source.colLabels;
        rowLabelsSet = source.rowLabelsSet;
        nRows = source.nRows;
        nColumns = source.nColumns;
        rowLabels = source.rowLabels;
        colLabels = source.colLabels;
        values = source.values;
        source.values.reset();
        source.nRows = 0;
        source.nColumns = 0;
        source.rowLabels.clear();
        source.colLabels.clear();
    }


    Matrix(const Matrix& source){ //costruttore di copia
        label = source.label;
        labelsSet = source.labelsSet;
        colLabels = source.colLabels;
        rowLabelsSet = source.rowLabelsSet;
        nRows = source.nRows;
        nColumns = source.nColumns;
        values = std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[nRows]);
        rowLabels = source.rowLabels;
        colLabels = source.colLabels;

        for (int i = 0; i < nRows ; i++) {
            values[i] = std::shared_ptr<double[]> (new double[nColumns]);
            for (int j = 0 ; j < nColumns ; j++){
                values[i][j] = source.values[i][j];
            }
        }
    }

    Matrix& operator=(const Matrix &source){
        if (this != &source){


            values.reset();
            rowLabels.clear();
            colLabels.clear();
            label = source.label;
            labelsSet = source.labelsSet;
            colLabels = source.colLabels;
            rowLabelsSet = source.rowLabelsSet;
            nRows = source.nRows;
            nColumns = source.nColumns;
            values = std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[nRows]);
            colLabels = source.colLabels;
            rowLabels = source.rowLabels;
            for (int i = 0 ; i < nRows ; i++){
                values[i] = std::shared_ptr<double[]> (new double[nColumns]);

                for (int j = 0 ; j < nColumns ; j++){
                    values[i][j] = source.values[i][j];
                }
            }

        }
        return *this;
    }
    Matrix& operator=( Matrix &&source){
        if (this != &source){
            values.reset();
            rowLabels.clear();
            colLabels.clear();
            label = source.label;
            labelsSet = source.labelsSet;
            colLabels = source.colLabels;
            rowLabelsSet = source.rowLabelsSet;
            nRows = source.nRows;
            nColumns = source.nColumns;
            values = source.values;
            source.values.reset();
            colLabels = source.colLabels;
            rowLabels = source.rowLabels;
        }
        return *this;
    }

    //modificare questo raw pointer in base all'input
    Matrix(int rows , int col, double* input) :labelsSet(false),rowLabelsSet(false),colLabelsSet(false) , nRows(rows), nColumns(col){
        values = std::shared_ptr<std::shared_ptr<double[]>[]> (new std::shared_ptr<double[]>[rows]);

        for (int i = 0; i < rows ; i++) values[i] = std::shared_ptr<double[]> (new double[col]);

        for (int i = 0; i < rows ; i++){
            rowLabels.push_back(std::vector<std::string>());
            for (int j = 0; j < col ; j++){
                values[i][j] = input[i*col + j];
            }
        }
        for (int i = 0; i < col ; i++) {
            colLabels.push_back(std::vector<std::string>());
        }

    }

    int getRowDimension() {return nRows;}
    int getColDimension(){return nColumns;}
    std::string  getLabel() {
        return label;
    }

    void setLabel(std::string label){
        this->label = label;
    }
    std::vector<std::string> getRowLabels(int index) {
        try {
            return rowLabels.at(index);
        }
        catch (std::exception e){
            std::cout << "fuori indice" << std::endl;
        }
        return std::vector<std::string>();

    }
    std::vector<std::string> getColLabels(int index) {
        try {
            return colLabels.at(index);
        }
        catch (std::exception e){
            std::cout << "fuori indice" << std::endl;
        }
        return std::vector<std::string>();
    }


    void setRowLabels(int index, std::vector<std::string> rowLabel){

        try {
            for (int i = 0; i < rowLabel.size(); i++) rowLabels.at(index).push_back(rowLabel.at(i));
            rowLabelsSet = true;
            if (colLabelsSet) labelsSet = true;
        }
        catch (std::exception e){
            std::cout << "error in setRowLabels";
        }
    }

    void setColLabels(int index, std::vector<std::string> colLabel){
        try {
            for (int i = 0; i < colLabel.size(); i++) colLabels.at(index).push_back(colLabel.at(i));
            colLabelsSet = true;
            if (rowLabelsSet) labelsSet = true;
        }
        catch (std::exception e){
            std::cout << "errore" ;
        }
    }

    double getValue(int row,int col){
        if (row >= nRows || col >= nColumns ) return -1;
        return values[row][col];
    }

    void setValue(int row,int col, double value){
        if (row >= nRows || col >= nColumns ) return ;
        values[row][col] = value;
    }

    double getValue(std::string rLabel, std::string cLabel){
        for (int i = 0 ; i < nRows ; i++){
            if (partOf(rLabel,rowLabels.at(i))){
                for (int j = 0; j < nColumns ; j++){
                    if (partOf(cLabel,colLabels.at(j))){
                        return values[i][j];
                    }
                }
            }
        }

        return -1;
    }


    static bool partOf(std::string str, std::vector<std::string> vec){
        for (int i = 0; i < vec.size(); i++) {
            if (vec.at(i).compare(str)==0) { return true; }
        }
        return false;
    }

    static bool partOf(std::vector<std::string> str, std::vector<std::string> vec){
        bool found;
        for (int i = 0; i < str.size(); i++) {
            found = false;
            for (int j = 0 ; j < vec.size() ; j++){
                if (str.at(i) == vec.at(j)) found = true;
            }
            if (!found) return false;
        }
        return true;
    }
//vectToString , toString

    void printMatrix(){
        std::cout << "etichette colonne " << std::endl;
        for (int i = 0 ; i < colLabels.size() ; i++){
            for (int j =  0 ;  j < colLabels.at(i).size() ; j++){
                std::cout << colLabels.at(i).at(j) << std::endl;
            }
            std::cout << "--------------" << std::endl;
        }

        std::cout << "etichette righe " << std::endl;
        for (int i = 0 ; i < rowLabels.size() ; i++){
            for (int j =  0 ;  j < rowLabels.at(i).size() ; j++){
                std::cout << rowLabels.at(i).at(j) << std::endl;
            }
            std::cout << "--------------" << std::endl;
        }

    }
};

#endif