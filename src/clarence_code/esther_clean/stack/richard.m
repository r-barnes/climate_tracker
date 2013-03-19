load tavg;
load prcp;
mnpgeo=prcp;
mntgeo=tavg;

%Develop grid of points to explore trajectories for
latmin=min( min(mnpgeo(:,1)), min(mntgeo(:,1)) );
latmax=max( max(mnpgeo(:,1)), max(mntgeo(:,1)) );
longmin=min( min(mnpgeo(:,2)), min(mntgeo(:,2)) );
longmax=max( max(mnpgeo(:,2)), max(mntgeo(:,2)) );
gridgeo=[];
for x = [latmin:3:latmax],
for y = [longmin:3:longmax],
	gridgeo=[[gridgeo];[x,y]];
end
end

%Convert all grids from geographic coordinates to projected planar coordinates
mnpxy=Xll2pl(mnpgeo, 'lambert');
mntxy=Xll2pl(mntgeo,'lambert');
gridxy=Xll2pl(gridgeo,'lambert');

%Find surfaces
mnpquad=makequad(mnpxy);
mntquad=makequad(mntxy);

%Solve the grid points for the surfaces
gridtrack=invertquad(gridxy,mnpquad(:,1),mntquad(:,1));

%Convert to a trajectory for a specific combination of P,T:
mnquad2442=rtriquad(mnpquad, mntquad, gridtrack);

mnbk=mnquad2442;

mnquad2442=double(mnbk');
[mnquad2442lat, mnquad2442lon] = pl2ll(mnquad2442(1,:), mnquad2442(2,:), 'lambert');

%Create the animation.
ravitcquadmap(mnpquad, mntquad, mnquad2442lat, mnquad2442lon, 'MN' ,'lambert' );
%avitcquadmap(mnpquad, mntquad, [], [], 'MN' ,'lambert' );
