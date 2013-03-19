function geoplot(lat, lon, loc, proj)

%% plots the nodes specified by lat-lon using the user specified projection

ax=usamap(loc);
setm(ax, 'mapprojection', proj);
set(ax, 'Visible','off');
latlim=getm(ax,'MapLatLimit');
lonlim=getm(ax,'MapLonLimit');
states = shaperead('usastatehi',...
'UseGeoCoords', true, 'BoundingBox', [lonlim', latlim']);
geoshow(ax, states)


plotm(lat, lon, 'rp')

end

