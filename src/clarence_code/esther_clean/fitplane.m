
function [zcoeff,r2]=fitplane(data, init)

%% function fitplane
% input:  
%        planes z=ax+by+c
%        data = 3 x n, where n is the number of the weather stations
%        data(1, :) = the x coordinate, data(2, :) = the y coordinate 
%        data(3, :) =temperature reading, data(4,:)=precipitation reading
%                
%       
% output: v = 2 x 3 the coefficient matrix of the two planes with 
%         variables x,y,z, of the form ax+by+cz+d=0
%         v(1,:)   coefficients [a b c d] of the temperature plane
%         v(2,:)   coefficients [a b c d] of the precipitation plane
%
% functions called: plane

%x = data(:,1);
%y = data(:,2);
location=[data(:,1), data(:,2)];
z = data(:,3);
% init=[1 1 1]';

opts=optimset('MaxFunEvals', 3000, 'MaxIter', 1500, 'Tolfun', 1e-4, 'TolX',1e-5);
[zcoeff,resnorm]=lsqcurvefit(@plane,init,location,z,[],[],opts);
ssq=sum((z-mean(z)).^2);
r2=1-(resnorm/ssq);
end