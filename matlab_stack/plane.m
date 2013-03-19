function F=plane(a, location)

xdata=location(:,1);
ydata=location(:,2);
F=a(1)*xdata+a(2)*ydata+a(3);
end
