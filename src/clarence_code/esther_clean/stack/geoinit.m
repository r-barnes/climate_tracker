function [latlim, lonlim, ax, states]=geoinit(loc, proj)

%% initiate an axis with a given projection, and calling the usamap 
% function from the toolbox

ax=usamap(loc);
setm(ax, 'mapprojection', proj);
set(ax, 'Visible','off');
latlim=getm(ax,'MapLatLimit');
lonlim=getm(ax,'MapLonLimit');
states = shaperead('usastatehi','UseGeoCoords', true, 'BoundingBox', [lonlim', latlim']);
geoshow(ax, states);


%plotm(lat, lon, 'rp')

end

