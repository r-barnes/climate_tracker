function pre=rearrange_data(mnushcn)
% Takes Nx5 array such that each row is "year1 | year2 | latitude | longitude | value"
% Returns array such that each row is [X Y V],
% X is the x-coordinate, y is the y-coordinate, and V is value at (x,y)
% Each year is separated by a row of [NaN NaN NaN] (a row with no entry)
% R: Takes the input array, finds places in array where the year increments, and puts a NaN between those places
% Ignores year2 entirely.
% Only extracts last three columns
% This can be done in C
year1=mnushcn(:,1)';
year2=mnushcn(:,2)';
lat=mnushcn(:,3)';
lon=mnushcn(:,4)';
val=mnushcn(:,5)';
j=0;
lastrow=size(mnushcn,1)-1;
for t=1:lastrow														%Loop
    if mnushcn(t,1)+1 == mnushcn(t+1,1)								%If current year+1 equals next year
        j=j+1;
        year1=[year1(1:t+j-1),eval('NaN'),year1(t+j:end)];
        year2=[year2(1:t+j-1),eval('NaN'),year2(t+j:end)];
        lat=[lat(1:t+j-1),eval('NaN'),lat(t+j:end)];
        lon=[lon(1:t+j-1),eval('NaN'),lon(t+j:end)];
        val=[val(1:t+j-1),eval('NaN'),val(t+j:end)];
    end
end
year1=[year1,eval('NaN')];
year2=[year2,eval('NaN')];
lat=[lat,eval('NaN')];
lon=[lon,eval('NaN')];
val=[val,eval('NaN')];
re_mnushcn=[year1;year2;lat;lon;val]';
rawpre=re_mnushcn;
%rawtemp=re_mnushcn(2289:end,:);
pre=rawpre(:,3:5);
%n=[eval('NaN') eval('NaN') eval('NaN')];
%pre=cat(1,pre,n);
%temp=rawtemp(:,3:5);

end
