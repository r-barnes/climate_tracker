function fntriplotm(triplelatlon)

% fntriplot creates a the trajectory of the points located by the xy coordinates
% in coords, where the index indicates the relative year. 
% fntriplot is used by 'avitriplecountourlin'
%
% input = coords 2 x number of year
% output is a plot
%
% functions called = none

coords=triplelatlon;
s=size(coords,2);
%figure
%set(gca, 'DataAspectRatio',[1 1 1], 'PlotBoxAspectRatio', [1 1 1]);

hold on
plotm(coords(1,1:s), coords(2,1:s), '-.b')


for i=1:s
    plotm(coords(1,i),coords(2,i),'b.','MarkerSize',6, 'MarkerFaceColor',[0.75 0.75 0.5])
end


hold off

end
