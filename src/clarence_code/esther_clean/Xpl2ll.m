function lldata=Xpl2ll(data, proj)

% read data of weather stations values where the locations are in the 
% planar form, then convert it to lat-lon form with the user specified
% projection 'proj'.
%
% input: data= Nx3, each row is organized as [lat lon V],
%        where lat lon is the geographical coordinate of the weather station
%        and V is the value (eg. temperature) at the location with
%        coordinate (x,y)
% 
%        each year is separated by a row of [NaN NaN NaN]
%        eg.  a row with no entry
%
% output: same size as the input 'data'
%
% functions called: ll2pl


lldata=zeros(size(data));
[lldata(:,1),lldata(:,2)]=pl2ll(data(:,1), data(:,2),proj);
lldata(:,3)=data(:,3);

end

