function pldata=Xll2pl(data, proj)

% read data of weather stations values where the locations are in the form
% of lat lon, then convert it to planar form with the user specified
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


%pldata=zeros(size(data));
%[pldata(:,1),pldata(:,2)]=ll2pl(data(:,1), data(:,2),proj);
%pldata(:,3)=data(:,3);
%Richard:
[data(:,2),data(:,3)]=ll2pl(data(:,2), data(:,3),proj);
pldata=data;

end
