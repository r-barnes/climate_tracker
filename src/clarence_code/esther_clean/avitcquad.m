function avitcquad(coefp,coeft, x, y, coord)


%% avitcquad
% avtriplecountourlin creates an avi file that trackes the triple points
% and the contours of the precipitations and temperature climate
% approximation using linear/ plane fit.
%
% inputs: 
% 1. coefp, coeft are the coefficients of the quadratic surfaces which are the outputs of 'makequad'
% 2. x, y, are the gridded intervals for which the data applies, 
% eg, for Minnesota, x=[-2:0.1:2], y=[40:0.1:50].
% 3. coord is the coordinates of the triple points, which are the outputs of 'triquad'
%
% output: is an animation saved in triplecontour.avi, and can be played
% using eg. quickplayer in mac.
% 
% functions called: fntriplot 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


[X,Y]=meshgrid(x,y);


mov=avifile('triplecontourquad.avi', 'fps',5);

    

for j=1:70
   
  Z=coefp(1,j)*X.^2+coefp(2,j)*Y.^2+coefp(3,j)*X.*Y+coefp(4,j)*X+coefp(5,j)*Y+coefp(6,j);
  W=coeft(1,j)*X.^2+coeft(2,j)*Y.^2+coeft(3,j)*X.*Y+coeft(4,j)*X+coeft(5,j)*Y+coeft(6,j);

  [C,g]=contour(X,Y,Z);
  set(g,'ShowText','on','TextStep',get(g,'LevelStep')*0.5, 'Visible', 'on');
  text_handle = clabel(C,g);
  set(text_handle,'BackgroundColor',[1 .6 1],'Edgecolor',[0.2 0.2 0.2]);
  colormap cool;
  axis equal;
  hold on
  [D,h]=contour(X,Y,W);
  set(h,'ShowText','on','TextStep',get(h,'LevelStep')*0.5);
  %text_handle = clabel(D,h);
  %set(text_handle,'BackgroundColor',[1 1 .6],'Edgecolor',[0.7 0.7 0.7]);
  colormap summer;
  
  fntriplot(coord(:,1:j));
  
  xnow=num2str(coord(1,j));
  ynow=num2str(coord(2,j));
  coordnow=strcat('(',xnow,',',ynow,')');

  text(coord(1,j), coord(2,j)+1, coordnow)
  
  grid on
  
  hold off
  
  fig1=getframe;
  mov=addframe(mov, fig1);  
  
end

 mov=close(mov);



end 