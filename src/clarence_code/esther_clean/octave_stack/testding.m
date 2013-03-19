function testding(coefp,coeft,tlat, tlon, loc, proj)

%% s is the number of frames that will be created this function
s=size(coefp,2);

tbreak=1;
tsize=size(tlat,2);

find(isnan(tlat))

for j=1:s  % loops through the years

   
   for t=1:tsize
      if isnan(tlat(t))
tlat(tbreak:(tbreak-1)+j)
tlon(tbreak:(tbreak-1)+j)
%	plotm(tlat(tbreak:(tbreak-1)+j), tlon(tbreak:(tbreak-1)+j), 'b-');
        tbreak=t+1
      end
   end

end



end
