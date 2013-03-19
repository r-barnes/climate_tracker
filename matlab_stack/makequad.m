function [coef,years,r2]=makequad(data)

% read data then create an array in which each column [a b c]
% is the coefficient of a quadratic surface f(x,y) of the form
% f(x,y)=ax^2+by^2+cxy+dx+ey+g
%
% input: data= Nx4, each row is organized as [YEAR X Y V],
%        where X is the x-coordinate, y is the y-coordinate, and V is the
%        value (eg. temperature) at the location with coordinate (x,y)
%
%       each year is separated by a row of [NaN NaN NaN NaN]
%       eg.  a row with no entry
%
% output: coef = 6 x number of year in planar coordinate coefficients
% each column is the coefficient of the quad f(x,y), that
% estimates the climate parametrized by the temperature or precipitation
% of that year
%
% functions called: fitquad


%%%% initialization
%
% the index variable 'i' runs along the rows of the data, i.e., the weather
% stations.
i=1; j=1;

% coef is the first output variable, and its length, ie. the number of years, is
% unknown at the beginning of the run.
% memory usage will be more efficient if the number of years, numyear, is known,
% if numyear is known, we initialize coef as follows:
% coef=zeros(6, numyear);
coef=zeros(6,1);

% r2 is the place holder for the second output variable. makequads computes
% the r^2 of each year's approximation of the fitted surface, using the
% matlab function lsqcurvefit called by fitquad the final out
% put is 'r2', and is assigned after all the r^2 is computed
r2=zeros(1,1);

%This is the matrix of initial parameters for the fitting function
%we will define it later, when we find data!
init=NaN;

years=[];

while (j < size(data,1)+1)         %Loop through all years.
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
		init=[1 1 1 1 1 A(1,4)]';
	end

	[year,B,R]=fitquad(A,init);          %Compute best fit coefficients and R^2.
	init=B;                         %Use that to start the next step.
	coef=cat(2, coef, B);           %Collect the coefficients.
	r2=cat(2,r2,R);                 %Collect the R^2 values.
	years=[years,year];

	j=i;                     %Advance to the next year.
end

s=size(coef);                      %Discard the first column of coefficients and
coef=coef(:, 2:s(2));              %R^2, which are just convenient
r2=r2(:,2:s(2));                   %initialization columns.

end


function [year,zcoeff,r2]=fitquad(data,init)

% input:  
%        planes z=ax^2+by^2+cxy+dx+ey+f
%        data =nx3, where n is the number of the weather stations
%        data(2, :) = the x coordinate, data(3, :) = the y coordinate 
%        data(4, :) =data reading
%                
%       
% output: v = 6 x 1 the coefficient matrix of the two planes with 
%         variables x,y,z, of the form ax+by+cz+d=0
%         v(1,:)   coefficients [a b c d] of the temperature plane
%         v(2,:)   coefficients [a b c d] of the precipitation plane

year=data(1,1);
x = data(:,2);
y = data(:,3);
location=[x, y];
z = data(:,4);
opts=optimset('MaxFunEvals', 4000, 'TolFun', 1e-10, 'MaxIter', 1000); %, 'Display', 'Off');
[zcoeff,resnorm]=lsqcurvefit(@quad,init,location,z,[],[],opts);
ssq=sum((z-mean(z)).^2);
r2=1-(resnorm/ssq);
end
