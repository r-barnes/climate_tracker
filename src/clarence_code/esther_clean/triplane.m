function  xy=triplane(coefP, coefT, combo)
% triplane solves for the x,y coordinate of the triple point
% ie. a point with a combination of Precipitation p0, and temperature t0
% given by the input combo.
%
% input = coefP and coefT are the coefficients of the planes p(x,y) and t(x,y) approximating 
% precipitation and temperature -- exactly in this order--  created using makeplanes
% combo is a column vector [p0;t0] of prec and temp combination to be
% tracked
% 
% output = xy is the xy-coordinate of the solution to the system of equations 
% p(x,y)=p0; 
% t(x,y)=t0.
% the output xy is an array of size 2 x number of year.
%
% note:to plot use triplot
%
% functions called : solvelin


%%%% preparing coefP and coefT

% making sure that coefP and coefT have the same number of years
% if coefT has more years, truncate coefT to the size of coefP
if (size(coefT,2) > size(coefP,2))
coefT=coefT(:, 1:size(coefP,2));

% otherwise, if coefP has more years then truncate coefP to the size of
% coefT
else
coefP=coefP(:, 1:size(coefT,2));    
    
end

% assigning the number of years, without loss of generality, using coefT.
numyear=size(coefT, 2);

% initializing the place holder for the coordinates of the triplepoints.
xy=zeros(2, numyear);

% transposing coefP and coefT to fit the input structure of solvelin
P=coefP';
T=coefT';

%%%% solving for the x-y in the system of equations:
% p(x,y)=p0; 
% t(x,y)=t0.
% where combo=[p0, t0]
%

for i=1:numyear

    xy(:, i) = solvelin([P(i,:);T(i,:)], combo); 
   
end


end










function  xy=solvelin(v,z)

% input: v = 2 x 4 coefficient matrix, 
%           each row is the coefficients of 
%           an equation of the plane in the variable x, y and z,
%           of the form z=ax+by+c 
%           
%        z = 1 x 2, is the value matrix 
%           eg, z(1) is the prescribed temperature value,
%           while z(2) is the prescribed precipitation  value 
% output: xy= 2 x 1, the x, y coordinate where the two planes intersect
%         that is, the x,y coordinate where the prescribed combination
% for temperature and precipitation 
%         values exist

a = v(1, :);
b = v(2, :);
syms x y 

f(1) = a(1)*x + a(2)*y + a(3)-z(1);
f(2) = b(1)*x + b(2)*y + b(3)-z(2);
format('short');
[X, Y]=solve(f(1), f(2));
xy=vpa([X; Y], 4);

end
