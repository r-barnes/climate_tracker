function [coef,r2]=makequad(data)

% read data then create an array in which each column [a b c]
% is the coefficient of a quadratic surface f(x,y) of the form
% f(x,y)=ax^2+by^2+cxy+dx+ey+g
%
% input: data= Nx3, each row is organized as [X Y V],
%        where X is the x-coordinate, y is the y-coordinate, and V is the
%        value (eg. temperature) at the location with coordinate (x,y)
%
%       each year is separated by a row of [NaN NaN NaN]
%       eg.  a row with no entry
%
% output: coef = 6 x number of year
% each column is the coefficient of the quad f(x,y), that
% estimates the climate parametrized by the temperature or precipitation
% of that year
%
% functions called: fitquad


%%%% initialization
%
% the index variable 'i' runs along the rows of the data, i.e., the weather
% stations.
z=data(1,3);         %Start with the value from the first weather station.
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
init=[1 1 1 1 1 z]';

while (j < size(data,1)+1)         %Loop through all years.

   A = data(j,:);                  %Extract the next weather station's x, y, v.

   while (isnan(data(i+1))~=1)     %Extract the rest of the weather stations
       A=cat(1,A,data(i+1,:));     %of that year.
       i = i+1;
   end
	A
   [B,R]=fitquad(A,init);          %Compute best fit coefficients and R^2.
   init=B;                         %Use that to start the next step.
   coef=cat(2, coef, B);           %Collect the coefficients.
   r2=cat(2,r2,R);                 %Collect the R^2 values.

   j=i+2; i=j;                     %Advance to the next year.
end

s=size(coef);                      %Discard the first column of coefficients and
coef=coef(:, 2:s(2));              %R^2, which are just convenient
r2=r2(:,2:s(2));                   %initialization columns.

end


function [zcoeff,r2]=fitquad(data,init)

% input:  
%        planes z=ax^2+by^2+cxy+dx+ey+f
%        data =nx3, where n is the number of the weather stations
%        data(1, :) = the x coordinate, data(2, :) = the y coordinate 
%        data(3, :) =temperature reading, data(4,:)=precipitation reading
%                
%       
% output: v = 6 x 1 the coefficient matrix of the two planes with 
%         variables x,y,z, of the form ax+by+cz+d=0
%         v(1,:)   coefficients [a b c d] of the temperature plane
%         v(2,:)   coefficients [a b c d] of the precipitation plane

x = data(:,1);
y = data(:,2);
location=[x, y];
z = data(:,3);

%opts=optimset('MaxFunEvals', 4000, 'TolFun', 1e-10, 'MaxIter', 1000);
%opts=optimset('TolFun',1e-10);
%[zcoeff,resnorm]=lsqcurvefit(@quad,init,location,z,[],[],opts);

[f,zcoeff,cvg,iter,corp,covp,covr,stdresid,Z,r2]=leasqr(location,z,[1,1],@quad);

%ssq=sum((z-mean(z)).^2);
%r2=1-(resnorm/ssq);
end
