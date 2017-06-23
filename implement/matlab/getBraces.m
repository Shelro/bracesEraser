function [mouth,brace] = getBraces(filename)

%To detect Face  
FDetect = vision.CascadeObjectDetector;  
  
%Read the input image  
I = imread(filename);  
  
%Returns Bounding Box values based on number of objects  
face_dtect = step(FDetect,I);  
  
%To detect Mouth  
mouth_detect = vision.CascadeObjectDetector('Mouth','MergeThreshold',100);  
  
face_dtect=step(mouth_detect,I);  
 
for i = 1:size(face_dtect,1)  
    mouth = imcrop(I,face_dtect);
    
    [m,n,f] = size(mouth);
    
    % Remove the part whose color is not silver, the part left is braces
    r = mouth(:,:,1);g = mouth(:,:,2);b = mouth(:,:,3);
    for e=1:m
        for t=1:n
            if((r(e,t)<140 || r(e,t)>180)||(g(e,t)<140 ||g(e,t)>180)||(b(e,t)<140 ||b(e,t)>180))
                r(e,t)=255;
                g(e,t)=255;
                b(e,t)=255;
            end
        end
    end
    m = cat(3,r,g,b);
    
    ostuThreshold = graythresh(m);
    segmentation = im2bw(m, ostuThreshold);
    rgb = m;
    se=strel('disk',2);
   
    %If some part is huge that might be mouth or others instead of braces,
    %replace them with white pixels.
    r=rgb(:,:,1);g=rgb(:,:,2);b=rgb(:,:,3);
    im = segmentation;
    im = imerode(im,se);
    im = imfill(im, 'holes');
    r(~im)=255;
    g(~im)=255;
    b(~im)=255;
    rgb=cat(3,r,g,b);
    ostuThreshold = graythresh(rgb);
    segmentation = im2bw(rgb, ostuThreshold);
   
    %Expand the the part of braces
    se=strel('disk',2);
    r=mouth(:,:,1);g=mouth(:,:,2);b=mouth(:,:,3);
    im = segmentation;
    im = imerode(im,se);
    r(~im)=0;
    g(~im)=255;
    b(~im)=0;
    brace=cat(3,r,g,b);
end  