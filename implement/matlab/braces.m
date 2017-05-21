%%^^^^^^^^^^^^author: jagadeesh  
%%^^^^^^^^^^^mail id: g.jagadeesh458@gmail.com  
%^^^^^^^project: face and noise and eye detection for matlab image  
%processing tool box  
clear all  
clc  
close all  
  
%To detect Face  
FDetect = vision.CascadeObjectDetector;  
  
%Read the input image  
I = imread('19.jpeg');  
  
%Returns Bounding Box values based on number of objects  
face_dtect = step(FDetect,I);  
  
figure(1),  
imshow(I); hold on  
for i = 1:size(face_dtect,1)  
    rectangle('Position',face_dtect(i,:),'LineWidth',5,'LineStyle','-','EdgeColor','m');  
end  
title('Face Detection');  
hold off;  
% for i=1:size(v,2)  
%     z=[z,norm(cv(i,:)-s,2)];  
%     if(rem(i,20)==0),imshow(reshape(v(:,i),112,92)),end;  
%     drawnow;  
% end  
%   
% [a,i]=min(z);  
% subplot(122);  
% imshow(reshape(v(:,i),112,92));title('Found!','FontWeight','bold','Fontsize',16,'color','red');  
%   
  
  
%To detect Mouth  
mouth_detect = vision.CascadeObjectDetector('Mouth','MergeThreshold',100);  
  
face_dtect=step(mouth_detect,I);  
  
  
figure(2),  
imshow(I); hold on  
for i = 1:size(face_dtect,1)  
    rectangle('Position',face_dtect(i,:),'LineWidth',4,'LineStyle','-','EdgeColor','r'); 
    mouth = imcrop(I,face_dtect);
    ostuThreshold = graythresh(mouth);
    segmentation = im2bw(mouth, ostuThreshold);
    rgb = mouth;
    se=strel('disk',1);
    for k=1:2
        r=rgb(:,:,1);g=rgb(:,:,2);b=rgb(:,:,3);
        im = segmentation;
        im= imdilate(im,se);
        im = imerode(im,se);
        im = imfill(im, 'holes');
        figure,  imshow(im);
        r(~im)=255;
        g(~im)=255;
        b(~im)=255;
        rgb=cat(3,r,g,b);
        figure,imshow(rgb);
        ostuThreshold = graythresh(rgb);
        segmentation = im2bw(rgb, ostuThreshold);
    end
    
    r=rgb(:,:,1);g=rgb(:,:,2);b=rgb(:,:,3);
    im = segmentation;
    im = imerode(im,se);
    im = imfill(im, 'holes');
    figure,  imshow(im);
    r(~im)=255;
    g(~im)=255;
    b(~im)=255;
    rgb=cat(3,r,g,b);
    figure,imshow(rgb);
    ostuThreshold = graythresh(rgb);
    segmentation = im2bw(rgb, ostuThreshold);
    
    r=mouth(:,:,1);g=mouth(:,:,2);b=mouth(:,:,3);
    im = segmentation;
    figure,  imshow(im);
    r(~im)=0;
    g(~im)=255;
    b(~im)=0;
    rgbre4=cat(3,r,g,b);
    figure,imshow(rgbre4);
end  

hold off;