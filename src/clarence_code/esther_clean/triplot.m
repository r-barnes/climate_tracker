function triplot(coords)

% triplot creates a the trajectory of the points located by the xy coordinates
% in coords, where the index indicates the relative year
%
% input = coords 2 x number of year
% output is a plot
%
% functions called = none

n=size(coords,2);
first=floor(n/3);
second=floor(2*n/3);

figure
set(gca, 'DataAspectRatio',[1 1 1], 'PlotBoxAspectRatio', [1 1 1]);

hold on

% this is the thread that connects the triple points.
plot(coords(1,2:n), coords(2,2:n), '-.m')


% the triple points in the first third of the running period 
% are plotted as yellow stars, the second third as blue stars
% and the last third as red stars.

for i=2:first
    plot(coords(1,i),coords(2,i),'y*')
   % text(coords(1,i),coords(2,i),int2str(i), 'BackgroundColor','y')
    
end

for i=(first+1):second
    plot(coords(1,i),coords(2,i),'b*')
    %text(coords(1,i),coords(2,i),int2str(i), 'BackgroundColor','r')
    
end

for i=second:n
    plot(coords(1,i),coords(2,i),'r*')
    %text(coords(1,i),coords(2,i),int2str(i), 'BackgroundColor','b')

end

hold off

end
