#include<iostream>
#include<algorithm>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
using namespace std;

unsigned char DATA[512][512];

int main(){
    FILE *ich;
    if( (ich = fopen("lena512.raw", "rb" )) == nullptr ) {
        cout << "failed to open input file\n";
        exit(0);
    }

    fread(DATA, sizeof(unsigned char), 512 * 512, ich);
    fclose(ich);

    double mse = 0;

    for(int i = 0; i < 512; i++){
        for(int j = 0; j < 512; j++){
            int t ;
            if(DATA[i][j] < 64){
                t = 32;
            }else if(DATA[i][j] < 128){
                t = 96;
            }else if(DATA[i][j] < 192){
                t = 160;
            }else {
                t = 224;
            }
            mse += (t-DATA[i][j])*(t-DATA[i][j]);
        }
    }
    mse /= 512*512;
    double psnr = 10 * log10(255 * 255 / mse);
    cout << "MSE: "  << mse << endl;
    cout << "PSNR: " << psnr << endl;

    FILE *och;
    if( (och = fopen("a.raw", "wb" )) == nullptr ) {
        cout << "failed to create output file\n";
        exit(0);
    }

    fwrite(DATA, sizeof(unsigned char), 512 * 512, och);
    fclose(och);

    return 0;
}