function avitrackclusters(K,C, x, y)


i=1;
j=1;
m=1;



fig1=figure('visible', 'off');
mov=avifile('triplecluster.avi', 'fps', 5);
hax=axes;



while (j<size(K,1)+1)
    
    A=K(j,:);

    while (isnan(K(i+1))~=1)
        A=cat(1,A,K(i+1,:));
        i=i+1;
    end
    
    
    yr=num2str(m);
     
      plot(A(:,2), A(:,1), 'rp', C(m,2), C(m,1), 'ob', 'parent', hax);
      text(mean(x), y(1)+1, yr, 'fontsize', 24);
     
    fig1=getframe;
    mov=addframe(mov, fig1); 
    

    j=i+2; 
    i=j;
    m=m+1;
end

mov=close(mov);
end
