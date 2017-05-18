//
//  braces.cpp
//  tryc++
//
//  Created by 张映艺 on 2017/5/11.
//  Copyright © 2017年 张映艺. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include <opencv2/highgui//highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(){
    Mat img = imread("/Users/zhangyingyi/Documents/重要文件/同济/学习/计算机视觉/FP/image/image/1.jpg");
    //imshow("ResultImage", img);
    
    CascadeClassifier mFaceDetector;
    CascadeClassifier mMouthDetector;
    //载入两个人脸特征分类器文件，可以从opencv的安装目录中找到
    if( mFaceDetector.empty() )
        mFaceDetector.load( "/Users/zhangyingyi/Documents/重要文件/同济/学习/计算机视觉/FP/image/image/haarcascade_frontalface_default.xml" );
    if( mMouthDetector.empty() )
        mMouthDetector.load( "/Users/zhangyingyi/Documents/重要文件/同济/学习/计算机视觉/FP/image/image/haarcascade_mcs_mouth.xml" );
    
    vector< cv::Rect > faceVec;
    Mat mElabImage;//备份frame图像
    img.copyTo( mElabImage );
    Mat gray;
    cvtColor(img,gray,CV_BGR2GRAY); //转换成灰度图，因为harr特征从灰度图中提取
    equalizeHist(gray,gray);  //直方图均衡行
    mFaceDetector.detectMultiScale( gray, faceVec, 1.1);//, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
    int i, j;
    for(  i=0; i<faceVec.size(); i++ ){
        rectangle( mElabImage, faceVec[i], CV_RGB(255,0,0), 2 );
        Mat face = img(faceVec[i]);
       
        //检测嘴巴
        vector< cv::Rect > mouthVec;
        Rect halfRect = faceVec[i];
        halfRect.height /= 2;
        halfRect.y += halfRect.height;
        
        Mat halfFace = img( halfRect );
        
        Mat halfgray;
        cvtColor(halfFace,halfgray,CV_BGR2GRAY); //转换成灰度图，因为harr特征从灰度图中提取
        equalizeHist(halfgray,halfgray);  //直方图均衡行

        mMouthDetector.detectMultiScale( halfgray, mouthVec, 2.5);
        
        for(  j=0; j<mouthVec.size(); j++ ){
            Rect rect = mouthVec[j];
            rect.x += halfRect.x;
            rect.y += halfRect.y;
            
            rectangle( mElabImage, rect, CV_RGB(255,255,255), 2 );
            
            Mat mouth = halfFace(mouthVec[j]);
            //imshow("face", mouth);
    }
}
    
    
    //在窗口中显示图像
    imshow("Extracted Frame",mElabImage);
    
    waitKey(0);
    return 0;
}
