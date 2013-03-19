function avitcquadmap(coefp,coeft,tlat, tlon, loc, proj)

%% avitcquadmap
%  avitcquadmap creates a vido file (avi format) that tracks specific points
% on the contours of the precipitation and temperature climate
% approximation using quadratic surface fit on a map projection using the Matlab's Mapping
% Toolbox
%
% inputs: 
% 1. coefp, coeft are the coefficients of the quadratic surface which are 
%  the outputs of 'makequads'
% 2. tlat, tlon are the coordinates of the tracked points, 
%  which are the outputs of 'triquad'
%  after being transformed to the geocoordinate ie, (lat, lon).
% 3. loc is any initializiation location which is acceptable to the matlab function
% 'usamap' in the matlab toolbox.
% 4. proj is the projection used in the transformation, eg 'Lambert').
%
% output: is an animation saved in tcquadmap.avi, and can be played
% using eg. quickplayer in mac.
% 
% functions called: geoinit
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% creating the file tcquadmap.avi to hold the frames 
mov=avifile('tcquadmap.avi', 'fps',3);

%% s is the number of frames that will be created this function
s=size(coefp,2);

for j=1:s  % loops through the years

%%  geoinit initializes the frame 
%   in this routine, we extract the first and second outputs 
%   of geoinit, latlim and lonlim, which are the latitude and longitude
%   limits of the map created by geoinit(loc, proj). This defines a
%   rectangle in the projected x,y space. 

    [latlim, lonlim]=geoinit(loc,proj);
%%  creating mesh graticules within the lon and lat limits given by lonlim 
%   and latlim  (NOTE: Here the entire range is divided into sixty grid
%   points in the latitude-longitude space. Sixty is just a convient number
%   that is close enough to continuous for Minnesota and other states. It
%   has nothing to do with minutes or seconds or arc.)
   
    dlat=(latlim(2)-latlim(1))/60; %dlat is the width of the lat graticule
    dlon=(lonlim(2)-lonlim(1))/60; %dlon is the width of the lon graticule   
    lat=latlim(1):dlat:latlim(2);  %lat is the partitioned subinterval for latitude
    lon=lonlim(1):dlon:lonlim(2);  %lon is the partitioned subinterval for longitude 
                            
    [X,Y]=meshgrat(lat, lon);      % note: meshgrat is a function included 
                                   % in matlab mapping toolbox, see help 
                                   % for more information 
                                   
% lat and lon are vectors containing the locations of the vertical and
% horizontal mesh lines. X,Y are ssimilar but in a form that can be processed
% by the matlab software tools for mapping.

%%  converting the lat lon coordinate [X,Y] in the mesh graticules to 
%   planar coordinate [x,y] using the user specified projection 'proj'  
  
    [x,y]=ll2pl(X, Y, proj); % ll2pl (l-at l-on 2 pl-anar) is a function 
                             % saved in ll2pl.m
    
%% creating the precipitation and temperature quadratic surfaces,
%  here we use the planar coordinate [x,y]

   P=coefp(1,j)*x.^2+coefp(2,j)*y.^2+coefp(3,j)*x.*y+coefp(4,j)*x+ ...
     coefp(5,j)*y+coefp(6,j); %precipitation surfaces
   T=coeft(1,j)*x.^2+coeft(2,j)*y.^2+coeft(3,j)*x.*y+coeft(4,j)*x+...
     coeft(5,j)*y+coeft(6,j); %temperature surfaces
 % P and T are each two-dimensional arrays representing values of
 % precipitation and temperature at the grid points of x and y.
   
%% plotting the points tlat, tlon, and the contours given by the 
%  coefficients in coefp and coeft
%  note: plotm and contourm are functions included in the mapping toolbox

   contourm(X, Y, P, 'g', 'Showtext', 'on');
   contourm(X, Y, T, 'r', 'showtext', 'on');
   plotm(tlat(1:j), tlon(1:j), 'b-'); 
   
%% capturing the frame and putting it away in the "mov" file and closing the current
%  frame
   fig1=getframe;
   mov=addframe(mov, fig1);  
   close(gcf);
end

   mov=close(mov); % closing the file mov.

end
