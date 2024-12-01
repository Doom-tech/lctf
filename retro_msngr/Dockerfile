FROM ubuntu:18.04

EXPOSE 443

RUN apt-get update
RUN apt-get -y install apache2

RUN a2enmod cgi
RUN a2enmod rewrite
RUN a2enmod ssl

RUN service apache2 restart


COPY ./cert/ssl.crt /etc/apache2/ssl/ssl.crt
COPY ./cert/ssl.key /etc/apache2/ssl/ssl.key
RUN mkdir -p /var/run/apache2/

RUN echo "                       \n \
<Directory /var/www/html>        \n \
   Options +ExecCGI              \n \
   AddHandler cgi-script .m  \n \
</Directory>                     \n \
ErrorLog /dev/stderr			 \n \
TransferLog /dev/stdout			 \n \
<VirtualHost *:443>	\n\
     SSLEngine on	\n\
     SSLCertificateFile /etc/apache2/ssl/ssl.crt	\n\
     SSLCertificateKeyFile /etc/apache2/ssl/ssl.key	\n\
</VirtualHost>						\n\
" >> /etc/apache2/apache2.conf

RUN chmod -R u+rwx,g+x,o+x /var/www/html

RUN apt install -y curl net-tools

CMD /usr/sbin/apache2ctl -D FOREGROUND
