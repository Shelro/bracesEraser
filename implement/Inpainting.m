function zi= Inpainting( img,mask,nIteration )
%UNTITLED Summary of this function goes here
[width,height] = size(img); %获取图像宽度和高度 
a2=1;
	for k=1:nIteration
        
        for h=1:height
            for w=1:width
                if 	(mask(h,w)==0)
                    
                    
                if (w<width&w>1)
					E=w+1;
					W=w-1;
                end
				if (w==1)
					E=w+1;
					W=w+1;
                end
				if (w==width)
					E=w-1;
					W=w-1;
                end
				
				if (h<height&h>1)
					S=h+1;
					N=h-1;
                end
				if (h==1)
					S=h+1;
					N=h+1;
                end
				if (h==height)

					S=h-1;
					N=h-1;
                end
				
				
				Io=img(h,w);
				Ie=img(h,E);
				Iw=img(h,W);
				In=img(N,w);
				Is=img(S,w);
				
				Ine=img(N,E);
				Inw=img(N,W);
				Ise=img(S,E);
				Isw=img(S,W);
				
				We=a2+((Ie-Io)^2)+(0.25*(Ine+In-Is-Ise)^2);
				We=1/sqrt(We);
				
				Ww=a2+((Iw-Io)^2)+(0.25*(Inw+In-Is-Isw)^2);
				Ww=1/sqrt(Ww);
				
				Ws=a2+((Is-Io)^2)+(0.25*(Ise+Ie-Iw-Isw)^2);
				Ws=1/sqrt(Ws);
				
				Wn=a2+((In-Io)^2)+(0.25*(Ine+Ie-Iw-Inw)^2);
				Wn=1/sqrt(Wn);
				
			    img(h,w)=(Ie*We+Iw*Ww+In*Wn+Is*Ws)/(We+Ww+Ws+Wn);
                    
                    
                    
                end 
                
            end
        end
    end
    

    zi=img;
    

end

