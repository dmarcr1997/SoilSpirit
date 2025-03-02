# Rover Command Server
Node.js server to track commands between the camera and the rover. I decided to go with this approach due to issues with stable communications between the rover and the XIAO camera. This server code sits on a Zimaboard NAS I have which allows for quick communication between any devices connected to the remote.

## Overview
The Rover Command Server solves the problem of unreliable direct communication between my camera and rover it is always-on service that:

1. Maintains a command queue
2. Tracks camera connection status
3. Provides endpoints for both the camera and rover
4. Logs Rover and Camera Activity

## Core Features
- **Command Queue**: Stores commands from rover(FORWARD, BACKWARD, TURN_LEFT, TURN_RIGHT, STOP)
- **Heartbeat System**: Tracks camera status
- **Automatic Queue Clearing**: Clears commands if not connected to camera
- **Logging**: Records all server activity on logs

## Endpoints
### Camera
- **POST `/command`**: Adds a command to the queue
  ```json
  {
    "command": "FORWARD"
  }
  ```

- **POST `/heartbeat`**: Camera health ping
  ```json
  {}
  ```

### Rover
- **GET `/next-command`**: Gets next command for rover and removes from command queue
  ```json
  {
    "command": "FORWARD",
    "queueLength": 0,
    "cameraConnected": true
  }
  ```

- **GET `/status`**: Checks server, queue, and camera status
  ```json
  {
    "queueLength": 1,
    "cameraConnected": true,
    "lastHeartbeat": "2025-03-01T15:30:45.123Z"
  }
  ```

## Setup and Deployment
Built for Zimaboard, but this can be used on any computer that has docker installed

### Prerequisites
- Docker
- Node.js (if running locally, no Docker)

### Installation
1. Clone this repository:
   ```bash
   git clone <repository-url>
   cd <repository-directory>/server
   ```

2. Build the Docker image:
   ```bash
   docker build -t rover-command-server .
   ```

3. Run the container:
   ```bash
   docker run -d --name rover-server \
     -p 3000:3000 \
     -v ~/rover-logs:/app/logs \
     --restart unless-stopped \
     rover-command-server
   ```

### Running Without Docker
1. Install dependencies:
   ```bash
   npm install
   ```

2. Start the server:
   ```bash
   node server.js
   ```

## Logs
Logs are stored in the `logs` directory with filenames based on the current date (`server-YYYY-MM-DD.log`).

## Testing
Testing in the terminal:

```bash
# Sever Status
curl http://localhost:3000/status

# Add Rover command
curl -X POST http://localhost:3000/command \
  -H "Content-Type: application/json" \
  -d '{"command":"FORWARD"}'

# Send camera heartbeat
curl -X POST http://localhost:3000/heartbeat

# Get next command for rover
curl http://localhost:3000/next-command
```

## License
[MIT LICENSE](../LICENSE)
