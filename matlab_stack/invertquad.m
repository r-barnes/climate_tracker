function  sols=invertquad(lat,lon,psurf,tsurf)
% invertquad takes x,y coordinates and returns the t,p coordinates they correspond to
%
% input = pts is a list of points to solve the quadratic surfaces for
% tsurf, psurf are the temperature and precipitation surfaces
%
% output = matrix where each row refers to one of the input points.
% First two columns are the input point, used to localise initial tracking search
% Second two columns are the p-t solution

[x,y]=ll2pl(lat,lon,'lambert');
pt=[x,y];

sols=parameterquad(pt,psurf,tsurf);

function f = parameterquad(x,a,b)

f(1) = a(1)*x(1)^2 + a(2)*x(2)^2 + a(3)*x(1)*x(2)+a(4)*x(1)+a(5)*x(2)+a(6);
f(2) = b(1)*x(1)^2 + b(2)*x(2)^2 + b(3)*x(1)*x(2)+b(4)*x(1)+b(5)*x(2)+b(6);

end

%% end of triquad
end
