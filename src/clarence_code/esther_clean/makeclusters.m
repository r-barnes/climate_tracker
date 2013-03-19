function [K,C]= makeclusters(data, thresh)

% read data then create an array in which each column [a b c]
% is the coefficient of a plane f(x,y) of the form 
% f(x,y)=ax+by+c
%
% input: data= Nx3, each row is organized as [X Y V],
%        where X is the x-coordinate, y is the y-coordinate, and v is the
%        value (eg. temperature) at the location with coordinate (x,y)
% 
%       each year is separated by a row of [NaN NaN NaN]
%       eg.  a row with no entry
%
% output: clusters of locations with coordinate (x,y) whose values v>thresh
%
% functions called: clustresh


%%%% initialization
%
% the index variable 'i' runs along the rows of the data, 
% the index variable 'j' runs along the annual-blocks of the data.
i=1;
j=1;


K=zeros(1,2);
C=zeros(1,2);
% find annual-blocks in data and fit a plane over each year.
while (j < size(data,1)+1)
   
   %finding annual blocks 
   A = data(j,:);
    
   while (isnan(data(i+1))~=1)
       A=cat(1,A,data(i+1,:));
       i = i+1;
   end
 
   
   %finding clusters of locations (x,y) with values v>thresh
   tempK=clustresh(A,thresh);
   tempC=centresh(A,thresh);
   
   %adding the clusters and centers to the collection
   K=cat(1,K,[tempK; [NaN NaN]]);
   C=cat(1,C,tempC);
   
   % advance to the next year, here we add 2 to the last j since j+1 is [NaN NaN NaN]
   j=i+2;
   i=j;
end
sK=size(K,1);
K=K(2:sK,:);
sC=size(C,1);
C=C(2:sC,:);
end





