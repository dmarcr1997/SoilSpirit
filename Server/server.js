const express = require('express');
const fs = require('fs');
const path = require('path');
const app = express();
const port = 3000;

const logsDirectory = path.join(__dirname, 'logs');
if(!fs.existsSync(logsDirectory)) {
	fs.mkdirSync(logsDirectory, {recursive: true});
}

const getLogFileName = () => {
	const now = new Date(); //log for each day
	const dateString = now.toISOString().split('T')[0] // get YYYY-MM-DD
	return path.join(logsDirectory, `server-${dateString}.log`);
};

const logFile = getLogFileName();

const logger = (message) => {
	const timestamp = new Date().toISOString();
	const logMessage = `[${timestamp}] ${message}\n`;
	fs.appendFileSync(logFile, logMessage); //Add to logfile
}

const checkCameraStatus = () => {
	const cameraConnected = (Date.now() - lastCameraHB) < CAMERA_TIMEOUT;
	if(!cameraConnected && commandQueue.length > 0) {
		logger(`Lost Camera Connection. CLEARING COMMAND QUEUE OF: ${commandQueue.length} commands`);
		commandQueue = [];
	}
	return cameraConnected;
};

app.use(express.json());
app.use((req, res, next) => {
	res.header('Access-Control-Allow-Origin', '*');
	res.header('Access-Control-Allow-Headers', 'Origin, X-Requested-With, Content-Type, Accept');
	res.header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
	if (req.method === 'OPTIONS') return res.status(200).end()
	next()
});

app.use((req, res, next) => {
	logger(`${req.method} ${req.url} ${JSON.stringify(req.body)}`);
	next();
}); //logger middleware to track requests

let commandQueue = [];
let active = false;
let lastCameraHB = Date.now();
const CAMERA_TIMEOUT = 120000;

app.post('/command', (req, res) => {
	const {command} = req.body;
	
	if(!command) {
		logger('Error: NO COMMAND');
		return res.status(400).json({
			status: 'error',
			message: 'Command is required'
		});
	}
	if (command.toUpperCase() === "START") {
                active = true;
                res.status(200).json({
                        status: 'success',
                        message: 'Starting Rover',
                        queueLength: 0
	        });
                commandQueue.push({
                        command: "OD",
                        timestamp: Date.now()
                });
        } else if (command.toUpperCase() === "CONTROL_STOP") {
                active = false;
                res.status(200).json({
                        status: 'success',
                        message: 'Stopping Rover',
                        queueLength: 1
	        });
                commandQueue.push({
                        command: command.toUpperCase(),
                        timestamp: Date.now()
                });
        } else {
			if(commandQueue.length >= 3) {
				commandQueue = [];
			}
			commandQueue.push({
					command: command.toUpperCase(),
					timestamp: Date.now()
			});

			lastCameraHB = Date.now();
			
			logger(`Command added: ${command}`);
			res.status(200).json({
					status: 'success',
					message: 'Command added to queue',
					queueLength: commandQueue.length
			});
        }
});

app.post('/heartbeat', (req, res) => {
	lastCameraHB = Date.now();
	logger('Camera HB received');
	res.status(200).json({ status: 'success' });
});

app.get('/next-command', (req, res) => {
	const cameraConnected = checkCameraStatus();
	if(commandQueue.length > 0) {
		const nextCommand = commandQueue.shift();
		logger(`Sending command to rover: ${nextCommand.command}`);
		res.status(200).json({
			command: nextCommand.command,
			queueLength: commandQueue.length,
			cameraConnected
		});
	} else {
		logger('No commands in queue, STOPPING ROVER');
		res.status(200).json({
			command: active ? "OD" : "FULL_STOP",
			queueLength: 0,
			cameraConnected
		});
	}
});

app.get('/status', (req, res) => {
	res.status(200).json({
		status: 'Server Running',
		message: `CAMERA CONNECTED: ${checkCameraStatus()}`,
        queue: commandQueue.length
	});
});

setInterval(() => {
	const cameraConnected = checkCameraStatus();
	if(!cameraConnected) {
		logger('Camera Status: Disconnected');
	} else {
		logger('Camera Status: Connected');
	}
}, 3000);


app.listen(port, '0.0.0.0', () => {
	logger(`Command queue running at http://0.0.0.0:${port}`);
});
