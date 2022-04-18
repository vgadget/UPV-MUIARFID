
% Recibe una matriz en la que cada columna son los pesos
% Cada columna es un vector de pesos (cada col es un clasificador)
% Nos quedamos con la mejor columna,AKA que discrimine más


function cstar=linmach(w,x)
  C=columns(w); cstar=1; max=-inf;
  for c=1:C % Recorre las columnas
    % Multiplica una columna (clasific) por la mat X de entrada
    g=w(:,c)'*x; 
    if (g>max) max=g; cstar=c; endif; end
endfunction

% devuelve cstar
