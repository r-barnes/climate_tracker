function F = quad(a,location)
	xdata=location(:,1);
	ydata=location(:,2);
	F=a(1)*(xdata).^2+a(2)*ydata.^2+a(3)*xdata.*ydata+a(4)*xdata+a(5)*ydata+a(6);
end
