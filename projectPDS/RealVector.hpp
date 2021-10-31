#ifndef RV
#define RV
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <memory>




template <typename T> class RealVector{
    struct  RealVectorData {
        std::string label;
        std::vector<std::string> valueLabes; //etichetta (R=y, R = n)..
        std::shared_ptr<T[]> values;  //valori
        int size;

    };
private:
    /*
    std::string label;
    std::vector<std::string> valueLabes; //etichetta (R=y, R = n)..
    std::shared_ptr<double[]> values;  //valori
    int size;
     */
    std::shared_ptr<RealVectorData> rv_data;


public :
    RealVector() {rv_data = std::make_shared<RealVectorData>() ; rv_data->size = 0; rv_data->label="";};
    RealVector(int size)  {
        rv_data = std::make_shared<RealVectorData>() ;
        rv_data->size = size;
        rv_data->values = std::shared_ptr<T[]>(new T[size]);
    }
    RealVector(std::vector<T> input)  {
        rv_data = std::make_shared<RealVectorData>();
        rv_data->size = input.size();
        rv_data->values = std::shared_ptr<T[]>(new T[input.size()]);
        for (int i = 0 ; i < input.size() ; i++ ) rv_data->values[i] = input.at(i);

    }

    void operator()(int states) {
        rv_data = std::make_shared<RealVectorData>();
        rv_data->size = states;
        rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
    }

    RealVector(const RealVector& source) {
        rv_data = source.rv_data;
        /*
        size = source.size;
        values = std::shared_ptr<double[]>(new double[size]);
        labelsSet = source.labelsSet;
        label = source.label;
        for (int i = 0; i < source.size; i++) {
            values[i] = source.values[i];
        }
        valueLabes = source.valueLabes;
        */
    }

    RealVector(RealVector && source){
        rv_data =  source.rv_data;
        /*
        size = source.size;
        labelsSet = source.labelsSet;
        values = source.values;
        label = source.label;
        source.values.reset();
        valueLabes = source.valueLabes;
         */
    }

    /*
    ~RealVector(){
    }

     */


    RealVector& operator=(const RealVector &source){
        if (this != &source){
            rv_data = source.rv_data;
            /*
            values.reset();
            valueLabes.clear();
            size = source.size;
            label = source.label;
            values = std::shared_ptr<double[]>(new double[size]);
            labelsSet = source.labelsSet;
            for (int i = 0; i < source.size; i++) {
                values[i] = source.values[i];
            }
            valueLabes = source.valueLabes;
            */

        }
        return *this;
    }
    RealVector& operator=( RealVector &&source){
        if (this != &source){
            rv_data = source.rv_data;
            /*
            values.reset();
            valueLabes.clear();
            size = source.size;
            label = source.label;
            labelsSet = source.labelsSet;
            values = source.values;
            source.values.reset();
            valueLabes = source.valueLabes;
            */
        }
        return *this;
    }



    int getDimension() { return rv_data->size;}

    std::string  getLabel() {return rv_data->label;}


    std::string getLabel(int index){
        try {
            return rv_data->valueLabes[index];
        }
        catch(std::exception& e){
            return "";
        }
    }
    std::vector<std::string> getLabels() {return rv_data->valueLabes;}

    void setLabels (const std::vector<std::string>& labels){//labels.size dovrebbe essere uguale a this->size
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            for (int i = 0; i < rv_data->size ; i++) rv_data->values[i] = tmp->values[i];
        }
        rv_data->valueLabes.clear();
        for (std::string s : labels) rv_data->valueLabes.push_back(s);
    }

    void setLabels(std::string& label,const std::vector<std::string>& labels){
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            for (int i = 0; i < rv_data->size ; i++) rv_data->values[i] = tmp->values[i];
        }

        rv_data->valueLabes.clear();
        for (std::string s : labels) { rv_data->valueLabes.push_back(s) ;}
        this->rv_data->label = label;

    }

    T getValue(int index) {
        if (index >= rv_data->size) return -1;
        return rv_data->values[index];
    }

    T getValue(const std::string &str) {
        for (int i = 0 ; i < rv_data->valueLabes.size();i++){
            if (str == rv_data->valueLabes.at(i)) return rv_data->values[i];
        }
        return -1;
    }


    void setValue(int index , T value){
        if (index < rv_data->size) {
            if (rv_data.operator bool() == false || rv_data.unique() == false) {
                std::shared_ptr<RealVectorData> tmp = rv_data;

                rv_data = std::make_shared<RealVectorData>();
                rv_data->size = tmp->size;
                rv_data->label = tmp->label;
                rv_data->valueLabes = tmp->valueLabes;
                rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
                for (int i = 0; i < rv_data->size; i++) rv_data->values[i] = tmp->values[i];
            }
            rv_data->values[index] = value;
        }
    }

    void setValues(const std::shared_ptr<T[]> &input){
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
        }
        for (int i = 0; i < rv_data->size ; i++){
            rv_data->values[i] = input[i];
        }
    }

    std::shared_ptr<T[]> getValues(){
        return rv_data->values;
    }

    void setValue( const std::string& str , T value){
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            for (int i = 0; i < rv_data->size ; i++) rv_data->values[i] = tmp->values[i];
        }

        for (int i = 0 ; i < rv_data->valueLabes.size() ; i++){
            if (str == rv_data->valueLabes.at(i)){
                rv_data->values[i] = value;
                return;
            }
        }
    }

    bool isAllOnes() {
        for (int i = 0 ; i < rv_data->size ; i++) {
            if (rv_data->values[i] != 1)return false;
        }
        return true;
    }

    void toAllOnes() {
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);

        }
        for (int i = 0 ; i < rv_data->size ; i++) {
            rv_data->values[i] = 1;
        }
    }


    void toAllZeros() {
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            }
        for (int i = 0 ; i < rv_data->size ; i++) {
            rv_data->values[i] = 0;
        }
    }

    void normalise(){   //normalizzazione
        T sum=0;
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            for (int i = 0; i < rv_data->size ; i++) rv_data->values[i] = tmp->values[i];
        }
        for (int i = 0 ; i < rv_data->size; i++) sum+=rv_data->values[i];
        if (sum != 0){
            for (int i = 0 ; i < rv_data->size; i++) rv_data->values[i]= rv_data->values[i]/sum;
        }
    }

    void termProduct(RealVector v1, RealVector v2){
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            for (int i = 0; i < rv_data->size ; i++) rv_data->values[i] = tmp->values[i];
        }
        for (int i = 0; i < rv_data->size; i++)
        {
            rv_data->values[i] = v1.getValue(i) * v2.getValue(i);
        }
    }

    void divide(RealVector v1, RealVector v2){
        if (rv_data.operator bool() == false || rv_data.unique() == false){
            std::shared_ptr<RealVectorData> tmp = rv_data;

            rv_data = std::make_shared<RealVectorData>();
            rv_data->size = tmp->size;
            rv_data->label = tmp->label;
            rv_data->valueLabes = tmp->valueLabes;
            rv_data->values = std::shared_ptr<T[]>(new T[rv_data->size]);
            for (int i = 0; i < rv_data->size ; i++) rv_data->values[i] = tmp->values[i];
        }
        for (int i = 0; i < rv_data->size ; i++)
        {
            T x = v1.getValue(i);
            T y = v2.getValue(i);
            if (x == 0 && y == 0)
                rv_data->values[i] = 0;
            else if (y == 0)
            {
                std::cout << "eccezione, non si può dividere per zero" << std::endl;
                //lanciare eccezione
                rv_data->values[i] = 0;
            }
            else
                rv_data->values[i] = x / y;
        }

    }



    void printTest(){
        T val;
        for (int i = 0 ; i < rv_data->size ; i++) {
            val = rv_data->values[i] * 100;
            std::cout <<  std::setprecision(2) << std::fixed << val << "%" <<  " ";
        }
        std::cout << std::endl;
    }

    std::string GetValuesString() const {
        std::stringstream values_string;
        for (int i = 0; i < rv_data->size; i++) {
            values_string << " " << rv_data->values[i];
        }
        return values_string.str();
    }

};
#endif