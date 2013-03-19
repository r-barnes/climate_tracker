function F=planefunc(location, pquad, tquad)
x=location(1);
y=location(2);
F(1)=pquad(1)*x+pquad(2)*y+pquad(3);
F(2)=tquad(1)*x+tquad(2)*y+tquad(3);
end
