FROM python:3.9-alpine
ENV NGINX_CONFIGURE_ARGS=--with-http_realip_module
RUN apk update && apk add openssl sqlite nginx
WORKDIR /app
COPY requirements.txt .
RUN pip3 install -r requirements.txt
RUN mkdir keys &&  \
    openssl genrsa -out keys/private.pem 2048 && \
    openssl rsa -in keys/private.pem -out keys/public.pem -RSAPublicKey_out
COPY . .
COPY nginx.conf /etc/nginx/nginx.conf
CMD gunicorn --bind 0.0.0.0:8888 main:app && nginx -g 'daemon off;'
