function centroids=trackcentroid(data, threshold)

% trackcentroid collects the [lat lon] coordinate of the centroids of 
% 'data' above certain threshold
% data is the standardized [lon lat value]
% data format: [real real real]


i=1;
j=1;


tcentroids=zeros(1, 2);
c=zeros(1,2);


% find annual-blocks in data and fit a plane over each year.
while (j < size(data,1)+1)
   
   %finding annual blocks 
   A = data(j,:);
    
   while (isnan(data(i+1))~=1)
       A=cat(1,A,data(i+1,:));
       i = i+1;
   end
    
   
   c=centresh(A, threshold);
   
   tcentroids=cat(1, tcentroids, c);
   
   
   % advance to the next year, here we add 2 to the last j since j+1 is [NaN NaN NaN]
   j=i+2;
   i=j;
end

% reporting the output 

s=size(tcentroids,1);
centroids=[tcentroids(2:s,2) tcentroids(2:s,1)];

end

