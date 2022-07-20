#include <cstdio>
#include <cstdlib>
#include <cstring>

/*
计算2个边界框的IOU
    输入参数： 边界框a和边界框b的坐标
        x1 < x2, y1 < y2
        边界框1:(xa1,ya1,xa2,ya2)
        边界框1:(xb1,yb1,xb2,yb2)
    输出：
        边界框a和b的IOU
    IOU = a和b的交集/a和b的并集
*/

double IoU(double boxA[4], double boxB[4]){
    if(
        boxA[0] > boxA[2] ||
        boxA[1] > boxA[3] ||
        boxB[0] > boxB[2] ||
        boxB[1] > boxB[3]
    ) return 0.0; //不符合边界框要求

    double S_a = (boxA[2] - boxA[0]) * (boxA[3] - boxA[1]); //boxA的面积
    double S_b = (boxB[2] - boxB[0]) * (boxB[3] - boxB[1]); //boxB的面积
    // printf("%.2f %.2f ", S_a, S_b);
    
    double interLeftX = boxA[0] > boxB[0] ? boxA[0] : boxB[0]; //交集的左X 两个框的左点的X的较大者
    double interRightX = boxA[2] < boxB[2] ? boxA[2] : boxB[2]; //交集的右X 两个框的右点的X的较小者
    double interLeftY = boxA[1] > boxB[1] ? boxA[1] : boxB[1];
    double interRightY = boxA[3] < boxB[3] ? boxA[3] : boxB[3];

    double w = interRightX - interLeftX;
    double h = interRightY - interLeftY;

    if(w < 0 || h < 0){
        // printf("w: %.2f h: %.2f\n", w,h);
        return 0.0;
    }

    double S_inter = (interRightX - interLeftX) * (interRightY - interLeftY);
    // printf("%.2f ", S_inter);

    return S_inter / (S_a + S_b - S_inter);
}

typedef struct b_s
{
    double xyxy[4];
    double score;
}box_score;

void boxCopy(box_score* dest, const box_score* box){
    dest->score = box->score;
    memcpy(dest->xyxy, box->xyxy, 4*sizeof(double));
    return;
}

box_score* NMS(box_score* bbox, double threshold, int num, int* lenRETURN){
    //首先按照置信度分数对边界框排序
    //从中选择置信度最大的一个边界框i加入输出集合
    //从剩下的边界框中将与边界框i的IoU大于给定阈值的边界框删除
    //从剩下的边界框中重复上述过程，直到没有边界框
    //return NMS之后的边界框的下标
    int cnt = num;
    box_score* BoxReturn = (box_score*)malloc(cnt * sizeof(box_score));
    box_score* boxTMP = (box_score*)malloc(cnt * sizeof(box_score));

    int r = 0;
    while(cnt>0){
        int maxI = 0;
        double maxV = bbox->score;
        for(int i = 1; i < cnt; i++){
            if((bbox+i)->score > maxV){
                maxV = (bbox+i)->score;
                maxI = i;
            }
        }
        printf("%f %d\n", (bbox+maxI)->score, maxI);
        boxCopy((BoxReturn+ (r++)), (bbox+maxI)); //将置信度最大的放入NMS输出集合

        int i = 0, k = 0;
        for(; i < cnt; i++){
            if(i == maxI) continue;
            double iou = IoU((bbox+maxI)->xyxy, (bbox+i)->xyxy);
            printf("%.4f\n",iou);
            if(iou < threshold){
                boxCopy((boxTMP+(k++)), (bbox+i));
            }
        }
        for(int j = 0; j < k; j++){
            boxCopy((bbox+j), (boxTMP+j));
        }
        cnt = k;
        printf("%d\n",cnt);
    }
    *lenRETURN = r;
    return BoxReturn;
}

double gtbox[4] = {70, 80, 120, 150};
double pred_bbox[][4] = {
    {15, 18, 47, 60},
    {90, 50, 130, 100},
    {70, 80, 120, 145},
    {130, 160, 250, 280},
    {25.6, 66.1, 113.3, 147.8}
};

box_score box_s1[4] = {
    {11.5, 12, 311.4, 410.6, 0.85},
    {0.5, 1, 300.4, 400.5, 0.97},
    {200.5, 300, 700.4, 1000.6, 0.65},
    {250.5, 310, 700.4, 1000.6, 0.72}
};

box_score box_s2[6] = {
    {15, 18, 47, 60,           0.3503},
    {90, 50, 130, 100,         0.4084},
    {70, 80, 120, 145,         0.7064},
    {130, 160, 250, 280,       0.1846},
    {25.6, 66.1, 113.3, 147.8, 0.6571},
    {70, 80, 120, 150,         0.6797}
};

int main(int argc, char const *argv[])
{
    int len = 0;
    box_score* boxR = NMS(box_s2, 0.3, 6, &len);
    for(int i = 0; i < len; i++){
        for(int j = 0; j < 4; j++){
            printf("%f ", boxR[i].xyxy[j]);
        }
        printf("%f\n", boxR[i].score);
    }
    return 0;
}
