#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define N 512 * 512
#define K 4


unsigned char DATA[N];
int center[N];
double mean[K] ={32, 96, 160, 224};

// 计算每个点之间的距离
double getDistance(double x, double y)
{
    double d;
    d = sqrt((x - y) * (x - y));
    return d;
}

// compute the center of each cluster
// 重新计算每个簇的中心
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

// compute the mean square error
// 计算每个簇的均方差
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

// Put N points in clusters
// 将N个点根据距离分类到簇中
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
            // 计算每个点和簇中心的距离
            distance[j] = getDistance(DATA[i], mean[j]);
        }
        for(q = 0; q < K; ++q)
        {
            if(distance[q] < min)
            {
                // 如果有距离更近的簇 那么就更新点所属于的簇
                min = distance[q];
                center[i] = q;
            }
        }
    }
    printf("-----------------------------\n");
}

int main()
{
    // 对图像进行一维K-Means
    FILE *ich;
    int n = 0;
    double psnr;
    if( (ich = fopen("lena512.raw", "rb" )) == NULL ) {
        cout << "failed to open input file\n";
        exit(0);
    }
    fread(DATA, sizeof(unsigned char),512*512,ich);
    fclose(ich);

    // 第一次聚类
    cluster();          // Apply Clustering algorithm
    // 计算误差平方和
    double temp1 = getE();        //  Calculate mean square error for the fisrt time
    double mse = temp1/512/512;
    psnr = 10*log10(255*255/mse);
    cout << "MSE: " << mse << endl;
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
    cout << "MSE: " << mse << endl;
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
        cout << "MSE: " << mse << endl;
        cout << "PSNR: " << psnr << endl;
        printf("--------------The %dst cluster over---------------\n\n", n);
    }

    printf("The total number of iteration is: %d\n\n", n);  // calcuate the number of iterations

        FILE *och;
    if( (och = fopen("b.raw", "wb" )) == nullptr ) {
        cout << "failed to create output file\n";
        exit(0);
    }

    fwrite(DATA, sizeof(unsigned char), 512 * 512, och);
    fclose(och);

    return 0;
}