function K = clustresh(A, thresh)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

%A=[lat lon value]

j=find(A(:,3)>thresh);
K=[A(j,2) A(j, 1)];
end

