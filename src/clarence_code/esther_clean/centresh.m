function  c = centresh(A, threshold)

%A=[lon lat value];

j=find(A(:,3)>threshold);
lon=A(:,1);
meanlon=mean(lon(j));
lat=A(:,2);
meanlat=mean(lat(j));

c=[meanlat meanlon];
end

