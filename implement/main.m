


%[i1,i2,i3,c,d,mov] = inpaint('wave1.bmp','узуж.bmp',[0 255 0]);
[mouth,brace] = getBraces('5.jpeg');
[i1,i2,i3,c,d,mov] = inpaint(mouth,brace,[0 255 0]);
   plotall;           % quick and dirty plotting script
   close; movie(mov); % grab some popcorn 
   