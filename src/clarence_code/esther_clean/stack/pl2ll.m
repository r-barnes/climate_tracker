function [lat, lon] = pl2ll(x, y, proj)
% pl2ll (PLanar 2 Lat Lon -- here, l is letter el, not the number one)
% is a function that transform the vector [lat lon]
% into planar coordinate [x y] using a user specified projection 'proj'
% where proj is a string.
% for example, using the lambert projection, and execution of this 
% function would be: [x y]=112pl(lat, lon, 'lambert')
% 
%
% The list of accepted projection can be found in Matlab Help by searching
% "Map Projections Reference"


mstruct=defaultm(proj);
mstruct.origin=[30 -90];
mstruct=defaultm(mstruct);

[lat, lon]=minvtran(mstruct, x, y);
end

