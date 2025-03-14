docker stop rover-server;
docker rm rover-server;
docker build -t rover-command-server .;
docker run -d --name rover-server   -p 3000:3000   -v ~/rover-logs:/app/logs   --restart unless-stopped   rover-command-server;