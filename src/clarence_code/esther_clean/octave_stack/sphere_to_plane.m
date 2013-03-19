%See: http://mathworld.wolfram.com/LambertConformalConicProjection.html
function out = sphere_to_plane(inp,projection,phi1,phi2,reflat,reflon) #Todo: Redo with variable argument lists
	lat=inp(:,1);
	lon=inp(:,2);
	phi1=phi1*pi/180;
	phi2=phi2*pi/180;
	lat=lat*pi/180;
	lon=lon*pi/180;
	reflat=reflat*pi/180;
	reflon=reflon*pi/180;
	if(projection=='lambert')
		n=log(cos(phi1)*sec(phi2))/log(tan(pi/4+phi2/2)*cot(pi/4+phi1/2));
		F=cos(phi1)*(tan(pi/4+phi1/2)).^n;
		rho=  F*cot(pi/4+lat   /2).^n;
		rho_o=F*cot(pi/4+reflat/2).^n;
		x=rho.*sin(n*(lon-reflon));
		y=rho_o-rho.*cos(n*(lon-reflon));
		out=[x,y];
	end
end
