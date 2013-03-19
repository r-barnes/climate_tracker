function avitclinmap(coefp,coeft,tlat, tlon, loc, proj)

%% avitcquadmap
%  avitcquadmap creates an avi file that tracks the triple points
% and the contours of the precipitations and temperature climate
% approximation using linear/ plane fit on a map using the Matlab's Mapping
% Toolbox
%
% inputs: 
% 1. coefp, coeft are the coefficients of the quadratic surface which are the outputs of 'makequads'
% 2. tlat, tlon are coord is the coordinates of the triple points, which are the outputs of 'triquad'
% after transformed to the geocoordinate ie, (lat, lon).
% 3. loc is the location which is acceptable by the matlab function
% 'usamap' in the matlab toolbox
% 4. proj is the projection used in the the transformation
%
% output: is an animation saved in tclinmap.avi, and can be played
% using eg. quickplayer in mac.
% 
% functions called: geoinit
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% creating the file tcquadmap.avi to hold the frames 
mov=avifile('tclinmap.avi', 'fps',3);

%% s is the number of frames that will be created this function
s=size(coefp,2);

for j=1:s
%%  geoinit initializes the frame 
    [latlim, lonlim]=geoinit(loc,proj);
%%  creating mesh graticules within the lon and lat limits given by lonlim and latlim  
    dlat=(latlim(2)-latlim(1))/60;
    dlon=(lonlim(2)-lonlim(1))/60;    
    lat=latlim(1):dlat:latlim(2);
    lon=lonlim(1):dlon:lonlim(2);
    [X,Y]=meshgrat(lat, lon);
%%  converting the lat lon coordinate [X,Y] in the mesh graticules to planar coordinate [x,y]
%   using the projection defined  
    [x,y]=ll2pl(X, Y, proj);
%%  creating the planes using planar coordinate [x,y]  
%   see documentation

   P=coefp(1,j)*x+coefp(2,j)*y+coefp(3,j);
   T=coeft(1,j)*x+coeft(2,j)*y+coeft(3,j);
   
%% plotting the triple points tlat, tlon, and the contours given by the coefficients in coefp and coeft   
   plotm(tlat(1:j), tlon(1:j), 'b-'); 
   contourm(X, Y, P, 'g', 'Showtext', 'on');
   contourm(X, Y, T, 'r', 'showtext', 'on');
%% capturing the frame and putting it away in mov and closing the current
%  frame
   fig1=getframe;
   mov=addframe(mov, fig1);  
   close(gcf);

end

   mov=close(mov);

end
