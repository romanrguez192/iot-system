FROM node:18.12.0-alpine3.15

WORKDIR /app

COPY package*.json ./

RUN npm install

COPY . .

EXPOSE 1883
EXPOSE 8888

CMD ["npm", "start"]
