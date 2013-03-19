function  xysoln=triquad(coefP, coefT, combo)
% triquad solves for the x,y coordinate of the triple point
% ie. a point with a combination of Precipitation p0, and temperature t0
% given by the input combo.
%
% input = coefP and coefT are the coefficients of the quadratic surfaces p(x,y) and t(x,y) approximating 
% precipitation and temperature -- exactly in this order--  created using 'makequad'
% combo is a column vector [p0;t0] of prec and temp combination to be
% tracked
% center is a column vector [c0; c1] needed in fsolve, called by fsolveq.
%
% output = xy is the xy-coordinate of the solution to the system of equations 
% p(x,y)=p0; 
% t(x,y)=t0.
% the output xy is an array of size 2 x number of year.
%
% note:to plot use triplot
%



%%%% preparing coefP and coefT
% making sure that coefP and coefT have the same number of years
% if coefT has more years, truncate coefT to the size of coefP
if (size(coefT,2) > size(coefP,2))
	coefT=coefT(:, 1:size(coefP,2));
else
	coefP=coefP(:, 1:size(coefT,2));
end

center=[0;0];

% assigning the number of years, without loss of generality, using coefT.
numyear=size(coefT, 2);

% initializing the place holder for the coordinates of the triplepoints.
vsoln=zeros(2,numyear);

% ensuring the coefficients are of type double
coefP=double(coefP);
coefT=double(coefT);
center=double(center);


%%%% solving for the x-y in the system of equations:
% p(x,y)=p0; 
% t(x,y)=t0.
% where combo=[p0, t0]
%

for i=1:numyear
    v=[coefP(:,i)'; coefT(:,i)'];
    locsoln = fsolveq(v, combo, center); 
    vsoln(:,i)=locsoln;
    center=double(locsoln);
end

xysoln=vsoln;

%% fsolveq

function  xy=fsolveq(v,z, center)

% input: v = 2 x 6 coefficient matrix, 
%           each row is the coefficients of 
%           an equation of the plane in the variable x, y and z,
%           of the form z=ax+by+c 
%           
%        z = 1 x 2, is the value matrix 
%           eg, z(1) is the prescribed temperature value,
%           while z(2) is the prescribed precipitation  value 
%        
%        center = a known center of the data region
%
% output: xy= 2x 1, the x, y coordinate where the two planes intersect
%         that is, the x,y coordinate where the prescribed combination
%         for temperature and precipitation values exist
% function called: parameterquad, fsolve
%


a = v(1, :);
b = v(2, :);
opts=optimset('TolFun', 1e-10, 'MaxIter', 2000, 'MaxFunEval', 2000, 'Display', 'off');

f=@(r)parameterquad(r,a,b,z);

Z=fsolve(f,center, opts);

xy=vpa(Z,4);



%
function f = parameterquad(x,a,b,z)

f(1) = a(1)*x(1)^2 + a(2)*x(2)^2 + a(3)*x(1)*x(2)+a(4)*x(1)+a(5)*x(2)+a(6)-z(1);
f(2) = b(1)*x(1)^2 + b(2)*x(2)^2 + b(3)*x(1)*x(2)+b(4)*x(1)+b(5)*x(2)+b(6)-z(2);

end

%% end of fsolveq
end

%% end of triquad
end
