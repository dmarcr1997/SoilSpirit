FROM node:18-alpine

WORKDIR /app

RUN mkdir -p /app/logs

COPY package.json .
RUN npm install

COPY server.js .

EXPOSE 3000

VOLUME ["/app/logs"]

CMD ["node", "server.js"]