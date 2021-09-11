#ifndef RV
#define RV
#include <string>
#include <vector>
#include <iostream>
class RealVector{
private:
    std::string label;
    std::vector<std::string> valueLabes; //etichetta (R=y, R = n)..
    bool labelsSet ;
    double* values;  //valori
    int size;


public :
    RealVector() {values = nullptr;size = 0;label="null";};
    RealVector(int size) :labelsSet(false) , size(size){
        values = new double[size];
    }
    RealVector(std::vector<double> input) :labelsSet(false) , size(input.size()) {
        values = new double[input.size()];
        for (int i = 0 ; i < input.size() ; i++ ) values[i] = input.at(i);

    }

    void operator()(int states) {
        size = states;
        labelsSet = false;
        values = new double[size];
    }

    RealVector(const RealVector& source) {
        size = source.size;
        values = new double[size];
        labelsSet = source.labelsSet;
        label = source.label;
        for (int i = 0; i < source.size; i++) {
            values[i] = source.values[i];
        }
        valueLabes = source.valueLabes;
    }

    RealVector(RealVector && source){
        size = source.size;
        labelsSet = source.labelsSet;
        values = source.values;
        label = source.label;
        source.label = nullptr;
        valueLabes = source.valueLabes;
    }

    ~RealVector(){
        if (values != nullptr) delete [] values;
    }


    RealVector& operator=(const RealVector &source){
        if (this != &source){
            if (values != nullptr) delete [] values;
            values = nullptr;
            valueLabes.clear();
            size = source.size;
            label = source.label;
            values = new double[size];
            labelsSet = source.labelsSet;
            for (int i = 0; i < source.size; i++) {
                values[i] = source.values[i];
            }
            valueLabes = source.valueLabes;

        }
        return *this;
    }
    RealVector& operator=( RealVector &&source){
        if (this != &source){
            if (values != nullptr) delete [] values;
            values = nullptr;
            valueLabes.clear();
            size = source.size;
            label = source.label;
            labelsSet = source.labelsSet;
            values = source.values;
            source.values = nullptr;
            valueLabes = source.valueLabes;
        }
        return *this;
    }



    int getDimension() { return size;}

    std::string  getLabel() {return label;}

    void setLabel(std::string label) {this->label= label;}

    std::string getLabel(int index){
        try {
            return valueLabes[index];
        }
        catch(std::exception e){
            std::cout << "nessun  valore";
        }
        return "null";
    }
    std::vector<std::string> getLabels() {return valueLabes;}

    void setLabels (std::vector<std::string> labels){//labels.size dovrebbe essere uguale a this->size
        for (int i = 0 ; i < labels.size() ; i++) valueLabes.push_back(labels.at(i));
    }

    void setLabels(std::string label, std::vector<std::string> labels){
        for (int i = 0 ; i < labels.size() ; i++) { valueLabes.push_back(labels.at(i)) ;}
        this->label = label;

    }

    double getValue(int index) {
        if (index >= size) return -1;
        return values[index];
    }

    double getValue(std::string str) {
        for (int i = 0 ; i < valueLabes.size();i++){
            if (str == valueLabes.at(i)) return values[i];
        }
        return -1;
    }


    void setValue(int index , double value){
        if (index < size)
            values[index] = value;
    }

    void setValues(double * input){
        for (int i = 0; i < size ; i++){
            values[i] = input[i];
        }
    }

    double* getValues(){
        return values;
    }

    void setValue( std::string str , double value){
        for (int i = 0 ; i < valueLabes.size() ; i++){
            if (str == valueLabes.at(i)){
                values[i] = value;
                return;
            }
        }
    }

    bool isAllOnes() {
        for (int i = 0 ; i < size ; i++) {
            if (values[i] != 1) return false;
        }
        return true;
    }

    void toAllOnes() {
        for (int i = 0 ; i < size ; i++) {
            values[i] = 1;
        }
    }


    void toAllZeros() {
        for (int i = 0 ; i < size ; i++) {
            values[i] = 0;
        }
    }

    void normalise(){   //normalizzazione
        double sum=0;
        for (int i = 0 ; i < size; i++) sum+=values[i];
        if (sum != 0){
            for (int i = 0 ; i < size; i++) values[i]= values[i]/sum;
        }
    }

    void termProduct(RealVector v1, RealVector v2){
        for (int i = 0; i < size; i++)
        {
            values[i] = v1.getValue(i) * v2.getValue(i);
        }
    }

    void divide(RealVector v1, RealVector v2){ //dove serve questa divisione?
        for (int i = 0; i < size ; i++)
        {
            double x = v1.getValue(i);
            double y = v2.getValue(i);
            if (x == 0 && y == 0)
                values[i] = 0;
            else if (y == 0)
            {
                std::cout << "eccezione, non si può dividere per zero" << std::endl;
                //lanciare eccezione
            }
            else
                values[i] = x / y;
        }

    }



    void printTest(){
        for (int i = 0 ; i < size ; i++) std::cout << values[i] << " " << std::endl;
    }

};
#endif