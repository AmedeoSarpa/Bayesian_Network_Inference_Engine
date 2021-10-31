#ifndef MATRIX
#define MATRIX
#include <vector>
#include <string>
#include <iostream>
#include <memory>




template <typename  T> class Matrix{
    struct MatrixData {
        std::string label; //nome della matrice
        std::shared_ptr<std::shared_ptr<T[]>[]> values; //matrice dei valori
        int nRows, nColumns;
        std::vector<std::vector<std::string>> rowLabels; //etichette tabelle e colonne
        std::vector<std::vector<std::string>> colLabels;
    };
private :
    std::shared_ptr<MatrixData> m_data;


public :
    Matrix(){ m_data = std::make_shared<MatrixData>(); m_data->nRows=0;m_data->nColumns=0;};

    void operator()(){
        m_data = std::make_shared<MatrixData>(); m_data->nRows=0;m_data->nColumns=0;
    }
    Matrix(int rows , int col) {
        m_data = std::make_shared<MatrixData>();
        m_data->nRows = rows; m_data->nColumns = col;
        m_data->values =  std::shared_ptr<std::shared_ptr<T[]>[]>(new std::shared_ptr<T[]>[rows]);
        for (int i = 0; i < rows ; i++) {
            m_data->values[i] = std::shared_ptr<T[]>(new T[col]);
            m_data->rowLabels.push_back(std::vector<std::string>());
            //for (int j = 0; j < col; j++) m_data->values[i][j] = -1;
        }


        for (int i = 0; i < col ; i++) {
            m_data->colLabels.push_back(std::vector<std::string>());
        }


    }




    Matrix (Matrix && source){
        m_data = source.m_data;

    }


    Matrix(const Matrix& source){
        m_data = source.m_data;
    }

    Matrix& operator=(const Matrix &source){
        if (this != &source){
            m_data = source.m_data;
        }
        return *this;
    }
    Matrix& operator=( Matrix &&source){
        if (this != &source){
            m_data = source.m_data;
        }
        return *this;
    }

    Matrix(int rows , int col, std::vector<T> input){
        m_data = std::make_shared<MatrixData>();
        m_data->nRows = rows;
        m_data->nColumns = col;


        m_data->values = std::shared_ptr<std::shared_ptr<T[]>[]> (new std::shared_ptr<T[]>[rows]);


        for (int i = 0; i < rows ; i++){
            m_data->values[i] = std::shared_ptr<T[]> (new T[col]);
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



    std::vector<std::string>& getRowLabels(int index) {
        if (index < m_data->rowLabels.size()) return m_data->rowLabels.at(index);

    }



    std::vector<std::string>& getColLabels(int index) {
        if (index < m_data->colLabels.size()) return m_data->colLabels.at(index);


    }



    void setRowLabels(int index,const std::vector<std::string>& rowLabel){

        if (m_data.unique() == false) {
            std::shared_ptr<MatrixData> tmp = m_data;

            m_data = std::make_shared<MatrixData>();
            m_data->nColumns = tmp->nColumns;
            m_data->nRows = tmp->nRows;
            m_data->label = tmp->label;
            m_data->rowLabels = tmp->rowLabels;
            m_data->colLabels = tmp->colLabels;
            m_data->values = std::shared_ptr<std::shared_ptr<T[]>[]>(new std::shared_ptr<T[]>[m_data->nRows]);

            for (int i = 0; i < m_data->nRows; i++) {
                m_data->values[i] = std::shared_ptr<T[]>(new T[m_data->nColumns]);
                for (int j = 0; j < m_data->nColumns; j++) {
                    m_data->values[i][j] = tmp->values[i][j];
                }
            }
        }

        try {

            m_data->rowLabels.at(index).clear();
            for (std::string str : rowLabel) m_data->rowLabels.at(index).push_back(str);
        }
        catch (std::exception &e){
            std::cout << "error during set rowLabel" ;
            m_data->rowLabels.at(index).push_back("");
        }
    }

    void setColLabels(int index,const  std::vector<std::string>& colLabel){
        if (m_data.unique() == false) {
            std::shared_ptr<MatrixData> tmp = m_data;

            m_data = std::make_shared<MatrixData>();
            m_data->nColumns = tmp->nColumns;
            m_data->nRows = tmp->nRows;
            m_data->label = tmp->label;
            m_data->rowLabels = tmp->rowLabels;
            m_data->colLabels = tmp->colLabels;
            m_data->values = std::shared_ptr<std::shared_ptr<T[]>[]>(new std::shared_ptr<T[]>[m_data->nRows]);

            for (int i = 0; i < m_data->nRows; i++) {
                m_data->values[i] = std::shared_ptr<T[]>(new T[m_data->nColumns]);
                for (int j = 0; j < m_data->nColumns; j++) {
                    m_data->values[i][j] = tmp->values[i][j];
                }
            }
        }
        try {
            m_data->colLabels.at(index).clear();
            for (std::string str : colLabel)  m_data->colLabels.at(index).push_back(str);

        }
        catch (std::exception &e){
            std::cout << "error during  set colLabel" ;
            m_data->colLabels.at(index).push_back("");
        }
    }

    T getValue(int row,int col){
        if (row >= m_data->nRows || col >= m_data->nColumns ) return -1;
        return m_data->values[row][col];
    }

    void setValue(int row,int col, T value){
        if (row >= m_data->nRows || col >= m_data->nColumns ) return ;
        if (m_data.operator bool() == false || m_data.unique() == false) {
            std::shared_ptr<MatrixData> tmp = m_data;

            m_data = std::make_shared<MatrixData>();
            m_data->nColumns = tmp->nColumns;
            m_data->nRows = tmp->nRows;
            m_data->label = tmp->label;
            m_data->colLabels = tmp->colLabels;
            m_data->rowLabels = tmp->rowLabels;
            m_data->values = std::shared_ptr<std::shared_ptr<T[]>[]>(new std::shared_ptr<T[]>[m_data->nRows]);

            for (int i = 0; i < m_data->nRows; i++) {
                m_data->values[i] = std::shared_ptr<T[]>(new T[m_data->nColumns]);
                for (int j = 0; j < m_data->nColumns; j++) {
                    m_data->values[i][j] = tmp->values[i][j];
                }
            }
        }
        m_data->values[row][col] = value;
    }

    T getValue(const std::string& rLabel, const std::string& cLabel){
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


    static bool partOf(const std::string &str, std::vector<std::string> vec){
        return std::any_of(vec.begin(),vec.end(),[&](const std::string& s){return s == str; });
    }


    static bool partOf(std::vector<std::string> str, const std::vector<std::string> &vec){
        bool found;
        for (int i = 0; i < str.size(); i++) {
            found = false;
            for (std::string s : vec){
                if (str.at(i) == s) found = true;
            }
            if (!found) return false;
        }
        return true;
    }


    /*
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
    */
};

#endif