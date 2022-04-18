close all; clc; clear; % Clear screen, variables and close windows 

load gauss2D; % Carga el dataset


[N,L]=size(data); % Obtiene el tamaño de la matriz de datos

D=L-1; % Tamaño de los vectores de caracteristicas

classes=unique(data(:,L)); % Obtiene todas las clases disponibles

C=numel(classes); % Numero de clases disponibles

rand("seed",23); % Establece una semilla para vaolres aleatorios

data=data(randperm(N),:); % Randomiza los datos

% Entrenamos con el 70% de los datos
percent = .7;
%train = data(1:round(percent*N), :);
train = data(:, :);
test = data(round(percent*N)+1:end,:);

% COMIENZO EXPERMIENTO: efecto de a

##printf("\n#a \t\t E \t\t k \t\t Error \t\t Error (%%)");
##
##for a = [.1 1 10 100 1000 10000 100000]
##  
##  [w,E,k]=perceptron(train, 0.1, a); 
##
##  % Test con el 30% de los datos
##  predicted = zeros(size(test, 1), 1);
##
##  for i = 1:size(test, 1)
##    
##    muestra = [1 test(i,1:D)]';
##    predicted(i) = classes(linmach(w, muestra));
##    
##  endfor
##
##  [num_errors confusion_matrix] = confus(test(:,L), predicted); %  Matriz de confusion
##
##  error_rate = num_errors/size(test, 1);
##  r = 1.96*(sqrt(error_rate*(1-error_rate)/size(test, 1))); % Intervalo de confianza
##  I = [(error_rate-r), (error_rate+r)];
##
##  output_precision(2); 
##  
##  printf("\n %.2f\t\t%.2f\t\t%i\t\t%i\t\t%.2f", a, E, k, num_errors ,error_rate*100);
##  
##  printf("\t I [%f, %f]\n", I(1)*100, I(2)*100);
##  
##endfor


% COMIENZO EXPERMIENTO: efecto de b

printf("\n#b \t\t E \t\t k \t\t Error \t\t Error (%%)");

a = 0.15;
for b = [0.1]; %[0.001 0.007 0.05] %[.1 1 10 100 1000 10000 100000]
  
  [w,E,k]=perceptron(train, b, a); 

  % Test con el 30% de los datos
  predicted = zeros(size(test, 1), 1);

  for i = 1:size(test, 1)
    
    muestra = [1 test(i,1:D)]';
    predicted(i) = classes(linmach(w, muestra));
    
  endfor

  [num_errors confusion_matrix] = confus(test(:,L), predicted); %  Matriz de confusion

  error_rate = num_errors/size(test, 1);
  r = 1.96*(sqrt(error_rate*(1-error_rate)/size(test, 1))); % Intervalo de confianza
  I = [(error_rate-r), (error_rate+r)];

  output_precision(2); 
  
  printf("\n %.2f\t\t%.2f\t\t%i\t\t%i\t\t%.2f", b, E, k, num_errors ,error_rate*100);
  
  printf("\t I [%f, %f]\n", I(1)*100, I(2)*100);  
  
endfor

save_precision(4); 
save("S_w","w");



