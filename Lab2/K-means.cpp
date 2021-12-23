#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <math.h>  
#include <time.h>  
  
#define N 11  
#define K 3  
  
struct Point
{  
    float x;  
    float y;  
};  
  
int center[N];  ///  determin which cluster each point belongs to 
  
Point point[N] = {  
    {2.0, 10.0},  
    {2.0, 5.0},  
    {8.0, 4.0},  
    {5.0, 8.0},  
    {7.0, 5.0},  
    {6.0, 4.0},  
    {1.0, 2.0},  
    {4.0, 9.0},  
    {7.0, 3.0},  
    {1.0, 3.0},  
    {3.0, 9.0}  
};  
  
Point mean[K];  ///  save the center of each cluster  

// 计算每个点之间的距离  
float getDistance(Point point1, Point point2)  
{  
    float d;  
    d = sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));  
    return d;  
}  
  
/// compute the center of each cluster
// 重新计算每个簇的中心
void getMean(int center[N])  
{  
    Point tep;  
    int i, j, count = 0;  
    for(i = 0; i < K; ++i)  
    {  
        count = 0;  
        tep.x = 0.0;   /// set to zero after the center of each cluster is derived  
        tep.y = 0.0;  
        for(j = 0; j < N; ++j)  
        {  
            if(i == center[j])  
            {  
                count++;  
                tep.x += point[j].x;  
                tep.y += point[j].y;  
            }  
        }  
        tep.x /= count;  
        tep.y /= count;  
        mean[i] = tep;  
    }  
    for(i = 0; i < K; ++i)  
    {  
        printf("The new center point of %d is : \t( %f, %f )\n", i+1, mean[i].x, mean[i].y);  
    }  
}  
  
// compute the mean square error
// 计算每个簇的均方差
float getE()  
{  
    int i, j;  
    float cnt = 0.0, sum = 0.0;  
    for(i = 0; i < K; ++i)  
    {  
        for(j = 0; j < N; ++j)  
        {  
            if(i == center[j])  
            {  
                cnt = (point[j].x - mean[i].x) * (point[j].x - mean[i].x) + (point[j].y - mean[i].y) * (point[j].y - mean[i].y);  
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
    float min;  
    float distance[N][K];  
    for(i = 0; i < N; ++i)  
    {  
        min = 999999.0;  
        for(j = 0; j < K; ++j)  
        {  
            // 计算每个点和簇中心的距离
            distance[i][j] = getDistance(point[i], mean[j]);  
  
            /* printf("%f\n", distance[i][j]);  /// used to measure the distance between each point and 3 centers*/  
        }  
        for(q = 0; q < K; ++q)  
        {  
            if(distance[i][q] < min)  
            {  
                // 如果有距离更近的簇 那么就更新点所属于的簇
                min = distance[i][q];  
                center[i] = q;  
            }  
        }  
        printf("( %.0f, %.0f )\t in cluster-%d\n", point[i].x, point[i].y, center[i] + 1);  
    }  
    printf("-----------------------------\n");  
}  
  
int main()  
{  
    int i, j, n = 0;  
    float temp1;  
    float temp2, t;  
    printf("----------Data sets----------\n");  
    for(i = 0; i < N; ++i)  
    {  
        printf("\t( %.0f, %.0f )\n", point[i].x, point[i].y);  
    }  
    printf("-----------------------------\n");  
  
    /* 
        Choose current time as random number 
        srand((unsigned int)time(NULL)); 
    
        for(i = 0; i < K; ++i) 
        { 
            j = rand() % K; 
            mean[i].x = point[j].x; 
            mean[i].y = point[j].y; 
        } 
    */  
    mean[0].x = point[0].x;      /// Initialize K beginning points
    mean[0].y = point[0].y;  
  
    mean[1].x = point[3].x;  
    mean[1].y = point[3].y;  
  
    mean[2].x = point[6].x;  
    mean[2].y = point[6].y;  
  
    cluster();         /// Apply Clustering algorithm
    temp1 = getE();        ///  Calculate mean square error for the fisrt time  
    n++;                   ///  n The nuber of iterations
  
    printf("The E1 is: %f\n\n", temp1);  
  
    getMean(center); 
    cluster();  
    temp2 = getE();        ///   Re-calculate mean square based on the new cluster center
    n++;  
  
    printf("The E2 is: %f\n\n", temp2);  
  
    while(fabs(temp2 - temp1) != 0)   ///  Compare the diference, if unequal, continue 
    {  
        temp1 = temp2;  
        getMean(center);  
        cluster();
        temp2 = getE();
        n++;  
        printf("The E%d is: %f\n", n, temp2);  
    }  
  
    printf("The total number of iteration is: %d\n\n", n);  /// calcuate the number of iterations 
    system("pause");  
    return 0;  
}  
