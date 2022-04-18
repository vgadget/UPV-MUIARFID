%
% data -> Matriz de datos de caracteristicas de N filas (ult. fila etiqueta de clase)
% b -> Margen
% a -> Factor de aprendizaje
% K -> Numero de iteraciones
% iw -> Pesos iniciales


function [w,E,k]=perceptron(data,b,a,K,iw)
  
  [N,L]=size(data); % Tam de la matriz de datos
  D=L-1; % Numero de columnas excluyendo la columna con la clase
  
  labs = unique(data(:,L));  % Etiquetas de clases disponibles
  C= numel(labs); % Numero de clases disponibles
  
  % Solo necesitamos data como parametro
  % si no nos dan más que data, establecemos valores iniciales de los
  % params que faltan
  % nargin = number of input argument
  if (nargin<5) w=zeros(D+1,C); else w=iw; end % Pesos a 0 si no se reciben
  if (nargin<4) K=200; end; % Por defexto 200 iteraciones
  if (nargin<3) a=1.0; end; % factor de aprendizaje 1 por defecto
  if (nargin<2) b=0.1; end; % Margen 0.1 por defecto
  
  % Recorremos un numero determinado de iteraciones
  for k=1:K
    
    % Flag de error a 0. Cuando se haga una iteracion donde no haya errores, 
    % terminaremos
    E=0; 
    
    for n=1:N % Por cada fila (muestra)
      
      % Obtenemos vector columna de caracteristicas de la muestra
      % Con su termino independiente (notacion homogenea)
      xn=[1 data(n,1:D)]'; 
      
      % Obtenemos la clase de la muestra 
      % concretamente el indice dentro de la lista de clases
      cn=find(labs==data(n,L)); 
      
      er=0; % Error de clasificación dado unos pesos
      g=w(:,cn)'*xn; % Valor del disriminante
      
      %Por cada clase disponible dada una muestra
      for c=1:C; 
        
        % Si el valor del discriminante es mayor para una clase incorrecta
        if (c!=cn && w(:,c)'*xn+b>g)
          % Para toda clase que se confunda con la clase, se le castiga
          % se le resta el facrtor de aprendizaje a la incorrecta
	        w(:,c)=w(:,c)-a*xn; 
          er=1; % Encendemos el flag de error
         end; 
        end
      
      % Si para una muestra ha habido alguna equivocción 
      if (er) 
        % Para todas las filas de la clase correcta, se efectua aprendizaje
        % basado en la muestra actual que se ha tenido errores respecto al resto de
        % clases incorrectas, es decir se recompensa a la clase 
	      w(:,cn) = w(:,cn)+a*xn; 
        E=E+1; % Se suna un error a la iteración (Por muestra) se activa el flag
      end; 
     end
    
    % Cuando no haya errores para ninguna muestra, paramos
    % aunque no hayamos hecho todas las iteraciones.
    if (E==0) break; end; end 

endfunction
