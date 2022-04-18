FROM --platform=linux/amd64 ubuntu:20.04

COPY condaInstall.sh .

RUN ["chmod", "+x", "./condaInstall.sh"]

RUN ./condaInstall.sh -b

SHELL ["/bin/bash", "-c"] 

RUN source ~/.bashrc

EXPOSE 8888
EXPOSE 8080
EXPOSE 80