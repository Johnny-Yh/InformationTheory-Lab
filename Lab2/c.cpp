#include<iostream>
#include<algorithm>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
using namespace std;

unsigned char DATABUF[512][512];

#define N 512*512
#define K 8


unsigned char DATA[N];
unsigned char DATA1[N];
int center[N];
double mean[K] ={16,48,80,112,144,176,208,240};

double getDistance(double x, double y)
{
    double d;
    d = sqrt((x - y) * (x - y));
    return d;
}

void getMean()
{
    int i, j, count = 0;
    for(i = 0; i < K; ++i)
    {
        count = 0;
        double t = 0;
        for(j = 0; j < N; ++j)
        {
            if(i == center[j])
            {
                count++;
                t += DATA[j];
            }
        }
        t /= count;
        mean[i] = t;
    }
    for(i = 0; i < K; ++i)
    {
        printf("The new center point of %d is : \t %f,\n", i+1, mean[i]);
    }
}

float getE()
{
    int i, j;
    double cnt = 0.0, sum = 0.0;
    for(i = 0; i < K; ++i)
    {
        for(j = 0; j < N; ++j)
        {
            if(i == center[j])
            {
                cnt = (DATA[j]-mean[i])*(DATA[j]-mean[i]);
                sum += cnt;
            }
        }
    }
    return sum;
}

void cluster()
{
    int i, j, q;
    double min;
    double distance[K];
    for(i = 0; i < N; ++i)
    {
        min = 999999.0;
        for(j = 0; j < K; ++j)
        {
            distance[j] = getDistance(DATA[i], mean[j]);
        }
        for(q = 0; q < K; ++q)
        {
            if(distance[q] < min)
            {
                min = distance[q];
                center[i] = q;
            }
        }
        DATA1[i] = mean[center[i]];
    }
    printf("-----------------------------\n");
}

int main(){
    FILE *ich;
    if( (ich = fopen("lena512.raw", "rb" )) == NULL ) {
        cout << "failed to open input file\n";
        exit(0);
    }
    fread(DATABUF, sizeof(unsigned char),512*512,ich);
    fclose(ich);

    cout << "uniformly quantization" << endl;
    double mse = 0;
    for(int i = 0; i<512; i++){
        for(int j = 0; j<512; j++){
            int t ;
            DATA[i*512+j] = DATABUF[i][j];
            if(DATABUF[i][j]<32){
                t = 16;
            }else if(DATABUF[i][j]<64){
                t = 48;
            }else if(DATABUF[i][j]<96){
                t = 80;
            }else if(DATABUF[i][j]<128){
                t = 112;
            }else if(DATABUF[i][j]<160){
                t = 144;
            }else if(DATABUF[i][j]<192){
                t = 176;
            }else if(DATABUF[i][j]<224){
                t = 208;
            }else{
                t = 240;
            }
            mse += (t-DATABUF[i][j])*(t-DATABUF[i][j]);
        }
    }
    mse /= 512*512;
    double psnr = 10*log10(255*255/mse);
    cout << "MSE: "  << mse << endl;
    cout << "PSNR: " << psnr << endl;

    printf("-----------------------------\n");

    cout << "non-uniformly quantization" << endl;

    int n;
    // 第一次聚类
    cluster();          // Apply Clustering algorithm
    // 计算误差平方和
    double temp1 = getE();        //  Calculate mean square error for the fisrt time
    mse = temp1/512/512;
    psnr = 10*log10(255*255/mse);
    cout << "MSE: "  << mse << endl;
    cout << "PSNR: " << psnr << endl;
    // 聚类的次数
    n++;                   //  n The nuber of iterations

    printf("--------------The 1st cluster over---------------\n\n");

    // 第二次聚类
    getMean();
    cluster();
    double temp2 = getE();        //   Re-calculate mean square based on the new cluster center
    mse = temp2/512/512;
    psnr = 10*log10(255*255/mse);
    cout << "MSE: "  << mse << endl;
    cout << "PSNR: " << psnr << endl;
    n++;

    printf("--------------The 2nd cluster over---------------\n\n");

    // 迭代循环，直到相同时停止
    while(abs(temp2 - temp1) > 0.0001)   //  Compare the diference, if unequal, continue
    {
        temp1 = temp2;
        getMean();
        cluster();
        temp2 = getE();
        mse = temp2/512/512;
        n++;
        psnr = 10*log10(255*255/mse);
        cout << "MSE: "  << mse << endl;
        cout << "PSNR: " << psnr << endl;
        printf("--------------The %dst cluster over---------------\n\n", n);
    }

    printf("The total number of iteration is: %d\n\n", n);  // calcuate the number of iterations

    FILE *och;
    if( (och = fopen("c.raw", "wb" )) == nullptr ) {
        cout << "failed to create output file\n";
        exit(0);
    }

    fwrite(DATA1, sizeof(unsigned char), 512 * 512, och);
    fclose(och);

    return 0;
}