function [coef,years,r2]=makeplane(data)

% read data then create an array in which each column [a b c]
% is the coefficient of a plane f(x,y) of the form 
% f(x,y)=ax+by+c
%
% input: data= Nx3, each row is organized as [YEAR X Y V],
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
init=NaN;

years=[];

% find annual-blocks in data and fit a plane over each year.
while (j < size(data,1)+1)
	if(isnan(data(j,1)))		   %No data for this year
		j=j+1;
		continue
	end

	A=[];
	i=j;
	while (~isnan(data(i,1)))     %Extract the rest of the weather stations
	    A=cat(1,A,data(i,:));     %of that year.
	    i = i+1;
	end

	if(isnan(init))
		init=[1 1 A(1,4)]';
	end
    
   % fitting the plane
   [year,B,R]=fitplane(A, init);
   
   % new initial coefficient for the next fitting
   init=B;
   
   % concatenate 'B', the new coefficient along the second dimension of coef
   % ie.  pasting 'B' along the previous array 'coef'
   % similarly, concatenate 'R' along tempr^2.
   coef=cat(2, coef, B);
   tempr2=cat(2,tempr2,R);

	years=[years,year];

	j=i;                     %Advance to the next year.
end

% reporting the output 
s=size(coef);

% recall that both coef and tempr2 are initialized by zeroes, ie. their
% first columns are zeroes.
coef=coef(:, 2:s(2));
r2=tempr2(:,2:s(2));
end



function [year,zcoeff,r2]=fitplane(data, init)

%% function fitplane
% input:  
%        planes z=ax+by+c
%        data = 3 x n, where n is the number of the weather stations
%        data(2, :) = the x coordinate, data(3, :) = the y coordinate 
%        data(4, :) = data reading
%                
%       
% output: v = 2 x 3 the coefficient matrix of the two planes with 
%         variables x,y,z, of the form ax+by+cz+d=0
%         v(1,:)   coefficients [a b c d] of the temperature plane
%         v(2,:)   coefficients [a b c d] of the precipitation plane
%
% functions called: plane

year=data(1,1);
location=[data(:,2), data(:,3)];
z = data(:,4);

opts=optimset('MaxFunEvals', 3000, 'MaxIter', 1500, 'Tolfun', 1e-4, 'TolX',1e-5);
[zcoeff,resnorm]=lsqcurvefit(@plane,init,location,z,[],[],opts);
ssq=sum((z-mean(z)).^2);
r2=1-(resnorm/ssq);
end
