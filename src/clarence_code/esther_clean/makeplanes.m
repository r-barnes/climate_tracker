function [coef,r2]=makeplanes(data)

% read data then create an array in which each column [a b c]
% is the coefficient of a plane f(x,y) of the form 
% f(x,y)=ax+by+c
%
% input: data= Nx3, each row is organized as [X Y V],
%        where X is the x-coordinate, y is the y-coordinate, and V is the
%        value (eg. temperature) at the location with coordinate (x,y)
% 
%       each year is separated by a row of [NaN NaN NaN]
%       eg.  a row with no entry
%
% output: coef = 3 x number of year
% each column is the coefficient of the plane f(x,y)=ax+by+c, that 
% estimates the climate parametrized by the temperature or precipitation
% of that year

% functions called: fitplane


%%%% initialization
%
% the index variable 'i' runs along the rows of the data, 
% the index variable 'j' runs along the annual-blocks of the data.
i=1;
j=1;

% coef is the first output variable, and its length, ie. the number of years, is
% unknown at the beginning of the run.
% memory usage will be more efficient if the number of year, numyear, is known, 
% if numyear is known, we initialize coef as follows: 
% coef=zeros(3, numyear);
coef=zeros(3,1);

% tempr2 is the place holder for the second output variable. makeplanes computes 
% the r^2 of each year's approximation of the fitted plane, using the
% matlab function lsqcurvefit called by fitplane
% the final out
% out put is 'r2', and is assigned after all the r^2 is computed
tempr2=zeros(1,1);


%initial guess of the coefficient, works well for MN. 
%note: needs improvement so that it is independent of the location
init=[1 1 120]'; 



% find annual-blocks in data and fit a plane over each year.
while (j < size(data,1)+1)
   
   %finding annual blocks 
   A = data(j,:);
    
   while (isnan(data(i+1))~=1)
       A=cat(1,A,data(i+1,:));
       i = i+1;
   end
    
   % fitting the plane
   [B,R]=fitplane(A, init);
   
   % new initial coefficient for the next fitting
   init=B;
   
   % concatenate 'B', the new coefficient along the second dimension of coef
   % ie.  pasting 'B' along the previous array 'coef'
   % similarly, concatenate 'R' along tempr^2.
   coef=cat(2, coef, B);
   tempr2=cat(2,tempr2,R);
   
   % advance to the next year, here we add 2 to the last j since j+1 is [NaN NaN NaN]
   j=i+2;
   i=j;
end

% reporting the output 
s=size(coef);

% recall that both coef and tempr2 are initialized by zeroes, ie. their
% first columns are zeroes.
coef=coef(:, 2:s(2));
r2=tempr2(:,2:s(2));
end

