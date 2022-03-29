#include<iostream>
#include <complex>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

const double PI = 3.14159265358979323846;

vector<complex<double>> fft(vector<complex<double>> v){
    //base case
    if(v.size()==1) return v;

    // recursive step
    vector<complex<double>> even,odd;
    for(int i=0;i<v.size()/2;i++){
        complex<double> e(v[i*2]),o(v[i*2+1]);
        even.push_back(e);
        odd.push_back(o);
    }
    even=fft(even);
    odd=fft(odd);
    vector<complex<double>> result(v.size());
    for(int i=0;i<v.size()/2;i++){
        double n=(i*PI*2)/v.size();
        complex<double> sk(cos(n),sin(n)),e(even[i]),o(odd[i]);

        result[i]=e+(o*sk);
        result[i+v.size()/2]=e-(o*sk);
    }
    return result;
}
//input audio format:
//name: "inputaudioN.data, where N is a number from 1 to 5 (there are 5 inputdata files given)
int main() {
    //number of the file
    int fileNum;
    cout<<"Specify Inputaudio number to analyze: "<<endl;
    cin>>fileNum;
    if(fileNum<1 || fileNum>5){
        cout<<"File Number invalid";
        return 1;
    }
    //fstreams definition
    ifstream ifs("inputaudio"+ to_string(fileNum)+".data",ifstream::binary);
    ofstream ofs("outputVAD"+ to_string(fileNum)+".data",ifstream::binary);
    ofstream validation("outputVAD"+ to_string(fileNum)+".txt");
    //processing
    while(ifs.good()){
        vector<complex<double>> v;
        vector<signed char> write;
        //every package is made at most of 160 samples
      for(int i=0;i<160 && ifs.good();i++){
          signed char c;
          ifs.read((char*)&c, sizeof(c));
          v.push_back(complex<double>((double)c,0));
          write.push_back(c);
      }
      //FFT applied on vector of samples
      v=fft(v);
      //freq
      double freq=sqrt(v[0].real()*v[0].real()+v[0].imag()*v[0].imag());
      for(auto i: v){
          freq=max(freq,sqrt(i.real()*i.real()+i.imag()*i.imag()));
      }
      //check if freq is in range of human audible sound
      bool isInRange=(freq>=200 && freq<=3400);
      //write in validation if the file is in human audible sound or not
      validation.put(isInRange+'0');
      validation.put('\n');
      //write in ofs the original file if it is in range, all zero otherwise
      for(auto i: write){
          if(isInRange) ofs.write((char*)&i,sizeof(i));
          else{
              signed char tmp=0;
              ofs.write((char*)&tmp,sizeof(tmp));
          }
      }
    }
    ifs.close();
    ofs.close();
    validation.close();
    return 0;
}
