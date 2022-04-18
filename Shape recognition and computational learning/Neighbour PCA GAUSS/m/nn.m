#!/usr/bin/octave -qf
if (nargin!=2) printf("%s <tr> <te>\n",program_name()); exit; end
arg_list=argv(); Tr=arg_list{1}; Te=arg_list{2};
load(sprintf(Tr)); tr=data; [NTr,L]=size(tr); D=L-1;
labs=unique(data(:,L)); C=numel(labs);
load(sprintf(Te)); te=data; NTe=rows(te); clear data;
recolabs=zeros(1,NTe);
for i=1:NTe
  tei=te(i,1:D)';
  nmin=1; min=inf;
  for n=1:NTr
    trn=tr(n,1:D)'; aux=tei-trn; d=aux'*aux;
    if (d<min) min=d; nmin=n; endif
  end
  recolabs(i)=tr(nmin,L);
end
[Nerr m]=confus(te(:,L),recolabs);
printf("%s %s %d %d %.1f\n",Tr,Te,Nerr,NTe,100.0*Nerr/NTe);
m
