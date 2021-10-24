#ifndef MATRIX
#define MATRIX
#include <vector>
#include <string>
#include <iostream>
#include <memory>

struct MatrixData {
    std::string label; //nome della matrice
    std::shared_ptr<std::shared_ptr<double[]>[]> values; //matrice dei valori
    int nRows, nColumns;
    std::vector<std::vector<std::string>> rowLabels; //capire bene a cosa servono questi
    std::vector<std::vector<std::string>> colLabels;

};



class Matrix{
private :
    std::shared_ptr<MatrixData> m_data;
    /*
    std::string label; //nome della matrice
    std::shared_ptr<std::shared_ptr<double[]>[]> values; //matrice dei valori
    int nRows, nColumns;
    std::vector<std::vector<std::string>> rowLabels; //capire bene a cosa servono questi
    std::vector<std::vector<std::string>> colLabels;
     */

public :
    Matrix(){ m_data = std::make_shared<MatrixData>(); m_data->nRows=0;m_data->nColumns=0;};

    void operator()(){
        m_data = std::make_shared<MatrixData>(); m_data->nRows=0;m_data->nColumns=0;
    }
    Matrix(int rows , int col) {
        m_data = std::make_shared<MatrixData>();
        m_data->nRows = rows; m_data->nColumns = col;
        m_data->values =  std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[rows]);
        for (int i = 0; i < rows ; i++) {
            m_data->values[i] = std::shared_ptr<double[]>(new double[col]);
            m_data->rowLabels.push_back(std::vector<std::string>());  //incerto
            for (int j = 0; j < col; j++) m_data->values[i][j] = -1;
        }
        for (int i = 0; i < col ; i++) {
            m_data->colLabels.push_back(std::vector<std::string>()); //incerto
        }

    }




    Matrix (Matrix && source){
        m_data = source.m_data;
        /*
        m_data->label = source.m_data->label;
        colLabels = source.colLabels;
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
        */
    }


    Matrix(const Matrix& source){ //costruttore di copia
        /*
        label = source.label;
        colLabels = source.colLabels;
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
         */
        m_data = source.m_data;
    }

    Matrix& operator=(const Matrix &source){
        if (this != &source){
            m_data = source.m_data;
            /*
            values.reset();
            rowLabels.clear();
            colLabels.clear();
            label = source.label;
            colLabels = source.colLabels;
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
            */
        }
        return *this;
    }
    Matrix& operator=( Matrix &&source){
        if (this != &source){
            m_data = source.m_data;
            /*
            values.reset();
            rowLabels.clear();
            colLabels.clear();
            label = source.label;
            colLabels = source.colLabels;
            nRows = source.nRows;
            nColumns = source.nColumns;
            values = source.values;
            source.values.reset();
            colLabels = source.colLabels;
            rowLabels = source.rowLabels;
            */
        }
        return *this;
    }

    Matrix(int rows , int col, std::vector<double> input){
        m_data = std::make_shared<MatrixData>();
        m_data->nRows = rows;
        m_data->nColumns = col;


        m_data->values = std::shared_ptr<std::shared_ptr<double[]>[]> (new std::shared_ptr<double[]>[rows]);

        for (int i = 0; i < rows ; i++) m_data->values[i] = std::shared_ptr<double[]> (new double[col]);

        for (int i = 0; i < rows ; i++){
            m_data->rowLabels.push_back(std::vector<std::string>());
            for (int j = 0; j < col ; j++){
                m_data->values[i][j] = input[i*col + j];
            }
        }
        for (int i = 0; i < col ; i++) {
            m_data->colLabels.push_back(std::vector<std::string>());
        }

    }

    int getRowDimension() {return m_data->nRows;}
    int getColDimension(){return m_data->nColumns;}



    std::vector<std::string> getRowLabels(int index) {
        try {
            return m_data->rowLabels.at(index);
        }
        catch (std::exception e){
            return std::vector<std::string>();
        }
    }



    std::vector<std::string> getColLabels(int index) {
        try {
            return m_data->colLabels.at(index);
        }
        catch (std::exception e){
            return std::vector<std::string>();
        }

    }


    void setRowLabels(int index, std::vector<std::string> rowLabel){

        if (m_data.operator bool() == false || m_data.unique() == false) {
            std::shared_ptr<MatrixData> tmp = m_data;
            m_data = std::make_shared<MatrixData>();
            m_data->nColumns = tmp->nColumns;
            m_data->nRows = tmp->nRows;
            m_data->label = tmp->label;
            m_data->colLabels = tmp->colLabels;
            m_data->values = std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[m_data->nRows]);

            for (int i = 0; i < m_data->nRows; i++) {
                m_data->values[i] = std::shared_ptr<double[]>(new double[m_data->nColumns]);
                for (int j = 0; j < m_data->nColumns; j++) {
                    m_data->values[i][j] = tmp->values[i][j];
                }
            }
        }

        try {
            for (int i = 0; i < rowLabel.size(); i++) m_data->rowLabels.at(index).push_back(rowLabel.at(i));
        }
        catch (std::exception e){
            std::cout << "errore settaggio rowLabel" ;
            m_data->rowLabels.at(index).push_back("");
        }
    }

    void setColLabels(int index, std::vector<std::string> colLabel){
        if (m_data.operator bool() == false || m_data.unique() == false) {
            std::shared_ptr<MatrixData> tmp = m_data;
            m_data = std::make_shared<MatrixData>();
            m_data->nColumns = tmp->nColumns;
            m_data->nRows = tmp->nRows;
            m_data->label = tmp->label;
            m_data->rowLabels = tmp->rowLabels;
            m_data->values = std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[m_data->nRows]);

            for (int i = 0; i < m_data->nRows; i++) {
                m_data->values[i] = std::shared_ptr<double[]>(new double[m_data->nColumns]);
                for (int j = 0; j < m_data->nColumns; j++) {
                    m_data->values[i][j] = tmp->values[i][j];
                }
            }
        }
        try {
            for (int i = 0; i < colLabel.size(); i++) m_data->colLabels.at(index).push_back(colLabel.at(i));

        }
        catch (std::exception e){
            std::cout << "errore settaggio colLabel" ;
            m_data->colLabels.at(index).push_back("");
        }
    }

    double getValue(int row,int col){
        if (row >= m_data->nRows || col >= m_data->nColumns ) return -1;
        return m_data->values[row][col];
    }

    void setValue(int row,int col, double value){ //qui bisonga fare la copia
        if (row >= m_data->nRows || col >= m_data->nColumns ) return ;
        if (m_data.operator bool() == false || m_data.unique() == false) {
            std::shared_ptr<MatrixData> tmp = m_data;
            m_data = std::make_shared<MatrixData>();
            m_data->nColumns = tmp->nColumns;
            m_data->nRows = tmp->nRows;
            m_data->label = tmp->label;
            m_data->colLabels = tmp->colLabels;
            m_data->rowLabels = tmp->rowLabels;
            m_data->values = std::shared_ptr<std::shared_ptr<double[]>[]>(new std::shared_ptr<double[]>[m_data->nRows]);

            for (int i = 0; i < m_data->nRows; i++) {
                m_data->values[i] = std::shared_ptr<double[]>(new double[m_data->nColumns]);
                for (int j = 0; j < m_data->nColumns; j++) {
                    m_data->values[i][j] = tmp->values[i][j];
                }
            }
        }
        m_data->values[row][col] = value;
    }

    double getValue(std::string rLabel, std::string cLabel){
        for (int i = 0 ; i < m_data->nRows ; i++){
            if (partOf(rLabel,m_data->rowLabels.at(i))){
                for (int j = 0; j < m_data->nColumns ; j++){
                    if (partOf(cLabel,m_data->colLabels.at(j))){
                        return m_data->values[i][j];
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

    void printMatrix(){
        std::cout << "etichette colonne " << std::endl;
        for (int i = 0 ; i < m_data->colLabels.size() ; i++){
            for (int j =  0 ;  j < m_data->colLabels.at(i).size() ; j++){
                std::cout << m_data->colLabels.at(i).at(j) << std::endl;
            }
            std::cout << "--------------" << std::endl;
        }

        std::cout << "etichette righe " << std::endl;
        for (int i = 0 ; i < m_data->rowLabels.size() ; i++){
            for (int j =  0 ;  j < m_data->rowLabels.at(i).size() ; j++){
                std::cout << m_data->rowLabels.at(i).at(j) << std::endl;
            }
            std::cout << "--------------" << std::endl;
        }

    }
};

#endif