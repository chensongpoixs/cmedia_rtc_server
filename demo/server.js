// Copyright Epic Games, Inc. All Rights Reserved.

//-- Server side logic. Serves pixel streaming WebRTC-based page, proxies data back to Streamer --//

var express = require('express');
var app = express();

const fs = require('fs');
const path = require('path');
const querystring = require('querystring');
const bodyParser = require('body-parser');
const logging = require('./modules/logging.js');
logging.RegisterConsoleLogger();

// Command line argument --configFile needs to be checked before loading the config, all other command line arguments are dealt with through the config object

const defaultConfig = { 
	LogToFile: true,
	HomepageFile: "/websocket/player.html", 
	EnableWebserver: true,  
	HttpPort: 80 
};

const argv = require('yargs').argv;
var configFile = (typeof argv.configFile != 'undefined') ? argv.configFile.toString() : path.join(__dirname, 'config.json');
console.log(`configFile ${configFile}`);
const config = require('./modules/config.js').init(configFile, defaultConfig);



//If not using authetication then just move on to the next function/middleware
var isAuthenticated = redirectUrl => function (req, res, next) { return next(); }


if (config.LogToFile) {
	logging.RegisterFileLogger('./logs');
}

console.log("Config: " + JSON.stringify(config, null, '\t'));

var http = require('http').Server(app);

 

 
const helmet = require('helmet');
var hsts = require('hsts');
var net = require('net');

 
 


if(config.EnableWebserver) 
{
	//console.log('--------------websocket-----------------');
	//Setup folders
	app.use(express.static(path.join(__dirname, '/websocket')))
	app.use('/images', express.static(path.join(__dirname, './images')))
	app.use('/scripts', [isAuthenticated('/login'),express.static(path.join(__dirname, '/scripts'))]);
	//app.use('/', [isAuthenticated('/login'), express.static(path.join(__dirname, '/custom_html'))])
}



if(config.EnableWebserver) {
	app.get('/', isAuthenticated('/login'), function (req, res) {
		homepageFile = (typeof config.HomepageFile != 'undefined' && config.HomepageFile != '') ? config.HomepageFile.toString() : defaultConfig.HomepageFile;
		homepageFilePath = path.join(__dirname, homepageFile)

		fs.access(homepageFilePath, (err) => {
			if (err) {
				console.error('Unable to locate file ' + homepageFilePath)
				res.status(404).send('Unable to locate file ' + homepageFile);
			}
			else {
				res.sendFile(homepageFilePath);
			}
		});
	});
}

//Setup http and https servers
http.listen(defaultConfig.HttpPort, function () {
	console.logColor(logging.Green, 'Http listening on *: ' + defaultConfig.HttpPort);
});

 

console.logColor(logging.Cyan, `Running chen-WebSever`);

