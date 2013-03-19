function [x, y] = ll2pl(lat, lon, proj)
% ll2pl (Lat Lon 2 PLanar -- here, l is letter el, not the number one)
% is a function that transform the vector [lat lon]
% into planar coordinate [x y] using a user specified projection 'proj'
% where proj is a string.
% for example, using the lambert projection, and execution of this 
% function would be: [x y]=112pl(lat, lon, 'lambert')
% the list of accepted projection can be found in Matlab Help by searching
% "Map Projections Reference"
%
% Note that the vertically oriented 'lat' comes first in the input, then
% followed by the horizontally oriented 'lon', whereas in the output, 
% the horizontal 'x' comes before vertical 'y'


mstruct=defaultm(proj);
mstruct.origin=[30 -90];
mstruct=defaultm(mstruct);


%note: x is the horizontal direction and y is vertical
[x, y]=mfwdtran(mstruct, lat, lon);
end

